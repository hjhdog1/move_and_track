/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** VCAN minidriver (VxD) for PCcan.
** This is the "main program".
*/

#define DEVICE_MAIN
#include <stdinc.h>
#include "pccvxd.h"
#undef DEVICE_MAIN


#define KVAPCCX_Major           VCAND_Major
#define KVAPCCX_Minor           VCAND_Minor
#define KVAPCCX_DeviceID        UNDEFINED_DEVICE_ID
#define KVAPCCX_Init_Order      UNDEFINED_INIT_ORDER
#define KVAPCCX_Index           ""


#include "pccan_version.h"
#include "version.h"
#include "pcchw.h"
#include "pccxilin.h"

Declare_Virtual_Device(KVAPCCX)         // Declare Device Data Block

//------------------------------------------------------------------------------
// Global
//------------------------------------------------------------------------------

struct card {
    IRQHANDLE         IrqHandle;        // IRQ handle
    BYTE              Irq;              // IRQ number
    DEVNODE           Devnode;          // DEVNODE
    PPCCANcontext     pHwContext;       // Hardware context
    PVchannel         pChan[PCCAN_CHANNEL_COUNT];
    VPICD_HWInt_THUNK IrqThunk;         // IRQ thunk
    VPICD_EOI_THUNK   EoiThunk;         // EOI thunk
} card[2];

//------------------------------------------------------------------------------
// Forward declarations.
//------------------------------------------------------------------------------

CONFIGRET OnConfigure(CONFIGFUNC cf, SUBCONFIGFUNC scf, DEVNODE devnode,
                      DWORD refdata, ULONG flags);

//------------------------------------------------------------------------------
// Interrupt handler
//------------------------------------------------------------------------------

// IRQ Handler
BOOL __stdcall PCCANIrqHandler(VMHANDLE hVM, IRQHANDLE hIRQ)
{
    (void)hVM;
    
    if (hIRQ == card[0].IrqHandle) {
        PCCANInterruptService(card[0].pHwContext);
    } else if (hIRQ == card[1].IrqHandle) {
        PCCANInterruptService(card[1].pHwContext);
    }
    VPICD_Phys_EOI(hIRQ);
    return TRUE;
}

// EOI Handler
VOID __stdcall PCCANEoiHandler(VMHANDLE hVM, IRQHANDLE hIRQ)
{
    VPICD_Clear_Int_Request(hVM, hIRQ);
}

//
// Load VCAND.vxd
//
static int loadVCAND(void)
{
    PDEVICEINFO pDevInfo;
    PDDB pDdb;
    DWORD result;

#   ifdef DEBUG
    DEBUG1("Loading VCAND.\n");
#   endif
    result = VXDLDR_LoadDevice("VCAND.VXD", VXDLDR_INIT_DEVICE,
                               &pDevInfo, &pDdb);
    if (result) {
        switch (result) {
            case VXDLDR_ERR_FILE_OPEN_ERROR:
#               ifdef DEBUG
                DEBUG1("ERROR: VCAND.VXD file not found.\n");
#               endif
                break;
                
            default:
#               ifdef DEBUG
                DEBUG1("ERROR: VCAND.VXD can´t be loaded, error=%u.\n", result);
#               endif
                break;
        }
        return FALSE;
    }

    return TRUE;
}


//
// Clean up a card object.
//
static void cleanUp( int cardNo )
{
    int i;

#   ifdef DEBUG
    DEBUG1("Cleaning up card %d\n", cardNo);
#   endif

    for (i=0; i<PCCAN_CHANNEL_COUNT; i++) {
        if (card[cardNo].pChan[i]) {
            VDeregisterChannel(card[cardNo].pChan[i]);
        }
    }
    
    for (i=0; i<PCCAN_CHANNEL_COUNT; i++) {
        if (card[cardNo].pChan[i]) {
            VDeleteChannel(card[cardNo].pChan[i]);
            card[cardNo].pChan[i] = 0;
        }
    }
    
    if (card[cardNo].pHwContext) {
        VFree(card[cardNo].pHwContext);
        card[cardNo].pHwContext = 0;
    }

    // Release interrupts
    if (card[cardNo].IrqHandle) {
        VPICD_Physically_Mask(card[cardNo].IrqHandle);
        VPICD_Force_Default_Behavior(card[cardNo].IrqHandle);

        // Tell VCAND the interrupt handle is no more valid
        card[cardNo].IrqHandle = 0;
        VSetNotifyInt(card[cardNo].IrqHandle, card[cardNo].Irq);
    }

    card[cardNo].Devnode = 0;
}


