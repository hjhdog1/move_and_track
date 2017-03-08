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

/* Funtion prototypes */
STATUS RequestSID2SupportData ( unsigned short *pFreezeFrameDTC, unsigned long FFDTCCount );
STATUS RequestSID2Pid2Data ( unsigned short *pFreezeFrameDTC, unsigned short *pFFDTCSupportCount, unsigned long *pFFDTCCount, unsigned short *FFSupOnPendFault );
STATUS VerifyFFDTCinDTCList ( unsigned short *pFreezeFrameDTC );
STATUS VerifyAllSID2Data ( unsigned short *pFreezeFrameDTC, unsigned short FFDTCSupportCount, unsigned long FFDTCCount );

/*
*******************************************************************************
** VerifyFreezeFrameSupportAndData - Function to verify SID2 freeze frame support and data
*******************************************************************************
*/
STATUS VerifyFreezeFrameSupportAndData(void)
{
static unsigned short FreezeFramePendingDTC[OBD_MAX_ECUS] = {0}; // array of pending Freeze Frame DTCs
unsigned short FreezeFrameDTC[OBD_MAX_ECUS];    // array of the Freeze Frame data from SID $02 PID $02 request
unsigned short FFSupOnPendFault;                // Set if Freeze Frame stored on pending fault
unsigned long  FFDTCCount;                      // the count of ECUs which report a Freeze Frame DTC
unsigned short FFDTCSupportCount;               // the count of ECUs which support SID $02 PID $02

	/* Initialize data */
	FFDTCCount = 0;
	FFDTCSupportCount = 0;
	FFSupOnPendFault = FALSE;
	memset (FreezeFrameDTC, 0, sizeof (FreezeFrameDTC) );

	// Clear Error Count for ERROR_RETURN
	ErrorCount();

	/* Request SID 2 PID 2 data */
	if ( RequestSID2Pid2Data ( FreezeFrameDTC, &FFDTCSupportCount, &FFDTCCount, &FFSupOnPendFault) != PASS )
	{
		return FAIL;
	}

	/* Request SID 2 support data */
	if (RequestSID2SupportData( FreezeFrameDTC, FFDTCCount ) != PASS)
	{
		return FAIL;
	}

	/* Check if a DTC is stored */
	/* 8/8/04; If FF stored on pending fault then verify that it is stored in SID $07 */
	if ( ( gOBDDTCStored    == TRUE ) ||
	     ( FFSupOnPendFault == TRUE ) )		/* If FF stored on pending fault */
	{
		/* Verify the DTC in the freeze frame is in the list of stored DTCs */
		if ( VerifyFFDTCinDTCList ( FreezeFrameDTC ) != PASS )
		{
			return FAIL;
		}

		/* Verify that all SID 2 PID data is valid */
		if ( VerifyAllSID2Data ( FreezeFrameDTC, FFDTCSupportCount, FFDTCCount ) != PASS )
		{
			return FAIL;
		}
	}
	else
	{
		/* Verify that all SID 2 PID data is valid */
		/* Per J1699 rev 11.6; min support must be verified */
		if ( VerifyAllSID2Data ( FreezeFrameDTC, FFDTCSupportCount, FFDTCCount ) != PASS )
		{
			return FAIL;
		}
	}

	/* test 6.5.1 - save FF DTC to compare during next test phase (7.5.1) */
	if (TestPhase == eTestPendingDTC)
	{
		unsigned long  EcuIndex;

		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			FreezeFramePendingDTC[EcuIndex] = FreezeFrameDTC[EcuIndex];
		}
	}

	/* test 7.5.1 - compare FF DTC to previous test phase (6.5.1) */
	if (TestPhase == eTestConfirmedDTC)
	{
		unsigned long  EcuIndex;

		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (FreezeFramePendingDTC[EcuIndex] != 0 &&
			    FreezeFramePendingDTC[EcuIndex] != FreezeFrameDTC[EcuIndex])
			{
				LogPrint("FAILURE: ECU %X  Confirmed Freeze Frame DTCs don't match Pending FF DTCs\n", GetEcuId(EcuIndex) );
				ERROR_RETURN;
				break;
			}
		}
	}

	/* Try group support if ISO15765 */
	if ( gOBDList[gOBDListIndex].Protocol == ISO15765 &&
	     FFDTCCount != 0 )
	{
		STATUS ret_code = VerifyGroupFreezeFrameSupport();
		if (ret_code != PASS)
			return ret_code;

		/* Tests 6.x - 9.x only */
		if (TestPhase == eTestPendingDTC    || TestPhase == eTestConfirmedDTC        ||
		    TestPhase == eTestFaultRepaired || TestPhase == eTestNoFault3DriveCycle)
		{
			ret_code = VerifyGroupFreezeFrameResponse ();
			if (ret_code != PASS)
				return ret_code;
		}
	}

	if ( ErrorCount() != 0 )
	{
		return FAIL;
	}

	return PASS;
}

