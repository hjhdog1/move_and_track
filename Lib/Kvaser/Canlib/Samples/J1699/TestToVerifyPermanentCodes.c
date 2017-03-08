/*
********************************************************************************
** SAE J1699-3 Test Source Code
**
**  Copyright (C) 2007. http://j1699-3.sourceforge.net/
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

/* These Test 10.9 functions and variable are reused
   here in Test 9.19 */
extern SID9IPT Test10_9_Sid9Ipt[OBD_MAX_ECUS];
extern STATUS VerifyIPTData (void);
extern STATUS RunDynamicTest10 (unsigned long tEngineStartTimeStamp);

/*
*******************************************************************************
** TestToVerifyPermanentCodes -
** Function to run test to read permanent codes
*******************************************************************************
*/
STATUS TestToVerifyPermanentCodes(void)
{
	unsigned long tEngineStartTimeStamp;
	STATUS        ret_code;
	unsigned long error_count;

	/* Prompt user to induce fault and set pending DTC */
	LogPrint( "\n\n**** Test 9.8 (Induce circuit fault to set pending DTC) ****\n" );

	LogUserPrompt( "With engine and ignition off, disconnect a sensor that is tested\n"
	               "continuously (e.g., ECT, TP, IAT, MAF, etc.) to set a fault\n"
	               "that will generate a MIL light and a single DTC on only one ECU\n"
	               "with the engine idling in a short period of time (i.e. < 10 seconds).\n\n"
	               "The selected fault must illuminate the MIL using two driving cycles,\n"
	               "not one driving cycle (like GM \"Type A\" DTC) to allow proper testing\n"
	               "of Service 07 and freeze frame.\n\n"
	               "NOTE: If a DTC that sets in two driving cycles cannot be tested,\n"
	               "it is acceptable to use a fault that sets in one driving cycle.\n"
	               "If this is the case, a pending DTC, a confirmed DTC, and MIL\n"
	               "will be set on the first driving cycle.\n", ENTER_PROMPT );

	LogUserPrompt( "Start engine, let idle for one minute or whatever time it takes to set\n"
	               "a pending DTC.\n\n"
	               "NOTE: Some powertrain control systems have engine controls that can start and\n"
	               "stop the engine without regard to ignition position. The operator must ensure\n"
	               "that the engine is on when performing the test (e.g. turn on A/C or defroster).\n", ENTER_PROMPT );

	LogUserPrompt( "Turn ignition off (engine off) for 30 seconds. Keep sensor disconnected.\n", ENTER_PROMPT);

	LogUserPrompt( "Start engine, let idle for one minute or whatever time it takes to set\n"
	               "a confirmed DTC and illuminate the MIL.\n\n"
	               "NOTE: Some powertrain control systems have engine controls that can start and\n"
	               "stop the engine without regard to ignition position. The operator must ensure\n"
	               "that the engine is on when performing the test (e.g. turn on A/C or defroster).\n", ENTER_PROMPT );

	LogUserPrompt( "Turn ignition off (engine off) for 30 seconds. Keep sensor disconnected\n"
	               "(This completes the drving cycle and allows permanent code to be set).\n", ENTER_PROMPT );

	LogUserPrompt( "Turn ignition on. DO NOT crank engine.\n", ENTER_PROMPT );

	LogPrint( "**** Test 9.8 PASSED ****\n" );


	/* Establish communication, engine off */
	LogPrint( "\n\n**** Test 9.9 (Establish communications, Engine Off) ****\n" );
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 9.9 FAILED ****\n" );
		LogPrint( "Protocol determination failed." );
		return(FAIL);
	}
	else
	{
		LogPrint( "**** Test 9.9 PASSED ****\n" );
	}


	gOBDDTCPermanent = TRUE;


	/* Request DTC, engine off */
	LogPrint( "\n\n**** Test 9.10 (Request DTCs, Engine Off) ****\n" );
	if ( VerifyDTCStoredData() != PASS )
	{
		LogPrint( "**** Test 9.10 FAILED ****\n" );
		if ( TestContinue( "DTC stored data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 9.10 PASSED ****\n" );
	}


	/* Verify stored DTC data, engine off */
	LogPrint( "\n\n**** Test 9.11 (Request permanent DTCs, Engine Off) ****\n" );
	if ( VerifyPermanentCodeSupport() != PASS )
	{
		LogPrint( "**** Test 9.11 FAILED ****\n" );
		if ( TestContinue( "Permanent Code support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 9.11 PASSED ****\n" );
	}


	/* Prompt user to induce fault to be able to retrieve permanent DTC */
	LogPrint( "\n\n**** Test 9.12 (Repair Circuit Fault) ****\n" );

	LogUserPrompt( "Turn ignition off (engine off) for 30 seconds.\n", ENTER_PROMPT );

	LogUserPrompt( "Connect sensor.\n", ENTER_PROMPT );

	LogUserPrompt( "Turn ignition on.  DO NOT crank engine.\n", ENTER_PROMPT );

	LogPrint( "**** Test 9.12 PASSED ****\n" );


	/* Establish communication, engine off */
	LogPrint( "\n\n**** Test 9.13 (Establish communications and clear codes, Engine Off)****\n" );
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 9.13 FAILED ****\n" );
		LogPrint( "Protocol determination failed." );
		return(FAIL);
	}
	else
	{
		if ( ClearCodes() != PASS )
		{
			LogPrint( "**** Test 9.13 FAILED ****\n" );
			if ( TestContinue( "Clear diagnostic information failed. Continue?" ) == 'N' )
			{
				return(FAIL);
			}
		}

		LogPrint( "**** Test 9.13 PASSED ****\n" );
	}


	/* Verify MIL status, engine off */
	LogPrint( "\n\n**** Test 9.14 (Verify MIL status, Engine Off) ****\n" );
	if ( VerifyMILData() != PASS )
	{
		LogPrint( "**** Test 9.14 FAILED ****\n" );
		if ( TestContinue( "MIL / DTC status failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 9.14 PASSED ****\n" );
	}


	/* Verify stored DTC data, engine off */
	LogPrint( "\n\n**** Test 9.15 (Request permanent DTCs, Engine Off) ****\n" );
	if ( VerifyPermanentCodeSupport() != PASS )
	{
		LogPrint( "**** Test 9.15 FAILED ****\n" );
		if ( TestContinue( "Permanent Code support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 9.15 PASSED ****\n" );
	}


	/* Prompt user to repair circuit fault and complete one drive cycle */
	LogPrint( "\n\n**** Test 9.16 (Complete One Driving Cycle With Fault Repaired) ****\n" );

	LogUserPrompt( "Start engine, let idle for one minute or whatever time it takes\n"
	               "to run monitor and detect that there is no malfunction.\n"
	               "(Monitor may have already run with engine off.)\n\n"
	               "NOTE: Some powertrain control systems have engine controls that can start and\n"
	               "stop the engine without regard to ignition position. The operator must ensure\n"
	               "that the engine is on when performing the test (e.g. turn on A/C or defroster).\n", ENTER_PROMPT );

	LogUserPrompt( "Turn ignition off (engine off) for 30 seconds\n"
	               "(This completes one driving cycle with no fault).\n", ENTER_PROMPT);

	LogUserPrompt( "Start engine, let idle for one minute or whatever time it takes\n"
	               "to detect that there is no malfunction.\n"
	               "(This starts the second driving cycle; however, second driving cycle\n"
	               "will not be complete until key is turned off.)\n\n"
	               "NOTE: Some powertrain control systems have engine controls that can start and\n"
	               "stop the engine without regard to ignition position. The operator must ensure\n"
	               "that the engine is on when performing the test (e.g. turn on A/C or defroster).\n", ENTER_PROMPT );

	LogPrint( "**** Test 9.16 PASSED ****\n" );


	tEngineStartTimeStamp = GetTickCount ();


	/* Establish communication, engine off */
	LogPrint( "\n\n**** Test 9.17 (Establish communications, Engine Running) ****\n" );
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 9.17 FAILED ****\n" );
		LogPrint( "Protocol determination failed." );
		return(FAIL);
	}
	else
	{
		LogPrint( "**** Test 9.17 PASSED ****\n" );
	}


	gOBDDTCPermanent = TRUE;


	/* Verify stored DTC data, engine off */
	LogPrint( "\n\n**** Test 9.18 (Request Permanent DTCs, Engine Running) ****\n" );
	if ( VerifyPermanentCodeSupport() != PASS )
	{
		LogPrint( "**** Test 9.18 FAILED ****\n" );
		if ( TestContinue( "Permanent Code support failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint( "**** Test 9.18 PASSED ****\n" );
	}


	/* User prompt, run test */
	LogPrint("\n\n**** Test 9.19 (Complete Permanent Code Drive Cycle) ****\n");

	/* Initialize Array */
	memset (Test10_9_Sid9Ipt, 0x00, sizeof(Test10_9_Sid9Ipt));
	if ( VerifyIPTData() != PASS )
	{
		if ( TestContinue( "Unable to capture Initial OBDCOND and IGNCNT. Continue?" ) == 'N' )
		{
			LogPrint("**** Test 9.19 FAILED ****\n");
			return(FAIL);
		}
	}
	
	// stop tester-present message
	StopPeriodicMsg (FALSE);
	Sleep (gOBDRequestDelay);
	LogPrint ("INFORMATION: Stop periodic messages\n");

	gIgnoreNoResponse = TRUE;

	ErrorFlags (ER_BYPASS_USER_PROMPT | ER_CONTINUE);
	ErrorCount();   /* clear error count */

	gSuspendScreenOutput = TRUE;
	ret_code = RunDynamicTest10 (tEngineStartTimeStamp);
	gSuspendScreenOutput = FALSE;

	ErrorFlags (0);

	// re-start tester-present message
	StartPeriodicMsg ();

	gIgnoreNoResponse = FALSE;

	error_count = ErrorCount();

	if ( ret_code == PASS && error_count == 0 )
	{
		LogPrint("**** Test 9.19 PASSED ****\n");
	}
	else
	{
		if (error_count != 0)
		{
			LogPrint ("INFORMATION: Errors detected.\n");
		}

		if (ret_code == ABORT)
		{
			LogPrint("**** Test 9.19 INCOMPLETE ****\n");
		}
		else /* FAIL or PASS w/ errors */
		{
			LogPrint("**** Test 9.19 FAILED ****\n");
		}

		if ( TestContinue( "Do you wish to continue?\n" ) == 'N' )
		{
			return FAIL;
		}
	}

	LogUserPrompt("Stop the vehicle in a safe location and turn the ignition off.\n", ENTER_PROMPT);

	StopPeriodicMsg (TRUE);


	/* Prompt user to induce fault to be able to retrieve permanent DTC */
	LogPrint( "\n\n**** Test 9.20  ****\n" );

	LogUserPrompt( "Turn ignition on. Do not crank engine.\n", ENTER_PROMPT );

	LogPrint( "**** Test 9.20 PASSED ****\n" );



	gOBDDTCPermanent = FALSE;


	/* Establish communication, engine off */
	LogPrint( "\n\n**** Test 9.21 (Establish Communications, Engine Off) ****\n" );
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 9.21 FAILED ****\n" );
		LogPrint( "Protocol determination failed." );
		return(FAIL);
	}
	else
	{
		LogPrint( "**** Test 9.21 PASSED ****\n" );
	}

	return(PASS);
}
