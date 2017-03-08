/*
**                   Copyright 2007 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** This sample program enumerates all CAN channels in the computer and displays
** some information about them.
** ---------------------------------------------------------------------------
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


char CANCHANNEL_OPMODE_TEXT[5][20] = {"Unknown","None","Infrastructure","Reserved","AdHoc"};


void Usage(int argc, char* argv[])
{
  printf("\nCANLIB Channel Enumeration & Data\n");
  printf("\nA Part of the CANLIB SDK - see www.kvaser.com for more info.\n");
  printf("\n");
  printf("\nUsage: channeldata \n");

  exit(1);
}


void Check(char* id, canStatus stat)
{
  char buf[50];
  if (stat != canOK) {
    buf[0] = '\0';
    canGetErrorText(stat, buf, sizeof(buf));
    printf("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
  }
}

void main(int argc, char* argv[])
{
  canStatus stat;
  int i, chanCount;

  printf("Starting...\n");

  canInitializeLibrary();

  stat = canGetNumberOfChannels(&chanCount);
  Check("canGetNumberOfChannels", stat);
  if (chanCount<0 || chanCount > 1000) {
    printf("ChannelCount = %d but I don't believe it.\n", chanCount);
    exit(1);
  } else {
    printf("%d channels.\n", chanCount);
  }

  for (i=0; i<chanCount; i++) {
    DWORD tmp;
    DWORD quad[2];
    char name[64];
    unsigned long ean[2];

    printf("== Channel %d ===============================\n", i);

    stat = canGetChannelData(i, canCHANNELDATA_CHANNEL_CAP, &tmp, sizeof(tmp));
    Check("canGetChannelData", stat);
    printf("Channel Capabilities =  0x%08lx ", tmp);
    if (tmp & canCHANNEL_CAP_EXTENDED_CAN) printf("Ext ");
    if (tmp & canCHANNEL_CAP_BUS_STATISTICS) printf("Stat ");
    if (tmp & canCHANNEL_CAP_ERROR_COUNTERS) printf("ErrCnt ");
    if (tmp & canCHANNEL_CAP_CAN_DIAGNOSTICS) printf("Diag ");
    if (tmp & canCHANNEL_CAP_GENERATE_ERROR) printf("ErrGen ");
    if (tmp & canCHANNEL_CAP_GENERATE_OVERLOAD) printf("OvlGen ");
    if (tmp & canCHANNEL_CAP_TXREQUEST) printf("TxRq ");
    if (tmp & canCHANNEL_CAP_TXACKNOWLEDGE) printf("TxAck ");
    if (tmp & canCHANNEL_CAP_VIRTUAL) printf("Virt ");
    if (tmp & canCHANNEL_CAP_SIMULATED) printf("Simulated ");
    if (tmp & canCHANNEL_CAP_REMOTE) printf("Remote ");

    printf("\n");

    stat = canGetChannelData(i, canCHANNELDATA_TRANS_CAP, &tmp, sizeof(tmp));
    Check("canGetChannelData", stat);
    printf("DRVcan Capabilities =   0x%08lx ", tmp);
    if (tmp & canDRIVER_CAP_HIGHSPEED) printf("HiSpd ");
    printf("\n");

    stat = canGetChannelData(i, canCHANNELDATA_CHANNEL_FLAGS, &tmp, sizeof(tmp));
    Check("canGetChannelData", stat);
    printf("Channel Flags =         0x%08lx\n", tmp);

    stat = canGetChannelData(i, canCHANNELDATA_CARD_TYPE, &tmp, sizeof(tmp));
    Check("canGetChannelData", stat);
    printf("Board type =            0x%08lx", tmp);
    switch (tmp) {
      case canHWTYPE_NONE:
        printf(" (Unknown)\n");
        break;
      case canHWTYPE_VIRTUAL:
        printf(" (Virtual)\n");
        break;
      case canHWTYPE_LAPCAN:
        printf(" (LAPcan family)\n");
        break;
      case canHWTYPE_PCCAN:
        printf(" (PCcan Family)\n");
        break;
      case canHWTYPE_PCICAN:
        printf(" (PCIcan Family)\n");
        break;
      case canHWTYPE_USBCAN:
        printf(" (USBcan Family)\n");
        break;
      case canHWTYPE_PCICAN_II:
        printf(" (PCIcan II Family)\n");
        break;
      case canHWTYPE_USBCAN_II:
        printf(" (USBcan II Family (including Kvaser Memorator))\n");
        break;
      case canHWTYPE_LEAF:
        printf(" (Kvaser Leaf Family)\n");
        break;
      case canHWTYPE_PC104_PLUS:
        printf(" (PC104+ Family)\n");
        break;
      case canHWTYPE_PCICANX_II:
        printf(" (PCIcanx II Family)\n");
        break;
      case canHWTYPE_MEMORATOR_PRO:
      case canHWTYPE_UVP:
        printf(" (Kvaser Memorator Professional Family)\n");
        break;
      case canHWTYPE_USBCAN_PRO:
        printf(" (Kvaser USBcan Professional Family)\n");
        break;
      case canHWTYPE_IRIS:
        printf(" (Kvaser BlackBird)\n");
        break;
      case canHWTYPE_EAGLE:
        printf(" (Kvaser Eagle)\n");
        break;
      case canHWTYPE_MINIPCIE:
        printf(" (Kvaser Mini PCI Express)\n");
        break;
      case canHWTYPE_USBCAN_LIGHT:
        printf(" (Kvaser USBcan Light)\n");
        break;
      default:
        printf(" (Unsupported)\n");
        break;
    }

    stat = canGetChannelData(i, canCHANNELDATA_CARD_NUMBER, &tmp, sizeof(tmp));
    Check("canCHANNELDATA_CARD_NUMBER", stat);
    printf("Board Number =          0x%08lx\n", tmp);

    stat = canGetChannelData(i, canCHANNELDATA_CHAN_NO_ON_CARD, &tmp, sizeof(tmp));
    Check("canCHANNELDATA_CHAN_NO_ON_CARD", stat);
    printf("Channel no on board =   0x%08lx\n", tmp);

    stat = canGetChannelData(i, canCHANNELDATA_CARD_SERIAL_NO, quad, sizeof(quad));
    Check("canCHANNELDATA_CARD_SERIAL_NO", stat);
    printf("Board S/N =             0x%08lx 0x%08lx\n", quad[0], quad[1]);

    stat = canGetChannelData(i, canCHANNELDATA_TRANS_SERIAL_NO, quad, sizeof(quad));
    Check("canCHANNELDATA_TRANS_SERIAL_NO", stat);
    printf("DRVcan S/N =            0x%08lx 0x%08lx\n", quad[0], quad[1]);

    stat = canGetChannelData(i, canCHANNELDATA_CARD_FIRMWARE_REV, quad, sizeof(quad));
    Check("canCHANNELDATA_CARD_FIRMWARE_REV", stat);
    printf("Board F/W version =     0x%08lx 0x%08lx\n", quad[0], quad[1]);

    stat = canGetChannelData(i, canCHANNELDATA_CARD_HARDWARE_REV, quad, sizeof(quad));
    Check("canCHANNELDATA_CARD_HARDWARE_REV", stat);
    printf("Board H/W version =     0x%08lx 0x%08lx\n", quad[0], quad[1]);

    stat = canGetChannelData(i, canCHANNELDATA_CARD_UPC_NO, ean, sizeof(ean));
    Check("canCHANNELDATA_CARD_UPC_NO", stat);
    printf("Board UPC/EAN =         0x%08x 0x%08x\n", ean[1], ean[0]);

    stat = canGetChannelData(i, canCHANNELDATA_TRANS_UPC_NO, ean, sizeof(ean));
    Check("canCHANNELDATA_TRANS_UPC_NO", stat);
    printf("DRVcan UPC/EAN =        0x%08x 0x%08x\n", ean[1], ean[0]);

    stat = canGetChannelData(i, canCHANNELDATA_CHANNEL_NAME, name, sizeof(name));
    Check("canCHANNELDATA_CHANNEL_NAME", stat);
    printf("Channel name =          '%s'\n", name);

    stat = canGetChannelData(i, canCHANNELDATA_REMOTE_OPERATIONAL_MODE, &tmp, sizeof(tmp));
    if (stat != canERR_NOT_IMPLEMENTED) {
      Check("canCHANNELDATA_REMOTE_OPERATIONAL_MODE", stat);
      printf("Operational mode =      %d (%s)\n", tmp, CANCHANNEL_OPMODE_TEXT[tmp]);

      stat = canGetChannelData(i, canCHANNELDATA_REMOTE_PROFILE_NAME, name, sizeof(name));
      Check("canCHANNELDATA_REMOTE_PROFILE_NAME", stat);
      printf("Profile name =          '%s'\n", name);

      stat = canGetChannelData(i, canCHANNELDATA_REMOTE_HOST_NAME, name, sizeof(name));
      Check("canCHANNELDATA_REMOTE_HOST_NAME", stat);
      printf("Remote host name =      '%s'\n", name);

      stat = canGetChannelData(i, canCHANNELDATA_REMOTE_MAC, name, sizeof(name));
      Check("canCHANNELDATA_REMOTE_MAC", stat);
      printf("MAC =                   '%s'\n", name);
    } else {
      printf("Device is not capable of acting in remote operational mode\n");
    }
    printf("\n");

  }
}
