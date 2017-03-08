/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This module contains the code specific to the CANCard card.
*/
#include <stdlib.h>
#include <dos.h>
#include <memory.h>
#include <string.h>
#include <direct.h>
//#if defined(_WINDOWS) || defined(_Windows)
//#include <windows.h>
//#endif

#include <std.h>
#include <canlib.h>
// #include <util.h>
#include "global.h"
#include "cancard.h"
#include "cancwrap.h"
#include "libcanc.h"
#include "misc.h"

#ifdef _MSC_VER
#define CHDIR _chdir
#define GET_CURRENT_DISK (_getdrive() - 1)
#define SET_CURRENT_DISK(X) (void)(_chdrive(X+1))
#define GETCWD(BUF, MAXLEN) (void)_getcwd((BUF),(MAXLEN))
#endif
#ifdef __BORLANDC__
#define CHDIR chdir
#define GET_CURRENT_DISK (getdisk())
#define SET_CURRENT_DISK(X) (void)(setdisk(X))
#define GETCWD(BUF, MAXLEN) (void)getcwd((BUF),(MAXLEN))
#endif

#define TX_FIFO_SIZE    24          // Actually 32, but be conservative...
#define SPEC x.cc

#define ccSTATE_COLDSTART     0x01
#define ccSTATE_BUSPARAMS     0x02
#define ccSTATE_BUSDRIVER     0x04
#define ccSTATE_BUSON         0x08

#ifndef _WINDOWS
#define CALLBACK            _far _pascal
typedef void (CALLBACK*     FARPROC)(void);
typedef void far *HINSTANCE;
#endif


#define UndefinedDriver    (-2)
PRIVATE short CANCdriverData[8] = {
   UndefinedDriver,     // Off
   UndefinedDriver,     // Tristate
   UndefinedDriver,     // Up
   UndefinedDriver,     // Down
   -1,                  // Pushpull
   UndefinedDriver,     // Inverted up
   UndefinedDriver,     // Inverted down
   UndefinedDriver,     // Inverted pushpull
};

// Please make sure the order here is the one defined by the N_...
// symbols in cancard.h!!
//
// Perhaps we should import the routines by their ordinal number,
// but then we can't tell whether a particular routine really exists or not.
// 
struct {
   char     *Name;  // Name of function in DLL
   FARPROC  Proc;   // procedure instance
   int      Needed; // Should we exit if the function is not found?
} Fun[] = {
   { "INIPC_initialize_board",          NULL, TRUE  }, // 0
   { "CANPC_reset_board",               NULL, FALSE }, // 1
   { "CANPC_reset_chip",                NULL, FALSE }, // 2
   { "CANPC_initialize_chip",           NULL, FALSE }, // 3
   { "CANPC_initialize_chip2",          NULL, FALSE }, // 4
   { "CANPC_set_mode",                  NULL, FALSE }, // 5
   { "CANPC_set_mode2",                 NULL, FALSE }, // 6
   { "CANPC_set_acceptance",            NULL, FALSE }, // 7
   { "CANPC_set_acceptance2",           NULL, FALSE }, // 8
   { "CANPC_set_output_control",        NULL, FALSE }, // 9
   { "CANPC_set_output_control2",       NULL, FALSE }, // 10
   { "CANPC_initialize_interface",      NULL, FALSE }, // 11
   { "CANPC_define_object",             NULL, FALSE }, // 12
   { "CANPC_define_object2",            NULL, FALSE }, // 13
   { "CANPC_enable_fifo",               NULL, FALSE }, // 14
   { "CANPC_optimize_rcv_speed",        NULL, FALSE }, // 15
   { "CANPC_enable_dyn_obj_buf",        NULL, FALSE }, // 16
   { "CANPC_enable_timestamps",         NULL, FALSE }, // 17
   { "CANPC_enable_fifo_transmit_ack",  NULL, FALSE }, // 18
   { "CANPC_enable_fifo_transmit_ack2", NULL, FALSE }, // 19
   { "CANPC_get_version",               NULL, FALSE }, // 20
   { "CANPC_start_chip",                NULL, FALSE }, // 21
   { "CANPC_send_remote_object",        NULL, FALSE }, // 22
   { "CANPC_send_remote_object2",       NULL, FALSE }, // 23
   { "CANPC_supply_object_data",        NULL, FALSE }, // 24
   { "CANPC_supply_object_data2",       NULL, FALSE }, // 25
   { "CANPC_supply_rcv_object_data",    NULL, FALSE }, // 26
   { "CANPC_supply_rcv_object_data2",   NULL, FALSE }, // 27
   { "CANPC_send_object",               NULL, FALSE }, // 28
   { "CANPC_send_object2",              NULL, FALSE }, // 29
   { "CANPC_write_object",              NULL, FALSE }, // 30
   { "CANPC_write_object2",             NULL, FALSE }, // 31
   { "CANPC_read_rcv_data",             NULL, FALSE }, // 32
   { "CANPC_read_rcv_data2",            NULL, FALSE }, // 33
   { "CANPC_read_xmt_data",             NULL, FALSE }, // 34
   { "CANPC_read_xmt_data2",            NULL, FALSE }, // 35
   { "CANPC_read_ac",                   NULL, FALSE }, // 36
   { "CANPC_send_data",                 NULL, FALSE }, // 37
   { "CANPC_send_data2",                NULL, FALSE }, // 38
   { "CANPC_send_remote",               NULL, FALSE }, // 39
   { "CANPC_send_remote2",              NULL, FALSE }, // 40
   { "CANPC_set_trigger",               NULL, FALSE }, // 41
   { "CANPC_set_trigger2",              NULL, FALSE }, // 42
   { "CANPC_reinitialize",              NULL, FALSE }, // 43
   { "CANPC_reset_rcv_fifo",            NULL, FALSE }, // 44
   { "CANPC_get_time",                  NULL, FALSE }, // 45
};