//******************************************************************************
//
//	Function:	RequestSID2SupportData
//
//	Purpose:	Purpose of this routine is to verify that SID 2 PID 00
//				returns a support record.  Continue requesting support
//				PIDS thru highest supported group.
//
//******************************************************************************
//	DATE		MODIFICATION
//	10/28/03	Created function based on embedded logic originally
//				contained in the routine 'VerifyFreezeFrameSupportAndData'.
//	07/13/04	Updated to request first unsupported PID support PID.
//******************************************************************************
STATUS RequestSID2SupportData( unsigned short *pFreezeFrameDTC, unsigned long FFDTCCount )
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	unsigned long ulPIDSupport;
	SID_REQ SidReq;

	ulPIDSupport = 0;		/* Used to verify that if no PID support, failure may be flagged */

	/* Request SID 2 support data */
	for (IdIndex = 0x00; IdIndex < 0x100; IdIndex += 0x20)
	{
		SidReq.SID    = 2;
		SidReq.NumIds = 2;
		SidReq.Ids[0] = (unsigned char)IdIndex;
		SidReq.Ids[1] = 0; /* Frame #0 */

		if ( SidRequest(&SidReq, SID_REQ_ALLOW_NO_RESPONSE) != PASS )
		{
			/* If there were any Freeze Frame DTCs, there must be a response to PID 0x00 */
			if ( FFDTCCount != 0 &&
			     IdIndex == 0x00 )
			{
				LogPrint("FAILURE: SID $2 PID $00 support request failed\n");
				return FAIL;
			}
		}

		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			/* If the next group is supported, request it */
			if (gOBDResponse[EcuIndex].Sid2PidSupport[IdIndex >> 5].IDBits[3] & 0x01)
			{
				break;
			}
		}

		/* If no ECUs support the next group, stop requests */
		if (EcuIndex >= gOBDNumEcus)
		{
			break;
		}
	}


	/* Request next unsupported PID-support PID	*/
	if ( IdIndex != 0xE0 )
	{
		SidReq.SID    = 2;
		SidReq.NumIds = 2;
		SidReq.Ids[1] = 0; /* Frame #0 */
		SidReq.Ids[0] = (unsigned char)IdIndex += 0x20;

		gIgnoreNoResponse = TRUE;

		if ( SidRequest(&SidReq, SID_REQ_NORMAL) == PASS )
		{
			/* J1850 & ISO9141 - No response preferred, but positive response allowed */
			if ( gOBDList[gOBDListIndex].Protocol == ISO15765 )
			{
				gIgnoreNoResponse = FALSE;
				LogPrint("FAILURE: Unexpected response to SID $2 Unsupported PID-Support PID request!\n");
				ERROR_RETURN;
			}
		}
		gIgnoreNoResponse = FALSE;
	}


	/* Verify ECU did not drop out */
	if (VerifyLinkActive() != PASS)
	{
		ERROR_RETURN;
	}
	return PASS;
}

