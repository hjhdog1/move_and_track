/*
**
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
** The following design parameters/rationale were used during the development
** of this source code:
** 1 - Keep it as simple as possible
** 2 - Single source code file (simplify publishing, compiling and revision
**		 control)
** 3 - Make response data look like ISO15765/ISO15031 to simplify processing
**		 wherever possible
** 4 - Log all screen text, user prompts and diagnostic message traffic
** 5 - Use globals for response information storage to simplify usage across
**		 functions
** 6 - Make all code capable of being compiled with the free GNU C-compiler
** 7 - Use only native C-language, ANSI-C runtime library and SAE J2534 API
**		 functions
**
** Where to get the free C-language compiler:
**     Go to www.cygwin.com website and download and install the Cygwin
**     environment.
**
** How to build:
**     From the Cygwin shell environment type:
**     "gcc -mno-cygwin *.c -o j1699.exe"
**     in the directory that contains the j1699.c source code file and a
**     j2534.h header file from your PassThru interface vendor.
**
** How to run:
**     First you will need to install the J2534 software from the vendor that
**     supplied your PassThru vehicle interface hardware. Then, from a MS-DOS
**     window, in the directory with the j1699.exe file, type "J1699" to run
**     the program.  User prompts will lead you through the program.
**
** Program flow:
**     The program flow is the same as the SAE J1699-3 document.  Please refer
**     to the document for more information.
**
** Log file:
**     The log filename created will be a concatenation of the vehicle
**     information that the user enters (model year, manufacturer and make)
**     along with a numeric value and a ".log" extension.  The numeric value
**     will automatically be incremented by the program to the next available
**     number.
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
** j1699 revision
*/
const char szJ1699_VER[] = "J1699-3";

const char szAPP_REVISION[] = "13.07.00";

const char szBUILD_DATE[] = __DATE__;

/*
** j2534 interface pointers
*/
PTCONNECT PassThruConnect = 0;
PTDISCONNECT PassThruDisconnect = 0;
PTREADMSGS PassThruReadMsgs = 0;
PTWRITEMSGS PassThruWriteMsgs = 0;
PTSTARTPERIODICMSG PassThruStartPeriodicMsg = 0;
PTSTOPPERIODICMSG PassThruStopPeriodicMsg = 0;
PTSTARTMSGFILTER PassThruStartMsgFilter = 0;
PTSTOPMSGFILTER PassThruStopMsgFilter = 0;
PTSETPROGRAMMINGVOLTAGE PassThruSetProgrammingVoltage = 0;
PTREADVERSION PassThruReadVersion = 0;
PTGETLASTERROR PassThruGetLastError = 0;
PTIOCTL PassThruIoctl = 0;
PTOPEN  PassThruOpen = 0;
PTCLOSE PassThruClose = 0;

/*
** OBD type definitions
*/
const char *OBD_TYPE[14] = {
"reserved",
"OBD II",
"OBD",
"OBD and OBDII",
"OBD I",
"NO OBD",
"EOBD",
"EOBD and OBD II",
"EOBD and OBD",
"EOBD, OBD and OBD II",
"JOBD",
"JOBD and OBD II",
"JOBD and EOBD",
"JOBD, EOBD and OBD II",
};

/* Global variables */
char gLogFileName[80] = {0};
char gUserModelYear[80] = {0};
int  gModelYear = 0;
char gUserMake[80] = {0};
char gUserModel[80] = {0};
unsigned long gUserNumEcus = 0;

