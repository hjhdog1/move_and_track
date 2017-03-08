/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This module contains global data structures and routines for their
** maintenance.
*/

#include <stdlib.h>
#include <memory.h>
#include <dos.h>

#include "std.h"
#include "canlib.h"
#include "timer.h"
#include "global.h"
#include "irq.h"

/*
** Here are the handles used by most of the routines. Each time canOpen
** is called, one of these is returned.
*/
PUBLIC handleData Handle[16];
PUBLIC circData Circuits[16];
PUBLIC cardData Cards[4];
PUBLIC bufData  BufferHeads[16];
PUBLIC unsigned int Timeout_1ms;  // This many (almost) empty loop turns will take 1 ms

// Initialized is TRUE if the library has been initialized
PUBLIC int Initialized = FALSE;
PUBLIC int LibMainCalled = FALSE;

PUBLIC canStatus GlobalInit(void)
{
   unsigned int i;
   unsigned long startTime, now;
   unsigned int timeout;

   memset(Handle, 0, sizeof(Handle));
   memset(Cards, 0, sizeof(Cards));
   memset(Circuits, 0, sizeof(Circuits));
   memset(BufferHeads, 0, sizeof(BufferHeads));
   
   // Calculate a suitable timeout limit.
   timeout = 0;
   startTime = GETTIMER();
   do {
      for (i=0; i<500; i++) timeout++;
      now = GETTIMER();
   } while (now - startTime < 10 && timeout != 65000U);
   timeout /= 10;

   Timeout_1ms = timeout;

   return canOK;
}


PUBLIC int IsHandleValid(int hnd)
{
   return (hnd >= 0
           && hnd < COUNTOF(Handle)
           && Handle[hnd].options != 0);
}

PUBLIC int IsHandleValidAndOpen(int hnd)
{
   return (hnd >= 0
           && hnd < COUNTOF(Handle)
           && Handle[hnd].options != 0
           && (Handle[hnd].circ->circStatus & CIRCSTAT_OPEN));
}

PUBLIC cardData * allocateCard(void)
{
    int i;

    for (i=0; i<COUNTOF(Cards); i++) {
        if (!Cards[i].valid) {
            memset(&Cards[i], 0, sizeof(Cards[i]));
            Cards[i].valid = TRUE;
            return &Cards[i];
        }
    }
    return NULL;
}

PUBLIC circData * allocateCircuit(void)
{
    int i;

    for (i=0; i<COUNTOF(Circuits); i++) {
        if (!Circuits[i].valid) {
            memset(&Circuits[i], 0, sizeof(Circuits[i]));
            Circuits[i].valid = TRUE;
            return &Circuits[i];
        }
    }
    return NULL;
}

/*
** Attaches to the specified circuit.
*/
PUBLIC canStatus attachCircuit(circData* h, unsigned int flag)
{
   canStatus stat = canOK;
   if (h->refCount == 0) {
       h->circStatus = CIRCSTAT_OPEN;
       if (!(flag & canWANT_EXCLUSIVE)) h->circStatus |= CIRCSTAT_SHARED;
       stat = h->InitCircuit(h);
       if (stat == canOK) h->refCount++;
   }
   return stat;
}

/*
** Detaches from the specified circuit.
*/
PUBLIC canStatus detachCircuit(circData* h)
{
   canStatus stat = canOK;
   if (h->refCount == 1) {
      h->circStatus &= ~(CIRCSTAT_OPEN | CIRCSTAT_SHARED);
      stat = h->KillCircuit(h);
      h->refCount--;
   }
   return stat;
}

//
// Attaches to the specified card. Returns TRUE if noone else
// has been here before us.
//
PUBLIC canStatus attachCard(cardData* h, unsigned int flag)
{
   canStatus stat = canOK;

   ARGUSED(flag);
   if (h->refCount == 0) {
       if (h->desiredIRQ < 0) {
           stat = canERR_PARAM;
       } else {
           stat = activateIRQ((WORD)h->desiredIRQ,
                              (InterruptRoutine)h->IntRtn,
                              (void*)h);
           if (CANSTATUS_SUCCESS(stat)) {
               h->usedIRQ = h->desiredIRQ;
               stat = h->InitCard(h);
               if (CANSTATUS_SUCCESS(stat)) {
                   h->status |= canCARD_INIT;
                   h->refCount++;
               }
           } else {
               (void)deactivateIRQ((WORD)h->desiredIRQ);
           }
       }
   }
   return stat;
}

