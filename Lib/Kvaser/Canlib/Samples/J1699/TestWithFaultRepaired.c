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
* DATE          MODIFICATION
* 05/01/04      Renumber all test cases to reflect specification.  This section
*               has been indicated as section 8 in Draft 15.8.
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
** TestWithFaultRepaired -
** Function to run test with fault repaired
*******************************************************************************
*/
STATUS TestWithFaultRepaired(void)
{
	/* Prompt user to fix fault and perform first two drive cycles */
	LogPrint("\n\n**** Test 8.1 (Fault repaired) ****\n");
	LogUserPrompt("Turn key off for at least thirty (30) seconds and\n"
	              "reconnect sensor", ENTER_PROMPT);
	LogUserPrompt("Start engine and let idle for whatever time it takes to run the monitor\n"
	              "and detect that there is no malfunction", ENTER_PROMPT);
	LogUserPrompt("Turn key off for at least thirty (30) seconds\n"
	              "(this completes one driving cycle)", ENTER_PROMPT);
	LogUserPrompt("Start engine and let idle for whatever time it takes to run the monitor\n"
	              "and detect that there is no malfunction", ENTER_PROMPT);

	gOBDEngineRunning = TRUE;

	LogPrint("**** Test 8.1 PASSED ****\n");

	LogPrint("\n\n**** Test 8.2 ****\n");
	/* Determine the OBD protocol to use */
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 8.2 FAILED ****\n" );
		LogPrint( "Protocol determination failed.\n" );
		return(FAIL);
	}
	else
	{
		LogPrint("**** Test 8.2 PASSED ****\n");
	}

	/* Check for a pending DTC */
	LogPrint("\n\n**** Test 8.3 ****\n");

	/* flush the STDIN stream of any user input before loop */
	clear_keyboard_buffer ();

	LogPrint("INFORMATION: Waiting for pending DTC to clear...(press any key to stop test)\n");
	while (!_kbhit())
	{
		if (IsDTCPending() == FALSE)
		{
			break;
		}
		Sleep(500);
	}

	/* Beep */
	printf("\007\n");

	/* Flush the STDIN stream of any user input above */
	clear_keyboard_buffer ();

	/* Set flag to indicate a pending DTC should NOT be present */
	gOBDDTCPending = FALSE;

	/* Verify pending DTC data */
	if (VerifyDTCPendingData() != PASS)
	{
		LogPrint("**** Test 8.3 FAILED ****\n");
		if ( TestContinue( "DTC pending data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 8.3 PASSED ****\n");
	}

	/* Verify stored DTC data */
	LogPrint("\n\n**** Test 8.4 ****\n");
	if (VerifyDTCStoredData() != PASS)
	{
		LogPrint("**** Test 8.4 FAILED ****\n");
		if ( TestContinue( "DTC stored data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}

	/* Verify MIL and DTC status is cleared */
	if (LogUserPrompt("Is MIL light ON?", YES_NO_PROMPT) != 'Y')
	{
		LogPrint("**** Test 8.4 FAILED ****\n");
		if ( TestContinue( "MIL light check failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}

	if (VerifyMILData() != PASS)
	{
		LogPrint("**** Test 8.4 FAILED ****\n");
		if ( TestContinue( "MIL / DTC status failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 8.4 PASSED ****\n");
	}

	/* Verify freeze frame support and data */
	LogPrint("\n\n**** Test 8.5 ****\n");
	if (VerifyFreezeFrameSupportAndData() != PASS)
	{
		LogPrint("**** Test 8.5 FAILED ****\n");
		if ( TestContinue( "Freeze frame support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 8.5 PASSED ****\n");
	}


	/* Link active test to verify communication remained active for ALL protocols
	 */
	if ( VerifyLinkActive() != PASS )
	{
		return( FAIL );
	}


	gOBDDTCPermanent = TRUE;
	

	/* Verify permanent codes */
	LogPrint( "\n\n**** Test 8.6 ****\n" );
	if ( VerifyPermanentCodeSupport() != PASS )
	{
		LogPrint( "**** Test 8.6 FAILED ****\n" );
		if ( TestContinue( "Permanent code support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 8.6 PASSED ****\n" );
	}

	return(PASS);
}