unsigned long gRespTimeOutofRange = 0;
unsigned long gRespTimeTooSoon = 0;
unsigned long gRespTimeTooLate = 0;
unsigned long gDetermineProtocol =0;
/*
** Added variable to satify changes required for operator prompt 2.
*/
unsigned long gUserNumEcusReprgm = 0;
unsigned char gOBDKeywords[2] = {0};
unsigned char gOBDFailureBypassed = FALSE;      // set if at least one of the static tests or dynamic tests failed
unsigned char gOBDTestFailed = FALSE;           // set if any test 10 or test 11 subsections failed
unsigned char gOBDDetermined = FALSE;
unsigned long gOBDRequestDelay = 100;
unsigned long gOBDMaxResponseTimeMsecs = 100;
unsigned long gOBDMinResponseTimeMsecs = 0;		/* 6/07/04 - Added to verify Min response time */
unsigned long gOBDListIndex = 0;
unsigned long gOBDFoundIndex = 0;
PROTOCOL_LIST gOBDList[OBD_MAX_PROTOCOLS] = {0};
unsigned long gOBDNumEcus = 0;
unsigned long gOBDNumEcusResp = 0;              /* 06/17/04 - Added to pass number of responding cntrls to call routine */
unsigned char gOBDEngineRunning = FALSE;
unsigned char gOBDEngineWarm = FALSE;
unsigned char gOBDDTCPending = FALSE;
unsigned char gOBDDTCStored = FALSE;
unsigned char gOBDDTCHistorical = FALSE;
unsigned char gOBDDTCPermanent = FALSE;
unsigned char gOBDIMDriveCycle = FALSE;
unsigned char gOBDDieselFlag = FALSE;
unsigned char gOBDHybridFlag = FALSE;
unsigned char gOBDEuropeFlag = FALSE;
unsigned long gOBDProtocolOrder = 0;
unsigned long gOBDMonitorCount = 0;
unsigned long gOBDAggregateResponseTimeMsecs = 0;
unsigned long gOBDAggregateResponses = 0;
unsigned long gLastLogTime = 0;
unsigned char gIgnoreNoResponse = FALSE;
unsigned char gIgnoreUnsupported = FALSE;
unsigned char gSuspendScreenOutput = FALSE;
unsigned char gSuspendLogOutput = FALSE;
OBD_DATA gOBDResponse[OBD_MAX_ECUS] = {0};
OBD_DATA gOBDCompareResponse[OBD_MAX_ECUS] = {0};
char gVIN[18] = {0};
FILE *ghLogFile = NULL;
FILE *ghTempLogFile = NULL;
PASSTHRU_MSG gTesterPresentMsg = {0};

char gszLogBufferCopy[LOG_BUFFER_COPY_SIZE] = {0};
unsigned long gLogBufferCopyIndex = 0;

unsigned long gOBDNumEcusCan = 0  ;               /* by Honda */
unsigned char gOBDResponseTA[OBD_MAX_ECUS] = {0}; /* by Honda */

long gSid1VariablePidSize = 0;

extern DTC_LIST DTCList[OBD_MAX_ECUS] = {0};

unsigned long DeviceID = 0;

TEST_PHASE  TestPhase = eTestNone;
unsigned char  TestSubsection;      // test subsection to be used in conjunction with TestPhase (ie test phase 5, subsection 14 - 5.14)

char *gBanner =
"\n\n"
"  JJJJJJJJJJJJJ      1       6666666666666    9999999999999    9999999999999\n"
"         J          11       6           6    9           9    9           9\n"
"         J         1 1       6                9           9    9           9\n"
"         J           1       6                9           9    9           9\n"
"         J           1       6                9           9    9           9\n"
"         J           1       6                9           9    9           9\n"
"         J           1       6666666666666    9999999999999    9999999999999\n"
"         J           1       6           6                9                9\n"
"         J           1       6           6                9                9\n"
"         J           1       6           6                9                9\n"
"         J           1       6           6                9                9\n"
"  J      J           1       6           6    9           9    9           9\n"
"  JJJJJJJJ         11111     6666666666666    9999999999999    9999999999999\n"
"\n"
"  Copyright (C) 2002 Drew Technologies. http://j1699-3.sourceforge.net/\n"
"\n"
"  This program is free software; you can redistribute it and/or modify\n"
"  it under the terms of the GNU General Public License as published by\n"
"  the Free Software Foundation; either version 2 of the License, or\n"
"  (at your option) any later version.\n"
"\n"
"  This program is distributed in the hope that it will be useful,\n"
"  but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"  GNU General Public License for more details.\n"
"\n\n";

char gszTempLogFilename[MAX_PATH] = {0};
BOOL fTest10 = FALSE;

STATUS RunStaticTests (void);
STATUS RunDynamicTests (void);

STATUS OpenTempLogFile (void);

void LogResponseStats (void);

BOOL WINAPI HandlerRoutine (DWORD dwCtrlType);
void CleanupAndExit (void);

