/*
**                        Copyright 1995,1996 by KVASER AB
**            P.O Box 4076 S-51104 KINNA, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This module contains the entry points to the library.
** The code typically performs tasks like parameter checking and
** conversion and then calls the appropriate hardware-dependent
** routines.  Also, all readings from the ring buffer take place here.
**
*/
#include <stdlib.h>
#include <string.h>
#include <dos.h>            // _enable, _disable
#include <assert.h>

#include <std.h>
#include <canlib.h>
// #include <util.h>
#include "global.h"
#include "irq.h"
#include "timer.h"
#include "version.h"
#include "misc.h"

#include "libpccan.h"
#include "libcanc.h"

// For lapCAN - not yet working.
// #include "hwLapCAN.h"
// #include "lpaction.h" // qqq lapcan!

#define VALIDATE(h) \
    if (!Initialized) return canERR_NOTINITIALIZED; \
    if (!IsHandleValidAndOpen(h)) return canERR_INVHANDLE

/*
** Forward declarations.
*/
#ifndef _WINDOWS
PRIVATE int LibMain(void);
#endif

/* These following two routines should be moved to util.c someday. */

/* The time values are read using GetMsTimer() which returns the timer value in ms.
** When it reaches 0x100000000/1193 = 3600140.23 ms or about one hour,
** 'ReadTimer()/TIMERFREQ' will wrap causing troubles when it is used in timeouts
** etc (where two times are simply subtracted). In theese cases, one should use
** the TimeDiff-function instead which works for time differences less than one
** hour
*/
#define TIMERPERIOD 3600140L
/*
** Calculates (t2-t1) mod TIMERPERIOD
*/
PRIVATE DWORD TimeDiff(DWORD t2, DWORD t1)
{
    // The times are DWORDs/1193, so they will fit in a signed long.
    long d = (long)t2 - (long)t1;
    if (d < 0)
        d += TIMERPERIOD;
    return d;
}

/* Timeout handling.
** Call it first with argument 0 to initialize it, then call it with the
** timeout, and if timeout occur, 1 is returned.
**
** To avoid a bug in the timer routines, two 'timeout/2' has to occur before
** it is considered valid. This means that at worst, only half of the timeout
** value will be used.
*/
PRIVATE int Timeout(DWORD timeout)
{
    static DWORD t0;
    DWORD t1;
    static int toCount;

    if (timeout == 0) {
        t0 = GETTIMER();
        toCount = 0;
    } else {
        t1 = GETTIMER();
        if (TimeDiff(t1, t0) > timeout/2) {
            if (toCount++)
                return 1;
            else
                t0 = GETTIMER(); // Try again
        }
    }
    return 0;
}

//
// canInitializeLibrary - an initialization entry point with
// a more sensible name.
//
void CANLIBAPI canInitializeLibrary( void)
{
    (void)canLocateHardware();
}

/*
** canLocateHardware - initializes the data structures using the information
** found in kvaser.ini. Called once, when the DLL initializes.
*/
canStatus CANLIBAPI canLocateHardware(void)
{
    char entry[] = "CARD0x";
    char card[20];
    int i, type;
    canStatus stat;

    // Has someone been here before us?
    if (Initialized) return canOK;
#ifndef _WINDOWS
    LibMain();
#endif

    // We search only for "CARD00" to "CARD09".
    for (i=0; i<=9; i++) {
        entry[5] = (char)(i + '0');
        (void)GetConfigFileString("CARDS", entry, "", card,
                                  sizeof(card), IniFile);
        if (strlen(card) != 0) {
            type = (int)GetConfigFileInt(card, "TYPE", canCARD_ANY, IniFile);
            switch (type) {
                case canCARD_ANY:
                    return canERR_INIFILE;

                case canCARD_PCCAN:
                case canCARD_PCCAN_OEM:
                    stat = PCCANLocateHardware(card);
                    if (CANSTATUS_FAILURE(stat)) return stat;
                    break;

                case canCARD_CANCARD:
                    stat = CANCLocateHardware(card);
                    if (CANSTATUS_FAILURE(stat)) return stat;
                    break;

                case canCARD_AC2:
                    // AC2 - not yet supported.
                    break;

                case canCARD_LAPCAN:
                    //stat = LAPCANLocateHardware(card);
                    //if (CANSTATUS_FAILURE(stat)) return stat;
                    return canERR_INIFILE;
                    break;

                case canCARD_ISACAN:
                    return canERR_INIFILE;


                default:
                    return canERR_INIFILE;
            }
        }
    }
    Initialized = TRUE;
    return canOK;
}


