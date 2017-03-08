/* gensig.c: Signal generator for CANdb testing */
/* $Id$ */

/***************************************************************************/

#include <windows.h>
#include <mmsystem.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "canlib.h"

/***************************************************************************/
/* Defines: */

#ifndef M_PI
#  define M_PI          3.14159265358979323846
#endif

/***************************************************************************/
/* Static variables: */

/***************************************************************************/


static void errorExit (char *message, canStatus stat)
{
  char buf [50];
  if (stat != canOK) {
    buf[0] = '\0';
    canGetErrorText (stat, buf, sizeof (buf));
    fprintf (stderr, "%s: failed, stat=%d (%s)\n", message, (int)stat, buf);
    exit (1);
  }
} /* errorExit */


static void usage (void)
{
  printf ("\nGenSig - a sample program from CANLIB SDK (http://www.kvaser.com)\n"
          "This program generates a stream of CAN messages. The data in the messages\n"
          "form a signal whose shape can be selected using the command-line flags\n"
          "described below.\n\n"
          "If you happen to have a CANalyzer, you can use gensig.dbc to look\n"
          "at the signal(s).\n\n"
          "  -B<speed>: Set the CAN bus bit rate; 1000, 500, 250 or 125 kbps.\n"
          "  -rt: Boost our priority to realtime.\n"
          "  -all: Generate all shapes.\n"
          "  -trig: Generate a sine and a cosine wave.\n"
          "  -ramp: Generate a ramp.\n"
          "  -digital: Generate a square wave.\n"
          "  -float: Generate messages with floating point numbers,\n"
          "          forming sine/cosine waves.\n"
          "  -motorola: Use Motorola byte order (Big Endian.)\n"
          "  -sleep<N>: Sleep N ms extra in the main loop (use to slow down program).\n"
          "\n"
          "The program prints the following characters:\n"
          "  *  One character per 1000 'trig' messages.\n"
          "  #  One character per 1000 'digital' messages.\n"
          "  +  One character per 1000 'ramp' messages.\n"
          "  .  One character per 1000 'float' messages.\n"
         );
  
  exit (1);
} /* usage */


static void help (void)
{
  printf ("Available commands:\n");
  printf ("  q: Change debug level\n");
  printf ("ESC: Quit.\n");
} /* help */


/*
** Use the performance counter to get the current time.
*/
static unsigned long get_time_1ms (void)
{
  LARGE_INTEGER freq, time;
  QueryPerformanceFrequency (&freq);
  QueryPerformanceCounter (&time);
  freq.QuadPart /= 1000;
  time.QuadPart /= freq.QuadPart;
  return time.LowPart;
} /* get_time_1ms */


static void clear_msg (unsigned char *msg)
{
  memset (msg, 0, 8);
} /* clear_msg */


static int store_int (unsigned char *msg, int pos, int len, int value, int motorola)
{
  int bpos = 0, bit = 0;

  if ((pos >= 64) || (pos < 0) || (len < 1) || ((pos + len - 1) >= 64)) return -1;
  if (!msg) return -1;

  bpos = pos / 8;
  bit = pos % 8;

  while (len > 0) {
    unsigned int mask = 0, v;
    if (len == 1) mask = 0x01;
    else if (len == 2) mask = 0x03;
    else if (len == 3) mask = 0x07;
    else if (len == 4) mask = 0x0f;
    else if (len == 5) mask = 0x1f;
    else if (len == 6) mask = 0x3f;
    else if (len == 7) mask = 0x7f;
    else if (len >= 8) mask = 0xff;
    mask = (mask << bit) & 0xff;
    if (motorola) {
      int sh = len - (8 - bit);
      if (sh < 0) sh = 0;
      v = ((value >> sh) << bit) & 0xff;
    }
    else v = (value << bit) & 0xff;
    msg [bpos] = (msg [bpos] & ~mask) | (v & mask);
    if (!motorola) value = value >> (8 - bit);
    len -= 8 - bit;
    bit = 0;
    ++bpos;
  }

  return 0;
} /* store_int */


