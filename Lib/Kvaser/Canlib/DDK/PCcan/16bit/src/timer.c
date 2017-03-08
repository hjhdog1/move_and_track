/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#include <stdlib.h>
#include <dos.h>

#include "std.h"
#include "canlib.h"
#ifdef _WINDOWS
#include <mmsystem.h>
#endif

#include "timer.h"

#define VTD_TICKS          0x100
#define VTD_MILLISECONDS   0x101

#ifdef __BORLANDC__
#define _MK_FP MK_FP
#endif

/*
** Returns the system time with milliseconds resolution.
** No assumptions about the format of the return value must be made,
** only that it is a monotonically increasing "tick counter".
*/
#ifdef _WINDOWS
/*
** In Windows, we can use the multimedia timers directly.
*/
PUBLIC DWORD GetTimer(void)
{
    return timeGetTime();
}

PUBLIC DWORD GetMsTimer(void)
{
  return timeGetTime() / TIMERFREQ;
}

#else /*_WINDOWS*/

/*
** DOS Only.
** If we are running in a DOS box under Windows, we can read the
** multimedia timer if we ask the VTD. 
** The code reads the VTD in a DOS box under Windows.
*/
PRIVATE DWORD (far *vtd)(void);


#ifdef _MSC_VER
#define EMIT(X) __asm _emit X
#endif
#ifdef __BORLANDC__
#define EMIT(X) __emit__(X);
#endif

/*
** Type == VTD_MILLISECONDS (101h) -> result in milliseconds
** Type == VTD_TICKS (100h) -> result in ticks @ 840 ns
*/
PRIVATE DWORD GetVTD(int typ)
{
   unsigned int b0, b1;

	__asm   pushf			  // Unfortunately, we have to turn off interrupts...
	__asm	cli				  // ...as those interrupting us are not polite...
	__asm   mov   ax, typ	  // ...enough to save EAX.
	vtd();
	EMIT(0x66)                // Operand-size override
	__asm   mov   bx, ax      // Save lower part
	__asm   mov   cx, 16
	EMIT(0x66)
	__asm   shr   ax, cl      // Shift down higher part

	__asm   mov   b0, bx
	__asm   mov   b1, ax

    __asm   pop ax            // Pop flags
    __asm   and ax, 0x200     // IF flag
    __asm   jz      noint     // Don't enable interupts if they were disabled.
	__asm	sti
noint:

	return (DWORD)(((DWORD)b1) << 16) + b0;
}


/*
** DOS Only.
** Read the fast timer, read the clock, read the timer again -- if MSB has
** wrapped, repeat.
*/
PRIVATE void near dummy (void) {}

/* Defunct, left for debugging purposes.
unsigned readtimer1 (void) // From delay.cas
{
  __asm pushf                    // Save interrupt flag
  __asm cli                      // Disable interrupts                        
  __asm mov  al,0h               // Latch timer 0                             
  __asm out  43h,al
      dummy();                 // Waste some time 
  __asm in   al,40h              // Counter --> bx                            
  __asm mov  bl,al               // LSB in BL                                 
      dummy();                 // Waste some time
  __asm in   al,40h
  __asm mov  bh,al               // MSB in BH
  __asm not  bx                  // Need ascending counter
  __asm popf                     // Restore interrupt flag
  return( _BX );
}
*/
/*
** The procedure is taken from delay.cas. We have to determine if
** the timer counts in steps of 1 or 2.
*/
/* Also from delay.cas:
** Description    Determine the multiplier required to convert milliseconds
**                to an equivalent interval timer value.  Interval timer 0
**                is normally programmed in mode 3 (square wave), where
**                the timer is decremented by two every 840 nanoseconds;
**                in this case the multiplier is 2386.  However, some
**                programs and device drivers reprogram the timer in mode 2,
**                where the timer is decremented by one every 840 ns; in this
**                case the multiplier is halved, i.e. 1193.
**
**                When the timer is in mode 3, it will never have an odd value.
**                In mode 2, the timer can have both odd and even values.
**                Therefore, if we read the timer 100 times and never
**                see an odd value, it's a pretty safe assumption that
**                it's in mode 3.  This is the method used in timer_init.
*/

/*
** Hard DOS Only. (In DOS boxes under Windows, we are using the VTD)
** Initialize the timer chip.
*/
PRIVATE void initializeTimer(void)
{
  unsigned char status;
/* Some debugging code:
{
  unsigned char tbl[20];
  char c = 0;
  long i = 0;

  __asm pushf
  __asm cli

  for (c = 0; c < 20; c++) {
    WRITE_PORT_UCHAR(0x20, 0x0a); dummy();
    tbl[c] = READ_PORT_UCHAR(0x20);

    for (i = 0; i < 10000L; i++)
      ;
  }
  __asm popf

  for (i = 0; i < 20; i++)
    printf(" %02x", tbl[i]);
  printf("\n");
}*/

  //
  // Verify that timer0 is in one of the two usual modes: 2 or 3.
  //
  __asm pushf
  __asm cli
  __asm mov  al,0e2h             /* Read back timer 0 status        */
  __asm out  43h,al
      dummy();                 /* Waste some time */
  __asm in   al,40h              /* Status */
  __asm mov status,al
  __asm popf

  if ((status & 0x3f) == 0x36 || (status & 0x3f) == 0x34)
     return;

  //
  // Change timer 0 to mode 3. Apparently, we have to write a counter
  // value after the control word.
  //
  WRITE_PORT_UCHAR(0x43, 0x36);
  dummy();
  WRITE_PORT_UCHAR(0x40,0);
  dummy();
  WRITE_PORT_UCHAR(0x40,0);
}

