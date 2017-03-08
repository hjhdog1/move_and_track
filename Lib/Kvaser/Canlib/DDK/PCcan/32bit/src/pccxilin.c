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

#include "pccxilin.h"
#include <pccan.h>
#include "pcc_time.h"
#include "pcc82200.h"
#include "pcc82527.h"
#include "pcc1000.h"

/*
 ** A routine for connecting or disconnecting interrupt lines.
 ** h        A pointer to the appropriate cardData struct.
 ** circ     The channel assigned by LocateHardware.
 **          0=Intel 1, 1=Intel 2, 2=Philips 1, 3=Philips 2
 ** connect  TRUE if the interrupt shall be activated, FALSE otherwise
 */
void PCCAN_ConnectIRQ(PPCCANcontext pCtx, int chan, unsigned connect)
{
    BYTE val, x;
    IoAddress addr = pCtx->baseAddress + XILINX_OFFSET;
    unsigned int shift;

    if (connect) {
#       ifdef DEBUG
        DEBUG1("Connecting chan %d to irq %d\n", chan, pCtx->IRQ);
#       endif
        switch(pCtx->IRQ) {
            case 2:
            case 9:
                val = 1;
                break;
            case 3:
                val = 2;
                break;
            case 5:
                val = 3;
                break;
            default:
                val = 0; // Disconnect.
        }
    } else {
#       ifdef DEBUG
        DEBUG1("Disconnecting chan %d from xilinx\n", chan);
#       endif
        val = 0;
    }
    switch (chan) {
        case 0: shift = 0; break;
        case 1: shift = 2; break;
        case 2: shift = 4; break;
        case 3: shift = 6; break;
        default:
            shift = 8; // chan has an illegal value, set shift to something safe
            ;
    }

    x = READ_PORT_UCHAR((addr + XREG_IRQ));
    x &= ~(3U<<shift);
    x |= (val<<shift);
    WRITE_PORT_UCHAR(addr + XREG_IRQ, x);
#   ifdef DEBUG
    DEBUG1("Wrt %x to %x\n", x, addr + XREG_IRQ);
#   endif
}

//
// Detect the card, but do not initialize it.
//
Vstatus PCCAN_DetectCard(PPCCANcontext pCtx)
{
    int chan, i;
    BYTE port;
    IoAddress addr;

#   if defined(DEBUG)
    DEBUG1("[PCCAN_DetectCard]\n");
#   endif

    // Assert reset.
    addr = pCtx->baseAddress + XILINX_OFFSET;
    port = READ_PORT_UCHAR(addr + XREG_CTRL);
    port &= ~XREG_CTRL_NORESET;
    WRITE_PORT_UCHAR(addr + XREG_CTRL, port);

    // Sanity check I.
    port = READ_PORT_UCHAR(addr + XREG_INTERRUPT);
    if ((port & 0x0F) != 0x0F) return VERR_HW_NOT_PRESENT;

    // Sanity check II.
    WRITE_PORT_UCHAR(addr + XREG_INTERRUPT, 0);
    port = READ_PORT_UCHAR(addr + XREG_INTERRUPT);
    if ((port & 0xF0) != 0x00) return VERR_HW_NOT_PRESENT;
    
    // Use the on-board clock and remove reset.
    WRITE_PORT_UCHAR(addr + XREG_CLOCK, XREG_CLOCK_ONBOARD);
    WRITE_PORT_UCHAR(addr + XREG_CTRL, XREG_CTRL_NORESET);
    
    chan = 0;
    for (i=0; i<PCCAN_CHANNEL_COUNT; i++) {
        
        addr = pCtx->baseAddress + INTEL_OFFSET + (WORD)(i * 0x1000);

        if (PCC527_ProbeChannel(pCtx, addr, i)) {
           
            pCtx->C[chan].circuitType = CIRCTYPE_82527;
            pCtx->C[chan].address = addr;
            pCtx->C[chan].chipNo = i;
            DIAGNOSTIC("PCcan/%d (chip %d) is an 82527.\n", chan, i);
            
        } else if (PCC1000_ProbeChannel(pCtx, addr, i)) {

            pCtx->C[chan].circuitType = CIRCTYPE_SJA1000;
            pCtx->C[chan].address = addr;
            pCtx->C[chan].chipNo = i;
            DIAGNOSTIC("PCcan/%d (chip %d) is an SJA1000.\n", chan, i);

        } else if (PCC200_ProbeChannel(pCtx, addr, i)) {
            
            pCtx->C[chan].circuitType = CIRCTYPE_82C200;
            pCtx->C[chan].address = addr;
            pCtx->C[chan].chipNo = i;
            DIAGNOSTIC("PCcan/%d (chip %d) is an 82C200.\n", chan, i);
            
        } else {
            
            pCtx->C[chan].circuitType = 0;
            pCtx->C[chan].address = 0;
            pCtx->C[chan].chipNo = i;
            DIAGNOSTIC("PCcan: chip %d is not mounted.\n", i);
            continue;
            
        }
        chan++;
    }

    pCtx->channelCount = chan;

    if (chan > 0) {
        pCtx->isPresent = TRUE;
        return VSUCCESS;
    } else {
        pCtx->isPresent = FALSE;
        return VERR_HW_NOT_PRESENT;
    }
}

