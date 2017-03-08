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
** VerifyDTCPendingData - Function to verify SID7 DTC pending data
*******************************************************************************
*/
STATUS VerifyDTCPendingData(void)
{
	SID_REQ SidReq;
	unsigned long EcuIndex;
	unsigned long NumDTCs;
	unsigned long DataOffset;

	// Clear Error Count for ERROR_RETURN
	ErrorCount();

	/* Request SID 7 data */
	SidReq.SID = 7;
	SidReq.NumIds = 0;
	if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
	{
		/* If ISO15765 or ISO14230 protocol and no response, it is a failure */
		if ( gOBDList[gOBDListIndex].Protocol == ISO15765 )
		{
			LogPrint( "WARNING: SID $7 request failed\n" );
			return(FAIL);
		}

		else if ( ErrorCount() != 0 )
		{
			return(FAIL);
		}

		else
		{
			return(PASS);
		}
	}

	/* Check if a pending DTC is not supposed to be present */
	if (gOBDDTCPending == FALSE)
	{
		/* Verify that PID 7 reports no DTCs pending */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid7Size == 0)
			{
				/* 8/8/04: If ISO15765 protocol every ECU must respond; unless ECU does not support */
				if ( gOBDList[gOBDListIndex].Protocol == ISO14230 )
				{
					LogPrint( "FAILURE: ECU %X  Insufficient data returned for Sid $7 request\n", GetEcuId (EcuIndex) );
					return(FAIL);
				}
				else /* ISO15765; ECU shall not respond if SID $07 not supported */
				{
					LogPrint( "WARNING: ECU %X  SID $07 ECU did not respond; verify SID $7 not supported!\n", GetEcuId (EcuIndex) );
				}
				continue;
			}

			/* If ISO15765 protocol every ECU must respond */
			if (gOBDList[gOBDListIndex].Protocol == ISO15765)
			{
				if (gOBDResponse[EcuIndex].Sid7[0] != 0x00)
				{
					LogPrint( "FAILURE: ECU %X  Sid $7 indicates DTC pending\n", GetEcuId (EcuIndex) );
					return(FAIL);
				}
			}
			else
			{
				/* response should contain multiple of 6 data bytes */
				if (gOBDResponse[EcuIndex].Sid7Size % 6 != 0)
				{
					LogPrint ( "FAILURE: ECU %X  Sid $7 response size error\n", GetEcuId (EcuIndex) );
					return FAIL;
				}

				/* validation of all DTCs reported in response. */
				for ( DataOffset = 0; DataOffset < (unsigned long)(gOBDResponse[EcuIndex].Sid7Size / 2); DataOffset++ )
				{
					if ( ( gOBDResponse[EcuIndex].Sid7[DataOffset*2] != 0x00) || ( gOBDResponse[EcuIndex].Sid7[DataOffset*2+1] != 0x00))
					{
						LogPrint( "FAILURE: ECU %X  Sid $7 indicates DTC pending\n", GetEcuId (EcuIndex) );
						return(FAIL);
					}
				}
			}
		}
	}
	else
	{
		/* Verify that SID 7 reports DTCs pending */
		NumDTCs = 0;
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid7Size == 0)
			{
				/* 8/8/04: If ISO15765 protocol every ECU must respond; unless ECU does not support */
				if ( gOBDList[gOBDListIndex].Protocol  == ISO14230 )
				{
					LogPrint( "FAILURE: ECU %X  Insufficient data returned for Sid $7 request\n", GetEcuId (EcuIndex) );
					return(FAIL);
				}
				else /* ISO15765  ECU shall not respond if SID $7 not supported */
				{
					LogPrint( "WARNING: ECU %X  SID $7 pending - ECU did not respond; verify SID $7 not supported!\n", GetEcuId (EcuIndex) );
				}
				continue;
			}

			/* Print out all the DTCs */
			for (DataOffset = 0; DataOffset < gOBDResponse[EcuIndex].Sid7Size; DataOffset += 2)
			{
				if ((gOBDResponse[EcuIndex].Sid7[DataOffset] != 0) ||
				(gOBDResponse[EcuIndex].Sid7[DataOffset + 1] != 0))
				{
					/* Process based on the type of DTC */
					switch (gOBDResponse[EcuIndex].Sid7[DataOffset] & 0xC0)
					{
						case 0x00:
						{
							LogPrint("INFORMATION: Pending DTC P%02X%02X detected\n",
							gOBDResponse[EcuIndex].Sid7[DataOffset] & 0x3F,
							gOBDResponse[EcuIndex].Sid7[DataOffset + 1]);
						}
						break;
						case 0x40:
						{
							LogPrint("INFORMATION: Pending DTC C%02X%02X detected\n",
							gOBDResponse[EcuIndex].Sid7[DataOffset] & 0x3F,
							gOBDResponse[EcuIndex].Sid7[DataOffset + 1]);
						}
						break;
						case 0x80:
						{
							LogPrint("INFORMATION: Pending DTC B%02X%02X detected\n",
							gOBDResponse[EcuIndex].Sid7[DataOffset] & 0x3F,
							gOBDResponse[EcuIndex].Sid7[DataOffset + 1]);
						}
						break;
						case 0xC0:
						{
							LogPrint("INFORMATION: Pending DTC U%02X%02X detected\n",
							gOBDResponse[EcuIndex].Sid7[DataOffset] & 0x3F,
							gOBDResponse[EcuIndex].Sid7[DataOffset + 1]);
						}
						break;
					}
					NumDTCs++;
				}
			}
		}

		/* If no pending DTCs found, then fail */
		if (NumDTCs == 0)
		{
			LogPrint( "FAILURE: No ECU indicates SID $7 DTC pending\n" );
			return(FAIL);
		}
	}

	/* Link active test to verify communication remained active for ALL protocols
	 */
	if (VerifyLinkActive() != PASS)
	{
		return( FAIL );
	}

	if ( ErrorCount() != 0 )
	{
		return(FAIL);
	}

	return(PASS);
}
