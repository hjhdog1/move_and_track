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
** 
*/
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <windows.h>
#include <conio.h>

#include <canlib.h>

#ifdef __BORLANDC__
#pragma argsused
#endif
void Usage(int argc, char* argv[])
{
    printf("\nCANLIB Thread Test 4\n");
    printf("\n");
    printf("\nUsage: thread [flags]\n");
    printf("   -aX         Use channel number X as source\n");
    printf("   -bY         Use channel number Y as destination\n");
    printf("   (note: X must be different from Y. Default X=0, Y=1)\n");
    printf("   -B<value>   Set the bitrate. Value is any of 1000,500,250,125.\n");
    printf("               Default bitrate is 125 kbit/s.\n");
    printf("   -v          Be verbose.\n");
    printf("   -Lnnn       Run nnn \"loops\". Default is 500.\n");

    exit(1);
}

//
// Global variables for the command-line options.
// 
int Bitrate = canBITRATE_125K;   //
int Verbose = 0;
int MaxLoops = 500;

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

    hnd = canOpenChannel(ctrl, 0);
    if (hnd < 0) {
        printf("canOpenChannel failed, stat=%d\n", hnd);
        exit(0);
    }

    //
    // Using our new shiny handle, we specify the baud rate
    // using one of the convenient canBITRATE_xxx constants.
    //
    // The bit layout is in depth discussed in most CAN
    // controller data sheets, and on the web at
    // http://www.kvaser.se.
    //
    stat = canSetBusParams(hnd, Bitrate, 0, 0, 0, 0, 0);
    if (stat < 0) {
        printf("canSetBusParams failed, stat=%d\n", stat);
    }

    //
    // Then we start the ball rolling.
    // 
    stat = canBusOn(hnd);
    if (stat < 0) {
        printf("canBusOn failed, stat=%d\n", stat);
    }

    // Return the handle; our caller will need it for
    // further exercising the CAN controller.
    return hnd;
}

CRITICAL_SECTION cs;

static long TimeToLeave;
static long TotalSent[4];
static long TotalReceived[4];

#define NR_MSGS 3
#define SRC_ID  300
#define P_NAP   50
#define C_NAP   0

HANDLE events[4];
int channels[4];


DWORD WINAPI SendAndReceive(PVOID p)
{
    int         handle;
    long        id, id_read;
    canStatus   stat;
    BYTE        msg[8];
    int         i, idx, count;

    idx = (int) p;
    
    handle = InitCtrl(channels[idx]);
    TotalSent[idx] = 0;
    count = 0;
    WaitForSingleObject(events[idx], INFINITE);
    
    while (TRUE) {

        id = SRC_ID + idx;
        
        for (i=0; i<NR_MSGS; i++) {
            int dlc, flags;
            // Send one..
            //memcpy(msg, &i, sizeof(i));
            memcpy(msg, &count, sizeof(count));
            count++;
            stat = canWrite(handle, id, msg, 8, 0);
            Check("canWrite A", stat);
            TotalSent[idx]++;

            // And receive all that is pending..
            do {
                stat = canRead(handle, &id_read, NULL, &dlc, &flags, NULL);
                if (stat == canOK) {
                    if (Verbose > 1) printf("R");
                    TotalReceived[idx]++;
                    // if (id_read != SRC_ID) printf("Id=%d dlc=%d flags=0x%x\n", id_read, dlc, flags);
                    if (flags & canMSG_ERROR_FRAME) printf("x");
                }
            } while (stat == canOK);
        }
        stat = canWriteSync(handle, 1000);
        Check("canWriteSync A", stat);
        Sleep(P_NAP);
        
        if (TimeToLeave) break;
    }
    return 0;
}

DWORD WINAPI ReadOne(PVOID p)
{
    int         handle;
    canStatus   stat;
    int         idx;

    idx = (int) p;
    handle = InitCtrl(channels[idx]);
    TotalReceived[idx] = 0;
    WaitForSingleObject(events[idx], INFINITE);
    
    while (TRUE) {

        //
        // Call canReadSyncSpecific to wait for the message(s) that the other threads
        // are sending back and forth, then call canRead to consume the messages.
        //
        do {
            stat = canReadSyncSpecific(handle, SRC_ID, 500);
            if (stat == canOK) {
                if (Verbose > 1) printf("r");
            } else {
                if (Verbose > 1) printf("T");
            }
            while (canRead(handle, NULL, NULL, NULL, NULL, NULL) == canOK) {
                TotalReceived[idx]++;
            }
        } while (stat == canOK);

        //
        // Call canReadSyncSpecific again to wait for a non-existent id - this time
        // we expect a timeout.
        //
        do {
            stat = canReadSyncSpecific(handle, SRC_ID+5, 50);
            if (stat == canOK) {
                // Whoops. qqq
                printf("Q");
            }
        } while (stat == canOK);

        if (TimeToLeave) break;
        Sleep(C_NAP);
    }

    return 0;
}