PRIVATE canStatus
   FindHardware(const canHWDescr FAR * const hwdescr,
                const canSWDescr FAR * const swdescr,
                const unsigned int flags,
                cardData **card,
                circData **circ,
                bufData  **buf)
{
   int i, found;
   cardData *c;
   circData *circuit;

   *card = NULL;
   *circ = NULL;
   *buf = NULL;

   // Search for a suitable card

   found = FALSE;
   c = Cards;
   for (i=0; i<COUNTOF(Cards); i++) {
       if (c->valid) {
           if (c->cardType == hwdescr->cardType
               || hwdescr->cardType == canCARD_ANY) {
               found = TRUE;
               break;
           }    
       }    
       c++;
   }

   if (!found) {
      return canERR_NOTFOUND;
   }

   // Find an available circuit

   found = FALSE;
   circuit = NULL;
   for (i=0; i<COUNTOF(c->Circuits); i++) {
      circuit = (circData*) c->Circuits[i];
      if (circuit && circuit->valid) {
         // Does the circuit type match?
         if (circuit->circuitType == hwdescr->circuitType
         || hwdescr->circuitType == canCIRCUIT_ANY) {
            // Anyone can open an unopened circuit.
            // An open circuit can be reopened only if canWANT_EXCLUSIVE
            // is NOT specified.
            if  ((!(circuit->circStatus & CIRCSTAT_OPEN)) ||
                     ((circuit->circStatus & CIRCSTAT_OPEN)
                     && (circuit->circStatus & CIRCSTAT_SHARED)
                     && (!(flags & canWANT_EXCLUSIVE)))) {
               // Does the channel match?
               if (hwdescr->channel == (int)circuit->channel
               || hwdescr->channel == canCHANNEL_ANY) {
                  // Do we require extended CAN?
                  if (((flags & canWANT_EXTENDED)
                     && (circuit->capabilities & canCIRC_EXTENDED))
                  || ((flags & canWANT_EXTENDED) == 0)) {
                     found = TRUE;
                     break;
                  }
               }
            }
         }
      }
      circuit++;
   }

   if (!found) {
      return canERR_NOTFOUND;
   }

   *card = c;
   *circ = circuit;
   *buf = circuit->buf;

   ARGUSED(swdescr);
   ARGUSED(flags);

   return canOK;
}

/*
** canOpen
** Opens a specific CAN circuit, returning a handle.
**
** Input:
**    canHWDescr *hwdescr   A struct describing the hardware; see
**                                 canLocateHw.
**    canSWDescr* swdescr   A struct describing buffer sizes and
**                                 such things. May be ommitted in which
**                                 case suitable(?) default values will
**                                 be supplied.
**    short flags                  Currently unused (intended for wantXXX)
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    >=0 : a handle to the opened circuit
*/
int CANLIBAPI canOpen(const canHWDescr FAR * const hwdescr,
                      const canSWDescr FAR * const swdescr,
                      const unsigned int flags)
{
   cardData* card;
   circData* circ;
   bufData*  buf;
   handleData* h;
   int hnd;
   canStatus stat;

   // Are the routines initialized ?
   if (!Initialized)
      return (int)canERR_NOTINITIALIZED;

   // Allocate a handle
   hnd = allocateHandle();
   if (hnd < 0) return (int)canERR_NOHANDLES;

   // Find appropriate data structures
   stat = FindHardware(hwdescr, swdescr, flags, &card, &circ, &buf);
   if (CANSTATUS_FAILURE(stat)) {
       deallocateHandle(hnd);
       return (int)stat;
   }

   // qqq
   card->desiredIRQ = card->defaultIRQ;
   // qqqLAPCAN card->desiredIRQ = assignedIRQ; // qqq Lapcan specific!!
 
   stat = attachCard(card, flags);
   if (CANSTATUS_FAILURE(stat)) {
       deallocateHandle(hnd);
       return (int)stat;
   }

   stat = attachCircuit(circ, flags);
   if (CANSTATUS_FAILURE(stat)) {
      (void)detachCard(card);
      deallocateHandle(hnd);
      return (int)stat;
   }

   if (!buf) {
      stat = CreateBuf(&buf, hwdescr, swdescr, flags);
      if (CANSTATUS_SUCCESS(stat)) circ->buf = buf;
   }
   if (CANSTATUS_SUCCESS(stat)) {
      stat = attachBuffer(buf);
      if (CANSTATUS_FAILURE(stat)) (void)detachBuffer(buf);
   }
   if (CANSTATUS_FAILURE(stat)) {
      (void)detachCircuit(circ);
      (void)detachCard(card);
      deallocateHandle(hnd);
      return (int)stat;
   }

   stat = setupHandle(hnd, card, circ, buf);
   if (CANSTATUS_FAILURE(stat)) {
      (void)detachBuffer(buf);
      (void)detachCircuit(circ);
      (void)detachCard(card);
      deallocateHandle(hnd);
      return (int)stat;
   }

   h = &Handle[hnd];
   if (flags & canWANT_EXTENDED) {
      h->options |= HOPTION_PREFER_EXTENDED;
      (void)circ->EnableExtended(circ, TRUE);
   }

   return hnd;
   // Note: had a sane person designed this call, canOpen would have
   // returned canINVALID_HANDLE on failure and a second call returning
   // the actual error code would have been provided.
   // I apologize.  / The Author.
}

