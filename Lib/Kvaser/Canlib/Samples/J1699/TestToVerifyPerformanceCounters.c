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
** 04/26/04      Added comments: Corrections required for in-use performace
**               counter testing.
** 05/01/04      Renumber all test cases to reflect specification.  This section
**               has been indicated as section 11 in Draft 15.4.
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "j2534.h"
#include "j1699.h"
#include "ScreenOutput.h"

enum IM_Status {Complete=0, Incomplete, NotSupported, Invalid};

enum IM_Status Test11IMStatus[OBD_MAX_ECUS][11];
enum IM_Status CurrentIMStatus[11];

SID1    Test11_Sid1Pid1[OBD_MAX_ECUS];

SID9IPT Test10_9_Sid9Ipt[OBD_MAX_ECUS];
SID9IPT Test11_5_Sid9Ipt[OBD_MAX_ECUS];
SID9IPT Test11_11_Sid9Ipt[OBD_MAX_ECUS];
SID9IPT Test11CurrentDisplayData[OBD_MAX_ECUS];

SID1    PreviousSid1Pid1[OBD_MAX_ECUS];

const char * szIM_Status[] = {"Complete", "Incomplete", "Not Supported", "Invalid"};

BOOL IsIM_ReadinessComplete (SID1 * pSid1);
BOOL EvaluateSid9Ipt (int EcuIndex, int test_stage, BOOL bDisplayErrorMsg);
STATUS RunDynamicTest11 (void);

//-----------------------------------------------------------------------------
// Dynamic Test 11.x screen
//-----------------------------------------------------------------------------
#define COL1            1
#define COL2            25
#define COL3            40
#define COL4            60
#define COL5            65
#define COL6            70
#define COL7            75

#define COL_ECU         (COL1+12)
#define SPACE           9

#define STATUS_ROW      6
#define PRESS_ESC_ROW   23

#define STRING_WIDTH    13
#define NUMERIC_WIDTH   5
#define HEX_WIDTH       4
#define ECU_WIDTH       8


StaticTextElement  _string_elements11_INF840[] =
{
	// ECU List
	{"ECU List:",   COL1, 2},
	{"ECU Status:", COL1, 3},
	{"1", COL_ECU + 0 * SPACE, 1},
	{"2", COL_ECU + 1 * SPACE, 1},
	{"3", COL_ECU + 2 * SPACE, 1},
	{"4", COL_ECU + 3 * SPACE, 1},
	{"5", COL_ECU + 4 * SPACE, 1},
	{"6", COL_ECU + 5 * SPACE, 1},
	{"7", COL_ECU + 6 * SPACE, 1},
	{"8", COL_ECU + 7 * SPACE, 1},

	// column 1
	{"I/M STATUS",                COL1, STATUS_ROW+0},
	{"Oxygen Sensor Response",    COL1, STATUS_ROW+1},
	{"Oxygen Sensor Heater",      COL1, STATUS_ROW+2},
	{"Catalyst Monitor",          COL1, STATUS_ROW+3},
	{"Catalyst Heater",           COL1, STATUS_ROW+4},
	{"A/C",                       COL1, STATUS_ROW+5},
	{"Evaportive Emissions",      COL1, STATUS_ROW+6},
	{"EGR",                       COL1, STATUS_ROW+7},
	{"AIR",                       COL1, STATUS_ROW+8},
	{"Fuel Trim",                 COL1, STATUS_ROW+9},
	{"Misfire",                   COL1, STATUS_ROW+10},
	{"Comp / Comp",               COL1, STATUS_ROW+11},

	// column 2
	{"RATE BASED COUNTER",        COL3, STATUS_ROW+0},
	{"OBD Monitoring Conditions", COL3, STATUS_ROW+1},
	{"Ignition Counter",          COL3, STATUS_ROW+2},

	{"Initial",                   COL4, STATUS_ROW+4},
	{"Current",                   COL6, STATUS_ROW+4},

	{"N",                         COL4, STATUS_ROW+5},
	{"D",                         COL5, STATUS_ROW+5},

	{"N",                         COL6, STATUS_ROW+5},
	{"D",                         COL7, STATUS_ROW+5},

	// column 2 - two values
	{"Catalyst B1",               COL3, STATUS_ROW+6},
	{"Catalyst B2",               COL3, STATUS_ROW+7},
	{"Oxygen Sensor B1",          COL3, STATUS_ROW+8},
	{"Oxygen Sensor B2",          COL3, STATUS_ROW+9},
	{"EGR",                       COL3, STATUS_ROW+10},
	{"AIR",                       COL3, STATUS_ROW+11},
	{"EVAP",                      COL3, STATUS_ROW+12},
	{"Sec O2 Sensor B1",          COL3, STATUS_ROW+13},
	{"Sec O2 Sensor B2",          COL3, STATUS_ROW+14},

	// misc
	{"Press ESC to exit, a number to change ECU display, or F to FAIL", COL1, PRESS_ESC_ROW},
	{"", 0, PRESS_ESC_ROW+1}
};

const int _num_string_elements11_INF840 = sizeof(_string_elements11_INF840)/sizeof(_string_elements11_INF840[0]);

StaticTextElement  _string_elements11_INFB32[] =
{
	// ECU List
	{"ECU List:",   COL1, 2},
	{"ECU Status:", COL1, 3},
	{"1", COL_ECU + 0 * SPACE, 1},
	{"2", COL_ECU + 1 * SPACE, 1},
	{"3", COL_ECU + 2 * SPACE, 1},
	{"4", COL_ECU + 3 * SPACE, 1},
	{"5", COL_ECU + 4 * SPACE, 1},
	{"6", COL_ECU + 5 * SPACE, 1},
	{"7", COL_ECU + 6 * SPACE, 1},
	{"8", COL_ECU + 7 * SPACE, 1},

	// column 1
	{"I/M STATUS",                COL1, STATUS_ROW+0},
	{"EGS",                       COL1, STATUS_ROW+1},
	{"PM",                        COL1, STATUS_ROW+2},
	{"NMHC Catalyst",             COL1, STATUS_ROW+3},
	{"NOx Aftertreatment",        COL1, STATUS_ROW+4},
	{"iso/sae reserved",          COL1, STATUS_ROW+5},
	{"iso/sae reserved",          COL1, STATUS_ROW+6},
	{"EGR",                       COL1, STATUS_ROW+7},
	{"BP",                        COL1, STATUS_ROW+8},
	{"Fuel Trim",                 COL1, STATUS_ROW+9},
	{"Misfire",                   COL1, STATUS_ROW+10},
	{"Comp / Comp",               COL1, STATUS_ROW+11},

	// column 2
	{"RATE BASED COUNTER",        COL3, STATUS_ROW+0},
	{"OBD Monitoring Conditions", COL3, STATUS_ROW+1},
	{"Ignition Counter",          COL3, STATUS_ROW+2},

	{"Initial",                   COL4, STATUS_ROW+4},
	{"Current",                   COL6, STATUS_ROW+4},

	{"N",                         COL4, STATUS_ROW+5},
	{"D",                         COL5, STATUS_ROW+5},

	{"N",                         COL6, STATUS_ROW+5},
	{"D",                         COL7, STATUS_ROW+5},

	// column 2 - two values
	{"HCCAT",                     COL3, STATUS_ROW+6},
	{"NCAT",                      COL3, STATUS_ROW+7},
	{"NADS",                      COL3, STATUS_ROW+8},
	{"PM",                        COL3, STATUS_ROW+9},
	{"EGS",                       COL3, STATUS_ROW+10},
	{"EGR",                       COL3, STATUS_ROW+11},
	{"BP",                        COL3, STATUS_ROW+12},

	// misc
	{"Press ESC to exit, a number to change ECU display, or F to FAIL", COL1, PRESS_ESC_ROW},
	{"", 0, PRESS_ESC_ROW+1}
};

