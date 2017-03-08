/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This module contains the code specific to the PCCAN card.
** We support the 82C200s (there are two of them on a PCCAN card) and
** 82527 (also two of them), but the 82526 is not supported.
*/
#include <stdlib.h>
#include <dos.h>
#include <memory.h>
#include <string.h>

#include <std.h>
#include <canlib.h>
#include "pccan.h"
#include "82c200.h"
#include "i82527.h"
// #include <util.h>
#include "timer.h"
#include "global.h"
#include "libpccan.h"
#include "misc.h"

// PRIVATE circData* XilinxIntToCircuit[4];

// Forward declarations
PRIVATE void PCCAN_82C200_Interrupt(circData *h, WORD address, BYTE ireg);
PRIVATE void PCCAN_82C200_Receive(circData *h, WORD address);
PRIVATE void PCCAN_82C200_Transmit(circData *h, WORD address);
PRIVATE void PCCAN_82C200_Error(circData *h, WORD address);
//PRIVATE void PCCAN_82C200_Overrun(circData *h, WORD address);
//PRIVATE void PCCAN_82C200_Wakeup(circData *h, WORD address);
PRIVATE void PCCAN_INTEL527_Interrupt(circData *h, WORD address, BYTE ireg);
PRIVATE void PCCAN_INTEL527_Receive(circData* h, WORD address, int bufNo);
PRIVATE void PCCAN_INTEL527_Transmit(circData* h, WORD address);
PRIVATE void PCCAN_INTEL527_Error(circData *h, WORD address);

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
#define I527_RX_EXT_BUF          4
#define I527_RX_STD_BUF          5
#define I527_FIRST_RX_BUF        2
#define I527_LAST_RX_BUF         5

// Accept this many ACK errors; the rest are ignored.
#define MAX_ACK_ERR_COUNT        16

/*
** Initialize the data structures. Called from LocateHardware.
*/
PUBLIC canStatus PCCANlibInit(void)
{
   int i;
   static int Done = FALSE;

   if (!Done) {
//      for (i=0; i<COUNTOF(XilinxIntToCircuit); i++) {
//         XilinxIntToCircuit[i] = NULL;
//      }
      Done = TRUE;
   }
   return canOK;
}

/*
** This one is called just before the library unloads.
*/
PUBLIC canStatus PCCANlibExit(void)
{
   return canOK;
}

/*
** Find the available PCCAN cards.
** For the moment these routines only support one card
** but it's quite simple to extend the routines
**
** This routine is called when the library is initialized and leaves the
** result of its findings in the global variables baseAddress and xilinxIntReg.
*/
/*-------------------------------------------------------------------

OBSOLETE but useful in an installation program.

PRIVATE int findCards(void)
{
   int    i = 0;
   BYTE   x;

   //
   // Check the xilinx vector to find the PCCAN card
   //
   //
   // Fast detta funkar junte om det finns interruptisar väntande!
   //
   while (Addresses[i] != 0) {
      xilinxIntReg = Addresses[i] + XILINX_OFFSET;
      x = READ_PORT_UCHAR(xilinxIntReg);  // Xilinx interrupt register.
      // 0x0f means no interrupts.
      // This construction also implies that under DOS or
      // when these routines are _statically_ linked, only _one_
      // application can access the PCCAN card(s).
      if (x == 0x0f) {
         baseAddress = Addresses[i];
         break;
      }
      if ((x&0x0F) == 0x00) {
         // The interrupt register is moved in some versions of PCCAN.
         xilinxIntReg += 2;
         x = READ_PORT_UCHAR(xilinxIntReg)&0x0f;  // Xilinx interrupt.
         if (x == 0x0f) {
            baseAddress = Addresses[i];
            break;
         }
      }
      i++;
   }
   return (baseAddress == 0) ? canERR_NOTFOUND : canOK;
   // Hmm. Man borde dra i reset (via xilinxen) här så att alla
   // philipsar verkligen har 0x21 i statusregistret
}

-------------------------------------------------------------------------*/


/*
** An internal routine for connecting or disconnecting interrupt lines.
** h        A pointer to the appropriate cardData struct.
** circ     The channel assigned by LocateHardware.
**          0=Intel 1, 1=Intel 2, 2=Philips 1, 3=Philips 2
** connect  TRUE if the interrupt shall be activated, FALSE otherwise
*/
PRIVATE void connectIRQ(cardData* h, unsigned circ, unsigned connect)
{
   BYTE val, x;
   WORD addr = h->address;
   WORD shift;

   if (connect) {
      switch(h->usedIRQ) {
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
      val = 0;
   }
   switch (circ) {
      case 0: shift = 0; break;
      case 1: shift = 2; break;
      case 2: shift = 4; break;
      case 3: shift = 6; break;
      default:
         shift = 8; // circ has an illegal value, set shift to something safe
         ;
   }

   x = READ_PORT_UCHAR((addr + XILINX_OFFSET + XREG_IRQ));
   x &= ~(3U<<shift);
   x |= (val<<shift);
   WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_IRQ, x);
}



/*
** Forward declarations
*/
PRIVATE canStatus PCCANLocateHardware_10(char *cardname);
PRIVATE canStatus findFreeCircuits(WORD baseAddress, cardData* card, char* cardname);
PRIVATE void PCCANinterrupt(cardData* card);

/*
** Initialize the datastructures, using the hardware information found
** in the ini file.
*/
PUBLIC canStatus PCCANLocateHardware(char *cardname)
{
   int version;

   version = (int)GetConfigFileInt(cardname, "VERSION", -1, IniFile);

   switch (version) {
      case -1:
         return canERR_INIFILE;

      case VERSION_NUMBER(1,0):
         // Includes 1.0B, 1.0B-3
         return PCCANLocateHardware_10(cardname);

      case VERSION_NUMBER(1,1):
         // Nonexistent as yet
      case VERSION_NUMBER(2,0):
         // Nonexistent as yet
      default:
         return canERR_INIFILE;
   }
}

/*
** Find all hardware on a PCCAN 1.0B card.
*/
PRIVATE canStatus PCCANLocateHardware_10(char *cardname)
{
   int tmp;
   WORD addr;
   cardData* c;
   canStatus stat;

   tmp = (int)GetConfigFileInt(cardname, "BASE", -1, IniFile);
   if (tmp < 0) return canERR_INIFILE;
   addr = (WORD)tmp;

   // Reset the card, and let it remain in that state.
   WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_CTRL, 0);

   c = allocateCard();
   if (!c) return canERR_NOHANDLES;

   c->address = addr;
   c->supportedIRQ = (1<<3) | (1<<5) | (1<<9);
   c->usedIRQ = -1;
   c->desiredIRQ = (int)GetConfigFileInt(cardname, "IRQ", -1, IniFile);
   c->defaultIRQ = c->desiredIRQ;
   c->valid = TRUE;
   c->cardType = (int)GetConfigFileInt(cardname, "TYPE", -1, IniFile);
   c->refCount = 0;
   c->status = 0;

   (void)GetConfigFileString(cardname, "NAME", "", c->name,
                       sizeof(c->name), IniFile);

   (void)GetConfigFileString(cardname, "VENDOR", "", c->vendor,
                       sizeof(c->vendor), IniFile);

   (void)GetConfigFileString(cardname, "VERSIONSTRING", "", c->version,
                       sizeof(c->version), IniFile);

   stat = findFreeCircuits(addr, c, cardname);

   c->InitCard = PCCANInitCard;
   c->KillCard = PCCANkillCard;
   c->IntRtn = PCCANinterrupt;

   
   return stat;
}

