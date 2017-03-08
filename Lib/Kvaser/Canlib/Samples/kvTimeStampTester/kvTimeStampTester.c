/*
**                         Copyright 2006 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** A test that generates a number of bursts of messages that are sent onto two
** channels. All messages are picked up by both channels (txAcks on) and verified
** that they are the same as the sent ones. On common request it is also verified
** that two consecutive timestamps from a channel differs (i.e. time increases)
** Statistics are kept on 
**   * how well the timestamps match
**   * number of error frames on each channel
**   * total number of received messages
**
** The test will currently fail if messages are lost or received more than once.
**
*/

#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include "canlib.h"

#define MAX_DIFF          5000  // Number of 10 us the clocks of the handles may differ
#define BURST_SIZE        100   // Number of messages to be sent each round
#define NUMBER_OF_BURSTS  100   // Number of test rounds
#define DIFF_ALLOWED      80    // Number of us timestamps may differ

typedef struct msgDataIntT {
  long id;
  unsigned dlc;
  char data[8];
  unsigned flags;
} msgDataT;


int First;
int Second;
int FirstHardwareType;
int SecondHardwareType;
int LoopCount;

int Verbose;    
int QuitOnError;
int Bitrate = 1000000L;
int BitrateConverted = canBITRATE_1M;

int blippState = -1;
static char testHeader[128];
int ErrorsFound;



static  int i1, i2;
static  long id1, id2;
static  unsigned dlc1, dlc2, flag1, flag2;
static  char data1[8], data2[8];
static  msgDataT msgs1[BURST_SIZE], msgs2[BURST_SIZE];
static  long t1, t2, tTmp, tMax, tMin, tMaxDiff=0;
static  long ndiff[MAX_DIFF*2], nTot=0;
static  int tStart;
static  unsigned h1Errors, h2Errors, h1ErrTot=0, h2ErrTot=0;
static  kvTimeDomain mytd;
static  kvTimeDomainData kvtData;

static void setupHandles(int handle1, int handle2);
static void resetHandles(int handle1, int handle2);
static void buildMsgBurst_std (void);
static void buildMsgBurst_dlc0 (void);
static void buildMsgBurst_mixed (void);
static void sendMsgBurst (int burstNr, int handle1, int handle2);
static void readMsgPair (int burstNr, int msgNr, int handle1, int handle2);
static void compareMsgPair (int burstNr, int msgNr);
static void compareTimeStamps(int burstNr, int msgNr);
static void printSomeStuff (void);

void Usage(void)
{
  printf("\nThis program tests time stamping with Kvaser canlib.\n");
  printf("http://www.kvaser.com\n\n");


  printf("Usage: program [flags]\n"
         "   -aX              Use channel X\n"
         "   -bY              Use channel Y\n"
         "   (note: X must be different from Y. Default X=0, Y=1)\n"
         "   -Bnnn            Set the speed to nnn bit/s [default = 1 Mbit/s].\n"
         "   -Lnnn            Run nnn loops.\n"
         "   -r(andomize)     Randomize.\n"
         "   -r(andomize)=nnn Randomize with seed = nnn.\n"
         "   -i               Ignore errors, and try to continue to run.\n"
         "   -s(ilent)        Run tests silently and only report errors.\n"
        );

  exit(1);
}

void beginTest(char *s)
{
  if (Verbose) {
    printf("%s ", s);
    fflush(stdout);
  }
  strcpy(testHeader, s);
}

void Error(char* s, ...)
{
  char t[256];
  va_list ap;

  va_start(ap, s);
  vsprintf(t, s, ap);
  va_end(ap);
  printf("\n[%s] %s\n", testHeader, t);
  fflush(stdout);
  ErrorsFound++;
  if (QuitOnError) exit(1);
}

#ifndef __BORLANDC__
int random(int num)
{
  int i = 0;
  i |= rand() & 0xFF;
  i <<= 8;
  i |= rand() & 0xFF;
  i <<= 8;
  i |= rand() & 0xFF;
  i <<= 8;
  i |= rand() & 0xFF;
  return abs(i % num);
}