#ifdef _WINDOWS
static HINSTANCE DLLInst;
#endif

/*
** Initializes the data structures.
*/
PUBLIC canStatus CANClibInit(void)
{
   static int Done = FALSE;
   int i;

   if (!Done) {
      Done = TRUE;
      for (i=0; i<COUNTOF(Fun); i++) {
         Fun[i].Proc = NULL;
      }
   }
#ifdef _WINDOWS
   DLLInst = (HINSTANCE)0;
#endif
   return canOK;
}

PUBLIC canStatus CANClibExit(void)
{
   return canOK;
}

/*
** Forward declarations...
*/
PRIVATE canStatus CANCLocateHardware_20(char *cardname);
PRIVATE void CANCinterrupt(cardData* card);
PRIVATE canStatus findFreeCircuits(cardData* card);

PUBLIC canStatus CANCLocateHardware(char *cardname)
{
   int version;

   version = (int)GetConfigFileInt(cardname, "VERSION", -1, IniFile);

   switch (version) {
      case -1:
         return canERR_INIFILE;

      case VERSION_NUMBER(2,0):
         return CANCLocateHardware_20(cardname);

      default:
         return canERR_INIFILE;
   }
}

// A little fix for making it AC2-compatible?
static int ItsAnAC2 = FALSE;
static int AC2MemSpace = 0;

