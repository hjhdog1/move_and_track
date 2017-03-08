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
** 04/26/04      As defined in document titled "J1699-3_v11.4_draft.doc",
**               section "Verify Service $08 - Request control of on-board
**               system, test or component, engine running", table 66
**               conditional data is defined as follows:
**               C2 =	Conditional - value indicates TIDs supported; range
**               of supported TIDs depends on selected TID value (see C1) for
**               ISO 15765-4.
**               For J1850, ISO9141-2 and ISO 14230-4, Data A-E shall be
**               filled with $00 if unused.
**               Corrected logic to reflect this statement.
********************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

STATUS RequestSID8SupportData (void);
void   FormatSID8SupReq (unsigned long IdIndex, SID_REQ *SidReq);
int    VerifySid8PidSupportData (void);

/*
*******************************************************************************
** VerifyControlSupportAndData -
** Function to verify SID8 control support and data
*******************************************************************************
*/
STATUS VerifyControlSupportAndData(void)
{
	STATUS	ret_code;

	// Clear Error Count for ERROR_RETURN
	ErrorCount();

	/* Request SID 8 support data */
	ret_code = RequestSID8SupportData ();
	if (ret_code == FAIL)
		return FAIL;

	if (ret_code == ABORT)
		return PASS;

	/* Try group support if ISO15765 */
	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{
		/*return(VerifyGroupControlSupport());*/
		/* 4/30/04 - Restructure logic to allow for Link Active test after completion of
		 *           group request
		 */
		if(VerifyGroupControlSupport() == FAIL)
		{
			return (FAIL);
		}
	}

	/* Link active test to verify communication remained active for ALL protocols
	 */
	if (VerifyLinkActive() != PASS)
	{
		return (FAIL);
	}

	if ( ErrorCount() != 0 )
	{
		return(FAIL);
	}

	return (PASS);
}

/*
********************************************************************************
**
**	FUNCTION:		FormatSID8SupReq
**
**	DESCRIPTION:	Routine
********************************************************************************
** DATE          MODIFICATION
** 05/12/04      Common routine to allow the addition of testcase 5.16.3.
**
********************************************************************************
*/
void FormatSID8SupReq (unsigned long IdIndex, SID_REQ *SidReq)
{

	SidReq->SID = 8;								//	SID $08

	SidReq->Ids[0] = (unsigned char)IdIndex;		//	Test ID (request Test ID values)

	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{
		// Documented in ISO15031-5, "ISO_DIS_15031-5.4_(E)_RevOn_2004-01-20.doc"
		// Annex F
		//"For ISO 15765-4 protocol DATA_A - DATA_E shall not be included in the
		// request and response message."

		SidReq->NumIds = 1;
	}
	else
	{
		// Documented in ISO15031-5, "ISO_DIS_15031-5.4_(E)_RevOn_2004-01-20.doc"
		// Annex F

		//"For ISO 9141-2, ISO 14230-4, and SAE J1850 DATA_A - DATA_E should be
		// set to $00 for a request and response message."

		SidReq->NumIds = 6;

		SidReq->Ids[1] = 0; /* Data record of Test ID */
		SidReq->Ids[2] = 0;
		SidReq->Ids[3] = 0;
		SidReq->Ids[4] = 0;
		SidReq->Ids[5] = 0;
	}
}