/*
** Search for cicuits on the board (PCCAN 1.0B-3).
** The base address is passed as a parameter and we have just resetted the
** card, so the following algorithm should work.
*/
PRIVATE canStatus findFreeCircuits(WORD baseAddress,
                             cardData* card,
                             char* cardname)
{
   unsigned int ctrlno, port, channel;
   WORD         offset;
   circData*    circ;
   char         entry[10];

   strcpy(entry, "Force ");
   // An entry "Force0=1" will force the code to assume a 82c200
   // An entry "Force0=2" will force the code to assume a 82527

   // Check Philips
   for (ctrlno = 0; ctrlno< 2; ctrlno++) {
      entry[5] = (char)(ctrlno + '2');
      offset = (WORD)(PHILIPS_OFFSET + ctrlno * 0x1000);
      port   = READ_PORT_UCHAR(offset + baseAddress);
      if (port == 0x21 || port == 0x3F || port == 0x3E ||
         (GetConfigFileInt(cardname, entry, 0, IniFile) == 1)) {
         circ = allocateCircuit();
         if (!circ) return canERR_NOHANDLES;
         circ->circStatus = 0;
         circ->circuitType = PCCAN_PHILIPS;
         circ->address = offset + baseAddress;
         channel = ctrlno + 2;
         circ->channel = channel;
         card->Circuits[ctrlno] = circ;
         circ->card = card;
         circ->capabilities = 0;
         circ->valid = TRUE;
         circ->BusOn = PCCANbusOn;
         circ->BusOff = PCCANbusOff;
         circ->SetBusParams = PCCANbusParams;
         circ->SetOutputControl = PCCANbusOutputControl;
         circ->SetMask = PCCANaccept;
         circ->GetStatus = PCCANstatus;
         circ->TXAvailable = PCCANTXAvailable;
         circ->StartWrite = PCCANstartWrite;
         circ->InitCircuit = PCCANInitCircuit;
         circ->KillCircuit = PCCANkillCircuit;
         circ->EnableExtended = PCCANEnableExtended;
//       XilinxIntToCircuit[channel] = circ;
      }
   }

   // Check Intel 527
   for (ctrlno = 0; ctrlno<2; ctrlno++) {
      entry[5] = (char)(ctrlno + '0');
      offset = (WORD)(INTEL_OFFSET + ctrlno * 0x1000);
      port   = READ_PORT_UCHAR(offset + baseAddress + 2) & 0x7f;
      if (port == 0x61 ||
          (GetConfigFileInt(cardname, entry, 0, IniFile) == 2)) {
         circ = allocateCircuit();
         if (!circ) return canERR_NOHANDLES;
         circ->circStatus = 0;
         circ->circuitType = PCCAN_INTEL527;
         circ->address = offset + baseAddress;
         channel = ctrlno;
         circ->channel = channel;
         card->Circuits[2+ctrlno] = circ;
         circ->capabilities = canCIRC_EXTENDED|canCIRC_FULLCAN;
         circ->card = card;
         circ->BusOn = PCCANbusOn;
         circ->BusOff = PCCANbusOff;
         circ->SetBusParams = PCCANbusParams;
         circ->SetOutputControl = PCCANbusOutputControl;
         circ->SetMask = PCCANaccept;
         circ->GetStatus = PCCANstatus;
         circ->TXAvailable = PCCANTXAvailable;
         circ->StartWrite = PCCANstartWrite;
         circ->InitCircuit = PCCANInitCircuit;
         circ->KillCircuit = PCCANkillCircuit;
         circ->EnableExtended = PCCANEnableExtended;
         circ->valid = TRUE;
//       XilinxIntToCircuit[channel] = circ;
      }
   }

   return canOK;
}



/*
** Initialize the specified circuit. Enable its interrupts.
**
** Here's a '527 specific note.
** 
** We will use buffer 1 to send from. We will flip the Xtd bit in buffer
** 1 as required and the buffer will be held in the invalid state unless
** a transmission is pending.  We are using buffers 2-5 to receive in,
** one buffer each for std-rtr, ext-rtr, std and ext.  All global masks
** are set to zeros ("don't care").
** 
** This approach has a few drawbacks.
** 1) We will see RTRs but not their identifiers.
** 2) #1 _when_enabled_ will automatically respond to any RTR.
**    It is only enabled during transmissions however.
** 
** There are alternatives, but they are not very good. For example, if
** we use the global buffer #15 we can not receive std and ext at the
** same time. Using another buffer in parallel with #15 requires us to
** set the global masks to don't care and then we will have the
** not-so-desirable autoresponding feature again.  Buffer #15 will not
** see RTR's, either.
*/

PUBLIC canStatus PCCANInitCircuit(circData* h)
{
   WORD addr = h->address;

   if (h->circuitType == PCCAN_PHILIPS) {

      // Initialize the circuit (Write a RR)
      WRITE_PORT_UCHAR(addr + CCNTRL, RR);

      // Set the CBP (0x40)(bypass comparators) in the SJA1000.
      // This is needed for PCcan-F.
      // Also set ClockOff (0x10) while we're here.
      WRITE_PORT_UCHAR(addr + 31, 0x40 | 0x10);

      // Activate all interrupt sources in the CAN circuit
      WRITE_PORT_UCHAR(addr + CCNTRL, OIE|EIE|RIE|TIE|RR);

      // Set default output control (pushpull, tx on both tx0 & tx1)
      (void)PCCANbusOutputControl(h, canPUSHPULL);

      // Set default bus parameters
      // 1MB/s, Tseg1=5q, Tseg2=3q, SJW=1, 1 sampling point, fast mode.
      (void)PCCANbusParams(h, 1000000L, 4, 3, 0, 1, 0);

      // Accept all messages by default.
      WRITE_PORT_UCHAR(addr + CACC, 0);
      WRITE_PORT_UCHAR(addr + CACM, 0xff);

      // Connect the INT line to the IRQ on the ISA bus.
      connectIRQ(h->card, h->channel, TRUE);

      h->circStatus |= CIRCSTAT_INIT;

      return canOK;

   } else if (h->circuitType == PCCAN_INTEL527) {
      int i;

      // Check that hardware reset has been deasserted.
      if (READ_PORT_UCHAR(addr + CAN_CPUIR) & CPUIR_RSTST) {
         return canERR_HARDWARE;
      }

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

      // Setup #15 to accept all messages.
      //WRITE_PORT_UCHAR(addr + CAN_MSG15MASK0, 0);
      //WRITE_PORT_UCHAR(addr + CAN_MSG15MASK1, 0);
      //WRITE_PORT_UCHAR(addr + CAN_MSG15MASK2, 0);
      //WRITE_PORT_UCHAR(addr + CAN_MSG15MASK3, 0);
      
      // Set default output control (pushpull, tx on both tx0 & tx1)
      (void)PCCANbusOutputControl(h, canPUSHPULL);

      // Set default bus parameters
      // 1MB/s, Tseg1=5q, Tseg2=3q, SJW=1, 1 sampling point, fast mode.
      (void)PCCANbusParams(h, 1000000L, 4, 3, 0, 1, 0);

      // Set all message buffers to
      // Invalid and Interrupts disable.
      for (i = 1; i <= 15; i++) {
         unsigned int port = CANmsg(addr, i) + CANmsg_CTRL0;
         WRITE_PORT_UCHAR(port, CLR(MsgVal) & CLR(TxIE) & CLR(RxIE));
      }

      // receive standard RTR frames
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_RTR_BUF) + CANmsg_CONF, CANmsg_Transmit | CANmsg_Standard);
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_RTR_BUF) + CANmsg_CTRL0, SET(MsgVal) & SET(RxIE));
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_RTR_BUF) + CANmsg_CTRL1, CLR(TxRqst) & SET(CPUUpd));

      // receive extended RTR frames
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_RTR_BUF) + CANmsg_CONF, CANmsg_Transmit | CANmsg_Extended);
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_RTR_BUF) + CANmsg_CTRL0, SET(MsgVal) & SET(RxIE));
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_RTR_BUF) + CANmsg_CTRL1, CLR(TxRqst) & SET(CPUUpd));

      // receive standard data frames
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_BUF) + CANmsg_CONF, CANmsg_Receive | CANmsg_Standard);
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_BUF) + CANmsg_CTRL0, SET(MsgVal) & SET(RxIE));
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_BUF) + CANmsg_CTRL1, CLR(TxRqst) & CLR(CPUUpd));

      // receive extended data frames
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_BUF) + CANmsg_CONF, CANmsg_Receive | CANmsg_Extended);
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_BUF) + CANmsg_CTRL0, SET(MsgVal) & SET(RxIE));
      WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_BUF) + CANmsg_CTRL1, CLR(TxRqst) & CLR(CPUUpd));

      // send messages
      WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CONF, CANmsg_Transmit);
      WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CTRL0, CLR(MsgVal) & SET(TxIE));
      WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CTRL1, SET(CPUUpd) & CLR(TxRqst));

      // Connect the INT line to the IRQ on the ISA bus.
      connectIRQ(h->card, h->channel, TRUE);

      // Enable interrupts.
      WRITE_PORT_UCHAR(addr + CAN_CTRL,
                       CTRL_INIT|CTRL_CCE|CTRL_EIE|CTRL_IE);
      // qqq don't enable CTRL_SIE for now.

      h->circStatus |= CIRCSTAT_INIT;

      return canOK;
   }
   return canERR_PARAM;
}


