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
#include "hermes.h"

#include "herm_hw.h"
#include "pcc_time.h"
#include "herm1000.h"
#include "amcc5920.h"

// Macros so we can use char* and byte offsets
#define WritePortUlong(port, value) WRITE_PORT_ULONG((ULONG*)(port), (value))
#define ReadPortUlong(port) READ_PORT_ULONG((ULONG*)(port))



//
// HERMESInterruptService()
// ISR for the connected interrupt for NT.
//
#ifdef __WIN_NT__
BOOLEAN HERMESInterruptService(IN PKINTERRUPT interrupt, IN OUT PVOID context)
{
    return HERMES_Interrupt((PHERMEScontext)context);
}
#endif


//
// HERMESInterruptService()
// ISR for the connected interrupt for W95.
//
#ifdef __WIN_95__
int HERMESInterruptService(PHERMEScontext pCtx)
{
    return HERMES_Interrupt(pCtx);
}
#endif


#ifdef __BORLANDC__
#pragma argsused
#endif
Vstatus HERMESTransmitEvent(PVchannel pChan, PVevent pEvent, unsigned long tout)
{
    Vstatus         vErr;
    PHERMEScontext   pCtx = (PHERMEScontext)pChan->pHwContext;
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
            vErr = HERMES_InitChannel(pCtx, chan);
            break;

        case V_EXIT_HARDWARE:
            vErr = HERMES_KillChannel(pCtx, chan);
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


// qqq Registers in the Xilinx; to be moved to a separate header file.
#define XILINX_REVINT   7

//
// Detect the card, but do not initialize it.
//
Vstatus HERMES_DetectCard(PHERMEScontext pCtx)
{
    int chan, i;
    IoAddress addr;
    int xilinxRev;

#   if defined(DEBUG)
    DEBUG1("[HERMES_DetectCard]\n");
#   endif

    // Set (one) Wait State needed(?) by the CAN chip on the ADDON bus of S5920
    // WritePortUlong(pCtx->s5920BaseAddress + S5920_PTCR, 0x81818181L );

    // Assert PTADR# - we're in passive mode so the other bits are not important
    WritePortUlong(pCtx->s5920BaseAddress + S5920_PTCR, 0x80808080L );
    
    xilinxRev = READ_PORT_UCHAR(pCtx->xilinxAddress + XILINX_REVINT) >> 4;
    xilinxRev = 16 - xilinxRev;
    DIAGNOSTIC(HERMES_NAME ": FPGA Rev %d\n", xilinxRev);
    
#if defined(DEBUG)
    {
        int i, j;
        IoAddress x = pCtx->baseAddress;
        for (i=0; i<64; i++) {
            DEBUG1("%04x  ", x + i*16);
            for (j=0; j<16; j++) {
                DEBUG1("%02x ", READ_PORT_UCHAR(x + i*16 + j));
            }
            DEBUG1("\n");
        }
    }
#endif

#if 0
    { // qqq
        int i, j;
        char s[100], t[100];
        IoAddress x = pCtx->s5920BaseAddress;
        for (i=0; i<4; i++) {
            sprintf(s, "%04x  ", x + i*16);
            for (j=0; j<16; j++) {
                sprintf(t, "%02x", READ_PORT_UCHAR(x + i*16 + j));
                strcat(s, t);
            }
            DIAGNOSTIC(s);
        }
    }
#endif
    
    // qqq remove this one when we release the board..
    DIAGNOSTIC(HERMES_NAME ": using %d bytes per circuit.\n", HERMES_BYTES_PER_CIRCUIT);
    
    chan = 0;
    for (i=0; i<HERMES_CHANNEL_COUNT; i++) {

        // Each controller has HERMES_BYTES_PER_CIRCUIT bytes.
        // This is "hardcoded" on the PCB and in the Xilinx.
        addr = pCtx->baseAddress + (WORD)(i * HERMES_BYTES_PER_CIRCUIT);

        if (HERMES_ProbeChannel(pCtx, addr, i)) {

            pCtx->C[chan].circuitType = CIRCTYPE_SJA1000;
            pCtx->C[chan].address = addr;
            pCtx->C[chan].chipNo = i;
            DIAGNOSTIC(HERMES_NAME "/%d (chip %d) is an SJA1000.\n", chan, i);

        } else {

            pCtx->C[chan].circuitType = 0;
            pCtx->C[chan].address = 0;
            pCtx->C[chan].chipNo = i;
            DIAGNOSTIC(HERMES_NAME ": chip %d is not mounted.\n", i);
            continue;

        }
        chan++;
    }

#if defined(DEBUG)
    {
        int i, j;
        IoAddress x = pCtx->baseAddress;
        for (i=0; i<64; i++) {
            DEBUG1("%04x  ", x + i*16);
            for (j=0; j<16; j++) {
                DEBUG1("%02x ", READ_PORT_UCHAR(x + i*16 + j));
            }
            DEBUG1("\n");
        }
    }
#endif

    
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
Vstatus HERMES_InitCard(PHERMEScontext pCtx)
{
    IoAddress addr;
    unsigned int i;
    DWORD tmp;

#   if defined(DEBUG)
    DEBUG1("[HERMES_InitCard]\n");
#   endif

    // The card must be present!
    if (!pCtx->isPresent) return VERROR;

    addr = pCtx->baseAddress;

    ResetTime();

    // enable interrupts from card
    tmp = ReadPortUlong(pCtx->s5920BaseAddress + S5920_INTCSR);
    tmp |= INTCSR_ADDON_INTENABLE_M;
    WritePortUlong(pCtx->s5920BaseAddress + S5920_INTCSR, tmp);

    
    for (i=0; i<pCtx->channelCount; i++) {
        PVchannel p;
        p = pCtx->channel[i];

        // Some administrative work...
        p->firmwareVersion      = 0;
        p->hardwareVersion      = 0;
        p->transceiverSernum[0] = 0;
        p->transceiverSernum[1] = 0;
        p->licenseMask1         = 0;
        p->licenseMask2         = 0;

        // Default values.
        p->transceiverCapabilities = 0;
        p->circuitCapabilities = 0;

        // This is not implemented yet, qqq.
        p->cardSernum = 0;

        p->transceiverCapabilities = TRANSCEIVER_CAP_HIGHSPEED;

        p->circuitCapabilities = CHANNEL_CAP_EXTENDED_CAN |
                                 CHANNEL_CAP_ERROR_COUNTERS |
                                 CHANNEL_CAP_CAN_DIAGNOSTICS |
                                 CHANNEL_CAP_TXREQUEST |
                                 CHANNEL_CAP_TXACKNOWLEDGE;

    }

    return VSUCCESS;
}


/*
 ** Kill the HERMES card entirely.
 */
Vstatus HERMES_KillCard(PHERMEScontext pCtx)
{
    IoAddress addr = pCtx->baseAddress;
    ULONG tmp;

#   if defined(DEBUG)
    DEBUG1("[HERMES_KillCard]\n");
#   endif

    // Disable interrupts from card
    tmp = ReadPortUlong(pCtx->s5920BaseAddress + S5920_INTCSR);
    tmp &= ~INTCSR_ADDON_INTENABLE_M;
    WritePortUlong(pCtx->s5920BaseAddress + S5920_INTCSR, tmp);

    return VSUCCESS;
}


void HERMES_EmergencyExit(PHERMEScontext pCtx, int chan)
{
    DIAGNOSTIC(HERMES_NAME ": Channel %02x runaway.\n", chan);
    DIAGNOSTIC(HERMES_NAME ": I give up.\n");
    
    HERMES_KillCard(pCtx);
    return;
}

/*
 ** This routine is called from the (real) interrupt routine.
 */
int HERMES_Interrupt(PHERMEScontext pCtx)
{
    unsigned int chan;
    ULONG        tmp;
    unsigned int loopmax = 1000; // qqq somewhat arbitrary
    int handled;

    handled = FALSE;
    tmp = ReadPortUlong(pCtx->s5920BaseAddress + S5920_INTCSR);

    while (tmp & INTCSR_INTERRUPT_ASSERTED_M) {
        if (--loopmax == 0) {
            // Kill the card.
            HERMES_EmergencyExit(pCtx, tmp);
            return handled;
        }
        for (chan=0; chan < pCtx->channelCount; chan++) {
            handled |= HERMES_ChannelInterrupt(pCtx, chan);
        }
        tmp = ReadPortUlong(pCtx->s5920BaseAddress + S5920_INTCSR);
    }
    return handled;
}

#ifdef __WIN_95__

unsigned long inportl(unsigned short port)
{
    __asm {
        mov dx, port
        db 0edh          // in dx, eax
    }
}

void outportl(unsigned short port, unsigned long value)
{
    __asm {
        mov dx, port
        mov eax, value
        db 0efh          // out dx, eax
    }
}

#endif
