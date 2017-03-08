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
*******************************************************************************
** Log2534Error - Function to lookup J2534 function error descriptions 
**                and send to console and log file.
*******************************************************************************
*/
void Log2534Error(char *LogString, unsigned long RetVal, char *FunctionName)
{
    char PrintBuffer[240];
    char ErrorBuffer[240];
    char LogBuffer[1024];

    if (FunctionName != NULL && RetVal != STATUS_NOERROR)
    {
        PassThruGetLastError(ErrorBuffer);
        sprintf(PrintBuffer, "FAILURE: %s %s (%s returned %ld)\n",
        LogString, ErrorBuffer, FunctionName, RetVal);
    }
    else
    {
        sprintf(PrintBuffer, "INFORMATION: %s\n", LogString);
    }

    sprintf(LogBuffer, PrintBuffer);
    fputs(LogBuffer, ghLogFile);
    fflush(ghLogFile);
    printf("%s", PrintBuffer);
}

