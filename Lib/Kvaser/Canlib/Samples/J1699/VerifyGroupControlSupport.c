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
** 05/08/04      Added logic to transmit $C0 & $E0, then not results.  As
**               defined in test case 5.7.2 of draft spec 11.4.
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
** VerifyGroupControlSupport -
** Function to verify SID8 group support (ISO15765 only)
*******************************************************************************
*/
STATUS VerifyGroupControlSupport(void)
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	unsigned long IdBitIndex;
	SID_REQ SidReq;

	int bReqNextGroup;		/* Binary logic for request for next group! */

	/* Save the original response data for comparison */
	memcpy(&gOBDCompareResponse[0], &gOBDResponse[0], sizeof(gOBDResponse));

	/* Request SID 8 support data as a group */
	SidReq.SID = 8;
	SidReq.NumIds = 6;
	SidReq.Ids[0] = 0x00;
	SidReq.Ids[1] = 0x20;
	SidReq.Ids[2] = 0x40;
	SidReq.Ids[3] = 0x60;
	SidReq.Ids[4] = 0x80;
	SidReq.Ids[5] = 0xA0;
	if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
	{
		LogPrint("FAILURE: Sid8 group support request failed\n");
		return(FAIL);
	}
	/* Set next group select to false */
	bReqNextGroup = 0;

	/* Determine if next group support request is required */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		/* Index 0x05 corresponds to 0xA0 */
		if ( gOBDResponse[EcuIndex].Sid8TidSupport[5].IDBits[3] & 0x01 )
		{
			bReqNextGroup = 1;
		}
	}

	/* Request next group if supported by previous request! */
	if ( bReqNextGroup == 1 )
	{
		/**************************************************************************/
		/* 05/08/04 - Test support for $C0 & $E0 as specified in draft spec 11.4. */
		/**************************************************************************/
		/* Request SID 8 support data as a group */
		SidReq.SID = 8;
		SidReq.NumIds = 2;
		SidReq.Ids[0] = 0xC0;
		SidReq.Ids[1] = 0xE0;
		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			LogPrint( "FAILURE: SID $8 group support request failed\n" );
			return(FAIL);
		}
	}

	/* Verify that all SID 8 TID support data compares */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		if ( gOBDResponse[EcuIndex].Sid8TidSupportSize !=
		     gOBDCompareResponse[EcuIndex].Sid8TidSupportSize )
		{
			LogPrint( "FAILURE: ECU %X  SID $8 group/individual support response size mismatch\n", GetEcuId(EcuIndex) );
			return(FAIL);
		}

		for (IdIndex = 0; IdIndex < 8; IdIndex++)
		{
			for (IdBitIndex = 0; IdBitIndex < 4; IdBitIndex++)
			{
				if (gOBDResponse[EcuIndex].Sid8TidSupport[IdIndex].IDBits[IdBitIndex] !=
				gOBDCompareResponse[EcuIndex].Sid8TidSupport[IdIndex].IDBits[IdBitIndex])
				{
					LogPrint( "FAILURE: ECU %X  SID $8 group support request failed\n", GetEcuId(EcuIndex) );
					return(FAIL);
				}
			}
		}
	}

	return(PASS);
}