//
// Just consume all messages on the bus.
//
DWORD WINAPI Sink(PVOID p)
{
    int         handle, idx;

    idx = (int) p;
    handle = InitCtrl(channels[idx]);
    TotalReceived[idx] = 0;
    WaitForSingleObject(events[idx], INFINITE);

    while (TRUE) {

        while (canRead(handle, NULL, NULL, NULL, NULL, NULL) == canOK) {
            if (Verbose > 1) printf("x");
            TotalReceived[idx]++;
        }

        if (TimeToLeave) break;
        Sleep(C_NAP);
    }

    return 0;
}



//
////////////////////////////////////////////////////////////////////////////
//
void main(int argc, char* argv[])
{
    int i;
    DWORD tid;
    HANDLE t1, t2, t3, t4;


    for (i=0; i<4; i++) channels[i] = i;
    
    //
    // First, parse the command-line arguments.
    //
    for (i=1; i<argc; i++) {
        int tmp, c;
        if (sscanf(argv[i], "-a%d%c", &tmp, &c) == 1) {
            channels[0] = tmp;
        } else if (sscanf(argv[i], "-b%d%c", &tmp, &c) == 1) {
            channels[1] = tmp;
        } else if (sscanf(argv[i], "-c%d%c", &tmp, &c) == 1) {
            channels[2] = tmp;
        } else if (sscanf(argv[i], "-d%d%c", &tmp, &c) == 1) {
            channels[3] = tmp;
        } else if (sscanf(argv[i], "-B%d%c", &tmp, &c) == 1) {
            switch (tmp) {
                case 1000 : Bitrate = canBITRATE_1M; break;
                case 500  : Bitrate = canBITRATE_500K; break;
                case 250  : Bitrate = canBITRATE_250K; break;
                case 125  : Bitrate = canBITRATE_125K; break;
                default : Usage(argc, argv);
            }
        } else if (strcmp(argv[i], "-v") == 0) {
            Verbose++;
        } else if (sscanf(argv[i], "-L%d%c", &tmp, &c) == 1) {
            MaxLoops = tmp;
        } else {
            Usage(argc, argv);
        }
    }

    printf("\nCANLIB Thread Test 4\n");
    memset(TotalSent, 0, sizeof(TotalSent));
    memset(TotalReceived, 0, sizeof(TotalReceived));
    
    canInitializeLibrary();

    InitializeCriticalSection(&cs);
    TimeToLeave = FALSE;
    
    t1 = t2 = t3 = t4 = NULL;
    
    if (channels[0] != 99) t1 = CreateThread(NULL, 0, SendAndReceive, (LPVOID)0, CREATE_SUSPENDED, &tid);
    if (channels[1] != 99) t2 = CreateThread(NULL, 0, SendAndReceive, (LPVOID)1, CREATE_SUSPENDED, &tid);
    if (channels[2] != 99) t3 = CreateThread(NULL, 0, ReadOne, (LPVOID)2, CREATE_SUSPENDED, &tid);
    if (channels[3] != 99) t4 = CreateThread(NULL, 0, Sink, (LPVOID)3, CREATE_SUSPENDED, &tid);

    printf("\n");
    if (t1) printf("SendAndReceive on channel %d\n", channels[0]);
    if (t2) printf("SendAndReceive on channel %d\n", channels[1]);
    if (t3) printf("ReadOne on channel %d\n", channels[2]);
    if (t4) printf("Sink on channel %d\n", channels[3]);
    printf("\n");

    for (i=0; i<4; i++) events[i] = CreateEvent(0, FALSE, FALSE, 0);
    
    ResumeThread(t1);
    ResumeThread(t2);
    ResumeThread(t3);
    ResumeThread(t4);

    Sleep(100);
    for (i=0; i<4; i++) SetEvent(events[i]);
    
    while (TRUE) {
        Sleep(200);
        if (_kbhit()) {
            _getch();
            break;
        }
        if (--MaxLoops == 0) break;
    }

    TimeToLeave = TRUE;
    WaitForSingleObject(t1, 2000);
    WaitForSingleObject(t2, 2000);
    WaitForSingleObject(t3, 2000);
    WaitForSingleObject(t4, 2000);

    printf("\nChannel      Sent     Received\n");
    for (i=0; i<4; i++) {
        printf("%7d%10d%13d\n", i, TotalSent[i], TotalReceived[i]);
    }

}