const int _num_string_elements11_INFB32 = sizeof(_string_elements11_INFB32)/sizeof(_string_elements11_INFB32[0]);

DynamicValueElement  _dynamic_elements11[] =
{
	// ECUs
	{COL_ECU + 0 * SPACE, 2, ECU_WIDTH}, // ECU 1
	{COL_ECU + 1 * SPACE, 2, ECU_WIDTH}, // ECU 2
	{COL_ECU + 2 * SPACE, 2, ECU_WIDTH}, // ECU 3
	{COL_ECU + 3 * SPACE, 2, ECU_WIDTH}, // ECU 4
	{COL_ECU + 4 * SPACE, 2, ECU_WIDTH}, // ECU 5
	{COL_ECU + 5 * SPACE, 2, ECU_WIDTH}, // ECU 6
	{COL_ECU + 6 * SPACE, 2, ECU_WIDTH}, // ECU 7
	{COL_ECU + 7 * SPACE, 2, ECU_WIDTH}, // ECU 8

	// ECU Done
	{COL_ECU + 0 * SPACE, 3, HEX_WIDTH}, // ECU Status 1
	{COL_ECU + 1 * SPACE, 3, HEX_WIDTH}, // ECU Status 2
	{COL_ECU + 2 * SPACE, 3, HEX_WIDTH}, // ECU Status 3
	{COL_ECU + 3 * SPACE, 3, HEX_WIDTH}, // ECU Status 4
	{COL_ECU + 4 * SPACE, 3, HEX_WIDTH}, // ECU Status 5
	{COL_ECU + 5 * SPACE, 3, HEX_WIDTH}, // ECU Status 6
	{COL_ECU + 6 * SPACE, 3, HEX_WIDTH}, // ECU Status 7
	{COL_ECU + 7 * SPACE, 3, HEX_WIDTH}, // ECU Status 8

	// column 1
	{COL2, STATUS_ROW+1,  STRING_WIDTH}, // Oxygen Sensor Response
	{COL2, STATUS_ROW+2,  STRING_WIDTH}, // Oxygen Sensor Heater
	{COL2, STATUS_ROW+3,  STRING_WIDTH}, // Catalyst Monitor
	{COL2, STATUS_ROW+4,  STRING_WIDTH}, // Catalyst Heater
	{COL2, STATUS_ROW+5,  STRING_WIDTH}, // A/C
	{COL2, STATUS_ROW+6,  STRING_WIDTH}, // Evaportive Emissions
	{COL2, STATUS_ROW+7,  STRING_WIDTH}, // EGR
	{COL2, STATUS_ROW+8,  STRING_WIDTH}, // AIR
	{COL2, STATUS_ROW+9,  STRING_WIDTH}, // Fuel Trim
	{COL2, STATUS_ROW+10, STRING_WIDTH}, // Misfire
	{COL2, STATUS_ROW+11, STRING_WIDTH}, // Comp / Comp

	// column 2
	{COL6, STATUS_ROW+1, NUMERIC_WIDTH}, // OBD Monitoring Conditions
	{COL6, STATUS_ROW+2, NUMERIC_WIDTH}, // Ignition Counter

	// column 2 - initial values
	{COL4, STATUS_ROW+6, NUMERIC_WIDTH},    // Catalyst B1 N / NMHC Catalyst N
	{COL5, STATUS_ROW+6, NUMERIC_WIDTH},    // Catalyst B1 D / NMHC Catalyst D

	{COL4, STATUS_ROW+7, NUMERIC_WIDTH},    // Catalyst B2 N / NOx Aftertreatment N
	{COL5, STATUS_ROW+7, NUMERIC_WIDTH},    // Catalyst B2 D / NOx Aftertreatment D

	{COL4, STATUS_ROW+8, NUMERIC_WIDTH},    // Oxygen Sensor B1 N / NAD N
	{COL5, STATUS_ROW+8, NUMERIC_WIDTH},    // Oxygen Sensor B1 D / NAD N

	{COL4, STATUS_ROW+9, NUMERIC_WIDTH},    // Oxygen Sensor B2 N / PM Filter N
	{COL5, STATUS_ROW+9, NUMERIC_WIDTH},    // Oxygen Sensor B2 D / PM Filter D

	{COL4, STATUS_ROW+10, NUMERIC_WIDTH},   // EGR N / EGS N
	{COL5, STATUS_ROW+10, NUMERIC_WIDTH},   // EGR D / EGS D

	{COL4, STATUS_ROW+11, NUMERIC_WIDTH},   // AIR N / EGR N
	{COL5, STATUS_ROW+11, NUMERIC_WIDTH},   // AIR D / EGR D

	{COL4, STATUS_ROW+12, NUMERIC_WIDTH},   // EVAP N / Boost Pressure
	{COL5, STATUS_ROW+12, NUMERIC_WIDTH},   // EVAP D / Boost Pressure

	{COL4, STATUS_ROW+13, NUMERIC_WIDTH},   // Secondary Oxygen Sensor B1 N
	{COL5, STATUS_ROW+13, NUMERIC_WIDTH},   // Secondary Oxygen Sensor B1 D

	{COL4, STATUS_ROW+14, NUMERIC_WIDTH},   // Secondary Oxygen Sensor B2 N
	{COL5, STATUS_ROW+14, NUMERIC_WIDTH},   // Secondary Oxygen Sensor B2 D

	// column 2 - current values
	{COL6, STATUS_ROW+6, NUMERIC_WIDTH},    // Catalyst B1 N / NMHC Catalyst N
	{COL7, STATUS_ROW+6, NUMERIC_WIDTH},    // Catalyst B1 D / NMHC Catalyst D

	{COL6, STATUS_ROW+7, NUMERIC_WIDTH},    // Catalyst B2 N / NOx Aftertreatment N
	{COL7, STATUS_ROW+7, NUMERIC_WIDTH},    // Catalyst B2 D / NOx Aftertreatment D

	{COL6, STATUS_ROW+8, NUMERIC_WIDTH},    // Oxygen Sensor B1 N / NAD N
	{COL7, STATUS_ROW+8, NUMERIC_WIDTH},    // Oxygen Sensor B1 D / NAD N

	{COL6, STATUS_ROW+9, NUMERIC_WIDTH},    // Oxygen Sensor B2 N / PM Filter N
	{COL7, STATUS_ROW+9, NUMERIC_WIDTH},    // Oxygen Sensor B2 D / PM Filter D

	{COL6, STATUS_ROW+10, NUMERIC_WIDTH},   // EGR N / EGS N
	{COL7, STATUS_ROW+10, NUMERIC_WIDTH},   // EGR D / EGS D

	{COL6, STATUS_ROW+11, NUMERIC_WIDTH},   // AIR N / EGR N
	{COL7, STATUS_ROW+11, NUMERIC_WIDTH},   // AIR D / EGR D

	{COL6, STATUS_ROW+12, NUMERIC_WIDTH},   // EVAP N / Boost Pressure
	{COL7, STATUS_ROW+12, NUMERIC_WIDTH},   // EVAP D / Boost Pressure

	{COL6, STATUS_ROW+13, NUMERIC_WIDTH},   // Secondary Oxygen Sensor B1 N
	{COL7, STATUS_ROW+13, NUMERIC_WIDTH},   // Secondary Oxygen Sensor B1 D

	{COL6, STATUS_ROW+14, NUMERIC_WIDTH},   // Secondary Oxygen Sensor B2 N
	{COL7, STATUS_ROW+14, NUMERIC_WIDTH}    // Secondary Oxygen Sensor B2 D
};

const int _num_dynamic_elements11 = sizeof(_dynamic_elements11)/sizeof(_dynamic_elements11[0]);

#define ECU_ID_INDEX                0
#define ECU_STATUS_INDEX            8

