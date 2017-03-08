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
* DATE      MODIFICATION
* 05/05/04  Added function per draft specification J1699 rev 11.4 test case
*           5.18.1.
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
** VerifyReverseGroupDiagnosticSupport -
** Function to verify SID1 group support (ISO15765 only)
*******************************************************************************
*/
STATUS VerifyReverseGroupDiagnosticSupport(void)
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	unsigned long IdBitIndex;
	SID_REQ SidReq;

	/* Request SID 1 support data as a group */
	SidReq.SID = 1;
	SidReq.NumIds = 2;
	SidReq.Ids[0] = 0xE0;
	SidReq.Ids[1] = 0xC0;
	if (SidRequest(&SidReq, SID_REQ_ALLOW_NO_RESPONSE) != PASS)
	{
		LogPrint("FAILURE: SID $1 group support request failed\n");
		return(FAIL);
	}

	/* Request SID 1 support data as a group */
	SidReq.SID = 1;
	SidReq.NumIds = 6;
	SidReq.Ids[0] = 0xA0;
	SidReq.Ids[1] = 0x80;
	SidReq.Ids[2] = 0x60;
	SidReq.Ids[3] = 0x40;
	SidReq.Ids[4] = 0x20;
	SidReq.Ids[5] = 0x00;
	if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
	{
		LogPrint("FAILURE: SID $1 group support request failed\n");
		return(FAIL);
	}

	/* Verify that all SID 1 PID support data compares */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		for (IdIndex = 0; IdIndex < 8; IdIndex++)
		{
			for (IdBitIndex = 0; IdBitIndex < 4; IdBitIndex++)
			{
				if ( gOBDResponse[EcuIndex].Sid1PidSupport[IdIndex].IDBits[IdBitIndex] !=
					 gOBDCompareResponse[EcuIndex].Sid1PidSupport[IdIndex].IDBits[IdBitIndex] )
				{
					LogPrint("FAILURE: ECU %X  SID $1 reverse group support request failed\n", GetEcuId(EcuIndex) );
					return(FAIL);
				}
			}
		}
	}

	/* Request last group of six PIDs supported by each ECU */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		SidReq.SID = 1;
		SidReq.NumIds = 0;
		for (IdIndex = 0xFF; (IdIndex > 0) && (SidReq.NumIds < 6); IdIndex--)
		{
			/* Don't use PID supported PIDs */
			if (
				(IdIndex != 0x00) && (IdIndex != 0x20) && (IdIndex != 0x40) &&
				(IdIndex != 0x60) && (IdIndex != 0x80) && (IdIndex != 0xA0) &&
				(IdIndex != 0xC0) && (IdIndex != 0xE0)
				)
			{
				/* If PID is supported, request it */
				if (IsSid1PidSupported (EcuIndex, IdIndex) == TRUE)
				{
					SidReq.Ids[SidReq.NumIds++] = (unsigned char)IdIndex;
				}
			}
		}

		/* If ECU supports PIDS then request them */
		if (SidReq.NumIds != 0)
		{
			if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
			{
				LogPrint("FAILURE: Sid1 group request failed\n");
				return(FAIL);
			}
		}
	}

	return(PASS);
}