/*
** Kill a certain CAN circuit.
*/
PUBLIC canStatus PCCANkillCircuit(circData* h)
{
   WORD timeout = (WORD)(TIMEOUT * Timeout_1ms);
   WORD addr = h->address;

   if (h->circuitType == PCCAN_PHILIPS) {

      while (!(READ_PORT_UCHAR(addr + CSTAT) & TBA)) {
         if (--timeout == 0) break;
      }

      // Disconnect the interupt line
      connectIRQ(h->card, h->channel, FALSE);

      // Reset the 82C200. This will be read as 0x21 next time.
      WRITE_PORT_UCHAR(addr + CCNTRL, 0x21);

      h->circStatus &= ~CIRCSTAT_INIT;

   } else if (h->circuitType == PCCAN_INTEL527) {

      while (!(READ_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CTRL1) & CTRL1_TxRqst)) {
         if (--timeout == 0) break;
      }

      // Disconnect the interupt line
      connectIRQ(h->card, h->channel, FALSE);

      // Reset the 82527.
      // The interrupt enable bits are cleared.
      WRITE_PORT_UCHAR(addr + CAN_CTRL, CTRL_INIT);

      h->circStatus &= ~CIRCSTAT_INIT;

   }
   return canOK;
}

PUBLIC canStatus PCCANInitCard(cardData* h)
{
   WORD addr = h->address;

   // Reset.
   WRITE_PORT_UCHAR(h->address + XILINX_OFFSET + XREG_CTRL, 0);

   // Use the on-board clock
   WRITE_PORT_UCHAR(addr + XILINX_OFFSET + XREG_CLOCK, XREG_CLOCK_ONBOARD);

   // Remove reset.
   WRITE_PORT_UCHAR(h->address + XILINX_OFFSET + XREG_CTRL, XREG_CTRL_NORESET);
   h->status |= canCARD_INIT;

   return canOK;
}


/*
** Kill the PCCAN card entirely.
*/
PUBLIC canStatus PCCANkillCard(cardData* h)
{
   // Disconnect all IRQ's.
   WRITE_PORT_UCHAR(h->address + XILINX_OFFSET + XREG_IRQ, 0);
   // Set the card in reset status and let it remain there.
   WRITE_PORT_UCHAR(h->address + XILINX_OFFSET + XREG_CTRL, 0);
   h->status &= ~canCARD_INIT;
   return canOK;
}


/*
** Go on bus.
*/
PUBLIC canStatus PCCANbusOn(circData *h)
{
    BYTE tmp;
    WORD addr = h->address;

    if (h->circuitType == PCCAN_PHILIPS) {
        // Try to go on bus
        tmp = READ_PORT_UCHAR(addr + CCNTRL);
        WRITE_PORT_UCHAR(addr + CCOM, COS);
        WRITE_PORT_UCHAR(addr + CCNTRL, tmp |  RR);
        WRITE_PORT_UCHAR(addr + CCNTRL, tmp & ~RR);
        // Notify interested parties
        PCCAN_82C200_Error(h, addr);
        return canOK;
    } else if (h->circuitType == PCCAN_INTEL527) {
        int i;
        for (i=I527_FIRST_RX_BUF; i<=I527_LAST_RX_BUF; i++) {
            WRITE_PORT_UCHAR(CANmsg(addr, i) + CANmsg_CTRL1, CLR(NewDat));
        }
        //  Go on-bus: clear INIT and CCE
        tmp = READ_PORT_UCHAR(addr + CAN_CTRL);
        WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp |  (CTRL_INIT | CTRL_CCE));
        WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp & ~(CTRL_INIT | CTRL_CCE));
        // Notify interested parties
        PCCAN_INTEL527_Error(h, addr);
        return canOK;
    }
    h->ackErrCount = 0;
    return canERR_PARAM;
}

/*
** Go off bus.
*/
PUBLIC canStatus PCCANbusOff(circData *h)
{
   BYTE tmp;
   WORD addr = h->address;

   if (h->circuitType == PCCAN_PHILIPS) {
      tmp = READ_PORT_UCHAR(addr + CCNTRL);
      WRITE_PORT_UCHAR(addr + CCNTRL, tmp|RR);
      // Notify interested parties
      PCCAN_82C200_Error(h, addr);
      return canOK;
   } else if (h->circuitType == PCCAN_INTEL527) {
      //  Go off-bus: set INIT and CCE
      tmp = READ_PORT_UCHAR(addr + CAN_CTRL);
      tmp |= (CTRL_INIT | CTRL_CCE);
      WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp);
      // Notify interested parties
      PCCAN_INTEL527_Error(h, addr);
      return canOK;
   }
   h->ackErrCount = 0;
   return canERR_PARAM;
}

