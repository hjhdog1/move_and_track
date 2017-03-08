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
*               has been indicated as section 10 in Draft 15.4.
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"
#include "ScreenOutput.h"

STATUS VerifyIPTData (void);
STATUS ReadVIN (void);
STATUS PrintCALIDs (void);
int StartLogFile (void);
STATUS RunDynamicTest10 (unsigned long tEngineStartTimeStamp);

/* Initial data for Test 11.x. Declared in TestToVerifyPerformanceCounters.c */
extern SID9IPT Test10_9_Sid9Ipt[OBD_MAX_ECUS];

/*
*******************************************************************************
** Test 10.x dynamic screen mapping
*******************************************************************************
*/

StaticTextElement  _string_elements10[] =
{
	{"Drive the vehicle in the following maner, at an altitude < 8000 ft", 1, 0},
	{"(BARO < 22 in Hg) and ambient temperature > or = 20 deg F,", 1, 1},
	{"so that the OBD Condition Counter will increment:", 1, 2},

	{"- Continuous time > or = 30 seconds with vehicle speed < or = 1 MPH", 1, 4},
	{"  and accelerator pedal released.", 2, 5},
	{"- Cumulative time > or = 300 seconds with vehicle speed > or = 25 MPH.", 1, 6},
	{"- Cumulative time since engine start > or = 600 seconds.", 1, 7},

	{"OBD Drive Cycle Status", 1, 9},

	{" 30 Seconds Idle Timer:", 1, 11},
	{"300 Seconds at speeds greater then 25 MPH Timer:", 1, 12},
	{"600 Seconds Total Drive Timer:", 1, 13},

	{"ECU ID:", 1, 15},

	{"Initial OBDCOND:", 1, 17},
	{"Current OBDCOND:", 1, 18},

	{"Initial IGNCTR:", 1, 20},
	{"Current IGNCTR:", 1, 21},

	{"Speed:", 60, 12},

	{"Press ESC to abort", 1, 23},
	{"", 0, 24},
};

const int _num_string_elements10 = sizeof(_string_elements10)/sizeof(_string_elements10[0]);

#define SPACE       9
#define ECU_WIDTH   8
#define NUM_WIDTH   5

#define COL1        18

#define IDLE_TIME_ROW       11
#define RUN_TIME_ROW        12
#define TOTAL_TIME_ROW      13
#define ECU_ID_ROW          15
#define INI_OBD_COND_ROW    17
#define CUR_OBD_COND_ROW    18
#define INI_IGN_CNT_ROW     20
#define CUR_IGN_CNT_ROW     21
#define ECU_WIDTH   8

DynamicValueElement  _dynamic_elements10[] =
{
	{26, IDLE_TIME_ROW, NUM_WIDTH},      // 30 seconds idle drive cycle timer
	{51, RUN_TIME_ROW, NUM_WIDTH},       // 300 seconds at speeds greater then 25 MPH timer
	{33, TOTAL_TIME_ROW, NUM_WIDTH},     // 600 seconds total drive timer

	{COL1 + 0*SPACE, ECU_ID_ROW, ECU_WIDTH},   // ECU ID 1
	{COL1 + 1*SPACE, ECU_ID_ROW, ECU_WIDTH},   // ECU ID 2
	{COL1 + 2*SPACE, ECU_ID_ROW, ECU_WIDTH},   // ECU ID 3
	{COL1 + 3*SPACE, ECU_ID_ROW, ECU_WIDTH},   // ECU ID 4
	{COL1 + 4*SPACE, ECU_ID_ROW, ECU_WIDTH},   // ECU ID 5
	{COL1 + 5*SPACE, ECU_ID_ROW, ECU_WIDTH},   // ECU ID 6
	{COL1 + 6*SPACE, ECU_ID_ROW, ECU_WIDTH},   // ECU ID 7
	{COL1 + 7*SPACE, ECU_ID_ROW, ECU_WIDTH},   // ECU ID 8

	{COL1 + 0*SPACE, INI_OBD_COND_ROW, NUM_WIDTH},   // initial obdcond, ECU 1
	{COL1 + 1*SPACE, INI_OBD_COND_ROW, NUM_WIDTH},   // initial obdcond, ECU 2
	{COL1 + 2*SPACE, INI_OBD_COND_ROW, NUM_WIDTH},   // initial obdcond, ECU 3
	{COL1 + 3*SPACE, INI_OBD_COND_ROW, NUM_WIDTH},   // initial obdcond, ECU 4
	{COL1 + 4*SPACE, INI_OBD_COND_ROW, NUM_WIDTH},   // initial obdcond, ECU 5
	{COL1 + 5*SPACE, INI_OBD_COND_ROW, NUM_WIDTH},   // initial obdcond, ECU 6
	{COL1 + 6*SPACE, INI_OBD_COND_ROW, NUM_WIDTH},   // initial obdcond, ECU 7
	{COL1 + 7*SPACE, INI_OBD_COND_ROW, NUM_WIDTH},   // initial obdcond, ECU 8

	{COL1 + 0*SPACE, CUR_OBD_COND_ROW, NUM_WIDTH},   // current obdcond, ECU 1
	{COL1 + 1*SPACE, CUR_OBD_COND_ROW, NUM_WIDTH},   // current obdcond, ECU 2
	{COL1 + 2*SPACE, CUR_OBD_COND_ROW, NUM_WIDTH},   // current obdcond, ECU 3
	{COL1 + 3*SPACE, CUR_OBD_COND_ROW, NUM_WIDTH},   // current obdcond, ECU 4
	{COL1 + 4*SPACE, CUR_OBD_COND_ROW, NUM_WIDTH},   // current obdcond, ECU 5
	{COL1 + 5*SPACE, CUR_OBD_COND_ROW, NUM_WIDTH},   // current obdcond, ECU 6
	{COL1 + 6*SPACE, CUR_OBD_COND_ROW, NUM_WIDTH},   // current obdcond, ECU 7
	{COL1 + 7*SPACE, CUR_OBD_COND_ROW, NUM_WIDTH},   // current obdcond, ECU 8

	{COL1 + 0*SPACE, INI_IGN_CNT_ROW, NUM_WIDTH},   // initial ignctr, ECU 1
	{COL1 + 1*SPACE, INI_IGN_CNT_ROW, NUM_WIDTH},   // initial ignctr, ECU 2
	{COL1 + 2*SPACE, INI_IGN_CNT_ROW, NUM_WIDTH},   // initial ignctr, ECU 3
	{COL1 + 3*SPACE, INI_IGN_CNT_ROW, NUM_WIDTH},   // initial ignctr, ECU 4
	{COL1 + 4*SPACE, INI_IGN_CNT_ROW, NUM_WIDTH},   // initial ignctr, ECU 5
	{COL1 + 5*SPACE, INI_IGN_CNT_ROW, NUM_WIDTH},   // initial ignctr, ECU 6
	{COL1 + 6*SPACE, INI_IGN_CNT_ROW, NUM_WIDTH},   // initial ignctr, ECU 7
	{COL1 + 7*SPACE, INI_IGN_CNT_ROW, NUM_WIDTH},   // initial ignctr, ECU 8

	{COL1 + 0*SPACE, CUR_IGN_CNT_ROW, NUM_WIDTH},   // current ignctr, ECU 1
	{COL1 + 1*SPACE, CUR_IGN_CNT_ROW, NUM_WIDTH},   // current ignctr, ECU 2
	{COL1 + 2*SPACE, CUR_IGN_CNT_ROW, NUM_WIDTH},   // current ignctr, ECU 3
	{COL1 + 3*SPACE, CUR_IGN_CNT_ROW, NUM_WIDTH},   // current ignctr, ECU 4
	{COL1 + 4*SPACE, CUR_IGN_CNT_ROW, NUM_WIDTH},   // current ignctr, ECU 5
	{COL1 + 5*SPACE, CUR_IGN_CNT_ROW, NUM_WIDTH},   // current ignctr, ECU 6
	{COL1 + 6*SPACE, CUR_IGN_CNT_ROW, NUM_WIDTH},   // current ignctr, ECU 7
	{COL1 + 7*SPACE, CUR_IGN_CNT_ROW, NUM_WIDTH},   // current ignctr, ECU 8

	{60, IDLE_TIME_ROW, 4},              // RPM label
	{67, IDLE_TIME_ROW, NUM_WIDTH},      // RPM value
	{67, RUN_TIME_ROW, NUM_WIDTH},       // Speed
};

