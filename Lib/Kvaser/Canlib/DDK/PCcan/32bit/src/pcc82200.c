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
#include <82c200.h>
#include "pcc_time.h"
#include "pcc82200.h"


// Forward declarations
void PCC200_Interrupt(PPCCANcontext pCtx, int chan);
static void PCC200_Receive_ISR(PPCCANcontext pCtx, int chan);
static void PCC200_Transmit_ISR(PPCCANcontext pCtx, int chan);
static void PCC200_Error_ISR(PPCCANcontext pCtx, int chan);
static void PCC200_Overrun_ISR(PPCCANcontext pCtx, int chan);

Vstatus PCC200_WriteMessage(PPCCANcontext pCtx, int chan, Vevent *e);
Vstatus PCC200_BusParams(PPCCANcontext pCtx, int chan, Vevent *e);       
Vstatus PCC200_SetHwFilter(PPCCANcontext pCtx, int chan, Vevent *e);     
Vstatus PCC200_BusOn(PPCCANcontext pCtx, int chan, Vevent *e);           
Vstatus PCC200_BusOff(PPCCANcontext pCtx, int chan, Vevent *e);          
Vstatus PCC200_GetChipState(PPCCANcontext pCtx, int chan, Vevent *e);    
Vstatus PCC200_BusOutputControl(PPCCANcontext pCtx, int chan, Vevent *e);

Vstatus PCC200_StartWrite(PPCCANcontext pCtx, int chan, Vevent *e);
int PCC200_TXAvailable(PPCCANcontext pCtx, int chan);



//
// Is this an 82C200?
// Is is assumed that the RESET signal is NOT asserted.
//
BOOL PCC200_ProbeChannel(PPCCANcontext pCtx, IoAddress addr, int chan)
{
    BYTE port;

    ARGUSED(pCtx);
    ARGUSED(chan);
    // First, reset the chip. Works for 82c200, sja1000 and 82527!
    // The "2" will not be set on the 82527.
    WRITE_PORT_UCHAR(addr + 0,  0x21);  // Reset
    WRITE_PORT_UCHAR(addr + 31, 0x00);  // BCAN mode for sja1000
    WRITE_PORT_UCHAR(addr + 0,  0x21);  // Reset again

    // Now let's see what we've got.
    port = READ_PORT_UCHAR(addr + 0);

    // If we don't read 0x21 back, then it isn't an 82c200 not an sja1000.
    if (port != 0x21) return FALSE;

    // Try Pelican mode to detect sja1000.
    WRITE_PORT_UCHAR(addr + 31, 0x80);  // Pelican mode for sja1000
    port = READ_PORT_UCHAR(addr + 31);

    if (port & 0x80) return FALSE;      // It's an sja1000
    return TRUE;
}


Vstatus PCC200_InitChannel(PPCCANcontext pCtx, int chan)
{
    struct _pccan_hwchannel *p;
    IoAddress addr;

    p = (struct _pccan_hwchannel *) &(pCtx->C[chan]);

    // Setup the channel data structure.
    p->WriteMessage     = PCC200_WriteMessage;
    p->BusParams        = PCC200_BusParams;       
    p->SetHwFilter      = PCC200_SetHwFilter;     
    p->BusOn            = PCC200_BusOn;           
    p->BusOff           = PCC200_BusOff;          
    p->GetChipState     = PCC200_GetChipState;    
    p->BusOutputControl = PCC200_BusOutputControl;

    addr = p->address;
    
    // Initialize the circuit.
    WRITE_PORT_UCHAR(addr + CCNTRL, RR);

    // Force BCAN mode in case this is an sja1000.
    WRITE_PORT_UCHAR(addr + 31, 0);
    
    // Activate almost all interrupt sources in the CAN circuit.
    WRITE_PORT_UCHAR(addr + CCNTRL, OIE|EIE|RIE|TIE|RR);

    // Accept all messages by default.
    WRITE_PORT_UCHAR(addr + CACC, 0);
    WRITE_PORT_UCHAR(addr + CACM, 0xff);

    // Default 125 kbit/s, pushpull.
    WRITE_PORT_UCHAR(addr + CBT0, 0x07);
    WRITE_PORT_UCHAR(addr + CBT1, 0x23);
    WRITE_PORT_UCHAR(addr + COCNTRL, 0xda);
    
    // Connect the INT line to the IRQ on the ISA bus.
    PCCAN_ConnectIRQ(pCtx, p->chipNo, TRUE);

    return VSUCCESS;
}