void randomize(void)
{
  time_t t;
  srand((unsigned) time(&t));
}
#endif

void Blipp(void)
{
  if (!Verbose) return;
  switch (blippState) {
    case -1: printf(" "); break;
    case 0: printf("\b|"); break;
    case 1: printf("\b/"); break;
    case 2: printf("\b-"); break;
    case 3: printf("\b\\"); break;
  }
  if (++blippState > 3) blippState = 0;
}

void NoBlipp(void)
{
  blippState = -1;
  if (Verbose) {
    printf("\b ");
    fflush(stdout);
  }
}

void printFlag (unsigned flag)
{
  printf("Flag: 0x%04x = ", flag);
  if (flag & canMSG_RTR) {
    printf("[RTR]");
  }
  if (flag & canMSG_STD) {
    printf("[STD]");
  }
  if (flag & canMSG_EXT) {
    printf("[EXT]");
  }
  if (flag & canMSG_ERROR_FRAME) {
    printf("[ERROR_FRAME]");
  }
  if (flag & canMSG_WAKEUP) {
    printf("[WAKEUP]");
  }
  if (flag & canMSGERR_HW_OVERRUN) {
    printf("[HW_OVERRUN]");
  }
  if (flag & canMSGERR_SW_OVERRUN) {
    printf("[SW_OVERRUN]");
  }
  if (flag & canMSG_NERR) {
    printf("[NERR]");
  }
  if (flag & canMSG_TXACK) {
    printf("[TXACK]");
  }
  if (flag & canMSG_TXRQ) {
    printf("[TXRQ]");
  }
  printf("\n");
}