#define IM_READINESS_INDEX          16
// O2_SENSOR_RESP_INDEX        16
// O2_SENSOR_HEATER_INDEX      17
// CATALYST_MONITOR_INDEX      18
// CATALYST_HEATER_INDEX       19
// AIR_COND_INDEX              20
// EVAP_EMMISION_INDEX         21
// EGR_INDEX                   22
// AIR_INDEX                   23
// FUEL_TRIM_INDEX             24
// MISFIRE_INDEX               25
// COMP_COMP_INDEX             26

#define OBD_MONITOR_COND_INDEX      27
#define IGNITION_COUNTER_INDEX      28

#define IPT_INIT_INDEX              29
// CATCOMP1   /  HCCATCOMP          29
// CATCOND1   /  HCCATCOND
// CATCOMP2   /  NCATCOMP           31
// CATCOND2   /  NCATCOND
// O2SCOMP1   /  NADSCOMP           33
// O2SCOND1   /  NADSCOND
// O2SCOMP2   /  PMCOMP             35
// O2SCOND2   /  PMCOND
// EGRCOMP    /  EGSCOMP            37
// EGRCOND    /  EGSCOND
// AIRCOMP    /  EGRCOMP            39
// AIRCOND    /  EGRCOND
// EVAPCOMP   /  BPCOMP             41
// EVAPCOND   /  BPCOND
// SO2SCOMP1                        43
// SO2SCOND1
// SO2SCOMP2                        45
// SO2SCOND2

#define IPT_CUR_INDEX               47
// CATCOMP1   /  HCCATCOMP          47
// CATCOND1   /  HCCATCOND
// CATCOMP2   /  NCATCOMP           49
// CATCOND2   /  NCATCOND
// O2SCOMP1   /  NADSCOMP           51
// O2SCOND1   /  NADSCOND
// O2SCOMP2   /  PMCOMP             53
// O2SCOND2   /  PMCOND
// EGRCOMP    /  EGSCOMP            55
// EGRCOND    /  EGSCOND
// AIRCOMP    /  EGRCOMP            57
// AIRCOND    /  EGRCOND
// EVAPCOMP   /  BPCOMP             59
// EVAPCOND   /  BPCOND
// SO2SCOMP1                        61
// SO2SCOND1
// SO2SCOMP2                        63
// SO2SCOND2


#define SetFieldDec(index,val)  (update_screen_dec(_dynamic_elements11,_num_dynamic_elements11,index,val))
#define SetFieldHex(index,val)  (update_screen_hex(_dynamic_elements11,_num_dynamic_elements11,index,val))
#define SetFieldText(index,val) (update_screen_text(_dynamic_elements11,_num_dynamic_elements11,index,val))

