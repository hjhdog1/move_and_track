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
#include <i82527.h>
#include "pcc_time.h"
#include "pcc82527.h"

// Forward declarations
void        PCC527_Interrupt(PPCCANcontext pCtx, int chan);
static void PCC527_Receive_StdData_ISR(PPCCANcontext pCtx, int chan);
static void PCC527_Receive_ExtData_ISR(PPCCANcontext pCtx, int chan);
static void PCC527_Receive_StdRtr_ISR(PPCCANcontext pCtx, int chan);
static void PCC527_Receive_ExtRtr_ISR(PPCCANcontext pCtx, int chan);
static void PCC527_Transmit_ISR(PPCCANcontext pCtx, int chan);
static void PCC527_Status_ISR(PPCCANcontext pCtx, int chan);

Vstatus     PCC527_WriteMessage(PPCCANcontext pCtx, int chan, Vevent *e);
Vstatus     PCC527_BusParams(PPCCANcontext pCtx, int chan, Vevent *e);       
Vstatus     PCC527_SetHwFilter(PPCCANcontext pCtx, int chan, Vevent *e);     
Vstatus     PCC527_BusOn(PPCCANcontext pCtx, int chan, Vevent *e);           
Vstatus     PCC527_BusOff(PPCCANcontext pCtx, int chan, Vevent *e);          
Vstatus     PCC527_GetChipState(PPCCANcontext pCtx, int chan, Vevent *e);    
Vstatus     PCC527_BusOutputControl(PPCCANcontext pCtx, int chan, Vevent *e);

Vstatus     PCC527_StartWrite(PPCCANcontext pCtx, int chan, Vevent *e);
int         PCC527_TXAvailable(PPCCANcontext pCtx, int chan);


#include <pshpack1.h>
typedef union {
    DWORD L;
    struct { WORD lsw, msw; } W;
    struct { BYTE b0, b1, b2, b3; } B;
} WL;
#include <poppack.h>

#define I527_TX_BUF              1
#define I527_RX_EXT_RTR_BUF      2
#define I527_RX_STD_RTR_BUF      3


//
// Is this an 82527?
// Is is assumed that the RESET signal is NOT asserted.
//
BOOL PCC527_ProbeChannel(PPCCANcontext pCtx, IoAddress addr, int chan)
{
    BYTE port;

    ARGUSED(pCtx);
    ARGUSED(chan);
    // First, reset the chip. Works for 82c200, sja1000 and 82527!
    // The "2" will not be set on the 82527.
    WRITE_PORT_UCHAR(addr + 0, 0x21);

    // Now let's see what we've got.
    port = READ_PORT_UCHAR(addr + 0);

    // A true-blue 82527 will read 0x01.
    return (port == 0x01);
}

/*
 ** Initialize the specified circuit. Enable its interrupts.
 **
 ** We will use buffer 1 to send from. We will flip the Xtd bit in buffer
 ** 1 as required and the buffer will be held in the invalid state unless
 ** a transmission is pending.
 **
 ** We are using buffers 2 for receiving standard remote frames and
 ** buffer 3 for extended remote frames. Buffer 14 and 15 are used
 ** for standard and extended data frames or vice versa, depending
 ** on whether we should optimize for standard or extended CAN!
 ** All global masks are initially set to zeros ("don't care") but may
 ** be changed by the user.
 ** 
 ** This approach has a few drawbacks.
 ** 1) We will see RTRs but not their identifiers nor their length.
 ** 2) #1 _when_enabled_ will automatically respond to any RTR.
 **    It is only enabled during transmissions however.
 ** 
 */