/*
** canClose
** Closes a circuit previously opened with canOpen.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canClose(const int hnd)
{
   VALIDATE(hnd);

   (void)canWriteSync(hnd, 1000L);

   (void)dissolveHandle(hnd);
   (void)detachCircuit(Handle[hnd].circ);
   (void)detachBuffer(Handle[hnd].buf);
   (void)detachCard(Handle[hnd].card);
   deallocateHandle(hnd);
   return canOK;
}

/*
** canBusOn
** Puts an open circuit on-bus.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canBusOn(const int hnd)
{
   circData* c;
   VALIDATE(hnd);
   c = Handle[hnd].circ;
   Handle[hnd].rxFirstMsg = c->NextMsg;
   c->TXErrorCounter = 0;
   c->RXErrorCounter = 0;
   c->hwOverrun = FALSE;
   c->swOverrun = FALSE;
   c->TotalOverruns = 0;
   return c->BusOn(c);
}

/*
** canBusOff
** Takes an open circuit off-bus.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canBusOff(const int hnd)
{
   circData* h;
   VALIDATE(hnd);
   h = Handle[hnd].circ;
   return h->BusOff(h);
}

/*
** canBusParams
** Sets the bus parameters for an open circuit.
**
** Input:
**    int handle         The handle to the circuit
**    long freq          The bus frequency, i.e. 1/freq = the length
**                       of a bit. If the frequency is < 0, then
**                       certain default parameters are used
**                       (see canTranslateBaud.)
**    int tseg1          The number of quantas before the sampling
**                       point. The start bit is NOT counted.
**    int tseg2          The number of quantas after the sampling
**                       point.
**    int sjw            The Sync Jump Width (number of quantas - 1)
**    int nr_Samp        Number of sampling points, 1 or 3, usually 1
**    int syncmode       1=sync on falling&rising edge
**                       0=sync of falling edge only
**                       According to the CAN gurus,
**                       this parameter should always be 0.
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canSetBusParams(const int hnd,
                           long freq,
                           unsigned int tseg1,
                           unsigned int tseg2,
                           unsigned int sjw,
                           unsigned int noSamp,
                           unsigned int syncmode)
{
   circData* h;
   canStatus stat;
   VALIDATE(hnd);
   h = Handle[hnd].circ;

   if (freq<0) {
      stat = canTranslateBaud(&freq, &tseg1, &tseg2, &sjw, &noSamp, &syncmode);
      if (CANSTATUS_FAILURE(stat)) return stat;
   }

   if (freq == 0L) return canERR_PARAM;

   stat = h->SetBusParams(h, (unsigned long)freq, tseg1, tseg2,
                          sjw, noSamp, syncmode);
   if (CANSTATUS_FAILURE(stat)) return stat;

   h->freq = (unsigned long)freq;
   h->tseg1 = tseg1;
   h->tseg2 = tseg2;
   h->sjw = sjw;
   h->nosamp = noSamp;
   h->syncmode = syncmode;

   return canOK;
}

canStatus CANLIBAPI canGetBusParams(const int hnd,
                              long FAR * freq,
                              unsigned int FAR * tseg1,
                              unsigned int FAR * tseg2,
                              unsigned int FAR * sjw,
                              unsigned int FAR * noSamp,
                              unsigned int FAR * syncmode)
{
   circData* h;
   VALIDATE(hnd);
   h = Handle[hnd].circ;

   *freq = (long)h->freq;
   *tseg1 = h->tseg1;
   *tseg2 = h->tseg2;
   *sjw = h->sjw;
   *noSamp = h->nosamp;
   *syncmode = h->syncmode;

   return canOK;
}

/*
** canOutputControl
** Sets the specified driver type for an open circuit.
** "Driver type" is such things as open collector, push-pull etc.
**
** Input:
**    int handle                 The handle to the circuit
**    int drivertype             The driver type, any of canOFF, canPUSHPULL,
**                               et al; see canlib.h
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canSetBusOutputControl(const int hnd,
                                  const unsigned int drivertype)
{
   canStatus stat;
   circData* circ;

   VALIDATE(hnd);
   circ = Handle[hnd].circ;
   stat = circ->SetOutputControl(circ, drivertype);
   if (CANSTATUS_FAILURE(stat)) return stat;

   circ->driver = drivertype;
   return canOK;
}

canStatus CANLIBAPI canGetBusOutputControl(const int hnd,
                                     unsigned int FAR * drivertype)
{
   circData* circ;

   VALIDATE(hnd);
   circ = Handle[hnd].circ;
   *drivertype = circ->driver;
   return canOK;
}


/*
** canAccept
** Sets the acceptance mask for an open circuit.  The implementation is such
** that no "acceptable" message may be rejected, but a "rejectable" message
** may be accepted. There are also provisions for setting the mask and
** code directly.
**
** Input:
**    int handle           The handle to the circuit
**    long envelope        CAN id to accept or reject
**    int flag             One of
**                            canACCEPT - accept the envelope
**                            canREJECT - reject the envelope
**                            canMASK - set the acceptance mask directly
**                            canCODE - set the acceptance code directly
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canAccept(const int hnd,
                        const long envelope,
                        const unsigned int flag)
{
   circData* circ;
   VALIDATE(hnd);
   circ = Handle[hnd].circ;
   return circ->SetMask(circ, envelope, flag);
}

/*
** canReadStatus
** Returns the status for the specified circuit. Note that the format of
** the status word is currently unspecified and therefore circuit-dependent.
** This will probably change in the future.
**
** Input:
**    int handle                    The handle to the circuit
**    long *flags                   Pointer to a longword into which the
**                                  status flags will be written.
**
** Output:
**    long *flags                   The status flags.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canReadStatus(const int hnd,
                        unsigned long FAR * const flags)
{
   circData* c;
   handleData* h;
   canStatus stat;
   
   VALIDATE(hnd);
   h = &Handle[hnd];
   c = h->circ;
   stat = c->GetStatus(c, flags);

   if (stat == canOK) {
       bufData *b;
       b = c->buf;
       if (h->rxFirstMsg != c->NextMsg) *flags |= canSTAT_RX_PENDING;
       if (b->txMsgs > 0) *flags |= canSTAT_TX_PENDING;
   }
   return stat;
}

/*
** canReadErrorCounters
** Returns the error counters for the specified circuit.
**
** Input:
**    int handle                    The handle to the circuit
**    int *txErr                    Pointer to an int to which the number of
**                                  transmit errors will ber written.
**    int *rxErr                    Pointer to an int to which the number of
**                                  receive errors will ber written.
**    int *ovErr                    Pointer to an int to which the number of
**                                  overrun errors will ber written.
**
** Output:
**    int *txErr                    See above.
**    int *rxErr                    See above.
*     int *ovErr                    See above.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canReadErrorCounters(int hnd,
                               unsigned int FAR * txErr,
                               unsigned int FAR * rxErr,
                               unsigned int FAR * ovErr)
{
   circData* h;
   VALIDATE(hnd);
   h = Handle[hnd].circ;

   DISABLE_INTERRUPTS;

   if (txErr) {
      *txErr = h->TXErrorCounter;
   }

   if (rxErr) {
      *rxErr = h->RXErrorCounter;
   }

   if (ovErr) {
      *ovErr = h->TotalOverruns;
   }
   
   ENABLE_INTERRUPTS;

   return canOK;
}

/*
** Internal routine.  Appends a message to the transmit queue.
*/
PRIVATE canStatus appendToTxQueue(int hnd, long id, void* msgPtr,
                            unsigned int dlc, unsigned int flag)
{
   bufData* h = Handle[hnd].buf;

   unsigned int i = h->txTail;
   txMessage* p = &h->txBuffer[i];
   BYTE* q = p->data;
   BYTE* msg = (BYTE*)msgPtr;

   if (h->txMsgs >= h->txBufSize || !p->available) {
      return canERR_TXBUFOFL;   // Transmit buffer overflow!
   }

   p->id        = id;
   p->dlc       = dlc;
   p->flags     = flag;
   p->available = FALSE;
   
   if (msg) {
      switch (dlc) {
         case 8:
            *q++ = *msg++;
            // Fall through
         case 7:
            *q++ = *msg++;
            // Fall through
         case 6:
            *q++ = *msg++;
            // Fall through
         case 5:
            *q++ = *msg++;
            // Fall through
         case 4:
            *q++ = *msg++;
            // Fall through
         case 3:
            *q++ = *msg++;
            // Fall through
         case 2:
            *q++ = *msg++;
            // Fall through
         case 1:
            *q++ = *msg++;
            // Fall through
         case 0:
         default:
            ;
      }
   }

   i++;
   if (i>=h->txBufSize) i = 0;
   h->txTail = i;
   h->txMsgs++;

   return canOK;
}