/*
*******************************************************************************
** TestToVerifyPerformanceCounters -
** Function to run test to verify performance counters
*******************************************************************************
*/
STATUS TestToVerifyPerformanceCounters(void)
{
	unsigned int  EcuIndex;
	unsigned int  IMReadinessIndex;
	STATUS        ret_code;
	BOOL          bRunTest11_2;

	SID_REQ       SidReq;
	SID1        * pSid1;

	BOOL          bSubTestFailed = FALSE;

	gOBDTestFailed = FALSE;

	// initialize arrays
	memset (Test11_Sid1Pid1, 0x00, sizeof(Test11_Sid1Pid1));
	memset (PreviousSid1Pid1, 0x00, sizeof(PreviousSid1Pid1));

	memset (Test11_5_Sid9Ipt, 0x00, sizeof(Test11_5_Sid9Ipt));
	memset (Test11_11_Sid9Ipt, 0x00, sizeof(Test11_11_Sid9Ipt));
	memset (Test11CurrentDisplayData, 0x00, sizeof(Test11CurrentDisplayData));

	for (IMReadinessIndex = 0; IMReadinessIndex < 11; IMReadinessIndex++)
	{
		for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
		{
			Test11IMStatus[EcuIndex][IMReadinessIndex] = NotSupported;
		}
		CurrentIMStatus[IMReadinessIndex] = Invalid;
	}


	LogPrint ("\n\n**** Test 11.1 (Verify performance counters) ****\n\n");

	if (gOBDEngineRunning == FALSE)
	{
		LogUserPrompt ("Turn ignition to crank position and start engine.\n\n"
		               "Press enter to continue.", ENTER_PROMPT);
		gOBDEngineRunning = TRUE;
	}

	// Test 11.1
	if (VerifyLinkActive () == FAIL)
	{
		DisconnectProtocol ();

		if (ConnectProtocol () == FAIL)
		{
			LogPrint ("FAILURE: ConnectProtocol() failed\n");
			LogPrint ("**** Test 11.1 FAILED ****\n");
			return FAIL;
		}
	}

	// Test 11.1.2 - SID 1 PID 1 request
	SidReq.SID    = 1;
	SidReq.NumIds = 1;
	SidReq.Ids[0] = 1;

	if (SidRequest (&SidReq, SID_REQ_NORMAL) != PASS)
	{
		LogPrint ("FAILURE: SID 1 PID 1 request failed\n");
		LogPrint ("**** Test 11.1 FAILED ****\n");
		return FAIL;
	}

	/* check if need to run test 11.2 */
	bRunTest11_2 = FALSE;
	for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
	{
		if (IsSid1PidSupported (EcuIndex, 1) == TRUE)
		{
			pSid1 = (SID1 *)&gOBDResponse[EcuIndex].Sid1Pid[0];

			/* save for EvaluateSid9Ipt () */
			Test11_Sid1Pid1[EcuIndex].PID = pSid1->PID;
			Test11_Sid1Pid1[EcuIndex].Data[0] = pSid1->Data[0];
			Test11_Sid1Pid1[EcuIndex].Data[1] = pSid1->Data[1];
			Test11_Sid1Pid1[EcuIndex].Data[2] = pSid1->Data[2];
			Test11_Sid1Pid1[EcuIndex].Data[3] = pSid1->Data[3];

			if (IsIM_ReadinessComplete (pSid1) == FALSE)
				bRunTest11_2 = TRUE;
		}
	}

	if ( LogSid9Ipt () != PASS )
	{
	    gOBDTestFailed = TRUE;
	}

	/* Retrieve Sid 9 IPT from the first run of test 10.9 */
	ReadSid9IptFromLogFile ("**** Test 10.9 ****",
	                        "**** Test 10.9",
	                        Test10_9_Sid9Ipt);

	if ( bRunTest11_2 == FALSE )
	{
		for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
		{
			GetSid9IptData ( EcuIndex, &Test11_5_Sid9Ipt[EcuIndex] );

			if ( EvaluateSid9Ipt ( EcuIndex, 5, FALSE ) == FALSE )
			{
				bRunTest11_2 = TRUE;
				break;
			}
		}
	}


	if ( gOBDTestFailed == TRUE )
	{
		LogPrint ("**** Test 11.1 FAILED ****\n");
		if ( TestContinue( "Errors detected.  Do you wish to continue?\n" ) == 'N' )
		{
			return FAIL;
		}
	}
	else
	{
		LogPrint ("**** Test 11.1 PASSED ****\n");
	}


	// Test 11.2
	LogPrint ("\n\n**** Test 11.2 ****\n");

	if (bRunTest11_2 == TRUE)
	{
		// tester-present message should already be active

		LogPrint ("INSTRUCTIONS:\n"
		          "Drive the vehicle in the manufactured-specified manner to complete all the\n"
		          "OBD monitors required to set all the supported I/M Readiness bits to a \"Ready\"\n"
		          "condition. The vehcile may have to \"soak\" with the ignition off (engine off).\n"
		          "Allow vehicle to soak according to the manufracturer-specified conditions in\n"
		          "order to run any engine-off diagnositics and/or prepare the vehicle for any \n"
		          "engine-running diagnostics on the next drive cycle that requires an engine-off\n"
		          "soak period.\n\n");

		if (LogUserPrompt ("Would you like to use the J1699 software as a monitor to view\n"
		                   "the status of the I/M Readiness bits?\n"
		                   "(Enter 'N' to exit the software if you are going to turn the vehicle off\n"
		                   "or don't need a monitor. You can return to this point, at any time,\n"
		                   "by re-starting the J1699 software and selecting 'Dynamic Tests'.)\n", YES_NO_PROMPT) != 'Y')
		{
			LogPrint ("**** Test 11.2 INCOMPLETE ****\n");
			return ABORT;
		}

		// stop tester-present message
		StopPeriodicMsg (TRUE);
		Sleep (gOBDRequestDelay);
		LogPrint ("INFORMATION: Stop periodic messages\n");

		ErrorFlags (ER_BYPASS_USER_PROMPT | ER_CONTINUE);
		ErrorCount();   /* clear error count */

		gSuspendLogOutput = TRUE;
		gSuspendScreenOutput = TRUE;
		ret_code = RunDynamicTest11 ();
		gSuspendScreenOutput = FALSE;
		gSuspendLogOutput = FALSE;

		ErrorFlags (0);

		// re-start tester-present message
		StartPeriodicMsg ();

		if (ret_code == ABORT)
		{
			if (ErrorCount() != 0)
			{
				LogPrint ("FAILURE: Errors detected.\n");
			}

			LogPrint ("**** Test 11.2 INCOMPLETE ****\n");
			return ret_code;
		}

		if ((ErrorCount() == 0) && (ret_code == PASS))
		{
			LogPrint ("**** Test 11.2 PASSED ****\n");
		}
		else
		{
			LogPrint("**** Test 11.2 FAILED ****\n");
			if ( TestContinue( "Errors detected.  Do you wish to continue?\n" ) == 'N' )
			{
				return FAIL;
			}
		}
	}
	else
	{
		LogPrint ("**** Test 11.2 PASSED ****\n");
	}


	// Test 11.3
	LogPrint ("\n\n**** Test 11.3 ****\n");

	gOBDIMDriveCycle = TRUE;

	if (VerifyIM_Ready () != PASS)
	{
		LogPrint ("**** Test 11.3 FAILED ****\n");
		if ( TestContinue( "IM Readiness Verification Failed.  Continue?\n" ) == 'N' )
		{
			return FAIL;
		}
	}
	else
	{
		LogPrint ("**** Test 11.3 PASSED ****\n");
	}


	// Test 11.4
	LogPrint ("\n\n**** Test 11.4 ****\n");

	if (VerifyMonitorTestSupportAndResults () != PASS)
	{
		LogPrint ("**** Test 11.4 FAILED ****\n");
		if ( TestContinue( "Monitor test support/results failed. Continue?" ) == 'N' )
		{
			return FAIL;
		}
	}
	else
	{
		LogPrint ("**** Test 11.4 PASSED ****\n");
	}


	// Test 11.5
	LogPrint ("\n\n**** Test 11.5 ****\n");

	bSubTestFailed = FALSE;

	if ( LogSid9Ipt () != PASS )
	{
		bSubTestFailed = TRUE;
		LogPrint ( "FAILURE: SID 9 IPT data missing\n" );
	}
	else
	{
		for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
		{
			GetSid9IptData ( EcuIndex, &Test11_5_Sid9Ipt[EcuIndex] );

			if ( EvaluateSid9Ipt ( EcuIndex, 5, TRUE ) == FALSE )
			{
				bSubTestFailed = TRUE;
			}
		}
	}

	if ( bSubTestFailed == TRUE )
	{
		if ( TestContinue( "Errors detected.  Do you wish to continue?\n" ) == 'N' )
		{
			LogPrint ( "**** Test 11.5 FAILED ****\n" );
			return FAIL;
		}
	}

	LogUserPrompt ("Turn ignition off (engine off) for 60 seconds.\n"
	               "Press enter to continue.\n", ENTER_PROMPT);

	DisconnectProtocol ();

	LogUserPrompt ("Turn key on without cranking or starting engine.\n"
	               "Press enter to continue.\n", ENTER_PROMPT);

	if ( bSubTestFailed == TRUE )
	{
		LogPrint ( "**** Test 11.5 FAILED ****\n" );
	}
	else
	{
		LogPrint ("**** Test 11.5 PASSED ****\n");
	}


	// Test 11.6
	LogPrint ("\n\n**** Test 11.6 ****\n");

	if (ConnectProtocol () == FAIL)
	{
		LogPrint ("FAILURE: ConnectProtocol() failed\n");
		LogPrint ("**** Test 11.6 FAILED ****\n");
		return FAIL;
	}
	LogPrint ("**** Test 11.6 PASSED ****\n");


	// Test 11.7
	LogPrint ("\n\n**** Test 11.7 ****\n");

	if (VerifyIM_Ready () != PASS)
	{
		LogPrint ("**** Test 11.7 FAILED ****\n");
		if ( TestContinue( "IM Readiness Verification Failed.  Continue?\n" ) == 'N' )
		{
			return FAIL;
		}
	}
	else
	{
		LogPrint ("**** Test 11.7 PASSED ****\n");
	}


	// Test 11.8
	LogPrint ("\n\n**** Test 11.8 ****\n");

	if (VerifyDTCStoredData () != PASS)
	{
		LogPrint ("**** Test 11.8 FAILED ****\n");
		return FAIL;
	}
	else
	{
		LogPrint ("**** Test 11.8 PASSED ****\n");
	}


	// Test 11.9
	LogPrint ("\n\n**** Test 11.9 ****\n");

	if (VerifyDTCPendingData () != PASS)
	{
		LogPrint ("**** Test 11.9 FAILED ****\n");
		return FAIL;
	}
	else
	{
		LogPrint ("**** Test 11.9 PASSED ****\n");
	}


	// Test 11.10
	LogPrint ("\n\n**** Test 11.10 ****\n");

	if (ClearCodes () != PASS)
	{
		LogPrint ("**** Test 11.10 FAILED ****\n");
		return FAIL;
	}
	else
	{
		LogPrint ("**** Test 11.10 PASSED ****\n");
	}


	// Test 11.11
	LogPrint ("\n\n**** Test 11.11 ****\n");

	if ( LogSid9Ipt () != PASS)
	{
		bSubTestFailed = TRUE;
		LogPrint ("FAILURE: SID 9 IPT data missing\n");
	}

	else
	{
		for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
		{
			GetSid9IptData ( EcuIndex, &Test11_11_Sid9Ipt[EcuIndex] );

			if ( EvaluateSid9Ipt ( EcuIndex, 11, TRUE ) == FALSE )
			{
				bSubTestFailed = TRUE;
			}
		}
	}

	if ( bSubTestFailed == TRUE )
	{
		LogPrint ("**** Test 11.11 FAILED ****\n");
		if ( TestContinue( "Errors detected.  Do you wish to continue?\n" ) == 'N' )
		{
			return FAIL;
		}
	}
	else
	{
		LogPrint ("**** Test 11.11 PASSED ****\n");
	}

	return ( gOBDTestFailed != TRUE ) ? PASS : FAIL;
}

/*
*******************************************************************************
** IsIM_ReadinessComplete
**
**  supported bits :: 1 -> supported,    0 -> not supported
**
**    status  bits :: 1 -> not complete, 0 -> complete (or not applicable)
**
*******************************************************************************
*/
BOOL IsIM_ReadinessComplete (SID1 * pSid1)
{
	if (pSid1->PID != 1)
		return FALSE;

	//     supported bits    status bits
	if ( ((pSid1->Data[1] & (pSid1->Data[1] >> 4)) & 0x07) != 0)
		return FALSE;

	//    supported bits   status bits
	if ( (pSid1->Data[2] & pSid1->Data[3]) != 0)
		return FALSE;

	return TRUE;
}