PRIVATE canStatus CANCLocateHardware_20(char *cardname)
{
   char datafile[_MAX_PATH];
   char DLLName[_MAX_PATH];
   canStatus stat = canOK;
   cardData* c;
   int tmp;

   (void)GetConfigFileString(cardname, "DLL", "",
                             DLLName, sizeof(DLLName), IniFile);
   if (strlen(DLLName) == 0) {
      return canERR_INIFILE;
   }

   (void)GetConfigFileString(cardname, "INIFILE", "", datafile,
                             sizeof(datafile), IniFile);

   if (GetConfigFileInt(cardname, "AC2Fix", -1, IniFile) == -1) {
       ItsAnAC2 = FALSE;
   } else {
       ItsAnAC2 = TRUE;
   }

   AC2MemSpace = (int)(GetConfigFileInt(cardname, "AC2MemSpace", -1, IniFile));
   if (AC2MemSpace == -1) AC2MemSpace = 0;
   
   
   // Allocate a new struct cardData and fill out its fields
   // The IRQ is read from LOADCARD.INI.
   c = allocateCard();
   if (!c) return canERR_NOHANDLES;

   c->address = 0;
   // Store the path to the loadcard.ini file.
   c->SPEC.Inifile = malloc(strlen(datafile)+1);
   strcpy(c->SPEC.Inifile, datafile);
   if (ItsAnAC2) {
       tmp = (int)GetConfigFileInt(cardname, "AC2IRQ", -1, IniFile);
   } else {
       tmp = (int)GetConfigFileInt("Resources", "Interrupt", -1, datafile);
   }
   c->supportedIRQ = tmp>0?(1<<tmp):0;
   c->usedIRQ = -1;
   c->desiredIRQ = -1;
   c->defaultIRQ = (tmp>0?tmp:-1);
   c->valid = TRUE;
   c->cardType = canCARD_CANCARD;
   c->refCount = 0;
   c->status = 0;

   (void)GetConfigFileString(cardname, "NAME", "", c->name,
                             sizeof(c->name), IniFile);

   (void)GetConfigFileString(cardname, "VENDOR", "", c->vendor,
                             sizeof(c->vendor), IniFile);

   (void)GetConfigFileString(cardname, "VERSIONSTRING", "", c->version,
                             sizeof(c->version), IniFile);

   stat = findFreeCircuits(c);

   c->InitCard = CANCInitCard;
   c->KillCard = CANCkillCard;
   c->IntRtn = CANCinterrupt;

   c->SPEC.boardInit = FALSE;
   c->SPEC.DLLName = malloc(strlen(DLLName)+1);
   strcpy(c->SPEC.DLLName, DLLName);

   // qqq Memory leak: DLLName & Inifile are never deallocated.
   
   return stat;
}

/*
** Allocate and fill out struct circData.
*/
PRIVATE canStatus findFreeCircuits(cardData* card)
{
   unsigned int ctrlno;
   circData* circ;

   // Currently, we support only the first controller.
   for (ctrlno = 0; ctrlno<1; ctrlno++) {
      circ = allocateCircuit();
      if (!circ) return canERR_NOHANDLES;
      circ->circStatus = 0;
      circ->circuitType = CANCARD_NEC72005;
      circ->address = 0;
      circ->channel = ctrlno;
      card->Circuits[ctrlno] = circ;
      circ->card = card;
      circ->valid = TRUE;
      circ->capabilities = canCIRC_EXTENDED|canCIRC_FULLCAN;
      circ->BusOn = CANCbusOn;
      circ->BusOff = CANCbusOff;
      circ->SetBusParams = CANCbusParams;
      circ->SetOutputControl = CANCbusOutputControl;
      circ->SetMask = CANCaccept;
      circ->GetStatus = CANCstatus;
      circ->TXAvailable = CANCTXAvailable;
      circ->StartWrite = CANCstartWrite;
      circ->InitCircuit = CANCInitCircuit;
      circ->KillCircuit = CANCkillCircuit;
      circ->EnableExtended = CANCEnableExtended;
      circ->SPEC.State = 0;
      circ->SPEC.Reinit = FALSE;
   }
   return canOK;
}