/*
********************************************************************************
** Main function
********************************************************************************
*/
int main(int argc, char **argv)
{
	unsigned long LogFileIndex;
	int           nTestChoice;
	STATUS        RetCode = FAIL;

	/* setup ctrl-C handler */
	SetConsoleCtrlHandler (HandlerRoutine, TRUE);

	/* use an unbuffered stdout */
	setbuf(stdout, NULL);

	/* Send out the banner */
	printf (gBanner);

	/* open temp log file */
	if (OpenTempLogFile () != PASS)
	{
		printf ("FAILURE: Cannot open temp log file\n");
		exit (FAIL);
	}

	/* Find the J2534 interface and attach to the DLL */
	if (FindJ2534Interface() != PASS)
	{
		printf ("FAILURE: J2534 interface not found\n");
		exit (FAIL);
	}

	/* Open J2534 device */
	{
		unsigned long RetVal = PassThruOpen (NULL, &DeviceID);
		if (RetVal != STATUS_NOERROR)
		{
			Log2534Error("", RetVal, "PassThruOpen");
			return(FAIL);
		}
	}

	/* Get user input to determine whic test to run */
	do
	{
		clear_keyboard_buffer ();
		printf ("PROMPT: \n");
		printf ("1. Run Static  Tests contained in Sections 5  through 9.\n");
		printf ("2. Run Dynamic Tests contained in Sections 10 through 11.\n\n");
		printf ("Enter choice (1 or 2): ");
		scanf  ("%d", &nTestChoice);
	}
	while (nTestChoice < 1 || nTestChoice > 2);


	/* Get the user input to create the log filename */
	clear_keyboard_buffer ();
	printf ("PROMPT: Enter model year of vehicle: ");
	gets (gUserModelYear);
	gModelYear = atoi(gUserModelYear);

	/* Collect user input for static tests */
	if (nTestChoice == 1)
	{
		clear_keyboard_buffer ();

		printf ("PROMPT: Enter make of vehicle (e.g. Dodge, Ford, GMC, Honda, Toyota...): ");
		gets (gUserMake);

		printf ("PROMPT: Enter vehicle model: ");
		gets (gUserModel);
	}

	/* Ask the user for some basic information about the vehicle */
	while ((gUserNumEcus < 1) || (gUserNumEcus > OBD_MAX_ECUS))
	{
		clear_keyboard_buffer ();
		printf ("PROMPT: How many OBD-II ECUs are on this vehicle (1 to 8)? ");
		scanf  ("%d", &gUserNumEcus);
	}

	while ((gUserNumEcusReprgm < 1) || (gUserNumEcusReprgm > OBD_MAX_ECUS))
	{
		clear_keyboard_buffer ();
		printf ("PROMPT: How many reprogrammable, OBD-II ECUs are on this vehicle (1 to 8)? ");
		scanf  ("%d", &gUserNumEcusReprgm);
	}

	printf ("PROMPT: Does the vehicle use compression ignition (i.e. diesel)? ");
	{
		char  userResponse[256];
		scanf ("%s", userResponse);
		if (toupper(userResponse[0]) == 'Y')
			gOBDDieselFlag = TRUE;
	}

	printf ("PROMPT: Is this a hybrid vehicle? ");
	{
		char  userResponse[256];
		scanf ("%s", userResponse);
		if (toupper(userResponse[0]) == 'Y')
			gOBDHybridFlag = TRUE;
	}

	/* Get the start time for the log file */
	gLastLogTime = GetTickCount();

	/* Run Static test */
	if (nTestChoice == 1)
	{
		/* Create the log file name from the user input */
		ghLogFile = NULL;
		for (LogFileIndex = 1; LogFileIndex < 1000000; LogFileIndex++)
		{
			sprintf (gLogFileName, "%s-%s-%s-%ld.log", gUserModelYear, gUserMake, gUserModel, LogFileIndex);

			/* Check if log file already exists */
			if ((ghLogFile = fopen (gLogFileName, "r")) != NULL)
			{
				fclose (ghLogFile);
			}
			else
			{
				break;
			}
		}

		/* Open the log file */
		ghLogFile = fopen (gLogFileName, "w+");
		if (ghLogFile == NULL)
		{
			printf ("FAILURE: Cannot open log file %s\n", gLogFileName);
			exit (FAIL);
		}

		/* Application version, build date, OS, etc */
		LogVersionInformation ();

		/* Copy temp log file to actual log file */
		if (AppendLogFile () != PASS)
		{
			printf ("FAILURE copying temp log file to %s\n", gLogFileName);
			StopTest (FAIL);
		}

		/* Echo user responses to the log file */
		LogPrint ("INFORMATION: Model Year of this vehicle?  %d\n", gModelYear);
		LogPrint ("INFORMATION: How many OBD-II ECUs are on this vehicle (1 to 8)?  %d\n", gUserNumEcus);
		LogPrint ("INFORMATION: How many reprogrammable, OBD-II ECUs are on this vehicle (1 to 8)? %d\n", gUserNumEcusReprgm);
		LogPrint ("INFORMATION: Does the vehicle use compression ignition (i.e. diesel)? %s\n", gOBDDieselFlag ? "YES" : "NO");
		LogPrint ("INFORMATION: Is this a hybrid vehicle? %s\n", gOBDHybridFlag ? "YES" : "NO");

		/* Ask for optional user information */
		LogUserPrompt("Enter your name and/or contact information (optional) ", ENTER_PROMPT);

		/* Run Static test */
		RetCode = RunStaticTests ();
		if ( RetCode == FAIL )
		{
			LogPrint("\n\n**** J1699 Static Test FAILURE ****\n\n");
		}
		else
		{
			LogPrint("\n\n**** All J1699 Static Tests PASSED ****\n\n");
		}

		/* Prompt user to run dynamic test */
		if (LogUserPrompt("Would you like to run dynamic tests?", YES_NO_PROMPT) == 'Y')
		{
			nTestChoice = 2;
			LogResponseStats ();
			fclose (ghLogFile);
			ghLogFile = ghTempLogFile;
			strcpy (gLogFileName, gszTempLogFilename);
		}

	}

	/* Run Dynamic test */
	if (nTestChoice == 2)
	{
		/* run dynamic tests */
		RetCode = RunDynamicTests ();
	}

	/* Done - passed */
	LogResponseStats ();
	StopTest (RetCode);

	return 0;
}