void Check(char* id, canStatus stat)
{
  char buf[50];
  if (stat != canOK) {
    buf[0] = '\0';
    canGetErrorText(stat, buf, sizeof(buf));
    Error("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
    if (QuitOnError) exit(1);
  }
}

int InitCtrlWithFlags(int ctrl, int bitrate, int flags)
{
  int stat;
  int hnd;

  canInitializeLibrary();

  hnd = canOpenChannel(ctrl, flags);
  if (hnd < 0) Check("canOpenChannel", hnd);

  if (bitrate < 0) {
    stat = canSetBusParams(hnd, bitrate, 0, 0, 0, 0, 0);
    Check("canSetBusParams", stat);
  } else if (bitrate == 5000) {
    // Special case for 5k
    stat = canSetBusParams(hnd, 5000, 16, 8, 1, 1, 0);
    Check("canSetBusParams", stat);
  } else if (bitrate == 10000) {
    // Special case for 10k
    stat = canSetBusParams(hnd, 10000, 12, 7, 1, 1, 0);
    Check("canSetBusParams", stat);
  } else if (bitrate == 20000) {
    // Special case for 20k
    stat = canSetBusParamsC200(hnd, 0x58, 0x67);
    Check("canSetBusParamsC200", stat);
  } else if (bitrate == 40000) {
    // Special case for 40k
    stat = canSetBusParamsC200(hnd, 0x58, 0x14);
    Check("canSetBusParamsC200", stat);
  } else {
    Error("Bad bitrate sent to canInitCtrlWithFlags: %u",bitrate);
  }

  stat = canBusOn(hnd);
  Check("canBusOn", stat);

  stat = canSetBusOutputControl(hnd, canDRIVER_NORMAL);
  Check("canSetBusOutputControl", stat);

  return hnd;
}

int InitCtrl(int ctrl, int bitrate)
{
  return InitCtrlWithFlags(ctrl, bitrate, canOPEN_EXCLUSIVE|canOPEN_ACCEPT_VIRTUAL);
}

int ReInitialize(int ctrl, int channel, int br)
{
  canClose(ctrl);
  return InitCtrl(channel, br);
}

//extern int vsprintf(char *buffer, const char *format, va_list arglist);

static void setupHandles(int handle1, int handle2)
{
  canStatus stat;
  int i = 1; // Turn on txAcks
  stat = canIoCtl(handle1, canIOCTL_SET_TXACK, &i, 4);
  if (stat != canOK) {
    Check("canIoCtl() failed to turn on txAcks on handle1", stat);
  }
  
  stat = canIoCtl(handle2, canIOCTL_SET_TXACK, &i, 4);
  if (stat != canOK) {
    Check("canIoCtl() failed to turn on txAcks on handle2", stat);
  }
  
  i = 10; // Set resolution to 10 us
  stat = canIoCtl(handle1, canIOCTL_SET_TIMER_SCALE, &i, 4);
  if (stat != canOK) {
    Check("canIoCtl() failed to set 10 us resolution on the first channel", stat);
  }
  
  i = 10; // Set resolution to 10 us
  stat = canIoCtl(handle2, canIOCTL_SET_TIMER_SCALE, &i, 4);
  if (stat != canOK) {
    Check("canIoCtl() failed to set 10 us resolution on the second channel", stat);
  }
  
  stat = kvTimeDomainCreate(&mytd);
  if (stat != canOK) {
    Check("kvTimeDomainCreate() failed!", stat);
  }
  
  stat = kvTimeDomainAddHandle(mytd, handle1);
  if (stat != canOK) {
    Check("kvTimeDomainAddHandle() failed for handle1!", stat);
  }
  
  stat = kvTimeDomainAddHandle(mytd, handle2);
  if (stat != canOK) {
    Check("kvTimeDomainAddHandle() failed for handle2!", stat);
  }
  
  stat = kvTimeDomainResetTime(mytd);
  if (stat != canOK) {
    Check("kvTimeDomainResetTime() failed!", stat);
  }
  
  stat = kvTimeDomainGetData(mytd, &kvtData, sizeof(kvTimeDomainData));
  if (stat != canOK) {
    Check("kvTimeDomainGetData() failed!", stat);
  }
  
  if (Verbose) {
    if (kvtData.nMagiSyncGroups > 1) {
      printf("WARNING: More than one magisync group. Consider connecting the"
             " interfaces\n         to the same USB root hub!\n");
    }
    if ((kvtData.nMagiSyncGroups + kvtData.nNonMagiSyncCards) == 1) {
      printf("INFO: The used channels are either MagiSynced or reside on "
             "the same\n      physical interface. Expect time stamps to be"
             " as good as the hardware\n      specification claims.\n");
    }
    else if ((kvtData.nMagiSyncGroups + kvtData.nNonMagiSyncCards) == 2) {
      printf("INFO: The used channels don't appear synchronized in any way."
             " In each test\n      the time stamp of the first message from"
             " both channels is used as\n      an offset subtracted from all"
             " subsequent timestamps. Hence, since\n      local clocks tend"
             " to drift, expect the time stamps to differ more\n      the"
             " longer the test runs.\n");
    }
    else {
      printf("WARNING: kvTimeDomainGetData reports an unexpected number of "
             "interfaces!\n         (magisynced = %u, others = %u)\n",
             kvtData.nMagiSyncGroups, kvtData.nNonMagiSyncCards);
    }
    if ((kvtData.nMagiSyncedMembers + kvtData.nNonMagiSyncedMembers) != 2) {
      printf("WARNING: kvTimeDomainGetData reports an unexpected number of"
             " members!\n         (magisynced = %u, others = %u)\n",
             kvtData.nMagiSyncedMembers, kvtData.nNonMagiSyncedMembers);
    }
  }
}

static void resetHandles(int handle1, int handle2)
{
  canStatus stat;
  int i = 0; // Turn off txAcks
  stat = canIoCtl(handle1, canIOCTL_SET_TXACK, &i, 4);
  if (stat != canOK) {
    Check("canIoCtl() failed to turn off txAcks", stat);
  }
    
  i = 1000; // Set resolution to the default 1 ms
  stat = canIoCtl(handle1, canIOCTL_SET_TIMER_SCALE, &i, 4);
  if (stat != canOK) {
    Check("canIoCtl() failed to set 1 ms resolution on the first channel", stat);
  }
     
  i = 1000; // Set resolution to the default 1 ms
  stat = canIoCtl(handle2, canIOCTL_SET_TIMER_SCALE, &i, 4);
  if (stat != canOK) {
    Check("canIoCtl() failed to set 1 ms resolution on the second channel", stat);
  }
  
  stat = kvTimeDomainDelete(mytd);
  if (stat != canOK) {
    Check("kvTimeDomainDelete() failed!", stat);
  }
}

static void buildMsgBurst_std (void)
{
  int i;
  unsigned j;
  
  i1 = i2 = 0;  // Reset indices
  
  for (i=0; i<BURST_SIZE; i++) {
    msgs1[i].id = random(2048) & 0xfffffffe;  // make even
    msgs1[i].flags = canMSG_STD;
    msgs1[i].dlc = rand()%9;
    for (j=0; j<msgs1[i].dlc; j++) {
      msgs1[i].data[j] = rand();
    }
    
    msgs2[i].id = random(2048) | 0x00000001;  // make odd
    msgs2[i].flags = canMSG_STD;
    msgs2[i].dlc = rand()%9;
    for (j=0; j<msgs2[i].dlc; j++) {
      msgs2[i].data[j] = rand();
    }
  }
}

static void buildMsgBurst_mixed (void)
{
  int i;
  unsigned j;
  
  i1 = i2 = 0;  // Reset indices
  
  for (i=0; i<BURST_SIZE; i++) {
    if (rand() & 0x01) {
      msgs1[i].id = random(1<<11) & 0xfffffffe;  // make even
      msgs1[i].flags = canMSG_STD;
    } else {
      msgs1[i].id = random(1<<29) & 0xfffffffe;  // make even
      msgs1[i].flags = canMSG_EXT;
    }
    msgs1[i].dlc = rand()%9;
    for (j=0; j<msgs1[i].dlc; j++) {
      msgs1[i].data[j] = rand();
    }
    
    if (rand() & 0x01) {
      msgs2[i].id = random(1<<11) | 0x00000001;  // make odd
      msgs2[i].flags = canMSG_STD;
    } else {
      msgs2[i].id = random(1<<29) | 0x00000001;  // make odd
      msgs2[i].flags = canMSG_EXT;
    }
    msgs2[i].dlc = rand()%9;
    for (j=0; j<msgs2[i].dlc; j++) {
      msgs2[i].data[j] = rand();
    }
  }
}

static void buildMsgBurst_dlc0 (void)
{
  int i;
  
  i1 = i2 = 0;  // Reset indices
  
  for (i=0; i<BURST_SIZE; i++) {
    msgs1[i].id = random(2048) & 0xfffffffe;  // make even
    msgs1[i].flags = canMSG_STD;
    msgs1[i].dlc = 0;
    msgs2[i].id = random(2048) | 0x00000001;  // make odd
    msgs2[i].flags = canMSG_STD;
    msgs2[i].dlc = 0;
  }
}

static void sendMsgBurst (int burstNr, int handle1, int handle2)
{
  canStatus stat;
  int i;
  
  // Send a burst of messages on both channels
  for (i=0; i<BURST_SIZE; i++) {
    stat = canWrite(handle1, 
                    msgs1[i].id,
                    msgs1[i].data, 
                    msgs1[i].dlc, 
                    msgs1[i].flags);
    if (stat != canOK) {
      printSomeStuff();
      printf("line %d, burst %d, msg %d\n", __LINE__, burstNr, i);
      Check("canWrite() failed", stat);
    }
    
    stat = canWrite(handle2, 
                    msgs2[i].id,
                    msgs2[i].data, 
                    msgs2[i].dlc, 
                    msgs2[i].flags);
    if (stat != canOK) {
      printSomeStuff();
      printf("line %d, burst %d, msg %d\n", __LINE__, burstNr, i);
      Check("canWrite() failed", stat);
    }
  }
}

static void readMsgPair (int burstNr, int msgNr, int handle1, int handle2)
{
  canStatus stat;
  long errorCounter = 0;
  for(;;) {
    stat = canReadWait(handle1, &id1, &data1, &dlc1, &flag1, &(unsigned long)t1, 4000);
    if (stat != canOK) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      Check("canRead() failed on handle1", stat);
    }
    if (flag1 & (canMSGERR_HW_OVERRUN | canMSGERR_SW_OVERRUN)) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      Error("canRead() reported an overrun on handle1, flag1=0x%x", flag1);
    } else if (flag1 & canMSG_ERROR_FRAME) {
      h1Errors++;
      errorCounter++;
       printf(". 0x%02x ",flag1);
    } else {
      break;
    }
    if(errorCounter > 1000) {
      Error("readMsgPair() A found too many error frames",errorCounter);
    }
  }
  errorCounter = 0;
    
  for(;;) {
    stat = canReadWait(handle2, &id2, &data2, &dlc2, &flag2, &(unsigned long)t2, 4000);
    if (stat != canOK) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      Check("canRead() failed on handle2", stat);
    }
    if (flag2 & (canMSGERR_HW_OVERRUN | canMSGERR_SW_OVERRUN)) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      Error("canRead() reported an overrun on handle2, flag2=0x%x", flag2);
    } else if (flag2 & canMSG_ERROR_FRAME) {
      h2Errors++;
      errorCounter++;
      printf("* 0x%02x ",flag2);
    } else {
      break;
    }
    if(errorCounter > 1000) {
      Error("readMsgPair() B found too many error frames",errorCounter);
    }
  }
}

