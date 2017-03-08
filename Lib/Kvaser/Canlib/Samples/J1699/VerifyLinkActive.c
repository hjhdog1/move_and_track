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
**	DATA		MODIFICATION
**	05/12/04	Enhanced logic to fail when an expeced ECU does not resond.
**	06/24/04	Check verifying support of PID $01 not requred to determine
**              if controller responded.
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

/******************************************************
* Partial fix for SF# 1719664                        */
BOOL gVerifyLink;   // flag to indicate SidResetResponseData (called by SidRequest) should not clear all pid supported pids
/*****************************************************/

/*
*******************************************************************************
** VerifyLinkActive - Function to see if link remained active.
*******************************************************************************
*/
STATUS VerifyLinkActive(void)
{

	SID_REQ			SidReq;
	unsigned long	EcuIndex = 0;
	unsigned long	EcuRespCnt = 0;

	LogPrint("\n\nINFORMATION: Verify Link Active\n");

    gVerifyLink = TRUE;
	
	/* Check if SID 1 PID 0 supported */
	SidReq.SID		= 1;
	SidReq.NumIds	= 1;
	SidReq.Ids[0]	= 0;
	if ((SidRequest(&SidReq, SID_REQ_NORMAL) == PASS) )
	{
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if ( gOBDResponse[EcuIndex].Sid1PidSupportSize > 0 )
			{
				/* We've found an OBD supported protocol */
				LogPrint("INFORMATION: Link Active on OBD %s protocol, ECU %X\n", gOBDList[gOBDListIndex].Name, GetEcuId(EcuIndex) );
				EcuRespCnt++;
			}
			else
			{
				LogPrint( "INFORMATION: No Response, ECU %X!\n", GetEcuId(EcuIndex)  );
			}
		}
	}

    gVerifyLink = FALSE;
	
	/* Verify that all controllers responded. */
	if ( EcuRespCnt == gOBDNumEcus )
	{
		LogPrint("\nINFORMATION: Link Active\n");
		return(PASS);
	}
	else
	{
		LogPrint("FAILURE: Expected number of controllers did not respond!\n");
		return(FAIL);
	}

}
