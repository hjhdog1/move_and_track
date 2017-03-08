/*
********************************************************************************
** SAE J1699-3 Test Source Code
**
**  Copyright (C) 2002 Drew Technologies. http://j1699-7.sourceforge.net/
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
* DATE          MODIFICATION
* 05/01/04      Renumber all test cases to reflect specification.  This section
*               has been indicated as section 7 in Draft 15.4.
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "j2534.h"
#include "j1699.h"

/*
*******************************************************************************
** TestWithConfirmedDtc -
** Function to run test with confirmed DTC by retaining the fault
*******************************************************************************
*/
STATUS TestWithConfirmedDtc(void)
{
	/* Prompt user to keep fault and start engine */
	LogPrint("\n\n**** Test 7.1 (Confirmed DTC) ****\n");
	LogUserPrompt("Turn key off for at least thirty (30) seconds and\n"
	              "keep sensor disconnected", ENTER_PROMPT);
	LogUserPrompt("Start engine and let idle for 1 minute or whatever time it takes to set\n"
	              "a confirmed DTC and illuminate the MIL\n" , ENTER_PROMPT);
	LogPrint("**** Test 7.1 PASSED ****\n");

	gOBDEngineRunning = TRUE;

	LogPrint("\n\n**** Test 7.2 ****\n");

	/* Determine the OBD protocol to use */
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 7.2 FAILED ****\n" );
		LogPrint( "Protocol determination failed.\n" );
		return(FAIL);
	}
	else
	{
		LogPrint("**** Test 7.2 PASSED ****\n");
	}

	/* Check for a pending DTC */
	LogPrint("\n\n**** Test 7.3 ****\n");

	/* flush the STDIN stream of any user input before loop */
	clear_keyboard_buffer ();

	LogPrint("INFORMATION: Checking for a pending DTC...(press any key to stop test)\n");
	while (!_kbhit())
	{
		if (IsDTCPending() == TRUE)
		{
			break;
		}
		Sleep(500);
	}

	/* Beep */
	printf("\007\n");

	/* Flush the STDIN stream of any user input above */
	clear_keyboard_buffer ();

	/* Set flag to indicate a stored DTC should be present */
	gOBDDTCStored = TRUE;

	/* Verify pending DTC data */
	if (VerifyDTCPendingData() != PASS)
	{
		LogPrint("**** Test 7.3 FAILED ****\n");
		if ( TestContinue( "DTC pending data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 7.3 PASSED ****\n");
	}

	LogPrint("\n\n**** Test 7.4 ****\n");

	/* Verify stored DTC data */
	if (VerifyDTCStoredData() != PASS)
	{
		LogPrint("**** Test 7.4 FAILED ****\n");
		if ( TestContinue( "DTC stored data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}

	/* Get DTC list from SID $03 */
	SaveDTCList (3);

	/* Check the MIL light */
	if (LogUserPrompt("Is MIL light ON?", YES_NO_PROMPT) != 'Y')
	{
		LogPrint("**** Test 7.4 FAILED ****\n");
		if ( TestContinue( "MIL light check failed (OFF with stored DTC). Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}

	/* Verify MIL and DTC status is cleared */
	if (VerifyMILData() != PASS)
	{
		LogPrint("**** Test 7.4 FAILED ****\n");
		if ( TestContinue( "MIL / DTC status failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 7.4 PASSED ****\n");
	}

	/* Verify freeze frame support and data */
	LogPrint("\n\n**** Test 7.5 ****\n");
	if (VerifyFreezeFrameSupportAndData() != PASS)
	{
		LogPrint("**** Test 7.5 FAILED ****\n");
		if ( TestContinue( "Freeze frame support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 7.5 PASSED ****\n");
	}

	/* Link active test to verify communication remained active for ALL protocols */
	if (VerifyLinkActive() != PASS)
	{
		return( FAIL );
	}

	return(PASS);
}