void printMsgs (void) {
  unsigned i;
  
  printf("c=1, t=%u, id=%x, l=%u, ", t1, id1, dlc1);
  for (i=0; i<dlc1; i++) printf("%02x", (unsigned char) data1[i]);
  printf(", ");
  printFlag(flag1);
  
  printf("c=2, t=%u, id=%x, l=%u, ", t2, id2, dlc2);
  for (i=0; i<dlc2; i++) printf("%02x", (unsigned char) data2[i]);
  printf(", ");
  printFlag(flag2);
}

static void compareMsgPair (int burstNr, int msgNr)
{
  unsigned i;
  
  if (flag1 & canMSG_TXACK && flag2 & canMSG_TXACK) {
    printSomeStuff();
    printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
    printMsgs();
    Error("The flags indicate tx_ack on both handles, flag1=%x, flag2=%x\n", flag1, flag2);
  }
  
  else if (flag1 & canMSG_TXACK) {
    // handle1 got a transmit ack and handle2 got a message
    
    if (((flag1 & (canMSG_STD | canMSG_EXT)) != (msgs1[i1].flags & (canMSG_STD | canMSG_EXT))) ||
        ((flag2 & (canMSG_STD | canMSG_EXT)) != (msgs1[i1].flags & (canMSG_STD | canMSG_EXT)))) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      printMsgs();
      Error("Sent and received msg identifier types don't match\n");
    }
    
    if (id1!=msgs1[i1].id || id2!=id1) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      printMsgs();
      Error("Ids don't match, msgs1[%d].id=%x, id1=%x, id2=%x\n", i1, msgs1[i1].id, id1, id2);
    }
    
    if (dlc1!=msgs1[i1].dlc || dlc2!=dlc1) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      printMsgs();
      Error("Dlc not correct, msgs1[%d].dlc=%d, dlc1=%d, dlc2=%d\n", i1, msgs1[i1].dlc, dlc1, dlc2);
    }
    
    for (i=0; i<dlc1; i++) {
      if (data1[i]!=msgs1[i1].data[i] || data1[i]!=data2[i]) {
        printSomeStuff();
        printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
        printMsgs();
        Error("Data wrong, msgs1[%d].data[%d]=%d, data1[%d]=%d, data2[%d]=%d\n",
               i1, i, msgs1[i1].data[i], i, data1[i], i, data2[i]);
      }
    }
    
    i1++;
  }
  
  else if (flag2 & canMSG_TXACK) {
    // handle2 got a transmit ack and handle1 got a message
    
    if (((flag1 & (canMSG_STD | canMSG_EXT)) != (msgs2[i2].flags & (canMSG_STD | canMSG_EXT))) ||
        ((flag2 & (canMSG_STD | canMSG_EXT)) != (msgs2[i2].flags & (canMSG_STD | canMSG_EXT)))) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      printMsgs();
      Error("Sent and received msg identifier types don't match\n");
    }
    
    if (id1!=msgs2[i2].id || id2!=id1) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      printMsgs();
      Error("Ids don't match, msgs2[%d].id=%x, id1=%x, id2=%x\n", i1, msgs2[i2].id, id1, id2);
    }
    
    if (dlc1!=msgs2[i2].dlc || dlc2!=dlc1) {
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      printMsgs();
      Error("Dlc not correct, msgs2[%d].dlc=%d, dlc1=%d, dlc2=%d\n", i1, msgs2[i2].dlc, dlc1, dlc2);
    }
    
    for (i=0; i<dlc1; i++) {
      if (data1[i]!=msgs2[i2].data[i] || data1[i]!=data2[i]) {
        printSomeStuff();
        printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
        printMsgs();
        Error("Data wrong, msgs2[%d].data[%d]=%d, data1[%d]=%d, data2[%d]=%d\n",
               i2, i, msgs2[i2].data[i], i, data1[i], i, data2[i]);
      }
    }
    
    i2++;
  } else {
    printSomeStuff();
    printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
    printMsgs();
    Error("None of the handles reported a tx_ack, flag1=%x, flag2=%x\n", flag1, flag2);
  }
}