//
// Initialize and start the device.
// Called from OnConfigure() when CM is configuring us.
//
// (If you load the vxd manually with Vireo's dbgmon.exe (for example),
// you should move this code to OnDeviceInit() instead for debugging.)
//
Vstatus OnConfigStart( DEVNODE devnode, BYTE irq, WORD portBase )
{
    int i;
    int cardNo;
    Vstatus vErr;
    DWORD vcandVersion;
    PCCANcontext *pCtx;
    struct VPICD_IRQ_Descriptor IRQdesc;

#   ifdef DEBUG
    DEBUG1("[OnConfigStart]\n");
#   endif

    // Reset the time to 0
    VInitTime();

    // Try to load the basic driver.
    loadVCAND();

    // Find a unused card descriptor.
    if (card[0].Devnode == 0) {
        cardNo = 0;
    } else if (card[1].Devnode == 0) {
        cardNo = 1;
    } else {
        vErr = VERROR; // qqq
        goto exit;
    }

    // Check VCAND
    if ((vcandVersion = VGetVersion()) == 0) {
#       ifdef DEBUG
        DEBUG1("ERROR: VCAND API not found\n");
#       endif
        vErr = VERROR; // qqq
        goto exit;
    }

    // From now on, DIAGNOSTIC output via VCAND is possible
#   ifdef DEBUG
    DIAGNOSTIC("PCcan V" VERSION_STRING " (Checked) " BUILD_TIME_STRING "\n");
#   else
    DIAGNOSTIC("PCcan V" VERSION_STRING " " BUILD_TIME_STRING "\n");
#   endif

    // check VCANDs version (2.4 or better req'd)
    if (vcandVersion < 0x204) {
        DIAGNOSTIC("ERROR: VCAND/PCcan version mismatch.\n");
        DIAGNOSTIC("Check vcand.vxd - it's too old!\n");
        vErr = VERROR; // qqq
        goto exit;
    }
    
    // Save the device node
    card[cardNo].Devnode = devnode;

#   ifdef DEBUG
    DEBUG1("PCcan: Initializing card %u.\n", cardNo);
#   endif

    // Allocate a context and clear it.
    pCtx = VMalloc(sizeof(PCCANcontext));
    memset(pCtx, 0, sizeof(*pCtx));
    card[cardNo].pHwContext = pCtx;

    pCtx->IRQ = irq;
    // We want the *BASE* address but Windows might (or rather, will) send us
    // one of the other address ranges. So we strip the upper bits.
    pCtx->baseAddress = portBase &= 0x3FF;

    DIAGNOSTIC("PCcan: Searching at base address %04Xh.\n", pCtx->baseAddress);
    DIAGNOSTIC("PCcan: Using IRQ %u.\n", irq);
    
    // Verify the card.
    if ((vErr = PCCAN_DetectCard(pCtx)) != VSUCCESS) {
        DIAGNOSTIC("ERROR: PCcan hardware not found.\n");
        goto error;
    }

    for (i=0; i<pCtx->channelCount; i++) {
        char s[32];
        
        sprintf(s, "PCcan Channel %u", i);
        card[cardNo].pChan[i] = VCreateChannel(HWTYPE_PCCAN, cardNo, i,
                                               s, i,
                                               sizeof(Vchan_extension));
    }

    for (i=0; i<pCtx->channelCount; i++) {
        card[cardNo].pChan[i]->pHwContext = pCtx;
        card[cardNo].pChan[i]->pHwTransmitEvent = PCCANTransmitEvent;
        card[cardNo].pChan[i]->transceiverType = TRANSCEIVER_TYPE_NONE;
    }

    // Register the channels with VCAND.
    for (i=0; i<pCtx->channelCount; i++) {
        VRegisterChannel(card[cardNo].pChan[i]);
    }

    for (i=0; i<pCtx->channelCount; i++) {
        pCtx->channel[i] = card[cardNo].pChan[i];
    }

    // Hook IRQ
    IRQdesc.VID_IRQ_Number       = irq;	// IRQ to virtualize
    IRQdesc.VID_Options          = 0;
    IRQdesc.VID_Hw_Int_Proc      = (DWORD)VPICD_Thunk_HWInt(PCCANIrqHandler, &card[cardNo].IrqThunk);
#   ifdef SIMULATE_HARDWARE_IRQ
    IRQdesc.VID_EOI_Proc         = (DWORD)VPICD_Thunk_EOI(PCCANEoiHandler, &card[cardNo].EoiThunk);
#   else
    IRQdesc.VID_EOI_Proc         = 0;
#   endif
    IRQdesc.VID_Virt_Int_Proc    = 0;
    IRQdesc.VID_Mask_Change_Proc = 0;
    IRQdesc.VID_IRET_Proc        = 0;
    IRQdesc.VID_IRET_Time_Out    = 500;
    card[cardNo].IrqHandle       = VPICD_Virtualize_IRQ(&IRQdesc);
    if (card[cardNo].IrqHandle == 0) {
        DIAGNOSTIC("ERROR: PCcan failed to hook IRQ %u.\n", irq);
        DIAGNOSTIC("Are there available IRQ's in this machine?\n");
        vErr = VERROR; // qqq
        goto error;
    }
    card[cardNo].Irq = irq;
    VPICD_Physically_Unmask(card[cardNo].IrqHandle);

    // Tell VCAND of a hardware interrupt to use
    VSetNotifyInt(card[cardNo].IrqHandle, irq);

    if ((vErr = PCCAN_InitCard(pCtx)) != VSUCCESS) {
        goto error;
    }
    
    return VSUCCESS;

error:
    cleanUp(cardNo);

exit:
    return vErr;
}