static int store_float (unsigned char *msg, int pos, float f_value, int motorola)
{
  int bpos = 0, bit = 0;
  int len = sizeof (float) * 8;
  unsigned int value = 0;

  if (sizeof (value) != sizeof (f_value)) return -1;

  memcpy (&value, &f_value, sizeof (value));

  if ((pos >= 64) || (pos < 0) || (len < 1) || ((pos + len - 1) >= 64)) return -1;
  if (!msg) return -1;

  bpos = pos / 8;
  bit = pos % 8;

  while (len > 0) {
    unsigned int mask = 0, v;
    if (len == 1) mask = 0x01;
    else if (len == 2) mask = 0x03;
    else if (len == 3) mask = 0x07;
    else if (len == 4) mask = 0x0f;
    else if (len == 5) mask = 0x1f;
    else if (len == 6) mask = 0x3f;
    else if (len == 7) mask = 0x7f;
    else if (len >= 8) mask = 0xff;
    mask = (mask << bit) & 0xff;
    if (motorola) {
      int sh = len - (8 - bit);
      if (sh < 0) sh = 0;
      v = ((value >> sh) << bit) & 0xff;
    }
    else v = (value << bit) & 0xff;
    msg [bpos] = (msg [bpos] & ~mask) | (v & mask);
    if (!motorola) value = value >> (8 - bit);
    len -= 8 - bit;
    bit = 0;
    ++bpos;
  }

  return 0;
} /* store_float */


/***************************************************************************/