//*****************************************************************************
// EvaluateSid9Ipt
//*****************************************************************************
BOOL EvaluateSid9Ipt ( int EcuIndex, int test_stage, BOOL bDisplayErrorMsg )
{
	int IptIndex;
	int count;
	int count1;
	int count2;
	int count3;
	int count4;

	int test_passed = TRUE;


	// Test 11.5
	if (test_stage == 5)
	{
		// IGNCNTR must be >= OBDCOND
		if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_IGNCNTR_INDEX] < Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_OBDCOND_INDEX] )
		{
			if ( bDisplayErrorMsg == TRUE )
				LogPrint ( "FAILURE: ECU %X  IGNCNTR less than OBDCOND\n", GetEcuId(EcuIndex) );
			test_passed = FALSE;
		}

		// OBDCOND must be >= other monitor condition counters
		for ( IptIndex = 3; IptIndex < Test11_5_Sid9Ipt[EcuIndex].NODI; IptIndex += 2 )
		{
			if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_OBDCOND_INDEX] < Test11_5_Sid9Ipt[EcuIndex].IPT[IptIndex] )
			{
				if ( bDisplayErrorMsg == TRUE )
					LogPrint ("FAILURE: ECU %X  OBDCOND not greater than all other monitor condition counters\n", GetEcuId(EcuIndex) );
				test_passed = FALSE;
			}
		}

		// IGNCNTR must be greater than the value in test 10.9
		if ( Test11_5_Sid9Ipt[EcuIndex].Flags != 0 &&
			 Test10_9_Sid9Ipt[EcuIndex].Flags != 0 )
		{
			count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_IGNCNTR_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_IGNCNTR_INDEX];
			if ( count < 1 )
			{
				if ( bDisplayErrorMsg == TRUE )
					LogPrint ("FAILURE: ECU %X  IGNCNTR incremented less than 1 since Test 10.9\n", GetEcuId(EcuIndex) );
				test_passed = FALSE;
			}
		}

		// OBDCOND must have incremented by at least 1 since test 10.9
		if ( Test11_5_Sid9Ipt[EcuIndex].Flags != 0 &&
			 Test10_9_Sid9Ipt[EcuIndex].Flags != 0 )
		{
			count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_OBDCOND_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_OBDCOND_INDEX];
			if ( count < 1 )
			{
				if ( bDisplayErrorMsg == TRUE )
					LogPrint ("FAILURE: ECU %X  OBDCOND incremented less than 1 since Test 10.9\n", GetEcuId(EcuIndex) );
				test_passed = FALSE;
			}
		}

		// if this vehicle supports SID 9 INF 8
		if ( Test11_5_Sid9Ipt[EcuIndex].INF == 0x08 )
		{
			// if CAT monitoring not supported (CAT_SUP and HCAT_SUP == 0), must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x03) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_CATCOMP1_INDEX] != 0 ||  // CATCOMP1
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_CATCOND1_INDEX] != 0 ||  // CATCOND1
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_CATCOMP2_INDEX] != 0 ||  // CATCOMP2
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_CATCOND2_INDEX] != 0 )   // CATCOND2
				{
					if (bDisplayErrorMsg == TRUE)
						LogPrint ("FAILURE: ECU %X  CATCOMP1, CATCOMP2, CATCOND1, CATCOND2 not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}
			// if CAT monitoring is supported, at least one bank must have increased
			else
			{
				count1 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_CATCOMP1_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_CATCOMP1_INDEX];
				count2 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_CATCOND1_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_CATCOND1_INDEX];
				count3 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_CATCOMP2_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_CATCOMP2_INDEX];
				count4 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_CATCOND2_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_CATCOND2_INDEX];

				if ( (count1 < 1 || count2 < 1) && (count3 < 1 || count4 < 1) )
				{
					if (bDisplayErrorMsg == TRUE)
						LogPrint ("FAILURE: ECU %X  Neither bank of the CAT monitor incremented\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if EVAP monitoring not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x04) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_EVAPCOMP_INDEX] != 0 || // EVAPCOMP
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_EVAPCOND_INDEX] != 0 )  // EVAPCOND
				{
					if (bDisplayErrorMsg == TRUE)
						LogPrint ("FAILURE: ECU %X  EVAPCOMP and EVAPCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}
			// if EVAP monitor supported, EVAPCOMP, EVAPCOND must have changed since test 10.9
			else
			{
				count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_EVAPCOMP_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_EVAPCOMP_INDEX];
				if ( count == 0 )
				{
					if (bDisplayErrorMsg == TRUE)
						LogPrint ("FAILURE: ECU %X  EVAPCOMP unchanged since Test 10.9\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}

				count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_EVAPCOND_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_EVAPCOND_INDEX];
				if ( count == 0 )
				{
					if (bDisplayErrorMsg == TRUE)
						LogPrint ("FAILURE: ECU %X  EVAPCOND unchanged since Test 10.9\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if AIR monitoring not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x08) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_AIRCOMP_INDEX] != 0 || // AIRCOMP
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_AIRCOND_INDEX] != 0 )  // AIRCOND
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  AIRCOMP and AIRCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}
			// if Air monitor supported, AIRCOMP, AIRCOND must have changed since test 10.9
			else
			{
				count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_AIRCOMP_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_AIRCOMP_INDEX];
				if ( count == 0 )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  AIRCOMP unchanged since Test 10.9\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}

				count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_AIRCOND_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_AIRCOND_INDEX];
				if ( count == 0 )
				{
					if (bDisplayErrorMsg == TRUE)
						LogPrint ("FAILURE: ECU %X  AIRCOND unchanged since Test 10.9\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if O2 monitoring not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x20) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_O2COMP1_INDEX] != 0 ||  // O2COMP1
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_O2COND1_INDEX] != 0 ||  // O2COND1
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_O2COMP2_INDEX] != 0 ||  // O2COMP2
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_O2COND2_INDEX] != 0 )   // O2COND2
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  O2COMP1, O2COMP2, O2COND1, O2COND2 not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}
			// if O2 monitoring is supported, at least one bank must have increased
			else
			{
				count1 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_O2COMP1_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_O2COMP1_INDEX];
				count2 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_O2COND1_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_O2COND1_INDEX];
				count3 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_O2COMP2_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_O2COMP2_INDEX];
				count4 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_O2COND2_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_O2COND2_INDEX];

				if ( (count1 < 1 || count2 < 1) && (count3 < 1 || count4 < 1) )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  Neither bank of the O2 monitor incremented\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if EGR monitor not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x80) == 0 )
			{
				 if ( gModelYear >= 2010 )
				{
					if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_INF8_EGRCOMP_INDEX] != 0 || // EGRCOMP
						 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_INF8_EGRCOND_INDEX] != 0 )  // EGRCOND
					{
						if (bDisplayErrorMsg == TRUE)
							LogPrint ("FAILURE: ECU %X  EGRCOMP and EGRCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
						test_passed = FALSE;
					}
				}
			}
			// if EGR monitor supported, EGRCOMP, EGRCOND must have changed since test 10.9
			else
			{
				count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_INF8_EGRCOMP_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_INF8_EGRCOMP_INDEX];
				if ( count == 0 )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ( "FAILURE: ECU %X  EGRCOMP unchanged since Test 10.9\n" , GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}

				count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_INF8_EGRCOND_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_INF8_EGRCOND_INDEX];
				if ( count == 0 )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  EGRCOND unchanged since Test 10.9\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}
		} // end if this vehicle supports SID 9 INF 8

		// if this vehicle supports SID 9 INF B
		else if ( Test11_5_Sid9Ipt[EcuIndex].INF == 0x0B )
		{
			// if HCAT monitoring not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x01) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_HCCATCOMP_INDEX] != 0 ||  // HCCATCOMP
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_HCCATCOND_INDEX] != 0 )   // HCCATCOND
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  HCCATCOMP, HCCATCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if NCAT monitoring not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x02) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_NCATCOMP_INDEX] != 0 ||  // NCATCOMP
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_NCATCOND_INDEX] != 0 ||  // NCATCOND
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_NADSCOMP_INDEX] != 0 ||  // NADSCOMP
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_NADSCOND_INDEX] != 0 )   // NADSCOND
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  NCATCOMP, NADSCOMP, NCATCOND, NADSCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}
			// if NCAT monitoring is supported, at least one bank must have increased
			else
			{
				count1 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_NCATCOMP_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_NCATCOMP_INDEX];
				count2 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_NCATCOND_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_NCATCOND_INDEX];
				count3 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_NADSCOMP_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_NADSCOMP_INDEX];
				count4 = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_NADSCOND_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_NADSCOND_INDEX];

				if ( (count1 < 1 || count2 < 1) && (count3 < 1 || count4 < 1) )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  Neither set of the NCAT monitor incremented\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if BP monitoring not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x08) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_BPCOMP_INDEX] != 0 ||   // BPCOMP
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_BPCOND_INDEX] != 0 )    // BPCOND
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  BPCOMP and BPCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if EGS monitoring not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x20) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_EGSCOMP_INDEX] != 0 || // EGSCOMP
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_EGSCOND_INDEX] != 0 )  // EGSCOMP
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  EGSCOMP and EGSCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if PM monitoring not supported, must be zero
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x40) == 0 )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_PMCOMP_INDEX] != 0 ||  // PMCOMP
					 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_PMCOND_INDEX] != 0 )   // PMCOND
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  PMCOMP and PMCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// if EGR monitor not supported, must be zero MY 2010 and later
			if ( (Test11_Sid1Pid1[EcuIndex].Data[2] & 0x80) == 0 )
			{
				if ( gModelYear >= 2010 )
				{
					if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_INFB_EGRCOMP_INDEX] != 0 || // EGRCOMP
						 Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_INFB_EGRCOND_INDEX] != 0 )  // EGRCOND
					{
						if (bDisplayErrorMsg == TRUE)
							LogPrint ("FAILURE: ECU %X  EGRCOMP and EGRCOND not supported, must be zero\n", GetEcuId(EcuIndex) );
						test_passed = FALSE;
					}
				}
			}
			// if EGR monitor supported, EGRCOMP, EGRCOND must have incremented since test 10.9
			else
			{
				count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_INFB_EGRCOMP_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_INFB_EGRCOMP_INDEX];
				if ( count == 0 )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  EGRCOMP unchanged since Test 10.9\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}

				count = Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_INFB_EGRCOND_INDEX] - Test10_9_Sid9Ipt[EcuIndex].IPT[IPT_INFB_EGRCOND_INDEX];
				if ( count == 0 )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  EGRCOND unchanged since Test 10.9\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}
		}
	} // end if this vehicle supports SID 9 INF B

	// Test 11.11
	if ( test_stage == 11 )
	{
		// if not supported, then cannot do comparsions
		if ( IsSid9InfSupported (EcuIndex, 0x08) == TRUE || IsSid9InfSupported (EcuIndex, 0x0B) )
		{
			// IGNCNTR must be >= OBDCOND
			if ( Test11_11_Sid9Ipt[EcuIndex].IPT[IPT_IGNCNTR_INDEX] < Test11_11_Sid9Ipt[EcuIndex].IPT[IPT_OBDCOND_INDEX] )
			{
				if ( bDisplayErrorMsg == TRUE )
					LogPrint ("FAILURE: ECU %X  IGNCNTR less than OBDCOND\n", GetEcuId(EcuIndex) );
				test_passed = FALSE;
			}

			// OBDCOND must be >= other monitor condition counters
			for ( IptIndex = 3; IptIndex < Test11_11_Sid9Ipt[EcuIndex].NODI; IptIndex += 2 )
			{
				if ( Test11_11_Sid9Ipt[EcuIndex].IPT[IPT_OBDCOND_INDEX] < Test11_11_Sid9Ipt[EcuIndex].IPT[IptIndex] )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  OBDCOND not greater than all other monitor condition counters\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}

			// IGNCNTR and OBDCOND must have the same values as recorded in step 11.5
			if ( Test11_11_Sid9Ipt[EcuIndex].IPT[IPT_IGNCNTR_INDEX]  != Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_IGNCNTR_INDEX] )
			{
				if ( bDisplayErrorMsg == TRUE )
					LogPrint ("FAILURE: ECU %X  IGNCNTR different from value in test 11.5\n", GetEcuId(EcuIndex) );
				test_passed = FALSE;
			}

			if ( Test11_11_Sid9Ipt[EcuIndex].IPT[IPT_OBDCOND_INDEX]  != Test11_5_Sid9Ipt[EcuIndex].IPT[IPT_OBDCOND_INDEX] )
			{
				if ( bDisplayErrorMsg == TRUE )
					LogPrint ("FAILURE: ECU %X  OBDCOND different from value in test 11.5\n", GetEcuId(EcuIndex) );
				test_passed = FALSE;

			}

			// OBD condition counters must continue to be non-zero if they were non-zero in test 11.5
			for ( IptIndex = 2; IptIndex < Test11_11_Sid9Ipt[EcuIndex].NODI; IptIndex++ )
			{
				if ( Test11_5_Sid9Ipt[EcuIndex].IPT[IptIndex] !=0 &&
				     Test11_11_Sid9Ipt[EcuIndex].IPT[IptIndex] == 0 )
				{
					if ( bDisplayErrorMsg == TRUE )
						LogPrint ("FAILURE: ECU %X  OBD counter which was non-zero in test 11.5 is now zero\n", GetEcuId(EcuIndex) );
					test_passed = FALSE;
				}
			}
		}
	}

	return test_passed;
}


