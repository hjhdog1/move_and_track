/*
**                         Copyright 1996-98 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This is a simple program that demonstrates how to use CANLIB at
** a fairly basic level.
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
    printf("\nCANLIB Demo Program\n");
    printf("(Part of the CANLIB SDK from KVASER AB - http://www.kvaser.se)\n");
    printf("\n");
    printf("This program demonstrates some programming techniques useful\n");
    printf("to the programmer using CANLIB.\n");
    printf("It requires a CAN interface supported by CANLIB, and runs\n");
    printf("under most versions of Windows.\n");
    printf("\nUsage: simple [flags]\n");
    printf("   -aX         Use channel number X as source\n");
    printf("   -bY         Use channel number Y as destination\n");
    printf("   (note: X must be different from Y. Default X=0, Y=1)\n");
    printf("   -B<value>   Set the bitrate. Value is any of 1000,500,250,125.\n");
    printf("               Default bitrate is 125 kbit/s.\n");

    exit(1);
}

//
// Global variables for the command-line options.
// 
int bitrate = canBITRATE_125K;   //
int first = 0;             // First channel #
int second = 1;            // Second channel #

//
// Check a status code and issue an error message if the code
// isn't canOK.
//
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
int InitCtrl(int ctrl)
{
    int stat;
    int hnd;

    //
    // First, open a handle to the CAN circuit. Specifying
    // canOPEN_EXCLUSIVE ensures we get a circuit that noone else
    // is using.
    //
    printf("canOpenChannel, channel %d... ", ctrl);
    hnd = canOpenChannel(ctrl, canOPEN_EXCLUSIVE);
    if (hnd < 0) {
        Check("canOpenChannel", (canStatus)hnd);
        exit(1);
    }
    printf("OK.\n");

    //
    // Using our new shiny handle, we specify the baud rate
    // using one of the convenient canBITRATE_xxx constants.
    //
    // The bit layout is in depth discussed in most CAN
    // controller data sheets, and on the web at
    // http://www.kvaser.se.
    //
    printf("Setting the bus speed...");
    stat = canSetBusParams(hnd, bitrate, 0, 0, 0, 0, 0);
    if (stat < 0) {
        printf("canSetBusParams failed, stat=%d\n", stat);
    }
    printf("OK.\n");

    //
    // Then we start the ball rolling.
    // 
    printf("Go bus-on...");
    stat = canBusOn(hnd);
    if (stat < 0) {
        printf("canBusOn failed, stat=%d\n", stat);
    }
    printf("OK.\n");

    // Return the handle; our caller will need it for
    // further exercising the CAN controller.
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
        stat = canWrite(srcHandle, srcID, &i, 8, 0);
        Check("canWrite", stat);
    }

    // Then, wait for the transmission to complete.
    // An error here usually means we are alone on the bus,
    // or that the bus is not terminated properly.
    stat = canWriteSync(srcHandle, 1000);
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
        stat = canWrite(srcHandle, srcID++, &i, sizeof(int), 0);
        Check("canWrite", stat);
    }
}

//
// Setup two handles and send several messages back and forth.
//
void PerformTest(void)
{
    int         handle1;
    int         handle2;

    long        id;
    canStatus   stat;
    BYTE        msg[8];
    int         i;

    handle1 = InitCtrl(first);
    handle2 = InitCtrl(second);

    printf("\n");
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

    printf("\nDemonstrating canReadWait()\n");
    printf("Send 10 messages from the first controller, and read them from\n");
    printf("the second one; wait until they arrive if necessary.\n");
    
    FillCircuit(handle1, 10, 100);
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    // At least one should have arrived by now.
    if (stat != canERR_NOMSG) {
        Check("canRead", stat);
    }

    //
    // Lets wait for the rest, but no longer than 100 ms for each message.
    //
    for (i=0; i<9; i++) {
        stat = canReadWait(handle2, &id, NULL, NULL, NULL, NULL, 100);
        Check("canReadWait", stat);
    }

    printf("\nDemonstrating canReadSync().\n");
    printf("Send one message from the first controller, pick it up at the\n");
    printf("second one after calling canReadSync().\n");

    FillCircuit(handle1, 1, 150);
    stat = canReadSync(handle2, 100);
    Check("canReadSync", stat);
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    Check("canRead", stat);
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    if (stat != canERR_NOMSG) Check("canRead", stat);

    //
    printf("\nDemonstrating canReadSpecific().\n");
    printf("Send a couple of messages and pick them up in the opposite order.\n");

    FillCircuit(handle1, 5, 350);
    stat = canWriteSync(handle1, 200);
    Check("canWriteSync", stat);
    for (i=4; i; i--) {
        stat = canReadSpecific(handle2, 350+i, msg, NULL, NULL, NULL);
        Check("canReadSpecific", stat);
        if (msg[0] != i) printf("Message %d corrupted.\n", i);
    }
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    if (stat != canERR_NOMSG) Check("canRead", stat);

    printf("\nTesting canReadSpecificSkip()\n");
    printf("Send a couple of messages and pick up the last one, skipping the\n");
    printf("others.\n");

    
    FillCircuit(handle1, 7, 380);           // Send 7 messages, id 380..386
    stat = canWriteSync(handle1, 200);      // Be patient for 200 ms
    Check("canWriteSync", stat);
    stat = canReadSpecificSkip(handle2, 386, msg,   // Id 386 should have arrived.
                               NULL, NULL, NULL);
    Check("canReadSpecificSkip", stat);
    if (msg[0] != 6) printf("Message corrupted.\n");
    // And now, there shouldn't be any more messages to read.
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    if (stat != canERR_NOMSG) Check("canRead", stat);

    //
    // That's all for today!
    //

    // These two are not really necessaey but for the completeness...
    (void)canBusOff(handle1);
    (void)canBusOff(handle2);
    
    canClose(handle1);
    canClose(handle2);
}



//
////////////////////////////////////////////////////////////////////////////
//
void main(int argc, char* argv[])
{
    int i;

    //
    // First, parse the command-line arguments.
    //
    for (i=1; i<argc; i++) {
        int tmp, c;
        if (sscanf(argv[i], "-a%d%c", &tmp, &c) == 1) {
          first = tmp;
        } else if (sscanf(argv[i], "-b%d%c", &tmp, &c) == 1) {
          second = tmp;
        } else if (sscanf(argv[i], "-B%d%c", &tmp, &c) == 1) {
            switch (tmp) {
                case 1000 : bitrate = canBITRATE_1M; break;
                case 500  : bitrate = canBITRATE_500K; break;
                case 250  : bitrate = canBITRATE_250K; break;
                case 125  : bitrate = canBITRATE_125K; break;
                default : Usage(argc, argv);
            }
        } else {
            Usage(argc, argv);
        }
    }

    //
    // Start the test.
    //
    printf("Starting...\n");

    //
    // Initialize CANLIB.
    //
    canInitializeLibrary();

    PerformTest();

    printf("\nThat's all for today!\n");

}

