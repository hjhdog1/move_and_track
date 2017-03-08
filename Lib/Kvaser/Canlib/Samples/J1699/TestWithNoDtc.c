
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
** DATE          MODIFICATION
** 05/01/04      Renumber all test cases to reflect specification.  This section
**               has been indicated as section 5 in Draft 15.4.
** 05/10/04      Test case 5.19, replaced existing link active logic with
**               routine call to 'VerifyLinkActive'.  Function will verity
**               response from total number of controllers.
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
** TestWithNoDtc -
** Function to run test with no malfunction, no DTC set
*******************************************************************************
*/
STATUS TestWithNoDtc(void)
{
	/*
	** Do MIL light check
	*/
	LogPrint("\n\n**** Test 5.1 (No DTC set) ****\n");
	if (CheckMILLight() != PASS)
	{
		LogPrint("**** Test 5.1 FAILED ****\n");
		if ( TestContinue( "MIL light check failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.1 PASSED ****\n");
	}

	/*
	** Determine the OBD protocol to use
	*/
	LogPrint("\n\n**** Test 5.2 ****\n");
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 5.2 FAILED ****\n" );
		LogPrint( "Protocol determination failed.\n" );
		return(FAIL);
	}
	else
	{
		LogPrint("**** Test 5.2 PASSED ****\n");
	}

	/* Clear codes */
	LogPrint("\n\n**** Test 5.3 ****\n");
	if (ClearCodes() != PASS)
	{
		LogPrint("**** Test 5.3 FAILED ****\n");
		if ( TestContinue( "Clear diagnostic information failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.3 PASSED ****\n");
	}

	/* Verify MIL and DTC status is cleared */
	LogPrint("\n\n**** Test 5.4 ****\n");
	if (VerifyMILData() != PASS)
	{
		LogPrint("**** Test 5.4 FAILED ****\n");
		if ( TestContinue( "MIL / DTC status failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.4 PASSED ****\n");
	}

	/* Verify monitor test support and results */
	LogPrint("\n\n**** Test 5.5 ****\n");
	if (VerifyMonitorTestSupportAndResults() != PASS)
	{
		LogPrint("**** Test 5.5 FAILED ****\n");
		if ( TestContinue( "Monitor test support/results failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.5 PASSED ****\n");
	}

	/* Verify diagnostic support and data */
	LogPrint("\n\n**** Test 5.6 ****\n");
	if (VerifyDiagnosticSupportAndData() != PASS)
	{
		LogPrint("**** Test 5.6 FAILED ****\n");
		if ( TestContinue( "Diagnostic support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.6 PASSED ****\n");
	}

	/* Verify control support and data */
	LogPrint("\n\n**** Test 5.7 ****\n");
	if (VerifyControlSupportAndData() != PASS)
	{
		LogPrint("**** Test 5.7 FAILED ****\n");
		if ( TestContinue( "Control system/test/component data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.7 PASSED ****\n");
	}

	/* Tell user to start engine */
	LogPrint("\n\n**** Test 5.8 ****\n");
	LogUserPrompt("Start engine and let idle for one (1) minute.\n"
	              "USER TIP: Set A/C to defrost on hybrid vehicles to keep engine running\n", ENTER_PROMPT);
	gOBDEngineRunning = TRUE;

	/* Determine the OBD protocol to use */
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 5.8 FAILED ****\n" );
		LogPrint( "Protocol determination failed.\n" );
		return(FAIL);
	}
	else
	{
		LogPrint("**** Test 5.8 PASSED ****\n");
	}

	/* Clear codes */
	LogPrint("\n\n**** Test 5.9 ****\n");
	if (ClearCodes() != PASS)
	{
		LogPrint("**** Test 5.9 FAILED ****\n");
		if ( TestContinue( "Clear diagnostic information failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.9 PASSED ****\n");
	}

	/* Verify diagnostic support and data */
	LogPrint("\n\n**** Test 5.10 ****\n");
	if (VerifyDiagnosticSupportAndData() != PASS)
	{
		LogPrint("**** Test 5.10 FAILED ****\n");
		if ( TestContinue( "Diagnostic support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.10 PASSED ****\n");
	}

	/* Verify freeze frame support and data */
	LogPrint("\n\n**** Test 5.11 ****\n");
	TestSubsection = 11;
	if (VerifyFreezeFrameSupportAndData() != PASS)
	{
		LogPrint("**** Test 5.11 FAILED ****\n");
		if ( TestContinue( "Freeze frame support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.11 PASSED ****\n");
	}

	/* Verify stored DTC data */
	LogPrint("\n\n**** Test 5.12 ****\n");
	if (VerifyDTCStoredData() != PASS)
	{
		LogPrint("**** Test 5.12 FAILED ****\n");
		if ( TestContinue( "DTC stored data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else if (VerifyMILData() != PASS)
	{
		LogPrint("**** Test 5.12 FAILED ****\n");
		if ( TestContinue( "MIL / DTC status failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.12 PASSED ****\n");
	}

	/* Verify O2 monitor test results */
	LogPrint("\n\n**** Test 5.13 ****\n");
	if (VerifyO2TestResults() != PASS)
	{
		LogPrint("**** Test 5.13 FAILED ****\n");
		if ( TestContinue( "O2 test results failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.13 PASSED ****\n");
	}

	/* Verify monitor test support and results */
	LogPrint("\n\n**** Test 5.14 ****\n");
	TestSubsection = 14;
	if (VerifyMonitorTestSupportAndResults() != PASS)
	{
		LogPrint("**** Test 5.14 FAILED ****\n");
		if ( TestContinue( "Monitor test support/results failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.14 PASSED ****\n");
	}

	/* Verify pending DTC data */
	LogPrint("\n\n**** Test 5.15 ****\n");
	if (VerifyDTCPendingData() != PASS)
	{
		LogPrint("**** Test 5.15 FAILED ****\n");
		if ( TestContinue( "DTC pending data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.15 PASSED ****\n");
	}

	/* Verify control support and data */
	LogPrint("\n\n**** Test 5.16 ****\n");
	if (VerifyControlSupportAndData() != PASS)
	{
		LogPrint("**** Test 5.16 FAILED ****\n");
		if ( TestContinue( "Control system/test/component data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.16 PASSED ****\n");
	}

	/* Verify vehicle information support and data */
	LogPrint("\n\n**** Test 5.17 ****\n");
	if (VerifyVehicleInformationSupportAndData() != PASS)
	{
		LogPrint("**** Test 5.17 FAILED ****\n");
		if ( TestContinue( "Vehicle information data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.17 PASSED ****\n");
	}

	/* Verify reverse order support */
	LogPrint("\n\n**** Test 5.18 ****\n");
	if (VerifyReverseOrderSupport() != PASS)
	{
		LogPrint("**** Test 5.18 FAILED ****\n");
		if ( TestContinue( "SID1 reverse order support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 5.18 PASSED ****\n");
	}

	/* Verify tester present support */
	LogPrint("\n\n**** Test 5.19 ****\n");

	/* Sleep for 15 seconds, then make sure the tester present messages kept the link alive */
	Sleep(15000);


	/*
	**Link active test to verify communication remained active for ALL protocols
	*/
	 if (VerifyLinkActive() != PASS)
	 {
		LogPrint("**** Test 5.19 FAILED ****\n");
		if ( TestContinue( "Tester present support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	 }
	else
	{
		LogPrint("**** Test 5.19 PASSED ****\n");
	}


	/* Verify diagnostic burst support */
	LogPrint( "\n\n**** Test 5.20 ****\n" );

	if ( VerifyDiagnosticBurstSupport() != PASS )
	{
		LogPrint( "**** Test 5.20 FAILED ****\n" );
		if ( TestContinue( "SID1 burst support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 5.20 PASSED ****\n" );
	}


	/* Verify Permanent Code Support */
	LogPrint( "\n\n**** Test 5.21 (Request Permanent DTCs, Engine Running) ****\n" );

	if ( VerifyPermanentCodeSupport() != PASS )
	{
		LogPrint( "**** Test 5.21 FAILED ****\n" );
		if ( TestContinue( "Permanent Code support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 5.21 PASSED ****\n" );
	}


	/* Verify Reserved Services */
	LogPrint( "\n\n**** Test 5.22 (Request Reserved Services) ****\n" );

	if ( VerifyReservedServices() != PASS )
	{
		LogPrint( "**** Test 5.22 FAILED ****\n" );
		if ( TestContinue( "Reserved Services test failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 5.22 PASSED ****\n" );
	}

	return(PASS);
}