/*
********************************************************************************
** RunStaticTests function
********************************************************************************
*/
STATUS RunStaticTests (void)
{
	/* Reset vehicle state */
	gOBDEngineRunning = FALSE;
	gOBDDTCPending    = FALSE;
	gOBDDTCStored     = FALSE;
	gOBDDTCHistorical = FALSE;
	gOBDProtocolOrder = 0;
	gOBDFailureBypassed = FALSE;

	/* Run tests 5.XX */
	TestPhase = eTestNoDTC;
	if (TestWithNoDtc() != PASS)
	{
		return FAIL;
	}

	/*
	** Sleep 5 seconds between each test to "drop out" of diagnostic session
	** so we can see if a different OBD protocol is found on the next search
	*/
	Sleep (5000);

	/* Run tests 6.XX */
	TestPhase = eTestPendingDTC;
	if (TestWithPendingDtc() != PASS)
	{
		return FAIL;
	}

	/*
	** Sleep 5 seconds between each test to "drop out" of diagnostic session
	** so we can see if a different OBD protocol is found on the next search
	*/
	Sleep (5000);

	/* Run tests 7.XX */
	TestPhase = eTestConfirmedDTC;
	if (TestWithConfirmedDtc() != PASS)
	{
		return FAIL;
	}

	/*
	** Sleep 5 seconds between each test to "drop out" of diagnostic session
	** so we can see if a different OBD protocol is found on the next search
	*/
	Sleep (5000);

	/* Run tests 8.XX */
	TestPhase = eTestFaultRepaired;
	if (TestWithFaultRepaired() != PASS)
	{
		return FAIL;
	}

	/*
	** Sleep 5 seconds between each test to "drop out" of diagnostic session
	** so we can see if a different OBD protocol is found on the next search
	*/
	Sleep (5000);

	/* Run tests 9.XX */
	TestPhase = eTestNoFault3DriveCycle;
	if (TestWithNoFaultsAfter3DriveCycles() != PASS)
	{
		return FAIL;
	}

	if ( ( gOBDList[gOBDListIndex].Protocol == ISO9141 ) ||
		 ( gOBDList[gOBDListIndex].Protocol == ISO14230 ) )
	{
		LogResponseStats ();
	}

	if ( gOBDTestFailed == TRUE )
	{
		return FAIL;
	}

	return PASS;
}

