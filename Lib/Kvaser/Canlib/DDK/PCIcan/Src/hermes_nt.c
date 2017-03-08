/*
**                         Copyright 1999 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
**
** Description:
**   Routines for Hermes and Windows NT/2000.
**
*/

#include <stdinc.h>
#include "hermes.h"
#include "hermes_nt.h"
#include "herm_hw.h"
#include "hermes_res.h"

#include "vcannt_version.h"
#include "version.h"
#include "vcan_msg.h"

static void HERMESCleanUp(IN PDRIVER_OBJECT DriverObject, HERMEScontext *pCtx);

VOID ScanPci(IN OUT ULONG * BoardCount,
             IN OUT ULONG * busNumber,
             IN OUT ULONG * slotNumber);

#define HERMES_IO_AREAS 3

///////////////////////////////////////////////////////////////////////
// CreateChannels ()
//---------------------------------------------------------------------
// This subroutine is called to create the only device of the driver
// and the corresponding dispatcher.
//
NTSTATUS CreateChannelsHermes(IN PDRIVER_OBJECT  DriverObject,
                              IN PUNICODE_STRING RegistryPath)
{
    PCM_FULL_RESOURCE_DESCRIPTOR    fullRes;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR partial;
    NTSTATUS            status;
    int                 i, j;
    PVchannel           pChan[HERMES_CHANNEL_COUNT];
    Vstatus             vErr;
    PDEVICE_OBJECT      DeviceObject;
    HERMEScontext       *pCtx;
    PDEVICE_EXTENSION   pDevExt;
    ULONG               boardCount;
    PCM_RESOURCE_LIST   res;
    KAFFINITY           Affinity;
    ULONG               MappedVector;
    KIRQL               irql;
    NTSTATUS            stat;
    ULONG               addresses[HERMES_IO_AREAS];
    ULONG               busNumber, slotNumber;
    ULONG               irqLevel, irqVector;

    /*ANSI_STRING     ansiStr;
    UNICODE_STRING  uniStr;*/

    DeviceObject = DriverObject->DeviceObject;
    pDevExt = (PDEVICE_EXTENSION)DriverObject->DeviceObject->DeviceExtension;
    
    status = STATUS_UNSUCCESSFUL;
    pCtx = VMalloc(sizeof(HERMEScontext));
    memset(pCtx, 0, sizeof(*pCtx));
    memset(pChan, 0, sizeof(pChan));

    pCtx->DriverObject = DriverObject;
    pCtx->DeviceObject = DriverObject->DeviceObject;

    { // qqq LAPcan test!
        int i;
        for (i=0x108; i<0x120; i++) {
            DIAGNOSTIC("%x %x", i, READ_PORT_UCHAR(i));
        }
    }

    
    //
    // We support only one board for now. qqq
    //
    boardCount = 0;
    ScanPci(&boardCount, &busNumber, &slotNumber);
    switch (boardCount) {
        case 0:
            DIAGNOSTIC(HERMES_NAME ": not found.\n");
            return STATUS_SUCCESS;
        case 1:
            break;
        default:
            DIAGNOSTIC(HERMES_NAME ": Wow! %d boards found", boardCount);
            DIAGNOSTIC(HERMES_NAME ": This is not supported yet\n");
            return STATUS_SUCCESS;
    }
    pCtx->busNumber = busNumber;

    /*Probably not needed: RtlInitAnsiString (&ansiStr, "Other");
    RtlInitUnicodeString (&uniStr, NULL);
    RtlAnsiStringToUnicodeString (&uniStr, &ansiStr, TRUE);*/

    // Note: DeviceObject is NOT optional in NT5!
    stat =  HalAssignSlotResources(RegistryPath,
                               NULL, // DriverClassName, optional
                               DriverObject,
                               DeviceObject,
                               PCIBus,
                               busNumber,
                               slotNumber,
                               &res // Resulting resource list
                               );
    
    if (stat != STATUS_SUCCESS) {
        DIAGNOSTIC("AssignRes, stat 0x%x\n", stat);
        return stat;
    }


    // qqq do more checks here...
    fullRes = &res->List[0];
               
    j = 0;
    for (i=0; i<fullRes->PartialResourceList.Count; i++) {
        partial = &fullRes->PartialResourceList.PartialDescriptors[i];
        switch (partial->Type) {
            case CmResourceTypePort:
                addresses[j++] = partial->u.Port.Start.LowPart;
                break;
            case CmResourceTypeInterrupt:
                irqVector = partial->u.Interrupt.Vector;
                irqLevel = partial->u.Interrupt.Level;
                break;
        }
    }

    if (j != HERMES_IO_AREAS) {
        DIAGNOSTIC(HERMES_NAME ": ERROR: got %d addresses", j);
    }
    ExFreePool(res);
    res = NULL;

    //
    // Translate the addresses
    //
    for (i=0; i<HERMES_IO_AREAS; i++) {
        PHYSICAL_ADDRESS PortAddr;
        LARGE_INTEGER tmp;
        ULONG AddressSpace;
        
        PortAddr.LowPart = addresses[i];
        PortAddr.HighPart = 0;
        AddressSpace = 1; // I/O space (but we're on Intel so we don't care)
        
        if (HalTranslateBusAddress(PCIBus, busNumber,
                                   PortAddr, &AddressSpace, &tmp))
        {
            addresses[i] = tmp.LowPart;
        } else {
            addresses[i] = 0;
        }
    }

    pCtx->s5920BaseAddress = (IoAddress)addresses[0];
    pCtx->baseAddress = (IoAddress)addresses[1];
    pCtx->xilinxAddress = (IoAddress)addresses[2];

    DIAGNOSTIC(HERMES_NAME ": Searching at address %04x/%04X/%04X.\n",
               addresses[0], addresses[1], addresses[2]);
    DIAGNOSTIC(HERMES_NAME ": Using vector 0x%02x level 0x%02x.\n", irqVector, irqLevel);

#if 0
    {
        int i, j;
        char s[100], t[100];
        IoAddress x = pCtx->s5920BaseAddress;
        for (i=0; i<2; i++) {
            sprintf(s, "%04x  ", x + i*16);
            for (j=0; j<16; j++) {
                sprintf(t, "%02x", READ_PORT_UCHAR(x + i*16 + j));
                strcat(s, t);
            }
            DIAGNOSTIC(s);
        }
    }
#endif
    
    // Verify the card.
    if ((vErr = HERMES_DetectCard(pCtx)) != VSUCCESS) {
        DIAGNOSTIC(HERMES_NAME ": hardware not found\n");
        status = STATUS_NO_SUCH_DEVICE;
        goto exit;
    }

    for (i=0; i<pCtx->channelCount; i++) {
        char s[32];

        sprintf(s, HERMES_NAME " Channel %u", i);
        pChan[i] = VCreateChannel(HWTYPE_HERMES, 0, i,
                                  s, i,
                                  sizeof(Vchan_extension));
    }

    for (i=0; i<pCtx->channelCount; i++) {
        pChan[i]->pHwContext = pCtx;
        pChan[i]->pHwTransmitEvent = HERMESTransmitEvent;
        pChan[i]->transceiverType = TRANSCEIVER_TYPE_NONE;
    }

    // Register the channels.
    for (i=0; i<pCtx->channelCount; i++) {
        VRegisterChannel(pChan[i]);
    }

    for (i=0; i<pCtx->channelCount; i++) {
        pCtx->channel[i] = pChan[i];
    }

    MappedVector = HalGetInterruptVector(PCIBus,            // Interface type
                                         pCtx->busNumber,   // Bus number
                                         irqLevel,          // Bus interrupt level
                                         irqVector,         // Bus interrupt vector
                                         &irql,             // From HAL: IRQ level
                                         &Affinity);        // Affinity mask

    pCtx->InterruptObject = NULL;
    KeInitializeSpinLock(&(pCtx->IsrSpinLock));
    stat = IoConnectInterrupt(&pCtx->InterruptObject,
                              HERMESInterruptService,
                              pCtx,
                              &(pCtx->IsrSpinLock),
                              MappedVector,
                              irql,
                              irql,
                              LevelSensitive,
                              TRUE,    // Shareable
                              Affinity,
                              FALSE);

    if (!NT_SUCCESS(stat)) {
        printf_event_log(MSG_TYPE_ERROR, HERMES_NAME ": failed IRQ allocation.");
        DIAGNOSTIC(HERMES_NAME ": ERROR: failed IRQ allocation. (%lx)\n", stat);
        HERMESFreeResources(pCtx);
        return VERROR; // qqq
    }

    if ((vErr = HERMES_InitCard(pCtx)) != VSUCCESS) {
        char *s = "ERROR: " HERMES_NAME " failed initialisation.";
        DIAGNOSTIC("%s\n", s);
        printf_event_log(MSG_TYPE_ERROR, s);
        status = STATUS_ADAPTER_HARDWARE_ERROR;
        goto error;
    }

    return STATUS_SUCCESS;

error:
    HERMESCleanUp(DriverObject, pCtx);

exit:
    return status;
}

