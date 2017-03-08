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


#define MAKE_TRANSID(portHandle,appTransId) ((BYTE)(((appTransId)<<3)|(portHandle&0x07)))
#define TRANSID_PORTHANDLE(transId) ((BYTE)((((transId)&0x07)==0x07) ? 0xFF : ((transId)&0x07)))
#define TRANSID_TRANSID(transId) ((BYTE)((transId)>>3))

#include "pccxilin.h"
#include <pccan.h>
#include <sja1000.h>
#include "pcc_time.h"
#include "pcc1000.h"

#include <pshpack1.h>
typedef union {
    DWORD L;
    struct { WORD lsw, msw; } W;
    struct { BYTE b0, b1, b2, b3; } B;
} WL;
#include <poppack.h>


// Forward declarations
void PCC1000_Interrupt(PPCCANcontext pCtx, int chan);
static void PCC1000_Receive_ISR(PPCCANcontext pCtx, int chan);
static void PCC1000_Transmit_ISR(PPCCANcontext pCtx, int chan);
static void PCC1000_Error_ISR(PPCCANcontext pCtx, int chan);
static void PCC1000_Overrun_ISR(PPCCANcontext pCtx, int chan);
static void PCC1000_BusError_ISR(PPCCANcontext pCtx, int chan);
static void PCC1000_ErrorPassive_ISR(PPCCANcontext pCtx, int chan);

Vstatus PCC1000_WriteMessage(PPCCANcontext pCtx, int chan, Vevent *e);
Vstatus PCC1000_BusParams(PPCCANcontext pCtx, int chan, Vevent *e);       
Vstatus PCC1000_SetHwFilter(PPCCANcontext pCtx, int chan, Vevent *e);     
Vstatus PCC1000_BusOn(PPCCANcontext pCtx, int chan, Vevent *e);           
Vstatus PCC1000_BusOff(PPCCANcontext pCtx, int chan, Vevent *e);          
Vstatus PCC1000_GetChipState(PPCCANcontext pCtx, int chan, Vevent *e);    
Vstatus PCC1000_BusOutputControl(PPCCANcontext pCtx, int chan, Vevent *e);

Vstatus PCC1000_StartWrite(PPCCANcontext pCtx, int chan, Vevent *e);
int PCC1000_TXAvailable(PPCCANcontext pCtx, int chan);



//
// Is this a SJA1000?
// Is is assumed that the RESET signal is NOT asserted.
//
BOOL PCC1000_ProbeChannel(PPCCANcontext pCtx, IoAddress addr, int chan)
{
    BYTE port;

    ARGUSED(pCtx);
    ARGUSED(chan);
    
    // First, reset the chip. Works for 82c200, sja1000 and 82527!
    // The "2" will not be set on the 82527.
    WRITE_PORT_UCHAR(addr + 0, 0x21);

    // Now let's see what we've got.
    port = READ_PORT_UCHAR(addr + 0);

    // If we don't read 0x21 back, then it isn't an sja1000.
    if (port != 0x21) return FALSE;

    // Try to set the Pelican bit.
    WRITE_PORT_UCHAR(addr + PCAN_CDR, PCAN_PELICAN);
    port = READ_PORT_UCHAR(addr + PCAN_CDR);

    if ((port & PCAN_PELICAN) == 0) return FALSE;

#if 0
    // WRITE_PORT_UCHAR(addr + PCAN_CDR, 0);
    // DIAGNOSTIC("Chip %d is an SJA1000.\n", chan);
    // DIAGNOSTIC("We'll run it in 82c200 mode however.\n");
    // return FALSE;
#endif

    return TRUE;
}