/*
********************************************************************************
** RunDynamicTests function
********************************************************************************
*/
STATUS RunDynamicTests (void)
{
	STATUS RetCode;

	gOBDFailureBypassed = FALSE;

	// flag for the ctrl-C handler to delete the 'vin' file
	fTest10 = TRUE;

	// Run tests 10.XX
	TestPhase = eTestInUseCounters;
	RetCode = TestToVerifyInUseCounters ();
	if (RetCode == FAIL)
	{
		if ( TestContinue( "Test 10 failed. Do you wish to continue with test 11? " ) != 'Y' )
		{
			CleanupAndExit ();
			return RetCode;
		}
	}
	else if (RetCode == ABORT)
	{
		if ( TestContinue( "Test 10 incomplete. Do you wish to continue with test 11? " ) != 'Y')
		{
			CleanupAndExit ();
			return RetCode;
		}
	}

	// from this point on, do not delete the 'vin' file
	fTest10 = FALSE;

	// Proceed to tests 11.XX
	TestPhase = eTestPerformanceCounters;
	if ( (RetCode = TestToVerifyPerformanceCounters()) != PASS)
	{
		return RetCode;
	}

	if ( gOBDTestFailed == TRUE )
	{
		LogPrint(" \n\n**** J1699 Dynamic Test FAILURE ****\n");
		return FAIL;
	}

	LogPrint(" \n\n**** All J1699 Dynamic Tests PASSED ****\n");
	return PASS;
}

/*
********************************************************************************
** OpenTempLogFile
********************************************************************************
*/
STATUS OpenTempLogFile (void)
{
	/* Get temp log filename */
	if (GetTempFileName (".", "j1699", 0, gszTempLogFilename) == 0)
		return FAIL;

	/* Open the log file */
	ghTempLogFile = ghLogFile = fopen (gszTempLogFilename, "w+");
	if (ghTempLogFile == NULL)
	{
		return FAIL;
	}

	return PASS;
}

/*
********************************************************************************
** AppendLogFile
********************************************************************************
*/
STATUS AppendLogFile (void)
{
	char buf[1024];

	/* move to beginning of temp log file */
	fflush (ghTempLogFile);
	fseek (ghTempLogFile, 0, SEEK_SET);

	/* move to end of log file */
	fseek (ghLogFile, 0, SEEK_END);

	/* append temp log file to official log file */
	while (fgets (buf, sizeof(buf), ghTempLogFile) != NULL)
		fputs (buf, ghLogFile);

	return PASS;
}

/*
********************************************************************************
** CleanupAndExit
********************************************************************************
*/
void CleanupAndExit ()
{
	// ctrl-C or other system events which terminate the test
	DisconnectProtocol ();

	LogResponseStats ();

	// close any open log files
	_fcloseall ();

	DeleteFile (gszTempLogFilename);

	// don't save the 'vin' file if test 10 did not complete successfully
	if (fTest10 == TRUE)
	{
		DeleteFile (gLogFileName);
		exit (1);
	}
}

/*
********************************************************************************
** HandlerRoutine
********************************************************************************
*/
BOOL WINAPI HandlerRoutine (DWORD dwCtrlType)
{
	// ctrl-C or other system events which terminate the test
	LogPrint("INFORMATION: Ctrl-C :: terminating application\n");

	CleanupAndExit ();

	exit (1);
}

/*
********************************************************************************
** LogResponseStats
********************************************************************************
*/
void LogResponseStats (void)
{
	LogPrint ("INFORMATION: Number of response time Out of Range = %d so far.\n", gRespTimeOutofRange);
	LogPrint ("INFORMATION: Number of response time sooner than allowed = %d.\n", gRespTimeTooSoon);
	LogPrint ("INFORMATION: Number of response time later than allowed = %d.\n", gRespTimeTooLate);
}

/*
********************************************************************************
** LogVersionInformation
********************************************************************************
*/
void LogVersionInformation (void)
{
	time_t current_time;
	struct tm *current_tm;
	unsigned long version;

	/* Put the date / time in the log file */
	time (&current_time);
	current_tm = localtime (&current_time);
	LogPrint ("INFORMATION: %s\n\n", asctime (current_tm));

	/* Let the user know what version is being run and the log file name */
	LogPrint ("INFORMATION: **** LOG FILENAME %s ****\n", gLogFileName);
	LogPrint ("INFORMATION: **** SAE %s Revision %s  (Buid Date: %s) ****\n\n", szJ1699_VER, szAPP_REVISION, szBUILD_DATE);
	LogPrint ("INFORMATION: **** NOTE: Timestamp on left is from the PC ****\n");
	LogPrint ("INFORMATION: **** NOTE: Timestamp with messages is from the J2534 interface ****\n\n");

	/* Log Microsoft Windows OS */
	version = GetVersion();
	if (version & 0x80000000)
	{
		LogPrint("WARNING: Windows 9X/ME (%08X)\n", version);
	}
	else
	{
		LogPrint("INFORMATION: Windows NT/2K/XP (%08X)\n", version);
	}
}