/*
 ** Kill a certain CAN circuit. Disconnect interrupt etc.
 ** Used at driver rundown.
 */
Vstatus PCC200_KillCircuit(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;

    // Disconnect the interupt line
    PCCAN_ConnectIRQ(pCtx, chan, FALSE);

    // Reset the 82C200. This will be read as 0x21 next time.
    WRITE_PORT_UCHAR(circAddr + CCNTRL, 0x21);

    return VSUCCESS;
}

Vstatus PCC200_KillChannel(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    tmp = READ_PORT_UCHAR(circAddr + CCNTRL);
    WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp | RR));

    return VSUCCESS;
}


/*
 ** Go on bus.
 */
Vstatus PCC200_BusOn(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    ARGUSED(e);
    pCtx->C[chan].overrun = 0;
    
    // Try to go on bus
    tmp = READ_PORT_UCHAR(circAddr + CCNTRL);
    WRITE_PORT_UCHAR(circAddr + CCOM, COS);
    WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp |  RR));
    WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp & ~RR));

    PCC200_GetChipState(pCtx, chan, NULL);

    return VSUCCESS;
}

/*
 ** Go off bus.
 */
#pragma argsused
Vstatus PCC200_BusOff(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    tmp = READ_PORT_UCHAR(circAddr + CCNTRL);
    WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp|RR));

    PCC200_GetChipState(pCtx, chan, NULL);

    return VSUCCESS;
}
#pragma argsused_off

/*
 ** Set the bus parameters.
 */
Vstatus PCC200_BusParams(PPCCANcontext pCtx, int chan, Vevent *e)
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
    sjw   = e->tagData.init.sjw - 1;  // Note "-1"
    tseg1 = e->tagData.init.tseg1;
    tseg2 = e->tagData.init.tseg2;
    sam   = e->tagData.init.sam;

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
    tmp = READ_PORT_UCHAR(circAddr + CCNTRL);
    resetStatus = tmp & RR;
    WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp|RR));

    WRITE_PORT_UCHAR(circAddr + CBT0, cbt0);
    WRITE_PORT_UCHAR(circAddr + CBT1, cbt1);
    tmp &= ~SPD;
    WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp|RR));

    if (resetStatus == FALSE) {
        tmp = READ_PORT_UCHAR(circAddr + CCNTRL);
        WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp & ~RR));
    }
    return VSUCCESS;
}


/*
 ** Set driver output type.
 */
Vstatus PCC200_BusOutputControl(PPCCANcontext pCtx, int chan, Vevent *e)
{
    short driver;
    BYTE tmp;
    IoAddress circAddr = pCtx->C[chan].address;

    switch (e->tagData.setOutputMode.mode) {
        case OUTPUT_MODE_NORMAL:
            driver = 0xda;  // Pushpull  (OCTP1|OCTN1|OCTP0|OCTN0|OCM1)
            break;
        case OUTPUT_MODE_SILENT:
            driver = 0x02;  // Tristate - silent mode (OCM1)
            break;
        /*case OUTPUT_MODE_SELFRECEPTION:
            driver = -1;
            break;
        case OUTPUT_MODE_OFF:
            driver = 0;
            break;*/
        default:
            driver = -1;
            break;
    }

    if (driver == -1) return VERR_WRONG_PARAMETER;

    // Save control register
    tmp = READ_PORT_UCHAR(circAddr + CCNTRL);
    // Put the circuit in Reset Mode
    WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp|RR));
    // Set the output control
    WRITE_PORT_UCHAR(circAddr + COCNTRL, (BYTE)(driver & 0xff));
    // Restore control register
    WRITE_PORT_UCHAR(circAddr + CCNTRL, tmp);

    return VSUCCESS;
}