Vstatus PCC1000_InitChannel(PPCCANcontext pCtx, int chan)
{
    struct _pccan_hwchannel *p;
    IoAddress addr;
    
    p = (struct _pccan_hwchannel *) &(pCtx->C[chan]);

    // Setup the channel data structure.
    p->WriteMessage     = PCC1000_WriteMessage;
    p->BusParams        = PCC1000_BusParams;       
    p->SetHwFilter      = PCC1000_SetHwFilter;     
    p->BusOn            = PCC1000_BusOn;           
    p->BusOff           = PCC1000_BusOff;          
    p->GetChipState     = PCC1000_GetChipState;    
    p->BusOutputControl = PCC1000_BusOutputControl;

    addr = p->address;
    
    // Reset the circuit...
    WRITE_PORT_UCHAR(addr + PCAN_MOD, PCAN_RM);
    // ...goto Pelican mode...
    WRITE_PORT_UCHAR(addr + PCAN_CDR, PCAN_PELICAN|PCAN_CBP|PCAN_CLOCKOFF);
    // ...and set the filter mode.
    WRITE_PORT_UCHAR(addr + PCAN_MOD, PCAN_RM|PCAN_AFM);
    
    // Activate almost all interrupt sources.
    WRITE_PORT_UCHAR(addr + PCAN_IER,
                     PCAN_BEIE|PCAN_EPIE|PCAN_DOIE|PCAN_EIE|PCAN_TIE|PCAN_RIE);

    // Accept all messages by default.
    WRITE_PORT_UCHAR(addr + PCAN_ACR0, 0);
    WRITE_PORT_UCHAR(addr + PCAN_ACR1, 0);
    WRITE_PORT_UCHAR(addr + PCAN_ACR2, 0);
    WRITE_PORT_UCHAR(addr + PCAN_ACR3, 0);
    WRITE_PORT_UCHAR(addr + PCAN_AMR0, 0xFF);
    WRITE_PORT_UCHAR(addr + PCAN_AMR1, 0xFF);
    WRITE_PORT_UCHAR(addr + PCAN_AMR2, 0xFF);
    WRITE_PORT_UCHAR(addr + PCAN_AMR3, 0xFF);

    // Default 125 kbit/s, pushpull.
    WRITE_PORT_UCHAR(addr + PCAN_BTR0, 0x07);
    WRITE_PORT_UCHAR(addr + PCAN_BTR1, 0x23);
    WRITE_PORT_UCHAR(addr + PCAN_OCR,  0xda);
    
    // Connect the INT line to the IRQ on the ISA bus.
    PCCAN_ConnectIRQ(pCtx, p->chipNo, TRUE);

    return VSUCCESS;
}


/*
 ** Kill a certain CAN circuit. Disconnect interrupt etc.
 ** Used at driver rundown.
 */
Vstatus PCC1000_KillCircuit(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;

    // Disconnect the interupt line
    PCCAN_ConnectIRQ(pCtx, chan, FALSE);

    // Reset the SJA1000 and return to BasicCAN mode.
    WRITE_PORT_UCHAR(circAddr + PCAN_MOD, PCAN_RM);
    WRITE_PORT_UCHAR(circAddr + PCAN_CDR, PCAN_CBP|PCAN_CLOCKOFF);
    WRITE_PORT_UCHAR(circAddr + PCAN_MOD, 0x21);

    return VSUCCESS;
}

Vstatus PCC1000_KillChannel(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;
    
    tmp = READ_PORT_UCHAR(circAddr + PCAN_MOD);
    WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp | PCAN_RM));

    return VSUCCESS;
}

/*
** Enable bus error interrupts, and reset the
** counters which keep track of the error rate
*/
static void ResetErrorCounter(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE ier;
    ier = READ_PORT_UCHAR(circAddr + PCAN_IER);
    WRITE_PORT_UCHAR(circAddr + PCAN_IER, (BYTE)(ier | PCAN_BEIE));
    pCtx->C[chan].ErrorCount = 0;        
    pCtx->C[chan].ErrorTime = GetCurrentTime();
}


/*
 ** Go on bus.
 */
Vstatus PCC1000_BusOn(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    ARGUSED(e);
    pCtx->C[chan].overrun = 0;
    ResetErrorCounter(pCtx, chan);
    
    // Go on bus
    tmp = READ_PORT_UCHAR(circAddr + PCAN_MOD);
    WRITE_PORT_UCHAR(circAddr + PCAN_CMR, PCAN_CDO);
    WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp | PCAN_RM));
    WRITE_PORT_UCHAR(circAddr + PCAN_TXERR, 0);
    WRITE_PORT_UCHAR(circAddr + PCAN_RXERR, 0);
    (void)READ_PORT_UCHAR(circAddr + PCAN_ECC);
    WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp & ~PCAN_RM));

    PCC1000_GetChipState(pCtx, chan, NULL);
    
    return VSUCCESS;
}

/*
 ** Go off bus.
 */
