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
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

/*
*******************************************************************************
** VerifyGroupFreezeFrameSupport -
** Function to verify SID2 group support (ISO15765 only)
*******************************************************************************
* DATE      MODIFICATION
* 05/05/04  Enhanced to request group support for PID $60, $80, & $A0 and
*           PID $C0 & $E0 if applicable.
* 05/08/04  Corrected incorrect index to 'Sid1PidSupport', correct index
*			is 'Sid2PidSupport'.  Logic used to determine if next group
*           is to be requested.
*******************************************************************************
*/
STATUS VerifyGroupFreezeFrameSupport(void)
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	unsigned long IdBitIndex;
	SID_REQ SidReq;

	int bReqNextGroup;		/* Binary logic for request for next group! */

	/* Save the original response data for comparison */
	memcpy(&gOBDCompareResponse[0], &gOBDResponse[0], sizeof(gOBDResponse));

	/* Request SID 2 support data as a group */
	SidReq.SID = 2;
	SidReq.NumIds = 6;
	SidReq.Ids[0] = 0x00;
	SidReq.Ids[1] = 0x00;
	SidReq.Ids[2] = 0x20;
	SidReq.Ids[3] = 0x00;
	SidReq.Ids[4] = 0x40;
	SidReq.Ids[5] = 0x00;
	if (SidRequest(&SidReq, SID_REQ_ALLOW_NO_RESPONSE) != PASS)
	{
		LogPrint("FAILURE: SID $2 group support request failed\n");
		return(FAIL);
	}

	/* Set next group select to false */
	bReqNextGroup = 0;

	/* Determine if next group support request is required */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		/* Index 0x02 corresponds to 0x40 */
		if ( gOBDResponse[EcuIndex].Sid2PidSupport[2].IDBits[3] & 0x01 )
		{
			bReqNextGroup = 1;
		}
	}

	/* Request next group if supported by previous request! */
	if ( bReqNextGroup == 1 )
	{
		/* Request SID 2 support data as a group */
		SidReq.SID = 2;
		SidReq.NumIds = 6;
		SidReq.Ids[0] = 0x60;
		SidReq.Ids[1] = 0x00;
		SidReq.Ids[2] = 0x80;
		SidReq.Ids[3] = 0x00;
		SidReq.Ids[4] = 0xA0;
		SidReq.Ids[5] = 0x00;
		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			LogPrint("FAILURE: SID $2 group support request failed\n");
			return(FAIL);
		}

		/* Set next group select to false */
		bReqNextGroup = 0;

		/* Determine if next group support request is required */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			/* Index 0x06 corresponds to 0xA0 */
			if ( gOBDResponse[EcuIndex].Sid2PidSupport[6].IDBits[3] & 0x01 )
			{
				bReqNextGroup = 1;
			}
		}

		/* Request next group if supported by previous request! */
		/*
		** 05/08/03 - Added condition to request only if previous
		** response indicated support.
		*/
		if ( bReqNextGroup == 1 )
		{
			/* Request SID 2 support data as a group */
			SidReq.SID = 2;
			SidReq.NumIds = 4;
			SidReq.Ids[0] = 0xC0;
			SidReq.Ids[1] = 0x00;
			SidReq.Ids[2] = 0xE0;
			SidReq.Ids[3] = 0x00;
			if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
			{
				LogPrint("FAILURE: SID $2 group support request failed\n");
				return(FAIL);
			}
		}

	}
	/* Verify that all SID 2 PID support data compares */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		if ( gOBDResponse[EcuIndex].Sid2PidSupportSize !=
		     gOBDCompareResponse[EcuIndex].Sid2PidSupportSize )
		{
			LogPrint("FAILURE: ECU %X  SID $2 group/individual support response size mismatch\n", GetEcuId(EcuIndex) );
			return(FAIL);
		}

		for (IdIndex = 0; IdIndex < 8; IdIndex++)
		{
			for (IdBitIndex = 0; IdBitIndex < 4; IdBitIndex++)
			{
				if (gOBDResponse[EcuIndex].Sid2PidSupport[IdIndex].IDBits[IdBitIndex] !=
				gOBDCompareResponse[EcuIndex].Sid2PidSupport[IdIndex].IDBits[IdBitIndex])
				{
					LogPrint("FAILURE: ECU %X  SID $2 group/individual support response mismatch\n", GetEcuId(EcuIndex) );
					return(FAIL);
				}
			}
		}
	}

	return (PASS);
}

