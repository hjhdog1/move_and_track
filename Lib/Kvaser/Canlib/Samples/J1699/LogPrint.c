/*
********************************************************************************
** SAE J1699-3 Test Source Code
**
**  Copyright (C) 2002 Drew Technologies. http://j1699-3.sourceforge.net/
**
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** This source code, when compiled and used with an SAE J2534-compatible pass
** thru device, is intended to run the tests described in the SAE J1699-3
** document in an automated manner.
**
** This computer program is based upon SAE Technical Report J1699,
** which is provided "AS IS"
**
** See j1699.c for details of how to build and run this test.
**
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

/*
***************************************************************************************************
** LogPrint - Function to send information to both the console and the log file
***************************************************************************************************
*/
void LogPrint(const char *LogString, ...)
{
    char LogBuffer[1024];
    char PrintBuffer[1024];
    unsigned long PrintIndex;

    va_list Args;
    va_start (Args, LogString);

    if (gSuspendScreenOutput == FALSE)
    {
        vfprintf(stdout, LogString, Args);
    }

    vsprintf (PrintBuffer, LogString, Args);
    va_end (Args);

    /* Get rid of any control characters at the beginning of the string before logging */
    for (PrintIndex = 0; 
         (PrintIndex < sizeof(PrintBuffer)) && 
           (PrintBuffer[PrintIndex] < ' ')  && 
           (PrintBuffer[PrintIndex] != '\0'); 
         PrintIndex++) { }

    sprintf(LogBuffer, "+%06ldms %s",(GetTickCount() - gLastLogTime), &PrintBuffer[PrintIndex]);

    gLastLogTime = GetTickCount();
    fputs(LogBuffer, ghLogFile);
    fflush(ghLogFile);
}

void ScreenPrint (const char * fmt, ...)
{
    if (gSuspendScreenOutput == FALSE)
    {
        va_list Args;
        va_start (Args, fmt);

        vfprintf (stdout, fmt, Args);
 
        va_end (Args);
   }
}
