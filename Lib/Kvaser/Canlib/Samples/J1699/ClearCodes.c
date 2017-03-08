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
***************************************************************************************************
** ClearCodes - Function to clear OBD diagnostic information
***************************************************************************************************
*/
STATUS ClearCodes(void)
{
	SID_REQ       SidReq;
	unsigned long EcuIndex;
	unsigned short number_pos_ack = 0;

	/* Reset the global DTC data */
	gOBDDTCPending = FALSE;
	gOBDDTCStored  = FALSE;

	/* Send SID 4 request */
	SidReq.SID = 4;
	SidReq.NumIds = 0;
	if ((SidRequest(&SidReq, SID_REQ_NORMAL) != PASS) && (gOBDEngineRunning == FALSE))
	{
		return FAIL;
	}

	/* Delay after request to allow time for codes to clear */
	Sleep (CLEAR_CODES_DELAY_MSEC);

	/* 06/17/04 - Test case 5.4.1, Compare Mode 4 response to number of specified ECUs KOEO */
	if (gOBDEngineRunning == FALSE)
	{
		if ( ( gOBDNumEcusResp == 0 ) ||
			 ( gOBDNumEcusResp > gUserNumEcus ) )
		{
			LogPrint( "FAILURE: Number of Service $04 responses in error\n" );
			return FAIL;
		}
	}


	/* all ECUs must respond with the same ackowledgement (positive or negative) with engine running */
	for (EcuIndex=0; EcuIndex<gUserNumEcus; EcuIndex++)
	{
		if (gOBDResponse[EcuIndex].Sid4Size > 0)
		{
			if (gOBDResponse[EcuIndex].Sid4[0] == 0x44)
			{
				number_pos_ack++;
			}
		}
	}


	/* Test 9.6, check positive/negative responses with engine running */
	if (gOBDEngineRunning == TRUE)
	{
		if ( number_pos_ack > 0 && number_pos_ack < gOBDNumEcusResp )
		{
			LogPrint( "WARNING: received both positive and negative responses from ECUs\n" );
		}
	}
	/* Test 5.3, 10.4 and 11.10 check for negative responses with engine off on 15765 */
	else
	{
		if ( gOBDList[gOBDListIndex].Protocol == ISO15765 && number_pos_ack < gOBDNumEcusResp )
		{
			LogPrint( "FAILURE: received a negative response from ECU\n" );
		}
	}
	return PASS;
}