/*
*******************************************************************************
** SelectECU
*******************************************************************************
*/
void SelectECU (int new_index, int old_index)
{
	int index;

	// un-highlite previous selection
	setrgb (-1);
	index = ECU_ID_INDEX + old_index;
	if ((ECU_ID_INDEX <= index) && (index <= ECU_ID_INDEX+OBD_MAX_ECUS))
		SetFieldHex (index, GetEcuId(old_index));

	// highlite new selection
	setrgb (6);
	index = ECU_ID_INDEX + new_index;
	if ((ECU_ID_INDEX <= index) && (index <= ECU_ID_INDEX+OBD_MAX_ECUS))
		SetFieldHex (index, GetEcuId(new_index));

	// restore screen attributes
	setrgb (-1);
}


/*
*******************************************************************************
** DisplayEcuData
*******************************************************************************
*/
void DisplayEcuData (int EcuIndex)
{
	int index;

	// write IM Status
	for ( index = 0; index < 10; index++ )
	{
		SetFieldText ( IM_READINESS_INDEX+index, szIM_Status[Test11IMStatus[EcuIndex][index]] );
	}

	// copy IM Status for active ECUs
	for ( index = 0; index < 11; index++ )
	{
		CurrentIMStatus[index] = Test11IMStatus[EcuIndex][index];
	}

	// write rate based counter
	SetFieldDec ( OBD_MONITOR_COND_INDEX, Test11CurrentDisplayData[EcuIndex].IPT[0] );
	SetFieldDec ( IGNITION_COUNTER_INDEX, Test11CurrentDisplayData[EcuIndex].IPT[1] );

	// write initial values for rate based counters
	for ( index = 2; index < Test10_9_Sid9Ipt[EcuIndex].NODI; index++ )
	{
		SetFieldDec ( OBD_MONITOR_COND_INDEX+index, Test10_9_Sid9Ipt[EcuIndex].IPT[index] );
	}

	// write current values for rate based counters
	for ( index = 2; index < Test11CurrentDisplayData[EcuIndex].NODI; index++ )
	{
		SetFieldDec ( (IPT_CUR_INDEX+(index-2)), Test11CurrentDisplayData[EcuIndex].IPT[index] );
	}
}


