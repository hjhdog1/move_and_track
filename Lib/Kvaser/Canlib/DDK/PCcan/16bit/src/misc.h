/*
**                     Copyright 1995-1998 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _MISC_H_
#define _MISC_H_

#define INI_FILE "KVASER.INI"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDOWS
  #define GetConfigFileInt(a,b,c,d) GetPrivateProfileInt(a,b,c,d)
  #define GetConfigFileString(a,b,c,d,e,f) GetPrivateProfileString(a,b,c,d,e,f)
#else
  #define TIMERFREQ 1193 // Unit: kHz

  typedef char far *LPCSTR;
  typedef unsigned int UINT;

  PUBLIC UINT GetConfigFileInt(LPCSTR lpszSection,
							   LPCSTR lpszEntry,
							   int dflt,
							   LPCSTR lpszFilename);

  PUBLIC int GetConfigFileString(LPCSTR lpszSection,
								 LPCSTR lpszEntry,
								 LPCSTR lpszDefault,
								 LPCSTR lpszReturnBuffer,
								 int cbReturnBuffer,
								 LPCSTR lpszFilename);
#endif

PUBLIC void MiscInit(void);
PUBLIC void MiscExit(void);

#ifdef __cplusplus
}
#endif

#endif