Vstatus PCC527_InitChannel(PPCCANcontext pCtx, int chan)
{
    int i;
    int buf;
    int tmp;
    char s[64];
    IoAddress addr;
    int chipNo;
    static unsigned int Verbose = 0xFFFFFFFF;

    struct _pccan_hwchannel *p;

    p = (struct _pccan_hwchannel *) &(pCtx->C[chan]);

    // Setup the channel data structure.
    p->WriteMessage     = PCC527_WriteMessage;
    p->BusParams        = PCC527_BusParams;       
    p->SetHwFilter      = PCC527_SetHwFilter;     
    p->BusOn            = PCC527_BusOn;           
    p->BusOff           = PCC527_BusOff;          
    p->GetChipState     = PCC527_GetChipState;    
    p->BusOutputControl = PCC527_BusOutputControl;

    chipNo = p->chipNo;
    addr = p->address;
    
    // Check that hardware reset has been deasserted.
    if (READ_PORT_UCHAR(addr + CAN_CPUIR) & CPUIR_RSTST) {
        return VERR_HW_NOT_READY;
    }

    tmp = 0;
    sprintf(s, "PCcanOptimizeForExtended_%d", chipNo);
    if ((VGetOption(s, &tmp) == FALSE) || (tmp == 0)) {
        if (Verbose & (1<<chipNo)) {
            DIAGNOSTIC("PCcan: optimizing chip %d for standard CAN.\n", chipNo);
        }
        pCtx->C[chan].i527_RxStdBuf = 15;
        pCtx->C[chan].i527_RxExtBuf = 14;
        pCtx->C[chan].i527_RxStdBufIReg = 2;
        pCtx->C[chan].i527_RxExtBufIReg = 16;
    } else {
        if (Verbose & (1<<chipNo)) {
            DIAGNOSTIC("PCcan: optimizing chip %d for extended CAN.\n", chipNo);
        }
        pCtx->C[chan].i527_RxStdBuf = 14;
        pCtx->C[chan].i527_RxExtBuf = 15;
        pCtx->C[chan].i527_RxStdBufIReg = 16;
        pCtx->C[chan].i527_RxExtBufIReg = 2;
    }

    tmp = 0;
    sprintf(s, "PCcanEnableBusDiagnostics_%d", chipNo);
    if ((VGetOption(s, &tmp) == FALSE) || (tmp == 0)) {
        pCtx->C[chan].useSIE = FALSE;
    } else {
        pCtx->C[chan].useSIE = TRUE;
    }
    if (Verbose & (1<<chipNo)) {
        DIAGNOSTIC("PCcan: chip %d %s bus diagnostics.\n",
               chipNo,
               pCtx->C[chan].useSIE ? "enabling" : "disabling");
    }

    Verbose &= (1 << chipNo);
    
    
    //Set DSC (makes the system clock SCLK = XTAL/2), and
    //    DMC (makes the memory clock MCLK = SCLK/2)
    // This gives SCLK = 8MHz and MCLK = 4MHz
    WRITE_PORT_UCHAR(addr + CAN_CPUIR, CPUIR_DSC|CPUIR_DMC);

    // Initialize the 82527
    WRITE_PORT_UCHAR(addr + CAN_CTRL, CTRL_INIT|CTRL_CCE);

    // Set all global masks to "don't care".
    WRITE_PORT_UCHAR(addr + CAN_STDGBLMASK0, 0x00);
    WRITE_PORT_UCHAR(addr + CAN_STDGBLMASK1, 0x00);
    WRITE_PORT_UCHAR(addr + CAN_EXTGBLMASK0, 0x00);
    WRITE_PORT_UCHAR(addr + CAN_EXTGBLMASK1, 0x00);
    WRITE_PORT_UCHAR(addr + CAN_EXTGBLMASK2, 0x00);
    WRITE_PORT_UCHAR(addr + CAN_EXTGBLMASK3, 0x00);

    // Setup the buffer #15 masks to "don't care".
    WRITE_PORT_UCHAR(addr + CAN_MSG15MASK0, 0);
    WRITE_PORT_UCHAR(addr + CAN_MSG15MASK1, 0);
    WRITE_PORT_UCHAR(addr + CAN_MSG15MASK2, 0);
    WRITE_PORT_UCHAR(addr + CAN_MSG15MASK3, 0);

    // Set all message buffers to Invalid and Interrupts disable.
    for (i = 1; i <= 15; i++) {
        IoAddress port = CANmsg(addr, i) + CANmsg_CTRL0;
        WRITE_PORT_UCHAR(port, CLR(MsgVal) & CLR(TxIE) & CLR(RxIE));
        port = CANmsg(addr, i) + CANmsg_CTRL1;
        WRITE_PORT_UCHAR(port, CLR(NewDat));
    }
    
    // receive standard RTR frames
    buf = I527_RX_STD_RTR_BUF;
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CONF, CANmsg_Transmit | CANmsg_Standard);
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL0, SET(MsgVal) & SET(RxIE));
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL1, CLR(TxRqst) & SET(CPUUpd));

    // receive extended RTR frames
    buf = I527_RX_EXT_RTR_BUF;
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CONF, CANmsg_Transmit | CANmsg_Extended);
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL0, SET(MsgVal) & SET(RxIE));
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL1, CLR(TxRqst) & SET(CPUUpd));

    // receive standard data frames
    buf = pCtx->C[chan].i527_RxStdBuf;
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CONF, CANmsg_Receive | CANmsg_Standard);
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL0, SET(MsgVal) & SET(RxIE));
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL1, CLR(TxRqst) & CLR(CPUUpd));

    // receive extended data frames
    buf = pCtx->C[chan].i527_RxExtBuf;
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CONF, CANmsg_Receive | CANmsg_Extended);
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL0, SET(MsgVal) & SET(RxIE));
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL1, CLR(TxRqst) & CLR(CPUUpd));

    // send messages
    buf = I527_TX_BUF;
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CONF, CANmsg_Transmit);
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL0, CLR(MsgVal) & SET(TxIE));
    WRITE_PORT_UCHAR(CANmsg(addr, buf) + CANmsg_CTRL1, SET(CPUUpd) & CLR(TxRqst));

    // Default 125 kbit/s, pushpull.
    WRITE_PORT_UCHAR(addr + CAN_BTR0, 0x07);
    WRITE_PORT_UCHAR(addr + CAN_BTR1, 0x23);
    WRITE_PORT_UCHAR(addr + CAN_BUSCON, BUSCON_COBY|BUSCON_DCT1|BUSCON_DCR1);
    
    // Connect the INT line to the IRQ on the ISA bus.
    PCCAN_ConnectIRQ(pCtx, p->chipNo, TRUE);

    // Enable interrupts.
    WRITE_PORT_UCHAR(addr + CAN_CTRL,
                     (BYTE)((pCtx->C[chan].useSIE ? CTRL_SIE : 0) |
                     CTRL_INIT|CTRL_CCE|CTRL_EIE|CTRL_IE));

    return TRUE;
}