Vstatus PCC1000_BusOff(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    ARGUSED(e);
    tmp = READ_PORT_UCHAR(circAddr + PCAN_MOD);
    WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp|PCAN_RM));

    PCC1000_GetChipState(pCtx, chan, NULL);

    return VSUCCESS;
}

/*
 ** Set the bus parameters.
 */
Vstatus PCC1000_BusParams(PPCCANcontext pCtx, int chan, Vevent *e)
{
    unsigned quantaPerCycle;
    DWORD brp;
    BYTE cbt0;
    BYTE cbt1;
    BYTE tmp;
    int resetStatus;
    DWORD freq;
    BYTE tseg1, tseg2, sjw, sam;
    IoAddress circAddr = pCtx->C[chan].address;

    freq  = e->tagData.init.bitRate;
    sjw   = e->tagData.init.sjw;
    tseg1 = e->tagData.init.tseg1;
    tseg2 = e->tagData.init.tseg2;
    sam   = e->tagData.init.sam;
    sjw--;

    quantaPerCycle = tseg1 + tseg2 + 1;
    if (quantaPerCycle == 0 || freq == 0) return VERR_WRONG_PARAMETER;

    brp = (8000000L * 64) / (freq * quantaPerCycle);
    if ((brp & 0x3F) != 0) {
        // Fraction != 0 : not divisible.
        return VERR_WRONG_PARAMETER;
    }
    brp = (brp >> 6) - 1;
    if (brp > 64 || sjw > 3 || quantaPerCycle < 8) {
        return VERR_WRONG_PARAMETER;
    }
    
    cbt0 = (BYTE)(((BYTE)sjw <<6) + brp);
    cbt1 = (BYTE)(((sam==3?1:0)<<7) + ((tseg2-1)<<4) + (tseg1-1));

    // Put the circuit in Reset Mode
    tmp = READ_PORT_UCHAR(circAddr + PCAN_MOD);
    resetStatus = tmp & PCAN_RM;
    WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp|PCAN_RM));

    WRITE_PORT_UCHAR(circAddr + PCAN_BTR0, cbt0);
    WRITE_PORT_UCHAR(circAddr + PCAN_BTR1, cbt1);

    if (resetStatus == FALSE) {
        tmp = READ_PORT_UCHAR(circAddr + PCAN_MOD);
        WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp & ~PCAN_RM));
    }
    return VSUCCESS;
}


/*
 ** Set driver output type.
 */
Vstatus PCC1000_BusOutputControl(PPCCANcontext pCtx, int chan, Vevent *e)
{
    short driver;
    BYTE tmp;
    IoAddress circAddr = pCtx->C[chan].address;
    Vstatus stat;

    // Save control register
    tmp = READ_PORT_UCHAR(circAddr + PCAN_MOD);
    // Put the circuit in Reset Mode
    WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp|PCAN_RM));
    
    switch (e->tagData.setOutputMode.mode) {
        
        case OUTPUT_MODE_NORMAL:
            driver = 0xDA;  // Pushpull  (OCTP1|OCTN1|OCTP0|OCTN0|OCM1)
            tmp &= ~PCAN_LOM;
            break;
            
        case OUTPUT_MODE_SILENT:
            driver = 0xDA;
            tmp |= PCAN_LOM;
            break;
#if 0
        case OUTPUT_MODE_SELFRECEPTION:
            driver = -1;
            break;
        case OUTPUT_MODE_OFF:
            driver = 0x02;  // Tristate (OCM1)
            tmp &= ~PCAN_LOM;
            break;
#endif
        default:
            driver = -1;
            break;
    }

    if (driver == -1) {
        stat = VERR_WRONG_PARAMETER;
    } else {
        // Set the output control
        WRITE_PORT_UCHAR(circAddr + PCAN_OCR, (BYTE)(driver & 0xff));
        // Restore control register
        WRITE_PORT_UCHAR(circAddr + PCAN_MOD, tmp);
        stat = VSUCCESS;
    }
    return stat;
}

