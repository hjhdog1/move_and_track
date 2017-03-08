/*
**                   Copyright 2005 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**   Demo program for CANLIB.
** ---------------------------------------------------------------------------
*/
#include <canlib.h>
#include "candemo.h"
#include "stdio.h"


// module globals
unsigned int   m_usedBaudRate                  = 0;
canHandle      m_usedChannel;
unsigned int   m_usedId;
unsigned int   m_usedFlags                     = 0;
unsigned int   m_Verbose                       = 1;

driverData     m_channelData;
driverData    *m_DriverConfig                 = &m_channelData;



////////////////////////////////////////////////////////////////////////////////////
//Function that prints the "help- menu"
////////////////////////////////////////////////////////////////////////////////////
void help(void)
{
  printf("\n********************************************\n*\n"); 
  printf("*  Keyboard commands:\n");
  printf("*   t       Transmit a Message\n");
  printf("*   b       Transmit a Message Burst\n");
  printf("*   r       Select Remote Message\n");
  printf("*   S       Request chip status\n");
  printf("*   s       Read chip status\n");
  printf("*   o       On/Off Bus\n");
  printf("*   c/C/1-9 Select Channel\n");
  printf("*   i/I     Select Transmit Id (Up/Down)\n");
  printf("*   x       Select Extended Id\n");
  printf("*   m       Toggle Output Mode\n");
  printf("*   v       Toggle Logging to Screen\n");
  printf("*   T       Toggle transmission acknowledges for the current channel\n");
  printf("*   k       Read the clock (call canReadTimer)\n");
  printf("*   h       Help\n");
  printf("*   d       Print Driver Configuration\n");
  printf("*   e       Clear Screen\n");
  printf("*   ESC     Exit\n*\n*");
  printf("\n********************************************\n");
}


//////////////////////////////////////////////////////////////////////////////////////
//Function that prints channel data
//////////////////////////////////////////////////////////////////////////////////////
void printDriverConfig( void )
{
  unsigned int i;

  printf("\nDriver Configuration:\n  ChannelCount=%u\n", m_DriverConfig->channelCount);
  for (i = 0; i < m_DriverConfig->channelCount; i++) {

    printf("  %s : Channel %u, isOnBus=%u, Baudrate=", 
           m_DriverConfig->channel[i].name,
           m_DriverConfig->channel[i].channel, 
           m_DriverConfig->channel[i].isOnBus, 
           m_usedBaudRate);

    switch(m_usedBaudRate) {
      case canBITRATE_1M:
        printf("canBITRATE_1M");
        break;
      case canBITRATE_500K:
        printf("canBITRATE_500K");
        break;
      case canBITRATE_250K:
        printf("canBITRATE_250K");
        break;
      case canBITRATE_125K:
        printf("canBITRATE_125K");
        break;
      case canBITRATE_100K:
        printf("canBITRATE_100K");
        break;
      case canBITRATE_62K:
        printf("canBITRATE_62K");
        break;
      case canBITRATE_50K:
        printf("canBITRATE_50K");
        break;
      default:
        printf("UNKNOWN");
    }
    printf("\n    ");

    if (m_DriverConfig->channel[i].driverMode == canDRIVER_NORMAL) {
      printf("Drivermode=canDRIVER_NORMAL\n");
    } else {
      printf ("Drivermode=canDRIVER_SILENT\n");
    }
  }
  
  printf("\n\n");
}