/*
 ** Kill a certain CAN circuit.Disconnect interrupt etc.
 ** Used at driver rundown.
 */
Vstatus PCC527_KillCircuit(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;

    // Disconnect the interupt line
    PCCAN_ConnectIRQ(pCtx, chan, FALSE);

    // Reset the 82527.
    // The interrupt enable bits are cleared.
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, CTRL_INIT);

    return VSUCCESS;
}

Vstatus PCC527_KillChannel(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    tmp = READ_PORT_UCHAR(circAddr + CAN_CTRL);
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, (BYTE)(tmp | (CTRL_INIT | CTRL_CCE)));
    return VSUCCESS;
}

/*
 ** Go on bus.
 */
#ifdef __BORLANDC__
#   pragma argsused
#endif
Vstatus PCC527_BusOn(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    pCtx->C[chan].overrun = 0;
    pCtx->C[chan].ErrorCount = 0;

    pCtx->C[chan].prevSR = 0;
    pCtx->C[chan].prevCR = 0;
    
    WRITE_PORT_UCHAR(CANmsg(circAddr, I527_RX_STD_RTR_BUF) + CANmsg_CTRL1, CLR(NewDat));
    WRITE_PORT_UCHAR(CANmsg(circAddr, I527_RX_EXT_RTR_BUF) + CANmsg_CTRL1, CLR(NewDat));
    WRITE_PORT_UCHAR(CANmsg(circAddr, pCtx->C[chan].i527_RxStdBuf) + CANmsg_CTRL1, CLR(NewDat));
    WRITE_PORT_UCHAR(CANmsg(circAddr, pCtx->C[chan].i527_RxExtBuf) + CANmsg_CTRL1, CLR(NewDat));

    //  Go on-bus: clear INIT and CCE
    tmp = READ_PORT_UCHAR(circAddr + CAN_CTRL);
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, (BYTE)(tmp |  (CTRL_INIT | CTRL_CCE)));
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, (BYTE)(tmp & ~(CTRL_INIT | CTRL_CCE)));

    PCC527_GetChipState(pCtx, chan, NULL);

    return VSUCCESS;
}

/*
 ** Go off bus.
 */
#ifdef __BORLANDC__
#   pragma argsused
#endif
Vstatus PCC527_BusOff(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    //  Go off-bus: set INIT and CCE
    tmp = READ_PORT_UCHAR(circAddr + CAN_CTRL);
    tmp |= (CTRL_INIT | CTRL_CCE);
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, tmp);

    PCC527_GetChipState(pCtx, chan, NULL);
    
    return VSUCCESS;
}

/*
 ** Set the bus parameters.
 */