/*
*******************************************************************************
** SaveSid9IptData
*******************************************************************************
*/
BOOL SaveSid9IptData (unsigned int EcuIndex, SID9IPT * pSid9Ipt)
{
	int index;
	BOOL rc = FALSE;


	// save Infotype and NODI
	Test11CurrentDisplayData[EcuIndex].INF = pSid9Ipt->INF;
	Test11CurrentDisplayData[EcuIndex].NODI = pSid9Ipt->NODI;

	// copy current counter values for active ECUs
	for ( index = 0; index < pSid9Ipt->NODI; index++ )
	{
		// note changes
		if ( Test11CurrentDisplayData[EcuIndex].IPT[index] != pSid9Ipt->IPT[index] )
		{
			// save current value of rate based counters
			Test11CurrentDisplayData[EcuIndex].IPT[index] = pSid9Ipt->IPT[index];

			rc = TRUE;
		}
	}

	return rc;
}


/*
*******************************************************************************
** UpdateSid9IptDisplay
*******************************************************************************
*/
void UpdateSid9IptDisplay ( unsigned int EcuIndex, SID9IPT * pSid9Ipt )
{
	int index;

	// write rate based counter
	SetFieldDec ( OBD_MONITOR_COND_INDEX, Test11CurrentDisplayData[EcuIndex].IPT[0] );
	SetFieldDec ( IGNITION_COUNTER_INDEX, Test11CurrentDisplayData[EcuIndex].IPT[1] );

	// update current counter values for active ECUs
	for ( index = 2; index < pSid9Ipt->NODI; index++ )
	{
		if ( pSid9Ipt->IPT[index] != Test11CurrentDisplayData[EcuIndex].IPT[index] )
		{
			SetFieldDec ( (IPT_CUR_INDEX+(index-2)), pSid9Ipt->IPT[index] );
		}
	}
}


/*
*******************************************************************************
** SaveSid1Pid1Data
*******************************************************************************
*/
BOOL SaveSid1Pid1Data (unsigned int EcuIndex)
{
	SID1 * pSid1;
	BOOL   rc = FALSE;

	pSid1 = (SID1 *)&gOBDResponse[EcuIndex].Sid1Pid[0];

	if (pSid1->PID == 1)
	{
		// note any differences
		if ((PreviousSid1Pid1[EcuIndex].Data[0] != pSid1->Data[0]) ||
		    (PreviousSid1Pid1[EcuIndex].Data[1] != pSid1->Data[1]) ||
		    (PreviousSid1Pid1[EcuIndex].Data[2] != pSid1->Data[2]) ||
		    (PreviousSid1Pid1[EcuIndex].Data[3] != pSid1->Data[3]) )
		{
			PreviousSid1Pid1[EcuIndex].Data[0] = pSid1->Data[0];
			PreviousSid1Pid1[EcuIndex].Data[1] = pSid1->Data[1];
			PreviousSid1Pid1[EcuIndex].Data[2] = pSid1->Data[2];
			PreviousSid1Pid1[EcuIndex].Data[3] = pSid1->Data[3];
			rc = TRUE;
		}

		// Oxygen Sensor Response
		if (pSid1->Data[2] & 1<<5)
			Test11IMStatus[EcuIndex][0] = (pSid1->Data[3] & 1<<5) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][0] = NotSupported;

		// Oxygen Sensor Heater
		if (pSid1->Data[2] & 1<<6)
			Test11IMStatus[EcuIndex][1] = (pSid1->Data[3] & 1<<6) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][1] = NotSupported;

		// Catalyst Monitor
		if (pSid1->Data[2] & 1<<0)
			Test11IMStatus[EcuIndex][2] = (pSid1->Data[3] & 1<<0) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][2] = NotSupported;

		// Catalyst Heater
		if (pSid1->Data[2] & 1<<1)
			Test11IMStatus[EcuIndex][3] = (pSid1->Data[3] & 1<<1) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][3] = NotSupported;

		// A/C
		if (pSid1->Data[2] & 1<<4)
			Test11IMStatus[EcuIndex][4] = (pSid1->Data[3] & 1<<4) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][4] = NotSupported;

		// Evaportive Emissions
		if (pSid1->Data[2] & 1<<2)
			Test11IMStatus[EcuIndex][5] = (pSid1->Data[3] & 1<<2) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][5] = NotSupported;

		// EGR
		if (pSid1->Data[2] & 1<<7)
			Test11IMStatus[EcuIndex][6] = (pSid1->Data[3] & 1<<7) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][6] = NotSupported;

		// AIR
		if (pSid1->Data[2] & 1<<3)
			Test11IMStatus[EcuIndex][7] = (pSid1->Data[3] & 1<<3) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][7] = NotSupported;

		// Fuel Trim
		if (pSid1->Data[1] & 1<<1)
			Test11IMStatus[EcuIndex][8] = (pSid1->Data[1] & 1<<5) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][8] = NotSupported;

		// Misfire
		if (pSid1->Data[1] & 1<<0)
			Test11IMStatus[EcuIndex][9] = (pSid1->Data[1] & 1<<4) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][9] = NotSupported;

		// Comp / Comp
		if (pSid1->Data[1] & 1<<2)
			Test11IMStatus[EcuIndex][10] = (pSid1->Data[1] & 1<<6) ? Incomplete : Complete;
		else
			Test11IMStatus[EcuIndex][10] = NotSupported;
	}

	return rc;
}

/*
*******************************************************************************
** UpdateSid1Pid1Display
*******************************************************************************
*/
void UpdateSid1Pid1Display (unsigned int EcuIndex)
{
	int index;

	for (index=0; index<11; index++)
	{
		if (CurrentIMStatus[index] != Test11IMStatus[EcuIndex][index])
		{
			CurrentIMStatus[index] = Test11IMStatus[EcuIndex][index];
			SetFieldText (IM_READINESS_INDEX+index, szIM_Status[Test11IMStatus[EcuIndex][index]]);
		}
	}
}

