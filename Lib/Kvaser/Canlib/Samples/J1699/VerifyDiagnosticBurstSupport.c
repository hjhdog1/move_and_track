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
**	DATE		MODIFICATION
**	05/10/04	Added logic to ensure that the data link has remained active 
**				with all expected controllers responding.
**	05/17/04	Added response total counter.  Added per request of J1699
**				workgroup.
*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

/*
*******************************************************************************
** VerifyDiagnosticBurstSupport -
** Function to verify SID1 diagnostic burst support
*******************************************************************************
*/
STATUS VerifyDiagnosticBurstSupport(void)
{
    SID_REQ SidReq;
    unsigned long StartTimeMsecs;
	unsigned long ulRespTotal = 0;	/* 5/17/04 - Added to ID total responses */
    unsigned long nPid0EcuCount, nPid1EcuCount;
    
    const unsigned long NumEcusSave = gOBDNumEcus;       /* save gOBDNumEcus */

    /*
    ** Count number of ECUs responding to SID 1 PID 0
    */
    SidReq.SID = 1;
    SidReq.NumIds = 1;
    SidReq.Ids[0] = 0;
    if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
    {
        LogPrint("FAILURE: Sid1 Pid0 burst support failed\n");
        return (FAIL);
    }

    nPid0EcuCount = gOBDNumEcusResp;

    /*
    ** Count number of ECUs responding to SID 1 PID 1
    */
    SidReq.SID = 1;
    SidReq.NumIds = 1;
    SidReq.Ids[0] = 1;
    if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
    {
        LogPrint("FAILURE: Sid1 Pid1 burst support failed\n");
        return (FAIL);
    }

    nPid1EcuCount = gOBDNumEcusResp;

    /*
    ** If no ECU supports both PID 0 and PID 1, then cannot run test
    */
    if (nPid0EcuCount == 0 && nPid1EcuCount == 0)
        return (PASS);
    
    /*
    ** Stop the tester present message before burst test since we are using the message
    ** in the burst test
    */
    if (StopPeriodicMsg (TRUE) != PASS)
    {
        return (FAIL);
    }

	/*
	** Wait for possible race conditions.
	** SidRequest will flush the queue.
	*/
    Sleep (gOBDRequestDelay);

    /* Request SID 1 PID 0x00 and PID 0x01 in alternating order for 5 seconds */
    SidReq.Ids[0] = 1;
    StartTimeMsecs = GetTickCount();
    while ((GetTickCount() - StartTimeMsecs) < 5000)
    {
        SidReq.SID = 1;
        SidReq.NumIds = 1;
        SidReq.Ids[0]++;
        SidReq.Ids[0] &= 0x01;

        /* set the number of expected responses */
        gOBDNumEcus = (SidReq.Ids[0] == 0) ? nPid0EcuCount : nPid1EcuCount;

        /* if no ECU supports this PID, skip to next PID */
        if (gOBDNumEcus == 0)
            continue;

        if (SidRequest(&SidReq, (SID_REQ_NO_PERIODIC_DISABLE | SID_REQ_RETURN_AFTER_ALL_RESPONSES)) != PASS)
        {
            gOBDNumEcus = NumEcusSave;
            LogPrint ("FAILURE: Sid1 burst support failed\n");
            return (FAIL);
        }
        else
        {
            ulRespTotal++;
        }
    }

    /* restore gOBDNumEcus */
    gOBDNumEcus = NumEcusSave;

    /* Restart periodic message when done */
    if (StartPeriodicMsg () != PASS)
    {
        return (FAIL);
    }

	/* 
	** Link active test to verify communication remained active for ALL protocols
	*/
	if (VerifyLinkActive() != PASS)
	{
		return (FAIL);
	}
		
	/* Log total number of response messages received from burst test. */
	LogPrint("\n\nINFORMATION: Burst test result: %d responses\n\n", ulRespTotal );

	/* Verify response total based on protocol. */
	switch (gOBDList[gOBDListIndex].Protocol)
	{
		case J1850VPW:
		case J1850PWM:
		{
			if ( ulRespTotal < 50 )
			{
				LogPrint("\n\nWARNING: Burst test result below minimum level of 50: %d responses\n\n", ulRespTotal );
			}
		}
		break;
		case ISO9141:
		case ISO14230:
		{
			if ( ulRespTotal < 4 )
			{
				LogPrint("\n\nWARNING: Burst test result below minimum level of 4: %d responses\n\n", ulRespTotal );
			}
		}
		break;
		case ISO15765:
		{
			if ( ulRespTotal < 100 )
			{
				LogPrint("\n\nWARNING: Burst test result below minimum level of 100: %d responses\n\n", ulRespTotal );
			}
		}
		break;
		default:
		{
			return (FAIL);
		}
	}
	
	return (PASS);
}