//
// Clean up for HERMES.
//
static void HERMESCleanUp(IN PDRIVER_OBJECT DriverObject, HERMEScontext *pCtx)
{
    unsigned int i;

#   ifdef DEBUG
    DEBUG1("Cleaning up HERMES.\n");
#   endif

    if (pCtx == NULL) return;

    for (i=0; i<pCtx->channelCount; i++) {
        if (pCtx->channel[i]) VDeregisterChannel(pCtx->channel[i]);
    }
    for (i=0; i<pCtx->channelCount; i++) {
        if (pCtx->channel[i]) VDeleteChannel(pCtx->channel[i]);
        pCtx->channel[i] = NULL;
    }

    HERMESFreeResources(pCtx);
    VFree(pCtx);
}


void HERMESDisconnectInterrupt(void *p)
{
    HERMEScontext   *pCtx = (HERMEScontext*) p;

#   ifdef DEBUG
    DEBUG1("HERMES: free resources & disco intr.\n");
#   endif

    HERMESFreeResources(pCtx);
}

/*
** Attempt to find our adapters in the PCI address space. 
*/
VOID ScanPci(IN OUT ULONG * BoardCount,
             IN OUT ULONG * busNumber,
             IN OUT ULONG * slotNumber)
{
    PCI_SLOT_NUMBER     SlotNumber;
    PPCI_COMMON_CONFIG  PciData; 
    UCHAR               buffer[PCI_COMMON_HDR_LENGTH]; 
    ULONG               i, f, j, bus; 
    ULONG               df;
    BOOLEAN             flag; 
    UCHAR               vendorString[5] = {0}; 
    UCHAR               deviceString[5] = {0}; 

    PciData = (PPCI_COMMON_CONFIG) buffer; 
    SlotNumber.u.bits.Reserved = 0; 

    df = 1;
    flag = TRUE; 
    for (bus = 0; flag; bus++) {
        for (i = 0; i < PCI_MAX_DEVICES  &&  flag; i++) {
            SlotNumber.u.bits.DeviceNumber = i;

            for (f = 0; f < PCI_MAX_FUNCTION; f++) {
                SlotNumber.u.bits.FunctionNumber = f;

                j = HalGetBusData (PCIConfiguration, bus,
                                   SlotNumber.u.AsULONG,
                                   PciData, PCI_COMMON_HDR_LENGTH);

                if (j == 0) {
                    // out of buses
                    flag = FALSE;
                    break;
                }

                if (PciData->VendorID == PCI_INVALID_VENDORID) {
                    // skip to next slot
                    break;
                }

                HalSetBusData (PCIConfiguration,
                               bus, SlotNumber.u.AsULONG,
                               PciData, 4);

                HalGetBusData (PCIConfiguration,
                               bus, SlotNumber.u.AsULONG,
                               PciData, PCI_COMMON_HDR_LENGTH);


                if (PciData->VendorID != HERMES_VENDOR_ID) continue;
                
                if (PciData->DeviceID == HERMES_DEVICE_ID ||
                    PciData->DeviceID == HERMES_DEVICE_ID_PROTO) {
                    int b;
                    
                    if (PciData->DeviceID == HERMES_DEVICE_ID_PROTO) {
                        DIAGNOSTIC(HERMES_NAME ": This is a prototype board.");
                    }

                    b = *BoardCount;
                    // For now, we're only interested in the first card.
                    if (b == 0) {
                        busNumber[b] = bus;
                        slotNumber[b] = SlotNumber.u.AsULONG;
                    } 
                    (*BoardCount)++;
                }
            }
        }
    }
}