/*******************************************************************************
**
**	Function:	RequestSID2Pid2Data
**
**	Purpose:	Purpose of this routine is to verify that SID 2 PID 02
**				returns stored DTC.  Assuming SID 2 PID 2 was supported
**				by the controller, data should be returned.
**
********************************************************************************
**	DATE		MODIFICATION
**	10/28/03	Created function based on embedded logic originally
**				contained in the routine 'VerifyFreezeFrameSupportAndData'.
**  08/08/04    Update to account for FF support on pending fault.  J1699
**              version 11.7 specification evaluation criteria outlines logic.
*******************************************************************************/
STATUS RequestSID2Pid2Data( unsigned short *pFreezeFrameDTC, unsigned short *pFFDTCSupportCount, unsigned long *pFFDTCCount, unsigned short *FFSupOnPendFault )
{
	unsigned long EcuIndex;
	SID_REQ SidReq;
	unsigned short NoFFCount = 0;

	/* Request SID 2 PID 2 data */
	SidReq.SID    = 2;
	SidReq.NumIds = 2;
	SidReq.Ids[0] = 2;
	SidReq.Ids[1] = 0; /* Frame #0 */

	gIgnoreUnsupported = TRUE;
	if (SidRequest(&SidReq, SID_REQ_ALLOW_NO_RESPONSE) != PASS)
	{
		gIgnoreUnsupported = FALSE;
		LogPrint("FAILURE: SID $2 PID $2 request failed\n");
		return FAIL;
	}
	gIgnoreUnsupported = FALSE;

	/* Check for freeze frame DTCs */
	for ( EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++ )
	{
		/* Clear Freeze Frame Array entry for this ECU */
		pFreezeFrameDTC[EcuIndex] = 0;

		/* Skip if no response or no DTCs stored */
		if ( gOBDResponse[EcuIndex].Sid2PidSize != 0 )
		{
			/* Increment the count of ECUs which support SID $02 PID $02 */
			*pFFDTCSupportCount = *pFFDTCSupportCount + 1;

			if ( gOBDResponse[EcuIndex].Sid2Pid[1] != 0 ||
			     gOBDResponse[EcuIndex].Sid2Pid[2] != 0 ||
			     gOBDResponse[EcuIndex].Sid2Pid[3] != 0 )
			{
				/* Increment the count of ECUs which report a Freeze Fram DTC */
				*pFFDTCCount = *pFFDTCCount + 1;

				/* Save the DTC */
				pFreezeFrameDTC[EcuIndex] = ((gOBDResponse[EcuIndex].Sid2Pid[2] * 256) +
				                              gOBDResponse[EcuIndex].Sid2Pid[3]);

				/* Check if a DTC is not supposed to be present */
				if ( ( gOBDDTCStored     == FALSE ) &&
				     ( gOBDDTCPending    == FALSE ) &&
				     ( gOBDDTCHistorical == FALSE ) )
				{
					LogPrint( "FAILURE: ECU %X  SID $2 PID $2 Frame %d DTC stored\n",
					          GetEcuId(EcuIndex),
					          gOBDResponse[EcuIndex].Sid2Pid[1]);
					ERROR_RETURN
				}

				else if ( ( gOBDDTCPending == TRUE  ) &&
				          ( gOBDDTCStored  == FALSE ) )
				{
					LogPrint( "INFORMATION: ECU %X  SID $2 PID $2 Frame %d DTC stored on pending fault\n",
					          GetEcuId(EcuIndex),
					          gOBDResponse[EcuIndex].Sid2Pid[1]);
					*FFSupOnPendFault = TRUE;
				}
			}
		}
	}

	/* Test 5.11.1 - If all ECUs report Freeze Frame DTCs stored, it is a Failure */
	if ( TestPhase == eTestNoDTC && TestSubsection == 11 &&
		 *pFFDTCCount == gOBDNumEcus )
	{
		LogPrint("FAILURE: All ECUs reported Freeze Frame DTCs stored\n");
		ERROR_RETURN
	}

	return PASS;
}

