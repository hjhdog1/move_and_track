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
** VerifyMILData - Function to verify OBD MIL / DTC status
*******************************************************************************
*/
STATUS VerifyMILData(void)
{
	SID_REQ SidReq;
	unsigned long NumOfECUsWithDTCs;
	unsigned long NumOfECUsWithMILAndDTCs;
	unsigned long EcuIndex;


	/* Request SID 1 PID 1 data */
	SidReq.SID = 1;
	SidReq.NumIds = 1;
	SidReq.Ids[0] = 1;
	if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
	{
		LogPrint("FAILURE: SID $01 PID $01 request failed\n");
		return(FAIL);
	}

	/* Record MIL status to log */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		if ( IsSid1PidSupported (EcuIndex, 0x01) == TRUE )
		{
			/* If PID 1 supported, there should be data */
			if ((gOBDResponse[EcuIndex].Sid1Pid[1] & 0x80) == 0x80)
			{
				LogPrint("INFORMATION: ECU %X  MIL Illuminated\n", GetEcuId(EcuIndex) );
			}
			else
			{
				LogPrint("INFORMATION: ECU %X  MIL NOT Illuminated\n", GetEcuId(EcuIndex) );
			}
		}
		else
		{
			LogPrint("INFORMATION: ECU %X  SID $01 PID $01 not supported\n", GetEcuId(EcuIndex) );
		}
	}

	/* Check if a DTC is supposed to be present */
	if (gOBDDTCStored == FALSE)
	{
		/* Verify MIL/DTC status for every ECU that responded */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			/* check for PID 1 support */
			if (IsSid1PidSupported (EcuIndex, 1) == TRUE)
			{
				/* PID 1 supported, check for data */
				if (gOBDResponse[EcuIndex].Sid1PidSize == 0)
				{
					LogPrint("FAILURE: ECU %X  Insufficient data returned for SID $01 request\n", GetEcuId(EcuIndex) );
					return(FAIL);
				}

				if (gOBDResponse[EcuIndex].Sid1Pid[1] != 0x00)
				{
					LogPrint("FAILURE: ECU %X  MIL/DTC status bit(s) set\n", GetEcuId(EcuIndex) );
					return(FAIL);
				}
			}
		}
	}
	else
	{
		/* Verify that SID 1 reports a DTC stored */
		for ( EcuIndex = 0, NumOfECUsWithDTCs = 0, NumOfECUsWithMILAndDTCs = 0;
			  EcuIndex < gOBDNumEcus;
			  EcuIndex++ )
		{
			/* If PID 1 supported, there should be data */
			if ( (IsSid1PidSupported (EcuIndex, 1) == TRUE) &&
			     (gOBDResponse[EcuIndex].Sid1PidSize == 0))
			{
				LogPrint("FAILURE: ECU %X  Insufficient data returned for SID $01 request\n", GetEcuId(EcuIndex) );
				return(FAIL);
			}

			/* Count ECUs with at least one DTC */
			if ((gOBDResponse[EcuIndex].Sid1Pid[1] & 0x7F) != 0x00)
			{
				NumOfECUsWithDTCs++;

				/* Count ECUs with at least one DTC and MIL is lit */
				if ((gOBDResponse[EcuIndex].Sid1Pid[1] & 0x80) != 0x00)
				{
					NumOfECUsWithMILAndDTCs++;
				}
			}
		}

		/* Check if any ECU reported a DTC */
		if (NumOfECUsWithDTCs == 0)
		{
			LogPrint("FAILURE: SID $01 indicates no ECU has DTC stored\n");
			return(FAIL);
		}

		if (gOBDDTCHistorical == TRUE)
		{
			/* If the DTC is historical, make sure MIL is off */
			if (NumOfECUsWithMILAndDTCs > 0)
			{
				LogPrint("WARNING: SID $01 indicates MIL light ON after 3 driving cycles with fault repaired\n");
			}
		}
		else
		{
			/* If the DTC is not historical, make sure MIL is on */
			if (NumOfECUsWithMILAndDTCs == 0)
			{
				LogPrint("FAILURE: SID $01 indicates MIL light OFF before 3 driving cycles with fault repaired\n");
				return(FAIL);
			}
		}
	}

	/* 09/15/04 - Test case 5.4.1, verify that at least 1 ECU responded but
	**                             not more then the total specified by the user.
	*/
	if ( gOBDEngineRunning == FALSE )
	{
		if ( ( gOBDNumEcusResp == 0 ) || ( gOBDNumEcusResp > gUserNumEcus ) )
		{
			LogPrint( "FAILURE: Number of SID $01 responses in error\n" );
			return(FAIL);
		}
	}

	return(PASS);
}
