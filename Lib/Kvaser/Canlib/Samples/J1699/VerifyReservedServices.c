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
** VerifyReservedServices -
** Function to verify that all ECUs respond correctly to reserved/unused services
*******************************************************************************
*/
STATUS VerifyReservedServices(void)
{
	unsigned char SidIndex;
	SID_REQ SidReq;
	unsigned char SavedFlag;
	unsigned long ErrCntTmp;                    // stores the error count when starting this function
	unsigned long ErrFlgTmp;                    // stores the state of the error flags when starting this function

	// Prepare Error Flags and Error Count for ERROR_RETURN
	ErrFlgTmp = ErrorFlags (ER_BYPASS_USER_PROMPT | ER_CONTINUE | ER_DONT_CLEAR_ERROR_COUNT);
	ErrCntTmp = ErrorCount();

	if ( gOBDList[gOBDListIndex].Protocol != ISO15765 )
	{
		ErrorFlags (ErrFlgTmp);  //reset the Error Flags
		return (PASS);
	}

	SavedFlag = gIgnoreNoResponse;
	gIgnoreNoResponse = TRUE;

	/* For each SID */
	for ( SidIndex = 0x0B; SidIndex <= 0x0F; SidIndex++ )
	{
		SidReq.SID = SidIndex;
		SidReq.NumIds = 0;
		if ( SidRequest( &SidReq, SID_REQ_NORMAL ) == PASS )
		{
			LogPrint( "FAILURE: Sid $%02X response\n", SidIndex );
			ERROR_RETURN;
			continue;
		}
	}

	gIgnoreNoResponse = SavedFlag;

	// If there where any errors in the data, fail
	if ( ErrorCount() != ErrCntTmp )
	{
		ErrorFlags (ErrFlgTmp);  //reset the Error Flags
		ERROR_RETURN;
        VerifyLinkActive();
        return (FAIL);
	}
	ErrorFlags (ErrFlgTmp);  //reset the Error Flags

	if ( VerifyLinkActive() != PASS )
	{
        return (FAIL);
	}

	return (PASS);
}
