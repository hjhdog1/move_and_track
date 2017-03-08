/*
**                   Copyright 2004 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**  Sample program for linlib.dll
**
**  Run it in two different console windows:
**  1) linTest 0
**  2) linTest -master 1
**
**  (1) will then operate as a slave on channel 0, and (2) will operate
**  as a master on channel 1.
** ---------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "linlib.h"

void printLinMessage(unsigned int id, unsigned char *msg, unsigned int len,
                     unsigned int flags, LinMessageInfo *msgInfo);


int optMaster = 0;
int linChannel = 0;

int main(int argc, char **argv)
{
  unsigned char serNo[8], eanNo[8];
  int ttype;
  int i;
  LinHandle h;
  LinStatus lres;
  unsigned long timeOffset;
  unsigned long tLastPoll, tNow;
  
  linInitializeLibrary();

  for (i=1; i<argc; i++) {
    int tmpL;
    char tmpC;
    if (strcmp(argv[i], "-m") == 0) optMaster++;
    else if (sscanf(argv[i], "%d%c", &tmpL, &tmpC) == 1) {
      linChannel = tmpL;
    }
  }

  lres = linGetTransceiverData(linChannel, eanNo, serNo, &ttype);
  if (lres != linOK) {
    printf("Error retrieving LIN transceiver information on channel %d.\n", linChannel);
    exit(1);
  }
  printf("Transceiver type: %d", ttype);
  printf(", EAN no: ");
  for (i = 7; i >= 0; i--) printf("%02x", eanNo[i]);
  printf(", Serial no: 0x"); 
  for (i = 7; i >= 0; i--) printf("%02x", serNo[i]);
  printf("\n");
  

  printf("Operating as %s on channel %d.\n", optMaster? "master" : "slave", linChannel);
  h = linOpenChannel(linChannel, optMaster ? LIN_MASTER : LIN_SLAVE);
  if (h != linOK) {
    printf("linOpenChannel failed (%d)\n", h);
    if (h == linERR_NOTFOUND) {
      printf("Did you remember to connect external power to the LIN transceiver?\n");
    }
    exit(1);
  }  

  lres = linSetBitrate(h, 10000);
  if (lres != linOK) {
    printf("Error %d setting LIN bitrate\n", lres);
    exit(1);
  }

  lres = linBusOn(h);
  
  if (lres != linOK) {
    printf("Error %d going on bus\n", lres);
    exit(1);
  }

  if (optMaster)
    lres = linWriteMessage(h, 23, "MASTER!", 7);
  else
    lres = linUpdateMessage(h, 23, "SLAVE!", 6);
  if (lres != linOK) {
    printf("Error %d writing LIN message\n", lres);
    exit(1);
  }

  timeOffset = linReadTimer(h);
  tLastPoll = timeOffset;
  for (;;) {
    unsigned int id;
    unsigned char msg[8];
    unsigned int len;
    unsigned int flags;
    LinMessageInfo msgInfo;
    if (linReadMessageWait(h, &id, msg, &len, &flags, &msgInfo, 1) == linOK) {
      msgInfo.timestamp -= timeOffset;
      printLinMessage(id, msg, len, flags, &msgInfo);
    }

    if (optMaster) {
      tNow = linReadTimer(h);
      if (tNow-tLastPoll > 1000) {
        int stat;
        if ((stat = linRequestMessage(h, 0x15)) != linOK) {
          printf("linRequestMessage() failed, status=%d\n", stat);
          exit(1);
        }          
        tLastPoll += 1000;
      }
    } else {
      tNow = linReadTimer(h);
      if (tNow-tLastPoll > 5000) {
        int stat;
        if ((stat = linWriteWakeup(h, 0, 0)) != linOK) {
          printf("linWriteWakeup() failed, status = %d\n", stat);
          exit(1);
        }
        tLastPoll += 5000;
      }
    }
  }

  lres = linBusOff(h);
  if (lres != linOK) {
    printf("Error %d going off bus\n", lres);
    exit(1);
  }

  lres = linClose(h);
  if (lres != linOK) {
    printf("Error %d calling linClose()\n", lres);
    exit(1);
  }
  return 0;
}

/* Print a LIN message to stdout.
*/
void printLinMessage(unsigned int id, unsigned char *msg, unsigned int len, unsigned int flags,
                     LinMessageInfo *msgInfo)
{
  unsigned int i;
  
  printf("%5d.%03d ", msgInfo->timestamp/1000, msgInfo->timestamp%1000);
  if (flags & LIN_TX)
    printf("Tx");
  else if (flags & LIN_RX)
    printf("Rx");
  else
    printf("??");
  printf(" %2d ", id);
  if (flags & LIN_WAKEUP_FRAME)
    printf("WakeUp");
  else if (flags & LIN_NODATA)
    printf("-");
  else {
    printf("{");
    for (i = 0; i < len; i++) {
      if (i)
        printf(" ");
      printf("%02x", msg[i]);
    }
    printf("}");
  }
  if (flags & (LIN_CSUM_ERROR|LIN_PARITY_ERROR|LIN_BIT_ERROR|LIN_SYNCH_ERROR)) {
    printf("[");
    if (flags & LIN_CSUM_ERROR)
      printf("C");
    if (flags & LIN_PARITY_ERROR)
      printf("P");
    if (flags & LIN_BIT_ERROR)
      printf("B");
    if (flags & LIN_SYNCH_ERROR)
      printf("X");
    printf("]");
  }
  printf(" pa: 0x%02x", msgInfo->idPar);
  printf(" cs: 0x%02x", msgInfo->checkSum);
  printf(" sb: %ld", msgInfo->synchBreakLength);
  printf(" fl: %ld", msgInfo->frameLength);
  printf(" br: %ld", msgInfo->bitrate);

  printf("\n");
}

