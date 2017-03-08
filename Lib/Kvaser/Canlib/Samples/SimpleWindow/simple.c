/*
**                        Copyright 1997 by KVASER AB
**            P.O Box 4076 S-51104 KINNAHULT, SWEDEN Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This is a simple program that demonstrates how to use CANLIB at
** a very basic level.
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <windows.h>

// Include files for CANLIB
#include <canlib.h>

// Include our own prototypes.
#include "simple.h"
#include "main.h"

//
// Global variables for the command-line options.
// 
// int first = 0;             // First channel #
// int second = 1;            // Second channel #

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
        Print("%s: failed, stat=%d (%s)", id, (int)stat, buf);
    }
}

//
// Setup a CAN controller.
//
int InitCtrl(int ctrl, int bitrate)
{
    int stat;
    int hnd;

    //
    // First, open a handle to the CAN circuit. Specifying
    // canOPEN_EXCLUSIVE ensures we get a circuit that noone else
    // is using.
    //
    Print("canOpenChannel, channel %d... ", ctrl);
    hnd = canOpenChannel(ctrl, canOPEN_EXCLUSIVE);
    if (hnd < 0) {
        Check("canOpenChannel", (canStatus)hnd);
        exit(1);
    }
    Print("OK.\n");

    //
    // Using our new shiny handle, we specify the baud rate
    // using one of the convenient canBITRATE_xxx constants.
    //
    // The bit layout is in depth discussed in most CAN
    // controller data sheets, and on the web at
    // http://www.kvaser.se.
    //
    Print("Setting the bus speed to %d...", bitrate);
    stat = canSetBusParams(hnd, bitrate, 0, 0, 0, 0, 0);
    if (stat < 0) {
        Print("canSetBusParams failed, stat=%d", stat);
    }
    Print("OK.");

    //
    // Then we start the ball rolling.
    // 
    Print("Go bus-on...");
    stat = canBusOn(hnd);
    if (stat < 0) {
        Print("canBusOn failed, stat=%d", stat);
    }
    Print("OK.");

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
        Print("Read %d messages, expected %d; %d corrupt, %d dropped, %d unexpected id.",
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
// Send several messages back and forth between two CAN circuits
// (which are assumed to be opened and onbus already.)
//
void PerformTest(int handle1, int handle2)
{
    long        id;
    canStatus   stat;
    BYTE        msg[8];
    int         i;

    //
    // First, send a couple of messages from one controller to another.
    //
    Print("Sending 100 messages from A to B.");
    PumpMessages(handle1, handle2, 100);

    Print("Sending 100 messages in the opposite direction.");
    PumpMessages(handle2, handle1, 100);

    Print("Sending 150 messages from A to B.");
    PumpMessages(handle1, handle2, 150);

    Print("Sending 150 messages in the opposite direction.");
    PumpMessages(handle2, handle1, 150);

    Print("");
    Print("Demonstrating canReadWait()");
    Print("Send 10 messages from the first controller, and read them from");
    Print("the second one; wait until they arrive if necessary.");
    
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

    Print("");
    Print("Demonstrating canReadSync().");
    Print("Send one message from the first controller, pick it up at the");
    Print("second one after calling canReadSync().");

    FillCircuit(handle1, 1, 150);
    stat = canReadSync(handle2, 100);
    Check("canReadSync", stat);
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    Check("canRead", stat);
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    if (stat != canERR_NOMSG) Check("canRead", stat);

    //
    Print("");
    Print("Demonstrating canReadSpecific().");
    Print("Send a couple of messages and pick them up in the opposite order.");

    FillCircuit(handle1, 5, 350);
    stat = canWriteSync(handle1, 200);
    Check("canWriteSync", stat);
    for (i=4; i; i--) {
        stat = canReadSpecific(handle2, 350+i, msg, NULL, NULL, NULL);
        Check("canReadSpecific", stat);
        if (msg[0] != i) Print("Message %d corrupted.", i);
    }
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    if (stat != canERR_NOMSG) Check("canRead", stat);

    Print("");
    Print("Testing canReadSpecificSkip()");
    Print("Send a couple of messages and pick up the last one, skipping the");
    Print("others.");

    
    FillCircuit(handle1, 7, 380);           // Send 7 messages, id 380..386
    stat = canWriteSync(handle1, 200);      // Be patient for 200 ms
    Check("canWriteSync", stat);
    stat = canReadSpecificSkip(handle2, 386, msg,   // Id 386 should have arrived.
                               NULL, NULL, NULL);
    Check("canReadSpecificSkip", stat);
    if (msg[0] != 6) Print("Message corrupted.");
    // And now, there shouldn't be any more messages to read.
    stat = canRead(handle2, &id, NULL, NULL, NULL, NULL);
    if (stat != canERR_NOMSG) Check("canRead", stat);

    //
    // That's all for today!
    //

    Print("Taking both circuits off bus...");
    // These two are not really necessaey but for the completeness...
    (void)canBusOff(handle1);
    (void)canBusOff(handle2);
    
    Print("Closing both circuits...");
    canClose(handle1);
    canClose(handle2);
}
