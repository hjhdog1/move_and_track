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

#include "pcchw.h"
#include "pccxilin.h"
#include "pcc_time.h"

// Forward declaration.
static void ReportLicense(PPCCANcontext pCtx, int chan);


//
// PCCANInterruptService()
// ISR for the connected interrupt for NT.
//
#ifdef __WIN_NT__
BOOLEAN PCCANInterruptService(IN PKINTERRUPT interrupt, IN OUT PVOID context)
{
    PCCAN_Interrupt((PPCCANcontext)context);
    return TRUE; // qqq shared interrupts
}
#endif


//
// PCCANInterruptService()
// ISR for the connected interrupt for W95.
//
#ifdef __WIN_95__
void PCCANInterruptService(PPCCANcontext pCtx)
{
    PCCAN_Interrupt(pCtx);
}
#endif


#ifdef __BORLANDC__
#pragma argsused
#endif
Vstatus PCCANTransmitEvent(PVchannel pChan, PVevent pEvent, unsigned long tout)
{
    Vstatus         vErr;
    PPCCANcontext   pCtx = (PPCCANcontext)pChan->pHwContext;
    int             chan = pChan->hwChannel;

#   ifdef DEBUG
    DEBUG1("TxEvt chan=%d evt=%d addr=%x\n", chan, pEvent->tag, pCtx->C[chan].address);
#   endif

    switch (pEvent->tag) {

        // Set the debug level
        case V_SET_DEBUGLEVEL:
            gDebugLevel = pEvent->tagData.setDebugLevel.level;
            vErr = VSUCCESS;
            break;

        // Initialize the hardware
        case V_INIT_HARDWARE:
            vErr = PCCAN_InitChannel(pCtx, chan);
            break;

        case V_EXIT_HARDWARE:
            vErr = PCCAN_KillChannel(pCtx, chan);
            break;

        case V_TRANSMIT_MSG:
            pEvent->tagData.msg.flags &= ~(MSGFLAG_TX|MSGFLAG_TXRQ);
            pEvent->tagData.msg.flags |= pChan->flags;
            vErr = pCtx->C[chan].WriteMessage(pCtx, chan, pEvent);
            break;

        case V_RESET_CLOCK:
            ResetTime();
            vErr = VSUCCESS;
            break;

        case V_RESET_CHIP:
            vErr = pCtx->C[chan].BusOff(pCtx, chan, pEvent);
            break;

        case V_INIT_CHIP:
            vErr = pCtx->C[chan].BusParams(pCtx, chan, pEvent);
            break;

        case V_SET_ACCEPTANCE:
            vErr = pCtx->C[chan].SetHwFilter(pCtx, chan, pEvent);
            break;

        case V_START_CHIP:
            vErr = pCtx->C[chan].BusOn(pCtx, chan, pEvent);
            (void)pCtx->C[chan].GetChipState(pCtx, chan, pEvent);
            break;

        case V_STOP_CHIP:
            vErr = pCtx->C[chan].BusOff(pCtx, chan, pEvent);
            (void)pCtx->C[chan].GetChipState(pCtx, chan, pEvent);
            break;
            
        case V_GET_STATISTIC:
            // qqq
            vErr = VSUCCESS;
            break;

        case V_GET_CHIP_STATE:
            vErr = pCtx->C[chan].GetChipState(pCtx, chan, pEvent);
            break;

        case V_SET_TIMER:
            // qqq
            vErr = VSUCCESS;
            break;

        case V_SET_OUTPUT_MODE:
            vErr = pCtx->C[chan].BusOutputControl(pCtx, chan, pEvent);
            break;

        case V_CHECK_LICENSE:
            ReportLicense(pCtx, chan);
            vErr = VSUCCESS;
            break;
            
        case V_SET_TRANSCEIVER:
        case V_FUNCTION:
            // qqq
            vErr = VSUCCESS;
            break;

        case V_READ_CLOCK:
            // qqq check this!
            vErr = VSUCCESS;
            break;
            
        default:
            vErr = VERROR;
            break;
    }

    return vErr;
}

//
// We don't have any license bits but provide an answer just to be polite.
//
static void ReportLicense(PPCCANcontext pCtx, int chan)
{
    PVchannel pChan;
    Vevent e;
    
    pChan = pCtx->channel[chan];
    
    e.tag           = V_CHECK_LICENSE_RESP;
    e.chanIndex     = pChan->index;
    e.transId       = 0;
    e.portHandle    = (BYTE) INVALID_PORTHANDLE;
    e.timeStamp     = 0;
    e.tagData.checkLicenseResp.isValid = 0;

    VDispatchReceiveEvent(pChan, &e);
}