static void compareTimeStamps(int burstNr, int msgNr)
{
  static baseDiff, t1old, t2old;
  
  if (burstNr==0 && msgNr==0) {
    if ((kvtData.nMagiSyncGroups == 1 && kvtData.nMagiSyncedMembers == 2) ||
        (kvtData.nNonMagiSyncCards == 1 && kvtData.nNonMagiSyncedMembers == 2)) {
      baseDiff = 0;
    }
    else {
      baseDiff = t1 - t2;
    }
  }
  else if (t1 == t1old || t2 == t2old) {
    if (FirstHardwareType != canHWTYPE_LAPCAN && SecondHardwareType != canHWTYPE_LAPCAN &&
        FirstHardwareType != canHWTYPE_VIRTUAL && SecondHardwareType != canHWTYPE_VIRTUAL) {
      //
      // Don't do this test for LAPcan or virtual channels because they might give several
      // message identical time stamps.
      //
      printSomeStuff();
      printf("burstNr %d, msgNr %d\n", burstNr, msgNr);
      printMsgs();
      Error("compareTimeStamps() Time not increasing: "
            "t1 = %u, t1old = %u, t2 = %u, t2old = %u us\n",
            t1*10, t1old*10, t2*10, t2old*10);
    }
  }
  t1old = t1;
  t2old = t2;
  
  tTmp = t1-t2 - baseDiff ;
  
  if (tTmp>tMax) tMax = tTmp;
  if (tTmp<tMin) tMin = tTmp;
  
  if (abs(tTmp) < MAX_DIFF-1) {
    ndiff[tTmp+MAX_DIFF]++;
  } else {
    printSomeStuff();
    printMsgs();
    Error("compareTimeStamps(): burstNr=%d, msgNr=%d, "
          "t1 - t2 - baseDiff = %d - %d - %d = %d us\n",
          burstNr, msgNr, t1*10, t2*10, baseDiff*10, (t1-t2)*10);
  }
}