//******************************************************************************
//
//	Function:	VerifyFFDTCinDTCList
//
//	Purpose:	Purpose of this routine is to verify the DTC in the freeze
//				frame is in the list of stored DTCs.
//
//******************************************************************************
//	DATE		MODIFICATION
//	10/28/03	Created function based on embedded logic originally
//				contained in the routine 'VerifyFreezeFrameSupportAndData'.
//******************************************************************************
STATUS VerifyFFDTCinDTCList( unsigned short *pFreezeFrameDTC )
{
	unsigned long  EcuIndex;
	unsigned long  AllEcuIndex;
	unsigned long  DTCIndex;
	unsigned long  StrdDTCCount = 0;
	unsigned long  MtchdDTCCount = 0;

	// Count all
	for ( EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++ )
	{
		if ( DTCList[EcuIndex].Size != 0 )
		{
			for ( DTCIndex = 0; DTCIndex < DTCList[EcuIndex].Size; DTCIndex += 2 )
			{
				if ( DTCList[EcuIndex].DTC[DTCIndex] != 0 ||
				     DTCList[EcuIndex].DTC[DTCIndex+1] != 0 )
				{
					StrdDTCCount++;
				}
			}
		}
	}

	/* Verify the DTC in the freeze frame is in the list of stored DTCs */
	for ( EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++ )
	{
		if ( pFreezeFrameDTC[EcuIndex] == 0x00 )
		{
			continue;
		}

		for ( AllEcuIndex = 0; AllEcuIndex < gOBDNumEcus; AllEcuIndex++ )
		{
			for ( DTCIndex = 0; DTCIndex < DTCList[AllEcuIndex].Size; DTCIndex += 2 )
			{
				if ( pFreezeFrameDTC[EcuIndex] == ( ( DTCList[AllEcuIndex].DTC[DTCIndex] * 256) +
				                                    ( DTCList[AllEcuIndex].DTC[DTCIndex + 1] ) ) )
				{
					MtchdDTCCount++;
					break;
				}
			}

			/* Break outer loop if we found a match */
			if ( DTCIndex < DTCList[AllEcuIndex].Size )
			{
				break;
			}
		}

		if ( AllEcuIndex >= gOBDNumEcus )
		{
			LogPrint("FAILURE: ECU %X  Freeze frame DTC not found in list of stored DTCs on any ECU\n", GetEcuId(EcuIndex) );
			return FAIL;
		}
	}

	if ( StrdDTCCount !=0  &&
	     MtchdDTCCount == 0 )
	{
		LogPrint("FAILURE: Stored DTC with no Freeze Frame DTC on any ECU\n");
		return FAIL;
	}

	return PASS;
}