//
// Stop the device.
// Called from OnConfigure() when CM is configuring us.
//
Vstatus OnConfigStop( DEVNODE devnode )
{
    int cardNo;
    
#   ifdef DEBUG
    DEBUG1("[OnConfigStop]\n");
#   endif

    // Find the card.
    if (devnode==card[0].Devnode) cardNo = 0;
    else if (devnode==card[1].Devnode) cardNo = 1;
    else return VERROR;

    // Shutdown the hardware.
    if (card[cardNo].pHwContext) {
        PCCAN_KillCard(card[cardNo].pHwContext);
    }

    // Clean up memory.
    cleanUp(cardNo);

    return VSUCCESS;
}



//------------------------------------------------------------------------------
// VxD Initialization Routines.
// These are all (almost) dummy routines as we are doing the real
// initialization in cooperation with the CM anyway.
//------------------------------------------------------------------------------

#pragma argsused
BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
#   ifdef DEBUG
    DEBUG1("[OnDeviceInit]\n");
#   endif
    VMemSet(card, 0, sizeof(card));
    return TRUE;
}

#pragma argsused
VOID OnSystemExit(VMHANDLE hVM)
{
#   ifdef DEBUG
    DEBUG1("[OnSystemExit]\n");
#   endif
}

BOOL OnSysDynamicDeviceInit(void)
{
#   ifdef DEBUG
    DEBUG1("[OnSysDynamicDeviceInit]\n");
#   endif
    return OnDeviceInit(0,0);
}

BOOL OnSysDynamicDeviceExit(void)
{
#   ifdef DEBUG
    DEBUG1("[OnSysDynamicDeviceExit\n");
#   endif

    OnSystemExit(0);
    return TRUE;
}

//------------------------------------------------------------------------------
// Handlers for Control Messages
//------------------------------------------------------------------------------

DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYSTEM_EXIT, OnSystemExit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(W32_DEVICEIOCONTROL, OnDeviceIoctl);
DefineControlHandler(PNP_NEW_DEVNODE, OnPnpNewDevnode);

//------------------------------------------------------------------------------
// CONTROL-Dispatcher
//------------------------------------------------------------------------------

