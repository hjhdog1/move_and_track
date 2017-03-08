/*
**                   Copyright 2007 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**   Sample program for the Softing emulation layer for CANLIB.
**   The program dumps all incoming CAN messages on channel 1 to the
**   screen, and sends a message with identifier 123 when you press the
**   space key.
**
** To build this program, link with sing32.lib and make sure that
** sing32.dll is somewhere in the path.
**
** When running the program, you should connect channel 1 and 2 on your
** hardware with a short, terminated CAN bus segment. It's sufficient
** to connect pin 2 (CAN_L) and pin 7 (CAN_H) in the DSUBs. The
** termination should be 120 Ohms across CANL and CAN_H in each end,
** but for a small test bus you can do with almost anything.
**
*/
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <sing.h>
#include <conio.h>

HANDLE h;

void Setup(void)
{
    int stat;

    stat = CANPC_reset_chip();
    if (stat != 0) {
        printf("CANPC_reset_chip stat=%d\n", stat);
        exit(1);
    }

    // Channel 1: 500 kBit/s, SJW=1, 1 sample.
    stat = CANPC_initialize_chip(2, 1, 4, 3, 0);
    if (stat != 0) {
        printf("CANPC_initialize_chip stat=%d\n", stat);
        exit(1);
    }

    // Channel 2: 500 kBit/s, SJW=1, 1 sample.
    stat = CANPC_initialize_chip2(2, 1, 4, 3, 0);
    if (stat != 0) {
      printf("CANPC_initialize_chip2 stat=%d\n", stat);
      exit(1);
    }

    // Accept all messages on channel 1
    stat = CANPC_set_acceptance(0, 0, 0L, 0L);
    if (stat != 0) {
        printf("CANPC_set_acceptance stat=%d\n", stat);
        exit(1);
    }

    // Accept all messages on channel 2
    stat = CANPC_set_acceptance2(0, 0, 0L, 0L);
    if (stat != 0) {
      printf("CANPC_set_acceptance2 stat=%d\n", stat);
      exit(1);
    }

    stat = CANPC_start_chip();
    if (stat != 0) {
        printf("CANPC_start_chip stat=%d\n", stat);
        exit(1);
    }
    
    CANPC_GetEventHandle(&h);
}


void main(void)
{
    int stat;
    param_struct p;
    unsigned long serial;
    
    stat = INIPC_initialize_board(0);
    if (stat != 0) {
        printf("INIPC_initialize_board stat=%d\n", stat);
        exit(1);
    }

    stat = CANPC_reset_board();
    if (stat != 0) {
        printf("CANPC_reset_board stat=%d\n", stat);
        exit(1);
    }

    stat = CANPC_get_serial_number(&serial);
    if (stat != 0) {
      printf("CANPC_get_serial_number stat=%d\n", stat);
      exit(1);
    }
    printf("Card S/N = %d\n", serial);
    
    Setup();
    
    while (TRUE) {
        int evt, i;

        do {
            evt = CANPC_read_ac(&p);
            switch (evt) {
                case -1:
                    printf("CANPC_read_ac() returned -1.\n");
                    exit(1);
                    
                case 0:
                    // No new event
                    break;
                    
                case 1:
                    printf("T=%08lu RX STD Id=%-8d Dlc=%d  ",
                           p.Time, p.Ident, p.DataLength);
                    for (i=0; i<p.DataLength; i++) {
                        printf("%02x ", p.RCV_data[i]);
                    }
                    putchar('\n');
                    break;

                case 2:
                    // STD RTR, left as an exercise
                    break;

                case 3:
                    printf("T=%08lu TX ACK Id=%-8d Dlc=%d  ",
                           p.Time, p.Ident, p.DataLength);
                    for (i=0; i<p.DataLength; i++) {
                        printf("%02x ", p.RCV_data[i]);
                    }
                    putchar('\n');
                    break;
                    
                case 4: // Not in FIFO mode
                case 5: // Bus status change
                case 6: // Not implemented
                case 7: // "additional error causes"
                case 8: // STD RTR TXACK
                    break;
                    
                case 9: // EXT frame
                    printf("T=%08lu RX XTD Id=%-8d Dlc=%d  ",
                           p.Time, p.Ident, p.DataLength);
                    for (i=0; i<p.DataLength; i++) {
                        printf("%02x ", p.RCV_data[i]);
                    }
                    putchar('\n');
                    break;

                case 10: // EXT TXACK
                case 11: // EXT RTR TXACK
                case 12: // EXT RTR
                    break;

                case 15: // Error frame
                  printf("T=%08lu ERROR  Id=%-8d Dlc=%d  ",
                         p.Time, p.Ident, p.DataLength);
                  for (i=0; i<p.DataLength; i++) {
                    printf("%02x ", p.RCV_data[i]);
                  }
                  putchar('\n');
                  break;
                  
                default:
                    printf("Unknown Event=%d\n", evt);
                    break;
            }
        } while (evt != 0);
        
        (void)WaitForSingleObject(h, 50);

        /*if (stat == WAIT_TIMEOUT) {
            DWORD t;
            CANPC_get_time(&t);
            printf("T=%08lu\n", t);
            lastTim = t;
        }*/
        
        if (_kbhit()) {
            switch (_getch()) {
                DWORD t;
                int foo;
                
                case 27:
                    exit(0);
                    break;
                    
                case 'T':
                case 't':
                    CANPC_get_time(&t);
                    printf("T=%08lu\n", t);
                    break;
                    
                case ' ':
                    CANPC_send_data(123, FALSE, 0, (unsigned char*)&foo);
                    break;

                case 'r':
                    CANPC_reinitialize();
                    Setup();
                    break;
            }
        }
    }
}