/*
*******************************************************************************
** VerifyGroupFreezeFrameResponse -
** Function to verify SID2 group response (ISO15765 only)
*******************************************************************************
*/
STATUS VerifyGroupFreezeFrameResponse (void)
{
	unsigned long  EcuIndex;
	unsigned long  IdIndex;
	unsigned long  GroupIndex;
	unsigned long  SingleIndex;
	unsigned long  ByteIndex;
	unsigned long  Index;
	unsigned long  NumGroupIds;

	SID_REQ SidReq, SidGroupReq;

	/* alloc enough memory for three (max size) Sid2 responses */
	unsigned char  Sid2Single[sizeof(SID2)*3], Sid2Group[sizeof(SID2)*3];
	unsigned short Sid2Size[3], Sid2GroupSize;

	/* test each ECU seperately */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		SidGroupReq.SID = 2;
		SidGroupReq.NumIds = 0;

		/* collect first 3 supported PIDs for current ECU */
		for (IdIndex = 1, NumGroupIds = 0; IdIndex < 256; IdIndex++)
		{
			if (IsSid2PidSupported (EcuIndex, IdIndex) == TRUE)
			{
				SidGroupReq.Ids[SidGroupReq.NumIds++] = (unsigned char)IdIndex;
				SidGroupReq.Ids[SidGroupReq.NumIds++] = 0x00;
				if (++NumGroupIds == 3)
					break;
			}
		}

		if (NumGroupIds == 0)
			continue;

		/* do group request */
		if (SidRequest (&SidGroupReq, SID_REQ_NORMAL) != PASS)
		{
			LogPrint("FAILURE: SID $2 group request failed\n");
			return(FAIL);
		}

		/* save the original group response data for comparison */
		Sid2GroupSize = gOBDResponse[EcuIndex].Sid2PidSize;
		memcpy (Sid2Group, gOBDResponse[EcuIndex].Sid2Pid, Sid2GroupSize);

		/* request each PID seperately */
		for (IdIndex = 0, ByteIndex = 0; IdIndex < NumGroupIds; IdIndex++)
		{
			SidReq.SID    = 2;
			SidReq.NumIds = 2;
			SidReq.Ids[0] = SidGroupReq.Ids[IdIndex * 2];
			SidReq.Ids[1] = 0; /* Frame #0 */

			if (SidRequest (&SidReq, SID_REQ_ALLOW_NO_RESPONSE) != PASS)
			{
				LogPrint("FAILURE: SID $2 PID request failed\n");
				return (FAIL);
			}

			/* save single response data */
			Sid2Size[IdIndex] = gOBDResponse[EcuIndex].Sid2PidSize;
			memcpy (&Sid2Single[ByteIndex], gOBDResponse[EcuIndex].Sid2Pid, Sid2Size[IdIndex]);
			ByteIndex += Sid2Size[IdIndex];
		}

		/* check total data */
		if (ByteIndex != Sid2GroupSize)
		{
			LogPrint("FAILURE: ECU %X  SID $2 PID group request length (%d) / individual request length (%d) mismatch\n", GetEcuId(EcuIndex), Sid2GroupSize, ByteIndex);
			return (FAIL);
		}

		/* compare PID message data */
		for (Index = 0, GroupIndex = 0; (Index < NumGroupIds) && (GroupIndex < Sid2GroupSize); Index++)
		{
			for (IdIndex = 0, SingleIndex = 0; (IdIndex < NumGroupIds) && (SingleIndex < Sid2GroupSize); IdIndex++)
			{
				if (Sid2Group[GroupIndex] == Sid2Single[SingleIndex])
				{
					if (memcmp (&Sid2Group[GroupIndex], &Sid2Single[SingleIndex], Sid2Size[IdIndex]) != 0)
					{
						LogPrint ("FAILURE: ECU %X  SID $2 PID group data / individual data mismatch\n", GetEcuId(EcuIndex) );
						return (FAIL);
					}
					break;
				}

				SingleIndex += Sid2Size[IdIndex];
			}

			if (IdIndex >= NumGroupIds)
			{
				LogPrint ("FAILURE: ECU %X  SID $2 PID group response / individual response mismatch\n", GetEcuId(EcuIndex) );
				return (FAIL);
			}

			GroupIndex += Sid2Size[IdIndex];
		}
	} // end for (EcuIndex

	return PASS;
}