/*
** Set the bus parameters.
*/
PUBLIC canStatus PCCANbusParams(circData* h,
                          unsigned long freq, unsigned int tseg1,
                          unsigned int tseg2,
                          unsigned int sjw, unsigned int noSamp,
                          unsigned int syncmode)
{
   sjw--;
   if (h->circuitType == PCCAN_PHILIPS) {
      unsigned quantasPerCycle;
      DWORD brp;
      BYTE cbt0;
      BYTE cbt1;
      WORD addr;
      BYTE tmp;
      int resetStatus;

      quantasPerCycle = tseg1 + tseg2 + 1;
      if (quantasPerCycle == 0 || freq == 0) return canERR_PARAM;
      brp = (8000000L * 64) / (freq * quantasPerCycle);
      if ((brp & 0x3F) != 0) {
         // Fraction != 0 : not divisible.
         return canERR_PARAM;
      }
      brp = (brp >> 6) - 1;

      if (brp > 64 || sjw > 3 || quantasPerCycle < 8) {
         return canERR_PARAM;
      }
      cbt0 = (BYTE)(((BYTE)sjw <<6) + brp);
      cbt1 = (BYTE)(((noSamp==3?1:0)<<7) + ((tseg2-1)<<4) + (tseg1-1));

      // Put the circuit in Reset Mode
      addr = h->address;
      DISABLE_INTERRUPTS;
      tmp = READ_PORT_UCHAR(addr + CCNTRL);
      resetStatus = tmp & RR;
      WRITE_PORT_UCHAR(addr + CCNTRL, tmp|RR);

      WRITE_PORT_UCHAR(addr + CBT0, cbt0);
      WRITE_PORT_UCHAR(addr + CBT1, cbt1);

      if (syncmode != 0)
         tmp |= SPD;
      else
         tmp &= ~SPD;
      WRITE_PORT_UCHAR(addr + CCNTRL, tmp|RR);

      if (resetStatus == FALSE) {
         tmp = READ_PORT_UCHAR(addr + CCNTRL);
         WRITE_PORT_UCHAR(addr + CCNTRL, tmp & ~RR);
      }
      ENABLE_INTERRUPTS;
      return canOK;
   } else if (h->circuitType == PCCAN_INTEL527) {
      unsigned int quantasPerCycle;
      DWORD brp;
      BYTE btr0;
      BYTE btr1;
      WORD addr;
      BYTE tmp;

      // SCLK is 8MHz
      quantasPerCycle = tseg1 + tseg2 + 1;
      if (quantasPerCycle == 0 || freq == 0) return canERR_PARAM;
      brp = (8000000L * 64) / (freq * quantasPerCycle);
      if ((brp & 0x3F) != 0) {
         // Fraction != 0 : not divisible.
         return canERR_PARAM;
      }
      brp = (brp >> 6) - 1;

      if (brp > 64 || sjw > 3 || quantasPerCycle < 8
          || tseg1<3 || tseg1 > 16 || tseg2 < 2 || tseg2 > 8) {
         return canERR_PARAM;
      }
      btr0 = (BYTE)(((BYTE)sjw <<6) + brp);
      btr1 = (BYTE)(((noSamp==3?1:0)<<7) + ((tseg2-1)<<4) + (tseg1-1));

      // Go off-bus.
      addr = h->address;
      DISABLE_INTERRUPTS;
      tmp = READ_PORT_UCHAR(addr + CAN_CTRL);
      WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp|CTRL_INIT | CTRL_CCE);

      WRITE_PORT_UCHAR(addr + CAN_BTR0, btr0);
      WRITE_PORT_UCHAR(addr + CAN_BTR1, btr1);

      // Syncmode is ignored. You shouldn't synchronize on both edges
      // anyway.

      // Restore control register
      WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp);

      ENABLE_INTERRUPTS;
      return canOK;
   }

   return canERR_PARAM;
}


/*
** Set driver output type.
*/
PUBLIC canStatus PCCANbusOutputControl(circData *h, unsigned int drivertype)
{
    short driver = -1;
    
   if (h->circuitType == PCCAN_PHILIPS) {

      WORD addr = h->address;
      BYTE tmp;

      switch (drivertype) {
          case canDRIVER_NORMAL:
              driver = 0xda;  // Pushpull  (OCTP1|OCTN1|OCTP0|OCTN0|OCM1)
              break;
          case canDRIVER_SILENT:
              driver = 0x02;  // Tristate - silent mode (OCM1)
              break;
          case canDRIVER_SELFRECEPTION:
              driver = -1;
              break;
          case canDRIVER_OFF:
              driver = 0;
              break;
          default:
              driver = -1;
              break;
      }

      if (driver == -1) return canERR_DRIVER;

      DISABLE_INTERRUPTS;

      // Save control register
      tmp = READ_PORT_UCHAR(addr + CCNTRL);
      // Put the circuit in Reset Mode
      WRITE_PORT_UCHAR(addr + CCNTRL, tmp|RR);
      // Set the output control
      WRITE_PORT_UCHAR(addr + COCNTRL, driver & 0xff);
      // Restore control register
      WRITE_PORT_UCHAR(addr + CCNTRL, tmp);

      ENABLE_INTERRUPTS;

      return canOK;
   } else if (h->circuitType == PCCAN_INTEL527) {
      WORD addr = h->address;
      BYTE tmp;

      switch (drivertype) {
          case canDRIVER_NORMAL:
              driver = BUSCON_COBY|BUSCON_DCT1|BUSCON_DCR1;      // Pushpull
              break;

          case canDRIVER_SILENT:
          case canDRIVER_SELFRECEPTION:
          case canDRIVER_OFF:
          default:
              break;
      }

      if (driver == -1) return canERR_DRIVER;

      DISABLE_INTERRUPTS;

      // Save control register
      tmp = READ_PORT_UCHAR(addr + CAN_CTRL);
      // Put the circuit in Reset Mode
      WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp|CTRL_INIT | CTRL_CCE);
      // Set the output control
      WRITE_PORT_UCHAR(addr + CAN_BUSCON, driver & 0xff);
      // Restore control register
      WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp);

      ENABLE_INTERRUPTS;

      return canOK;
   }
   return canERR_PARAM;
}

