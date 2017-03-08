/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/

#ifndef _HERMES_H_
#define _HERMES_H_

#include "stdinc.h"
#include "irq.h"
#include "version.h"

#define HERMES_NAME "PCIcan"


#define HERMES_VENDOR_ID                            0x10e8 // AMCC.
#define HERMES_DEVICE_ID_PROTO                      0x5920 // Temporary, qqq
#define HERMES_DEVICE_ID                            0x8406
#define HERMES_SUBSYSTEM_ID                         0 // qqq for now.
#define HERMES_SUBSYSTEM_VENDOR_ID                  0 // qqq for now.

#define HERMES_CHANNEL_COUNT 4

// Each sja1000 occupies this many bytes in I/O space.
#define HERMES_BYTES_PER_CIRCUIT                    0x20 // 0x80

#ifdef __WIN_NT__
typedef PUCHAR IoAddress;
typedef unsigned char BYTE;
typedef unsigned short WORD;
// typedef unsigned long DWORD;
#endif
#ifdef __WIN_95__
typedef WORD IoAddress;
#endif

// Accept this many errors; the rest are ignored until we have
// sent or received one message successfully.
#define MAX_ERROR_COUNT         128

// Accept up to MAX_ERROR_COUNT/2 errors within this number of
// 10 us ticks before turning error interrupts off.
#define ERROR_RATE              3000


typedef struct _hermes_context;
typedef struct _hermes_context HERMEScontext, *PHERMEScontext;

typedef Vstatus (*ChipFunction_t)(PHERMEScontext pCtx, int chan, PVevent e);

#define CIRCTYPE_NULL    0
#define CIRCTYPE_SJA1000 1

typedef struct _hermes_hwchannel {
    ChipFunction_t  WriteMessage;
    ChipFunction_t  BusParams;
    ChipFunction_t  SetHwFilter;
    ChipFunction_t  BusOn;
    ChipFunction_t  BusOff;
    ChipFunction_t  GetChipState;
    ChipFunction_t  BusOutputControl;

    BOOL            isInit;
    Vevent          currentTxMessage;
    BYTE            currentTransId;
    IoAddress       address;
    int             circuitType;
    int             chipNo;
    int             overrun;
    int             ErrorCount;
    DWORD           ErrorTime;

    // SJA1000 specific things.
    // None as yet.
} _hermes_hwchannel;


// structure for the hardware context
struct _hermes_context {
    IoAddress          baseAddress;         // The SJA1000's
    IoAddress          s5920BaseAddress;    // The 5920
    IoAddress          xilinxAddress;       // The registers in the Xilinx

    int                isPresent;
    unsigned int       channelCount;

    // Associated channels
    PVchannel          channel[HERMES_CHANNEL_COUNT];

    // Some data about the card; these are obtained when initializing the card.
    unsigned int       hwRevision; // qqq
    unsigned int       swRevision; // qqq
    unsigned int       swOptions;  // qqq

    _hermes_hwchannel   C[HERMES_CHANNEL_COUNT];
    
#if defined(__WIN_NT__)
    PDEVICE_OBJECT     DeviceObject;
    PDRIVER_OBJECT     DriverObject;
    PKINTERRUPT        InterruptObject;
    KSPIN_LOCK         IsrSpinLock;
    unsigned int       busNumber;
#elif defined(__WIN_95__)
    int                IRQ;
#endif
};


#endif