int main (int argc, char* argv[])
{
  HANDLE notification_event;
  int i, r, can_handle;
  int ready = 0;
  unsigned int btr0 = 0, btr1 = 0;

  int send_trig = 0,
      send_digital = 0,
      send_ramp = 0,
      send_float = 0,
      rt = 0,
      motorola = 0;

  unsigned int trig_id = 100,
               digital_id = 101,
               ramp_id = 102,
               float_id = 103;

  double sin1_period = 1.0,
         sin2_period = 0.7,
         sin1_amp = 100,
         sin2_amp = 100,
         cos1_period = 1.5,
         cos2_period = 0.5,
         cos1_amp = 100,
         cos2_amp =  50;

  unsigned long tstart;

  unsigned long iterations = 0;

  int tseg1 = 0,
      tseg2 = 0,
      sjw = 0,
      nosamp = 0,
      sync_mode = 0,
      prescaler = 0;
  long freq;
  int baudrate = canBITRATE_1M,
      baudrate_raw = -1,
      channel = 0;
  int debug_level = 0;
  unsigned int diag_id = 2040;
  unsigned long seq = 0;
  unsigned extra_sleep = 0;

  canInitializeLibrary ();

  for (i = 1; i < argc; i++) {
    int tmp, c;
    if (sscanf(argv[i], "-%d%c", &tmp, &c) == 1) {
      channel = tmp;
    }
    else if (sscanf(argv[i], "-sleep%d%c", &tmp, &c) == 1) {
      extra_sleep = tmp;
    }
    else if (strcmp (argv [i], "-sleep") == 0) {
      ++i;
      if (i >= argc) usage ();
      if (sscanf(argv[i], "%d%c", &tmp, &c) == 1) extra_sleep = tmp;
      else usage ();
    }

    else if (sscanf(argv[i], "-B%d%c", &tmp, &c) == 1) {
      baudrate_raw = tmp;
    }
    else if (strcmp (argv [i], "-B") == 0) {
      ++i;
      if (i >= argc) usage ();
      if (sscanf(argv[i], "%d%c", &tmp, &c) == 1) baudrate_raw = tmp;
      else usage ();
    }
    else if (strcmp (argv [i], "-rt") == 0) {
      rt = 1;
    }
    else if (strcmp (argv [i], "-all") == 0) {
      send_trig = 1;
      send_digital = 1;
      send_ramp = 1;
      send_float = 1;
    }
    else if (strcmp (argv [i], "-trig") == 0) {
      send_trig = 1;
    }
    else if (strcmp (argv [i], "-digital") == 0) {
      send_digital = 1;
    }
    else if (strcmp (argv [i], "-ramp") == 0) {
      send_ramp = 1;
    }
    else if (strcmp (argv [i], "-float") == 0) {
      send_float = 1;
    }
    else if (strcmp (argv [i], "-motorola") == 0) {
      motorola = 1;
    }
    else {
      usage ();
    }
  }

  if (baudrate_raw != -1) {
    switch (baudrate_raw) {
      case 1000: baudrate = canBITRATE_1M; break;
      case 500:  baudrate = canBITRATE_500K; break;
      case 250:  baudrate = canBITRATE_250K; break;
      case 125:  baudrate = canBITRATE_125K; break;
      default:   usage ();
                 break;
    }
  }

  if (!send_trig && !send_digital && !send_ramp) usage ();

  freq = baudrate;
  canTranslateBaud (&freq, &tseg1, &tseg2, &sjw, &nosamp, &sync_mode);
  prescaler = 16000000L / freq / (tseg1 + tseg2 + 1) / 2;
  btr0 = 0;
  btr1 = 0;
  btr0 |= ((sjw - 1) & 0x03) << 6;
  btr0 |= (prescaler - 1) & 0x3f;
  btr1 |= (nosamp == 3) ? 0x80 : 0;
  btr1 |= ((tseg2 - 1) & 0x07) << 4;
  btr1 |= (tseg1 - 1) & 0x0f;

  printf ("Parameters: \n");
  printf ("  debug_level=%d\n", debug_level);
  printf ("  baudrate=%d\n", baudrate);
  printf ("  baudrate_raw=%d/0x%08x\n", baudrate_raw, baudrate_raw);
  printf ("  freq=%d\n", freq);
  printf ("  prescaler=%d\n", prescaler);
  printf ("  channel=%d\n", channel);
  printf ("  btr0=%d/0x%02x, btr1=%d/0x%02x\n", btr0, btr0, btr1, btr1);
  printf ("  tseg1=%d, tseg2=%d\n", tseg1, tseg2);


  can_handle = canOpenChannel (channel, 0); 
  if (can_handle < 0) errorExit ("canOpenChannel()", (canStatus) can_handle);

  if ((btr0 != 0) && (btr1 != 0)) {
    r = canSetBusParamsC200 (can_handle, (unsigned char) btr0, (unsigned char) btr1);
  }
  else {
    r = canSetBusParams (can_handle, canBITRATE_1M, 0, 0, 0, 0, 0);
  }

  r = canBusOn (can_handle);
  if (r < 0) errorExit ("canBusOn", r);

  canIoCtl (can_handle,
            canIOCTL_GET_EVENTHANDLE,
            &notification_event,
            sizeof (notification_event));

  timeBeginPeriod (1);

  if (rt) {
    HANDLE thread_handle;
    HANDLE process_handle;

    process_handle = GetCurrentProcess ();
    SetPriorityClass (process_handle, REALTIME_PRIORITY_CLASS);
    thread_handle = GetCurrentThread ();
    SetThreadPriority (thread_handle, THREAD_PRIORITY_TIME_CRITICAL);
  }

  tstart = get_time_1ms ();

  iterations = 0;

  while (!ready) {
    int active_handle = 0;
    unsigned char msg [8];
    int flags, dlc;

    double ts = ((double) get_time_1ms () - tstart) / 1000;

    ++iterations;

    if (send_trig) {
      int cos_active = (iterations % 2) != 0;

      double sin1 = sin (ts / sin1_period * M_PI * 2) * sin1_amp,
             sin2 = sin (ts / sin2_period * M_PI * 2) * sin2_amp + sin2_amp,
             cos1 = cos (ts / cos1_period * M_PI * 2) * cos1_amp,
             cos2 = cos (ts / cos2_period * M_PI * 2) * cos2_amp + cos2_amp;

      int sin_state = 0;
      if (sin1 > 0) sin_state = 2;
      if (sin1 < 0) sin_state = 1;

      clear_msg (msg);
      store_int (msg,  0,  2, sin_state, motorola);
      store_int (msg, 16, 16, (int) sin1, motorola);
      store_int (msg, 32, 16, (int) cos1, motorola);
      store_int (msg, 48,  8, cos_active, motorola);
      if (cos_active) store_int (msg, 56,  8, (int) cos2, motorola);
                 else store_int (msg, 56,  8, (int) sin2, motorola);

      dlc = 8;
      flags = canMSG_STD;
      r = canWrite (can_handle, trig_id, msg, dlc, flags);
      if (r == canOK) {
        seq++;
        if ((seq % 1000) == 0) printf ("*");
      }
    }

    if (send_digital) {
      int d = 512;
      unsigned int t1 = ((iterations / d      ) & 0x01) * 255,
                   t2 = ((iterations / d /   2) & 0x01) * 255,
                   t3 = ((iterations / d /   4) & 0x01) * 255,
                   t4 = ((iterations / d /   8) & 0x01) * 255,
                   t5 = ((iterations / d /  16) & 0x01) * 255,
                   t6 = ((iterations / d /  32) & 0x01) * 255,
                   t7 = ((iterations / d /  64) & 0x01) * 255,
                   t8 = ((iterations / d / 128) & 0x01) * 255;
      clear_msg (msg);
      store_int (msg,  0,  8, t1, motorola);
      store_int (msg,  8,  8, t2, motorola);
      store_int (msg, 16,  8, t3, motorola);
      store_int (msg, 24,  8, t4, motorola);
      store_int (msg, 32,  8, t5, motorola);
      store_int (msg, 40,  8, t6, motorola);
      store_int (msg, 48,  8, t7, motorola);
      store_int (msg, 56,  8, t8, motorola);

      dlc = 8;
      flags = canMSG_STD;
      r = canWrite (can_handle, digital_id, msg, dlc, flags);
      if (r == canOK) {
        seq++;
        if ((seq % 1000) == 0) printf ("#");
      }
    }

    if (send_ramp) {
      unsigned int tri = iterations % 4000,
                   ramp = iterations % 1000;
      if (tri >= 2000) tri = 4000 - tri;
      tri -= 1000;
      clear_msg (msg);
      store_int (msg,  0, 16, ramp, motorola);
      store_int (msg, 16, 16, tri, motorola);

      dlc = 8;
      flags = canMSG_STD;
      r = canWrite (can_handle, ramp_id, msg, dlc, flags);
      if (r == canOK) {
        seq++;
        if ((seq % 1000) == 0) printf ("+");
      }
    }

    if (send_float) {
      double sin1 = sin (ts / sin1_period * M_PI * 2) * sin1_amp *  10,
             cos1 = cos (ts / cos1_period * M_PI * 2) * cos1_amp * 100;

      clear_msg (msg);
      store_float (msg,  0, (float) sin1, motorola);
      store_float (msg, 32, (float) cos1, motorola);

      dlc = 8;
      flags = canMSG_STD;
      r = canWrite (can_handle, float_id, msg, dlc, flags);
      if (r == canOK) {
        seq++;
        if ((seq % 1000) == 0) printf (".");
      }
    }


    active_handle = 0;

    if (r == canERR_TXBUFOFL) printf ("Error: TX overflow\n");

    { unsigned long n;
      INPUT_RECORD ir;
      if (PeekConsoleInput (GetStdHandle (STD_INPUT_HANDLE), &ir, 1, &n)) {
        if (n != 0) active_handle = 1;
      }
    }

    if (active_handle == 1) { /* keyboard */
      unsigned long n;
      INPUT_RECORD ir;

      ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &ir, 1, &n);
      if ((n == 1) && (ir.EventType == KEY_EVENT)) {
        if (ir.Event.KeyEvent.bKeyDown) {
          switch (ir.Event.KeyEvent.uChar.AsciiChar) {

            case 'q': /* change debug level */
                 if (debug_level > 0) debug_level = 0;
                                 else debug_level = 1;
                 printf ("debug_level=%d\n", debug_level);
                 break;

            case 'h': /* print help */
                 help ();
                 break;


            case 27: /* ESC: quit */
                 ready = TRUE;
                 break;
          }
        }
      }
    }

    if (extra_sleep) Sleep(extra_sleep + 1);
    else Sleep (1);
  }

  canBusOff (can_handle);
  canClose (can_handle);

  timeEndPeriod (1);

  return 0;
} /* main */


/***************************************************************************/