/*
** Reinit the card after removal/insertion.
** Currently unused. Methinks Softing should have done this in their
** DLL anyway.
**
** (If any function CANPC_.. returns -4, call this function and
** then call the offending function again)
*/
PUBLIC canStatus CANCReinit(circData* h)
{
   canStatus stat;

   if (h->SPEC.Reinit) return canERR_REINIT;
   h->SPEC.Reinit = TRUE;

   if (h->SPEC.State & ccSTATE_COLDSTART) {
      cardData* card = (cardData*)h->card;
      // Cold start
      card->SPEC.boardInit = FALSE;
      stat = CANCInitCard(card);
      h->SPEC.State = ccSTATE_COLDSTART;
      if (CANSTATUS_FAILURE(stat)) return stat;
   }

   if (h->SPEC.State & ccSTATE_BUSPARAMS) {
      // reset_chip is done. Time for the bus parameters.
      stat = CANCbusParams(h, h->freq, h->tseg1, h->tseg2, h->sjw,
                           h->nosamp, h->syncmode);
      if (CANSTATUS_FAILURE(stat)) return stat;
   }

   if (h->SPEC.State & ccSTATE_BUSDRIVER) {
      // reset_chip is done. Time for the bus parameters.
      stat = CANCbusOutputControl(h, h->driver);
      if (CANSTATUS_FAILURE(stat)) return stat;
   }

   if (h->SPEC.State & ccSTATE_BUSON) {
      stat = CANCbusOn(h);
      if (CANSTATUS_FAILURE(stat)) return stat;
   }

   h->SPEC.Reinit = FALSE;
   return canOK;
}


/*
** Initialize the specified circuit. Set default bus parameters.
*/
PUBLIC canStatus CANCInitCircuit(circData* h)
{
   if (h->circuitType == CANCARD_NEC72005) {

      // Kill BOTH chips, thanks to Softing.
      if (!ItsAnAC2) (void)CANPC_REINITIALIZE();
      (void)CANPC_RESET_CHIP();

      if (h->channel == 0) {
         // Set default bus parameters
         // 1MB/s, Tseg1=5q, Tseg2=3q, SJW=1, 1 sampling point, fast mode.
         (void)CANCbusParams(h, 1000000L, 4, 3, 0, 1, 0);
         (void)CANCbusOutputControl(h, canPUSHPULL);
         // Acceptance mask ...
         (void)CANPC_SET_ACCEPTANCE(0, 0, 0L, 0L);
      }

      h->circStatus |= CIRCSTAT_INIT;
      h->SPEC.State |= (ccSTATE_BUSPARAMS|ccSTATE_BUSDRIVER);

      return canOK;
   }

   return canERR_PARAM;
}

PUBLIC canStatus CANCkillCircuit(circData* h)
{
   
   if (h->circuitType == CANCARD_NEC72005) {
      if (!ItsAnAC2) (void)CANPC_REINITIALIZE();
      (void)CANPC_RESET_CHIP();
      h->circStatus &= ~CIRCSTAT_INIT;
      h->SPEC.State = ccSTATE_COLDSTART;
   }
   return canOK;
}

PUBLIC canStatus CANCInitCard(cardData* h)
{
   canStatus stat;
   int istat;
   char currDir[_MAX_PATH];
   char drive[_MAX_DRIVE], path[_MAX_PATH];
   int currDisk, newDisk;

#ifdef _WINDOWS
   //
   // Dynamically load cancard.dll and set up the function dispatch
   // table. From here on, all CANPC.. macros will work.
   //
   int j, error;
   unsigned int oldErrorMode;

   oldErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);
   DLLInst = LoadLibrary(h->SPEC.DLLName);
   (void)SetErrorMode(oldErrorMode);
   
   if (DLLInst < HINSTANCE_ERROR) {
      return canERR_DYNALOAD;
   }

   // Get the function addresses & make function instances
   // Some functions are not needed so if their loading fails
   // we are happy anyway.
   error = FALSE;
   for (j=0; j<COUNTOF(Fun); j++){
       FARPROC addr;

       Fun[j].Proc = NULL;
       addr = GetProcAddress(DLLInst, Fun[j].Name);
       if (addr != NULL) {
           addr = Fun[j].Proc = MakeProcInstance(addr, DLLInst);
           if (addr == NULL) error = TRUE;
       } else if (Fun[j].Needed) {
           error = TRUE;
       }
   }
   if (error) return canERR_DYNALIB;

#else
   // DOS... do nothing. The library is already statically linked.
