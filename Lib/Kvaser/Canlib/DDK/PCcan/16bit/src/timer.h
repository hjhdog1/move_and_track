/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

extern DWORD GetTimer(void);
extern DWORD GetMsTimer(void);

#define GETTIMER() GetMsTimer()

#ifdef _WINDOWS
  #define TIMERFREQ 1    // Unit: kHz
#else
  #define TIMERFREQ 1193 // Unit: kHz
#endif

extern void TimerInit(void);
extern void TimerExit(void);

#ifdef __cplusplus
}
#endif

#endif