#pragma argsused
BOOL __cdecl ControlDispatcher(DWORD dwControlMessage,
                               DWORD EBX,
                               DWORD EDX,
                               DWORD ESI,
                               DWORD EDI,
                               DWORD ECX)
{
    START_CONTROL_DISPATCH

    ON_DEVICE_INIT(OnDeviceInit);
    ON_SYSTEM_EXIT(OnSystemExit);
    ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
    ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
    ON_W32_DEVICEIOCONTROL(OnDeviceIoctl);
    ON_PNP_NEW_DEVNODE(OnPnpNewDevnode);

    END_CONTROL_DISPATCH

    return TRUE;
}

//------------------------------------------------------------------------------
// Device-IO-Control-Handler
//------------------------------------------------------------------------------

DWORD OnDeviceIoctl(PIOCTLPARAMS pIOC)
{
    Vstatus status = VSUCCESS;

    switch (pIOC->dioc_IOCtlCode) {

        case DIOC_OPEN:
#           ifdef DEBUG
            DEBUG1("[OnDeviceIoctl] DIOC_OPEN\n");
#           endif
            break;

        case DIOC_CLOSEHANDLE:
#           ifdef DEBUG
            DEBUG1("[OnDeviceIoctl] DIOC_CLOSEHANDLE\n");
#           endif
            break;

        default:
#           ifdef DEBUG
            DEBUG1("[OnDeviceIoctl] unknown command %u\n", pIOC->dioc_IOCtlCode);
#           endif
            status = VERR_WRONG_PARAMETER;
            break;
    }

#   ifdef DEBUG
    if (gDebugLevel>=2&&
        status&&
        status!=VERR_QUEUE_IS_EMPTY) DEBUG1("status=%d\n",status);
#   endif
    return status;
}




//------------------------------------------------------------------------------
// PnP
//------------------------------------------------------------------------------


//
// The following declarations define the strings used to identify calls to the driver
// when building for debug.
//
#ifdef DEBUG

#undef NUM_CONFIG_COMMANDS
#define NUM_CONFIG_COMMANDS     0x00000019      // For DEBUG.

char    CMFAR *lpszConfigName[NUM_CONFIG_COMMANDS]= \
{ \
"CONFIG_FILTER", \
"CONFIG_START", \
"CONFIG_STOP", \
"CONFIG_TEST", \
"CONFIG_REMOVE", \
"CONFIG_ENUMERATE", \
"CONFIG_SETUP", \
"CONFIG_CALLBACK", \
"CONFIG_APM", \
"CONFIG_TEST_FAILED", \
"CONFIG_TEST_SUCCEEDED", \
"CONFIG_VERIFY_DEVICE", \
"CONFIG_PREREMOVE", \
"CONFIG_SHUTDOWN", \
"CONFIG_PREREMOVE2", \
"CONFIG_READY", \
"CONFIG_PROP_CHANGE", \
"CONFIG_PRIVATE", \
"CONFIG_PRESHUTDOWN", \
"CONFIG_BEGIN_PNP_MODE", \
"CONFIG_LOCK", \
"CONFIG_UNLOCK", \
"CONFIG_IRP", \
"CONFIG_WAKEUP", \
"CONFIG_WAKEUP_CALLBACK", \
};

char *substart[] = {
    "DYNAMIC_START",
    "FIRST_START"
};
char *substop[] = {
    "DYNAMIC_STOP",
    "HAS_PROBLEM"
};
char *subremove[] = {
    "DYNAMIC",
    "SHUTDOWN",
    "REBOOT"
};
char *subtest[] = {
    "CAN_STOP",
    "CAN_REMOVE"
};
char *subapm[] = {
    "TEST_STANDBY",
    "TEST_SUSPEND",
    "TEST_STANDBY_FAILED",
    "TEST_SUSPEND_FAILED",
    "TEST_STANDBY_SUCCEEDED",
    "TEST_SUSPEND_SUCCEEDED",
    "RESUME_STANDBY",
    "RESUME_SUSPEND",
    "RESUME_CRITICAL",
    "UI_ALLOWED"
};
#endif  // DEBUG

