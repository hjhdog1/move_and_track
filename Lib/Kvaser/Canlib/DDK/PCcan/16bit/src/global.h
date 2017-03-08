/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// Card status
//   #define canCARD_OPEN          0x01
#define canCARD_INIT          0x01

typedef WORD canCardStatus;

// Circuit status
#define CIRCSTAT_OPEN         0x01
#define CIRCSTAT_SHARED       0x02
#define CIRCSTAT_INIT         0x04

// Circuit capabilities
#define canCIRC_EXTENDED      0x01
#define canCIRC_FULLCAN       0x02

/*
** Describes a circuit on a card.
*/
typedef struct tagCircData {
   BYTE                  valid;
   BYTE                  refCount;
   BYTE                  circuitType;
   BYTE                  _pad0;
   WORD                  busStatus;             // any of canSTAT_xxx
   WORD                  circStatus;            // any of CIRCSTAT_xxx (above)
   WORD                  capabilities;
   unsigned int          channel;
   WORD                  address;
   unsigned int          NextMsg;
   void*                 buf;
   void*                 card;
   void*                 handles[4];
   WORD                  TXErrorCounter;
   WORD                  RXErrorCounter;
   WORD                  ackErrCount;
   unsigned int          hwOverrun;             // hw overrun, yes/no
   unsigned int          swOverrun;             // sw overrun, yes/no
   unsigned int          txError;               // tx error, yes/no
   unsigned int          rxError;               // rx error, yes/no
   unsigned int          TotalOverruns;
   unsigned long         freq;
   unsigned int          tseg1;
   unsigned int          tseg2;
   unsigned int          sjw;
   unsigned int          nosamp;
   unsigned int          syncmode;
   unsigned int          driver;
   DWORD                 stdAccCode;
   DWORD                 stdAccMask;
   DWORD                 extAccCode;
   DWORD                 extAccMask;
   
   canStatus             (*InitCircuit)(struct tagCircData *);
   canStatus             (*KillCircuit)(struct tagCircData *);
   canStatus             (*BusOn)(struct tagCircData*);
   canStatus             (*BusOff)(struct tagCircData*);
   canStatus             (*SetBusParams)(struct tagCircData*,
                                         unsigned long freq,
                                         unsigned int tseg1,
                                         unsigned int tseg2,
                                         unsigned int sjw,
                                         unsigned int noSamp,
                                         unsigned int syncmode);
   canStatus             (*SetOutputControl)(struct tagCircData*,
                                         unsigned int drivertype);
   canStatus             (*SetMask)(struct tagCircData*,
                                    long envelope, unsigned int flags);
   canStatus             (*GetStatus)(struct tagCircData*,
                                      unsigned long* flags);
   int                   (*TXAvailable)(struct tagCircData*);
   canStatus             (*StartWrite)(struct tagCircData*,
                                       long id,
                                       void* msgPtr,
                                       unsigned int dlc,
                                       unsigned int flag);
   canStatus             (*EnableExtended)(struct tagCircData*,
                                           unsigned int onoff);
   union {
      struct {
         unsigned int   State;
         unsigned int   Reinit;
      } cc;
   } x;
} circData;

/*
** Describes a card.
*/
typedef struct tagCardData {
   BYTE           valid;
   BYTE           refCount;
   BYTE           cardType;
   BYTE           _pad0;
   canCardStatus  status;
   WORD           address;
   int            supportedIRQ;
   int            usedIRQ;
   int            desiredIRQ;
   int            defaultIRQ;
   circData*      Circuits[4];
   char           name[20];
   char           vendor[20];
   char           version[20];
   canStatus      (*InitCard)(struct tagCardData *);
   canStatus      (*KillCard)(struct tagCardData *);
   void           (*IntRtn)(struct tagCardData*);

   // And now... the ad hoc section!
   union {
      struct {
         // The CANcard.
         char*    Inifile;        // Path to loadcard.ini.
         int      boardInit;      // Is the board initialized?
         char*    DLLName;        // Path to cancard.dll.     
      } cc;
   } x;
} cardData;