//*****************************************************************************
//
//	Function:	RequestSID8SupportData
//
//	Purpose:	Purpose of this routine is to verify that SID 8 PID 00
//              returns a support record. Continue requesting support
//              PIDs thru the highest supported group.
//
//*****************************************************************************
STATUS RequestSID8SupportData (void)
{
	unsigned long IdIndex;
	unsigned long EcuIndex;
	unsigned long ulTIDSupport;	/* Evaluate $E0 TID support indication. */
	SID_REQ SidReq;

	/* Request SID 8 support data */
	for (IdIndex = 0x00; IdIndex < 0x100; IdIndex += 0x20)
	{
		FormatSID8SupReq( IdIndex, &SidReq );

		if (SidRequest(&SidReq, SID_REQ_NORMAL) == FAIL)
		{
			/* Mode 8 support is optional, just warn if there were no responses */
			if (IdIndex == 0x00)
			{
				LogPrint("WARNING: No response to Sid $8 support request\n");
				return (ABORT);
			}
		}

		/* Check if we need to request the next group */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid8TidSupport[IdIndex >> 5].IDBits[3] & 0x01)
			{
				break;
			}
		}
		if (EcuIndex >= gOBDNumEcus)
		{
			break;
		}
	}

	/* Flag error if ECU indicates no support */
	if (VerifySid8PidSupportData() == FAIL)
	{
		ERROR_RETURN;
	}

	/* J1699 V11.5 TC # 5.16.3 call for request of next unsupported TID support TID */

	/* Enhance logic to verify support information if request is at upper limit of $E0 */
	if ( IdIndex == 0xE0 )
	{
		/* Init variable to no-support */
		ulTIDSupport = 0;

		/* For each ECU */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			/* If MID is supported, keep looking */
			if ( ( gOBDResponse[EcuIndex].Sid8TidSupport[IdIndex >> 5].IDBits[0]		||
				   gOBDResponse[EcuIndex].Sid8TidSupport[IdIndex >> 5].IDBits[1]	    ||
				   gOBDResponse[EcuIndex].Sid8TidSupport[IdIndex >> 5].IDBits[2]	    ||
				(  gOBDResponse[EcuIndex].Sid8TidSupport[IdIndex >> 5].IDBits[3] & 0xFE ) ) != 0x00)
			{
				/* Flag as support indicated! */
				ulTIDSupport = 1;
			}
		}
		/* Flag as error if no support indicated in $E0 */
		if (ulTIDSupport == 0x00)
		{
			LogPrint("FAILURE: SID $8 PID $E0 support failure.  No PID support indicated!\n");
			ERROR_RETURN;
		}
	}
	else
	{

		FormatSID8SupReq( ( IdIndex + 0x20 ), &SidReq );

		gIgnoreNoResponse = TRUE;

		if ( SidRequest(&SidReq, SID_REQ_NORMAL) == PASS )
		{
			if (gOBDList[gOBDListIndex].Protocol == ISO15765)
			{
				LogPrint("FAILURE: TC# 5.16.3 - Unexpected response from ECU!\n");
				gIgnoreNoResponse = FALSE;
				return FAIL;
			}
		}

		gIgnoreNoResponse = FALSE;
	}

	/*
	** Per J1699 rev 11.5 TC# 5.16.3 - Verify ECU did not
	** drop out.
	*/
	if (VerifyLinkActive() != PASS)
	{
		return FAIL;
	}

	return PASS;
}

//*****************************************************************************
//  Function:	VerifySid8PidSupportData
//
//	Purpose:	Verify each controller supports at a minimum one PID.
//              Any ECU that responds that does not support at least
//              one PID is flagged as an error.
//
//*****************************************************************************
int VerifySid8PidSupportData (void)
{
	int				bReturn = PASS;
	int             bEcuResult;
	unsigned long	EcuIndex;
	unsigned long   Index;

	/* For each ECU */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		bEcuResult = FAIL;
		for (Index = 0; Index < gOBDResponse[EcuIndex].Sid8TidSupportSize; Index++)
		{
			/* If MID is supported, keep looking */
			if ( ( gOBDResponse[EcuIndex].Sid8TidSupport[Index].IDBits[0]		||
				   gOBDResponse[EcuIndex].Sid8TidSupport[Index].IDBits[1]	    ||
				   gOBDResponse[EcuIndex].Sid8TidSupport[Index].IDBits[2]	    ||
				 ( gOBDResponse[EcuIndex].Sid8TidSupport[Index].IDBits[3] & 0xFE ) ) != 0x00)
			{
				bEcuResult = PASS;
				break;
			}
		}

		if ((bEcuResult == FAIL) && (gOBDResponse[EcuIndex].Sid8TidSupportSize > 0))
		{
			LogPrint ("INFORMATION: ECU %X SID $8 invalid PID supported PIDs", GetEcuId(EcuIndex));
			bReturn = FAIL;
		}
	}

	return bReturn;
}

//*****************************************************************************
//
//	Function:	IsSid8TidSupported
//
//	Purpose:	Determine if SID 8 TID x is supported on specific ECU.
//              Need to have called GetSid8SupportData() previously.
//              If EcuIndex < 0 then check all ECUs.
//
//*****************************************************************************
unsigned int IsSid8TidSupported (unsigned int EcuIndex, unsigned int TidIndex)
{
	int index1;
	int index2;
	int mask;

	if (TidIndex == 0)
	{
		if ((signed int)EcuIndex < 0)
		{
			for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
			{
				if (gOBDResponse[EcuIndex].Sid8TidSupportSize > 0)
					return TRUE;
			}
		}
		else
		{
			if (gOBDResponse[EcuIndex].Sid8TidSupportSize > 0)
				return TRUE;
		}
	}

	TidIndex--;

	index1 =  TidIndex >> 5;
	index2 = (TidIndex >> 3) & 0x03;
	mask   = 0x80 >> (TidIndex & 0x07);

	if ((signed int)EcuIndex < 0)
	{
		for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid8TidSupport[index1].IDBits[index2] & mask)
				return TRUE;
		}
	}
	else
	{
		if (gOBDResponse[EcuIndex].Sid8TidSupport[index1].IDBits[index2] & mask)
			return TRUE;
	}

	return FALSE;
}