/*
** Construct a hardware filter.
** The parameter envelope has different meanings depending on the value
** of flags.
** flags          envelope
** =====          ========
** canFILTER_ACCEPT         Not implemented (qqq)
** canFILTER_REJECT         Not implemented (qqq)
** canFILTER_SET_CODE_STD   Acceptance code; the identifier is compared to
**                          selected bits in the acceptance code and the
**                          message is accepted if they are equal.
** canFILTER_SET_MASK_STD   Acceptance mask; a "1" means the corresponding
**                          bit in the acceptance code is relevant; a "0"
**                          means a "don't care" bit.
** canFILTER_SET_xxx_EXT    Same as xxx_STD above, but the envelope is assumed
**                          to be an extended "identifier".
*/
PUBLIC canStatus PCCANaccept(circData* h, long envelope, unsigned int flags)
{
   if (h->circuitType == PCCAN_PHILIPS) {

      BYTE code = 0;
      BYTE mask = 0xff;
      BYTE gotCode = FALSE;
      BYTE gotMask = FALSE;
      WORD addr = h->address;
      BYTE tmp;

      // Calculate acceptance code and acceptance mask
      switch (flags) {
         case canFILTER_SET_CODE_STD:
            code = (BYTE)(((DWORD)envelope)>>3);
            h->stdAccCode = envelope;
            gotCode = TRUE;
            break;

         case canFILTER_SET_MASK_STD:
            // Note the negation; in the 82C200,
            // a "0" means the bit is relevant.
            mask = (BYTE)(~(((DWORD)envelope)>>3));
            h->stdAccMask = envelope;
            gotMask = TRUE;
            break;

         case canFILTER_ACCEPT:
         case canFILTER_REJECT:
         default:
            return canERR_PARAM;
      }

      DISABLE_INTERRUPTS;
      // Save control register
      tmp = READ_PORT_UCHAR(addr + CCNTRL);
      // Put the circuit in Reset Mode
      WRITE_PORT_UCHAR(addr + CCNTRL, tmp|RR);

      // Set the acceptance code
      if (gotCode) WRITE_PORT_UCHAR(addr + CACC, code);

      // Set the acceptance mask
      if (gotMask) WRITE_PORT_UCHAR(addr + CACM, mask);

      // Restore control register
      WRITE_PORT_UCHAR(addr + CCNTRL, tmp);
      ENABLE_INTERRUPTS;

      return canOK;
   } else if (h->circuitType == PCCAN_INTEL527) {

      WORD addr = h->address;
      int stdFilter;
      
      BYTE tmp;
      WL code;
      WL mask;

      BYTE gotCode = FALSE;
      BYTE gotMask = FALSE;

      code.L = 0L;
      mask.L = 0L;
      stdFilter = FALSE;
      
      // Calculate acceptance code and acceptance mask
      switch (flags) {
         case canFILTER_SET_CODE_STD:
            code.W.lsw = (WORD)envelope;
            gotCode = TRUE;
            stdFilter = TRUE;
            h->stdAccCode = envelope;
            break;

         case canFILTER_SET_MASK_STD:
            mask.W.lsw = (WORD)envelope;
            gotMask = TRUE;
            stdFilter = TRUE;
            h->stdAccMask = envelope;
            break;

         case canFILTER_SET_CODE_EXT:
            code.L = (DWORD)envelope;
            gotCode = TRUE;
            stdFilter = FALSE;
            h->extAccCode = envelope;
            break;

         case canFILTER_SET_MASK_EXT:
            mask.L = (DWORD)envelope;
            gotMask = TRUE;
            stdFilter = FALSE;
            h->extAccMask = envelope;
            break;

         case canFILTER_ACCEPT:
         case canFILTER_REJECT:
         default:
            return canERR_PARAM;
      }

      DISABLE_INTERRUPTS;
      // Save control register
      tmp = READ_PORT_UCHAR(addr + CAN_CTRL);
      // Put the circuit in Reset Mode
      WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp|CTRL_INIT|CTRL_CCE);

      if (stdFilter) {
          // Set the acceptance code, standard CAN
          if (gotCode) {
              code.W.lsw <<= 5;
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_BUF) + CANmsg_ARB + 0,
                               mask.B.b1);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_BUF) + CANmsg_ARB + 1,
                               mask.B.b0);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_RTR_BUF) + CANmsg_ARB + 0,
                               mask.B.b1);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_STD_RTR_BUF) + CANmsg_ARB + 1,
                               mask.B.b0);
          }

          // Set the acceptance mask, standard CAN
          if (gotMask) {
              mask.W.lsw <<= 5;
              WRITE_PORT_UCHAR(addr + CAN_STDGBLMASK0, mask.B.b1);
              WRITE_PORT_UCHAR(addr + CAN_STDGBLMASK1, mask.B.b0);
          }
      } else {
          if (gotCode) {
              // Set the acceptance code, extended CAN
              code.L <<= 3;
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_BUF) + CANmsg_ARB + 0, code.B.b3);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_BUF) + CANmsg_ARB + 1, code.B.b2);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_BUF) + CANmsg_ARB + 2, code.B.b1);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_BUF) + CANmsg_ARB + 3, code.B.b0);

              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_RTR_BUF) + CANmsg_ARB + 0, code.B.b3);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_RTR_BUF) + CANmsg_ARB + 1, code.B.b2);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_RTR_BUF) + CANmsg_ARB + 2, code.B.b1);
              WRITE_PORT_UCHAR(CANmsg(addr, I527_RX_EXT_RTR_BUF) + CANmsg_ARB + 3, code.B.b0);
          }
          if (gotMask) {
              // Set the acceptance mask, extended CAN
              mask.L <<= 3;
              WRITE_PORT_UCHAR(addr + CAN_EXTGBLMASK0, mask.B.b3);
              WRITE_PORT_UCHAR(addr + CAN_EXTGBLMASK1, mask.B.b2);
              WRITE_PORT_UCHAR(addr + CAN_EXTGBLMASK2, mask.B.b1);
              WRITE_PORT_UCHAR(addr + CAN_EXTGBLMASK3, mask.B.b0);
          }
      }
      // Restore control register
      WRITE_PORT_UCHAR(addr + CAN_CTRL, tmp);
      ENABLE_INTERRUPTS;

      return canOK;
   }

   return canERR_PARAM;
}

PRIVATE WORD statusToBusStatus_82c200(BYTE SR, BYTE CR,
                                      WORD *txErr,
                                      WORD *rxErr)
{
    WORD stat = 0;
    
    switch (SR & (BS|ES)) {
        case BS:
            stat = canSTAT_BUS_OFF;
            *txErr = 255;
            *rxErr = 255;
            break;
            
        case BS|ES:
            stat = canSTAT_BUS_OFF;
            *txErr = 255;
            *rxErr = 255;
            break;

        case ES:
            // qqq "Error passive" is a lie, but what else should I do?
            stat = canSTAT_ERROR_WARNING|canSTAT_ERROR_PASSIVE;
            *txErr = 96;
            *rxErr = 96;
            break;
            
        case 0:
            stat = canSTAT_ERROR_ACTIVE;
            *txErr = 0;
            *rxErr = 0;
            break;
    }
    if (CR & RR) {
        stat |= canSTAT_INACTIVE;
        *txErr = 0;
        *rxErr = 0;
    }
    return stat;
}

PRIVATE WORD statusToBusStatus_82527(BYTE SR, BYTE CR,
                                     WORD *txErr,
                                     WORD *rxErr)
{
    WORD stat = 0;
    
    switch (SR & (STAT_BOFF|STAT_WARN)) {
        case STAT_BOFF:
        case STAT_BOFF|STAT_WARN:
            stat = canSTAT_BUS_OFF;
            *txErr = 255;
            *rxErr = 255;
            break;

        case STAT_WARN:
            // qqq "Error passive" is a lie, but what else should I do?
            stat = canSTAT_ERROR_WARNING|canSTAT_ERROR_PASSIVE;
            *txErr = 96;
            *rxErr = 96;
            break;
            
        case 0:
            stat = canSTAT_ERROR_ACTIVE;
            *txErr = 0;
            *rxErr = 0;
            break;
    }
    if (CR & CTRL_INIT) {
        stat |= canSTAT_INACTIVE;
        *txErr = 0;
        *rxErr = 0;
    }
    return stat;
}