/*******************************************************************************
**
**	Function:	VerifyAllSID2Data
**
**	Purpose:	Purpose of this routine is to Verify that all SID 2 PID
**				data is valid.  Verification of minimum support list is
**				performed here.
**
********************************************************************************
**	DATE		MODIFICATION
**	10/28/03	Created function based on embedded logic originally
**				contained in the routine 'VerifyFreezeFrameSupportAndData'.
**  07/22/04    Enhanced to allow min PID support check to be performed
**              w/no stored DTC.
********************************************************************************
*/
STATUS VerifyAllSID2Data( unsigned short *pFreezeFrameDTC, unsigned short FFDTCSupportCount, unsigned long FFDTCCount )
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	unsigned long ulPIDSupport;

	SID_REQ SidReq;

	unsigned char fPid02Supported = FALSE;      // Set if SID $2 PID $02 is supported
	unsigned char fPid04Supported = FALSE;      // Set if SID $2 PID $04 is supported
	unsigned char fPid05Supported = FALSE;      // Set if SID $2 PID $05 is supported
	unsigned char fPid0CSupported = FALSE;      // Set if SID $2 PID $0C is supported
	unsigned char fPid0DSupported = FALSE;      // Set if SID $2 PID $0D is supported
	unsigned char fPid11Supported = FALSE;      // Set if SID $2 PID $11 is supported
	unsigned char fPid45Supported = FALSE;      // Set if SID $2 PID $45 is supported
	unsigned char fPid49Supported = FALSE;      // Set if SID $2 PID $49 is supported
	unsigned char fPid67Supported = FALSE;      // Set if SID $2 PID $67 is supported

	unsigned long ErrCntTmp;                    // stores the error count when starting this function
	unsigned long ErrFlgTmp;                    // stores the state of the error flags when starting this function

	// Prepare Error Flags and Error Count for ERROR_RETURN
	ErrFlgTmp = ErrorFlags (ER_BYPASS_USER_PROMPT | ER_CONTINUE | ER_DONT_CLEAR_ERROR_COUNT);
	ErrCntTmp = ErrorCount();

	/* Verify that all SID 2 PID data is valid */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		/* If more than one ECU supports SID $2 Frame $00 PID $02, */
		/* clear the PID flags to check for support by each of those ECUs */
		if ( FFDTCSupportCount > 1 )
		{
			fPid02Supported = FALSE;
			fPid04Supported = FALSE;
			fPid05Supported = FALSE;
			fPid0CSupported = FALSE;
			fPid0DSupported = FALSE;
			fPid11Supported = FALSE;
			fPid45Supported = FALSE;
			fPid49Supported = FALSE;
			fPid67Supported = FALSE;
		}


		/* For each PID group */
		for (IdIndex = 0x01; IdIndex < 0x100; IdIndex++)
		{
			/* skip PID Supported PIDs */
			if (IdIndex == 0x00 || IdIndex == 0x20 || IdIndex == 0x40 || IdIndex == 0x60 ||
			    IdIndex == 0x80 || IdIndex == 0xA0 || IdIndex == 0xC0 || IdIndex == 0xE0)
				continue;

			/* If PID is supported, request it */
			if (IsSid2PidSupported (EcuIndex, IdIndex) == TRUE)
			{
				/* Set flags if PID supported */
				if (IdIndex == 0x02)
				{
					fPid02Supported = TRUE;
				}
				else if (IdIndex == 0x04)
				{
					fPid04Supported = TRUE;
				}
				else if (IdIndex == 0x05)
				{
					fPid05Supported = TRUE;
				}
				else if (IdIndex == 0x0C)
				{
					fPid0CSupported = TRUE;
				}
				else if (IdIndex == 0x0D)
				{
					fPid0DSupported = TRUE;
				}
				else if (IdIndex == 0x11)
				{
					fPid11Supported = TRUE;
				}
				else if (IdIndex == 0x45)
				{
					fPid45Supported = TRUE;
				}
				else if (IdIndex == 0x49)
				{
					fPid49Supported = TRUE;
				}
				else if (IdIndex == 0x67)
				{
					fPid67Supported = TRUE;
				}

				gIgnoreNoResponse = TRUE;
				SidReq.SID    = 2;
				SidReq.NumIds = 2;
				SidReq.Ids[0] = (unsigned char)IdIndex;
				SidReq.Ids[1] = 0; /* Frame #0 */
				if ( SidRequest( &SidReq, SID_REQ_NORMAL ) != PASS )
				{
					gIgnoreNoResponse = FALSE;
					/*
					** There must be a response for ISO15765 protocol when a freeze
					** frame DTC is set
					*/
					if ( gOBDList[gOBDListIndex].Protocol == ISO15765 &&
					     pFreezeFrameDTC[EcuIndex] != 0x00 )
					{
						LogPrint( "FAILURE: ECU %X  SID $2 PID $%02X request failed\n",
						          GetEcuId(EcuIndex),
						          IdIndex );
						ERROR_RETURN;
					}
				}

				else
				{
					gIgnoreNoResponse = FALSE;
					/*  If PID $02 is not = $0000, it is a Failure */
					if ( TestPhase == eTestNoDTC && TestSubsection == 11 &&
					     IdIndex == 0x02 &&
					     ( gOBDResponse[EcuIndex].Sid2Pid[2] != 0x00 ||
					       gOBDResponse[EcuIndex].Sid2Pid[3] != 0x00 ) )
					{
						LogPrint( "FAILURE: ECU %X  Response to SID $2 PID $02 request != $0000\n",
						          GetEcuId(EcuIndex),
						          IdIndex );
						ERROR_RETURN;
					}

					/* If ISO15765 and PID $02 == 0, there should be no response to PID request*/
					if ( gOBDList[gOBDListIndex].Protocol == ISO15765 &&
					     gOBDResponse[EcuIndex].Sid2PidSize != 0 &&
					     pFreezeFrameDTC[EcuIndex] == 0x00 &&
					     IdIndex != 0x02 )
					{
						LogPrint( "FAILURE: ECU %X  Response to SID $2 PID $%02X request when PID $02 == 0\n",
						          GetEcuId(EcuIndex),
						          IdIndex );
						ERROR_RETURN;
					}

					/*  If PID is supported and there is a Freeze Frame DTC, there should be a response */
					if ( ( gOBDResponse[EcuIndex].Sid2PidSize == 0) &&
					     ( pFreezeFrameDTC[EcuIndex] != 0x00 ) )
					{
						LogPrint( "FAILURE: ECU %X  No SID $2 PID $%02X data\n",
						          GetEcuId(EcuIndex),
						          IdIndex );
						ERROR_RETURN;
					}
				}

			} // if (IsSid2PidSupported...

		} // end for (IdIndex...


		/* Test If this ECU has a Freeze Frame DTCs, it must support at least one PID */
		if ( pFreezeFrameDTC[EcuIndex] != 0 )
		{
			for (IdIndex = 0; IdIndex < gOBDResponse[EcuIndex].Sid2PidSupportSize; IdIndex++)
			{
				/* If this ECU supports a PID, no need to check anymore PIDs on this ECU */
				if ( ( gOBDResponse[EcuIndex].Sid2PidSupport[IdIndex].IDBits[0]  ||
				       gOBDResponse[EcuIndex].Sid2PidSupport[IdIndex].IDBits[1]  ||
				       gOBDResponse[EcuIndex].Sid2PidSupport[IdIndex].IDBits[2]  ||
				     ( gOBDResponse[EcuIndex].Sid2PidSupport[IdIndex].IDBits[3] & 0xFE ) ) != 0x00)
				{
					ulPIDSupport = 1;
					break;
				}
			}

			/* There must be a supported PID */
			if ( ulPIDSupport == 0 )
			{
				LogPrint("FAILURE: ECU %X  SID $2 no PIDs are supported", GetEcuId(EcuIndex) );
				ERROR_RETURN
			}
		}


		/* If more than one ECU supports SID $2 Frame $00 PID $02 and */
		/* the current ECU has a Freeze Frame DTC, check that the required PIDs are supported */
		if ( FFDTCSupportCount > 1 &&
		     pFreezeFrameDTC[EcuIndex] )
		{
			if ( ( fPid02Supported == FALSE )   ||
			     ( fPid04Supported == FALSE )   ||
			     ( ( fPid05Supported == FALSE ) && ( fPid67Supported == FALSE ) ) ||
			     ( fPid0CSupported == FALSE )   ||
			     ( fPid0DSupported == FALSE )   ||
			     ( ( fPid11Supported == FALSE ) && ( fPid45Supported == FALSE ) && ( fPid49Supported == FALSE ) ) )
			{
				LogPrint("FAILURE: ECU %X  One or more required SID $2 PIDs (02,04,either 05 or 67,0C,0D and either 11, 45 or 49) are not supported\n", GetEcuId(EcuIndex));
				ERROR_RETURN;
			}
		}
	} // end for (EcuIndex...

	/* If only one ECU supports SID $2 Frame $00 PID $02, and has a Freeze Frame DTC */
	/* check that the required PIDs are supported by the vehicle */
	if ( FFDTCSupportCount == 1 &&
	     FFDTCCount == 1 )
	{
		if ( ( fPid02Supported == FALSE )   ||
		     ( fPid04Supported == FALSE )   ||
		     ( ( fPid05Supported == FALSE ) && ( fPid67Supported == FALSE ) ) ||
		     ( fPid0CSupported == FALSE )   ||
		     ( fPid0DSupported == FALSE )   ||
		     ( ( fPid11Supported == FALSE ) && ( fPid45Supported == FALSE ) && ( fPid49Supported == FALSE ) ) )
		{
			LogPrint( "FAILURE: One or more required SID $2 PIDs (02,04,either 05 or 67,0C,0D and either 11, 45 or 49) are not supported on this vehicle\n" );
			ERROR_RETURN;
		}
	}

	// If there where any missing required PIDs, fail
	if ( ErrorCount() != ErrCntTmp )
	{
		ErrorFlags (ErrFlgTmp);  //reset the Error Flags
		ERROR_RETURN;
	}
	ErrorFlags (ErrFlgTmp);  //reset the Error Flags

	return PASS;
}



//*****************************************************************************
//
//	Function:	IsSid2PidSupported
//
//	Purpose:	Determine if SID 2 PID x is supported on specific ECU.
//              Need to have obtained supported PIDs previously.
//              If EcuIndex < 0 then check all ECUs.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	05/24/05	Created common function for this logic.
//
//*****************************************************************************
unsigned int IsSid2PidSupported (unsigned int EcuIndex, unsigned int PidIndex)
{
	int index1;
	int index2;
	int mask;

	if (PidIndex == 0)
		return TRUE;            // all modules must support SID 02 PID 00

	PidIndex--;

	index1 =  PidIndex >> 5;
	index2 = (PidIndex >> 3) & 0x03;
	mask   = 0x80 >> (PidIndex & 0x07);

	if ((signed int)EcuIndex < 0)
	{
		for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid2PidSupport[index1].IDBits[index2] & mask)
				return TRUE;
		}
	}
	else
	{
		if (gOBDResponse[EcuIndex].Sid2PidSupport[index1].IDBits[index2] & mask)
			return TRUE;
	}

	return FALSE;
}