static void printSomeStuff (void)
{
  if (Verbose>1) {
    int i, nMsgs=0;
    int tTest=timeGetTime()-tStart;
    if (!tTest) tTest=1;
    printf("\n");
    for (i=0; i<MAX_DIFF*2; i++) {
      if (ndiff[i]) {
        nMsgs += ndiff[i];
        printf("INFO: %5u msg%s differed %6d us\n",
               ndiff[i], 
               ndiff[i]>1 ? "s" : " ", 
               (i - MAX_DIFF)*10);
      }
    }
    nTot += nMsgs;
    h1ErrTot += h1Errors;
    h2ErrTot += h2Errors;
    tMaxDiff = max(tMaxDiff, tMax-tMin);
    printf("INFO: maxDiff = %d us (%d us)\n", (tMax-tMin)*10, tMaxDiff*10);
    printf("INFO: time=%d ms, msg count=%u (%u), %u msgs/s\n",
           tTest, nMsgs, nTot, (nMsgs*1000)/tTest);
    if (h1Errors || h2Errors || h1ErrTot || h2ErrTot)
      printf("INFO: Error frames were detected: "
             "%u (%u) on handle1 and %u (%u) on handle2\n",
             h1Errors, h1ErrTot, h2Errors, h2ErrTot);
  }
}

