/*
**                   Copyright 2006-2007 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**  Example program for J1587 Linx.
**  Use together with two J1587 linx units connected with a loopback cable
**  Supply 12 VDC to pin 9. No termination, pullup etc needed.
** ---------------------------------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <conio.h>
#include <mmsystem.h>

#include "j1587lib.h"
#include "canlib.h"
#include "j1587example.h"
#include "util.h"


// defines

#define j1587WriteMessage(a,b,c) j1587WriteMessageWait(a,b,c,1,1000)

#define LENGTH_1_1  6
#define LENGTH_1_4  8

#define MAX_TIMER_DIFF 20
// Limit the inaccuracy of each measurement
#define MAX_MEASURE_DIFF (MAX_TIMER_DIFF/3)

#define ERR_ARGUMENTS         8001
#define ERR_BITRATE           8002
#define ERR_WRONG_ID_OR_DATA  8003
#define ERR_WRONG_ID          8004
#define ERR_WRITE_TIMEOUT     8005
#define ERR_WRITE_FAILED      8006
#define ERR_STR_MISMATCH      8007
#define ERR_WAKEUP            8008
#define ERR_NOTHING_TO_READ   8009
#define ERR_TIME_DIFF         8010
#define ERR_ILLEGAL_MSG       8011
#define ERR_LEGAL_MSG         8012

J1587Handle Master;
J1587Handle Slave;
int Bitrate;
int LoopCount;
char ListOfTests[64];

//--------------------------------------------------------------------
void Usage (void)
{
  printf("\nThis program demonstrates the Kvaser Linx J1587 APIs.\n");
  printf("(The progran is a part of CANLIB SDK, see http://www.kvaser.com\n");
  printf("\nUsage: program [flags]\n");
  printf("   -mX              Use Channel X as Normal\n");
  printf("   -sY              Use Channel Y as Node\n");
  printf("   (note: X must be different from Y. Default X=0, Y=1)\n");
  printf("   -Bnnn            Set the speed to nnn bit/s [default 10 000 bit/s].\n");
  printf("   -Lnnn            Run nnn loops.\n");
  printf("   -T=xxxx          Run only tests xxxx where x is a letter a..z\n");
  printf("   -NT=xxxx         Don't run tests xxxx where x is a letter a..z\n");
  printf("   -test=n          Run only test number n.\n");

  exit(ERR_ARGUMENTS);
}

//--------------------------------------------------------------------
int main (int argc, char* argv[])
{
  int i;
  
  // Default values.
  Master = 0;
  Slave = 1;
  Bitrate = 9600;
  LoopCount = 1;
  strcpy(ListOfTests, "abcdefghijklmnopqrstuvwxyz");

  // Parse the command line.
  for (i = 1; i < argc; i++) {
    int tmp;
    char c;
    char tmpS[1024];
    
    if (sscanf(argv[i], "-m%d%c", &tmp, &c) == 1) Master = tmp;
    else if (sscanf(argv[i], "-s%d%c", &tmp, &c) == 1) Slave = tmp;
    else if (sscanf(argv[i], "-B%d%c", &tmp, &c) == 1) Bitrate = tmp;
    else if (sscanf(argv[i], "-L%d%c", &tmp, &c) == 1) LoopCount = tmp;
    else if (sscanf(argv[i], "-L=%d%c", &tmp, &c) == 1) LoopCount = tmp;
    else if (sscanf(argv[i], "-T=%s", tmpS) == 1) {
      if (strchr(tmpS, ',') != NULL) {
        // Format -T=n,n,n,n
        char *s;
        strcpy(ListOfTests, "");
        s = strtok(tmpS, ",");
        while (s != NULL) {
        char tmp[2];
        tmp[0] = atoi(s) + 'a' - 1;
        tmp[1] = 0;
        strcat(ListOfTests, tmp);
        s = strtok(NULL, ",");
        }
      }
      else {
        // Format -T=abcdefg
        strcpy(ListOfTests, tmpS);
      }
    }
    else if (sscanf(argv[i], "-NT=%s", tmpS) == 1) {
      if (strchr(tmpS, ',') != NULL) {
        // Format -NT=n,n,n,n
        char *s;
        s = strtok(tmpS, ",");
        while (s != NULL) {
          ListOfTests[atoi(s)-1] = '@';
          s = strtok(NULL, ",");
        }
      } else {
        // Format -NT=abcdefg
        int i;
        for (i=0; i<(int)strlen(tmpS); i++) {
          char *c = strchr(ListOfTests, tmpS[i]);
          if (c) *c = '@';  // This means 'no test'
        }
      }
    }
    else if (sscanf(argv[i], "-test=%d%c", &tmp, &c) == 1) {
      ListOfTests[0] = 'a' + tmp - 1;
      ListOfTests[1] = '\0';
    }
    else Usage();
  }    
  
  PerformTest(argc, argv);
  return 0;
}

//--------------------------------------------------------------------
void PerformTest (int argc, char **argv)
{
  int masterHandle, slaveHandle;
  int i;
  J1587Status stat;

  printf("*** Begin Test ***\n");

  if (Bitrate > 20000)
  {
    printf("Unsupported bitrate, choose max bitrate 20 000 kbits/s");
    exit(ERR_BITRATE);
  }

  j1587InitializeLibrary();

  masterHandle = j1587OpenChannel(Master, J1587_NORMAL | J1587_WRITE);
  CheckAndPrintError(masterHandle, "ERROR: j1587OpenChannel M failed");
  
  slaveHandle =  j1587OpenChannel(Slave, J1587_NORMAL | J1587_READ);
  CheckAndPrintError(slaveHandle, "ERROR: j1587OpenChannel S failed");

  stat = j1587SetBitrate(masterHandle, Bitrate);
  CheckAndPrintError(stat, "ERROR: j1587SetBitrate M failed");
  
  stat = j1587SetBitrate(slaveHandle, Bitrate);
  CheckAndPrintError(stat, "ERROR: j1587SetBitrate S failed");
  
  stat = j1587BusOn(masterHandle);
  CheckAndPrintError(stat, "ERROR: j1587BusOn M failed");
  
  stat = j1587BusOn(slaveHandle);
  CheckAndPrintError(stat, "ERROR: j1587BusOn S failed");

  for (i = 0; i < LoopCount; i++) {

    int j;
    printf("\n*** Loop %d *** (", i);
    for (j = 0; j < argc; j++)
      printf("%s ", argv[j]);
    printf(")\n");
    fflush(stdout);
    for (j = 0; j < (int)strlen(ListOfTests); j++) {
      int run = 1;
      switch (ListOfTests[j] - 'a' + 1) {
        case 0:  continue;
        case 1:  Test1(masterHandle, slaveHandle); break;
        case 2:  Test2(masterHandle, slaveHandle); break;
        case 3:  Test3(masterHandle, slaveHandle); break;
        case 4:  Test4(masterHandle, slaveHandle); break;
        default:
          run = 0;
          break;
      }
      if (run) {
        printf("\n");
        fflush(stdout);
      }
    }
    if (_kbhit() && _getch() == 27) {
      break;
    }
  }

  stat = j1587BusOff(masterHandle);
  CheckAndPrintError(stat,"ERROR: j1587BusOff M failed");

  stat = j1587BusOff(slaveHandle);
  CheckAndPrintError(stat,"ERROR: j1587BusOff S failed");

  stat = j1587Close(masterHandle);
  CheckAndPrintError(stat,"ERROR: j1587Close M failed");

  stat = j1587Close(slaveHandle);
  CheckAndPrintError(stat,"ERROR: j1587Close S failed");

  printf("\n*** Test Completed Successfully (%d loops) ***\n", LoopCount);
}

//--------------------------------------------------------------------
int setup(int handle)
{
  return j1587Configure(handle,
                        J1587_REPORT_BAD_CHECKSUM |
                        J1587_REPORT_FRAME_DELAY  |
                        J1587_REPORT_CHAR_DELAY);
}

//--------------------------------------------------------------------
// simple test - update, request and read
void Test1(int mHandle, int sHandle)
{
  int i;
  int curId = 0;
  int rec, sent;
  int loop = 0;
  int stat;
  char buffer[256];
  unsigned char msg[256];
  unsigned int len;
  J1587MessageInfo msgInfo;
  DWORD dt, t0;

  stat = j1587BusOff(mHandle);
  CheckAndPrintError(stat,"ERROR: j1587BusOff M failed");
  stat = j1587BusOff(sHandle);
  CheckAndPrintError(stat,"ERROR: j1587BusOff S failed");

  stat = j1587BusOn(mHandle);
  CheckAndPrintError(stat,"ERROR: j1587BusOn M failed");
  stat = j1587BusOn(sHandle);
  CheckAndPrintError(stat,"ERROR: j1587BusOn S failed");

  stat = setup(mHandle);
  CheckAndPrintError(stat, "ERROR: j1587SetupJ1587 M failed");

  stat = setup(sHandle);
  CheckAndPrintError(stat, "ERROR: j1587SetupJ1587 S failed");

  // Send a number of messages from master
  // Read all messages from slave and make sure all messages arrived
  printf("1.1 ");

  rec = 0;
  sent = 25;
  for (i = 0; i < sent; i++)
  {
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "KV %d", curId + i);
    stat = j1587WriteMessage(mHandle, buffer, LENGTH_1_1);
    CheckAndPrintError(stat, "ERROR: j1587WriteMessage failed");
  }
  stat = j1587WriteSync(mHandle, &msgInfo, 1000);
  CheckAndPrintError(stat, "ERROR: j1587WriteSync failed");

  t0 = timeGetTime();
  while(rec < sent)
  {
    if (j1587ReadMessageWait(sHandle, msg, &len, &msgInfo, 10) == j1587OK) {
      memset(buffer, 0, sizeof(buffer));
      sprintf(buffer, "KV %d", rec);
      if ((len != LENGTH_1_1) || memcmp(msg, buffer, len)) {
        printf("Bad data returned for message %d (%d/%d)\n",
               rec, len, LENGTH_1_1);
        for (i = 0; i < (int)len; i++) {
          printf("%02x/%02x ", msg[i], buffer[i]);
        }
        printf("\n");
      }
      rec++;
    }
    if (timeGetTime() - t0 > (unsigned int)(sent * 20))
      break;
  }

  if (rec != sent)
  {
    printf("ERROR: Sent %d, got %d\n", sent, rec);
  }

  // Tests writeSync
  printf("1.2 ");

  t0 = timeGetTime();
  stat = j1587WriteSync(mHandle, &msgInfo, 100);
  dt = timeGetTime() - t0;
  CheckAndPrintError(stat,"ERROR: j1587WriteSync failed");

  if (dt > 30) {
    printf("ERROR: j1587WriteSync took %d ms\n", dt);
    exit(ERR_WRITE_TIMEOUT);
  }
  // qqq add test here

  // Take master offbuss and try to send a message
  // Go on bus again and try to send and receive
  printf("1.3 ");
  stat = j1587BusOff(mHandle);
  CheckAndPrintError(stat,"ERROR: j1587BusOff failed");

  stat = j1587WriteMessage(mHandle, buffer, 8);
  if (stat != j1587ERR_NOTRUNNING)
  {
    printf("ERROR: j1587WriteMessage failed %d\n", stat);
    exit(ERR_WRITE_FAILED);
  }

  stat = j1587BusOn(mHandle);
  CheckAndPrintError(stat,"ERROR: j1587BusOn failed");

  stat = j1587WriteMessage(mHandle, buffer, 8);
  CheckAndPrintError(stat,"ERROR: j1587WriteMessage failed");

  stat = j1587WriteSync(mHandle, &msgInfo, 1000); 
  CheckAndPrintError(stat,"ERROR: j1587WriteSync failed");

  j1587ReadMessageWait(sHandle, msg, &len, &msgInfo, 1000);
  CheckAndPrintError(stat,"ERROR: j1587ReadMessageWait failed");

  stat = setup(mHandle);
  CheckAndPrintError(stat, "ERROR: j1587SetupJ1587 M failed");

  stat = setup(sHandle);
  CheckAndPrintError(stat, "ERROR: j1587SetupJ1587 S failed");

  // Send a number of messages from normal
  // Read all messages from node and make sure all messages arrived
  printf("1.4 ");

  {
    rec = 0;
    sent = 25;
    
    for (i = 0; i < sent; i++) {
      memset(buffer, 0, sizeof(buffer));
      sprintf(buffer, "KV %d", curId + i);
      stat = j1587WriteMessage(mHandle, buffer, LENGTH_1_4);
      CheckAndPrintError(stat,"ERROR: j1587WriteMessage failed");
    }
    stat = j1587WriteSync(mHandle, &msgInfo, 3000);
    CheckAndPrintError(stat,"ERROR: j1587WriteSync failed");
    
    t0 = timeGetTime();
    while (rec < sent) {
      if (j1587ReadMessageWait(sHandle, msg, &len, &msgInfo, 10) == j1587OK) {
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "KV %d", rec);
        if ((len != LENGTH_1_4) || memcmp(msg, buffer, len)) {
          printf("Bad data returned for message %d (%d/%d)\n",
                 rec, len, LENGTH_1_4);
          for(i = 0; i < (int)len; i++) {
            printf("%02x/%02x ", msg[i], buffer[i]);
          }
          printf("\n");
        }
        rec++;
      }

      if (timeGetTime() - t0 > (unsigned int)(sent * 20))
        break;
    }
    
    if (rec != sent) {
      printf("ERROR: Sent %d, got %d\n", sent, rec);
    }
  }
  
}
//--------------------------------------------------------------------

// Get info
void Test2(int mHandle, int sHandle)
{
  unsigned char bootVerMajor[256];
  unsigned char bootVerMinor[256];
  unsigned char bootVerBuild[256];
  unsigned char appVerMajor[256];
  unsigned char appVerMinor[256];
  unsigned char appVerBuild[256];
  int stat;

  printf("2.1 ");
      
  stat = j1587GetFirmwareVersion(mHandle, bootVerMajor, bootVerMinor, bootVerBuild,
                               appVerMajor, appVerMinor, appVerBuild);
  CheckAndPrintError(stat,"ERROR: j1587GetFirmwareVersion failed");

  //printf("\nBoot ver %d.%d.%d\n", bootVerMajor, bootVerMinor, bootVerBuild);
  //printf("App ver %d.%d.%d\n", appVerMajor, appVerMinor, appVerBuild);
  // qqq junk???


}

// Same as canlib\Src\test\Test7\test8.c
// Tests j1587ReadTimer
//--------------------------------------------------------------------
void Test3(int mHandle, int sHandle)
{
  int i;
  DWORD t0, dt, t1, t2, delta;

  printf("3.1 ");

  do {
    t2 = timeGetTime();
    t0 = j1587ReadTimer(mHandle);
    t1 = timeGetTime();
  } while (abs(t2 - t1) > MAX_MEASURE_DIFF);

  dt = t1 - t0;

  for (i = 0; i < 100; i++) {

    do {
      t2 = timeGetTime();
      t0 = j1587ReadTimer(mHandle);
      t1 = timeGetTime();
    } while (abs(t2 - t1) > MAX_MEASURE_DIFF);

    delta = t1 - t0;

    if ((abs(delta - dt)) > MAX_TIMER_DIFF) {
            // The resolution of timeGetTime() is set as low as possible
            // by testur.c. (hardware dependent, but most likely 1 ms)
      printf("3.1 Delta T=%d (ms), loops=%d", abs(delta - dt), i);
      exit(ERR_TIME_DIFF);
    }
  }

  printf("3.2 ");
  do {
    t2 = timeGetTime();
    t0 = j1587ReadTimer(mHandle);
    t1 = timeGetTime();
  } while (abs(t2 - t1) > MAX_MEASURE_DIFF);

  dt = t1 - t0;

  for (i = 0; i < 50; i++) {

    do {
      t2 = timeGetTime();
      t0 = j1587ReadTimer(mHandle);
      t1 = timeGetTime();
    } while (abs(t2 - t1) > MAX_MEASURE_DIFF);

    delta = t1 - t0;

    if ((abs(delta - dt)) > MAX_TIMER_DIFF) {
            // The resolution of timeGetTime() is set as low as possible
            // (hardware dependent, but most likely 1 ms)
      printf("3.2 Delta T=%d (ms), loops=%d", abs(delta - dt), i);
      exit(ERR_TIME_DIFF);
    }
    Sleep(10);
  }
}
//--------------------------------------------------------------------

// test j1587SetupIllegalMessage and j1587SetupLIN
void Test4(int mHandle, int sHandle)
{
    char buffer[8];
    int stat;
    unsigned char msg[8];
    unsigned int len;
    J1587MessageInfo msgInfo;

    stat = j1587BusOff(mHandle);
    CheckAndPrintError(stat,"ERROR: j1587BusOff M failed");
    stat = j1587BusOff(sHandle);
    CheckAndPrintError(stat,"ERROR: j1587BusOff S failed");

    stat = j1587BusOn(mHandle);
    CheckAndPrintError(stat,"ERROR: j1587BusOn M failed");
    stat = j1587BusOn(sHandle);
    CheckAndPrintError(stat,"ERROR: j1587BusOn S failed");

    Sleep(125);

    sprintf(buffer, "Legal");
    
    printf("4.1 ");

    stat = j1587WriteMessage(mHandle, buffer, sizeof(buffer));
    CheckAndPrintError(stat,"ERROR: j1587WriteMessage failed");

    Sleep(125);

    stat = j1587ReadMessageWait(sHandle, msg, &len, &msgInfo, 1000);
    CheckAndPrintError(stat,"ERROR: j1587ReadMessageWait failed");
}
