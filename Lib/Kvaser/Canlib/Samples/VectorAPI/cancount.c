/*
**                   Copyright 2006 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**  Sample program demonstrating Vector's CAN API.
** ---------------------------------------------------------------------------
*/
#define  STRICT
#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include "vcand.h"

///////////////////////////////////////////////////////////////////////
// globals

VportHandle gPortHandle      = INVALID_PORTHANDLE;
Vaccess gChannelMask         = 0;
Vaccess gPermissionMask      = 0;
VDriverConfig *gDriverConfig = 0;
unsigned int gBitRate        = 1000000;
HANDLE threadHandle          = 0;
DWORD threadId               = 0;
HANDLE gEventHandle          = 0;
int threadRuning             = 0;
int gHwType                  = -1;
int gHwChannel               = 0;
unsigned long gRxCount       = 0;
unsigned long gRxOverflows   = 0;
int gLogEvents               = 0;
FILE *gLogFile               = 0;
char gLogFileName[256+1];

///////////////////////////////////////////////////////////////////////
// InitDriver ()
//---------------------------------------------------------------------
// Initializes the CAN driver.

static Vstatus InitDriver(int hwType, int hwChannel)
{
  Vstatus vErr;
  VsetAcceptance acc;

  // Open the driver
  vErr = ncdOpenDriver ();
  if (vErr) goto error;

  // Select a channel
  gChannelMask = ncdGetChannelMask(hwType, 0, hwChannel);
  if (gChannelMask == 0) return VERR_HW_NOT_PRESENT;

  // Open a port
  gPermissionMask = gChannelMask;
  vErr = ncdOpenPort(&gPortHandle, "CANcount",
                     gChannelMask, gPermissionMask,
                     &gPermissionMask, 1024);
  if (vErr) goto error;

  // If permission to initialize
  if (gPermissionMask) {

    // Initialize the channels
    vErr = ncdSetChannelBitrate(gPortHandle, gPermissionMask, gBitRate);
    if (vErr) goto error;

    // Reset the clock
    vErr = ncdResetClock(gPortHandle);
    if (vErr) goto error;

  } else {
    printf("No init access\n");
  }

  // Disable the TX and TXRQ notifications
  vErr = ncdSetChannelMode(gPortHandle, gChannelMask, 0, 0);
  if (vErr) goto error;

  // Set the acceptance filter
  acc.mask = 0x000; // relevant=1
  acc.code = 0x000;
  vErr = ncdSetChannelAcceptance(gPortHandle, gChannelMask, &acc);
  if (vErr) goto error;

  return VSUCCESS;
  

error:
  printf("ERROR: %s\n", ncdGetErrorString(vErr));

  if (gPortHandle != INVALID_PORTHANDLE) {
    ncdClosePort(gPortHandle);
    gPortHandle = INVALID_PORTHANDLE;
  }

  return vErr;
}



///////////////////////////////////////////////////////////////////////
// CleanUp()
//---------------------------------------------------------------------
// close the port and the CAN driver

static Vstatus CleanUp(void)
{
  ncdClosePort(gPortHandle);
  gPortHandle = INVALID_PORTHANDLE;
  ncdCloseDriver();
  return VSUCCESS; // No error handling
}


// command line help
static void usage(void)
{
  printf(
         "usage:\n"
         "  CANCOUNT [options]\n"
         "\n"
         "  Options:\n"
         "    -v   use a virtual channel\n"
         "    -hwN use hardware type N (any of HWTYPE_xxx)\n"
         "    -2   use channel 2 (default channel 1)\n"
         "    -l   log messages on screen\n"
         "    -bx  set bitrate to x (default=1000000)\n"
         "\n"
         "Either of -v or -hwN must be specified.\n"
        );
  exit(1);
}