Vstatus PCC527_BusParams(PPCCANcontext pCtx, int chan, Vevent *e)
{
    unsigned int quantaPerCycle;
    DWORD brp;
    BYTE btr0;
    BYTE btr1;
    BYTE tmp;
    DWORD freq;
    BYTE tseg1, tseg2, sjw, sam;
    IoAddress circAddr = pCtx->C[chan].address;

    freq  = e->tagData.init.bitRate;
    sjw   = e->tagData.init.sjw - 1;  // Note "-1"
    tseg1 = e->tagData.init.tseg1;
    tseg2 = e->tagData.init.tseg2;
    sam   = e->tagData.init.sam;
    
    // SCLK is 8MHz
    quantaPerCycle = tseg1 + tseg2 + 1;
    if (quantaPerCycle == 0 || freq == 0) return VERR_WRONG_PARAMETER;
    
    brp = (8000000L * 64) / (freq * quantaPerCycle);
    if ((brp & 0x3F) != 0) {
        // Fraction != 0 : not divisible.
        return VERR_WRONG_PARAMETER;
    }
    brp = (brp >> 6) - 1;
    if (brp > 64 || sjw > 3 || quantaPerCycle < 8
        || tseg1<3 || tseg1 > 16 || tseg2 < 2 || tseg2 > 8) {
        return VERR_WRONG_PARAMETER;
    }

    btr0 = (BYTE)(((BYTE)sjw <<6) + brp);
    btr1 = (BYTE)(((sam==3?1:0)<<7) + ((tseg2-1)<<4) + (tseg1-1));

    // Go off-bus.
    tmp = READ_PORT_UCHAR(circAddr + CAN_CTRL);
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, (BYTE)(tmp|CTRL_INIT | CTRL_CCE));

    WRITE_PORT_UCHAR(circAddr + CAN_BTR0, btr0);
    WRITE_PORT_UCHAR(circAddr + CAN_BTR1, btr1);

    // Syncmode is ignored. You shouldn't synchronize on both edges
    // anyway.

    // Restore control register
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, tmp);

    return VSUCCESS;
}


/*
 ** Set driver output type.
 */
Vstatus PCC527_BusOutputControl(PPCCANcontext pCtx, int chan, Vevent *e)
{
    short driver;
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;

    switch (e->tagData.setOutputMode.mode) {
        case OUTPUT_MODE_NORMAL:
            // Pushpull
            driver = BUSCON_COBY|BUSCON_DCT1|BUSCON_DCR1;
            break;

        case OUTPUT_MODE_SILENT:
//        case OUTPUT_MODE_SELFRECEPTION:
//        case OUTPUT_MODE_OFF:
            driver = -1;
            break;
            
        default:
            driver = -1;
            break;
    }

    if (driver == -1) return VERR_WRONG_PARAMETER;

    // Save control register
    tmp = READ_PORT_UCHAR(circAddr + CAN_CTRL);
    // Put the circuit in Reset Mode
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, (BYTE)(tmp|CTRL_INIT | CTRL_CCE));
    // Set the output control
    WRITE_PORT_UCHAR(circAddr + CAN_BUSCON, (BYTE)(driver & 0xff));
    // Restore control register
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, tmp);

    return VSUCCESS;
}

//
// Construct hardware filters from a circuit-independent pair of mask/code.
// Mask: 1=do care, 0=don't care
// Code: the identifier mask
// stdFilter = TRUE if this is a filter for std identifiers.
Vstatus PCC527_SetHwFilter(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE tmp;
    WL c;
    WL m;
    int buf;

    c.L = 0L;
    m.L = 0L;

    // Save control register
    tmp = READ_PORT_UCHAR(circAddr + CAN_CTRL);
    // Put the circuit in Reset Mode
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, (BYTE)(tmp|CTRL_INIT|CTRL_CCE));

    if (((e->tagData.acc.mask & EXT_MSG) == 0)
        && (e->tagData.acc.code & EXT_MSG) == 0) {
        // Set the acceptance code, standard CAN
        c.W.lsw = (WORD)(e->tagData.acc.code);
        m.W.lsw = (WORD)(e->tagData.acc.mask);
        
        c.W.lsw <<= 5;
        buf = pCtx->C[chan].i527_RxStdBuf;
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 0, c.B.b1);
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 1, c.B.b0);

        buf = I527_RX_STD_RTR_BUF;
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 0, c.B.b1);
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 1, c.B.b0);

        // Set the acceptance mask, standard CAN
        m.W.lsw <<= 5;
        WRITE_PORT_UCHAR(circAddr + CAN_STDGBLMASK0, m.B.b1);
        WRITE_PORT_UCHAR(circAddr + CAN_STDGBLMASK1, m.B.b0);
    } else {
        c.L = e->tagData.acc.code;
        m.L = e->tagData.acc.mask;
        // Set the acceptance code, extended CAN
        c.L <<= 3;

        buf = pCtx->C[chan].i527_RxExtBuf;
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 0, c.B.b3);
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 1, c.B.b2);
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 2, c.B.b1);
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 3, c.B.b0);

        buf = I527_RX_EXT_RTR_BUF;
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 0, c.B.b3);
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 1, c.B.b2);
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 2, c.B.b1);
        WRITE_PORT_UCHAR(CANmsg(circAddr, buf) + CANmsg_ARB + 3, c.B.b0);

        // Set the acceptance mask, extended CAN
        m.L <<= 3;
        WRITE_PORT_UCHAR(circAddr + CAN_EXTGBLMASK0, m.B.b3);
        WRITE_PORT_UCHAR(circAddr + CAN_EXTGBLMASK1, m.B.b2);
        WRITE_PORT_UCHAR(circAddr + CAN_EXTGBLMASK2, m.B.b1);
        WRITE_PORT_UCHAR(circAddr + CAN_EXTGBLMASK3, m.B.b0);
    }

    // Restore control register
    WRITE_PORT_UCHAR(circAddr + CAN_CTRL, tmp);
    return VSUCCESS;
}

