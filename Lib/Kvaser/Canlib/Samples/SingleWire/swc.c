/*
**                         Copyright 2000 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
**
** Description:
**
**
*/
/*
** Test program for SWC piggybacks on PCIcan.
** Not yet tested with LAPcan + DRVcan S
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <windows.h>

#include <canlib.h>

#ifdef __BORLANDC__
#pragma argsused
#endif
void Usage(int argc, char* argv[])
{
    printf("\nSWC Test Program\n");
    printf("\n");
    printf("\nUsage: swc [flags]\n");
    printf("   -aX         Use channel number X as source\n");
    printf("   -bY         Use channel number Y as destination\n");
    printf("   (note: X must be different from Y. Default X=0, Y=1)\n");

    exit(1);
}

int doWakeup;
int first, second;

void Check(char* id, canStatus stat)
{
    char buf[50];
    if (stat != canOK) {
        buf[0] = '\0';
        canGetErrorText(stat, buf, sizeof(buf));
        printf("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
    }
}

//
// Setup a CAN controller.
//
int InitCtrl(int ctrl, BYTE btr0, BYTE btr1, int mode)
{
    int stat;
    int hnd;

    hnd = canOpenChannel(ctrl, canOPEN_EXCLUSIVE);
    if (hnd < 0) {
        Check("canOpenChannel", (canStatus)hnd);
        exit(1);
    }
    stat = canSetBusParamsC200(hnd, btr0, btr1);
    if (stat < 0) {
        printf("canSetBusParamsC200 failed, stat=%d\n", stat);
    }

    stat = canBusOn(hnd);
    if (stat < 0) {
        printf("canBusOn failed, stat=%d\n", stat);
    }

    stat = canSetDriverMode(hnd, mode, 0);
    if (stat < 0) {
        printf("canSetDriverMode failed, stat=%d\n", stat);
    }
    
    return hnd;
}

//
// PumpMessages is a routine that sends a number of messages
// from one handle to another.
// 
void PumpMessages(int srcHandle, int dstHandle, int nrMsgs)
{
    int i, j, k, m;
    canStatus stat;
    long id;
    BYTE msg[8];
    const long srcID = 300L;

    // First, queue nrMsgs messages to the first handle.
    for (i=0; i<nrMsgs; i++) {
        stat = canWrite(srcHandle, srcID, &i, 8, doWakeup?canMSG_WAKEUP:0);
        Check("canWrite", stat);
    }

    // Then, wait for the transmission to complete.
    // An error here usually means we are alone on the bus,
    // or that the bus is not terminated properly.
    stat = canWriteSync(srcHandle, 5000);
    Check("canWriteSync", stat);

    // Now, let's verify ge got them all.
    i=j=k=m=0;
    while (canRead(dstHandle, &id, msg, NULL, NULL, NULL) == canOK) {
        if (msg[0] != i) j++;
        if (msg[0] == i-1) k++;
        if (id != srcID) m++;
        i++;
    } 
    // The status code will be canERR_NOMSG (-2) (and the loop will
    // terminate) when there are no more messages to read.

    // Issue an error message if it's necessary. Let's hope not.
    if ((i != nrMsgs) || j || k || m) {
        printf("Read %d messages, expected %d; %d corrupt, %d dropped, %d unexpected id.\n",
               i, nrMsgs, j, k, m);
    }
}

//
// FillCircuit queues nrMsgs messages to a handle, but
// increases the identifier by one for each message.
//
// The contents of each message will 0..nrMsgs-1 in the
// two (four for Win32) first bytes.
//
void FillCircuit(int srcHandle, int nrMsgs, int startId)
{
    int i;
    canStatus stat;
    long srcID = startId;

    for (i=0; i<nrMsgs; i++) {
        stat = canWrite(srcHandle, srcID++, &i, sizeof(int), doWakeup?canMSG_WAKEUP:0);
        Check("canWrite", stat);
    }
}

//
// Setup two handles and send several messages back and forth.
//
void PerformTest(BYTE btr0, BYTE btr1, int mode)
{
    int         handle1;
    int         handle2;

    handle1 = InitCtrl(first, btr0, btr1, mode);
    handle2 = InitCtrl(second, btr0, btr1, mode);

    //
    // First, send a couple of messages from one controller to another.
    //
    printf("Sending 100 messages from A to B.\n");
    PumpMessages(handle1, handle2, 100);

    printf("Sending 100 messages in the opposite direction.\n");
    PumpMessages(handle2, handle1, 100);

    printf("Sending 150 messages from A to B.\n");
    PumpMessages(handle1, handle2, 150);

    printf("Sending 150 messages in the opposite direction.\n");
    PumpMessages(handle2, handle1, 150);

    canClose(handle1);
    canClose(handle2);
}



//
////////////////////////////////////////////////////////////////////////////
//
void main(int argc, char* argv[])
{
    int i;

    printf("\nWelcome to the SWC Piggyback Test!\n");
    //
    // First, parse the command-line arguments.
    //
    first = 0;
    second = 1;
    for (i=1; i<argc; i++) {
        int tmp, c;
        if (sscanf(argv[i], "-a%d%c", &tmp, &c) == 1) {
                first = tmp;
        } else if (sscanf(argv[i], "-b%d%c", &tmp, &c) == 1) {
            second = tmp;
        } else {
            Usage(argc, argv);
        }
    }

    doWakeup = 0;
    canInitializeLibrary();

    printf("15.686 kbps...\n");
    PerformTest(0x5D, 0x4a, canTRANSCEIVER_LINEMODE_SWC_NORMAL);

    printf("20 kbps...\n");
    PerformTest(0x58, 0x3a, canTRANSCEIVER_LINEMODE_SWC_NORMAL);

    printf("33.333 kbps...\n");
    PerformTest(0x53, 0x36, canTRANSCEIVER_LINEMODE_SWC_NORMAL);

    printf("50 kbps...\n");
    PerformTest(0x4f, 0x16, canTRANSCEIVER_LINEMODE_SWC_NORMAL);

    printf("Going to fast mode. Insert load resistor (180 Ohm CAN_H to GND):");
    (void)getchar();

    printf("15.686 kbps...\n");
    PerformTest(0x5D, 0x4a, canTRANSCEIVER_LINEMODE_SWC_FAST);

    printf("20 kbps...\n");
    PerformTest(0x58, 0x3a, canTRANSCEIVER_LINEMODE_SWC_FAST);
    
    printf("33.333 kbps...\n");
    PerformTest(0x53, 0x36, canTRANSCEIVER_LINEMODE_SWC_FAST);

    printf("50 kbps...\n");
    PerformTest(0x4f, 0x16, canTRANSCEIVER_LINEMODE_SWC_FAST);

    printf("80 kbps...\n");
    PerformTest(0x49, 0x16, canTRANSCEIVER_LINEMODE_SWC_FAST);

    printf("100 kbps...\n");
    PerformTest(0x47, 0x16, canTRANSCEIVER_LINEMODE_SWC_FAST);
    
    printf("And now a couple of WAKEUPs:");
    (void)getchar();
    doWakeup = 1;

    printf("15.686 kbps...\n");
    PerformTest(0x5D, 0x4a, canTRANSCEIVER_LINEMODE_SWC_NORMAL);

    printf("20 kbps...\n");
    PerformTest(0x58, 0x3a, canTRANSCEIVER_LINEMODE_SWC_NORMAL);

    printf("33.333 kbps...\n");
    PerformTest(0x53, 0x36, canTRANSCEIVER_LINEMODE_SWC_NORMAL);

    printf("50 kbps...\n");
    PerformTest(0x4f, 0x16, canTRANSCEIVER_LINEMODE_SWC_NORMAL);
}