//
// Initialize a (detected) card.
// 
Vstatus PCCAN_InitCard(PPCCANcontext pCtx)
{
    IoAddress addr;
    unsigned int i;

#   if defined(DEBUG)
    DEBUG1("[PCCAN_InitCard]\n");
#   endif

    // The card must be present!
    if (!pCtx->isPresent) return VERROR;

    addr = pCtx->baseAddress;

    // Assert reset, use the on-board clock, remove reset.
    WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_CTRL, 0);
    WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_CLOCK, XREG_CLOCK_ONBOARD);
    WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_CTRL, XREG_CTRL_NORESET);

    ResetTime();

    for (i=0; i<pCtx->channelCount; i++) {
        PVchannel p;
        p = pCtx->channel[i];

        // Some administrative work...
        p->firmwareVersion = 0;
        p->hardwareVersion = 0;
        p->transceiverSernum[0] = 0;
        p->transceiverSernum[1] = 0;
        p->licenseMask1 = 0;
        p->licenseMask2 = 0;

        // Default values.
        p->transceiverCapabilities = 0;
        p->circuitCapabilities = 0;

        // This is not implemented yet, qqq.
        p->cardSernum = 0;

        switch(pCtx->C[i].circuitType) {
            case CIRCTYPE_82527:
                p->transceiverCapabilities = TRANSCEIVER_CAP_HIGHSPEED;
                p->circuitCapabilities = CHANNEL_CAP_EXTENDED_CAN |
                                         CHANNEL_CAP_TXREQUEST |
                                         CHANNEL_CAP_TXACKNOWLEDGE;
                break;

            case CIRCTYPE_82C200:
                p->transceiverCapabilities = TRANSCEIVER_CAP_HIGHSPEED;
                p->circuitCapabilities = CHANNEL_CAP_TXREQUEST |
                                         CHANNEL_CAP_TXACKNOWLEDGE;
                break;

            case CIRCTYPE_SJA1000:
                p->transceiverCapabilities = TRANSCEIVER_CAP_HIGHSPEED;
                p->circuitCapabilities = CHANNEL_CAP_EXTENDED_CAN |
                                         CHANNEL_CAP_ERROR_COUNTERS |
                                         CHANNEL_CAP_CAN_DIAGNOSTICS |
                                         CHANNEL_CAP_TXREQUEST |
                                         CHANNEL_CAP_TXACKNOWLEDGE;
                break;

        }
    }
    
    return VSUCCESS;
}