const int _num_dynamic_elements10 = sizeof(_dynamic_elements10)/sizeof(_dynamic_elements10[0]);

#define IDLE_TIMER          0
#define SPEED_25_MPH_TIMER  1
#define TOTAL_DRIVE_TIMER   2
#define ECU_ID              3
#define INITIAL_OBDCOND     11
#define CURRENT_OBDCOND     19
#define INITIAL_IGNCTR      27
#define CURRENT_IGNCTR      35

#define RPM_LABEL_INDEX     43
#define RPM_INDEX           44
#define SPEED_INDEX         45

#define SetFieldDec(index,val)  (update_screen_dec(_dynamic_elements10,_num_dynamic_elements10,index,val))
#define SetFieldHex(index,val)  (update_screen_hex(_dynamic_elements10,_num_dynamic_elements10,index,val))
#define SetFieldText(index,val) (update_screen_text(_dynamic_elements10,_num_dynamic_elements10,index,val))

/*
*******************************************************************************
** TestToVerifyInUseCounters - Function to run test to verify in-use counters with no faults
*******************************************************************************
*/
STATUS TestToVerifyInUseCounters (void)
{
	int           nNextTest = 0;
	STATUS        ret_code;
	unsigned long tEngineStartTimeStamp;
	unsigned long error_count;

	BOOL          bSubTestFailed = FALSE;


	// Clear Test Failure Flag
	gOBDTestFailed = FALSE;

	/* Initialize Array */
	memset (Test10_9_Sid9Ipt, 0x00, sizeof(Test10_9_Sid9Ipt));


	/* Prompt user to perform drive cycle to clear I/M readiness bits */
	LogPrint("\n\n**** Test 10.1 (Verify in-use counters) ****\n\n");

	LogUserPrompt("Turn key on without cranking or starting engine.\n"
	              "Press enter to continue.", ENTER_PROMPT);

	/* Engine should now be not running */
	gOBDEngineRunning = FALSE;

	/* Determine the OBD protocol to use */
	gOBDProtocolOrder = 0;

	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 10.1 FAILED ****\n" );
		LogPrint( "Protocol determination failed.\n" );
		return(FAIL);
	}
	else
	{
		LogPrint( "**** Test 10.1 PASSED ****\n" );
	}


	/* Get VIN and create (or append) log file */
	LogPrint("\n\n**** Test 10.2 ****\n");

	if ( (nNextTest = StartLogFile()) == 0)
	{
		LogPrint("FAILURE: Unable to Start Log File\n");
		LogPrint("**** Test 10.2 FAILED ****\n");
		return(FAIL);
	}

	/* Add CALIDs to the log file */
	if ( PrintCALIDs () != PASS )
	{
		LogPrint("**** Test 10.2 FAILED ****\n");
		if ( TestContinue( "Print CALIDs failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}

	LogPrint("**** Test 10.2 PASSED ****\n");


	if (nNextTest == 11)
	{
		return(PASS);
	}


	/* Identify ECUs that support SID 1 PID 1 */
	LogPrint("\n\n**** Test 10.3 ****\n");

	if (RequestSID1SupportData() != PASS)
	{
		LogPrint("**** Test 10.3 FAILED ****\n");
		if ( TestContinue( "Diagnostic support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 10.3 PASSED ****\n");
	}


	/* Clear DTCs (service 4) */
	LogPrint("\n\n**** Test 10.4 ****\n");

	if (ClearCodes() != PASS)
	{
		LogPrint("**** Test 10.4 FAILED ****\n");
		if ( TestContinue( "Clear codes failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 10.4 PASSED ****\n");
	}


	gOBDDTCHistorical = FALSE;


	/* Verify I/M readiness is "not ready" (service 1) */
	LogPrint("\n\n**** Test 10.5 ****\n");

	if (VerifyIM_Ready() != PASS)
	{
		LogPrint("**** Test 10.5 FAILED ****\n");
		if ( TestContinue( "Vehicle information data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 10.5 PASSED ****\n");
	}


	/* Verify Monitor Resets (service 6) */
	LogPrint("\n\n**** Test 10.6 ****\n");

	if (VerifyMonitorTestSupportAndResults() != PASS)
	{
		LogPrint("**** Test 10.6 FAILED ****\n");
		if ( TestContinue( "Diagnostic support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 10.6 PASSED ****\n");
	}


	/* Verify no pending DTCs (service 7) */
	LogPrint("\n\n**** Test 10.7 ****\n");

	if (VerifyDTCPendingData() != PASS)
	{
		LogPrint("**** Test 10.7 FAILED ****\n");
		if ( TestContinue( "Verify DTC Pending Data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 10.7 PASSED ****\n");
	}


	/* Verify no confirmed DTCs (service 3) */
	LogPrint("\n\n**** Test 10.8 ****\n");

	if (VerifyDTCStoredData() != PASS)
	{
		LogPrint("**** Test 10.8 FAILED ****\n");
		if ( TestContinue( "Verify DTC Stored Data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 10.8 PASSED ****\n");
	}


	/* Get in-use performance tracking (service 9 infotype 8 and B) */
	LogPrint("\n\n**** Test 10.9 ****\n");

	if ( VerifyIPTData() != PASS )
	{
		bSubTestFailed = TRUE;
		LogPrint("**** Test 10.9 FAILED ****\n");
		if ( TestContinue( "Verify IPT Stored Data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}

	LogUserPrompt("Turn ignition off (engine off) for 60 seconds.\n"
	              "Press enter to continue.", ENTER_PROMPT);

	DisconnectProtocol ();

	LogUserPrompt("Turn ignition to crank position and start engine.\n"
	              "Press enter to continue.", ENTER_PROMPT);

	tEngineStartTimeStamp = GetTickCount ();

	if ( bSubTestFailed == FALSE )
	{
		LogPrint("**** Test 10.9 PASSED ****\n");
	}


	/* Engine should now be running */
	gOBDEngineRunning = TRUE;

	LogPrint("\n\n**** Test 10.10 ****\n");

	if (ConnectProtocol() != PASS)
	{
		LogPrint("**** Test 10.10 FAILED ****\n");
		if ( TestContinue( "Protocol determination failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 10.10 PASSED ****\n");
	}


	/* User prompt, run test */
	LogPrint("\n\n**** Test 10.11 ****\n");

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
		LogPrint("**** Test 10.11 PASSED ****\n");
	}
	else
	{
		if (error_count != 0)
		{
			LogPrint ("INFORMATION: Errors detected.\n");
		}

		if (ret_code == ABORT)
		{
			LogPrint("**** Test 10.11 INCOMPLETE ****\n");
		}
		else /* FAIL or PASS w/ errors */
		{
			LogPrint("**** Test 10.11 FAILED ****\n");
		}

		if ( TestContinue( "Do you wish to continue?\n" ) == 'N' )
		{
			return FAIL;
		}
	}

	LogUserPrompt("Stop the vehicle in a safe location without turning the ignition off.\n\n"
	              "Press enter to continue.", ENTER_PROMPT);

	StopPeriodicMsg (TRUE);


	/* Verify engine warm data (service 1) */
	LogPrint("\n\n**** Test 10.12 ****\n");
	gOBDEngineWarm = TRUE;

	if (VerifyDiagnosticSupportAndData() == FAIL)
	{
		LogPrint("\n\n**** Test 10.12 FAILED ****\n");
		return FAIL;
	}

	LogPrint("\n\n**** Test 10.12 PASSED ****\n");


	/* Get in-use performance tracking (service 9 infotype 8) */
	LogPrint("\n\n**** Test 10.13 ****\n");

	if ( LogSid9Ipt() != PASS )
	{
		LogPrint ("FAILURE: SID 9 IPT data missing\n");
		LogPrint("**** Test 10.13 FAILED ****\n");
		if ( TestContinue( "Errors detected.  Do you wish to continue?\n" ) == 'N' )
		{
			return FAIL;
		}
	}
	else
	{
		LogPrint("\n\n**** Test 10.13 PASSED ****\n");
	}


	LogPrint("**** Section 10 of the Dynamic Test has completed %s. ****\n",
	         (gOBDTestFailed != TRUE) ? "successfully" : "unsuccessfully");

	return (gOBDTestFailed != TRUE) ? PASS : FAIL;
}

/*
*******************************************************************************
** substring
*******************************************************************************
*/
char * substring (char * str, const char * substr)
{
	int tok_len = strlen (substr);
	int n       = strlen (str) - tok_len;
	int i;

	for (i=0; i<=n; i++)
	{
		if (str[i] == substr[0])
			if (strncmp (&str[i], substr, tok_len) == 0)
				return &str[i];
	}

	return 0;
}

/*
*******************************************************************************
** StartLogFile - create new log file or open and append to existing one.
**                use VIN as log filename.
*******************************************************************************
*/
int StartLogFile (void)
{
	char buf[256];

	int nNextTest = 0;

	/* Get VIN */
	if (ReadVIN () == PASS)
	{
		/* use VIN as log filename */
		strcpy (gLogFileName, gVIN);
	}
	else
	{
		do
		{
			printf ("\nPROMPT: Unable to obtain VIN\n\nEnter VIN: ");
			scanf ("%s", buf);
		} while ((strlen (buf) + 1) > 18 /*sizeof (gVIN)*/);

		strcpy (gLogFileName, buf);
	}

	/* Check for log file from Tests 5.xx - 9.xx  */
	strcat (gLogFileName, ".log");
	if ( (ghTempLogFile != ghLogFile) && (ghLogFile != NULL) )
	{
		fclose (ghLogFile);
	}

	/* Test if log file already exists */
	ghLogFile = fopen (gLogFileName, "r+");
	if (ghLogFile == NULL)
	{
		/* file does not exist - create file and start with test 10 */
		ghLogFile = fopen (gLogFileName, "w+");
		if (ghLogFile == NULL)
		{
			printf ("FAILURE: Cannot open log file %s\n", gLogFileName);
			return 0;
		}

		/* log file doesn't exist. continue with Test 10.x */
		nNextTest = 10;
	}
	else
	{
		/* scan file for SID9 IPT data at Test 10.9 */
		ReadSid9IptFromLogFile ( "**** Test 10.9 ****", "**** Test 10.9", Test10_9_Sid9Ipt );

		/* log file already exists, go to Test 11.x */
		fputs ("\n****************************************************\n", ghLogFile);
		nNextTest = 11;
	}

	/* Application version, build date, OS, etc */
	LogVersionInformation ();

	/* Copy temp log file to actual log file */
	if (AppendLogFile () != PASS)
	{
		printf ("FAILURE: Error copying temp log file to %s\n", gLogFileName);
		return 0;
	}

	/* Echo user responses to the log file */
	LogPrint ("INFORMATION: Model Year of this vehicle?  %d\n", gModelYear);
	LogPrint ("INFORMATION: How many OBD-II ECUs are on this vehicle (1 to 8)?  %d\n", gUserNumEcus);
	LogPrint ("INFORMATION: How many reprogrammable, OBD-II ECUs are on this vehicle (1 to 8)? %d\n", gUserNumEcusReprgm);
	LogPrint ("INFORMATION: Does the vehicle use compression ignition (i.e. diesel)? %s\n", gOBDDieselFlag ? "YES" : "NO");
	LogPrint ("INFORMATION: Is this a hybrid vehicle? %s\n", gOBDHybridFlag ? "YES" : "NO");

	/* done */
	return nNextTest;
}


/******************************************************************************
//	Function:	ReadVIN
//
//	Purpose:	Purpose of this function is to read the VIN from the ECUs.
//				If an ECU returns a correctly formatted VIN, return TRUE,
//				otherwise return FAIL.
******************************************************************************/
STATUS ReadVIN (void)
{
	unsigned long  EcuIndex;
	unsigned long  NumResponses;
	SID_REQ        SidReq;
	SID9          *pSid9;
	unsigned long  SidIndex;

	/* Request SID 9 support data */
	if (RequestSID9SupportData() != PASS)
	{
		return FAIL;
	}

	/* INF Type Vin Count for non-ISO15765 only */
	if (gOBDList[gOBDListIndex].Protocol != ISO15765)
	{
		/* If INF is supported by any ECU, request it */
		if (IsSid9InfSupported (-1, INF_TYPE_VIN_COUNT) == TRUE)
		{
			SidReq.SID    = 9;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = INF_TYPE_VIN_COUNT;

			if (SidRequest (&SidReq, SID_REQ_NORMAL) != PASS)
			{
				LogPrint ("FAILURE: SID $9 INF $1 request failed\n");
				return FAIL;
			}

			NumResponses = 0;

			/* check responses */
			for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
			{
				/* Check the data to see if it is valid */
				pSid9 = (SID9 *)&gOBDResponse[EcuIndex].Sid9Inf[0];

				if (gOBDResponse[EcuIndex].Sid9InfSize != 0)
				{
					for (SidIndex = 0;
					     SidIndex < (gOBDResponse[EcuIndex].Sid9InfSize / sizeof (SID9));
					     SidIndex++ )
					{
						if (pSid9[SidIndex].INF == INF_TYPE_VIN_COUNT)
						{
							if (pSid9[SidIndex].NumItems != 0x05)
							{
								LogPrint ( "FAILURE: ECU %X  SID $9 INF $1 (VIN Count) NumItems = %d (should be 5)\n",
								           GetEcuId(EcuIndex),
								           pSid9[SidIndex].NumItems );
								return FAIL;
							}
						}
					}

					NumResponses++;
				}
			}

			if (NumResponses > 1)
			{
				LogPrint ("FAILURE: %u ECUs responded to SID $9 INF $1 (VIN COUNT).  Only 1 is allowed.\n", NumResponses);
				return FAIL;
			}
		}
	}

	/* If INF is supported by any ECU, request it */
	if (IsSid9InfSupported (-1, INF_TYPE_VIN) == TRUE)
	{
		SidReq.SID    = 9;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = INF_TYPE_VIN;

		if (SidRequest (&SidReq, SID_REQ_NORMAL) != PASS)
		{
			LogPrint ( "FAILURE: SID $9 INF $2 request failed\n" );
			return FAIL;
		}

		NumResponses = 0;

		/* check responses */
		for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
		{
			/* Check the data to see if it is valid */
			pSid9 = (SID9 *)&gOBDResponse[EcuIndex].Sid9Inf[0];

			for (SidIndex = 0;
			     SidIndex < (gOBDResponse[EcuIndex].Sid9InfSize / sizeof (SID9));
			     SidIndex++ )
			{
				if (pSid9[SidIndex].INF == INF_TYPE_VIN)
				{
					/* Copy the VIN into the global array, J1699 Rev 11.5 section 5.17.5 */
					if (gOBDList[gOBDListIndex].Protocol == ISO15765)
					{
						memcpy (gVIN, &pSid9[0].Data[0], 17);

						/* J1699 V 11.5 TC#5.17.5 call for check that there are no
						** pad bytes included in message.
						*/
						if (pSid9[0].Data[20] != 0x00)
						{
							LogPrint ( "FAILURE: ECU %X  SID $9 INF $2 VIN format error, must be 17 chars!\n", GetEcuId(EcuIndex) );
							return FAIL;
						}
					}
					else
					{
						if (SidIndex == 0)
						{
							gVIN[0] = pSid9[SidIndex].Data[3];
						}
						else if (SidIndex < 5)
						{
							memcpy (&gVIN[SidIndex*4 - 3], &pSid9[SidIndex].Data[0], 4);
						}
					}

					if ( (gOBDList[gOBDListIndex].Protocol == ISO15765) || (SidIndex == 4) )
					{
						if (++NumResponses != 0x01) /* only 1 ECU allowed to support VIN */
						{
							LogPrint ( "FAILURE: %u ECUs responded to SID $9 INF $2.  Only 1 is allowed.\n", NumResponses );
							return FAIL;
						}

						if (VerifyVINFormat () != PASS)
						{
							return FAIL;
						}
					}
				}
			}
		}

		return PASS;
	}

	/* No VIN support */
	return FAIL;
}


//*****************************************************************************
//
//	Function:	PrintCALIDs
//
//	Purpose:	print CALIDs to the log file
//
//*****************************************************************************
STATUS PrintCALIDs (void)
{
	unsigned long  EcuIndex;
	SID_REQ        SidReq;
	SID9          *pSid9;

	unsigned long  Inf3NumItems[OBD_MAX_ECUS] = {0};


	if (gOBDList[gOBDListIndex].Protocol != ISO15765)
	{
		if (IsSid9InfSupported (-1, 3) == PASS)
		{
			SidReq.SID    = 9;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = 3;

			if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
			{
				LogPrint ( "FAILURE: SID $9 INF $3 request failed\n" );
				return FAIL;
			}

			for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
			{
				/* If INF is not supported, skip to next ECU */
				if (IsSid9InfSupported (EcuIndex, 3) == FALSE)
					continue;

				/* Check the data to see if it is valid */
				pSid9 = (SID9 *)&gOBDResponse[EcuIndex].Sid9Inf[0];

				if (gOBDResponse[EcuIndex].Sid9InfSize == 0)
				{
					LogPrint ("FAILURE: ECU %X  No SID $9 INF $3 data\n", GetEcuId(EcuIndex));
					ERROR_RETURN;
				}

				if (pSid9[0].NumItems & 0x03)
				{
					LogPrint ( "FAILURE: ECU %X  SID $9 INF $3 (CALID Count) NumItems = %d (should be a multiple of 4)\n",
					           GetEcuId(EcuIndex),
					           pSid9[0].NumItems );
					ERROR_RETURN;
				}

				Inf3NumItems[EcuIndex] = pSid9[0].NumItems;
			}
		}
	}

	SidReq.SID    = 9;
	SidReq.NumIds = 1;
	SidReq.Ids[0] = 4;

	if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
	{
		LogPrint ("FAILURE: SID $9 INF $4 request failed\n");
		return FAIL;
	}

	for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
	{
		if (VerifyCALIDFormat (EcuIndex, Inf3NumItems[EcuIndex]) != PASS)
			ERROR_RETURN;
	}

	return PASS;
}


/*
*******************************************************************************
** VerifyIPTData
*******************************************************************************
*/
STATUS VerifyIPTData (void)
{
	SID_REQ       SidReq;
	unsigned int  EcuIndex;
	unsigned char  fInf8Supported = FALSE;
	unsigned char  fInfBSupported = FALSE;


	SidReq.SID    = 9;
	SidReq.NumIds = 1;

	/* Sid 9 Inf 8 request*/
	if ( IsSid9InfSupported (-1, 0x08) == TRUE )
	{
		fInf8Supported = TRUE;

		SidReq.Ids[0] = 8;
	}

	/* Sid 9 Inf B request*/
	if ( IsSid9InfSupported (-1, 0x0B) == TRUE )
	{
		fInfBSupported = TRUE;

		SidReq.Ids[0] = 0x0B;
	}


	if ( fInf8Supported == FALSE && fInfBSupported == FALSE )
	{
		LogPrint( "FAILURE: SID $9 IPT (INF $8 and INF $B) not supported\n" );
		return FAIL;
	}
	else if ( fInf8Supported == TRUE && fInfBSupported == TRUE )
	{
		LogPrint( "FAILURE: SID $9 IPT (INF $8 and INF $B) both supported\n" );
		return FAIL;
	}


	if ( SidRequest( &SidReq, SID_REQ_NORMAL ) != PASS)
	{
		LogPrint( "FAILURE: SID $9 INF $%X request failed\n", SidReq.Ids[0] );
		return (FAIL);
	}

	for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
	{
		if ( gOBDResponse[EcuIndex].Sid9InfSize > 0)
		{
			if ( (fInf8Supported == TRUE && VerifyINF8Data (EcuIndex) != PASS ) ||
			     (fInfBSupported == TRUE && VerifyINFBData (EcuIndex) != PASS ) )
			{
				return (FAIL);
			}

			if ( GetSid9IptData (EcuIndex, &Test10_9_Sid9Ipt[EcuIndex]) != PASS )
			{
				LogPrint( "FAILURE: ECU %X  Get SID $9 IPT Data failed\n", GetEcuId(EcuIndex) );
				return (FAIL);
			}
		}
	}

	if ( LogSid9Ipt() != PASS )
	{
		LogPrint( "FAILURE: Log SID $9 IPT Data failed\n" );
		return (FAIL);
	}

	return (PASS);
}


//*****************************************************************************
//
//	Function:	GetSid9IptData
//
//	Purpose:	Copy from gOBDResponse[EcuIndex].Sid9Inf into common,
//              protocol-independent format.
//
//*****************************************************************************
STATUS GetSid9IptData ( unsigned int EcuIndex, SID9IPT *pSid9Ipt )
{
	unsigned int    SidIndex;
	unsigned short *pData;
	SID9           *pSid9;
	unsigned int    IPT_MAX;


	memset (pSid9Ipt, 0, sizeof (SID9IPT));

	if ( gOBDResponse[EcuIndex].Sid9InfSize == 0 )
	{
		return FAIL;
	}

	if ( gOBDList[gOBDListIndex].Protocol == ISO15765 )
	{
		// If MY 2010 and later spark ignition vehicle
		if ( gModelYear >= 2010 )
		{
			// if this is a spark ingition vehicle
			if ( gOBDDieselFlag == FALSE )
			{
				// if INF 8 is notsupported
				if ( gOBDResponse[EcuIndex].Sid9Inf[0] != 0x08 )
				{
					LogPrint ( "FAILURE: ECU %X  Model Year 2010 and later spark ignition vehicles must use SID $9 INF $8 for IPT data\n", GetEcuId(EcuIndex) );
					ERROR_RETURN;
				}

				// In-Use Performance Data must contain 40 bytes of data
				if ( gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 40 )
				{
					LogPrint ( "FAILURE: ECU %X  SID $9 INF $8 (IPT) Data Size Error = %d (Must be 40 bytes!)\n",
					           GetEcuId(EcuIndex),
					           (gOBDResponse[EcuIndex].Sid9InfSize - 0x02) );
					ERROR_RETURN;
				}

				// SID9 INF8 NODI must equal $14
				if ( gOBDResponse[EcuIndex].Sid9Inf[1] != 0x14 )
				{
					LogPrint ( "FAILURE: ECU %X  SID $9 INF $8 NODI = %d (Must be 20 ($14))\n",
					           GetEcuId(EcuIndex),
					           gOBDResponse[EcuIndex].Sid9Inf[1] );
					ERROR_RETURN;
				}
			}

			// else this is a compression ingition vehicle
			else
			{
				// if INF B is notsupported
				if ( gOBDResponse[EcuIndex].Sid9Inf[0] != 0x0B )
				{
					LogPrint ( "FAILURE: ECU %X  Model Year 2010 and later compression ignition vehicles must use SID $9 INF $B for IPT data\n", GetEcuId(EcuIndex) );
					ERROR_RETURN;
				}

				// In-Use Performance Data must contain 40 bytes of data
				if ( gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 32 )
				{
					LogPrint ( "FAILURE: ECU %X  SID $9 INF $B (IPT) Data Size Error = %d (Must be 40 bytes!)\n",
					           GetEcuId(EcuIndex),
					           (gOBDResponse[EcuIndex].Sid9InfSize - 0x02) );
					ERROR_RETURN;
				}

				// SID9 INFB NODI must equal $10
				if ( gOBDResponse[EcuIndex].Sid9Inf[1] != 0x10 )
				{
					LogPrint ( "FAILURE: ECU %X  SID $9 INF $B NODI = %d (Must be 16 ($20))\n",
					           GetEcuId(EcuIndex),
					           gOBDResponse[EcuIndex].Sid9Inf[1] );
					ERROR_RETURN;
				}
			}
		}

		// If MY 2009 or earlier vehicle
		else
		{
			// In-Use Performance Data must contain 32 or 40 bytes of data
			if ( gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 32 &&
			     gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 40 )
			{
				LogPrint ( "WARNING: ECU %X  SID $9 INF $%02X (IPT) Data Size Error = %d (Must be 32 or 40 bytes!)\n",
				           GetEcuId(EcuIndex),
				           gOBDResponse[EcuIndex].Sid9Inf[0],
				           (gOBDResponse[EcuIndex].Sid9InfSize - 0x02));
				ERROR_RETURN;
			}

			// SID9 INF8 NODI must equal $10 or $14
			if ( gOBDResponse[EcuIndex].Sid9Inf[1] != 0x10 &&
			     gOBDResponse[EcuIndex].Sid9Inf[1] != 0x14 )
			{
				LogPrint ( "WARNING: ECU %X  SID $9 INF $%02X NODI = %d (Must be 16 ($10) or 20 ($14))\n",
				           GetEcuId(EcuIndex),
				           gOBDResponse[EcuIndex].Sid9Inf[0],
				           gOBDResponse[EcuIndex].Sid9Inf[1] );
				ERROR_RETURN;
			}
		}

		memcpy (pSid9Ipt, &(gOBDResponse[EcuIndex].Sid9Inf[0]), sizeof (SID9IPT));

		pSid9 = (SID9 *)&gOBDResponse[EcuIndex].Sid9Inf[0];

		// point to start of IPT data in structure
		pData = &(pSid9Ipt->IPT[0]);

		// calculate number of IPT data bytes
		IPT_MAX = (pSid9Ipt->NODI)*2;

		// swap low and high bytes
		for ( SidIndex = 2; SidIndex <= IPT_MAX; SidIndex+=2 )
		{
			*pData++ = gOBDResponse[EcuIndex].Sid9Inf[SidIndex] * 256
			         + gOBDResponse[EcuIndex].Sid9Inf[SidIndex+1];
		}
	}
	else
	{
		if (gOBDResponse[EcuIndex].Sid9InfSize / sizeof(SID9) != 0x08)
		{
			LogPrint ("FAILURE: ECU %X  SID $9 INF $8 (IPT) Data Size Error, Must be 32 bytes!\n", GetEcuId(EcuIndex) );
			return FAIL;
		}

		pSid9 = (SID9 *)&gOBDResponse[EcuIndex].Sid9Inf[0];

		pSid9Ipt->INF  = gOBDResponse[EcuIndex].Sid9Inf[0];
		pSid9Ipt->NODI = 16;

		// point to start of IPT data
		pData = &(pSid9Ipt->IPT[0]);

		// save IPT data to structure
		for ( SidIndex = 0; SidIndex < (gOBDResponse[EcuIndex].Sid9InfSize / sizeof(SID9)); SidIndex++ )
		{
			if (pSid9[SidIndex].INF != 0x08)
			{
				return FAIL;
			}

			*pData++ = pSid9[SidIndex].Data[0] * 256 + pSid9[SidIndex].Data[1];
			*pData++ = pSid9[SidIndex].Data[2] * 256 + pSid9[SidIndex].Data[3];
		}
	}

	// data structure is valid
	pSid9Ipt->Flags = 1;

	return PASS;
}


//******************************************************************************
// LogSid9Ipt
//*****************************************************************************
const char szECUID[]     = "ECU ID:";
const char szINF_SIZE[]  = "INF_SIZE:";

const char szINF8[][10]  = { "OBDCOND",
							 "IGNCNTR",
							 "CATCOMP1",
							 "CATCOND1",
							 "CATCOMP2",
							 "CATCOND2",
							 "O2COMP1",
							 "O2COND1",
							 "O2COMP2",
							 "O2COND2",
							 "EGRCOMP",
							 "EGRCOND",
							 "AIRCOMP",
							 "AIRCOND",
							 "EVAPCOMP",
							 "EVAPCOND",
							 "SO2SCOMP1",
							 "SO2SCOND1",
							 "SO2SCOMP2",
							 "SO2SCOND2" };

const char szINFB[][10]  = { "OBDCOND",
							 "IGNCNTR",
							 "HCCATCOMP",
							 "HCCATCOND",
							 "NCATCOMP",
							 "NCATCOND",
							 "NADSCOMP",
							 "NADSCOND",
							 "PMCOMP",
							 "PMCOND",
							 "EGSCOMP",
							 "EGSCOND",
							 "EGRCOMP",
							 "EGRCOND",
							 "BPCOMP",
							 "BPCOND" };

STATUS LogSid9Ipt (void)
{
	SID_REQ       SidReq;
	unsigned int  EcuIndex;
	unsigned int  IptIndex;
	SID9IPT       Sid9Ipt;
	unsigned char fInf8Supported = FALSE;
	unsigned char fInfBSupported = FALSE;


	SidReq.SID    = 9;
	SidReq.NumIds = 1;

	/* Sid 9 Inf 8 request*/
	if ( IsSid9InfSupported (-1, 0x08) == TRUE )
	{
		fInf8Supported = TRUE;

		/* Sid 9 Inf 8 request*/
		SidReq.Ids[0] = 0x08;
	}

	/* Sid 9 Inf B request*/
	if ( IsSid9InfSupported (-1, 0x0B) == TRUE )
	{
		fInfBSupported = TRUE;

		/* Sid 9 Inf 8 request*/
		SidReq.Ids[0] = 0x0B;
	}

	if ( fInf8Supported == FALSE && fInfBSupported == FALSE )
	{
		LogPrint( "FAILURE: SID $9 IPT (INF $8 and INF $B) not supported\n" );
		return FAIL;
	}
	else if ( fInf8Supported == TRUE && fInfBSupported == TRUE )
	{
		LogPrint( "FAILURE: SID $9 IPT (INF $8 and INF $B) both supported\n" );
		return FAIL;
	}

	if ( SidRequest( &SidReq, SID_REQ_NORMAL ) != PASS)
	{
		LogPrint("FAILURE: SID $9 INF $8 request failed\n");
		return FAIL;
	}

	for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
	{
		if ( gOBDResponse[EcuIndex].Sid9InfSize > 0 )
		{
			if ( GetSid9IptData ( EcuIndex, &Sid9Ipt ) != PASS )
			{
				return (FAIL);
			}

			// log OBDCOND, IGNCTR, all OBD monitor and completion counters
			LogPrint( "INFORMATION: %s %X\n", szECUID, GetEcuId (EcuIndex) );
			LogPrint( "INFORMATION: %s %X %d\n", szINF_SIZE, Sid9Ipt.INF, Sid9Ipt.NODI );

			for ( IptIndex = 0; IptIndex < Sid9Ipt.NODI; IptIndex++ )
			{
				LogPrint( "INFORMATION: %-8s = %u\n",
				          (Sid9Ipt.INF == 0x08) ? szINF8[IptIndex] : szINFB[IptIndex],
				          Sid9Ipt.IPT[IptIndex] );
			}
			LogPrint ("\n");
		}
	}

	return PASS;
}


//*****************************************************************************
// ReadSid9Ipt
//*****************************************************************************
BOOL ReadSid9Ipt (const char * szTestSectionEnd, SID9IPT Sid9Ipt[])
{
	char buf[256];
	char * p;
	int  count;
	unsigned int EcuIndex;
	unsigned int EcuId = 0;

	// search for ECU ID
	while (fgets (buf, sizeof(buf), ghLogFile) != 0)
	{
		if (substring (buf, szTestSectionEnd) != 0)     // end of Test XX section
			return FALSE;

		if ( (p = substring (buf, szECUID)) != 0)
		{
			p += sizeof (szECUID);
			EcuId = strtoul (p, NULL, 16);
			break;
		}
	}

	// find EcuIndex
	for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
	{
		if (EcuId == GetEcuId (EcuIndex))
		{
			break;
		}
	}

	if (EcuIndex >= gUserNumEcus)
	{
		return FALSE;
	}

	// search for IPT INFOTYPE and SIZE
	while (fgets (buf, sizeof(buf), ghLogFile) != 0)
	{
		if (substring (buf, szTestSectionEnd) != 0)     // end of Test XX section
			return FALSE;

		if ( (p = substring (buf, szINF_SIZE)) != 0)
		{
			p += sizeof (szINF_SIZE);
			Sid9Ipt[EcuIndex].INF = (unsigned char) strtod (p, NULL);
			p += 2;
			Sid9Ipt[EcuIndex].NODI = (unsigned char) strtod (p, NULL);
			break;
		}
	}

	// read counters
	for ( count = 0; count < Sid9Ipt[EcuIndex].NODI; count++ )
	{
		fgets (buf, sizeof(buf), ghLogFile);

		if ( ( Sid9Ipt[EcuIndex].INF == 0x08 && (p = substring (buf, szINF8[count])) != 0 ) ||
		     ( Sid9Ipt[EcuIndex].INF == 0x0B && (p = substring (buf, szINFB[count])) != 0 ) )
		{
			p = substring (p, "=") + 1;
			Sid9Ipt[EcuIndex].IPT[count] = (unsigned short)strtoul (p, NULL, 10);
			continue;
		}

		return FALSE;
	}

	// Sid 9 Inf 8 data valid
	Sid9Ipt[EcuIndex].Flags = 1;

	return TRUE;
}

/*
*******************************************************************************
** ReadSid9IptFromLogFile
*******************************************************************************
*/
BOOL ReadSid9IptFromLogFile ( const char * szTestSectionStart, const char * szTestSectionEnd, SID9IPT Sid9Ipt[] )
{
	char buf[256];
	unsigned int EcuIndex;

	// log file should be open
	if ( ghLogFile == NULL )
	{
		printf ("FAILURE: Log File not open\n");
		return FALSE;
	}

	// search from beginning of file
	fseek (ghLogFile, 0, SEEK_SET);

	while ( fgets (buf, sizeof(buf), ghLogFile) != 0 )
	{
		if ( substring (buf, szTestSectionStart) != 0 )
		{
			for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
			{
				if ( ReadSid9Ipt ( szTestSectionEnd, Sid9Ipt ) == FALSE )
				{
					break;
				}
			}
			fseek ( ghLogFile, 0, SEEK_END );
			return TRUE;
		}
	}

	// move to end of file
	fseek ( ghLogFile, 0, SEEK_END );
	return FALSE;
}


/*
*******************************************************************************
** RunDynamicTest10
*******************************************************************************
*/
STATUS RunDynamicTest10 (unsigned long tEngineStartTimeStamp)
{
	unsigned int   EcuIndex, TestState, bLoop, bFail, bRunTimeSupport;
	unsigned int   bSid9Ipt = 0;  // support INF for IPT data, default 0 = no support
	unsigned long  t1SecTimer, tDelayTimeStamp;

	BOOL           ErrorPrinted = FALSE;    // indicates whether or not the failure message has already been printed

    BOOL           bPermDTC;

	unsigned short tTestStartTime, tTestCompleteTime;
	unsigned short tTempTime;
	unsigned short tIdleTime;       // time at idle (stops at 30)
	unsigned short tAtSpeedTime;    // time at speeds > 25mph (stops at 300)
	unsigned short RPM, Speed, RunTime, OBDCond, IgnCnt;
	unsigned short tObdCondTimestamp[OBD_MAX_ECUS];

	SID_REQ        SidReq;
	SID9IPT        Sid9Ipt[OBD_MAX_ECUS];
	SID1          *pSid1;

	// determine PID support
	bRunTimeSupport = IsSid1PidSupported (-1, 0x1F);

	if ((IsSid1PidSupported (-1, 0x0C) == FALSE) && (bRunTimeSupport == FALSE))
	{
		LogPrint("FAILURE: SID $1 PIDs $0C (RPM) and $1F (RUNTM) not supported\n");
		return (FAIL);
	}

	/* Initialize Array */
	memset ( Sid9Ipt, 0x00, sizeof(Sid9Ipt) );

	if ( IsSid9InfSupported ( -1, 0x08 ) == TRUE )
	{
		bSid9Ipt = 0x08;
	}
	else if ( IsSid9InfSupported ( -1, 0x0B ) == TRUE )
	{
		bSid9Ipt = 0x0B;
	}

	// initialize static text elements
	init_screen (_string_elements10, _num_string_elements10);

	for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
	{
		// initialize ECU IDs
		SetFieldHex (ECU_ID+EcuIndex, GetEcuId (EcuIndex));

		// initialize initial OBDCONDs
		SetFieldDec (INITIAL_OBDCOND+EcuIndex, Test10_9_Sid9Ipt[EcuIndex].IPT[0]);

		// initialize initial IGNCTRs
		SetFieldDec (INITIAL_IGNCTR+EcuIndex, Test10_9_Sid9Ipt[EcuIndex].IPT[1]);

		tObdCondTimestamp[EcuIndex] = 0;

		// if INF 8/B not supported, print 0 (INF8/B will never update)
		if ( bSid9Ipt == 0 )
		{
			SetFieldDec (CURRENT_OBDCOND+EcuIndex, Sid9Ipt[EcuIndex].IPT[0]);
			SetFieldDec (CURRENT_IGNCTR+EcuIndex,  Sid9Ipt[EcuIndex].IPT[1]);
		 }
	}

	if (bRunTimeSupport == FALSE)
		SetFieldText(RPM_LABEL_INDEX, "RPM:");

	// flush the STDIN stream of any user input before loop
	clear_keyboard_buffer ();

	RPM = Speed = RunTime = OBDCond = IgnCnt = 0;
	tAtSpeedTime = 0;
	tIdleTime = 0;
	tTempTime = 0;
	tTestCompleteTime = 0;
	tTestStartTime = (unsigned short)(tEngineStartTimeStamp / 1000);
	tDelayTimeStamp = t1SecTimer = GetTickCount ();

	TestState = 0;

	//-------------------------------------------
	// loop until test completes
	//-------------------------------------------
	for (;;)
	{
		if ( TestPhase == eTestNoFault3DriveCycle )
		{
			//-------------------------------------------
			// request Permanent Codes - SID A
			//-------------------------------------------
			SidReq.SID    = 0x0A;
			SidReq.NumIds = 0;

			if ( SidRequest( &SidReq, SID_REQ_NO_PERIODIC_DISABLE ) != PASS )
			{
				if ( ErrorPrinted == FALSE )
                {
                    LogPrint( "FAILURE: SID $0A request failed\n" );
                }
				ErrorPrinted = TRUE;
			}

			bPermDTC = FALSE;
			for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
			{
				if ( gOBDResponse[EcuIndex].SidASize != 0 )
				{
					bPermDTC = TRUE;
				}
			}

			if ( bPermDTC == FALSE )
			{
				LogPrint ( "FAILURE: SID $0A - DTC erased prematurely.\n" );
				return (FAIL);
			 }
		}


		//-------------------------------------------
		// request RPM - SID 1 PID $0C
		//-------------------------------------------
		if (bRunTimeSupport == FALSE)
		{
			SidReq.SID    = 1;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = 0x0c;

			if (SidRequest( &SidReq, SID_REQ_NO_PERIODIC_DISABLE ) != PASS)
			{
				LogPrint("FAILURE: SID $1 PID $0C request failed\n");
				return (FAIL);
			}

			for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
			{
				if (gOBDResponse[EcuIndex].Sid1PidSize > 0)
				{
					pSid1 = (SID1 *)&gOBDResponse[EcuIndex].Sid1Pid[0];

					if (pSid1->PID == 0x0c)
					{
						RPM = pSid1->Data[0];
						RPM = RPM << 8 | pSid1->Data[1];

						// convert from 1 cnt = 1/4 RPM to 1 cnt = 1 RPM
						RPM >>= 2;
						break;
					}
				}
			}

			if (EcuIndex >= gUserNumEcus)
			{
				LogPrint ("FAILURE: SID $1 PID $0C missing response\n");
				return (FAIL);
			}
		}

		//-------------------------------------------
		// request Speed - SID 1 PID $0D
		//-------------------------------------------
		SidReq.SID    = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = 0x0d;

		if (SidRequest( &SidReq, SID_REQ_NO_PERIODIC_DISABLE ) != PASS)
		{
			LogPrint("FAILURE: SID $1 PID $0D request failed\n");
			return (FAIL);
		}

		for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid1PidSize > 0)
			{
				pSid1 = (SID1 *)&gOBDResponse[EcuIndex].Sid1Pid[0];

				if (pSid1->PID == 0x0d)
				{
					unsigned long temp = pSid1->Data[0];

					// convert from km/hr to mile/hr
					Speed = (unsigned short)( (temp * 6214) / 10000);
					break;
				}
			}
		}

		if (EcuIndex >= gUserNumEcus)
		{
			LogPrint ("FAILURE: SID $1 PID $0D missing response\n");
			return (FAIL);
		}

		//-------------------------------------------
		// request engine RunTime - SID 1 PID $1F
		//-------------------------------------------
		if (bRunTimeSupport == TRUE)
		{
			SidReq.SID    = 1;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = 0x1f;

			if (SidRequest( &SidReq, SID_REQ_NO_PERIODIC_DISABLE ) != PASS)
			{
				LogPrint("FAILURE: SID $1 PID $1F request failed\n");
				return (FAIL);
			}

			for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
			{
				if (gOBDResponse[EcuIndex].Sid1PidSize > 0)
				{
					pSid1 = (SID1 *)&gOBDResponse[EcuIndex].Sid1Pid[0];

					if (pSid1->PID == 0x1f)
					{
						RunTime = pSid1->Data[0];
						RunTime = RunTime << 8 | pSid1->Data[1];    // 1 cnt = 1 sec
						break;
					}
				}
			}

			if (EcuIndex >= gUserNumEcus)
			{
				LogPrint ("FAILURE: SID $1 PID $1F missing response\n");
				return (FAIL);
			}
		}
		else
		{
			RunTime = (unsigned short)(GetTickCount () / 1000) - tTestStartTime;
		}

		//-------------------------------------------
		// Get SID 9 IPT
		//-------------------------------------------
		if ( bSid9Ipt != 0 )
		{
			SidReq.SID    = 9;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = bSid9Ipt;

			if ( SidRequest( &SidReq, SID_REQ_NO_PERIODIC_DISABLE ) != PASS)
			{
				LogPrint( "FAILURE: SID $9 INF $%02X request failed\n", SidReq.Ids[0] );
				return FAIL;
			}

			for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
			{
				if ( GetSid9IptData (EcuIndex, &Sid9Ipt[EcuIndex]) == PASS )
				{
					SetFieldDec (CURRENT_OBDCOND+EcuIndex, Sid9Ipt[EcuIndex].IPT[0]);
					SetFieldDec (CURRENT_IGNCTR+EcuIndex,  Sid9Ipt[EcuIndex].IPT[1]);

					// check when OBDCOND counters increment
					if ( (tObdCondTimestamp[EcuIndex] == 0) &&
					     (Sid9Ipt[EcuIndex].IPT[0] > Test10_9_Sid9Ipt[EcuIndex].IPT[0]) )
					{
						tObdCondTimestamp[EcuIndex] = RunTime;
					}
				}
			}
		}

		//-------------------------------------------
		// Update current PIDs, total engine time
		//-------------------------------------------
		SetFieldDec (SPEED_INDEX, Speed);
		SetFieldDec (TOTAL_DRIVE_TIMER, RunTime);
		if (bRunTimeSupport == FALSE)
			SetFieldDec (RPM_INDEX, RPM);

		//-------------------------------------------
		// Determine phase of dynamic test
		// update screen
		//-------------------------------------------
		bLoop = TRUE;
		while (bLoop)
		{
			bLoop = FALSE;
			switch (TestState)
			{
				case 0:     // wait until idle  (RunTime > 0 or RPM > 450)
					if ( (bRunTimeSupport == TRUE) ? (RunTime > 0) : (RPM > 450) )
					{
						if ( (Speed <= 1) && (tIdleTime < 30) )
						{
							TestState = 1;
							tIdleTime = 0;
							bLoop = TRUE;
						}
						else if ( (Speed >= 25) && (tAtSpeedTime < 300) )
						{
							TestState = 2;
							bLoop = TRUE;
						}
						else if ( (tIdleTime >= 30) && (tAtSpeedTime >= 300) )
						{
							TestState = 4;
							bLoop = TRUE;
						}

						tTempTime = RunTime;
					}
					break;

				case 1:     // 30 seconds continuous time
					if ((Speed <= 1) && ( (bRunTimeSupport == FALSE) ? (RPM > 450) : 1) )
					{
						tIdleTime = min(tIdleTime + RunTime - tTempTime, 30);

						SetFieldDec (IDLE_TIMER, tIdleTime);

						if (tIdleTime >= 30)
						{
							TestState = 0;
							bLoop = TRUE;
						}
					}
					else
					{
						TestState = 0;
						bLoop = TRUE;
					}
					tTempTime = RunTime;
					break;

				case 2:     // 300 seconds cumulative time at Speed >= 25 MPH
					if (Speed >= 25 && ( (bRunTimeSupport == FALSE) ? (RPM > 450) : 1) )
					{
						tAtSpeedTime = min(tAtSpeedTime + RunTime - tTempTime, 300);
						tTempTime = RunTime;

						SetFieldDec (SPEED_25_MPH_TIMER, tAtSpeedTime);

						if (tAtSpeedTime >= 300)
						{
							TestState = 0;
							bLoop = TRUE;
						}
					}
					else
					{
						TestState = 0;
						bLoop = TRUE;
					}
					break;

				case 4:     // 600 seconds cumulative time
					if (RunTime >= 600 && ( (bRunTimeSupport == FALSE) ? (RPM > 450) : 1) )
					{
						tTestCompleteTime = RunTime;
						TestState = 5;
						bLoop = TRUE;
					}
					break;

				case 5:     // check for test pass/fail
					// check if any OBDCOND counters increment too soon or too late
					bFail = FALSE;
					for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
					{
						if ( (Test10_9_Sid9Ipt[EcuIndex].Flags != 0) && (tObdCondTimestamp[EcuIndex] != 0) )
						{
							if ( tObdCondTimestamp[EcuIndex] < (tTestCompleteTime - 20) )
							{
								LogPrint ( "FAILURE: ECU %X  OBDCOND incremented too soon (RUNTM = %u)\n",
								           GetEcuId(EcuIndex),
								           tObdCondTimestamp[EcuIndex] );
								bFail = TRUE;
							}

							if ( tObdCondTimestamp[EcuIndex] > (tTestCompleteTime + 20) )
							{
								LogPrint ( "FAILURE: ECU %X  OBDCOND incremented too late (RUNTM = %u)\n",
								           GetEcuId(EcuIndex),
								           tObdCondTimestamp[EcuIndex]);
								bFail = TRUE;
							}
						}
					}

					if ( bSid9Ipt == 0 )
					{
						LogPrint ("FAILURE: OBDCOND (SID $9 INF $8 and $B) not supported (RUNTM = %u)\n",
						          GetEcuId(EcuIndex), tObdCondTimestamp[EcuIndex]);
						bFail = TRUE;
					}

					if (bFail == TRUE)
					{
						LogPrint("INFORMATION: Idle Time = %d;  Speed Time = %d;  Run Time = %d\n",
						         tIdleTime, tAtSpeedTime, RunTime);
						return FAIL;
					}

					// check for OBDCOND for increment
					for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
					{
						if ( (Test10_9_Sid9Ipt[EcuIndex].Flags != 0) && (tObdCondTimestamp[EcuIndex] == 0) )
						{
							break;      // OBDCOND counter not yet incremented - keep running test
						}
					}

					if (EcuIndex >= gUserNumEcus)
					{
						LogPrint("INFORMATION: Idle Time = %d;  Speed Time = %d;  Run Time = %d\n",
						         tIdleTime, tAtSpeedTime, RunTime);
						return PASS;  // Test complete
					}

					// check for timeout
					if (RunTime >= (tTestCompleteTime + 20) )
					{
						LogPrint("FAILURE: More than 20 seconds has elapsed since the test completed "
						         "and not all the ECUs have incremented OBDCOND\n");
						LogPrint("INFORMATION: Idle Time = %d;  Speed Time = %d;  Run Time = %d\n",
						         tIdleTime, tAtSpeedTime, RunTime);
						return FAIL;
					}
					break;
			} // end switch (TestState)
		} // end while (bLoop)

		//-------------------------------------------
		// Check for ESC key and sleep
		//-------------------------------------------
		do
		{
			if (_kbhit () != 0)
			{
				if (_getch () == 27)    // ESC key
				{
					LogPrint("INFORMATION: Drive Cycle Test aborted by user\n\n");
					LogPrint("INFORMATION: Idle Time = %d;  Speed Time = %d;  Run Time = %d\n",
					         tIdleTime, tAtSpeedTime, RunTime);
					return ABORT;
				}
			}

			tDelayTimeStamp = GetTickCount ();

			Sleep ( min (1000 - (tDelayTimeStamp - t1SecTimer), 50) );

		} while (tDelayTimeStamp - t1SecTimer < 1000);

		t1SecTimer = tDelayTimeStamp;
	}

	LogPrint("FAILURE: Error in Drive Cycle Test\n\n");
	return FAIL;
}
