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

// CANLIB requires canlib.h.
#include <canlib.h>

#ifdef __BORLANDC__
#pragma argsused
#endif
void Usage(int argc, char* argv[])
{
    printf("\nCANLIB Traffic Generator\n\n");
    printf("(Part of the CANLIB SDK from KVASER AB - http://www.kvaser.se)\n");
    printf("\n");
    printf("This demo programs transmits messages from a CAN channel.\n");
    printf("It requires a CAN interface supported by CANLIB, and runs\n");
    printf("under Windows 95 and/or Windows NT.\n");
    printf("\nUsage: tx [flags]\n");
    printf("   -X          Use channel number X as source. (Default 0.)\n");
    printf("   -waitXXX    Sleep for XXX milliseconds between the bursts.\n");
    printf("   -burstXXX   Send XXX messages in each burst.\n");
    printf("   -B<value>   Set the bitrate. Value is any of 1000,500,250,125.\n");
    printf("               Default bitrate is 125 kbit/s.\n");
    printf("   -Sbbb,t1,t2 Set the bitrate using specified values for tseg1 and tseg2.\n");
    printf("               bbb=bitrate in bps.\n");
    printf("               t1=# of quanta before the sampling point, not including\n");
    printf("                  the sync segment.\n");
    printf("               t2=# of quanta after the sampling point.\n");
    printf("   -fail       Exit the program immediately if transmission fails.\n");
    printf("   -Lnnn       Use nnn (0..8) as message length.\n");
    printf("   -loopback   Run a loopback between channel X and X+1.\n");
    printf("   -q          Be quiet.\n");

    exit(1);
}

//
// Global variables for the command-line options.
// 
int Tseg1, Tseg2;
int Bitrate         = canBITRATE_125K;
int Channel         = 0;
int WaitTime        = 100;
int Fail            = FALSE;
int BurstSize       = 5;
int Quiet           = 0;
int MaxCount        = -1;
int CurrentCount    = 0;
int MessageLength   = 8;
int Loopback        = FALSE;

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
    stat = canSetBusParams(hnd, Bitrate, Tseg1, Tseg2, 1, 1, 0);

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


void Send(void)
{
    int         handle1;

    canStatus   stat;
    int         i;
    int         ready;
    const long srcID = 300L;

    // Open a channel, and open another one if we want a loopback.
    handle1 = InitCtrl(Channel);
    if (Loopback) {
        (void)InitCtrl(Channel+1);
    }

    ready = FALSE;
    while(!ready) {
        unsigned long t0;
        long timeToSleep;

        t0 = timeGetTime();
        //
        // Send a burst of messages.
        //
        for (i=0; i<BurstSize; i++) {
            stat = canWrite(handle1, srcID, &CurrentCount, MessageLength, 0);
            if (Quiet <= 1) {
                Check("canWrite", stat);
                if ((stat != canOK) && Fail) {
                    ready = TRUE;
                    break;
                }
            }
            CurrentCount++;
        }

        //
        // Print some statistics, sleep for a while and check the keyboard.
        //
        if (!Quiet) printf("%d..", CurrentCount);
        timeToSleep = WaitTime - (timeGetTime() - t0);
        if (timeToSleep > 0) Sleep(timeToSleep);
        if (_kbhit()) break;
    }

    printf("\nTransmitted %d messages.\n", CurrentCount);

    // These two are not really necessary but for the completeness...
    (void)canBusOff(handle1);
    canClose(handle1);
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
        int tmp1, tmp2;
        if (sscanf(argv[i], "-%d%c", &tmp, &c) == 1) {
                Channel = tmp;
        } else if (sscanf(argv[i], "-B%d%c", &tmp, &c) == 1) {
            switch (tmp) {
                case 1000 : Bitrate = canBITRATE_1M; break;
                case 500  : Bitrate = canBITRATE_500K; break;
                case 250  : Bitrate = canBITRATE_250K; break;
                case 125  : Bitrate = canBITRATE_125K; break;
                default : Usage(argc, argv);
            }
        } else if (sscanf(argv[i], "-S%d,%d,%d%c", &tmp, &tmp1, &tmp2, &c) == 3) {
            Bitrate = tmp;
            Tseg1 = tmp1;
            Tseg2 = tmp2;
        } else if (sscanf(argv[i], "-wait%d%c", &tmp, &c) == 1) {
            WaitTime = tmp;
        } else if (sscanf(argv[i], "-burst%d%c", &tmp, &c) == 1) {
            BurstSize = tmp;
        } else if (sscanf(argv[i], "-L%d%c", &tmp, &c) == 1) {
            MessageLength = tmp;
        } else if (strcmp(argv[i], "-fail") == 0) {
            Fail = TRUE;
        } else if (strcmp(argv[i], "-loopback") == 0) {
            Loopback = TRUE;
        } else if (strcmp(argv[i], "-q") == 0) {
            Quiet++;
        } else {
            Usage(argc, argv);
        }
    }

    canInitializeLibrary();
    
    //
    // Start the test.
    //
    printf("Starting...\n");

    Send();

    printf("\nThat's all for today!\n");

}

