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
*               has been indicated as section 6 in Draft 15.4.
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
** TestWithPendingDtc -
** Function to run test with a pending code induced by a fault
*******************************************************************************
*/
STATUS TestWithPendingDtc(void)
{
	/* Prompt user to induce fault and start engine */
	LogPrint("\n\n**** Test 6.1 (Pending DTC) ****\n");
	LogUserPrompt("Turn key off and disconnect a sensor (e.g. ECT, TP, IAT, MAF, etc.)\n"
	              "that will generate a MIL light with the engine idling for one ECU.\n"
	              "The selected fault must illuminate the MIL using two driving cycles\n"
	              "(not one like GM Type A) to allow for proper testing of Mode 7 and\n"
	              "freeze frame", ENTER_PROMPT);
	LogUserPrompt("Start engine and let idle", ENTER_PROMPT);
	gOBDEngineRunning = TRUE;

	LogPrint("**** Test 6.1 PASSED ****\n");

	/* Determine the OBD protocol to use */
	LogPrint("\n\n**** Test 6.2 ****\n");
	if ( DetermineProtocol() != PASS )
	{
		LogPrint( "**** Test 6.2 FAILED ****\n" );
		LogPrint( "Protocol determination failed.\n" );
		return(FAIL);
	}
	else
	{
		LogPrint("**** Test 6.2 PASSED ****\n");
	}

	LogPrint("\n\n**** Test 6.3 ****\n");
	/* flush the STDIN stream of any user input before loop */
	clear_keyboard_buffer ();

	/* Wait for pending DTC */
	LogPrint("INFORMATION: Waiting for pending DTC...(press any key to stop test)\n");
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

	/* Set flag to indicate a pending DTC should be present */
	gOBDDTCPending = TRUE;

	/* Verify there is a pending DTC */
	if (IsDTCPending() == FALSE)
	{
		LogPrint("**** Test 6.3 FAILED ****\n");
		if ( TestContinue( "No DTC pending. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		/* Get DTC list from SID $07 */
		SaveDTCList (7);

		/* Check if a DTC is stored as well (i.e. GM Type A) */
		if (IsDTCStored() == TRUE)
		{
			LogPrint("INFORMATION: Type A DTC detected, pending without a stored code functionality not tested\n");
			gOBDDTCStored = TRUE;
		}
	}

	/* Verify pending DTC data */
	if (VerifyDTCPendingData() != PASS)
	{
		LogPrint("**** Test 6.3 FAILED ****\n");
		if ( TestContinue( "DTC pending data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 6.3 PASSED ****\n");
	}

	/* Verify stored DTC data */
	LogPrint("\n\n**** Test 6.4 ****\n");
	if (VerifyDTCStoredData() != PASS)
	{
		LogPrint("**** Test 6.4 FAILED ****\n");
		if ( TestContinue( "DTC stored data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}

	/* Verify MIL and DTC status is cleared */
	if (VerifyMILData() != PASS)
	{
		LogPrint("**** Test 6.4 FAILED ****\n");
		if ( TestContinue( "MIL / DTC status failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 6.4 PASSED ****\n");
	}

	/* Verify freeze frame support and data */
	LogPrint("\n\n**** Test 6.5 ****\n");
	if (VerifyFreezeFrameSupportAndData() != PASS)
	{
		LogPrint("**** Test 6.5 FAILED ****\n");
		if ( TestContinue( "Freeze frame support/data failed. Continue?" ) == 'N' )
		{
			return(FAIL);
		}
	}
	else
	{
		LogPrint("**** Test 6.5 PASSED ****\n");
	}

	/* Link active test to verify communication remained active for ALL protocols */
	if (VerifyLinkActive() != PASS)
	{
		return( FAIL );
	}

	return(PASS);
}

//*****************************************************************************
//
//	Function:	SaveDTCList
//
//	Purpose:	Save the DTC list from SID $03 or $07
//
//*****************************************************************************
void SaveDTCList (int nSID)
{
	unsigned long  EcuIndex, index;
	unsigned short size;

	if (nSID != 3 && nSID != 7)
		return;

	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		size = DTCList[EcuIndex].Size = (nSID == 3) ? gOBDResponse[EcuIndex].Sid3Size
		                                            : gOBDResponse[EcuIndex].Sid7Size;
		for (index = 0; index < size; index++)
			DTCList[EcuIndex].DTC[index] = (nSID == 3) ? gOBDResponse[EcuIndex].Sid3[index]
			                                           : gOBDResponse[EcuIndex].Sid7[index];
	}
}
