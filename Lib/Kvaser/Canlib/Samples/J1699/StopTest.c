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
#include <conio.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

/*
*******************************************************************************
** StopTest - Function to stop all tests and exit
*******************************************************************************
*/
void StopTest (STATUS ExitCode)
{
    /* Print out the request / response statistics */
    if (gOBDAggregateResponses != 0)
    {
        LogPrint("INFORMATION: Average response time = %ldmsec (%ld requests)\n", 
        (gOBDAggregateResponseTimeMsecs / gOBDAggregateResponses), gOBDAggregateResponses);
    }

    if (ExitCode == FAIL)
    {
        LogPrint("\n\n**** Failures detected, view %s file ****\n", gLogFileName);
    }
    else if (ExitCode == ABORT)
    {
        LogPrint("\n\n**** Test terminated by user, view %s file ****\n", gLogFileName);
    }

    DisconnectProtocol();
    fclose(ghLogFile);

    PassThruClose (DeviceID);

    fclose (ghTempLogFile);
    DeleteFile (gszTempLogFilename);

    printf ("INFORMATION: J1699-3 application is terminating. Press any key to continue");
    _getch ();

    exit((int)ExitCode);
}