// Note: "1" in the mask == the bit is significant.
Vstatus PCC1000_SetHwFilter(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress circAddr = pCtx->C[chan].address;

    if (e->tagData.acc.mask & EXT_MSG || e->tagData.acc.code & EXT_MSG) {
        //
        // Extended CAN filter.
        //
        BYTE tmp;
        WL c, m;

        c.L = e->tagData.acc.code & ~EXT_MSG;
        m.L = e->tagData.acc.mask & ~EXT_MSG;
        c.L <<= 3;
        m.L <<= 3;
        m.L = ~m.L;
        m.L |= 7;

        // Save control register
        tmp = READ_PORT_UCHAR(circAddr + PCAN_MOD);
        // Put the circuit in Reset Mode
        WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp|PCAN_RM));

        // Set the acceptance code
        WRITE_PORT_UCHAR(circAddr + PCAN_ACR0, c.B.b3);
        WRITE_PORT_UCHAR(circAddr + PCAN_ACR1, c.B.b2);
        WRITE_PORT_UCHAR(circAddr + PCAN_ACR2, c.B.b1);
        WRITE_PORT_UCHAR(circAddr + PCAN_ACR3, c.B.b0);

        // Set the acceptance mask
        WRITE_PORT_UCHAR(circAddr + PCAN_AMR0, m.B.b3);
        WRITE_PORT_UCHAR(circAddr + PCAN_AMR1, m.B.b2);
        WRITE_PORT_UCHAR(circAddr + PCAN_AMR2, m.B.b1);
        WRITE_PORT_UCHAR(circAddr + PCAN_AMR3, m.B.b0);

        // Restore control register
        WRITE_PORT_UCHAR(circAddr + PCAN_MOD, tmp);
    } else {
        //
        // Standard CAN filter.
        //
        BYTE tmp;
        WL c, m;
        
        c.L = 0;
        m.L = 0;
        c.W.lsw = (WORD)(e->tagData.acc.code << 5);
        m.W.lsw = (WORD)(~(e->tagData.acc.mask << 5));
        m.W.lsw |= 0x000F;

        // Save control register
        tmp = READ_PORT_UCHAR(circAddr + PCAN_MOD);
        // Put the circuit in Reset Mode
        WRITE_PORT_UCHAR(circAddr + PCAN_MOD, (BYTE)(tmp|PCAN_RM));

        // Set the acceptance code
        WRITE_PORT_UCHAR(circAddr + PCAN_ACR0, c.B.b1);
        WRITE_PORT_UCHAR(circAddr + PCAN_ACR1, c.B.b0);
        WRITE_PORT_UCHAR(circAddr + PCAN_ACR2, 0xFF);
        WRITE_PORT_UCHAR(circAddr + PCAN_ACR3, 0xFF);

        // Set the acceptance mask
        WRITE_PORT_UCHAR(circAddr + PCAN_AMR0, m.B.b1);
        WRITE_PORT_UCHAR(circAddr + PCAN_AMR1, m.B.b0);
        WRITE_PORT_UCHAR(circAddr + PCAN_AMR2, 0xFF);
        WRITE_PORT_UCHAR(circAddr + PCAN_AMR3, 0xFF);

        // Restore control register
        WRITE_PORT_UCHAR(circAddr + PCAN_MOD, tmp);
    }
    
    return VSUCCESS;
}

static WORD statusToBusStatus(PPCCANcontext pCtx, int chan, WORD *txErr, WORD *rxErr)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE SR, CR;
    WORD stat = 0;
    
    SR = READ_PORT_UCHAR(circAddr + PCAN_SR);
    CR = READ_PORT_UCHAR(circAddr + PCAN_MOD);

    *txErr = READ_PORT_UCHAR(circAddr + PCAN_TXERR);
    *rxErr = READ_PORT_UCHAR(circAddr + PCAN_RXERR);

    switch (SR & (PCAN_BS|PCAN_ES)) {
        case PCAN_BS:
            stat = CHIPSTAT_BUSOFF;
            break;

        case PCAN_BS|PCAN_ES:
            stat = CHIPSTAT_BUSOFF;
            break;

        case PCAN_ES:
            stat = CHIPSTAT_ERROR_WARNING;
            if (*txErr > 127 || *rxErr > 127) {
                stat |= CHIPSTAT_ERROR_PASSIVE;
            }
            break;

        case 0:
            stat = CHIPSTAT_ERROR_ACTIVE;
            break;
    }
    if (CR & PCAN_RM) {
        stat = CHIPSTAT_BUSOFF; // qqq should be "inactive" or so.
    }

    return stat;
}