static WORD statusToBusStatus(BYTE SR, BYTE CR, WORD *txErr, WORD *rxErr)
{
    WORD stat = 0;

    switch (SR & (STAT_BOFF|STAT_WARN)) {
        case STAT_BOFF:
        case STAT_BOFF|STAT_WARN:
            stat = CHIPSTAT_BUSOFF;
            *txErr = 255;
            *rxErr = 255;
            break;

        case STAT_WARN:
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
    if (CR & CTRL_INIT) {
        stat = CHIPSTAT_BUSOFF; // qqq should be "inactive" or so.
        *txErr = 0;
        *rxErr = 0;
    }
    return stat;
}



/*
 ** Retrieve the current controller status.
 */
#ifdef __BORLANDC__
#   pragma argsused
#endif
Vstatus PCC527_GetChipState(PPCCANcontext pCtx, int chan, Vevent *e)
{
    // Obtain status & report.
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE SR, CR;
    WORD rxErr, txErr;
    PVchannel pChan;
    Vevent ev;
    BYTE stat;

    SR = READ_PORT_UCHAR(circAddr + CAN_STAT);
    CR = READ_PORT_UCHAR(circAddr + CAN_CTRL);

    pCtx->C[chan].prevSR = SR;
    pCtx->C[chan].prevSR = CR;
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
Vstatus PCC527_WriteMessage(PPCCANcontext pCtx, int chan, Vevent *e)
{
    if (PCC527_TXAvailable(pCtx, chan)) {
        PCC527_StartWrite(pCtx, chan, e);
        return VSUCCESS;
    } else {
        return VERR_TX_NOT_POSSIBLE;
    }
}


/*
 ** Start a transmission. May be called at interupt level.
 */
Vstatus PCC527_StartWrite(PPCCANcontext pCtx, int chan, Vevent *e)
{
    IoAddress p;
    PVchannel pChan;
    int i;
    IoAddress circAddr   = pCtx->C[chan].address;
    BYTE* msg       = e->tagData.msg.data;
    DWORD ident     = e->tagData.msg.id;
    BYTE flags      = (BYTE) e->tagData.msg.flags;
    BYTE dlc        = (BYTE) e->tagData.msg.dlc;
    BYTE tmp;

    pChan = pCtx->channel[chan];
    // Save a copy of the message.
    pCtx->C[chan].currentTxMessage = *e;
    pCtx->C[chan].currentTransId = MAKE_TRANSID(e->portHandle, e->transId);

    WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CTRL0,
                     CLR(MsgVal));
    WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CTRL1,
                     CLR(RmtPnd) & CLR(TxRqst) &  SET(CPUUpd));

    // Message configuration.
    tmp = (BYTE)CANmsg_DLC(dlc);
    if (flags & MSGFLAG_REMOTE_FRAME) {
        tmp |= CANmsg_Receive;
    } else {
        tmp |= CANmsg_Transmit;
    }
    
    if (ident & EXT_MSG) {
        tmp |= CANmsg_Extended;
    } else {
        tmp |= CANmsg_Standard;
    }
    
    WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CONF, tmp);

    // The identifier.
    if ((ident & EXT_MSG) == 0) {
        WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_ARB + 0, (BYTE)(ident >> 3));
        WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_ARB + 1, (BYTE)((ident&7)<<5));
    } else if (tmp & CANmsg_Extended) {
        WL x;
        x.L = ((DWORD)(ident & ~EXT_MSG) << 3);
        WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_ARB + 0, x.B.b3);
        WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_ARB + 1, x.B.b2);
        WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_ARB + 2, x.B.b1);
        WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_ARB + 3, x.B.b0);
    }

    p = CANmsg(circAddr, I527_TX_BUF) + CANmsg_DATA;

    for (i=0; i<dlc; i++) {
        WRITE_PORT_UCHAR(p++, *msg++);
    }

    // Request transmission; the message is not marked as valid yet
    WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CTRL1,
                     SET(NewDat) & SET(TxRqst) & CLR(CPUUpd));
    
    if (flags & MSGFLAG_TXRQ) {
        Vevent *ev = &pCtx->C[chan].currentTxMessage;
        ev->tag = V_RECEIVE_MSG;
        ev->timeStamp = GetCurrentTime();
        ev->tagData.msg.flags |= MSGFLAG_TXRQ;
        ev->tagData.msg.flags &= ~MSGFLAG_TX;
        ev->portHandle = TRANSID_PORTHANDLE(pCtx->C[chan].currentTransId);
        ev->transId = TRANSID_TRANSID(pCtx->C[chan].currentTransId);
        // Mark the message as valid.
        WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CTRL0,
                         SET(MsgVal));
        VDispatchReceiveEvent(pChan, ev);
        ev->tagData.msg.flags = flags;
    } else {
        // Mark the message as valid.
        WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CTRL0,
                         SET(MsgVal));
    }
    
    return VSUCCESS;
}