////////////////////////////////////////////////////////////////////////////////////
// Open one handle to each channel present in the system.
// Set bus parameters 
////////////////////////////////////////////////////////////////////////////////////
void InitDriver(void)
{
  int  i;
  int  stat;

  // Initialize ChannelData.
  memset(m_channelData.channel, 0, sizeof(m_channelData.channel));
  for (i = 0; i < MAX_CHANNELS; i++) {  
    m_channelData.channel[i].isOnBus      = 0;
    m_channelData.channel[i].driverMode   = canDRIVER_NORMAL;
    m_channelData.channel[i].channel      = -1;
    m_channelData.channel[i].hnd          = -1;
    m_channelData.channel[i].txAck        = 0; // Default is TxAck off
  }
  m_channelData.channelCount = 0;


  //
  // Enumerate all installed channels in the system and obtain their names
  // and hardware types.
  //

  //initialize CANlib
  canInitializeLibrary();

  //get number of present channels
  stat = canGetNumberOfChannels((int*)&m_channelData.channelCount);

  for (i = 0; (unsigned int)i < m_channelData.channelCount; i++) {
    canHandle  hnd;

    //obtain some hardware info from CANlib
    m_channelData.channel[i].channel = i;
    canGetChannelData(i, canCHANNELDATA_CHANNEL_NAME,
                      m_channelData.channel[i].name,
                      sizeof(m_channelData.channel[i].name));
    canGetChannelData(i, canCHANNELDATA_CARD_TYPE,
                      &m_channelData.channel[i].hwType,
                      sizeof(DWORD));

    //open CAN channel
    hnd = canOpenChannel(i, canOPEN_ACCEPT_VIRTUAL);
    if (hnd < 0) {
      // error
      PRINTF_ERR(("ERROR canOpenChannel() in initDriver() FAILED Err= %d. <line: %d>\n",
                  hnd, __LINE__));
    } 
    else {
      m_channelData.channel[i].hnd = hnd;
      if ((stat = canIoCtl(hnd, canIOCTL_FLUSH_TX_BUFFER, NULL, NULL)) != canOK)
        PRINTF_ERR(("ERROR canIoCtl(canIOCTL_FLUSH_TX_BUFFER) FAILED, Err= %d <line: %d>\n",
                    stat, __LINE__));
    }

    //set up the bus
    if (i == 0) {
      switch(m_usedBaudRate) {
        case 1000000:
          m_usedBaudRate = canBITRATE_1M;
          break;
        case 500000:
          m_usedBaudRate = canBITRATE_500K;
          break;
        case 250000:
          m_usedBaudRate = canBITRATE_250K;
          break;
        case 125000:
          m_usedBaudRate = canBITRATE_125K;
          break;
        case 100000:
          m_usedBaudRate = canBITRATE_100K;
          break;
        case 62500:
          m_usedBaudRate = canBITRATE_62K;
          break;
        case 50000:
          m_usedBaudRate = canBITRATE_50K;
          break;
        default:
          printf("Baudrate set to 125 kbit/s. \n");
          m_usedBaudRate = canBITRATE_125K;
          break;
      }
    }

    //set the channels busparameters
    stat = canSetBusParams(hnd, m_usedBaudRate, 0, 0, 0, 0, 0);
    if (stat < 0) {
      PRINTF_ERR(("ERROR canSetBusParams() in InitDriver(). Err = %d <line: %d>\n",
                  stat, __LINE__));
    }
  }
  printf("\n");
}