/*
** canWrite
** Queues a message for sending.
**
** Input:
**    int handle                    The handle to the circuit
**    long id                       The CAN id to use
**    void* msg                     Pointer to the data to send
**    int dlc                       The length of the message
**    int flag                      canFLAG_RTR : send an RTR.
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canWrite(int hnd, long id, void FAR * msg,
                       unsigned int dlc, unsigned int flag)
{
   circData* h;
   canStatus result = canOK;
   unsigned int timeout = TIMEOUT * Timeout_1ms;
   bufData* b;

   VALIDATE(hnd);
   h = Handle[hnd].circ;
   b = Handle[hnd].buf;

   if (dlc > 8) {
      return canERR_PARAM;
   }

   // Set the std/ext flag in case it is omitted
   if (!(flag & (canMSG_STD|canMSG_EXT))) {
      flag |= ((Handle[hnd].options & HOPTION_PREFER_EXTENDED)? canMSG_EXT : canMSG_STD);
   }

    if (b->txBufSize == 0) {
        while ((!h->TXAvailable(h)) && (--timeout != 0)) {
            // Nothing
        }
        if (h->TXAvailable(h)) {
            result = h->StartWrite(h, id, msg, dlc, flag);
        } else {
            result = canERR_TIMEOUT;
        }
    } else {
      // Check if queue is empty
      // We have to disable interrupts so that the possibly busy
      // CAN-transmitter doesn't interrupt before we have time to
      // append to it.
      DISABLE_INTERRUPTS;
      if (b->txMsgs == 0) {
            if (h->TXAvailable(h)) {
                // Available, write directly to it. It is ok to enable interrupts
                // as the transmit-interrupt will find an empty queue and do
                // nothing.
                ENABLE_INTERRUPTS;
                result = h->StartWrite(h, id, msg, dlc, flag);
            } else {
                // Not available, write to queue instead
                result = appendToTxQueue(hnd, id, msg, dlc, flag);
                ENABLE_INTERRUPTS;
            }
      } else {
            // TX queue isn't empty, add to queue
            result = appendToTxQueue(hnd, id, msg, dlc, flag);
            ENABLE_INTERRUPTS;
      }
    }

    return result;
}

/*
** canWriteSync
** Wait until the specified channel has actually sent all of its
** pending messages.
**
** Input:
**    int handle                    The handle to the circuit
**    unsigned long timeout         Timeout in ms
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canWriteSync(int hnd, unsigned long timeout)
{
    circData* h;
    bufData* b;

    VALIDATE(hnd);
    h = Handle[hnd].circ;
    b = Handle[hnd].buf;

    Timeout(0);

    // Wait for the transmit ring buffer (if any) to empty
    if (b->txBufSize != 0) {
      while (b->txMsgs) {
        if (timeout == 0 || Timeout(timeout)) 
          return canERR_TIMEOUT;
      }
    }

    // The transmit ring buffer is empty (or non-existant.)
    // Wait for the tx buffer on the chip to become available.
    while(!h->TXAvailable(h)) {
        if (timeout == 0 || Timeout(timeout))
            return canERR_TIMEOUT;
    }

    return canOK;
}

// qqq These macros need more work.
#if !defined(NDEBUG) && !defined(_MSC_VER)
#define DEBUGCODE( code) code
#else
#define DEBUGCODE( code)
#endif

/* Help routine for canRead() and canReadSpecific().
**
** Looks for and gets a message from the receive buffer.
** If timeout != 0, we wait for this long (in ms); if nothing is received
** canERR_TIMEOUT is returned, otherwise canERR_NOMSG.
** If a message is received, canOK is returned. Before that and if
** !keepMsg, the message is removed from the buffer and assigned to the
** arguments.
*/
PRIVATE canStatus canReadHelp(int hnd, long idWanted, long *id,
                              void* msgPtr,
                              unsigned int* dlc, unsigned int* flag,
                              unsigned long* time, unsigned long timeout,
                              unsigned int keepMsg)
{
   bufData       *b;
   unsigned int  i, j, rxbufsize, len, hndmask;
   Message       *buf, *m;
   BYTE          *msg, *p;
   handleData    *h;
   circData      *circ;
   int           found;
   // int wantMeta;
   canStatus     stat;
   int           skippingMessages;

   VALIDATE(hnd);

   msg = (BYTE*)msgPtr;
   stat = canOK;
   h = &Handle[hnd];
   b = h->buf;
   circ = h->circ;
   hndmask = (1U<<hnd);

   rxbufsize = b->rxBufSize;
   buf = b->rxBuffer;

   found = FALSE;
   // wantMeta = h->options & HOPTION_WANT_METAMSGS;

   Timeout(0);
   
   for(;;) {
      DEBUGCODE(int msgCount;)
      DISABLE_INTERRUPTS;
      DEBUGCODE(msgCount = 0;)
      // skippingMessages is TRUE when we are reading the buffer
      // and encounter messages that are not for us. As soon as we
      // find a message that IS for us but which we decide to keep
      // as we are not interested in it this time, skippingMessages
      // are set to FALSE.
      skippingMessages = TRUE;

      i = h->rxFirstMsg;
      j = circ->NextMsg;
      m = &buf[i];

      if (i != j) {
         for(;;) {
            
            if (i == j) {
                // We were about to check the position which the interrupt routine
                // are waiting to write to. We have reached the end.
                
                DEBUGCODE(if (msgCount == 0 && h->rxFirstMsg != circ->NextMsg))
                DEBUGCODE(asm nop;) // Debug.
                break;
            }

            // i and m point to first message item worth checking.
            i++;
            if (i>=rxbufsize) {
                i = 0;
            }
            // Now, i points to the next item to consider, but m is kept.

DEBUGCODE(if (m->targets))
DEBUGCODE(  msgCount++;)

            if (m->targets & hndmask) {
                // The message is for us...
                if (idWanted == CANID_WILDCARD || m->id == idWanted) {
                    // .. AND we are interested this time

                    //
                    // - but first
                    // we must check if it's a meta message (error frames,
                    // status changes etc) and if we want such messages.
                    //if (m->id != CANID_METAMSG || wantMeta) {
                    //    found = TRUE;
                    //}
                    //
                    
                    found = TRUE;
                    if (!keepMsg && skippingMessages) {
                        // We don't have to keep the message AND we are
                        // updating our message pointer
                        h->rxFirstMsg = i;
                    }
                    if (found) break; // We found something!
                }
                skippingMessages = FALSE;
            } else {
                // The message is not for us...
                if (skippingMessages) {
                    // ...update our message pointer.
                    h->rxFirstMsg = i;
                }
            }

            if (i == 0) {
                m = buf;
            } else {
                m++;
            }
         }
      }

      if (found || timeout == 0 || Timeout(timeout)) {
          break;
      }
      ENABLE_INTERRUPTS;
   }

   if (!found) {
      ENABLE_INTERRUPTS;
      if (timeout) {
         return canERR_TIMEOUT;
      } else {
         return canERR_NOMSG;
      }
   }

   if (keepMsg) {
      // We should not remove the message from the buffer
      ENABLE_INTERRUPTS;
      return canOK;
   }

   m->overwritten = FALSE;
   ENABLE_INTERRUPTS;

   if (id) *id = m->id;
   len = min(m->dlc, 8);
   if (dlc) *dlc = len;
   p = m->data;

   if (msg) {
      switch (len) {
      case 8:
         *msg++ = *p++;
         // Fall through
      case 7:
         *msg++ = *p++;
         // Fall through
      case 6:
         *msg++ = *p++;
         // Fall through
      case 5:
         *msg++ = *p++;
         // Fall through
      case 4:
         *msg++ = *p++;
         // Fall through
      case 3:
         *msg++ = *p++;
         // Fall through
      case 2:
         *msg++ = *p++;
         // Fall through
      case 1:
         *msg++ = *p++;
         // Fall through
      case 0:
      default:
         ;
      }
   }

   // Mark if buffer overrun, hw overrun, hw error eller wakeup
   // has occured.
   DISABLE_INTERRUPTS;
   if (flag) {
       *flag = m->flags;
       if (circ->hwOverrun) *flag |= canMSGERR_HW_OVERRUN;
       if (circ->swOverrun) *flag |= canMSGERR_SW_OVERRUN;
   }
   circ->hwOverrun = circ->swOverrun = FALSE;
   ENABLE_INTERRUPTS;

   if (time) {
      *time = m->time;
   }

   DISABLE_INTERRUPTS;
   if (m->overwritten) {
       // This message was overwritten just as we were reading it.
       // The contents is most probably corrupted.
       // qqq Is this really a reasonable return code?
       stat = canERR_NOMSG;
   } else {
       m->targets &= ~hndmask;
   }
   ENABLE_INTERRUPTS;

   return stat;
}


