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
*               has been indicated as section 9 in Draft 19.4.
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
** TestWithNoFaultsAfter3DriveCycles -
** Function to run test with no faults after 3 drive cycles
*******************************************************************************
*/
STATUS TestWithNoFaultsAfter3DriveCycles(void)
{
	/* Prompt user to complete the second and perform the third drive cycle */
	LogPrint("\n\n**** Test 9.1 (No faults after 3 drive cycles) ****\n");
	LogUserPrompt("Turn key off for at least thirty (30) seconds\n"
	              "(this completes two driving cycles)", ENTER_PROMPT);
	LogUserPrompt("Start engine and let idle for whatever time it takes to run the monitor\n"
	              "and detect that there is no malfunction", ENTER_PROMPT);
	LogUserPrompt("Turn key off for at least thirty (30) seconds\n"
	              "(this completes the three driving cycles with no fault)", ENTER_PROMPT);
	LogUserPrompt("Start engine and let idle for one (1) minute",
	ENTER_PROMPT);
	gOBDEngineRunning = TRUE;

	LogPrint("**** Test 9.1 PASSED ****\n");

	/* Set flag to indicate DTC should be historical */
	gOBDDTCHistorical = TRUE;

	/* Set flag to indicate no permanent DTC */
	gOBDDTCPermanent = FALSE;

	/* Determine the OBD protocol to use */
	LogPrint("\n\n**** Test 9.2 ****\n");
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 9.2 FAILED ****\n" );
		LogPrint( "Protocol determination failed.\n" );
		return(FAIL);
	}
	else
	{
		LogPrint("**** Test 9.2 PASSED ****\n");
	}

	/* Verify pending DTC data */
	LogPrint("\n\n**** Test 9.3 ****\n");
	if (VerifyDTCPendingData() != PASS)
	{
		LogPrint("**** Test 9.3 FAILED ****\n");
		if ( TestContinue( "DTC pending data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 9.3 PASSED ****\n");
	}

	/* Verify stored DTC data */
	LogPrint("\n\n**** Test 9.4 ****\n");
	if (VerifyDTCStoredData() != PASS)
	{
		LogPrint("**** Test 9.4 FAILED ****\n");
		if ( TestContinue( "DTC stored data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}

	/* Verify MIL and DTC status is cleared */
	if (VerifyMILData() != PASS)
	{
		LogPrint("**** Test 9.4 FAILED ****\n");
		if ( TestContinue( "MIL / DTC status failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 9.4 PASSED ****\n");
	}

	/* Verify freeze frame support and data */
	LogPrint("\n\n**** Test 9.5 ****\n");
	if (VerifyFreezeFrameSupportAndData() != PASS)
	{
		LogPrint("**** Test 9.5 FAILED ****\n");
		if ( TestContinue( "Freeze frame support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 9.5 PASSED ****\n");
	}


	/* Link active test to verify communication remained active for ALL protocols
	 */
	if (VerifyLinkActive() != PASS)
	{
		return( FAIL );
	}


	/* Verify permanent codes */
	LogPrint( "\n\n**** Test 9.6 (Request Permanent DTCs, Engine Running) ****\n" );
	if ( VerifyPermanentCodeSupport() != PASS )
	{
		LogPrint( "**** Test 9.6 FAILED ****\n" );
		if ( TestContinue( "Permanent code support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 9.6 PASSED ****\n" );
	}


	/* Verify Permanent Codes Drive Cycle */
	LogPrint( "\n\n**** Test 9.7 ****\n" );

	if ( gService0ASupported )
	{
		if ( LogUserPrompt( "Service $0A is supported by at least one ECU, do you wish\n"
		                    "to run the CARB drive cycle portion for Permanent DTCs?", YES_NO_PROMPT) == 'Y' )
		{
			LogPrint( "**** Test 9.7 PASSED ****\n" );

			TestToVerifyPermanentCodes();

			/* Verify permanent codes */
			LogPrint( "\n\n**** Test 9.22 (Request Permanent DTCs, Engine Off) ****\n" );
			if ( VerifyPermanentCodeSupport() != PASS )
			{
				LogPrint( "**** Test 9.22 FAILED ****\n" );
				if ( TestContinue( "Permanent code support failed. Continue?" ) == 'N' )
				{
					return(FAIL);
				}
			}
			else
			{
				LogPrint( "**** Test 9.22 PASSED ****\n" );
			}
		}

		else
		{
			LogPrint( "\nWARNING: Service $0A was supported but the operator chose not to run\n"
			          "the Permanent Code Drive Cycle test.\n\n" );

			LogPrint( "**** Test 9.7 PASSED ****\n" );
		}
	}
	/* Service $0A not supported */
	else
	{
		LogPrint( "\nWARNING: Service $0A was not supported therefore\n"
		          "the Permanent Code Drive Cycle was not tested.\n\n" );

		LogPrint( "**** Test 9.7 PASSED ****\n" );
	}


	/* Clear code in engine off status*/
	if ( LogUserPrompt( "You may now clear codes or exit Test 9.\n"
	                    "If you choose to clear all diagnostic information,\n"
	                    "you will then have the option of clearing codes with engine running.\n\n"
	                    "Clear all diagnostic information?", YES_NO_PROMPT) == 'Y' )
	{
		LogPrint( "\n\n**** Test 9.23 (Clear Codes) ****\n" );

		if ( LogUserPrompt( "Clear codes engine running?", YES_NO_PROMPT) != 'Y' )
		{
			/* Clear Codes Engine OFF */
			LogUserPrompt( "Turn key OFF for at least 30 seconds.\n", ENTER_PROMPT );
			LogUserPrompt( "Turn key ON with engine OFF. Do not crank engine.\n", ENTER_PROMPT );

			if ( DetermineProtocol() != PASS )
			{
				LogPrint( "**** Test 9.23 FAILED ****\n" );
				LogPrint( "Protocol determination failed\n" );
				LogUserPrompt( "Turn key OFF", ENTER_PROMPT );
				return(FAIL);
			}
			else
			{
				LogPrint( "\nINFORMATION: Protocol determination passed.\n" );
			}
		}

		if ( ClearCodes() != PASS )
		{
			LogPrint( "\n**** Test 9.23 FAILED ****\n" );
			if ( TestContinue( "Clear diagnostic information failed. Continue?" ) == 'N' )
			{
				LogUserPrompt( "Turn key OFF", ENTER_PROMPT );
				return(FAIL);
			}
		}
		else
		{
			LogPrint( "\nINFORMATION: Cleared diagnostics information successfully.\n" );
			LogPrint( "**** Test 9.23 PASSED ****\n" );
		}
	}

	/* Don't clear Codes, EXIT Test 9 */
	else
	{
		LogUserPrompt( "Turn key OFF", ENTER_PROMPT );

		LogPrint( "**** Test 9.22 PASSED ****\n" );
	}


	return(PASS);
}
