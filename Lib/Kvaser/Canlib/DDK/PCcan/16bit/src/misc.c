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

#include <std.h>
#include <canlib.h>
#include "global.h"
// #include <util.h>
#include "misc.h"


PUBLIC char FAR IniFile[] = INI_FILE;

/*
*****************************************************************************
** Ini-file handling routines.
** Only for DOS.
*****************************************************************************
*/

#ifndef _WINDOWS

#include <ctype.h>
#include <string.h>
#include <stdio.h>

PUBLIC UINT GetConfigFileInt(LPCSTR lpszSection,
                          LPCSTR lpszEntry,
                          int dflt,
                          LPCSTR lpszFilename)
{
   int rslt, i, state, error;
   char buf[30];

   GetConfigFileString(lpszSection, lpszEntry, "", buf, sizeof(buf), lpszFilename);
   rslt = 0;
   state = 0;
   error = FALSE;
   
   for (i=0; i<sizeof(buf); i++) {
      char c = buf[i];
      if (c == 0) break;
      if (state == 0) {
         if (c >= '0' && c <= '9') {
            rslt = rslt * 10 + c - '0';
         } else if ((rslt == 0) && (c == 'x' || c == 'X')) {
            state = 1;
         } else {
            error = TRUE;
            break;
         }
      } else {
         if (islower(c)) c -= 32;
         if (c >= '0' && c <= '9') {
            rslt = rslt * 16 + c - '0';
         } else if (c >= 'A' && c <= 'F') {
            rslt = rslt * 16 + c - 'A' + 10;
         } else {
            error = TRUE;
            break;
         }
      }

   }
   if (error) return dflt; else return rslt;

}

PRIVATE void SkipWhite(char **buf)
{
   char* p = *buf;
   while (*p && isspace(*p)) p++;
   *buf = p;
}

#define TOUPPER(c) ((c) >= 'a'?(c)-32:(c))

PRIVATE int MatchChar(char** buf, char c)
{
   char *p = *buf;
   SkipWhite(&p);
   if (TOUPPER(*p) == TOUPPER(c)) {
      *buf = ++p;
      return TRUE;
   } else return FALSE;
}


PRIVATE int MatchString(char** buf, char _far * s)
{
   char c1, c2, *p;
   p = *buf;
   SkipWhite(&p);

   while (c1 = *p, c2 = *s++, c1 && c2) {
      if (TOUPPER(c1) != TOUPPER(c2)) {
         return FALSE;
      }
      p++;
   }
   *buf = p;
   return TRUE;
}

PRIVATE char PathToInifile[_MAX_PATH];

PUBLIC int GetConfigFileString(LPCSTR lpszSection,
                               LPCSTR lpszEntry,
                               LPCSTR lpszDefault,
                               LPCSTR lpszReturnBuffer,
                               int cbReturnBuffer,
                               LPCSTR lpszFilename)
{
   int Failed = FALSE;
   FILE* f;

   // Try to find the ini file somewhere in the path.
   // The path cannot be saved as we may be called with different
   // filenames. Hopefully the cache will save us from the performance hit.
   _searchenv(lpszFilename, "PATH", PathToInifile);
   if (strlen(PathToInifile) == 0) {
      _fstrcpy(PathToInifile, lpszFilename);  // A last resort.		  
   }

   // Open the file..
   if (!Failed) {
      f = fopen(PathToInifile, "r");
      if (!f)
        Failed = TRUE;
      else {
         // locate the right section...
         char buf[128], *p;
         Failed = TRUE;
         while ((fgets(buf, sizeof(buf), f) != NULL) && Failed) {
            p = buf;
            if (MatchChar(&p, '[') &&
                MatchString(&p, lpszSection) &&
                MatchChar(&p, ']')) {
               // Correct section found.
               // locate the right entry...

               while ((fgets(buf, sizeof(buf), f) != NULL) && Failed) {
                  p = buf;
                  if (MatchString(&p, lpszEntry)
                  && MatchChar(&p, '=')) {
                     int len;
                     SkipWhite(&p);
                     len = strlen(p);
                     if (len > 0 && p[len-1] == '\n') p[len-1] = 0;
                     _fstrncpy(lpszReturnBuffer, p, cbReturnBuffer);
                     Failed = FALSE;
                     break;
                  } else if (MatchChar(&p, '[')) {
                     // ...but stop before the next section or EOF.
                     break;
                  }
               }
            }
            if (!Failed) break;
         }
         fclose(f);
      }
   }

   if (Failed) {
      // if nothing was found, return the default.
      _fstrncpy(lpszReturnBuffer, lpszDefault, cbReturnBuffer);
   }
   if (cbReturnBuffer > 0) lpszReturnBuffer[cbReturnBuffer-1] = '\0';
   return _fstrlen(lpszReturnBuffer);
}


#endif /*ifndef _WINDOWS*/

PUBLIC void MiscInit(void)
{
#ifndef _WINDOWS
	PathToInifile[0] = '\0';
#endif
}

PUBLIC void MiscExit(void)
{
}