//
// Detaches from the specified card. Returns TRUE if the card becomes
// free.
//
PUBLIC canStatus detachCard(cardData* h)
{
   canStatus stat = canOK;
   if (h->refCount == 1) {
      if (h->status & canCARD_INIT) stat = h->KillCard(h);

      if (CANSTATUS_SUCCESS(stat) && h->usedIRQ >= 0) {
         stat = deactivateIRQ((WORD)h->usedIRQ);
         h->usedIRQ = -1;
      }
      h->status &= ~canCARD_INIT;
      h->refCount--;
   }
   return stat;
}

//
// Allocate a handle.
//
PUBLIC int allocateHandle(void)
{
    unsigned int i;
    for (i=0; i<COUNTOF(Handle); i++) {
        if (!Handle[i].options) {
            memset(&Handle[i], 0, sizeof(Handle[i]));
            Handle[i].handleMask = (1<<i);
            Handle[i].options |= HOPTION_IS_USED;
            Handle[i].nr = i;
            return (int)i;
        }
    }
    return canINVALID_HANDLE;
}

//
// Deallocate a handle.
//
PUBLIC void deallocateHandle(int hnd)
{
   handleData *h = &Handle[hnd];
   h->options = 0;
   h->card = NULL;
   h->circ = NULL;
   h->buf = NULL;
}

PRIVATE void ClearBuf(bufData *b);
//
//
//
//
PUBLIC canStatus CreateBuf(bufData **buf,
                     const canHWDescr _far * const hwdescr,
                     const canSWDescr _far * const swdescr,
                     const unsigned int flags)
{
   bufData *b;
   int i;
   int found;

   b = BufferHeads;
   found = FALSE;
   for (i=0; i<COUNTOF(BufferHeads); i++) {
      if (b->refCount == 0) {
         found = TRUE;
         break;
      }
      b++;
   }
   if (!found) return canERR_NOHANDLES;

   // Fill in default values.
   if (!swdescr) {
      b->rxBufSize = DEFAULT_RXBUFFER_SIZE;
      b->txBufSize = DEFAULT_TXBUFFER_SIZE;
      b->alloc = malloc;
      b->deAlloc = free;
   } else {
      b->rxBufSize = swdescr->rxBufSize;
      if (swdescr->rxBufSize == 0)
         b->rxBufSize = DEFAULT_RXBUFFER_SIZE;
      b->txBufSize = swdescr->txBufSize;
      b->alloc = swdescr->alloc;
      if (!b->alloc) b->alloc = malloc;
      b->deAlloc = swdescr->deAlloc;
      if (!b->deAlloc) b->deAlloc = free;
   }

   b->rxBuffer = NULL;
   b->txBuffer = NULL;

   *buf = b;

   ARGUSED(hwdescr);
   ARGUSED(flags);
   return canOK;
}


/*
** Zero out the buffers.
*/
PRIVATE void ClearBuf(bufData *b)
{
   unsigned int bufsiz;
   Message *rbuf = b->rxBuffer;
   txMessage *tbuf = b->txBuffer;

   for (bufsiz = b->rxBufSize; bufsiz; bufsiz--) {
      rbuf->targets = 0;
      rbuf++;
   }

   for (bufsiz = b->txBufSize; bufsiz; bufsiz--) {
      tbuf->available = TRUE;
      tbuf++;
    }

   b->txMsgs = 0;
   b->txHead = b->txTail = 0;
}

PUBLIC canStatus attachBuffer(bufData* b)
{
   if (b->refCount == 0) {
        Message* rxMsgs = NULL;
        txMessage* txMsgs = NULL;

        // Allocate the receive buffer.
        rxMsgs = (Message*)
                 b->alloc(sizeof(Message) * b->rxBufSize);


        // Allocate the transmit buffer.
        if (b->txBufSize) {
            txMsgs = (txMessage*)
                     b->alloc(sizeof(txMessage) * b->txBufSize);
        }

        // Check that the buffers really were allocated.
        if ((!rxMsgs) || (b->txBufSize && (!txMsgs))) {
            if (rxMsgs)
                b->deAlloc(rxMsgs);
            if (txMsgs)
                b->deAlloc(txMsgs);
            return canERR_NOMEM;
        }

        b->rxBuffer = rxMsgs;
        b->txBuffer = txMsgs;
        b->refCount++;

        ClearBuf(b);
    }
    return canOK;
}