Vstatus PCC1000_GetChipState(PPCCANcontext pCtx, int chan, Vevent *e)
{
    // Obtain status & report.
    WORD rxErr, txErr;
    PVchannel pChan;
    Vevent ev;
    BYTE stat;

    ARGUSED(e);
    stat = (BYTE)statusToBusStatus(pCtx, chan, &rxErr, &txErr);

    pChan = pCtx->channel[chan];
    
    ev.tag = V_CHIP_STATE;
    ev.chanIndex = pChan->index;
    ev.transId = 0;
    ev.portHandle = (BYTE) INVALID_PORTHANDLE;
    ev.timeStamp = GetCurrentTime();
    ev.tagData.chipState.busStatus = stat;
    ev.tagData.chipState.txErrorCounter = (BYTE)rxErr;
    ev.tagData.chipState.rxErrorCounter = (BYTE)txErr;
    
    VDispatchReceiveEvent(pChan, &ev);
    
    return VSUCCESS;
}


//
// Write a message to the CAN controller, if it is available;
// otherwise let VCAND queue it.
//
Vstatus PCC1000_WriteMessage(PPCCANcontext pCtx, int chan, Vevent *e)
{
    if (e->tagData.msg.flags & MSGFLAG_ERROR_FRAME) {
        return VERR_WRONG_PARAMETER;
    }
    if (PCC1000_TXAvailable(pCtx, chan)) {
        PCC1000_StartWrite(pCtx, chan, e);
        return VSUCCESS;
    } else {
        return VERR_TX_NOT_POSSIBLE;
    }
}


/*
 ** Start a transmission. May be called at interupt level.
 */
Vstatus PCC1000_StartWrite(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress   p;
    PVchannel   pChan;
    IoAddress   circAddr = pCtx->C[chan].address;
    BYTE*       msg = e->tagData.msg.data;
    DWORD       ident = e->tagData.msg.id;
    BYTE        flags = e->tagData.msg.flags;
    BYTE        dlc = e->tagData.msg.dlc;
    int         i;

    pChan = pCtx->channel[chan];
    // Save a copy of the message.
    pCtx->C[chan].currentTxMessage = *e;
    pCtx->C[chan].currentTransId = MAKE_TRANSID(e->portHandle, e->transId);

    if (ident & EXT_MSG) {
        //
        // Extended CAN
        //
        WL id;
        BYTE x;

        id.L = ident & ~EXT_MSG;
        id.L <<= 3;
        
        x = (BYTE)(dlc | PCAN_FF_EXTENDED);
        if (flags & MSGFLAG_REMOTE_FRAME) x |= PCAN_FF_REMOTE;

        WRITE_PORT_UCHAR(circAddr + PCAN_MSGBUF, x);
        WRITE_PORT_UCHAR(circAddr + PCAN_XID0, id.B.b3);
        WRITE_PORT_UCHAR(circAddr + PCAN_XID1, id.B.b2);
        WRITE_PORT_UCHAR(circAddr + PCAN_XID2, id.B.b1);
        WRITE_PORT_UCHAR(circAddr + PCAN_XID3, id.B.b0);

        p = circAddr + PCAN_XDATA;
        for (i=0; i<dlc; i++) {
            WRITE_PORT_UCHAR(p++, *msg++);
        }
    } else {
        //
        // Standard CAN
        //
        BYTE x;
        x = dlc;
        if (flags & MSGFLAG_REMOTE_FRAME) x |= PCAN_FF_REMOTE;
        WRITE_PORT_UCHAR(circAddr + PCAN_MSGBUF, x);
        WRITE_PORT_UCHAR(circAddr + PCAN_SID0, (BYTE) (ident >> 3));
        WRITE_PORT_UCHAR(circAddr + PCAN_SID1, (BYTE) (ident << 5));

        p = circAddr + PCAN_SDATA;
        for (i=0; i<dlc; i++) {
            WRITE_PORT_UCHAR(p++, *msg++);
        }
    }

    if (flags & MSGFLAG_TXRQ) {
        Vevent *ev = &pCtx->C[chan].currentTxMessage;
        ev->tag = V_RECEIVE_MSG;
        ev->timeStamp = GetCurrentTime();
        ev->tagData.msg.flags |= MSGFLAG_TXRQ;
        ev->tagData.msg.flags &= ~MSGFLAG_TX;
        ev->portHandle = TRANSID_PORTHANDLE(pCtx->C[chan].currentTransId);
        ev->transId = TRANSID_TRANSID(pCtx->C[chan].currentTransId);
        WRITE_PORT_UCHAR(circAddr + PCAN_CMR, PCAN_TR);
        VDispatchReceiveEvent(pChan, ev);
        ev->tagData.msg.flags = flags;
    } else {
        WRITE_PORT_UCHAR(circAddr + PCAN_CMR, PCAN_TR);
    }

    if (pCtx->C[chan].ErrorCount > 0) {
        ResetErrorCounter(pCtx, chan);
    }
    
    return VSUCCESS;
}