/*
** Retrieve the current controller status. Needs more work (qqq).
*/
PUBLIC canStatus PCCANstatus(circData* h, unsigned long* flags)
{
    DWORD outStatus;
    WORD addr = h->address;

    outStatus = 0L;
   
    if (h->circuitType == PCCAN_PHILIPS) {
        BYTE SR, CR;
        SR = READ_PORT_UCHAR(addr + CSTAT);
        CR = READ_PORT_UCHAR(addr + CCNTRL);

        h->busStatus = statusToBusStatus_82c200(SR, CR,
                                                &h->RXErrorCounter,
                                                &h->TXErrorCounter);

    } else if (h->circuitType == PCCAN_INTEL527) {
        BYTE SR, CR;
        SR = READ_PORT_UCHAR(addr + CAN_STAT);
        CR = READ_PORT_UCHAR(addr + CAN_CTRL);
        h->busStatus = statusToBusStatus_82527(SR, CR,
                                        &h->RXErrorCounter,
                                        &h->TXErrorCounter);
    }        

    outStatus = h->busStatus;
    
    if (h->hwOverrun) outStatus |= canSTAT_HW_OVERRUN;
    if (h->swOverrun) outStatus |= canSTAT_SW_OVERRUN;
    if (h->RXErrorCounter > 0) h->rxError = TRUE;
    if (h->TXErrorCounter > 0) h->txError = TRUE;

    if (h->rxError) outStatus |= canSTAT_RXERR;
    if (h->txError) outStatus |= canSTAT_TXERR;
   
    if (!flags) return canERR_PARAM;

    *flags = outStatus;
    return canOK;
}

/*
** Start a transmission. May be called at interupt level.
*/
PUBLIC canStatus PCCANstartWrite(circData* h, long id, void* msgPtr,
                           unsigned int dlc, unsigned int flag)
{
    WORD p;
    WORD addr = h->address;
    BYTE* msg = (BYTE*) msgPtr;
    WORD ident = (WORD) id;

   if (h->circuitType == PCCAN_PHILIPS) {
      WRITE_PORT_UCHAR(addr + TBI, (BYTE)(ident>>3));
      WRITE_PORT_UCHAR(addr + TRTDL, (BYTE)(((ident & 0x7)<<5)
                           | (flag&canMSG_RTR?0x10:0)
                           | dlc));
      p = addr + TDS0;
      if (msgPtr) switch(dlc) {
         case 8:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 7:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 6:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 5:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 4:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 3:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 2:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 1:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 0:
         default:
            ;
      }

      WRITE_PORT_UCHAR(addr + CCOM, TR);
      return canOK;

   } else if (h->circuitType == PCCAN_INTEL527) {
      BYTE tmp;
      WL x;

      WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CTRL0,
                       CLR(MsgVal));
      WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CTRL1,
               CLR(RmtPnd) & CLR(TxRqst) &  SET(CPUUpd));

      // Message configuration.
      tmp = (BYTE)CANmsg_DLC(dlc);
      if (flag & canMSG_RTR) {
         tmp |= CANmsg_Receive;
      } else {
         tmp |= CANmsg_Transmit;
      }
      if (flag & canMSG_STD) {
         tmp |= CANmsg_Standard;
      } else if (flag & canMSG_EXT) {
         tmp |= CANmsg_Extended;
      }
      WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CONF, tmp);

      // The identifier.
      if (flag & canMSG_STD) {
         WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_ARB + 0, (BYTE)(ident >> 3));
         WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_ARB + 1, (BYTE)((ident&7)<<5));
      } else if (flag & canMSG_EXT) {
         x.L = (((DWORD)id) << 3);
         WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_ARB + 0, x.B.b3);
         WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_ARB + 1, x.B.b2);
         WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_ARB + 2, x.B.b1);
         WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_ARB + 3, x.B.b0);
      }

      p = CANmsg(addr, I527_TX_BUF) + CANmsg_DATA;

      if (msgPtr) switch(dlc) {
         case 8:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 7:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 6:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 5:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 4:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 3:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 2:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 1:
            WRITE_PORT_UCHAR(p++, *msg++);
            // Fall through
         case 0:
         default:
            ;
      }

      WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CTRL1,
               SET(NewDat) & SET(TxRqst) & CLR(CPUUpd));
      WRITE_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CTRL0,
                       SET(MsgVal));
      return canOK;
   }
   return canERR_PARAM;
}

PUBLIC int PCCANTXAvailable(circData* h)
{
   WORD addr = h->address;

   if (h->circuitType == PCCAN_PHILIPS) {
      if ((READ_PORT_UCHAR(addr + CSTAT) & TBA) == TBA)
         return TRUE;
      else
         return FALSE;
   } else if (h->circuitType == PCCAN_INTEL527) {

      if (READ_PORT_UCHAR(CANmsg(addr, I527_TX_BUF) + CANmsg_CTRL1) & CTRL1_TxRqst)
         return FALSE;
      else
         return TRUE;

   }
   return FALSE;
}

//qqq Obsolete
PUBLIC canStatus PCCANEnableExtended(circData* h, unsigned int onoff)
{
    ARGUSED(onoff);
    if (h->circuitType == PCCAN_INTEL527) {
        return canOK;
    } else {
        return canERR_PARAM;
    }
}


#define PCCAN_82C200_PROBE_INT(addr) (BYTE)(READ_PORT_UCHAR((addr) + CINT) & 0x1F)
#define PCCAN_INTEL527_PROBE_INT(addr) (BYTE)(READ_PORT_UCHAR((addr) + CAN_IR))