// The receive thread
DWORD WINAPI thread(PVOID par) {

  Vstatus       vErr;
  Vevent        *pEvent;

  threadRuning = 1;
  while (threadRuning) {

    // wait for receive event
    WaitForSingleObject(gEventHandle, 1000);

    // receive
    for (;;) { // read all events

      vErr = ncdReceive1(gPortHandle, &pEvent);
      if (vErr && vErr != VERR_QUEUE_IS_EMPTY) goto ncdError;
      if (vErr == VERR_QUEUE_IS_EMPTY) break;
      if (gLogEvents) printf("%s\n", ncdGetEventString(pEvent));
      if (pEvent->tag == V_RECEIVE_MSG) {
        if (gLogFile) fwrite(pEvent, sizeof(Vevent), 1, gLogFile);
        if (pEvent->tagData.msg.flags & MSGFLAG_OVERRUN) gRxOverflows++;
        gRxCount++;
      }
    } // for
  }
  return 0;

ncdError:
  printf("ERROR: %s\n", ncdGetErrorString(vErr));
  return 0;
}


///////////////////////////////////////////////////////////////////////
// main()
//---------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
  Vstatus       vErr;
  int           end, i;
  unsigned int  u;
  unsigned int lastRxCount;
  unsigned int time, lastTime;


  printf(
         "CANcount (Built at " __DATE__ " " __TIME__ ")\n"
        );

  // parse the command line
  for (i=1; i<argc; i++) {
    int tmpL;

    if (_stricmp(argv[i], "-h") == 0) {
      usage();
    }
    else if (_stricmp(argv[i], "-v") == 0) {
      gHwType = HWTYPE_VIRTUAL;
    }
    else if (sscanf(argv[i], "-hw%d", &tmpL) == 1) {
      gHwType = tmpL;
    }
    else if (_stricmp(argv[i], "-2") == 0) {
      gHwChannel = 1;
      printf("Using channel 2.\n");
    }
    else if (_stricmp(argv[i], "-l") == 0) {
      gLogEvents = 1;
      printf("Logging activated\n");
    }
    else if (sscanf(argv[i], "-b%u", &u) == 1) {
      if (u>5000 && u<=1000000)
        gBitRate = u;
      else
        usage();
    }
    else if (sscanf(argv[i], "-f%s", &gLogFileName) == 1) {
      gLogFile = fopen(gLogFileName, "wb");
    }
    else {
      usage();
    }
  }
  if (gHwType < 0) usage();
  
  printf("Hardware = %u\n", gHwType);
  printf("Channel = %u\n", gHwChannel+1);
  printf("Bitrate = %u BPS\n", gBitRate);
  if (gLogFile) printf("Logfile = %s\n", gLogFileName);
  printf("\n");

  // initialize the CAN driver
  vErr = InitDriver(gHwType, gHwChannel);
  if (vErr) goto error;

  // create a synchronisation object
  gEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
  vErr = ncdSetNotification(gPortHandle, (unsigned long*)&gEventHandle, 1);
  if (vErr) goto ncdError;

  // create a thread
  threadHandle = CreateThread(0, 0x1000, thread, 0, 0, &threadId);
  SetThreadPriority(threadHandle, THREAD_PRIORITY_NORMAL);

  // channel on bus
  vErr = ncdActivateChannel(gPortHandle, gChannelMask);
  if (vErr) goto ncdError;

  // main loop
  end = 0;
  lastTime = GetTickCount();
  lastRxCount = 0;

  while (!end) {
    Sleep(500);
    time = GetTickCount();
    if (time != lastTime) {
      printf("RX = %u msg/s, OVERFLOWS = %u    \r",
             ((gRxCount-lastRxCount)*1000)/(time-lastTime),
             gRxOverflows);
      lastTime = time;
      lastRxCount = gRxCount;
    }

    // check keyboard
    if (_kbhit()) {
      switch (_getch()) {
        case 27:
          end = 1;
          break;
        default:
          break;
      }
    }
  }


error:
  // stop thread
  if (threadHandle) {
    threadRuning = 0;
    WaitForSingleObject(threadHandle, 5000);
  }

  ncdDeactivateChannel(gPortHandle, gChannelMask);
  CleanUp();
  if (gLogFile) fclose(gLogFile);
  return 0;

ncdError:
  printf("ERROR: %s\n", ncdGetErrorString(vErr));
  goto error;

}