#define NR_OF_TESTS 3

char testName[NR_OF_TESTS][20] = {"1: Std msgs", "2: Short msgs", "3. Mixed msgs"};

void (*buildMsgBurst[NR_OF_TESTS])(void) = {
     buildMsgBurst_std, 
     buildMsgBurst_dlc0,
     buildMsgBurst_mixed
  };

int timeStampTester (int handle1, int handle2)
{
  int i, j, k;
  
  setupHandles(handle1, handle2);
  
  for (k=0; k<NR_OF_TESTS; k++) {
    tMax=-2000000000;
    tMin=2000000000;
    h1Errors=0;
    h2Errors=0;
    memset(ndiff, 0, sizeof(ndiff));
    beginTest(testName[k]);
    
    tStart=timeGetTime();
    for (i=0; i<NUMBER_OF_BURSTS; i++) {
      Blipp();
      
      // Generate a set of messages to be sent
      buildMsgBurst[k]();
      
      // Send the generated messages
      sendMsgBurst(i, handle1, handle2);
      
      // Read all messages on both handles and compare timestamps...
      for (j=0; j<2*BURST_SIZE; j++) {
        
        // Read one message from each handle
        readMsgPair(i, j, handle1, handle2);
          
        // Make sure that both messages are equal and that they are the expected pair
        compareMsgPair(i, j);
        
        // Compare the timestamps
        compareTimeStamps(i, j);
      }
    }
    NoBlipp();
    printSomeStuff();
  }

  if (tMax-tMin > DIFF_ALLOWED)
    Error("Timestamps differ to much (%d us)\n", (tMax-tMin)*10);
  
  resetHandles(handle1, handle2);

  return TRUE;
}

void PerformTest(int argc, char **argv)
{
    int handle1, handle2;
    int br, i;

    TIMECAPS tmcps;
    int needTimeEndPeriod = 0;
    
    switch (Bitrate) {
        case 1000000:
            br = canBITRATE_1M;
            break;
        case 500000:
            br = canBITRATE_500K;
            break;
        case 250000:
            br = canBITRATE_250K;
            break;
        case 125000:
            br = canBITRATE_125K;
            break;
        case 100000:
            br = canBITRATE_100K;
            break;
        case 62500:
            br = canBITRATE_62K;
            break;
        case 50000:
            br = canBITRATE_50K;
            break;
        case 20000:
            br = Bitrate;
            break;
        case 10000:
            br = Bitrate;
            break;
        case 5000:
            br = Bitrate;
            break;
        default:
            // qqq
            printf("Unsupported bitrate, choose 1000/500/250/125/100/62.5/50/20/10/5 k.\n");
            exit(1);
    }
    BitrateConverted = br;
    
    if (timeGetDevCaps(&tmcps, sizeof(TIMECAPS)) == TIMERR_NOERROR) {
      needTimeEndPeriod = 1;
      // Set resolution as low as possible
      timeBeginPeriod(tmcps.wPeriodMin);
      if (Verbose && tmcps.wPeriodMin >= 8)
        printf("PC-timer resolution set to %d\n", tmcps.wPeriodMin);
    } else {
      if (Verbose)
        printf("Couldn't set PC-timer resolution\n");
    }
    
    handle1 = InitCtrl(First, br);
    handle2 = InitCtrl(Second, br);

    for (i=0; i<LoopCount; i++) {

        if (Verbose) {
            int j;
            printf("\n*** Loop %d *** (", i);
            for (j=0; j<argc; j++) printf("%s ", argv[j]);
            printf(")\n");
            fflush(stdout);
        }
        
        canBusOff(handle1);
        canBusOff(handle2);            
        handle1 = ReInitialize(handle1, First, br);
        handle2 = ReInitialize(handle2, Second, br);
        timeStampTester(handle1, handle2);
        
        if (_kbhit() && _getch() == 27) {  // ESC hit?
          break;
        }
    }
    
    canClose(handle1);
    canClose(handle2);

    if (needTimeEndPeriod) {
      timeEndPeriod(tmcps.wPeriodMin); // reset resolution
    }
}