/*
** DOS Only.
*/
PRIVATE void restoreTimer(void)
{
  //
  // We will not restore the timer mode even if we changed it at startup.
  //
}

/*
** DOS Only.
** Read the timer value from timer0. Works with timer mode 2 and 4
** (the commonly used modes); returns a value from 0 to 0xffff which
** wraps once for each bios-timer interrupt.
** The "frequency" of the returned value should always be divided by
** 1193 to get milliseconds.
**
** A chrystal in a certain PC had the frequency 14.31818 MHz; when divided by
** 12 gives the frequency 119318,16666667 (in the AT technical reference,
** there is a chrystal at 2.38 MHz).
** 14.31818/12/65536 == 18.20650.  65536/3600 == 18.20444
*/
PRIVATE unsigned int ReadHWTimer (void)
{
  unsigned char status;
  unsigned short time;

  __asm pushf                    /* Save interrupt flag                       */
  __asm cli                      /* Disable interrupts                        */
  __asm mov  al,0c2h             /* Read back timer 0 count and status        */
  __asm out  43h,al
      dummy();                 /* Waste some time */
  __asm in   al,40h              /* Status */
  __asm mov  cl,al
      dummy();
  __asm in   al,40h              /* Counter --> bx                            */
  __asm mov  bl,al               /* LSB in BL                                 */
      dummy();                 /* Waste some time */
  __asm in   al,40h
  __asm mov  bh,al               /* MSB in BH                                 */
  __asm not  bx                  /* Need ascending counter                    */
  __asm popf                     /* Restore interrupt flag                    */

  __asm mov time,bx
  __asm mov status,cl

  if ((status & 0x0f) == 0x06) {
    // mode 3. Square wave. The timer goes twice from 0 to 0xfffe in increments
    // of two for each timer interrupt. We have to check the toggled pin to
    // determine in which of the two parts we are.
    time >>= 1;
    if (!(status & 0x80))
       time += 0x8000;
  }

  return time;
}

/*
** DOS Only.
** Interrupts should be enabled when called as the bios time will not be
** updated otherwise
*/
PRIVATE DWORD ReadTimer(void)
{
    unsigned int t0,t1,biosTime;

    do {
      t0 = ReadHWTimer();

      __asm pushf
      __asm cli

      biosTime =  *(unsigned int _far*)_MK_FP(0x40, 0x6c);
      // Check if a timer interrupt is pending; if so, add one to
      // biosTime.
      // Disable the interrupts so no one else tries to read anything from
      // the 8259 while we are accessing it.
      WRITE_PORT_UCHAR(0x20, 0x0a); // Read the IRR
      if (READ_PORT_UCHAR(0x20) & 0x01)
         biosTime++;
      t1 = ReadHWTimer();

      __asm popf

    } while (t1 < t0);

    return ((DWORD)biosTime << 16) + t1;
}

/*
** Here are the two routines the other modules actually use. 
*/
PUBLIC DWORD GetTimer(void)
{
   DWORD tmp;
   if (vtd != NULL) {
      tmp = GetVTD(VTD_TICKS);
   } else {
      tmp = ReadTimer();
   }
   return tmp;
}

/* Returns the timer value in ms.
** When it reaches 0x100000000/1193 = 3600140 or about one hour,
** 'ReadTimer()/TIMERFREQ' will wrap causing troubles if it is used in timeouts
** etc (where two times are simply subtracted). In theese cases, one should use
** the TimeDiff-function instead which works for time differences less than one
** hour
*/
PUBLIC DWORD GetMsTimer(void)
{
   DWORD tmp;
   if (vtd != NULL) {
      tmp = GetVTD(VTD_MILLISECONDS);
   } else {
      tmp = ReadTimer() / TIMERFREQ;
   }
   return tmp;
}

#endif /*_WINDOWS*/

PUBLIC void TimerInit(void)
{
#ifndef _WINDOWS

   vtd = NULL;

   // Find out whether we are running in a DOS box under Windows.
   __asm   mov   ax, 160Ah
   __asm   int   2fh
   // AX == 0 if this call is supported
   // BX == major*8 + minor version
   // CX == 2 -> Std , 3 -> Enh mode

   __asm   cmp   ax,ax
   __asm   jnz   notWindows
   // We are running under Windows.
   __asm   mov   ax, 1684h         // Get device entry point
   __asm   mov   bx, 5             // 5 = VTD
   __asm   mov   dx, 0
   __asm   mov   es, dx
   __asm   mov   di, dx
   __asm   int   2fh
   __asm   mov   ax, di
   __asm   mov   word ptr [vtd], ax
   __asm   mov   ax, es
   __asm   mov   word ptr [vtd+2], ax

notWindows:
	
   if (vtd == NULL) {
	   // No traces of the VTD can be detected. We use the hardware
	   // timer instead.
	   initializeTimer();
   }

#endif /*ifndef _WINDOWS*/
}


PUBLIC void TimerExit(void)
{
#ifndef _WINDOWS
   if (vtd == NULL)
          restoreTimer();
#endif
}