int PCC527_TXAvailable(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;

    if (READ_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CTRL1) & CTRL1_TxRqst)
        return FALSE;
    else
        return TRUE;
}


/*
 ** Handle an interrupt from an 82527.
 */
void PCC527_Interrupt(PPCCANcontext pCtx, int chan)
{
    BYTE ireg;
    WORD loopmax = 1000;
    IoAddress circAddr = pCtx->C[chan].address;
    
    ireg = READ_PORT_UCHAR(circAddr + CAN_IR);
    
    while (ireg) {

        if (--loopmax == 0) {
            // Kill the card.
            PCCAN_EmergencyExit(pCtx, chan);
            return;
        }

        // DIAGNOSTIC("IREG=%x\n", ireg);
        
        if (ireg == pCtx->C[chan].i527_RxExtBufIReg) {
            PCC527_Receive_ExtData_ISR(pCtx, chan);
        } else if (ireg == pCtx->C[chan].i527_RxStdBufIReg) {
            PCC527_Receive_StdData_ISR(pCtx, chan);
        } else if (ireg == I527_TX_BUF + 2) {
            PCC527_Transmit_ISR(pCtx, chan);
        } else if (ireg == I527_RX_EXT_RTR_BUF + 2) {
            PCC527_Receive_ExtRtr_ISR(pCtx, chan);
        } else if (ireg == I527_RX_STD_RTR_BUF + 2) {
            PCC527_Receive_StdRtr_ISR(pCtx, chan);
        } else if (ireg == 1) {
            // Status Register has changed.
            PCC527_Status_ISR(pCtx, chan);
        }
        ireg = READ_PORT_UCHAR(circAddr + CAN_IR);
    }
}


// A standard data frame.
static void PCC527_Receive_StdData_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    PVchannel pChan;
    Vevent e;
    int i;
    BYTE dlc, flags, tmp;
    WL id;
    BYTE* p;
    IoAddress data;
    int bufNo;
    IoAddress rxBuf;

    bufNo = pCtx->C[chan].i527_RxStdBuf;

    rxBuf = CANmsg(circAddr, bufNo);
    tmp = READ_PORT_UCHAR(rxBuf + CANmsg_CONF);

    e.timeStamp = GetCurrentTime();

    id.L = 0;
    id.B.b1 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 0);
    id.B.b0 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 1);
    id.W.lsw >>= 5;

    dlc   = tmp >> 4;
    if (dlc > 8) dlc = 8;
    flags = 0;
    data  = rxBuf + CANmsg_DATA;
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

    //
    // Check for hardware overflow.
    //
    tmp = READ_PORT_UCHAR(rxBuf + CANmsg_CTRL1);
    if (tmp & TEST(MsgLst)) {
        flags |= MSGFLAG_OVERRUN;
    }
    // Valid data?
    if (tmp & TEST(NewDat)) {
    
        e.tagData.msg.id = id.L;
        e.tagData.msg.flags = flags;
        e.tagData.msg.dlc = dlc;

        if (VDispatchReceiveEvent(pChan, &e) != VSUCCESS) {
            // Software overflow.
            pCtx->C[chan].overrun = MSGFLAG_OVERRUN;
        }

        // Re-enable SIE.
        if (pCtx->C[chan].ErrorCount > MAX_ERROR_COUNT && pCtx->C[chan].useSIE ) {
            BYTE cr;
            cr = READ_PORT_UCHAR(circAddr + CAN_CTRL);
            cr |= CTRL_SIE;
            WRITE_PORT_UCHAR(circAddr + CAN_CTRL, cr);
            pCtx->C[chan].ErrorCount = 0;        
        }
    }
    
    WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL0, CLR(IntPnd));
    WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL1, CLR(MsgLst) & CLR(NewDat) & CLR(RmtPnd));

}