PUBLIC canStatus detachBuffer(bufData* b)
{
    if (b->refCount == 1) {
        if (b->txBuffer) b->deAlloc(b->txBuffer);
        if (b->rxBuffer) b->deAlloc(b->rxBuffer);
        b->txBuffer = NULL;
        b->rxBuffer = NULL;
        b->refCount--;
    }
    return canOK;
}

PUBLIC canStatus setupHandle(int hnd,
                             cardData* card,
                             circData* circ,
                             bufData* buf)
{
   int i, found;
   handleData* h = &Handle[hnd];
   h->card = card;
   h->circ = circ;
   h->buf = buf;

   DISABLE_INTERRUPTS;
   h->rxFirstMsg = circ->NextMsg;

   found = FALSE;
   for (i=0; i<COUNTOF(circ->handles); i++) {
      if (circ->handles[i] == NULL) {
         circ->handles[i] = h;
         found = TRUE;
         break;
      }
   }
   ENABLE_INTERRUPTS;
   if (!found) return canERR_NOHANDLES;

   return canOK;
}

PUBLIC canStatus dissolveHandle(int hnd)
{
    handleData *h = &Handle[hnd];
    circData *circ = h->circ;
    int i;

    DISABLE_INTERRUPTS;
    for (i=0; i<COUNTOF(circ->handles); i++) {
        if (circ->handles[i] == h) {
            circ->handles[i] = NULL;
            break;
        }
    }
    ENABLE_INTERRUPTS;
    return canOK;
}

//
// Here are a few utility routines for posting meta-messages
// (like status changes, error frames, tx requests etc.)
//
PUBLIC void postMetaMessage(circData* h, int msgType)
{
   bufData* b;
   Message* buf;
   unsigned int i, rxbufsize;
   Message* m;
   WORD address;

   address = h->address;
   b = h->buf;
   rxbufsize = b->rxBufSize;
   buf = b->rxBuffer;

   // "Allocate" the next entry in the ring buffer.
   // Update the pointers.
   DISABLE_INTERRUPTS;
   i = h->NextMsg;
   m = &buf[i++];
   if (i>=rxbufsize) i=0;
   h->NextMsg = i;
   ENABLE_INTERRUPTS;

   m->id = CANID_METAMSG;
   switch(msgType) {
       /*case canMSG_STATUS:
           m->dlc = 0;
           m->flags = canMSG_STATUS;
           break;*/
           
       case canMSG_ERROR_FRAME:
           m->dlc = 2;      // qqq
           m->flags = canMSG_ERROR_FRAME;
           m->data[0] = 0;  // qqq
           m->data[1] = 0;  // qqq
           break;
           
       //case canMSG_TXACK:
       //case canMSG_TXRQ:
       default:
           // Should never occur.  qqq
           ;
   }
   
    // Sample receive time
    m->time = GETTIMER();

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

      if (hnd->options & HOPTION_WANT_METAMSGS) {
          m->targets |= hnd->handleMask;
      }
   }
}

PUBLIC void postTxMetaMessage(circData* h, int msgType,
                              long id, int dlc, void *data, int flags)
{
   bufData* b;
   Message* buf;
   unsigned int i, rxbufsize;
   Message* m;
   WORD address;
   WORD handleFlag;

   address = h->address;
   b = h->buf;
   rxbufsize = b->rxBufSize;
   buf = b->rxBuffer;

   // "Allocate" the next entry in the ring buffer.
   // Update the pointers.
   DISABLE_INTERRUPTS;
   i = h->NextMsg;
   m = &buf[i++];
   if (i>=rxbufsize) i=0;
   h->NextMsg = i;
   ENABLE_INTERRUPTS;

   switch(msgType) {
       case canMSG_TXACK:
           m->id = id;
           m->flags = flags | canMSG_TXACK;
           m->dlc = dlc;
           memcpy(m->data, data, dlc);
           handleFlag = HOPTION_WANT_TXACK;
           break;
           
       case canMSG_TXRQ:
           m->id = id;
           m->flags = flags | canMSG_TXRQ;
           m->dlc = dlc;
           memcpy(m->data, data, dlc);
           handleFlag = HOPTION_WANT_TXACK;
           break;
           
       default:
           // Should never occur.  qqq
           handleFlag = 0;
           break;
   }
   
    // Sample receive time
    m->time = GETTIMER();

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

      if (hnd->options & handleFlag) {
          m->targets |= hnd->handleMask;
      }
   }
}