typedef struct tagMessage {
    long          id;
    unsigned int  dlc;
    unsigned int  flags;
    BYTE          data[8];
    // error registers here
    BYTE          overwritten;
    WORD          targets;
    DWORD         time;
} Message;

typedef struct tagTxMessage {
    long          id;
    unsigned int  dlc;
    unsigned int  flags;
    BYTE          data[8];
    BYTE          available;
} txMessage;

/*
** Describes a buffer used by a circuit.
*/
typedef struct tagBufData {
   BYTE                  valid;
   BYTE                  refCount;
   unsigned int          rxBufSize;
   unsigned int          txBufSize;
   volatile unsigned int txMsgs;
   unsigned int          txHead;
   unsigned int          txTail;
   Message *             rxBuffer;
   txMessage *           txBuffer;
   canMemoryAllocator    alloc;
   canMemoryDeallocator  deAlloc;
} bufData;

#define DEFAULT_RXBUFFER_SIZE    200
#define DEFAULT_TXBUFFER_SIZE    200

#define HOPTION_IS_USED             0x01
#define HOPTION_WANT_METAMSGS       0x02
#define HOPTION_PREFER_EXTENDED     0x04
#define HOPTION_WANT_TXACK          0x08
#define HOPTION_WANT_TXREQ          0x10


/*
** Describes a handle to a circuit.
*/
typedef struct tagHandleData {
   unsigned int          nr;                   // Used in notify.
   WORD                  options;
   // There are currently no provisions for creating
   // "soft" masks. The hardware mask is always affected,
   // and all handles are notified when an event occurs.
//   unsigned long         stdAcceptCode;
//   unsigned long         stdAcceptMask;
//   unsigned long         extAcceptCode;
//   unsigned long         extAcceptMask;
   unsigned int          handleMask;
   cardData*             card;
   circData*             circ;
   bufData*              buf;
   unsigned int          rxFirstMsg;
   HWND                  hWnd;  // A window handle i Windows, a function in DOS.
   unsigned int          notifyFlags;
} handleData;


extern handleData Handle[16];
extern int        Initialized;
extern int        LibMainCalled;
extern circData   Circuits[16];
extern cardData   Cards[4];
extern bufData    BufferHeads[16];

PUBLIC canStatus GlobalInit(void);
PUBLIC int IsHandleValid(int hnd);
PUBLIC int IsHandleValidAndOpen(int hnd);
PUBLIC cardData* allocateCard(void);
PUBLIC circData* allocateCircuit(void);
PUBLIC canStatus attachCircuit(circData* h, unsigned int flag);
PUBLIC canStatus detachCircuit(circData* h);
PUBLIC canStatus attachCard(cardData* h, unsigned int flag);
PUBLIC canStatus detachCard(cardData* h);
PUBLIC int allocateHandle(void);
PUBLIC void deallocateHandle(int hnd);
PUBLIC canStatus CreateBuf(bufData **buf,
                     const canHWDescr _far * const hwdescr,
                     const canSWDescr _far * const swdescr,
                     const unsigned int flags);
PUBLIC canStatus attachBuffer(bufData* b);
PUBLIC canStatus detachBuffer(bufData* b);
PUBLIC canStatus setupHandle(int hnd,
                       cardData* card,
                       circData* circ,
                       bufData* buf);
PUBLIC canStatus dissolveHandle(int hnd);
PUBLIC void postTxMetaMessage(circData* h, int msgType,
                              long id, int dlc, void *data, int flags);
PUBLIC void postMetaMessage(circData* h, int msgType);


extern unsigned int Timeout_1ms;  // This many (almost) empty loop turns will take 1 ms
#define TIMEOUT      20           // Milliseconds

extern char _far IniFile[];


#endif