// An extended data frame.
static void PCC527_Receive_ExtData_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    PVchannel pChan;
    Vevent e;
    int i;
    BYTE dlc, flags, tmp;
    WL id;
    BYTE* p;
    IoAddress data;
    int bufNo;
    IoAddress rxBuf;

    bufNo = pCtx->C[chan].i527_RxExtBuf;

    rxBuf = CANmsg(circAddr, bufNo);
    tmp = READ_PORT_UCHAR(rxBuf + CANmsg_CONF);

    e.timeStamp = GetCurrentTime();

    id.L = 0;
    id.B.b3 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 0);
    id.B.b2 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 1);
    id.B.b1 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 2);
    id.B.b0 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 3);
    id.L >>= 3;
    id.L |= EXT_MSG;

    dlc   = tmp >> 4;
    if (dlc > 8) dlc = 8;
    flags = 0;
    data  = rxBuf + CANmsg_DATA;
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

    //
    // Check for hardware overflow.
    //
    tmp = READ_PORT_UCHAR(rxBuf + CANmsg_CTRL1);
    if (tmp & TEST(MsgLst)) {
        flags |= MSGFLAG_OVERRUN;
    }

    // Valid data?
    if (tmp & TEST(NewDat)) {
    
        e.tagData.msg.id = id.L;
        e.tagData.msg.flags = flags;
        e.tagData.msg.dlc = dlc;

        if (VDispatchReceiveEvent(pChan, &e) != VSUCCESS) {
            pCtx->C[chan].overrun = MSGFLAG_OVERRUN;
        }

        // Re-enable SIE.
        if (pCtx->C[chan].ErrorCount > MAX_ERROR_COUNT && pCtx->C[chan].useSIE ) {
            BYTE cr;
            cr = READ_PORT_UCHAR(circAddr + CAN_CTRL);
            cr |= CTRL_SIE;
            WRITE_PORT_UCHAR(circAddr + CAN_CTRL, cr);
            pCtx->C[chan].ErrorCount = 0;        
        }
    }
    
    WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL0, CLR(IntPnd));
    WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL1, CLR(MsgLst) & CLR(NewDat) & CLR(RmtPnd));
}


// A standard remote frame.
static void PCC527_Receive_StdRtr_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    PVchannel pChan;
    Vevent e;
    BYTE dlc, flags, tmp;
    WL id;
    IoAddress rxBuf;

    rxBuf = CANmsg(circAddr, I527_RX_STD_RTR_BUF);

    e.timeStamp = GetCurrentTime();

    //
    // A remote request.
    // The 527 writes neither the dlc nor the id in the buffer.
    //
    // Fake identifier...
    id.L = 0xFFFFFFFF & ~EXT_MSG;

    // ... and DLC.
    dlc = 0;
    flags = MSGFLAG_REMOTE_FRAME;

    if (pCtx->C[chan].overrun) {
        flags |= pCtx->C[chan].overrun;
        pCtx->C[chan].overrun = 0;
    }
    
    //
    // We can't check for hardware overflow because the CpuUpd/MsgLst
    // bit is always set...!
    //
    tmp = READ_PORT_UCHAR(rxBuf + CANmsg_CTRL1);

    // The RmtPnd bit isn't set either so we can't check for valid data..
    { // if (tmp & TEST(RmtPnd)) {

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

        // Re-enable SIE.
        if (pCtx->C[chan].ErrorCount > MAX_ERROR_COUNT && pCtx->C[chan].useSIE ) {
            BYTE cr;
            cr = READ_PORT_UCHAR(circAddr + CAN_CTRL);
            cr |= CTRL_SIE;
            WRITE_PORT_UCHAR(circAddr + CAN_CTRL, cr);
            pCtx->C[chan].ErrorCount = 0;        
        }
    }
    
    // Release receive buffer.
    // CPUUpd/MsgLst is always set ('cause we've set it up that way)
    // so we can't check for overflow.
    WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL0, CLR(IntPnd));
    WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL1, CLR(NewDat) & CLR(RmtPnd));
}