// 
// Handler for control message PNP_NEW_DEVNODE.
// The registry identifies this VxD as the device loader for the device.
// When the CONFIGMG sends the PNP_NEW_DEVNODE message, this VxD responds
// by registering as the *driver* for the specified devnode.
//
CONFIGRET OnPnpNewDevnode(DEVNODE Node, DWORD LoadType)
{
#   ifdef DEBUG
    DEBUG1("[OnPnpNewDevnode]\n");
#   endif
    
    switch (LoadType) {
        case DLVXD_LOAD_DEVLOADER:
            return CONFIGMG_Register_Device_Driver(
                Node,
                OnConfigure,
                0,
                CM_REGISTER_DEVICE_DRIVER_REMOVABLE |
                CM_REGISTER_DEVICE_DRIVER_DISABLEABLE
                );
    };

    return CR_DEFAULT;
}

//
// Configuration handler.
// This is the routine that this VxD supplies to CONFIGMG when registering as
// the device driver. It handles various configuration messages sent
// by CONFIGMG.
//
#pragma argsused
CONFIGRET OnConfigure(
                      CONFIGFUNC cf, 		// function id
                      SUBCONFIGFUNC scf,	// subfunction id
                      DEVNODE devnode,      // device node being configured
                      DWORD refdata, 		// context information (function specific)
                      ULONG flags           // function specific flags
                     )
{
    CMCONFIG config;

#   ifdef DEBUG
    char *subfunc = "";
    char id[MAX_DEVICE_ID_LEN];

    switch (cf)
    {
        case CONFIG_START:
            subfunc = substart[scf];
            break;
        case CONFIG_STOP:
            subfunc = substop[scf];
            break;
        case CONFIG_REMOVE:
            subfunc = subremove[scf];
            break;
        case CONFIG_TEST:
            subfunc = subtest[scf];
            break;
        case CONFIG_APM:
            subfunc = subapm[scf];
            break;
    }

    CONFIGMG_Get_Device_ID(devnode, id, sizeof(id), 0);
    
    if (cf < NUM_CONFIG_COMMANDS) {
        DEBUG1("[OnConfigure] cf=%s, scf=%s, deviceId=%s\n",
               lpszConfigName[cf], subfunc, id);
    } else {
        DEBUG1("[OnConfigure] cf=0x%x, scf=0x%x, deviceId='%s'\n", cf, scf, id);
    }
#   endif

    switch (cf) {

        case CONFIG_START:
            
            CONFIGMG_Get_Alloc_Log_Conf(&config, devnode,
                                        CM_GET_ALLOC_LOG_CONF_ALLOC);
#           ifdef DEBUG
            DEBUG1("irqCount=%u, irq=%d\n",
                   config.wNumIRQs,
                   config.bIRQRegisters[0]);
            DEBUG1("portCount=%u, portBase=0x%03X, portLen=%d\n",
                   config.wNumIOPorts,
                   config.wIOPortBase[0],
                   config.wIOPortLength[0]);
#           endif
            if (config.wNumIRQs > 0 && config.wIOPortBase[0] > 0) {
                Vstatus stat;

                stat = OnConfigStart(devnode, config.bIRQRegisters[0],
                                     config.wIOPortBase[0]);
#               ifdef DEBUG
                if (stat != VSUCCESS) DEBUG1("ERROR: CONFIG_START failed\n");
#               endif

                switch(stat) {
                    case VSUCCESS:
                        return CR_SUCCESS;
                    case VERR_HW_NOT_PRESENT:
                        return CR_DEVICE_NOT_THERE;
                    default:
                        return CR_FAILURE;
                }
            } else {
#               ifdef DEBUG
                DEBUG1("ERROR: CONFIG_START - no resources!\n");
#               endif
                return CR_INVALID_LOG_CONF;
            }

        case CONFIG_REMOVE:
        case CONFIG_STOP:
        case CONFIG_SHUTDOWN:
            if (OnConfigStop(devnode) == VSUCCESS) {
                return CR_SUCCESS;
            } else {
                return CR_FAILURE;
            }

        default:
            return CR_DEFAULT;
    }
}

