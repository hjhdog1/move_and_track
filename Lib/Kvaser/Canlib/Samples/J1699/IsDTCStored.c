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
** IsDTCStored - Function to check if SID3 reports a DTC stored
*******************************************************************************
*/
int IsDTCStored(void)
{
    SID_REQ       SidReq;
    STATUS        RetCode;
    unsigned long EcuIndex;
    unsigned char SavedFlag;

    /* Bypass the no response warning */
    SavedFlag = gIgnoreNoResponse;
    gIgnoreNoResponse = TRUE;

    /* Request SID 3 data */
    SidReq.SID = 3;
    SidReq.NumIds = 0;

    RetCode = SidRequest(&SidReq, SID_REQ_NORMAL);

    /* Restore flag */
    gIgnoreNoResponse = SavedFlag;

    if (RetCode != PASS)
    {
        /* If no response, assume no DTC pending */
        return(FALSE);
    }

    /* Check if SID 3 reports DTCs stored */
    for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
    {
        if (gOBDResponse[EcuIndex].Sid3Size == 0)
        {
            /* If no data, ignore */
        }
        /* Check if there is at least one DTC */
        else if ((gOBDResponse[EcuIndex].Sid3[0] != 0) || (gOBDResponse[EcuIndex].Sid3[1] != 0))
        {
            /* Break the loop if a DTC is stored */
            break;
        }
    }

    /* If no stored DTCs, return FALSE */
    if (EcuIndex == gOBDNumEcus)
    {
        return(FALSE);
    }

    return(TRUE);
}