/*
** This routine is called from the (real) interrupt routine in irq.c.
*/
PRIVATE void PCCANinterrupt(cardData* card)
{
     circData*    h;
     int          index;
     BYTE         ireg;
     BYTE         xInt;
     WORD         XIntReg;
     unsigned int loopmax = 1000;

    XIntReg = card->address + XILINX_OFFSET + XREG_INTERRUPT;

    // This while-loop is needed because the PIC is edge triggered.
    // We look for active INTs in the Xilinx instead!
    while ((xInt = (READ_PORT_UCHAR(XIntReg) & 0x0F)) != 0) {
        if (--loopmax == 0) {
            break;
        }
        for (index=0; index < 4; index++, xInt>>=1) {
            if (xInt & 0x01) continue; // No interrupt from circuit #index
            
            switch(index) {
              case 0:
                h = card->Circuits[2];
                break;
              case 1:
                h = card->Circuits[3];
                break;
              case 2:
                h = card->Circuits[0];
                break;
              case 3:
                h = card->Circuits[1];
                break;
            }
//          h = XilinxIntToCircuit[index];
            
            if (h && (h->circStatus & CIRCSTAT_OPEN) == CIRCSTAT_OPEN) {
                WORD address = h->address;

                // Has the circuit whose handle we just got interrupted?
                switch (h->circuitType) {

                    case PCCAN_PHILIPS:
                        ireg = PCCAN_82C200_PROBE_INT(address);
                        if (ireg != 0) {
                            PCCAN_82C200_Interrupt(h, address, ireg);
                        }
                        break;

                    case PCCAN_INTEL526:
                        // Not supported.
                        break;

                    case PCCAN_INTEL527:
                        ireg = PCCAN_INTEL527_PROBE_INT(address);
                        if (ireg != 0) {
                            PCCAN_INTEL527_Interrupt(h, address, ireg);
                        }
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

PRIVATE void PCCAN_82C200_Interrupt(circData* h,
                                     WORD address,
                                     BYTE ireg)
{
    WORD loopmax = 1000;

    while (ireg && loopmax--) {
        if (h->buf) {
            if (ireg & RIF) {
                PCCAN_82C200_Receive(h, address);
            }
            if (ireg & TIF) {
                PCCAN_82C200_Transmit(h, address);
            }
            if (ireg & EIF)
                PCCAN_82C200_Error(h, address);
            
            /*
            if (ireg & (EIF|OIF|WIF)) {
                if (ireg & OIF)
                     PCCAN_82C200_Overrun(h, address);
                if (ireg & WIF)
                    PCCAN_82C200_Wakeup(h, address);
            }
            */
        }
        ireg = PCCAN_82C200_PROBE_INT(address);

        if (loopmax == 0) {
            // Something hangs... or there is a considerable CAN traffic
            /// h->flags |= canFLAG_INT;   // qqq
            break;
        }
    }
}

PRIVATE void PCCAN_82C200_Receive(circData* h, WORD address)
{
   bufData* b;
   Message* buf;
   unsigned int i, rxbufsize;
   Message* m;
   BYTE rrtdl;
   BYTE* p;
   WORD data;
   unsigned int savedNextMsg;


   b = h->buf;
   rxbufsize = b->rxBufSize;
   buf = b->rxBuffer;
   savedNextMsg = i = h->NextMsg;
   m = &buf[i++];
   if (i>=rxbufsize) i=0;
   h->NextMsg = i;

   // Read controller data and store in buffer.
   rrtdl    = READ_PORT_UCHAR(address + RRTDL);
   m->id    = (READ_PORT_UCHAR(address + RBI)<<3) + (rrtdl>>5);
   m->dlc   = rrtdl & 0x0F;
   m->flags = ((rrtdl & 0x10)?canMSG_RTR:0) | canMSG_STD;
   data     = address + RDS0;
   p        = m->data;

   switch (m->dlc) {
      case 8:
         *p++ = READ_PORT_UCHAR(data++);
         // Fall through
      case 7:
         *p++ = READ_PORT_UCHAR(data++);
         // Fall through
      case 6:
         *p++ = READ_PORT_UCHAR(data++);
         // Fall through
      case 5:
         *p++ = READ_PORT_UCHAR(data++);
         // Fall through
      case 4:
         *p++ = READ_PORT_UCHAR(data++);
         // Fall through
      case 3:
         *p++ = READ_PORT_UCHAR(data++);
         // Fall through
      case 2:
         *p++ = READ_PORT_UCHAR(data++);
         // Fall through
      case 1:
         *p++ = READ_PORT_UCHAR(data++);
         // Fall through
      case 0:
      default:
         ;
   }

    if (READ_PORT_UCHAR(address + CSTAT) & DO) {
        h->hwOverrun = TRUE;
        h->TotalOverruns++;
        WRITE_PORT_UCHAR(address + CSTAT, COS);
    }

    // Release receive buffer
    WRITE_PORT_UCHAR(address + CCOM, RRB);

    // Sample receive time
    m->time = GETTIMER();

    // qqq Save error registers etc, if desired (use a flag in canOpen)

    m->overwritten = TRUE;
    if (m->targets) {
        // We're going to overwrite the buffer
        h->swOverrun = TRUE;
        h->TotalOverruns++;
    }
    m->targets = 0;

   for (i=0; i<COUNTOF(h->handles); i++) {
      handleData *hnd = h->handles[i];
      if (hnd == NULL) continue;

      // Mark all handles as interested.
      // qqq someday, we'll provide soft filtering for the
      // individual masks.
      m->targets |= hnd->handleMask;
      if (hnd->rxFirstMsg == savedNextMsg) {
          if (hnd->notifyFlags & canNOTIFY_RX) {
              NOTIFY(hnd->hWnd, canEVENT_RX, hnd->nr, 0);
          }
      }
   }
}


PRIVATE void PCCAN_82C200_Transmit(circData* h, WORD address)
{
   bufData* b = h->buf;
   unsigned int i;

   ARGUSED(address);
   //
   // Send the next message, if the transmit buffer is empty.
   //
   if (b->txMsgs != 0 ) {
      txMessage* t;
      i = b->txHead;
      t = &b->txBuffer[i];
      (void)PCCANstartWrite(h, t->id, t->data, t->dlc, t->flags);
      t->available = TRUE;
      i++;
      if (i >= b->txBufSize) i = 0;
      b->txHead = i;
      b->txMsgs--;
   }

   // Notify interested parties.
   // qqq Perhaps we should have a message-specific flag for this.
   // (I.e. "please tell me when this message is sent")
   for (i=0; i<COUNTOF(h->handles); i++) {
      handleData *hnd = h->handles[i];
      if (hnd == NULL) continue;

      if (hnd->notifyFlags & canNOTIFY_TX) {
         NOTIFY(hnd->hWnd, canEVENT_TX, hnd->nr, 0);
      }
   }
}


/*
** Handle error interrupts. In this way, the 82C200 will inform us of changes
** in the error or bus status.
*/
PRIVATE void PCCAN_82C200_Error(circData *h, WORD address)
{
   BYTE SR, CR;
   int i;
   
   SR = READ_PORT_UCHAR(address + CSTAT);
   CR = READ_PORT_UCHAR(address + CCNTRL);

   h->busStatus = statusToBusStatus_82c200(SR, CR,
                                        &h->RXErrorCounter,
                                        &h->TXErrorCounter);
   
   for (i=0; i<COUNTOF(h->handles); i++) {
      handleData *hnd = h->handles[i];
      if (hnd == NULL) continue;
      if (hnd->notifyFlags & canNOTIFY_RX) {
          NOTIFY(hnd->hWnd, canEVENT_STATUS, hnd->nr, 0);
      }
   }
}


/*
** Handle an interrupt from an 82527.
*/
PRIVATE void PCCAN_INTEL527_Interrupt(circData *h,
                                      WORD address,
                                      BYTE ireg)
{
    WORD loopmax = 1000;
    while (ireg && loopmax--) {
        if (h->buf) {
            if (ireg >= (I527_FIRST_RX_BUF+2) && ireg <= (I527_LAST_RX_BUF+2)) {
                // Buffer 1-4 interrupt
                PCCAN_INTEL527_Receive(h, address, ireg-2);
            } else if (ireg == (I527_TX_BUF + 2)) {
                // Buffer 5 interrupt
                PCCAN_INTEL527_Transmit(h, address);
            } else if (ireg == 1) {
                // Status Register has changed.
                PCCAN_INTEL527_Error(h, address);
            } else if (ireg == 2) {
                // Should not occur, but...
                WRITE_PORT_UCHAR(CANmsg(address, 15), CLR(IntPnd));
            } else {
                // Should not occur, but...
                WRITE_PORT_UCHAR(CANmsg(address, ireg-2), CLR(IntPnd));
            }
        }
        ireg = PCCAN_INTEL527_PROBE_INT(address);

        if (loopmax == 0) {
            // Something hangs... or there is a considerable CAN traffic
            /// h->flags |= canFLAG_INT;   // qqq
            break;
        }
    }
}

/*
** Receive a message from an 82527
*/
PRIVATE void PCCAN_INTEL527_Receive(circData* h, WORD address, int bufNo)
{
    bufData* b;
    Message* buf;
    int i;
    int rxbufsize;
    Message* m;
    BYTE conf;
    BYTE* p;
    WORD data;
    WL id;
    WORD rxBuf;
    unsigned int savedNextMsg;

    b = h->buf;                         // Pointer to the buffer head
    rxbufsize = b->rxBufSize;           // The size of the receive buffer
    buf = b->rxBuffer;                  // A pointer to the buffer itself
    savedNextMsg = i = h->NextMsg;      // Index of next
    m = &buf[i++];
    if (i>=rxbufsize) i=0;
    h->NextMsg = i;

    rxBuf = CANmsg(address, bufNo);

    conf = READ_PORT_UCHAR(rxBuf + CANmsg_CONF);

    m->dlc   = conf >> 4;
    m->flags = ((conf & CANmsg_Extended)?canMSG_EXT:canMSG_STD);

    if (bufNo == I527_RX_EXT_RTR_BUF || bufNo == I527_RX_STD_RTR_BUF) {
        //
        // A remote request.
        // The 527 writes neither the dlc nor the id in the buffer.
        //
        m->flags |= canMSG_RTR;
        m->id = CANID_WILDCARD;
        // We don't care about the data because this is an RTR.

        // Release receive buffer.
        // CPUUpd/MsgLst is always set ('cause we've set it up that way)
        // so we can't check for overflow.
        WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL0, CLR(IntPnd));
        WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL1, CLR(NewDat) & CLR(RmtPnd));
    } else {
        //
        // An ordinary data frame.
        //
        id.L = 0L;
        if (conf & CANmsg_Extended) {
            id.B.b3 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 0);
            id.B.b2 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 1);
            id.B.b1 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 2);
            id.B.b0 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 3);
            id.L >>= 3;
        } else {
            id.B.b1 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 0);
            id.B.b0 = READ_PORT_UCHAR(rxBuf + CANmsg_ARB + 1);
            id.W.lsw >>= 5;
        }

        m->id = id.L;

        data = rxBuf + CANmsg_DATA;
        p = m->data;
        switch (m->dlc) {
            case 8:
                *p++ = READ_PORT_UCHAR(data++);
                // Fall through
            case 7:
                *p++ = READ_PORT_UCHAR(data++);
                // Fall through
            case 6:
                *p++ = READ_PORT_UCHAR(data++);
                // Fall through
            case 5:
                *p++ = READ_PORT_UCHAR(data++);
                // Fall through
            case 4:
                *p++ = READ_PORT_UCHAR(data++);
                // Fall through
            case 3:
                *p++ = READ_PORT_UCHAR(data++);
                // Fall through
            case 2:
                *p++ = READ_PORT_UCHAR(data++);
                // Fall through
            case 1:
                *p++ = READ_PORT_UCHAR(data++);
                // Fall through
            case 0:
            default:
                ;
        }

        //
        // Check for overflow.
        //
        if (READ_PORT_UCHAR(rxBuf + CANmsg_CTRL1) & TEST(MsgLst)) {
            h->hwOverrun = TRUE;
            h->TotalOverruns++;
        }
        WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL0, CLR(IntPnd));
        WRITE_PORT_UCHAR(rxBuf + CANmsg_CTRL1, CLR(MsgLst) & CLR(NewDat) & CLR(RmtPnd));
    }

    // Sample receive time
    m->time = GETTIMER();

    // Save error counters etc, if desired (specified when canOpen was called)

    m->overwritten = TRUE;
    if (m->targets) {
        // We're going to overwrite the buffer
        h->swOverrun = TRUE;
        h->TotalOverruns++;
    }
    m->targets = 0;

    if (h->ackErrCount >= MAX_ACK_ERR_COUNT) {
        BYTE cr;
        cr = READ_PORT_UCHAR(address + CAN_CTRL);
        cr |= CTRL_SIE;
        WRITE_PORT_UCHAR(address + CAN_CTRL, cr);
    }

    for (i=0; i<COUNTOF(h->handles); i++) {
        handleData *hnd = h->handles[i];
        if (hnd == NULL) continue;

        // Mark all handles as interested.
        // qqq someday, we'll provide soft filtering for the
        // individual masks.
        m->targets |= hnd->handleMask;
        if (hnd->rxFirstMsg == savedNextMsg) {
            if (hnd->notifyFlags & canNOTIFY_RX) {
                NOTIFY(hnd->hWnd, canEVENT_RX, hnd->nr, 0);
            }
        }
    }
}

