/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#include <stdinc.h>
#include "pccvxd.h"

#include "pccres.h"
#include "pcchw.h"
#include "vcan_msg.h"

#define PCCAN_IOAREAS 12

static const struct {
    ULONG addr;
    ULONG size;
} ioMap[PCCAN_IOAREAS] = {
    {0x6000, 0x04},  // Ports in the Xilinx
    {0x2000, 0x40},  // Intel 82527 #1
    {0x2400, 0x40},
    {0x2800, 0x40},
    {0x2C00, 0x40},
    {0x3000, 0x40},  // Intel 82527 #2
    {0x3400, 0x40},
    {0x3800, 0x40},
    {0x3C00, 0x40},
    {0x4000, 0x40},  // Philips 82c200/SJA1000 #1
    {0x5000, 0x40},  // Philips 82c200/SJA1000 #2
    {0x0,    0x40}   // Mark the base address as used
};


Vstatus PCcanClaimResources(PPCCANcontext pCtx,
                            IN IoAddress baseAddress,
                            IN ULONG irq)
{
    ULONG sizeOfResourceList;
    PCM_RESOURCE_LIST resourceList;
    PCM_FULL_RESOURCE_DESCRIPTOR nextFrd;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR partial;
    BOOLEAN conflictDetected;
    KAFFINITY Affinity;
    ULONG MappedVector;
    KIRQL irql;
    int i, tmp;
    NTSTATUS         stat;

#ifdef DEBUG
    DEBUG1("[PCcanClaimResources]\n");
#endif

    
    // The PCCAN card (1.0B-3) uses no less than PCCAN_IOAREAS=11 different
    // areas in I/O space.  We must report all of these as well as the IRQ
    // we are going to use.  This makes for a total of 12 partial resource
    // descriptors (PRDs).
    sizeOfResourceList = sizeof(CM_FULL_RESOURCE_DESCRIPTOR);

    // The full resource descriptor (FRD) contains one PRD already. Make
    // room for 10 more for the IO_AREAS, and one for the IRQ.
    sizeOfResourceList += (PCCAN_IOAREAS)*sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);

    //
    // Take into account what preceedes the first FRD in the resource list.
    //
    sizeOfResourceList += FIELD_OFFSET(CM_RESOURCE_LIST, List[0]);

    resourceList = ExAllocatePool(PagedPool, sizeOfResourceList);

    if (!resourceList) {
        // qqq error log
        return VERROR; 
    }

    //
    // Zero out the list
    //
    RtlZeroMemory(resourceList, sizeOfResourceList);

    resourceList->Count = 1;
    nextFrd = &resourceList->List[0];

    nextFrd->InterfaceType = Isa;
    nextFrd->BusNumber = 0;

    //
    // We are going to report several port addresses and 1 interrupt
    //
    nextFrd->PartialResourceList.Count = PCCAN_IOAREAS+1;

    //
    // Now fill in the port data.  We don't wish to share
    // this port range with anyone.
    //
    // We are using the untranslated address which should be the same
    // as the translated on x86 platforms.
    //

    for (i = 0; i < PCCAN_IOAREAS; i++) {
        PHYSICAL_ADDRESS PortAddress;
        PHYSICAL_ADDRESS MappedAddress;
        ULONG            MemType;

        PortAddress.LowPart  = ioMap[i].addr + (ULONG)baseAddress;
        PortAddress.HighPart = 0;

        // Convert the I/O port address into a form NT likes.
        MemType = 1; // 1 = located in I/O space
        HalTranslateBusAddress(Isa,
                               0,
                               PortAddress,
                               &MemType,
                               &MappedAddress);

#if 0
        ASSERT(MemType == 1 &&
               MappedAddress.LowPart == PortAddress.LowPart &&
               MappedAddress.HighPart == 0);
#endif

        partial = &nextFrd->PartialResourceList.PartialDescriptors[i];
        partial->Type =             CmResourceTypePort;
        partial->ShareDisposition = CmResourceShareDriverExclusive;
        partial->Flags =            CM_RESOURCE_PORT_IO;
        partial->u.Port.Start =     PortAddress;
        partial->u.Port.Length =    ioMap[i].size;
    }

    //
    // This call will map our IRQ to a system vector. It will also fill
    // in the IRQL (the kernel-defined level at which our ISR will run),
    // and affinity mask (which processors our ISR can run on).
    //
    // We need to do this so that when we connect to the interrupt, we
    // can supply the kernel with this information.
    //
    MappedVector = HalGetInterruptVector(Isa,           // Interface type
                                         0,             // Bus number
                                          irq,   // Bus interrupt level
                                          irq,   // Bus interrupt vector
                                         &irql,         // From HAL: IRQ level
                                         &Affinity);    // Affinity mask

    //
    // Now fill in the irq stuff.
    //
    // Note: for IoReportResourceUsage, the Interrupt.Level and
    // Interrupt.Vector are bus-specific level and vector, just
    // as we passed in to HalGetInterruptVector, not the mapped
    // system vector we got back from HalGetInterruptVector.
    //
    partial = &nextFrd->PartialResourceList.PartialDescriptors[PCCAN_IOAREAS];
    partial->Type =                 CmResourceTypeInterrupt;
    partial->u.Interrupt.Level =    irq;
    partial->u.Interrupt.Vector =   irq;
    partial->ShareDisposition =     CmResourceShareDriverExclusive;
    partial->Flags =                CM_RESOURCE_INTERRUPT_LATCHED;
    
    stat = IoReportResourceUsage(NULL,
                          pCtx->DriverObject,
                          resourceList,
                          sizeOfResourceList,
                          NULL,
                          NULL,
                          0,
                          FALSE,
                          &conflictDetected);

    ExFreePool(resourceList);

    //
    // We get resource conflicts under NT5B2 for some reason. The PCI bus has
    // claimed most of the I/O ports... This is one way of solving it; another
    // way would be to report just the resources below 7FF and hope that's
    // enough.
    // When B3 arrives, we'll report this bug to MS.
    //
    if ((VGetOption("IgnoreResourceConflicts", &tmp) == FALSE) || (tmp == 0)) {
    } else {
        if (conflictDetected) {
            DIAGNOSTIC("PCcan: RESOURCE CONFLICT DETECTED\n");
            DIAGNOSTIC("PCcan: Conflict ignored.\n");
        }
        conflictDetected = 0;
    }
    
    if (conflictDetected) {
        printf_event_log(MSG_TYPE_ERROR, "PCcan resource conflict.");
        DIAGNOSTIC("PCcan: RESOURCE CONFLICT DETECTED\n");
        return VERROR; // qqq
    }
    
    //
    // Connect the device driver to the IRQ
    //
    if (!conflictDetected) {
        // qqq init DPC?
        NTSTATUS stat;

        pCtx->InterruptObject = NULL;
        stat = IoConnectInterrupt(&pCtx->InterruptObject,
                                  PCCANInterruptService,
                                  pCtx,
                                  NULL,
                                  MappedVector,
                                  irql,
                                  irql,
                                  Latched,
                                  FALSE,
                                  Affinity,
                                  FALSE);

        if (!NT_SUCCESS(stat)) {
            printf_event_log(MSG_TYPE_ERROR, "PCcan failed IRQ allocation.");
            DIAGNOSTIC("ERROR: PCcan failed IRQ allocation. (%lx)\n", stat);
            PCcanFreeResources(pCtx);
            return VERROR; // qqq
        }
    }

    if (conflictDetected) {
        PCcanFreeResources(pCtx);
        return VERROR;
    }

    return VSUCCESS;
}


void PCcanFreeResources(PPCCANcontext pCtx)
{
    CM_RESOURCE_LIST NullResourceList;
    BOOLEAN ResourceConflict;

#ifdef DEBUG
    DEBUG1("[PCcanFreeResources]\n");
#endif

    if (pCtx->InterruptObject) IoDisconnectInterrupt(pCtx->InterruptObject);
    pCtx->InterruptObject= NULL;

    //
    // Deallocate the resources.
    //
    RtlZeroMemory((PVOID)&NullResourceList, sizeof(NullResourceList));
    IoReportResourceUsage(NULL,
                          pCtx->DriverObject,
                          &NullResourceList,
                          sizeof(ULONG),
                          NULL,
                          NULL,
                          0,
                          FALSE,
                          &ResourceConflict);
}