#endif


   // Play nasty tricks with the working directory as the
   // cancard s/w isn't too clever.
   _splitpath(h->SPEC.Inifile, drive, path, NULL, NULL);

   currDisk = GET_CURRENT_DISK;
   if (drive[0] != 0) {
      newDisk = drive[0] - 'A';
      if (newDisk >= 32) newDisk -= 32;
      SET_CURRENT_DISK(newDisk);
   }
   GETCWD(currDir, sizeof(currDir));

   if (strlen(path) == 0) {
      strcpy(path, ".");
   } else {
      // Kill trailing backslash
      path[strlen(path)-1] = '\0';
   }

   stat = canOK;

   if (CHDIR(path) == 0) {
      // Initialize the library and the card; load the microcode etc.
      if (!h->SPEC.boardInit) {
         // Cold start.
          if (ItsAnAC2) {
              istat = INIPC_INITIALIZE_BOARD(AC2MemSpace);
          } else {
              istat = INIPC_INITIALIZE_BOARD(0);
          }
         if (istat < 0) {
            stat = canERR_DYNAINIT;
         } else {
            istat = CANPC_RESET_BOARD();
            if (istat < 0) {
               stat = canERR_DYNAINIT;
            } else {
               h->SPEC.boardInit = TRUE;
            }
         }
      } else {
         // Warm start.
         if (!ItsAnAC2) {
             istat = CANPC_REINITIALIZE();
             if (istat < 0) stat = canERR_DYNAINIT;
         }
      }
      if (stat == canOK) {
         istat = CANPC_RESET_CHIP();
         if (istat < 0) stat = canERR_DYNAINIT;
      }
   } else {
      stat = canERR_DYNAINIT;
   }
   //
   // Restore working directory
   //
   (void)CHDIR(currDir);
   SET_CURRENT_DISK(currDisk);

   if (CANSTATUS_FAILURE(stat)) return stat;

   h->status |= canCARD_INIT;

   return canOK;
}


/*
** Kill the CANcard card entirely.
*/
PUBLIC canStatus CANCkillCard(cardData* h)
{
   if (h->SPEC.boardInit) {
      if (!ItsAnAC2) (void)CANPC_REINITIALIZE();
      (void)CANPC_RESET_CHIP();
   }

   h->status &= ~canCARD_INIT;

#ifdef _WINDOWS
   {
       // Clear the function dispatch table and unload the library.
       int i;
       for (i=0; i<COUNTOF(Fun); i++) {
           if (Fun[i].Proc != NULL) {
               FreeProcInstance(Fun[i].Proc);
               Fun[i].Proc = NULL;
           }
       }
       if (DLLInst != (HINSTANCE)0) {
           FreeLibrary(DLLInst);
           DLLInst = (HINSTANCE)0;
       }
   }
#endif

   return canOK;
}



/*
** Go on bus.
*/
PUBLIC canStatus CANCbusOn(circData *h)
{
   if (h->circuitType == CANCARD_NEC72005) {
      // Try to go on bus
      if (!ItsAnAC2) (void)CANPC_ENABLE_FIFO();
      (void)CANPC_ENABLE_FIFO_TX_ACK();
      (void)CANPC_START_CHIP();
      h->SPEC.State |= ccSTATE_BUSON;
      return canOK;
   }
   return canERR_PARAM;
}

/*
** Go off bus.
*/
PUBLIC canStatus CANCbusOff(circData *h)
{
   if (h->circuitType == CANCARD_NEC72005) {
      if (!ItsAnAC2) (void)CANPC_REINITIALIZE();
      (void)CANPC_RESET_CHIP();
      h->SPEC.State = ccSTATE_COLDSTART;
      return canOK;
   }
   return canERR_PARAM;
}