void main(int argc, char* argv[])
{
    int i;
    canStatus stat;
    char tmpS[256];

    // Default values.
    First = 0;
    Second = 1;
    Bitrate = 1000000;
    LoopCount = 1;

    Verbose = 2;
    QuitOnError = 1;

    ErrorsFound = 0;
    FirstHardwareType = 0;
    SecondHardwareType = 0;
    
    if (argc <= 1) Usage();
    
    // Parse the command line.
    for (i=1; i<argc; i++) {
        int tmp;
        char c;
        if (strcmp(argv[i], "-s") == 0) Verbose=0;
        else if (strcmp(argv[i], "-silent") == 0) Verbose=0;
        else if (strcmp(argv[i], "-i") == 0) QuitOnError = FALSE;
        else if (sscanf(argv[i], "-a%d%c", &tmp, &c) == 1) First = tmp;
        else if (sscanf(argv[i], "-b%d%c", &tmp, &c) == 1) Second = tmp;
        else if (sscanf(argv[i], "-B%d%c", &tmp, &c) == 1) Bitrate = tmp;
        else if (sscanf(argv[i], "-randomize=%d%c", &tmp, &c) == 1) srand(tmp);
        else if (sscanf(argv[i], "-r=%d%c", &tmp, &c) == 1) srand(tmp);
        else if (strcmp(argv[i], "-r") == 0) randomize();
        else if (strcmp(argv[i], "-randomize") == 0) randomize();
        else if (sscanf(argv[i], "-L%d%c", &tmp, &c) == 1) LoopCount = tmp;
        else if (sscanf(argv[i], "-L=%d%c", &tmp, &c) == 1) LoopCount = tmp;
        else Usage();
    }

    printf("Starting test of time stamps with Kvaser CANLIB API.\n");
    printf("Time stamp resolution:  10 us\n\n");
    
    stat = canLocateHardware();
    Check("canLocateHardware", stat);
    
    stat = canGetChannelData(First, canCHANNELDATA_CHANNEL_NAME, tmpS, sizeof(tmpS));
    Check("canGetChannelData", stat);
    if (Verbose) printf("First channel:  %s.\n", tmpS);
    
    stat = canGetChannelData(Second, canCHANNELDATA_CHANNEL_NAME, tmpS, sizeof(tmpS));
    Check("canGetChannelData", stat);
    if (Verbose) printf("Second channel: %s.\n", tmpS);
    
    stat = canGetChannelData(First, canCHANNELDATA_CARD_TYPE, &FirstHardwareType, sizeof(FirstHardwareType));
    Check("canLocateHardware", stat);
    
    stat = canGetChannelData(Second, canCHANNELDATA_CARD_TYPE, &SecondHardwareType, sizeof(SecondHardwareType));
    Check("canLocateHardware", stat);
    
    PerformTest(argc, argv);

    if (ErrorsFound) {
        printf("\nTest completed with %d ERRORS.\n", ErrorsFound);
    } else {
        printf("\nTest SUCCESSFULLY completed.\n");
    }
    exit(0);
}





