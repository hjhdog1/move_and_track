/*
********************************************************************************
** SAE J1699-3 Test Source Code
**
**  Copyright (C) 2007. http://j1699-3.sourceforge.net/
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
*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

BOOL gService0ASupported = FALSE;

/*
*******************************************************************************
** VerifyPermanentCodeSupport -
** Function to verify SID A support
*******************************************************************************
*/
STATUS VerifyPermanentCodeSupport(void)
{
	unsigned long EcuIndex;
	SID_REQ SidReq;
	unsigned long NumDTCs;
	unsigned char SavedFlag;
	unsigned char TestFailed = FALSE;

	const unsigned long NumEcusSave = gOBDNumEcus;       /* save gOBDNumEcus */

	if ( gOBDList[gOBDListIndex].Protocol != ISO15765 )
	{
		return(PASS);

	}

	SavedFlag = gIgnoreNoResponse;
	gIgnoreNoResponse = TRUE;

	/* Count number of ECUs responding to SID A */
	SidReq.SID = 0x0A;
	SidReq.NumIds = 0;
	if ( SidRequest( &SidReq, SID_REQ_NORMAL ) != PASS )
	{
		gIgnoreNoResponse = SavedFlag;

		/* An ECU supports Service 0A */
		if ( gOBDNumEcusResp > 0 )
		{
			gService0ASupported = TRUE;
		}

		/* Permanent code support required for MY 2010 and beyond */
		if ( gModelYear >= 2010 )
		{
			LogPrint("FAILURE: SID $0A request failed\n");
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

	gIgnoreNoResponse = SavedFlag;


	/* An ECU supports Service 0A */
	if ( gOBDNumEcusResp > 0 )
	{
		gService0ASupported = TRUE;
	}


	/* If a permanent DTC is not supposed to be present */
	if ( gOBDDTCPermanent == FALSE )
	{
		/* Verify that SID A reports no DTCs stored */
		for ( EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++ )
		{
			/* Every ECU must respond */
			if ( gOBDResponse[EcuIndex].SidA[0] != 0x00 )
			{
				LogPrint("FAILURE: ECU %X  SID $0A reports Permanent DTC stored\n", GetEcuId(EcuIndex) );
				return(FAIL);
			}
		}
	}

	/* If a permanent DTC is supposed to be present */
	else
	{
		/* Verify that SID A reports DTCs stored */
		NumDTCs = 0;
		for ( EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++ )
		{
			NumDTCs += gOBDResponse[EcuIndex].SidASize;
		}

		/* If no stored DTCs found, then fail */
		if ( NumDTCs == 0 )
		{
			if ( gModelYear >= 2010 )
			{
				LogPrint("FAILURE: All ECUs  SID $0A report no Permanent DTC stored\n");
				return(FAIL);
			}
			else
			{
				LogPrint("WARNING: All ECUs  SID $0A report no Permanent DTC stored\n");
			}
		}
	}

	/* if this is test 8.6 and MY 2010 and beyond */
	if ( TestPhase == eTestFaultRepaired && gModelYear >= 2010 )
	{
		for ( EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++ )
		{
			if ( ( gOBDResponse[EcuIndex].Sid3Supported != 0 ||
			       gOBDResponse[EcuIndex].Sid7Supported != 0 ) &&
			     gOBDResponse[EcuIndex].SidASupported == 0 )
			{
				LogPrint("FAILURE: ECU %X  Supports SID $03 and SID $07 but not SID $0A\n", GetEcuId(EcuIndex));
				TestFailed = TRUE;
			}
		}

		if ( TestFailed == TRUE)
		{
			return(FAIL);
		}
	}

	return(PASS);
}