/*
*******************************************************************************
** RunDynamicTest11
*******************************************************************************
*/
STATUS RunDynamicTest11 (void)
{
	unsigned int  TestState, bLogMessage;
	unsigned long t1SecTimer, tDelayTimeStamp;
	unsigned int  EcuIndex, EcuMask, CurrentEcuIndex;
	unsigned int  IMReadyDoneFlags;
	unsigned int  Sid9IptDoneFlags;
	unsigned int  EcuDone;
	unsigned int  bSid9Ipt = 0;  // support IPT data INF, default 0 = no support

	const unsigned int EcuDoneMask = (1 << gUserNumEcus) - 1;

	SID_REQ       SidReq;
	SID1         *pSid1;

	unsigned char IPTSize = 0;
	unsigned char fInf8Supported = FALSE;
	unsigned char fInfBSupported = FALSE;

	BOOL          bSubTestFailed = FALSE;


	EcuDone          = 0;
	IMReadyDoneFlags = 0;
	Sid9IptDoneFlags = 0;


	// initialize static text elements
	if ( IsSid9InfSupported (-1, 0x08) == TRUE )
	{
		bSid9Ipt = 0x08;
		fInf8Supported = TRUE;
		init_screen (_string_elements11_INF840, _num_string_elements11_INF840);
	}

	else if ( IsSid9InfSupported (-1, 0x0B) == TRUE )
	{
		bSid9Ipt = 0x0B;
		fInfBSupported = TRUE;
		init_screen (_string_elements11_INFB32, _num_string_elements11_INFB32);
	}

	else
	{
		if ( gOBDDieselFlag == FALSE )
		{
			init_screen (_string_elements11_INF840, _num_string_elements11_INF840);
		}
		else
		{
			init_screen (_string_elements11_INFB32, _num_string_elements11_INFB32);
		}
	}


	if ( fInf8Supported == FALSE && fInfBSupported == FALSE )
	{
		LogPrint( "FAILURE: SID $9 IPT (INF $8 and INF $B) not supported\n" );
		bSubTestFailed = TRUE;
	}
	else if ( fInf8Supported == TRUE && fInfBSupported == TRUE )
	{
		LogPrint( "FAILURE: SID $9 IPT (INF $8 and INF $B) both supported\n" );
		bSubTestFailed = TRUE;
	}
	else if ( fInf8Supported == FALSE &&
	          gOBDDieselFlag == FALSE &&
	          gModelYear >= 2010 )
	{
		LogPrint( "FAILURE: SID $9 INF $8 not supported (Required for MY 2010 and latr Spark Ignition Vehicles)\n" );
		bSubTestFailed = TRUE;
	}
	else if ( fInfBSupported == FALSE &&
	          gOBDDieselFlag == TRUE &&
	          gModelYear >= 2010 )
	{
		LogPrint( "FAILURE: SID $ 9 INF $B not supported (Required for MY 2010 and latr Compression Ignition Vehicles)\n" );
		bSubTestFailed = TRUE;
	}


	for ( EcuIndex=0, EcuMask=1; EcuIndex < gUserNumEcus; EcuIndex++, EcuMask<<=1 )
	{
		SetFieldHex (ECU_ID_INDEX+EcuIndex, GetEcuId(EcuIndex));

		if (IsSid1PidSupported (EcuIndex, 1) == FALSE)
		{
			IMReadyDoneFlags |= EcuMask;
		}

		if ( IsSid9InfSupported (EcuIndex, 0x08) == FALSE &&
		     IsSid9InfSupported (EcuIndex, 0x0B) == FALSE)
		{
			Sid9IptDoneFlags |= EcuMask;

			if ( gOBDDieselFlag == FALSE )
			{
				Test11CurrentDisplayData[EcuIndex].NODI = 20;
				Test10_9_Sid9Ipt[EcuIndex].NODI = 20;
			}
			else
			{
				Test11CurrentDisplayData[EcuIndex].NODI = 16;
				Test10_9_Sid9Ipt[EcuIndex].NODI = 16;
			}
		}

		if ( (IMReadyDoneFlags & Sid9IptDoneFlags & EcuMask) != 0)
		{
			EcuDone |= EcuMask;
			SetFieldText (ECU_STATUS_INDEX+EcuIndex, "N/A");
		}
	}

	CurrentEcuIndex = 0;

	SelectECU (CurrentEcuIndex, -1);
	DisplayEcuData (CurrentEcuIndex);

	// flush the STDIN stream of any user input before loop
	clear_keyboard_buffer ();

	tDelayTimeStamp = t1SecTimer = GetTickCount ();
	TestState = 0;

	//-------------------------------------------
	// loop until test completes
	//-------------------------------------------
	for (;;)
	{
		//-------------------------------------------
		// request SID 1 PID 1
		//-------------------------------------------
		SidReq.SID    = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = 1;

		if (SidRequest (&SidReq, SID_REQ_NO_PERIODIC_DISABLE) != PASS)
		{
			LogMsgCopy ();
			LogPrint ("FAILURE: SID $1 PID $1 request failed\n");
			return FAIL;
		}

		bLogMessage = FALSE;
		for (EcuIndex = 0, EcuMask=1; EcuIndex < gUserNumEcus; EcuIndex++, EcuMask<<=1)
		{
			if (gOBDResponse[EcuIndex].Sid1PidSize > 0)
			{
				if (SaveSid1Pid1Data (EcuIndex) == TRUE)
				{
					bLogMessage = TRUE;

					if ( (IMReadyDoneFlags & EcuMask) == 0)
					{
						pSid1 = (SID1 *)&gOBDResponse[EcuIndex].Sid1Pid[0];
						if (IsIM_ReadinessComplete (pSid1) == TRUE)
							IMReadyDoneFlags |= EcuMask;
					}
				}

				if (EcuIndex == CurrentEcuIndex)
					UpdateSid1Pid1Display (EcuIndex);
			}
		}

		if (bLogMessage == TRUE)
		{
			LogMsgCopy ();
		}

		//-------------------------------------------
		// Get SID 9 IPT
		//-------------------------------------------
		if ( bSid9Ipt != 0 )
		{
			SidReq.SID    = 9;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = bSid9Ipt;

			if (SidRequest (&SidReq, SID_REQ_NO_PERIODIC_DISABLE) != PASS)
			{
				LogMsgCopy ();
				LogPrint ("FAILURE: SID $9 IPT request failed\n");
				return FAIL;
			}

			bLogMessage = FALSE;
			for ( EcuIndex = 0, EcuMask=1; EcuIndex < gUserNumEcus; EcuIndex++, EcuMask<<=1 )
			{
				if ( GetSid9IptData ( EcuIndex, &Test11_5_Sid9Ipt[EcuIndex] ) == PASS )
				{
					if ( EcuIndex == CurrentEcuIndex )
						UpdateSid9IptDisplay ( EcuIndex, &Test11_5_Sid9Ipt[EcuIndex] );

					if ( SaveSid9IptData ( EcuIndex, &Test11_5_Sid9Ipt[EcuIndex] ) == TRUE )
					{
						bLogMessage = TRUE;

						if ( (Sid9IptDoneFlags & EcuMask) == 0)
						{
							if ( EvaluateSid9Ipt ( EcuIndex, 5, FALSE ) == TRUE )
							{
								Sid9IptDoneFlags |= EcuMask;
							}
						}
					}
				}
			}

			if (bLogMessage == TRUE)
			{
				LogMsgCopy ();
			}
		}

		//-------------------------------------------
		// Check if test is complete
		//-------------------------------------------
		for (EcuIndex=0, EcuMask=1; EcuIndex < gUserNumEcus; EcuIndex++, EcuMask<<=1)
		{
			if ( (EcuDone & EcuMask) == 0)
			{
				if ( (IMReadyDoneFlags & Sid9IptDoneFlags & EcuMask) != 0)
				{
					EcuDone |= EcuMask;
					SetFieldText (ECU_STATUS_INDEX+EcuIndex, "Done");
				}
			}
		}

		if (EcuDone == EcuDoneMask)
		{
			if ( bSubTestFailed == TRUE )
			{
				return FAIL;
			}
			else
			{
				return PASS;
			}
		}

		//-------------------------------------------
		// Check for num or ESC key, delay 1 second
		//-------------------------------------------
		do
		{
			if (_kbhit () != 0)
			{
				char c = _getch ();
				if (c == 27)                    // ESC key
				{
					LogPrint ("INFORMATION: Test 11 aborted by user\n\n");
					return ABORT;
				}

				if ((c == 'F') || (c == 'f'))   // "FAIL" key
				{
					LogPrint ("INFORMATION: Test 11 failed by user\n\n");
					return FAIL;
				}

				if (('1' <= c) && (c <= '8'))   // new ECU index
				{
					EcuIndex = c - '1';         // zero-based index
					if (EcuIndex < gUserNumEcus && EcuIndex != CurrentEcuIndex)
					{
						SelectECU (EcuIndex, CurrentEcuIndex);
						DisplayEcuData (EcuIndex);
						CurrentEcuIndex = EcuIndex;
					}
				}
			}

			tDelayTimeStamp = GetTickCount ();

			Sleep ( min (1000 - (tDelayTimeStamp - t1SecTimer), 50) );

		} while (tDelayTimeStamp - t1SecTimer < 1000);

		t1SecTimer = tDelayTimeStamp;
	}

	LogPrint ("FAILURE: Error in RunDynamicTest11()\n\n");
	return FAIL;
}