//
// Construct hardware filters from a circuit-independent pair of mask/code.
// Mask: 1=do care, 0=don't care
// Code: the identifier mask
Vstatus PCC200_SetHwFilter(PPCCANcontext pCtx, int chan, Vevent *e)
{

    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp, c, m;

    if (e->tagData.acc.mask & EXT_MSG || e->tagData.acc.code & EXT_MSG) {
        return VERR_WRONG_PARAMETER;
    }

    // Calculate acceptance code and acceptance mask
    c = (BYTE)(((DWORD)(e->tagData.acc.code))>>3);
    m = (BYTE)(~((((DWORD)(e->tagData.acc.mask))>>3)));

    // Save control register
    tmp = READ_PORT_UCHAR(circAddr + CCNTRL);
    // Put the circuit in Reset Mode
    WRITE_PORT_UCHAR(circAddr + CCNTRL, (BYTE)(tmp|RR));

    // Set the acceptance code
    WRITE_PORT_UCHAR(circAddr + CACC, c);

    // Set the acceptance mask
    WRITE_PORT_UCHAR(circAddr + CACM, m);

    // Restore control register
    WRITE_PORT_UCHAR(circAddr + CCNTRL, tmp);
    return VSUCCESS;
}

static WORD statusToBusStatus(BYTE SR, BYTE CR, WORD *txErr, WORD *rxErr)
{
    WORD stat = 0;

    switch (SR & (BS|ES)) {
        case BS:
            stat = CHIPSTAT_BUSOFF;
            *txErr = 255;
            *rxErr = 255;
            break;

        case BS|ES:
            stat = CHIPSTAT_BUSOFF;
            *txErr = 255;
            *rxErr = 255;
            break;

        case ES:
            // "Error passive" is a lie, but what else should I do?
            stat = CHIPSTAT_ERROR_WARNING|CHIPSTAT_ERROR_PASSIVE;
            *txErr = 96;
            *rxErr = 96;
            break;

        case 0:
            stat = CHIPSTAT_ERROR_ACTIVE;
            *txErr = 0;
            *rxErr = 0;
            break;
    }
    if (CR & RR) {
        stat = CHIPSTAT_BUSOFF; // qqq should be "inactive" or so.
        *txErr = 0;
        *rxErr = 0;
    }
    return stat;
}


