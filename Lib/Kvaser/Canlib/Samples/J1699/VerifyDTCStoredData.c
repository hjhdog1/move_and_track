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
** DATE         MODIFICATION
** 07/20/04     Integrate enhancements to allow ISO14230 verfication as
**              specified in SAEJ1699 Rev 11.6
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
** VerifyDTCStoredData - Function to verify SID3 DTC stored data
**
*******************************************************************************
*/
STATUS VerifyDTCStoredData(void)
{
	SID_REQ SidReq;
	unsigned long EcuIndex;
	unsigned long NumDTCs;
	unsigned long DataOffset;

	// Clear Error Count for ERROR_RETURN
	ErrorCount();

	/* Request SID 3 data */
	SidReq.SID = 3;
	SidReq.NumIds = 0;
	if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
	{
		/* If DTC stored or historical DTC or ISO15765 protocol, there must be a response */
		if ( gOBDDTCStored == TRUE || gOBDDTCHistorical == TRUE ||
			 gOBDList[gOBDListIndex].Protocol == ISO15765 )
		{
			LogPrint("WARNING: SID $3 request failed\n");
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

	/* Check if a DTC is not supposed to be present */
	if ((gOBDDTCStored == FALSE) && (gOBDDTCHistorical == FALSE))
	{
		/* Verify that SID 3 reports no DTCs stored */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid3Size == 0)
			{
				/* 8/8/04: If ISO15765 protocol every ECU must respond; unless ECU does not support */
				if ( gOBDList[gOBDListIndex].Protocol  == ISO14230 )
				{
					LogPrint( "FAILURE: ECU %X  Insufficient data returned for SID $3 request\n", GetEcuId (EcuIndex) );
					return(FAIL);
				}
				else /* ISO15765; ECU shall not respond if SID $03 not supported */
				{
					LogPrint( "WARNING: ECU %X  SID $3 - ECU did not respond; verify SID $3 not supported!\n", GetEcuId (EcuIndex) );
				}
				continue;
			}

			/* If ISO15765 protocol every ECU must respond */
			if (gOBDList[gOBDListIndex].Protocol == ISO15765)
			{
				if (gOBDResponse[EcuIndex].Sid3[0] != 0x00)
				{
					LogPrint( "FAILURE: ECU %X  SID $3 indicates DTC stored\n", GetEcuId (EcuIndex) );
					return(FAIL);
				}
			}
			else
			{
				/* response should contain multiple of 6 data bytes */
				if (gOBDResponse[EcuIndex].Sid3Size % 6 != 0)
				{
					LogPrint ( "FAILURE: ECU %X  SID $3 response size error\n", GetEcuId (EcuIndex) );
					return FAIL;
				}

				/* validation of all DTCs reported in response */
				for ( DataOffset = 0; DataOffset < (unsigned long)(gOBDResponse[EcuIndex].Sid3Size / 2); DataOffset++ )
				{
					if ( ( gOBDResponse[EcuIndex].Sid3[DataOffset*2]   != 0x00 ) ||
						 ( gOBDResponse[EcuIndex].Sid3[DataOffset*2+1] != 0x00 ) )
					{
						LogPrint( "FAILURE: ECU %X  SID $3 indicates DTC stored\n", GetEcuId (EcuIndex) );
						return(FAIL);
					}
				}
			}
		}
	}
	else
	{
		/* Verify that SID 3 reports DTCs stored */
		NumDTCs = 0;
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid3Size == 0)
			{
				/* 8/8/04: If ISO15765 protocol every ECU must respond; unless ECU does not support */
				if ( gOBDList[gOBDListIndex].Protocol  == ISO14230 )
				{
					LogPrint( "FAILURE: ECU %X  Insufficient data returned for SID $3 request\n", GetEcuId (EcuIndex) );
					return(FAIL);
				}
				else /* ISO15765; ECU shall not respond if SID $03 not supported */
				{
					LogPrint( "WARNING: ECU %X  SID $3 confirmed - ECU did not respond; verify SID $3 not supported!\n", GetEcuId (EcuIndex) );
				}
				continue;
			}

			/* Print out all the DTCs */
			for (DataOffset = 0; DataOffset < gOBDResponse[EcuIndex].Sid3Size; DataOffset += 2)
			{
				if ((gOBDResponse[EcuIndex].Sid3[DataOffset] != 0) ||
				(gOBDResponse[EcuIndex].Sid3[DataOffset + 1] != 0))
				{
					/* Process based on the type of DTC */
					switch (gOBDResponse[EcuIndex].Sid3[DataOffset] & 0xC0)
					{
						case 0x00:
						{
							LogPrint("INFORMATION: Stored DTC P%02X%02X detected\n",
							gOBDResponse[EcuIndex].Sid3[DataOffset] & 0x3F,
							gOBDResponse[EcuIndex].Sid3[DataOffset + 1]);
						}
						break;
						case 0x40:
						{
							LogPrint("INFORMATION: Stored DTC C%02X%02X detected\n",
							gOBDResponse[EcuIndex].Sid3[DataOffset] & 0x3F,
							gOBDResponse[EcuIndex].Sid3[DataOffset + 1]);
						}
						break;
						case 0x80:
						{
							LogPrint("INFORMATION: Stored DTC B%02X%02X detected\n",
							gOBDResponse[EcuIndex].Sid3[DataOffset] & 0x3F,
							gOBDResponse[EcuIndex].Sid3[DataOffset + 1]);
						}
						break;
						case 0xC0:
						{
							LogPrint("INFORMATION: Stored DTC U%02X%02X detected\n",
							gOBDResponse[EcuIndex].Sid3[DataOffset] & 0x3F,
							gOBDResponse[EcuIndex].Sid3[DataOffset + 1]);
						}
						break;
					}
					NumDTCs++;
				}
			}
		}

		/* If no stored DTCs found, then fail */
		if (NumDTCs == 0)
		{
			LogPrint( "FAILURE: No ECU indicates SID $3 DTC stored\n" );
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