Vstatus PCCAN_InitChannel(PPCCANcontext pCtx, int chanNo)
{
#   if defined(DEBUG)
    DEBUG1("[PCCAN_InitChannel] chan=%u\n", chanNo);
#   endif

    // The card must be present!
    if (!pCtx->isPresent) return VERROR;
    if (pCtx->C[chanNo].isInit) return VSUCCESS;

    switch(pCtx->C[chanNo].circuitType) {
        case CIRCTYPE_82527:
            PCC527_InitChannel(pCtx, chanNo);
            break;
            
        case CIRCTYPE_82C200:
            PCC200_InitChannel(pCtx, chanNo);
            break;
            
        case CIRCTYPE_SJA1000:
            PCC1000_InitChannel(pCtx, chanNo);
            break;
    }

    pCtx->C[chanNo].isInit = TRUE;
    return VSUCCESS;
}


/*
 ** Kill the PCcan card entirely.
 */
Vstatus PCCAN_KillCard(PPCCANcontext pCtx)
{
    IoAddress addr = pCtx->baseAddress;

#   if defined(DEBUG)
    DEBUG1("[PCCAN_KillCard]\n");
#   endif

    // Disconnect all IRQ's.
    WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_IRQ, 0);
    // Reset the card.
    WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_CTRL, 0);
    WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_CTRL, XREG_CTRL_NORESET);
    return VSUCCESS;
}


Vstatus PCCAN_KillChannel(PPCCANcontext pCtx, int chan)
{
#   if defined(DEBUG)
    DEBUG1("[PCCAN_KillChannel] chan=%u\n", chan);
#   endif

    // The card must be present!
    if (!pCtx->isPresent) return VERROR;
    if (!pCtx->C[chan].isInit) return VSUCCESS;

    switch(pCtx->C[chan].circuitType) {
        case CIRCTYPE_82527:
            PCC527_KillChannel(pCtx, chan);
            break;
        case CIRCTYPE_82C200:
            PCC200_KillChannel(pCtx, chan);
            break;
        case CIRCTYPE_SJA1000:
            PCC1000_KillChannel(pCtx, chan);
            break;
    }
    pCtx->C[chan].isInit = FALSE;

    return VSUCCESS;
}


void PCCAN_EmergencyExit(PPCCANcontext pCtx, int chan)
{
    DIAGNOSTIC("PCCAN: Channel %02x runaway.\n", chan);
    DIAGNOSTIC("PCCAN: I give up.\n");
    PCCAN_KillCard(pCtx);
    return;
}

/*
 ** This routine is called from the (real) interrupt routine in irq.c.
 */
void PCCAN_Interrupt(PPCCANcontext pCtx)
{
    unsigned int chan;
    BYTE         xInt;
    IoAddress    XIntReg;
    unsigned int loopmax = 1000;

    XIntReg = pCtx->baseAddress + XILINX_OFFSET + XREG_INTERRUPT;

    xInt = READ_PORT_UCHAR(XIntReg) & 0x0F;
    
    while (xInt != 0x0F) {
        if (--loopmax == 0) {
            // Kill the card.
            PCCAN_EmergencyExit(pCtx, -1);
            return;
        }
        for (chan=0; chan < pCtx->channelCount; chan++) {

            if ((xInt & (1 << (pCtx->C[chan].chipNo))) == 0) {

                switch (pCtx->C[chan].circuitType) {

                    case CIRCTYPE_82C200:
                        PCC200_Interrupt(pCtx, chan);
                        break;

                    case CIRCTYPE_SJA1000:
                        PCC1000_Interrupt(pCtx, chan);
                        break;

                    case CIRCTYPE_82527:
                        PCC527_Interrupt(pCtx, chan);
                        break;

                    default:
                        ; // qqq
                
                }
            }
        }
        xInt = READ_PORT_UCHAR(XIntReg) & 0x0F;
    }
}