#ifdef __BORLANDC__
#   pragma argsused
#endif
Vstatus PCC200_GetChipState(PPCCANcontext pCtx, int chan, Vevent *e)
{
    // Obtain status & report.
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE SR, CR;
    WORD rxErr, txErr;
    PVchannel pChan;
    Vevent ev;
    BYTE stat;
    
    SR = READ_PORT_UCHAR(circAddr + CSTAT);
    CR = READ_PORT_UCHAR(circAddr + CCNTRL);

    stat = (BYTE)statusToBusStatus(SR, CR, &rxErr, &txErr);

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
Vstatus PCC200_WriteMessage(PPCCANcontext pCtx, int chan, Vevent *e)
{
    if ((e->tagData.msg.flags & MSGFLAG_ERROR_FRAME) ||
        (e->tagData.msg.id & EXT_MSG)) {
        return VERR_WRONG_PARAMETER;
    }
    if (PCC200_TXAvailable(pCtx, chan)) {
        PCC200_StartWrite(pCtx, chan, e);
        return VSUCCESS;
    } else {
        return VERR_TX_NOT_POSSIBLE;
    }
}


/*
 ** Start a transmission. May be called at interupt level.
 */
Vstatus PCC200_StartWrite(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress p;
    PVchannel pChan;
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE* msg = e->tagData.msg.data;
    WORD ident = (WORD) e->tagData.msg.id;
    BYTE flags = (BYTE) e->tagData.msg.flags;
    BYTE dlc = (BYTE)  e->tagData.msg.dlc;
    int i;

    pChan = pCtx->channel[chan];
    // Save a copy of the message.
    pCtx->C[chan].currentTxMessage = *e;
    pCtx->C[chan].currentTransId = MAKE_TRANSID(e->portHandle, e->transId);

    WRITE_PORT_UCHAR(circAddr + TBI, (BYTE)(ident>>3));
    WRITE_PORT_UCHAR(circAddr + TRTDL,
                     (BYTE)(((ident & 0x7)<<5)
                            | (flags & MSGFLAG_REMOTE_FRAME ? 0x10 : 0)
                            | dlc));
    p = circAddr + TDS0;
    for (i=0; i<dlc; i++) {
        WRITE_PORT_UCHAR(p++, *msg++);
    }

    if (flags & MSGFLAG_TXRQ) {
        Vevent *ev = &pCtx->C[chan].currentTxMessage;
        ev->tag = V_RECEIVE_MSG;
        ev->timeStamp = GetCurrentTime();
        ev->tagData.msg.flags |= MSGFLAG_TXRQ;
        ev->tagData.msg.flags &= ~MSGFLAG_TX;
        ev->portHandle = TRANSID_PORTHANDLE(pCtx->C[chan].currentTransId);
        ev->transId = TRANSID_TRANSID(pCtx->C[chan].currentTransId);
        WRITE_PORT_UCHAR(circAddr + CCOM, TR);
        VDispatchReceiveEvent(pChan, ev);
        ev->tagData.msg.flags = flags;
    } else {
        WRITE_PORT_UCHAR(circAddr + CCOM, TR);
    }
    return VSUCCESS;
}


int PCC200_TXAvailable(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;

    return ((READ_PORT_UCHAR(circAddr + CSTAT) & TBA) == TBA);
}



void PCC200_Interrupt(PPCCANcontext pCtx, int chan)
{
    BYTE ireg;
    int loopmax = 1000;
    IoAddress circAddr = pCtx->C[chan].address;

    ireg = READ_PORT_UCHAR(circAddr + CINT);
    
    while (ireg & 0x1f) {

        if (--loopmax == 0) {
            // Kill the card.
            PCCAN_EmergencyExit(pCtx, chan);
            return;
        }
        
        if (ireg & RIF) {
            PCC200_Receive_ISR(pCtx, chan);
        }
        if (ireg & TIF) {
            PCC200_Transmit_ISR(pCtx, chan);
        }
        if (ireg & EIF) {
            PCC200_Error_ISR(pCtx, chan);
        }
        if (ireg & OIF) {
            PCC200_Overrun_ISR(pCtx, chan);
        }
        if (ireg & WIF) {
#           if defined(DEBUG)
            DEBUG1("Huh? Wakeup Interrupt!\n");
#           endif
        }
        ireg = READ_PORT_UCHAR(circAddr + CINT);
    }
}


static void PCC200_Receive_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;

    PVchannel pChan;
    Vevent e;
    int i;
    BYTE rrtdl, dlc, flags;
    WORD id;
    BYTE* p;
    IoAddress data;
    BYTE SR;

    SR = READ_PORT_UCHAR(circAddr + CSTAT);

    while (SR & RBS) {

        e.timeStamp = GetCurrentTime();

        rrtdl = READ_PORT_UCHAR(circAddr + RRTDL);
        id    = (READ_PORT_UCHAR(circAddr + RBI)<<3) + (rrtdl>>5);
        dlc   = rrtdl & 0x0F;
        if (dlc > 8) dlc = 8;
        flags = (rrtdl & 0x10) ? MSGFLAG_REMOTE_FRAME : 0;
        data  = circAddr + RDS0;
        p     = e.tagData.msg.data;

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

        e.tagData.msg.id = id;
        e.tagData.msg.flags = flags;
        e.tagData.msg.dlc = dlc;

        if (VDispatchReceiveEvent(pChan, &e) != VSUCCESS) {
            pCtx->C[chan].overrun = MSGFLAG_OVERRUN;
        }
        // Release receive buffer
        WRITE_PORT_UCHAR(circAddr + CCOM, RRB);
        
        SR = READ_PORT_UCHAR(circAddr + CSTAT);
    }
}


static void PCC200_Transmit_ISR(PPCCANcontext pCtx, int chan)
{
    PVchannel pChan = pCtx->channel[chan];
    Vevent *current, e;

    // Send a tx ack.
    current = &pCtx->C[chan].currentTxMessage;
    if  (current->tagData.msg.flags & MSGFLAG_TX) {
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
        PCC200_StartWrite(pCtx, chan, &e);
    }
}


/*
 ** Handle error interrupts. In this way, the 82C200 will inform us of changes
 ** in the error or bus status.
 */
static void PCC200_Error_ISR(PPCCANcontext pCtx, int chan)
{
    PCC200_GetChipState(pCtx, chan, NULL);
}


static void PCC200_Overrun_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress addr = pCtx->C[chan].address;

    WRITE_PORT_UCHAR(addr + CCOM, COS);
    pCtx->C[chan].overrun = MSGFLAG_OVERRUN;
    PCC200_Receive_ISR(pCtx, chan);
}