/*
** Set the bus parameters.
*/
PUBLIC canStatus CANCbusParams(circData* h,
                         unsigned long freq, unsigned int tseg1,
                         unsigned int tseg2,
                         unsigned int sjw, unsigned int noSamp,
                         unsigned int syncmode)
{
    ARGUSED(syncmode);
   if (h->circuitType == CANCARD_NEC72005) {
      unsigned int quantasPerCycle;
      DWORD brp;

      quantasPerCycle = tseg1 + tseg2 + 1;
      brp = (8000000L * 64) / (freq * quantasPerCycle);
      if ((brp & 0x3F) != 0) {
         // Fraction != 0 : not divisible.
         return canERR_PARAM;
      }
      brp = (brp >> 6);
      if (brp > 64 || sjw > 4 || sjw < 1 || quantasPerCycle < 8) {
         return canERR_PARAM;
      }
      if (noSamp != 1) return canERR_PARAM;

      (void)CANPC_INITIALIZE_CHIP((int)brp, (int)sjw, (int)tseg1, (int)tseg2, 1);
      (void)CANPC_SET_MODE(0, 0);

      h->SPEC.State |= ccSTATE_BUSPARAMS;
      return canOK;
   }
   return canERR_PARAM;
}


/*
** Set driver output type.
*/
PUBLIC canStatus CANCbusOutputControl(circData *h, unsigned int drivertype)
{
   if (drivertype > canINVPUSHPULL) {
      return canERR_PARAM;
   }

   if (h->circuitType == CANCARD_NEC72005) {
      short driver;

      driver = CANCdriverData[drivertype];
      if (driver == UndefinedDriver) return canERR_DRIVER;

      // We don't support different driver configurations for the moment.
      // Just the ordinary push-pull.
      if (ItsAnAC2) {
          (void)CANPC_SET_OUTPUT_CONTROL(0);
      } else {
          (void)CANPC_SET_OUTPUT_CONTROL(-1);
      }
      h->SPEC.State |= ccSTATE_BUSDRIVER;
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
** canFILTER_ACCEPT   Not implemented
** canFILTER_REJECT   Not implemented
** canFILTER_CODE_STD     Acceptance code; the identifier is compared to selected bits
**                in the acceptance code and the message is accepted if
**                they are equal.
** canFILTER_MASK_STD     Acceptance mask; a "1" means the corresponding bit in the
**                acceptance code is relevant; a "0" means a "don't care"
**                bit.
*/
PUBLIC canStatus CANCaccept(circData* h, long envelope, unsigned int flags)
{
   if (h->circuitType == CANCARD_NEC72005) {

      unsigned long code = 0L;
      unsigned long mask = 0L;
      //BYTE gotCode = FALSE;
      //BYTE gotMask = FALSE;

      // Calculate acceptance code and acceptance mask
      switch (flags) {
         case canFILTER_ACCEPT:
            // qqq
            break;

         case canFILTER_REJECT:
            // qqq
            break;

         case canFILTER_SET_CODE_STD:
            code = (unsigned long)envelope;
            h->stdAccCode = envelope;
            break;

         case canFILTER_SET_MASK_STD:
            mask = (unsigned long)envelope;
            h->stdAccMask = envelope;
            break;

         case canFILTER_SET_CODE_EXT:
            code = (unsigned long)envelope;
            h->extAccCode = envelope;
            break;

         case canFILTER_SET_MASK_EXT:
            mask = (unsigned long)envelope;
            h->extAccMask = envelope;
            break;
            
         default:
            return canERR_PARAM;
      }

      (void)CANPC_SET_ACCEPTANCE((unsigned int)h->stdAccCode,
                                 (unsigned int)h->stdAccMask,
                                 h->extAccCode,
                                 h->extAccMask);

      return canOK;
   }

   return canERR_PARAM;
}

/*
** Retrieve the current controller status.
*/
PUBLIC canStatus CANCstatus(circData* h, unsigned long* flags)
{
    unsigned long outStatus;
    
    outStatus = h->busStatus;

    if (h->hwOverrun) outStatus |= canSTAT_HW_OVERRUN;
    if (h->swOverrun) outStatus |= canSTAT_SW_OVERRUN;
    if (h->RXErrorCounter > 0) h->rxError = TRUE;
    if (h->TXErrorCounter > 0) h->txError = TRUE;

    if (h->rxError) outStatus |= canSTAT_RXERR;
    if (h->txError) outStatus |= canSTAT_TXERR;

    if (!flags || (h->circuitType != CANCARD_NEC72005)) {
        return canERR_PARAM;
    }

    *flags = outStatus;
    
    return canOK;
}

/*
** Start a transmission. May be called at interupt level.
*/
PUBLIC canStatus CANCstartWrite(circData* h, long id, void* msgPtr,
                          unsigned int dlc, unsigned int flag)
{
   int ext;

   ext = (flag & canMSG_STD)?0:((flag & canMSG_EXT)?1:0);
   if (h->circuitType == CANCARD_NEC72005) {
      if (flag & canMSG_RTR) {
         (void)CANPC_SEND_REMOTE((unsigned long)id, ext, (int)dlc);
      } else {
         (void)CANPC_SEND_DATA((unsigned long)id, ext, (int)dlc, msgPtr);
      }
      return canOK;
   }
   return canERR_PARAM;
}

PUBLIC int CANCTXAvailable(circData* h)
{
   if (h->circuitType == CANCARD_NEC72005) {
      bufData* b = (bufData*) h->buf;
      return (b->txMsgs < TX_FIFO_SIZE);
   }
   return FALSE;
}

PUBLIC canStatus CANCEnableExtended(circData* h, unsigned int onoff)
{
   ARGUSED(onoff);
   ARGUSED(h);
   return canOK;
}



PRIVATE void CANCARD_NEC72005_Receive(circData *h, param_struct *p,
                                      int std, int rtr);
PRIVATE void CANCARD_NEC72005_Transmit(circData* h, param_struct *p);
PRIVATE void CANCARD_NEC72005_Error(circData *h, param_struct *p);
//PRIVATE void CANCARD_NEC72005_Overrun(circData *h, param_struct *p);

/*
** This routine is called from the (real) interrupt routine in irq.c.
*/
PRIVATE void CANCinterrupt(cardData* card)
{
   circData* h;
   param_struct p;
   int stat, loops;

   loops = 1000;
   h = card->Circuits[0];  // qqq
   while (loops--) {
      stat = CANPC_READ_AC(&p);
      // Watch out for spurious interrupts.
      if (h->circStatus & CIRCSTAT_INIT) switch (stat) {
         case 0:
            // No more events
            loops = 0;
            break;

         case 1:
            // Std frame received
            CANCARD_NEC72005_Receive(h, &p, TRUE, FALSE);
            break;

         case 2:
            // Std remote frame received
            CANCARD_NEC72005_Receive(h, &p, TRUE, TRUE);
            break;

         case 3:
            // Tx done, std frame
            CANCARD_NEC72005_Transmit(h, &p);
            break;

         case 4:
            // Remote tx FIFO overrun
            break;

         case 5:
            // Bus status changed
            CANCARD_NEC72005_Error(h, &p);
            break;

         case 6:
            // Not implemented
            break;

         case 7:
            // Additional errors
            break;

         case 8:
            // Tx done, std RTR
            CANCARD_NEC72005_Transmit(h, &p);
            break;

         case 9:
            // Ext frame received
            CANCARD_NEC72005_Receive(h, &p, FALSE, FALSE);
            break;

         case 10:
            // Tx done, ext frame
            CANCARD_NEC72005_Transmit(h, &p);
            break;

         case 11:
            // Tx done, ext RTR
            CANCARD_NEC72005_Transmit(h, &p);
            break;

         case 12:
            // Ext RTR received
            CANCARD_NEC72005_Receive(h, &p, FALSE, TRUE);
            break;

         case -4:
            // card timeout
            break;

         default:
            // Vaaafff?
            break;
      }
   }
}

PRIVATE void CANCARD_NEC72005_Receive(circData *h, param_struct *p,
                                      int std, int rtr)
{
   bufData* b;
   Message *buf, *m;
   unsigned int i, rxbufsize;
   BYTE *ptr, *data;
   unsigned int savedNextMsg;


   b = h->buf;
   rxbufsize = b->rxBufSize;
   buf = b->rxBuffer;
   savedNextMsg = i = h->NextMsg;
   m = &buf[i++];
   if (i>=rxbufsize) i=0;
   h->NextMsg = i;

   // Read controller data and store in buffer.
   m->id    = (long)p->Ident;
   m->dlc   = (unsigned int)p->DataLength;
   m->flags = (rtr?canMSG_RTR:0) | (std?canMSG_STD:canMSG_EXT);

   data     = p->RCV_data;
   ptr      = m->data;
   switch (m->dlc) {
      case 8:
         *ptr++ = *data++;
         // Fall through
      case 7:
         *ptr++ = *data++;
         // Fall through
      case 6:
         *ptr++ = *data++;
         // Fall through
      case 5:
         *ptr++ = *data++;
         // Fall through
      case 4:
         *ptr++ = *data++;
         // Fall through
      case 3:
         *ptr++ = *data++;
         // Fall through
      case 2:
         *ptr++ = *data++;
         // Fall through
      case 1:
         *ptr++ = *data++;
         // Fall through
      case 0:
      default:
         ;
   }

   m->time = p->Time;

   // qqq Save error registers etc, if desired (use a flag in canOpen)

   m->overwritten = TRUE;
   if (p->RCV_fifo_lost_msg) {
       h->hwOverrun = TRUE;
       h->TotalOverruns += p->RCV_fifo_lost_msg;
   }
   if (m->targets) {
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


PRIVATE void CANCARD_NEC72005_Transmit(circData* h, param_struct *p)
{
   bufData* b = h->buf;
   unsigned int i;

   ARGUSED(p);
   //
   // Send the next message, if the transmit buffer is empty.
   //
   if (b->txMsgs != 0 ) {
      txMessage* t;
      i = b->txHead;
      t = &b->txBuffer[i];
      (void)CANCstartWrite(h, t->id, t->data, t->dlc, t->flags);
      t->available = TRUE;
      i++;
      if (i >= b->txBufSize) i = 0;
      b->txHead = i;
      b->txMsgs--;
   }

   // Post message if enabled
   // qqq Perhaps we should have a message-specific flag for this.
   // (I.e. "please tell me when this message is sent")
   for (i=0; i<COUNTOF(h->handles); i++) {
      handleData *hnd = h->handles[i];
      if (hnd == NULL) break;

      if (hnd->notifyFlags & canNOTIFY_TX) {
         NOTIFY(hnd->hWnd, canEVENT_TX, hnd->nr, 0); // qqq, 0??
      }
   }

}

PRIVATE void CANCARD_NEC72005_Error(circData *h, param_struct *p)
{
    int i;

    switch (p->Bus_state) {
        case 0:
            h->RXErrorCounter = 0;
            h->TXErrorCounter = 0;
            h->busStatus = 0;
            break;
            
        case 1:
            h->RXErrorCounter = 96;
            h->TXErrorCounter = 96;
            h->busStatus = canCIRCSTAT_ERROR_PASSIVE|canCIRCSTAT_ERROR_WARNING;
            break;
            
        case 2:
            h->RXErrorCounter = 255;
            h->TXErrorCounter = 256;
            h->busStatus = canCIRCSTAT_BUS_OFF;
            break;
    }

    for (i=0; i<COUNTOF(h->handles); i++) {
        handleData *hnd = h->handles[i];
        if (hnd == NULL) continue;
        if (hnd->notifyFlags & canNOTIFY_RX) {
            NOTIFY(hnd->hWnd, canEVENT_STATUS, hnd->nr, 0);
        }
    }
}


/*
PRIVATE void CANCARD_NEC72005_Overrun(circData *h, param_struct *p)
{
    enable();
    ARGUSED(p);
    // Set flags and increase hw overrun counter
    //h->flags |= canFLAG_OVR;   qqq
    h->hwOverrun++;
}

PRIVATE void CANCARD_NEC72005_Wakeup(circData *h,  param_struct *p)
{
    ARGUSED(p);
    // Set flags and increase hw wakeup counter
    /// h->flags |= canFLAG_WAKEUP; qqq
    ARGUSED(h);
}

*/