int PCC1000_TXAvailable(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;

    return ((READ_PORT_UCHAR(circAddr + PCAN_SR) & PCAN_TBS) == PCAN_TBS);
}


void PCC1000_Interrupt(PPCCANcontext pCtx, int chan)
{
    BYTE ireg;
    int loopmax = 1000;
    IoAddress circAddr = pCtx->C[chan].address;

    ireg = READ_PORT_UCHAR(circAddr + PCAN_IR);
    
    while (ireg) {

        if (--loopmax == 0) {
            // Kill the card.
            PCCAN_EmergencyExit(pCtx, chan);
            return;
        }

        if (ireg & PCAN_RI) {
            PCC1000_Receive_ISR(pCtx, chan);
        }
        if (ireg & PCAN_TI) {
            PCC1000_Transmit_ISR(pCtx, chan);
        }
        if (ireg & PCAN_EI) {
            PCC1000_Error_ISR(pCtx, chan);
        }
        if (ireg & PCAN_DOI) {
            PCC1000_Overrun_ISR(pCtx, chan);
        }
        if (ireg & PCAN_WUI) {
#           ifdef DEBUG
            DEBUG1("Huh? Wakeup Interrupt!\n");
#           endif
        }
        if (ireg & PCAN_BEI) {
            PCC1000_BusError_ISR(pCtx, chan);
        }
        if (ireg & PCAN_EPI) {
            PCC1000_ErrorPassive_ISR(pCtx, chan);
        }
        ireg = READ_PORT_UCHAR(circAddr + PCAN_IR);
    }
}


static void PCC1000_Receive_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;

    PVchannel pChan;
    Vevent e;
    int i;
    BYTE dlc, flags;
    BYTE* p;
    IoAddress data;
    BYTE SR;
    WL id;

    SR = READ_PORT_UCHAR(circAddr + PCAN_SR);

    while (SR & PCAN_RBS) {
        BYTE tmp;
        
        e.timeStamp = GetCurrentTime();

        tmp = READ_PORT_UCHAR(circAddr + PCAN_MSGBUF);
        dlc = (BYTE)(tmp & 0x0F);
        if (dlc > 8) dlc = 8;
        flags = (BYTE)((tmp & PCAN_FF_REMOTE) ? MSGFLAG_REMOTE_FRAME : 0);
        id.L = 0;
        
        if (tmp & PCAN_FF_EXTENDED) {
            //
            // Extended CAN
            //
            id.B.b3 = READ_PORT_UCHAR(circAddr + PCAN_XID0);
            id.B.b2 = READ_PORT_UCHAR(circAddr + PCAN_XID1);
            id.B.b1 = READ_PORT_UCHAR(circAddr + PCAN_XID2);
            id.B.b0 = READ_PORT_UCHAR(circAddr + PCAN_XID3);
            id.L >>= 3;
            id.L |= EXT_MSG;
            data = circAddr + PCAN_XDATA;
        } else {
            //
            // Standard CAN
            //
            id.B.b1 = READ_PORT_UCHAR(circAddr + PCAN_SID0);
            id.B.b0 = READ_PORT_UCHAR(circAddr + PCAN_SID1);
            id.L >>= 5;
            data = circAddr + PCAN_SDATA;
        }
        
        p = e.tagData.msg.data;

        for (i=0; i<dlc; i++) {
            *p++ = READ_PORT_UCHAR(data++);
        }
        
        if (pCtx->C[chan].overrun) {
            flags |= pCtx->C[chan].overrun;
            pCtx->C[chan].overrun = 0;
        }

        pChan = pCtx->channel[chan];
        e.tag = V_RECEIVE_MSG;
        e.chanIndex = pChan->index;
        // Transid/porthandle black magic.
        e.transId = TRANSID_TRANSID(0);
        e.portHandle = TRANSID_PORTHANDLE(0);

        e.tagData.msg.id = id.L;
        e.tagData.msg.flags = flags;
        e.tagData.msg.dlc = dlc;

        if (VDispatchReceiveEvent(pChan, &e) != VSUCCESS) {
            pCtx->C[chan].overrun = MSGFLAG_OVERRUN;
        }
        // Release receive buffer
        WRITE_PORT_UCHAR(circAddr + PCAN_CMR, PCAN_RRB);
        
        SR = READ_PORT_UCHAR(circAddr + PCAN_SR);
    }

    if (pCtx->C[chan].ErrorCount > 0) {
        ResetErrorCounter(pCtx, chan);
    }
}