PRIVATE void PCCAN_INTEL527_Transmit(circData* h, WORD address)
{
   bufData* b = h->buf;
   unsigned int i;

   // Kill our TX buffer ASAP.
   WRITE_PORT_UCHAR(CANmsg(address, I527_TX_BUF) + CANmsg_CTRL0,
                    CLR(MsgVal) & CLR(IntPnd));

   if (h->ackErrCount >= MAX_ACK_ERR_COUNT) {
       BYTE cr;
       cr = READ_PORT_UCHAR(address + CAN_CTRL);
       cr |= CTRL_SIE;
       WRITE_PORT_UCHAR(address + CAN_CTRL, cr);
   }
   //
   // Send the next message, if the transmit buffer is empty.
   //
   if (b->txMsgs != 0 ) {
      txMessage* t;
      i = b->txHead;
      t = &b->txBuffer[i];
      (void)PCCANstartWrite(h, t->id, t->data, t->dlc, t->flags);
      t->available = TRUE;
      i++;
      if (i >= b->txBufSize) i = 0;
      b->txHead = i;
      b->txMsgs--;
   }

   // Notify interested parties.
   // qqq Perhaps we should have a message-specific flag for this.
   // (I.e. "please tell me when this message is sent")
   for (i=0; i<COUNTOF(h->handles); i++) {
      handleData *hnd = h->handles[i];
      if (hnd == NULL) continue;

      if (hnd->notifyFlags & canNOTIFY_TX) {
         NOTIFY(hnd->hWnd, canEVENT_TX, hnd->nr, 0);
      }
   }

}

//
// Handle changes in the status register. This may be both
// error interrupts and status changes.
//
PRIVATE void PCCAN_INTEL527_Error(circData *h, WORD address)
{
    BYTE SR, CR;
    WORD newStatus, newRxErr, newTxErr;
    int i;
   
    SR = READ_PORT_UCHAR(address + CAN_STAT);
    CR = READ_PORT_UCHAR(address + CAN_CTRL);
    newStatus = statusToBusStatus_82527(SR, CR, &newRxErr, &newTxErr);

    switch (SR & 0x07) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            // ACK error
            // If we get more than 16 ACK errors, calm the
            // 527 down a bit.
            if (++h->ackErrCount < MAX_ACK_ERR_COUNT) break;
            CR = READ_PORT_UCHAR(address + CAN_CTRL);
            CR &= ~CTRL_SIE;
            WRITE_PORT_UCHAR(address + CAN_CTRL, CR);
            // qqq don't forget to re-enable SIE when
            // we have recieved or transmitted a msg.
            // qqq this isn't done now.
            break;
            
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
    }
    
    // Error frames?
    if ((SR & 0x07) != 0) {
        // LEC is != 0. There must - in essence - be error frames on the bus.
        // qqq report this
    }

    // Change of status?
    if (newStatus != h->busStatus
        || newRxErr != h->RXErrorCounter
        || newTxErr != h->TXErrorCounter) {

        h->busStatus = newStatus;
        h->RXErrorCounter = newRxErr;
        h->TXErrorCounter = newTxErr;

        for (i=0; i<COUNTOF(h->handles); i++) {
            handleData *hnd = h->handles[i];
            if (hnd == NULL) continue;
            if (hnd->notifyFlags & canNOTIFY_RX) {
                NOTIFY(hnd->hWnd, canEVENT_STATUS, hnd->nr, 0);
            }
        }
    }
}
