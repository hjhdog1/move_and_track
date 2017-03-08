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
**	DATE		MODIFICATION
**	05/10/04	Modified to request ALL PIDs starting from $FF, not $E0 as
**				originally coded.
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
** VerifyReverseOrderSupport - Function to verify SID1 reverse order support
*******************************************************************************
*/
STATUS VerifyReverseOrderSupport(void)
{
	unsigned long EcuIndex;
	unsigned int  IdIndex;
	unsigned long IdBitIndex;
	SID_REQ SidReq;

	/* Save the original response data for comparison */
	memcpy(&gOBDCompareResponse[0], &gOBDResponse[0], sizeof(gOBDResponse));

	// Clear Error Count for ERROR_RETURN
	ErrorCount();

	/* Ignore no responses */
	gIgnoreNoResponse = TRUE;

	/* Request SID 1 support data */
	for (IdIndex = 0xE0; IdIndex < 0x100; IdIndex -= 0x20)  //Spec ver11.4, start @ 0xE0
	{
		SidReq.SID = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = (unsigned char)IdIndex;
		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			/* There must be a response to PID 0x00 */
			if (IdIndex == 0x00)
			{
				LogPrint("FAILURE: SID $1 request failed\n");
				gIgnoreNoResponse = FALSE;
				return(FAIL);
			}
		}
		else if (IdIndex != 0x00)
		{
			for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
			{
				/* Check if data received and PID is not supported */
				if ( (gOBDResponse[EcuIndex].Sid1PidSize != 0) &&
				     ((gOBDCompareResponse[EcuIndex].Sid1PidSupport[(IdIndex - 1) >> 5].IDBits[
				     ((IdIndex - 1) >> 3) & 0x03] & (0x80 >> ((IdIndex - 1) & 0x07))) == 0))
				{
					/* If ISO15765 protocol, this is an error */
					if (gOBDList[gOBDListIndex].Protocol == ISO15765)
					{
						LogPrint("FAILURE: ECU %X  Unsupported SID $1 PID response\n", GetEcuId(EcuIndex) );
						gIgnoreNoResponse = FALSE;
						ERROR_RETURN;
						gIgnoreNoResponse = TRUE;
					}
				}
			}
		}
	}

	/* Verify that all SID 1 PID support data compares */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		for (IdIndex = 0; IdIndex < 8; IdIndex++)
		{
			for (IdBitIndex = 0; IdBitIndex < 4; IdBitIndex++)
			{
				if ( gOBDResponse[EcuIndex].Sid1PidSupport[IdIndex].IDBits[IdBitIndex] !=
				     gOBDCompareResponse[EcuIndex].Sid1PidSupport[IdIndex].IDBits[IdBitIndex])
				{
					LogPrint("FAILURE: ECU %X  SID $1 support request in reverse order failed\n", GetEcuId(EcuIndex) );
					gIgnoreNoResponse = FALSE;
					return(FAIL);
				}
			}
		}
	}

	/* Determine size of PIDs $06, $07, $08, $09 */
	if (DetermineVariablePidSize () != PASS)
	{
		ERROR_RETURN;
	}

	/* Warn the user that this could take a while */
	printf("INSTRUCTIONS: Please wait (press any key to abort)...\n");
	clear_keyboard_buffer ();

	/*
	** 05/10/04 - Changed start PID from $E0 to #FF as specified in
	**            J1699 Draft 11.4.
	** Request SID 1 PID data in reverse
	** 07/19/04 - Correct comment declare issue.
	*/
	for (IdIndex = 0xFF; IdIndex < 0x100; IdIndex -= 0x01)
	{
		/* Bypass PID request if it is a support PID (already done above) */
		if ((IdIndex & 0x1F) != 0)
		{
			printf("INFORMATION: Checking PID $%02X\r",IdIndex);
			SidReq.SID = 1;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = (unsigned char)IdIndex;
			SidRequest(&SidReq, SID_REQ_NORMAL);
			for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
			{
				/* Check if data received and PID is not supported */
				if ( (gOBDResponse[EcuIndex].Sid1PidSize != 0) &&
					 (IsSid1PidSupported (EcuIndex, IdIndex) == FALSE) )
				{
					/* If ISO15765 protocol, this is an error */
					if (gOBDList[gOBDListIndex].Protocol == ISO15765)
					{
						LogPrint("FAILURE: ECU %X  Unsupported SID $1 PID response failure\n", GetEcuId(EcuIndex) );
						gIgnoreNoResponse = FALSE;
						ERROR_RETURN;
						gIgnoreNoResponse = TRUE;
					}
				}

				/* Check if no data received and PID is supported */
				if ( (gOBDResponse[EcuIndex].Sid1PidSize == 0) &&
					 (IsSid1PidSupported (EcuIndex, IdIndex) == TRUE) )
				{
					LogPrint("FAILURE: ECU %X  SID $1 PID in reverse order failure\n", GetEcuId(EcuIndex) );
					gIgnoreNoResponse = FALSE;
					ERROR_RETURN;
					gIgnoreNoResponse = TRUE;
				}
			}
		}

		/* Abort test if user presses a key */
		if (_kbhit())
		{
			LogPrint("FAILURE: User abort\n");
			clear_keyboard_buffer ();
			gIgnoreNoResponse = FALSE;
			return(FAIL);
		}
	}

	printf("\n");

	/*
	** If ISO15765 protocol, make sure the required OBDMID/SDTID values are supported
	** and try group support.  Verify group test only for ISO15765 only!
	*/
	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{

		/* 5/03/04 - Restructure logic to allow for Link Active test after completion of
		*           group request
		*/
		if(VerifyReverseGroupDiagnosticSupport() == FAIL)
		{
			return(FAIL);
		}
	}

	/* Link active test to verify communication remained active for ALL protocols
	 */
	if (VerifyLinkActive() != PASS)
	{
		return(FAIL);
	}

	gIgnoreNoResponse = FALSE;

	if ( ErrorCount() != 0 )
	{
		return(FAIL);
	}

	return(PASS);
}

