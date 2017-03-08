// JoyStick.cpp : a program that reads the position of a joystick
// (connected to the game port on the computer) and sends CAN messages
// based on these readings.
//
// Mattias Busck, Accurate Technologies

#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include "canlib.h"
#include "canstat.h"


typedef struct 
{
  char x;
  char y;
  char z;
  char r;
  unsigned short buttons;
  unsigned short pov;
} JoyData;


bool PrintJoystickInfo(int id, JOYCAPS &jc)
{

  if (joyGetDevCaps(id, &jc, sizeof(jc)) != JOYERR_NOERROR )
    return false;

  printf("Joystick %d:\n", id);
  printf(" Manufacturer: %s\n", jc.szPname);
  printf(" Num Buttons: %d\n", jc.wMaxButtons);
  printf(" Num Axes: %d\n", jc.wNumAxes );

  return true;
}

void print_usage_exit()
{
  printf("usage: joystick [channel][bitrate][id][msperiod]\n");
  exit(1);
}

void CanCheck(canStatus stat)
{
  if (stat < 0) 
  {
    char errorstring[1000];
    canGetErrorText(stat, errorstring, 1000);
    printf("can error: %s\n", errorstring);
  }
}

canStatus CalcBusParams(long & bitrate, unsigned int &tseg1, unsigned int &tseg2, unsigned int &sjw)
{
  if (bitrate > 0) 
  {
    switch (bitrate)
    {
    case 1000000: bitrate = canBITRATE_1M;   break;
    case 500000:  bitrate = canBITRATE_500K; break;
    case 250000:  bitrate = canBITRATE_250K; break;
    case 125000:  bitrate = canBITRATE_125K; break;
    case 100000:  bitrate = canBITRATE_100K; break;
    case 83333:   bitrate = canBITRATE_83K;  break;
    case 62500:   bitrate = canBITRATE_62K;  break;
    case 50000:   bitrate = canBITRATE_50K;  break;
    default:      bitrate = -100;
    }
  }
  unsigned int nosamp;
  unsigned int syncmode;
  return canTranslateBaud(&bitrate, &tseg1, &tseg2, &sjw, &nosamp, &syncmode);
} 

int main(int argc, char* argv[])
{
  int channel = 0;
  long bitrate = 500000;
    unsigned int tseg1;
    unsigned int tseg2; 
    unsigned int sjw;

  int id = 500;
  int period = 100;
  if (argc > 1 && !sscanf(argv[1], "%d", &channel)) print_usage_exit();
  if (argc > 2 && !sscanf(argv[2], "%d", &bitrate)) print_usage_exit();
  if (argc > 3 && !sscanf(argv[3], "%d", &id)) print_usage_exit();
  if (argc > 4 && !sscanf(argv[4], "%d", &period)) print_usage_exit();

  canInitializeLibrary();
  canHandle h;

  char channelName[500];
  CanCheck(canGetChannelData(channel, canCHANNELDATA_CHANNEL_NAME, &channelName, sizeof(channelName)));
  CanCheck((canStatus)(h = canOpenChannel(channel, canOPEN_ACCEPT_VIRTUAL)));
  CalcBusParams(bitrate,tseg1, tseg2, sjw);
  CanCheck(canSetBusParams(h, bitrate, tseg1, tseg2, sjw, 1, 0));
  CanCheck(canBusOn(h));

  JOYCAPS jc;
  if (!PrintJoystickInfo(JOYSTICKID1, jc)) {
    printf("No joystick found!\n");
    return 0;
  }

  printf("Sending joystick data on %s @%d bps using id %d @ %d ms\n", channelName, bitrate, id, period);

  JoyData j;
  int counter = 0;
  bool isOverflow = 0;

  for(;;) {
    JOYINFOEX jex;
    
    jex.dwSize = sizeof(jex);
    jex.dwFlags = JOY_RETURNALL;

    if ( joyGetPosEx(JOYSTICKID1, &jex) != JOYERR_NOERROR) {
      printf("Error reading joystick values!\n");
      return 0;
    }

    j.x = (char)(jex.dwXpos / 327.67 - 100);
    j.y = (char)(jex.dwYpos / -327.67 + 100);
    j.z = (char)(jex.dwZpos / -327.67 + 100);
    j.r = (char)(jex.dwRpos / 327.67 - 100);
    j.pov = (unsigned short)(jex.dwPOV / 100);
    // Set enable signal
    if (jex.dwPOV == 0xFFFF) j.pov = 0;
    else j.pov |= 0x8000;
    j.buttons = (unsigned short)jex.dwButtons;

    canStatus stat = canWrite(h, id, &j, sizeof(j), 0);

    if (stat != canERR_TXBUFOFL) CanCheck(stat);

    if ((counter += period) >= 200)
    {
      printf("X:%4d Y:%4d Z:%4d R:%4d POV:%4d BTN:%3X %s\r", j.x, j.y, j.z, j.r, j.pov & 0x7ffff, j.buttons, stat == canERR_TXBUFOFL ? "TX OVERFLOW" : "           ");
      counter = 0;
    }
    Sleep(period);
  }

  return 0;
}