/*
** canRead
** Reads a message from the receive buffer.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    long* id                      Pointer to the id
**    void* msg                     Pointer to the message data
**    int* dlc                      Pointer to the message length
**    int* flag                     Pointer to a flag word.
**                                  May contain any of the CANFLAG_xxx flags.
**    unsigned long* time           Pointer to the time stamp (in ms)
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canRead(int hnd,
                      long FAR * id,
                      void FAR * msgPtr,
                      unsigned int FAR * dlc,
                      unsigned int FAR * flag,
                      unsigned long FAR * time)
{
  return canReadHelp(hnd, CANID_WILDCARD, id, msgPtr, dlc, flag, time, 0L, 0);
}

/*
** canReadSpecific
** Reads a message with a specified id from the receive buffer. Any
** preceeding message not matching the specified identifier will be kept in
** the receive buffer.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    msg
**    dlc
**    flag
**    time
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/


canStatus CANLIBAPI canReadSpecific(int hnd,
                              long idWanted,
                              void FAR * msg,
                              unsigned int FAR * dlc,
                              unsigned int FAR * flag,
                              unsigned long FAR * time)
{
  return canReadHelp(hnd, idWanted, NULL, msg, dlc, flag, time, 0L, 0);
}


/*
** canReadWait
** Reads a message from the receive buffer. If no message is available
** canReadWait waits until a message (witn any id) is available or a timeout
** occurs.
**
** Input:
**    int handle                    The handle to the circuit
**    unsigned long timeout
**
** Output:
**    msg
**    dlc
**    flag
**    time
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canReadWait(int hnd,
                          long FAR * id,
                          void FAR * msg,
                          unsigned int FAR * dlc,
                          unsigned int FAR * flag,
                          unsigned long FAR *time,
                          unsigned long timeout)
{
  return canReadHelp(hnd, CANID_WILDCARD, id, msg, dlc, flag, time, timeout, 0);
}

/*
** canReadSpecificSkip
** Reads a message with a specified id from the receive buffer. Any
** preceeding message not matching the specified identifier will be skipped.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    msg
**    dlc
**    flag
**    time
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canReadSpecificSkip(int hnd,
                                  long id,
                                  void FAR * msg,
                                  unsigned int FAR * dlc,
                                  unsigned int FAR * flag,
                                  unsigned long FAR * time)
{
   canStatus stat;
   long actual_id;

   do {
      stat = canReadHelp(hnd, CANID_WILDCARD, &actual_id, msg, dlc, flag, time, 0L, 0);
      if (stat == canOK && id == actual_id) {
         break;
      }
   } while (CANSTATUS_SUCCESS(stat));

   return stat;
}


/*
** canReadSync
** Waits until the receive buffer contains at least one message or a timeout
** occurs.
**
** Input:
**    int handle                    The handle to the circuit
**    unsigned long timeout
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canReadSync(int hnd, unsigned long timeout)
{
  return canReadHelp(hnd, CANID_WILDCARD, NULL, NULL, NULL, NULL, NULL, timeout, 1);
}

/*
** canReadSyncSpecific
** Waits until the receive buffer contains a message with the specified
** id, or a timeout occurs.
**
** Input:
**    int handle                    The handle to the circuit
**    long id
**    unsigned long timeout
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canReadSyncSpecific(int hnd, long id, unsigned long timeout)
{
  return canReadHelp(hnd, id, NULL, NULL, NULL, NULL, NULL, timeout, 1);
}

/*
** canInstallAction
** Installs an action routine which is called at interrupt level when a
** message with the specified id arrives.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canInstallAction(int hnd, long id, int ( FAR *fn)())
{
   VALIDATE(hnd);
   ARGUSED(hnd);
   ARGUSED(id);
   ARGUSED(fn);
   return canERR_PARAM;
}

/*
** canUninstallAction
** Uninstalls an action routine previously installed
** with canInstallAction.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canUninstallAction(int hnd, long id)
{
   VALIDATE(hnd);
   ARGUSED(id);
   return canERR_PARAM;
}

/*
** canInstallOwnBuffer
** Installs a buffer to which the receive interrupt routine will write any
** message with the specified id.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canInstallOwnBuffer(int hnd, long id, unsigned int len, void FAR * buf)
{
   VALIDATE(hnd);
   ARGUSED(id);
   ARGUSED(len);
   ARGUSED(buf);
   return canERR_PARAM;
}

/*
** canUninstallOwnBuffer
** Uninstalls a buffer installed previously with canInstallOwnBuffer.
**
** Input:
**    int handle                    The handle to the circuit
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canUninstallOwnBuffer(int hnd, long id)
{
   VALIDATE(hnd);
   ARGUSED(id);
   return canERR_PARAM;
}

/*
** canSetNotify
** Specifies that a notification message be sent to the specified window
** when a message with the specified id arrives. In DOS, the window handle
** is a function pointer instead; the corresponding function will be called
** when the message arrives.
**
** Input:
**    int handle                    The handle to the circuit
**    HWND aHWnd
**    int  aNotifyFlags
**
** Output:
**    None.
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canSetNotify(int hnd, HWND aHWnd, unsigned int aNotifyFlags)
{
   handleData* h;

   VALIDATE(hnd);
   h = &Handle[hnd];

   DISABLE_INTERRUPTS;
   h->hWnd = aHWnd;
   h->notifyFlags = aNotifyFlags;
   ENABLE_INTERRUPTS;

   return canOK;
}

#if defined(_WINDOWS) && defined(_MSC_VER)
// Here's an identical entry point for thunking.
canStatus CANLIBAPI canSetNotify_32(int hnd, HWND aHWnd, unsigned int aNotifyFlags)
{
    return canSetNotify(hnd, aHWnd, aNotifyFlags);
}
#endif

/*
** LibMain
** Library entry point; called automatically when the DLL loads. In DOS
** this routine must be called manually and before any canXXX routine
** is used. This routine can't fail.
**
** Input:
**    None.
**
** Output:
**    None.
**
** Returns:
**    1, to indicate success.
*/
/* Library entry point */
#ifdef _WINDOWS
#ifdef LINT
int FAR PASCAL
#else
int _export WINAPI
#   endif
LibMain (HANDLE hInstance,
         WORD wDataSeg,
         WORD cbHeapSize,
         LPSTR lpCmdLine)
{
   ARGUSED(hInstance);
   ARGUSED(wDataSeg);
   ARGUSED(cbHeapSize);
   ARGUSED(lpCmdLine);
#else
PRIVATE int LibMain(void)
{
#endif
   if (!LibMainCalled) {
      IRQInit();
      MiscInit();
      TimerInit();
      (void)GlobalInit();
      (void)PCCANlibInit();
      (void)CANClibInit();
   }

   LibMainCalled = TRUE;
   
   return 1;  // Report success to Windows
}


/*
** WEP
** The library deinstallation routine. Called automatically when the DLL
** unloads. In DOS, it must be called manually.
** (It is called automatically by Borland's WEP.)
**
*/
#if defined(_WINDOWS)
#   if defined(_MSC_VER)
        int WINAPI _WEP (int nParameter)        // For MSC
#   else
        int WINAPI WEP (int nParameter)         // For BCC
#   endif                
#else
    int WEP(int nParameter)        // For DOS
#endif
{
   ARGUSED(nParameter);
   if (LibMainCalled) {
       int i;

       // Muffle all cards
       for (i=0; i<COUNTOF(Cards); i++) {
           cardData* c = &Cards[i];
           if (c->valid && c->refCount > 0 && c->KillCard)
               (void)c->KillCard(c);
       }

       IRQExit();
       (void)PCCANlibExit();
       (void)CANClibExit();
       MiscExit();
       TimerExit();
   }
   
   Initialized = FALSE;
   LibMainCalled = FALSE;

   return 1; // Always return success
}

#if defined(_WINDOWS) && defined(_MSC_VER)
//
// Thunk interface routines. Used in conjunction with MSC only.
//
extern BOOL FAR PASCAL
   thk_ThunkConnect16(LPSTR      pszParm16,
                      LPSTR      pszParm32,
                      HINSTANCE  hInst,
                      DWORD      dwReason);

/*
 ** Here's the entry point when thunking from 32 bits.
 */
BOOL FAR PASCAL __export DllEntryPoint(DWORD dwReason,
                                       WORD  hInst,
                                       WORD  wDS,
                                       WORD  wHeapSize,
                                       DWORD dwReserved1,
                                       WORD  wReserved2)
{
    if (!thk_ThunkConnect16("CANLIB.DLL",
                            "CANWRP32.DLL",
                            hInst,
                            dwReason)) {
        return FALSE;
    }
    return TRUE;
}
#endif /* defined(_WINDOWS) && defined(_MSC_VER) */

/*
** canTranslateBaud
** Translate the constants BAUD_xxx to appropriate register values.
**
** Input:
**    long freq
**
** Output:
**    long freq,
**    unsigned int tseg1
**    unsigned int tseg2
**    unsigned int sjw
**    unsigned int nosamp
**    unsigned int syncMode
**
** Returns:
**    <0 : error, any of canERR_xxx
**    canOK
*/
canStatus CANLIBAPI canTranslateBaud(long FAR * const freq,
                               unsigned int FAR * const tseg1,
                               unsigned int FAR * const tseg2,
                               unsigned int FAR * const sjw,
                               unsigned int FAR * const nosamp,
                               unsigned int FAR * const syncMode)
{
   switch (*freq) {
      case BAUD_1M:
         *freq     = 1000000L;
         *tseg1    = 4;
         *tseg2    = 3;
         *sjw      = 1;
         *nosamp   = 1;
         *syncMode = 0;
         break;
      case BAUD_500K:
         *freq     = 500000L;
         *tseg1    = 4;
         *tseg2    = 3;
         *sjw      = 1;
         *nosamp   = 1;
         *syncMode = 0;
         break;
      case BAUD_250K:
         *freq     = 250000L;
         *tseg1    = 4;
         *tseg2    = 3;
         *sjw      = 1;
         *nosamp   = 1;
         *syncMode = 0;
         break;
      case BAUD_125K:
         *freq     = 125000L;
         *tseg1    = 10;
         *tseg2    = 5;
         *sjw      = 1;
         *nosamp   = 1;
         *syncMode = 0;
         break;
      case BAUD_100K:
         *freq     = 100000L;
         *tseg1    = 10;
         *tseg2    = 5;
         *sjw      = 1;
         *nosamp   = 1;
         *syncMode = 0;
         break;
      case BAUD_62K:
         *freq     = 62500L;
         *tseg1    = 10;
         *tseg2    = 5;
         *sjw      = 1;
         *nosamp   = 1;
         *syncMode = 0;
         break;
      case BAUD_50K:
         *freq     = 50000L;
         *tseg1    = 10;
         *tseg2    = 5;
         *sjw      = 1;
         *nosamp   = 1;
         *syncMode = 0;
         break;
      default:
         return canERR_PARAM;
   }
   return canOK;
}

/*
** canGetErrorText
**
** Input:
**
** Output:
**
** Returns:
**
*/
canStatus CANLIBAPI canGetErrorText(canStatus err,
                                    char FAR * buf,
                                    unsigned int bufsiz)
{
   char *s = NULL;
   int errInt = (int)err;

   // If someone's treating enums as bytes, cut away the upper 8 bits.
   errInt = (int)(errInt & 0xFF);

   if (!buf) return canERR_PARAM;

   switch (err) {
      case canOK:                   s = "OK"; break;
      case canERR_PARAM:            s = "Error in parameter"; break;
      case canERR_NOMSG:            s = "No messages"; break;
      case canERR_NOTFOUND:         s = "Specified device not found"; break;
      case canERR_NOMEM:            s = "Out of memory"; break;
      case canERR_NOCHANNELS:       s = "No channels avaliable"; break;
      case canERR_BUFOFL:           s = "Buffer overflow"; break;
      case canERR_TIMEOUT:          s = "Timeout occurred"; break;
      case canERR_NOTINITIALIZED:   s = "Library not initialized"; break;
      case canERR_NOHANDLES:        s = "Can't get handle"; break;
      case canERR_INVHANDLE:        s = "Handle is invalid"; break;
      case canERR_INIFILE:          s = "Error in the ini-file"; break;
      case canERR_DRIVER:           s = "Driver type not supported"; break;
      case canERR_TXBUFOFL:         s = "Transmit buffer overflow"; break;
//    case canERR_OVERWRITE:        s = "The message was overwritten"; break;
      case canERR_HARDWARE:         s = "A hardware error was detected"; break;
      case canERR_DYNALOAD:         s = "Can not find requested DLL"; break;
      case canERR_DYNALIB:          s = "DLL seems to be wrong version"; break;
      case canERR_DYNAINIT:         s = "Error initializing DLL"; break;
      case canERR_REINIT:           s = "Reinitialization failed"; break;
      default:
      {
          char s[64];
#ifdef _WINDOWS
          wsprintf(s, "Unknown error (%d)", errInt);
#else
          strcpy(s, "Unknown error");
#endif
          strncpy(buf, s, (unsigned)bufsiz);
          return canOK;
      }
   }

   strncpy(buf, s, (unsigned)bufsiz);
   if (bufsiz > 0) buf[bufsiz-1] = '\0';

   return canOK;
}

/*
** canGetVersion
**
** Input:
**
** Output:
**
** Returns:
**
*/
unsigned short CANLIBAPI canGetVersion(void)
{
   return (MAJOR_VERSION<<8) + MINOR_VERSION;
}

/*
** canGetCircuits
**
** Input:
**
** Output:
**
** Returns:
**
*/
canStatus CANLIBAPI canGetCircuits(int FAR * context,
                             char FAR * name,
                             char FAR * vendor,
                             char FAR * version,
                             unsigned int FAR * cardtype,
                             unsigned int FAR *circtype,
                             unsigned int FAR * channel)
{
   int tmp;
   cardData* card;
   circData* circ;

   if (!context) return canERR_PARAM;
   if (*context < 0 || *context >= COUNTOF(Circuits)) {
      *context = -1;
      return canERR_PARAM;
   }

   tmp = *context;
   circ = &Circuits[tmp];

   if (!circ->valid) {
      *context = -1;
      return canERR_PARAM;
   }

   (*context)++;

   card = (cardData*)circ->card;
   if (cardtype) *cardtype = card->cardType;
   if (circtype) *circtype = circ->circuitType;
   if (channel) *channel = circ->channel;
   if (name) strcpy(name, card->name);
   if (vendor) strcpy(vendor, card->vendor);
   if (version) strcpy(version, card->version);

   return canOK;
}

/*
** canIoCtl
**
** Input:
**
** Output:
**
** Returns:
**
*/
canStatus CANLIBAPI canIoCtl(int handle,
                       unsigned int func,
                       void FAR * buf,
                       unsigned int buflen)
{
   handleData* h;
   circData* c;
   canStatus stat = canOK;
   
   VALIDATE(handle);
   h = &Handle[handle];
   c = h->circ;
   ARGUSED(buf);
   ARGUSED(buflen);

   switch (func) {
       case canIOCTL_PREFER_EXT:
           h->options |= HOPTION_PREFER_EXTENDED;
           stat = c->EnableExtended(c, TRUE);
           break;

       case canIOCTL_PREFER_STD:
           h->options &= ~HOPTION_PREFER_EXTENDED;
           stat = c->EnableExtended(c, FALSE);
           break;

       case canIOCTL_LOCAL_ECHO_ON:
           // qqq Not implemented yet.
           // h->options |= HOPTION_LOCAL_ECHO;
           break;

       case canIOCTL_LOCAL_ECHO_OFF:
           // qqq Not implemented yet.
           // h->options &= ~HOPTION_LOCAL_ECHO;
           break;

       case canIOCTL_CLEAR_ERROR_COUNTERS:
           c->TXErrorCounter = 0;
           c->RXErrorCounter = 0;
           c->TotalOverruns = 0;
           c->hwOverrun = FALSE;
           c->swOverrun = FALSE;
           c->txError = FALSE;
           c->rxError = FALSE;
           break;

       default:
           stat = canERR_PARAM;
           break;
   }
   return stat;
}

/*
** canReadTimer
** Returns the same time scale as is stored in the CAN messages.
**
** Input:
**
** Output:
**
** Returns:
**
*/
unsigned long CANLIBAPI canReadTimer(void)
{
    return GETTIMER();
}

#if defined(_WINDOWS) && defined(_MSC_VER)
// Used when thunking.
unsigned long CANLIBAPI canReadTimer_32(void)
{
    return GETTIMER();
}
#endif

unsigned long CANLIBAPI canReadTimerEx(int handle)
{
    ARGUSED(handle);
    return GETTIMER();
}
