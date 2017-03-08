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
** VerifyO2TestResults - Function to verify SID5 O2 test results
*******************************************************************************
*/
STATUS VerifyO2TestResults(void)
{
    SID_REQ SidReq;

    /* Bypass the no response warning if ISO15765 */
    if (gOBDList[gOBDListIndex].Protocol == ISO15765) 
    {
        gIgnoreNoResponse = TRUE;
    }
    
    /* Request SID 5 TID 0 */
    SidReq.SID = 5;
    SidReq.NumIds = 2;
    SidReq.Ids[0] = 0;
    SidReq.Ids[1] = 2; /* O2 sensor number #2 */
    if (SidRequest(&SidReq, SID_REQ_ALLOW_NO_RESPONSE) != PASS)
    {
        /* Response is optional */
        gIgnoreNoResponse = FALSE;
        return(FAIL);
    }

    gIgnoreNoResponse = FALSE;

    /* Verify that no response received for CAN protocols */
    if ((gOBDList[gOBDListIndex].Protocol == ISO15765) &&
        (gOBDNumEcusResp != 0))
    {
        LogPrint("FAILURE: Response received for Mode 5 on ISO15765 protocol\n");
        return(FAIL);
    }

    return(PASS);
}