static void PCC1000_Transmit_ISR(PPCCANcontext pCtx, int chan)
{
    PVchannel pChan = pCtx->channel[chan];
    Vevent *current, e;

    // Send a tx ack.
    current = &pCtx->C[chan].currentTxMessage;
    if (current->tagData.msg.flags & MSGFLAG_TX) {
        Vevent *ev = &pCtx->C[chan].currentTxMessage;
        ev->tag = V_RECEIVE_MSG;
        ev->timeStamp = GetCurrentTime();
        ev->tagData.msg.flags &= ~MSGFLAG_TXRQ;
        ev->portHandle = TRANSID_PORTHANDLE(pCtx->C[chan].currentTransId);
        ev->transId = TRANSID_TRANSID(pCtx->C[chan].currentTransId);
        VDispatchReceiveEvent(pChan, current);
    }

    // We know that the tx buffer is free. Send the next message, if any.
    if (VReadData(pChan->pTxQueue, &e) == VSUCCESS) {
        PCC1000_StartWrite(pCtx, chan, &e);
    }
}


/*
 ** Handle error interrupts. Happens when the bus status or error status
 ** bits in the status register changes.
 */
static void PCC1000_Error_ISR(PPCCANcontext pCtx, int chan)
{
    PCC1000_GetChipState(pCtx, chan, NULL);
}


static void PCC1000_Overrun_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress addr = pCtx->C[chan].address;

    WRITE_PORT_UCHAR(addr + PCAN_CMR, PCAN_CDO);
    pCtx->C[chan].overrun = MSGFLAG_OVERRUN;
    PCC1000_Receive_ISR(pCtx, chan);
}

static void PCC1000_BusError_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    PVchannel pChan;
    Vevent e;
    BYTE ECC;

    ECC = READ_PORT_UCHAR(circAddr + PCAN_ECC);
    
    e.timeStamp = GetCurrentTime();

    pChan = pCtx->channel[chan];
    e.tag = V_RECEIVE_MSG;
    e.chanIndex = pChan->index;
    // Transid/porthandle black magic.
    e.transId = TRANSID_TRANSID(0);
    e.portHandle = TRANSID_PORTHANDLE(0);

    e.tagData.msg.id = 0x800 + ECC;
    e.tagData.msg.flags = MSGFLAG_ERROR_FRAME;
    e.tagData.msg.dlc = 0;

    if (VDispatchReceiveEvent(pChan, &e) != VSUCCESS) {
        pCtx->C[chan].overrun = MSGFLAG_OVERRUN;
    }

    //
    // Muffle the sja1000 if we get too many errors.
    //
    pCtx->C[chan].ErrorCount++;
    if (pCtx->C[chan].ErrorCount == MAX_ERROR_COUNT/2) {
        //
        // Half done, store current time
        //
        pCtx->C[chan].ErrorTime = GetCurrentTime();
    } else if (pCtx->C[chan].ErrorCount > MAX_ERROR_COUNT) {
        if ((GetCurrentTime() - pCtx->C[chan].ErrorTime) > ERROR_RATE) {
            //
            // Error rate reasonable, restart counters
            //
            pCtx->C[chan].ErrorCount = 0;
            pCtx->C[chan].ErrorTime = GetCurrentTime();
        } else {
            BYTE ier;
            ier = READ_PORT_UCHAR(circAddr + PCAN_IER);
            WRITE_PORT_UCHAR(circAddr + PCAN_IER, (BYTE)(ier & ~PCAN_BEIE));
        }
    }
    
}

static void PCC1000_ErrorPassive_ISR(PPCCANcontext pCtx, int chan)
{
    PCC1000_GetChipState(pCtx, chan, NULL);
}