// An extended remote frame.
static void PCC527_Receive_ExtRtr_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    PVchannel pChan;
    Vevent e;
    BYTE dlc, flags, tmp;
    WL id;
    IoAddress rxBuf;

    rxBuf = CANmsg(circAddr, I527_RX_EXT_RTR_BUF);

    e.timeStamp = GetCurrentTime();

    //
    // A remote request.
    // The 527 writes neither the dlc nor the id in the buffer.
    //
    // Fake identifier...
    id.L = 0xFFFFFFFF | EXT_MSG;

    // ... and DLC.
    dlc = 0;
    flags = MSGFLAG_REMOTE_FRAME;

    if (pCtx->C[chan].overrun) {
        flags |= pCtx->C[chan].overrun;
        pCtx->C[chan].overrun = 0;
    }

    //
    // We can't check for hardware overflow because the CpuUpd/MsgLst
    // bit is always set...!
    //
    tmp = READ_PORT_UCHAR(rxBuf + CANmsg_CTRL1);

    // The RmtPnd bit isn't set either so we can't check for valid data..
    { // if (tmp & TEST(RmtPnd)) {

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

        // Re-enable SIE.
        if (pCtx->C[chan].ErrorCount > MAX_ERROR_COUNT && pCtx->C[chan].useSIE ) {
            BYTE cr;
            cr = READ_PORT_UCHAR(circAddr + CAN_CTRL);
            cr |= CTRL_SIE;
            WRITE_PORT_UCHAR(circAddr + CAN_CTRL, cr);
            pCtx->C[chan].ErrorCount = 0;        
        }
    }
    // Release receive buffer.
    // CPUUpd/MsgLst is always set ('cause we've set it up that way)
    // so we can't check for overflow.
    WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL0, CLR(IntPnd));
    WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL1, CLR(NewDat) & CLR(RmtPnd));
}


static void PCC527_Transmit_ISR(PPCCANcontext pCtx, int chan)
{
    PVchannel pChan = pCtx->channel[chan];
    Vevent *current, e;
    IoAddress circAddr = pCtx->C[chan].address;

    // Kill our TX buffer ASAP, to avoid spurious answers
    // to remote requests.
    WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CTRL0,
                     CLR(MsgVal) & CLR(IntPnd));

    // Manually reset the txRqst bit. Transmission may hang otherwise
    // for some unknown reason (perhaps the 82527 isn't fast enough
    // when updating this bit??)
    WRITE_PORT_UCHAR(CANmsg(circAddr, I527_TX_BUF) + CANmsg_CTRL1, CLR(TxRqst));

    // Re-enable SIE.
    if (pCtx->C[chan].ErrorCount > MAX_ERROR_COUNT && pCtx->C[chan].useSIE ) {
        BYTE cr;
        cr = READ_PORT_UCHAR(circAddr + CAN_CTRL);
        cr |= CTRL_SIE;
        WRITE_PORT_UCHAR(circAddr + CAN_CTRL, cr);
        pCtx->C[chan].ErrorCount = 0;        
    }

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
        PCC527_StartWrite(pCtx, chan, &e);
    }
}

//
// Handle changes in the status register. This may be both
// error interrupts and status changes.
//
static void PCC527_Status_ISR(PPCCANcontext pCtx, int chan)
{
    IoAddress circAddr = pCtx->C[chan].address;
    BYTE SR, CR;
    BYTE oldSR, oldCR;

    oldSR = pCtx->C[chan].prevSR;
    oldCR = pCtx->C[chan].prevCR;
    SR = READ_PORT_UCHAR(circAddr + CAN_STAT);
    CR = READ_PORT_UCHAR(circAddr + CAN_CTRL);
    pCtx->C[chan].prevSR = SR;
    pCtx->C[chan].prevSR = CR;

    //
    // Error frames? (I.e. is LEC != 0?)
    //
    if ((SR & 0x07) != 0) {

        PVchannel pChan;
        Vevent e;
        BYTE flags;

        //
        // If we get too many errors, calm the 527 down a bit.
        //
        pCtx->C[chan].ErrorCount++;
        if (pCtx->C[chan].ErrorCount > MAX_ERROR_COUNT) {
            CR = READ_PORT_UCHAR(circAddr + CAN_CTRL);
            CR &= ~CTRL_SIE;
            WRITE_PORT_UCHAR(circAddr + CAN_CTRL, CR);
            // SIE will be re-enabled when
            // we have recieved or transmitted a msg.
        }
        
        e.timeStamp = GetCurrentTime();

        flags = MSGFLAG_ERROR_FRAME;

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

        e.tagData.msg.id = 0x900 + (SR & 0x07);
        e.tagData.msg.flags = flags;
        e.tagData.msg.dlc = 0;

        if (VDispatchReceiveEvent(pChan, &e) != VSUCCESS) {
            pCtx->C[chan].overrun = MSGFLAG_OVERRUN;
        }

        // Reset LEC.
        SR &= 0xF8;
        WRITE_PORT_UCHAR(circAddr + CAN_STAT, SR);
    }
    
    //
    // Report a status change.
    // Should be done only if status HAS changed.
    //
    if (((SR ^ oldSR) & (0xC0)) || ((CR ^ oldCR) & (0x01))) {
        PCC527_GetChipState(pCtx, chan, NULL);
    }
}