//////////////////////////////////////////////////
// Go off bus and close all open handles.
//////////////////////////////////////////////////
void Cleanup(void)
{
  unsigned int i;
  int          stat;

  for (i = 0; i < m_channelData.channelCount; i++) {
    if ((stat = canBusOff(m_channelData.channel[i].hnd)) != canOK)
      PRINTF_ERR(("ERROR canBusOff() FAILED Err= %d. <line: %d>\n", stat, __LINE__));


    if ((stat = canClose(m_channelData.channel[i].hnd)) != canOK) {
      PRINTF_ERR(("ERROR canClose() in Cleanup() FAILED Err= %d. <line: %d>\n",
                  stat, __LINE__));
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
// CheckAbort()
//
// check at transmission of a message burst, if key pressed
// if key pressed -> stop transmission
////////////////////////////////////////////////////////////////////////////////
int CheckAbort(void)
{
  INPUT_RECORD   ir;
  unsigned long  n;

  GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), &n);

  while (n > 0) {
    ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &ir, 1, &n);
    if ((n == 1) && (ir.EventType == KEY_EVENT)) {
      if (ir.Event.KeyEvent.bKeyDown)
        return TRUE;
    }
    n--;
  }
  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// Transmit a message using channel, id and flags from the global variables
// If print is TRUE the transmitted msg will be printed
//////////////////////////////////////////////////////////////////////////////
canStatus TransmitMessage(BOOL print)
{
  unsigned char msg[8];
  int           i;

  if (!m_Verbose) print = FALSE;

  if (print) {
    // The time stamp is not available when we call canWrite so it's
    // omitted here. (If we want to see when the message was sent, we
    // first need to enable transmission acknowledges with canIoCtl.
    // Then all sent messages will appear also as received messages,
    // but with the canMSG_TXACK flag set.
    printf("Ch:%u ID:%08x DLC:%u Flg:%02x            Data:",
           m_usedChannel, m_usedId, sizeof(msg), m_usedFlags);
  }

  for (i = 0; i < 8; i++) {
    msg[i] = (unsigned char) i;
    if (print) printf("%02x ", msg[i]);
  }

  if (print) printf("\n");

  return canWrite(m_DriverConfig->channel[m_usedChannel].hnd, m_usedId,
                  msg, sizeof(msg), m_usedFlags);
}



/*************************************************************/
/*************************************************************/
// MAIN
///////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  HANDLE        th[MAX_CHANNELS + 1];
  static int    running                 = 1;
  DWORD         active_handle;
  char          c;
  int           stat;
  unsigned int  i;

  m_usedId      = 0;
  m_usedChannel = 0;

  system("cls");
  printf("\n\nWelcome to yet another CANLIB Test Application, compiled at " __DATE__ "\n"
         "(Check out http://www.kvaser.com for more info.)\n\n"
         "Usage:\n"
         "  CANdemo [baudrate] [identifier]\n"
         "Press h for help\n\n\n -----------------------------------------------------------------\n\n");



  // Commandline can contain baudrate and an identifier, both optional.
  if (argc > 1) {
    m_usedBaudRate = atoi(argv[1]);
    if (m_usedBaudRate) {
      printf("Baudrate = %u\n\n", m_usedBaudRate);
      argc--;
      argv++;
    }
  }
  if (argc > 1) {
    sscanf (argv[1], "%lx", &m_usedId );
    if (m_usedId) {
      printf("Identifier = 0x%lx\n\n", m_usedId);   
    }
  }

  // open channel and set busparams, etc...
  InitDriver();

  //get std_input event handle
  th[0] = GetStdHandle(STD_INPUT_HANDLE);
  if (th[0] == INVALID_HANDLE_VALUE) 
    PRINTF_ERR(("ERROR inv handle (std_input). <line: %d>\n", __LINE__));

  for (i = 1; i < (m_channelData.channelCount + 1); i++) {
    HANDLE tmp;

    //go on bus (every channel)
    stat = canBusOn(m_channelData.channel[i-1].hnd);
    if (stat < 0) {
      PRINTF_ERR(("ERROR canBusOn(). Err = %d <line: %d>\n", stat, __LINE__));
    } 
    else {
      m_DriverConfig->channel[i-1].isOnBus = 1;
    }

    //get CAN - eventHandles
    stat = canIoCtl(m_channelData.channel[i-1].hnd,
                    canIOCTL_GET_EVENTHANDLE,
                    &tmp,
                    sizeof(tmp)); 
    if (stat < 0) {
      PRINTF_ERR(("canIoCtl(canIOCTL_GET_EVENTHANDLE) FAILED. Err = %d <line: %d>\n",
                  stat, __LINE__));
    }
    th[i] = tmp;
  }

  printDriverConfig();
  printf("\n");

  ///////////////////////////////////////////
  //Main LOOP
  while (running) {
    active_handle = WaitForMultipleObjects(m_channelData.channelCount + 1,
                                           th,
                                           FALSE /*any*/,
                                           INFINITE);

    //
    // CAN hardware event?
    //
    if (((active_handle - WAIT_OBJECT_0) > 0) &&
        ((active_handle - WAIT_OBJECT_0) <= m_channelData.channelCount)) 
    {
      unsigned int    j;
      long            id;
      unsigned char   data[8];
      unsigned int    dlc;
      unsigned int    flags;
      DWORD           time;
      int             moreDataExist;

      do {
        moreDataExist = 0;
        for (i = 0; i < m_channelData.channelCount; i++) {
          stat = canRead(m_channelData.channel[i].hnd, &id, &data[0], &dlc, &flags, &time);
          switch (stat) {
            case canOK:
              if (m_Verbose) {
                printf("RxMsg: Ch:%u ID:%08x DLC:%u Flg:%02x T:%08x Data:",
                       m_channelData.channel[i].channel, id, dlc, flags, time);
                if ((flags & canMSG_RTR) == 0) {
                  for (j = 0; j < dlc; j++) {
                    printf("%02x ", data[j]);
                  }
                }
                printf("\n");
              }
              moreDataExist = 1;
              break;
                
            case canERR_NOMSG:
              // No more data on this handle
              break;
              
            default:
              PRINTF_ERR(("ERROR canRead() FAILED, Err= %d <line: %d>\n", stat, __LINE__));
              break;
          }
        }
      } while (moreDataExist);
    }
    //STD_INPUT event
    else if (active_handle == WAIT_OBJECT_0) 
    {
      unsigned long  n;
      INPUT_RECORD   ir;

      ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &ir, 1, &n);

      if ((n == 1) && (ir.EventType == KEY_EVENT)) {
        if (ir.Event.KeyEvent.bKeyDown) {

          //pressed key switch
          switch ((c = ir.Event.KeyEvent.uChar.AsciiChar)) {

            //==================================================
            case 'V':
            case 'v':
              m_Verbose = !m_Verbose;
              if (m_Verbose) printf("Logging to screen ON. \n");
              else printf("Logging to screen OFF. \n");
              break;

            //==================================================
            case 't':
            {
              int stat;

              if (m_Verbose) {
                printf("TxMsg: ");
              }
              if (m_DriverConfig->channel[m_usedChannel].isOnBus  == 0) {
                printf("Cannot transmit message. \n");
                printf("Channel %d is OFF bus. \n", m_usedChannel);
                break;
              }
              stat = TransmitMessage(TRUE);
              if (stat < 0) {
                PRINTF_ERR(("ERROR TransmitMessage() FAILED Err= %d <line: %d>\n",
                            stat, __LINE__));
              }
            }
            break;

            //==================================================
            case 'T':
            {
              int stat;
              int tmp;

              if (m_Verbose) {
                printf("Toggling TxAcks on channel %d", m_usedChannel);
              }
              tmp = m_DriverConfig->channel[m_usedChannel].txAck? 0 : 1;
              stat = canIoCtl(m_DriverConfig->channel[m_usedChannel].hnd,
                              canIOCTL_SET_TXACK,
                              &tmp,
                              sizeof(tmp));
              if (stat == canOK) {
                m_DriverConfig->channel[m_usedChannel].txAck = tmp;
                printf(" - now %s\n", tmp? "on" : "off");
              } else {
                printf(" failed with error %d on line %d\n", stat, __LINE__);
              }
            }
            break;
            
            //==================================================
            case 'k':
            {
              printf("Time on channel %d: 0x%08x\n",
                     m_usedChannel,
                     canReadTimer(m_DriverConfig->channel[m_usedChannel].hnd));
            }
            break;
            
            //==================================================
            case 'B':
            case 'b':
            {
              if (m_Verbose) printf("Transmit message BURST. \n");

              // transmit a message burst
              unsigned char msg[8];
              int           i;
              int           stat;

              if (m_DriverConfig->channel[m_usedChannel].isOnBus == 0) {
                printf("Cannot transmit burst. \n");
                printf("Channel %d is OFF bus. \n", m_usedChannel);
                break;
              }

              for (i = 0; i < 8; i++) {
                msg[i] = (unsigned char) i;
              }

              printf("Press any key to stop.\n");

              for (i = 0; ; i++) {
                //Send bursts of 250 messages
                for (int j = 0; j < 250; j++) {
                  stat = TransmitMessage(FALSE);
                  if (stat != canOK) {
                    char err[100];
                    canGetErrorText((canStatus)stat, err, 100);
                    printf("ERROR: %s\n", err);
                    break;
                  }
                }
                if(stat != canOK)
                  break;
                //Wait until all the messages are sent
                stat = canWriteSync(m_DriverConfig->channel[m_usedChannel].hnd, 2000);
                if (stat != canOK) {
                  char err[100];
                  canGetErrorText((canStatus)stat, err, 100);
                  printf("ERROR: %s\n", err);
                  break;
                }

                if ((i % 20) == 0) {
                  printf("*");
                  if (CheckAbort()) {
                    printf("\nTransmission finished\n");
                    break;
                  }
                }
              }
              //flush transmit buffer
              if (canIoCtl(m_DriverConfig->channel[m_usedChannel].hnd,
                           canIOCTL_FLUSH_TX_BUFFER,
                           NULL,
                           NULL) != canOK) {
                PRINTF_ERR(("ERROR canIoCtl() in 'b' FAILED. Err = %d <line: %d>\n", __LINE__));
              }
              break;
            }

            //==================================================
            case 'R':
            case 'r':
              // Toggle sending of remote frames.
              m_usedFlags ^= canMSG_RTR;
              if (m_Verbose) {
                printf("Now sending %s frames.\n", m_usedFlags & canMSG_RTR? "remote" : "data");
              }
              break;

            //==================================================
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              m_usedChannel = c - '1';
              goto select;

            //==================================================
            case 'C': // channel selection
              m_usedChannel--;

              if (m_usedChannel < 0) 
                m_usedChannel = m_channelData.channelCount - 1;

              if (m_Verbose) 
                printf("Current channel = %d \n", m_usedChannel);

              break;

            //==================================================
            case 'c': // channel selection
              m_usedChannel++;
select:
              if (m_usedChannel >= (int)m_channelData.channelCount) 
                m_usedChannel = 0;

              if (m_Verbose) 
                printf("Current channel = %d \n", m_usedChannel);

              break;

            //==================================================
            case 'X': // id-type selection
            case 'x':
              // Toggle sending of extended frames.
              m_usedFlags ^= canMSG_EXT;
              if (m_Verbose) {
                printf("Now sending %s frames.\n",
                       m_usedFlags & canMSG_EXT? "extended" : "standard");
              }

              break;

            case 'i': // id selection
              m_usedId++;
              if (m_Verbose) 
                printf("Id set to 0x%08x\n", m_usedId);

              break;

            //==================================================
            case 'I':
              if (m_usedId > 0) m_usedId--;

              if (m_Verbose) 
                printf("Id set to 0x%08x\n", m_usedId);

              break;

            //==================================================
            case 'S':
              if ((stat = canRequestChipStatus(m_DriverConfig->channel[m_usedChannel].hnd)) != canOK) {
                PRINTF_ERR(("ERROR canRequestChipStatus() FAILED. Err= %d <line: %d>\n", stat, __LINE__));
              }
              break;
              
            //==================================================
            case 's':
              // print chip status here
              if (m_Verbose) { 
                printf("Channel %d is ", m_usedChannel);
                if (m_DriverConfig->channel[m_usedChannel].isOnBus) 
                  printf("ON bus. \n");
                else printf("OFF bus. \n");
              }
              if (m_Verbose) {
                DWORD flags;
                if ((stat = canReadStatus(m_DriverConfig->channel[m_usedChannel].hnd, &flags)) != canOK) {
                  PRINTF_ERR(("ERROR canReadStatus() FAILED. Err= %d <line: %d>\n", stat, __LINE__));
                }
                else {
                  printf("Bus status : ");
                  if (flags & canSTAT_ERROR_PASSIVE)
                    printf("canSTAT_ERROR_PASSIVE ");
                  if (flags & canSTAT_BUS_OFF)
                    printf("canSTAT_BUS_OFF ");
                  if (flags & canSTAT_ERROR_WARNING)
                    printf("canSTAT_ERROR_WARNING ");
                  if (flags & canSTAT_ERROR_ACTIVE)
                    printf("canSTAT_ERROR_ACTIVE ");
                  if (flags & canSTAT_TX_PENDING)
                    printf("canSTAT_TX_PENDING ");
                  if (flags & canSTAT_RX_PENDING)
                    printf("canSTAT_RX_PENDING ");
                  if (flags & canSTAT_TXERR)
                    printf("canSTAT_TXERR ");
                  if (flags & canSTAT_RXERR)
                    printf("canSTAT_RXERR ");
                  if (flags & canSTAT_HW_OVERRUN)
                    printf("canSTAT_HW_OVERRUN ");
                  if (flags & canSTAT_SW_OVERRUN)
                    printf("canSTAT_SW_OVERRUN ");

                  printf("\n");
                }
              }

              break;

            //==================================================
            case 'M':
            case 'm':
              // change output mode normal / silent
            {
              int stat;

              if (m_DriverConfig->channel[m_usedChannel].driverMode == canDRIVER_NORMAL) {
                stat = canSetBusOutputControl(m_DriverConfig->channel[m_usedChannel].hnd, canDRIVER_SILENT);
                if (stat < 0) {
                  PRINTF_ERR(("ERROR canSetBusOutputControl(). Err = %d <line: %d>\n", stat, __LINE__));
                }
                else {
                  if (m_Verbose) printf("Channel %d Drivermode set to canDRIVER_SILENT. \n", m_usedChannel);
                  m_DriverConfig->channel[m_usedChannel].driverMode = canDRIVER_SILENT;
                }

              }
              else {
                stat = canSetBusOutputControl(m_DriverConfig->channel[m_usedChannel].hnd, canDRIVER_NORMAL);
                if (stat < 0) {
                  PRINTF_ERR(("ERROR canBusOn(). Err = %d <line: %d>\n", stat, __LINE__));
                } 
                else {
                  if (m_Verbose) printf("Channel %d Drivermode set to canDRIVER_NORMAL. \n", m_usedChannel);
                  m_DriverConfig->channel[m_usedChannel].driverMode = canDRIVER_NORMAL;
                }
              }
            }
            break;

            //==================================================
            case 'O':
            case 'o':
            {
              int stat;
              // Go on bus/off bus  here for currentChannel
              if (m_DriverConfig->channel[m_usedChannel].isOnBus) {
                stat = canBusOff(m_DriverConfig->channel[m_usedChannel].hnd);
                if (stat < 0) {
                  PRINTF_ERR(("ERROR canBusOff(). Err = %d <line: %d>\n", stat, __LINE__));
                }
                else {
                  if (m_Verbose) printf("Channel %d Off bus. \n", m_usedChannel);
                  m_DriverConfig->channel[m_usedChannel].isOnBus = 0;
                }

              }
              else {
                stat = canBusOn(m_DriverConfig->channel[m_usedChannel].hnd);
                if (stat < 0) {
                  PRINTF_ERR(("ERROR canBusOn(). Err = %d <line: %d>\n", stat, __LINE__));
                } 
                else {
                  if (m_Verbose) printf("Channel %d On bus. \n", m_usedChannel);
                  m_DriverConfig->channel[m_usedChannel].isOnBus = 1;
                }
              }
            }
            break;

            //==================================================
            case 27:    // Esccape
            case 'q':
            case 'Q':
              running = FALSE;
              break;

            //==================================================
            case 'H':
            case 'h':
              help();
              break;

            //==================================================
            case 'E':
            case 'e':
              system("cls");
              break;

            //==================================================
            case 'D':
            case 'd':
              printDriverConfig();
              break;

            //==================================================
            default:
              break;
          }  //switch
        } 
      } 
    } //event type
  } // while


  // Go off bus & close channels.
  Cleanup();
  return NULL;

}
