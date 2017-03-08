/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** "Echo Server" for CANLIB. If you send a message to this little program,
** it will respond with the same message where the identifier is increased by one.
*/
/* First, include the whole known universe. */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

// CANLIB requires the following #include.
#include <canlib.h>

#ifdef __BORLANDC__
#pragma argsused
#endif
void Usage(int argc, char* argv[])
{
    printf("\nCANLIB Echo Server\n");
    printf("(Part of the CANLIB SDK from KVASER AB - http://www.kvaser.se)\n");
    printf("\n");
    printf("This is a sample program which acts as an \"Echo Server\".\n");
    printf("If you send a message to this little program, it will respond with\n");
    printf("the same message where the identifier is increased by one.\n");
    printf("\n");
    printf("It requires a CAN interface supported by CANLIB, and runs\n");
    printf("under most versions of Windows.\n");
    printf("\nUsage: tx [flags]\n");
    printf("   -X          Use channel number X as source. (Default 0.)\n");
    printf("   -B<value>   Set the bitrate. Value is any of 1000,500,250,125.\n");
    printf("               Default bitrate is 125 kbit/s.\n");
    printf("   -Sbbb,t1,t2 Set the bitrate using specified values for tseg1 and tseg2.\n");
    printf("               bbb=bitrate in bps.\n");
    printf("               t1=# of quanta before the sampling point, not including\n");
    printf("                  the sync segment.\n");
    printf("               t2=# of quanta after the sampling point.\n");
    printf("   -q          Be quiet.\n");

    exit(1);
}

//
// Global variables for the command-line options.
// 
int Tseg1, Tseg2;
int Bitrate     = canBITRATE_125K;
int Channel     = 0;
int Quiet       = 0;

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
    hnd = canOpenChannel(ctrl, canOPEN_EXCLUSIVE );
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

//
// Calculate the next valid identifier; valid as according to the CAN spec.
// Technically you will probably get along with sending the "illegal"
// identifiers too.
//
void IncreaseId(long *id, int flags)
{
    unsigned long i = *(unsigned long*)id;
    
    i++;
    
    if (flags & canMSG_STD) {
        if (i > 2031) i = 0;
    } else if (flags & canMSG_EXT) {
        if ((i & 0x03F0) == 0x03F0) {
            i = (i & 0xFFFFFC00) + 0x800 + (i & 0x0F);
        }
        i &= 0x1FFFFFFF;
    }
    *id = i;
}

void EchoLoop(void)
{
    int             handle;
    long            id;
    canStatus       stat;
    BYTE            msg[8];
    int             Ready;
    unsigned int    i, dlc, flags;
    unsigned long   time;

    handle = InitCtrl(Channel);

    Ready = FALSE;
    do {
        do {
            //
            // Read a message; timeout after 250 ms.
            //
            stat = canReadWait(handle, &id, msg, &dlc, &flags, &time, 250);
            if (stat != canOK) break;

            //
            // Calculate a new id.
            //
            IncreaseId(&id, flags);

            //
            // Send the reply.
            //
            stat = canWrite(handle, id, msg, dlc,
                            flags & (canMSG_STD|canMSG_EXT|canMSG_RTR));
            Check("canWrite", stat);

            //
            // Print the message we just sent.
            //
            if (!Quiet) {
                printf("%8lu%c%c%c  %lu   ",
                       id,
                       flags & canMSG_EXT        ? 'x' : ' ',
                       flags & canMSG_RTR        ? 'R' : ' ',
                       flags & canMSGERR_OVERRUN ? 'o' : ' ',
                       dlc);

                // Print the data bytes, but not for Remote Frames.
                if ((flags & canMSG_RTR) == 0) {
                    for (i=0; i<dlc; i++) printf("%3u ", msg[i]);
                    for (; i<8; i++) printf("    ");
                }

                // Print the time, in raw format.
                printf("%08lu\n", time);
            }
        } while (stat == canOK);

        //
        // Check the keyboard.
        //
        if (_kbhit()) {
            int c = _getch();
            switch (c) {
                case 27:
                    Ready = TRUE;
                default:
                    ;
            }
        }
    } while (!Ready);

    (void)canBusOff(handle);
    (void)canClose(handle);
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
        } else if (strcmp(argv[i], "-q") == 0) {
            Quiet++;
        } else {
            Usage(argc, argv);
        }
    }

    if (!Quiet) printf("Starting...\n");
    //
    // Initialize CANLIB.
    //
    canInitializeLibrary();

    EchoLoop();

    printf("\nThat's all for today!\n");
}

