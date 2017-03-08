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

/*  Funtion prototypes  */
STATUS SetupRequestMSG    (SID_REQ *, PASSTHRU_MSG *);
STATUS ProcessLegacyMsg   (SID_REQ *, PASSTHRU_MSG *, unsigned long *, unsigned long *, unsigned long *, unsigned long *, unsigned long	*, unsigned long);
STATUS ProcessISO15765Msg (SID_REQ *, PASSTHRU_MSG *, unsigned long *, unsigned long *, unsigned long *, unsigned long *, unsigned long *);
void   LogTS (PASSTHRU_MSG *, unsigned long *);

/* Wait / Pending data */
static unsigned long ulEcuWaitFlags = 0;            /* up to 32 ECUs */
static unsigned long ulResponsePendingDelay = 0;

/*
*******************************************************************************
** SidRequest - Function to request a service ID
*******************************************************************************
*/
STATUS SidRequest(SID_REQ *SidReq, unsigned long Flags)
{
	PASSTHRU_MSG  RxMsg;
	PASSTHRU_MSG  TxMsg;
	unsigned long NumMsgs;
	unsigned long RetVal;
	unsigned long StartTimeMsecs;
	unsigned long NumResponses;
	unsigned long NumFirstFrames;
	unsigned long TxTimestamp;
	unsigned long ExtendResponseTimeMsecs;
	unsigned long SOMTimestamp;
	unsigned long ErrCntTmp;        // stores the error count when starting this function
	unsigned long ErrFlgTmp;        // stores the state of the error flags when starting this function

	/* Initialize local variables */
	ExtendResponseTimeMsecs = 0;
	TxTimestamp             = 0;
	SOMTimestamp            = 0;

	/* Reset wait variables */
	ulEcuWaitFlags          = 0;
	ulResponsePendingDelay  = 0;

	// Prepare Error Flags and Error Count for ERROR_RETURN
	ErrFlgTmp = ErrorFlags (ER_BYPASS_USER_PROMPT | ER_CONTINUE | ER_DONT_CLEAR_ERROR_COUNT);
	ErrCntTmp = ErrorCount();

	/* if gSuspendLogOutput is true, then clear buffer */
	ClearLogMsgCopy ();

	/* If not burst test, stop tester present message and delay
	** before each request to avoid exceeding minimum OBD request timing */
	if ( ( Flags & SID_REQ_NO_PERIODIC_DISABLE ) == 0)
	{
		/* Stop the tester present message before each request */
		StopPeriodicMsg (FALSE);

		gOBDList[gOBDListIndex].TesterPresentID = -1;

		Sleep (gOBDRequestDelay);
	}

	/* Setup request message based on the protocol */
	if ( SetupRequestMSG( SidReq, &TxMsg ) != PASS )
	{
		ERROR_RETURN;
	}

	/* Clear the transmit queue before sending request */
	RetVal = PassThruIoctl (gOBDList[gOBDListIndex].ChannelID, CLEAR_TX_BUFFER, NULL, NULL);

	if (RetVal != STATUS_NOERROR)
	{
		Log2534Error("", RetVal, "PassThruIoctl(CLEAR_TX_BUFFER)");
		ERROR_RETURN;
	}

	/* Clear the receive queue before sending request */
	RetVal = PassThruIoctl (gOBDList[gOBDListIndex].ChannelID, CLEAR_RX_BUFFER, NULL, NULL);

	if (RetVal != STATUS_NOERROR)
	{
		Log2534Error("", RetVal, "PassThruIoctl(CLEAR_RX_BUFFER)");
		ERROR_RETURN;
	}

	/* Send the request */
	NumMsgs = 1;
	RetVal  = PassThruWriteMsgs (gOBDList[gOBDListIndex].ChannelID, &TxMsg, &NumMsgs, 500);

	if (RetVal != STATUS_NOERROR)
	{
		/*  don't log timeouts during DetermineProtocol */
		if (!(gDetermineProtocol == 1 && RetVal == ERR_TIMEOUT))
		{
			Log2534Error("", RetVal, "PassThruWriteMsgs");
		}
		ERROR_RETURN;
	}

	/* Log the request message to compare to what is sent */
	LogMsg( &TxMsg, LOG_REQ_MSG );

	/* Reset the response data buffers */
	if ( SidResetResponseData( &TxMsg ) != PASS )
	{
		LogPrint("FAILURE: Cannot reset SID response data\n");
		ERROR_RETURN;
	}

	/*
	** Allow sufficient time for first response (echo of transmitted message),
	** especially for ISO9141/ISO14230 which may take a while if bus is busy
	*/
	NumMsgs = 1;
	RetVal  = PassThruReadMsgs( gOBDList[gOBDListIndex].ChannelID, &RxMsg, &NumMsgs,
								(5 * gOBDMaxResponseTimeMsecs) );  /*extend response time: the multiplier is changed to 5 from 3*/
	if ( (RetVal != STATUS_NOERROR) &&
	     (RetVal != ERR_BUFFER_EMPTY) &
	     (RetVal != ERR_NO_FLOW_CONTROL) )
	{
		/* Log undesirable returns */
		Log2534Error("In SidRequest -", RetVal, "Initial PassThruReadMsgs");
		ERROR_RETURN;
	}


	// If there where any errors up through sending the request, there was a failure
	if ( ErrorCount() != ErrCntTmp )
	{
		ErrorFlags (ErrFlgTmp); //reset the Error Flags
		return(FAIL);
	}

	/*
	** Read the response(s) with a timeout of twice what is allowed so
	** we can see late responses.
	*/
	NumResponses	= 0;
	NumFirstFrames  = 0;
	StartTimeMsecs	= GetTickCount();

	while ( (   NumMsgs == 1 )					||
			(   GetTickCount() - StartTimeMsecs) <
			( ( 5 * gOBDMaxResponseTimeMsecs ) + ExtendResponseTimeMsecs ) ) /*extend response time: the multiplier is changed to 5 from 3*/
	{
		/* If a message was received, process it */
		if ( NumMsgs == 1 )
		{
			/* Save all read messages in the log file */
			LogMsg(&RxMsg, LOG_NORMAL_MSG);

			/* Process response based on protocol */
			switch (gOBDList[gOBDListIndex].Protocol)
			{
				case J1850VPW:
				case J1850PWM:
				case ISO9141:
				case ISO14230:
				{
					ProcessLegacyMsg ( SidReq,
									   &RxMsg,
									   &StartTimeMsecs,
									   &NumResponses,
									   &TxTimestamp,
									   &ExtendResponseTimeMsecs,
									   &SOMTimestamp,
									   Flags );
				}
				break;
				case ISO15765:
				{
					ProcessISO15765Msg ( SidReq,
										 &RxMsg,
										 &StartTimeMsecs,
										 &NumResponses,
										 &NumFirstFrames,
										 &TxTimestamp,
										 &ExtendResponseTimeMsecs );
				}
				break;
				default:
				{
					ERROR_RETURN;
				}
			}
		}

		/* If all expected ECUs responded and flag is set, don't wait for timeout */
		/* NOTE: This mechanism is only good for single message response per ECU */
		if ( ( NumResponses >= gOBDNumEcus )	&&
			 ( Flags & SID_REQ_RETURN_AFTER_ALL_RESPONSES ) )
		{
			break;
		}

		/* Read the next response */
		NumMsgs = 1;
		RetVal	= PassThruReadMsgs( gOBDList[gOBDListIndex].ChannelID,
									&RxMsg,
									&NumMsgs,
								( ( 5 * gOBDMaxResponseTimeMsecs ) + ExtendResponseTimeMsecs ) );  /*extend response time: the multiplier is changed to 5 from 3*/

		if ( (RetVal != STATUS_NOERROR) &&
			 (RetVal != ERR_BUFFER_EMPTY) &
			 (RetVal != ERR_NO_FLOW_CONTROL) )
		{
			/* Log undesirable returns */
			Log2534Error("In SidRequest -", RetVal, "Loop PassThruReadMsgs");
		}
	}

	/* Restart the periodic message if protocol determined and not in burst test */
	if ( ( gOBDDetermined == TRUE )	&&
		 ( (Flags & SID_REQ_NO_PERIODIC_DISABLE ) == 0 ) )
	{
		if ( gOBDList[gOBDListIndex].TesterPresentID == -1 )
		{
			if (StartPeriodicMsg () != PASS)
			{
				ERROR_RETURN;
			}
		}
	}

	/* 06/17/04 - Update to preserve the total number of ECUs responding and allow
	**            calling routine to access information.
	*/
	gOBDNumEcusResp = NumResponses;

	/* Return code based on whether this protocol supports OBD */
	if (NumResponses > 0)
	{
		if (gOBDDetermined == FALSE)
		{
			gOBDNumEcus = NumResponses;
			LogPrint("INFORMATION: %d OBD ECU(s) found\n", gOBDNumEcus);

			/* Check if number of OBD-II ECUs entered by user matches the number detected */
			if (gOBDNumEcus != gUserNumEcus)
			{
				LogPrint("FAILURE: Number of OBD-II ECUs detected does not match the number entered.\n");
				ERROR_RETURN;
			}

		}
		// If there wheren't any errors since sending the request, pass
		if ( ErrorCount() == ErrCntTmp )
		{
			ErrorFlags (ErrFlgTmp); //reset the Error Flags
			return(PASS);
		}
	}

	// If there where any errors since sending the request, there was a failure
	if ( ErrorCount() != ErrCntTmp )
	{
		ErrorFlags (ErrFlgTmp); //reset the Error Flags
		return(FAIL);
	}
	ErrorFlags (ErrFlgTmp); //reset the Error Flags

	if (Flags & SID_REQ_ALLOW_NO_RESPONSE)
	{
		/* calling function must determine any actual responses */
		return PASS;
	}

	if ( (gOBDDetermined == TRUE ) && ( gIgnoreNoResponse == FALSE ) )
	{
		LogPrint("WARNING: No response to OBD request\n");
	}
	return (FAIL);
}

//*****************************************************************************
//
//	Function:	ProcessLegacyMsg
//
//	Purpose:	Purpose is to process response based on connection to
//              J1850VPW, J1850PWM, ISO9141, or ISO14230 .
//
//  NOTE:       Considered consolidating ProcessISO15765Msg with
//              ProcessLegacyMsg.  The many unique difference would
//              complicate the 'streamling' of the logic.  Hence the
//              routines have intentionally remained separate.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	11/01/03	Isolated routine into a function.
//  07/16/04    Enhanced to turn on tester present in the event a timing error
//              was flagged.
//
//*****************************************************************************
STATUS ProcessLegacyMsg( SID_REQ		*pSidReq,
						 PASSTHRU_MSG	*pRxMsg,
						 unsigned long  *ulStartTimeMsecs,
						 unsigned long  *ulNumResponses,
						 unsigned long	*ulTxTimestamp,
						 unsigned long	*ulExtendResponseTimeMsecs,
						 unsigned long	*ulSOMTimestamp,
						 unsigned long  Flags )
{
	unsigned long ulResponseDelta = 0;

	/* Check for echoed request message */
	if (pRxMsg->RxStatus & TX_MSG_TYPE)
	{
		if ( (pRxMsg->DataSize >= 4) && (pRxMsg->Data[3] == pSidReq->SID) )
		{
			/* Save the timestamp */
			*ulTxTimestamp = pRxMsg->Timestamp;
		}
	}
	/* Get Start of Message timestamp to compute response time (ISO9141 & ISO14230 only) */
	else if (pRxMsg->RxStatus & START_OF_MESSAGE)
	{
		*ulSOMTimestamp = pRxMsg->Timestamp;
	}
	/* Check for NAK response message */
	else if ( (pRxMsg->DataSize >= 6)  &&
			  (pRxMsg->Data[3] == NAK) &&
			  (pRxMsg->Data[4] == pSidReq->SID) )
	{
		if (gOBDList[gOBDListIndex].Protocol != ISO9141 && gOBDList[gOBDListIndex].Protocol != ISO14230)
		{
			*ulSOMTimestamp = pRxMsg->Timestamp;    /* for J1850xxx */
		}

		ulResponseDelta = *ulSOMTimestamp - *ulTxTimestamp;

		/* same timestamp logic used to get EOM, SOM and DELTA */

		/* Tally up the response statistics */
		gOBDAggregateResponseTimeMsecs += ulResponseDelta / 1000;
		gOBDAggregateResponses++;

		/* Display for non-class 2 */
		if (gOBDList[gOBDListIndex].Protocol == ISO9141 ||
		    gOBDList[gOBDListIndex].Protocol == ISO14230)
		{
			/* 6/7/04 - Print time stamps to log file for visual time verification. */
			LogPrint("EOM: %lu usec, ",  *ulTxTimestamp );
			LogPrint("SOM: %lu usec, ",	 *ulSOMTimestamp);
			LogPrint("DELTA: %lu usec\n", ulResponseDelta);
		}

		/* Check if response was too soon */
		if ((ulResponseDelta / 1000) < gOBDMinResponseTimeMsecs)
		{
			LogPrint("FAILURE: OBD Response was sooner than allowed (< %dmsec)\n", gOBDMinResponseTimeMsecs);
			gRespTimeOutofRange++;
			gRespTimeTooSoon++;

			/* Restart the periodic message if protocol determined and not in burst test */
			if (gOBDList[gOBDListIndex].TesterPresentID == -1)
			{
				LogPrint("INFORMATION: StartPeriodicMsg\n");
				StartPeriodicMsg ();
			}

			ERROR_RETURN;
		}

		/* Check if response was late */
		if ((ulResponseDelta / 1000) > (gOBDMaxResponseTimeMsecs + 1 + *ulExtendResponseTimeMsecs))
		{
			LogPrint("FAILURE: OBD Response was later than allowed (> %dmsec)\n",
					 (gOBDMaxResponseTimeMsecs + *ulExtendResponseTimeMsecs));
			gRespTimeOutofRange++;
			gRespTimeTooLate++;

			/* Restart the periodic message if protocol determined and not in burst test */
			if (gOBDList[gOBDListIndex].TesterPresentID == -1)
			{
				StartPeriodicMsg ();
			}

			ERROR_RETURN;
		}

		/* If response was not late, reset the start time */
		*ulStartTimeMsecs = GetTickCount();
		*ulTxTimestamp = pRxMsg->Timestamp;

		if (pRxMsg->Data[5] != NAK_RESPONSE_PENDING)
		{
			(*ulNumResponses)++;
		}

		/* Save the response information */
		if (SidSaveResponseData (pRxMsg, pSidReq) != PASS)
		{
			LogPrint("FAILURE: Cannot save SID response data\n");
			ERROR_RETURN;
		}

		/*check the kind of response received for the vehicle*/
		if (pRxMsg->Data[5] == NAK_NOT_SUPPORTED /*0x11*/)
		{
			LogPrint("\nINFORMATION: Service $%02X not supported.\n", pRxMsg->Data[4]);
		}
		else if (pRxMsg->Data[5] == NAK_INVALID_FORMAT /*0x12*/)
		{
			LogPrint("\nINFORMATION: Service $%02X supported. Unsupported PID request.\n", pRxMsg->Data[4]);
		}
		else if (pRxMsg->Data[5] == NAK_SEQUENCE_ERROR /*0x22*/)
		{
			LogPrint("\nINFORMATION: Service $%02X supported. Conditions not correct.\n", pRxMsg->Data[4]);
		}

		/* If response pending, extend the wait time to worst case P3 max */
		if (pRxMsg->Data[5] == NAK_RESPONSE_PENDING)
		{
			/* Don't allow extended response time for infotype 6 */
			/* No group requests for legacy protocols */
			if (pSidReq->SID != 9 && pSidReq->Ids[0] != 6)
			{
				LogPrint("\nINFORMATION: Service $%02X supported. Response Pending.\n", pRxMsg->Data[4]);
				*ulExtendResponseTimeMsecs = 3000;
			}
		}
	}
	/* Check for response message */
	else if ( (pRxMsg->DataSize >= 4) && (pRxMsg->Data[3]  == (pSidReq->SID + OBD_RESPONSE_BIT)) )
	{
		if (gOBDList[gOBDListIndex].Protocol != ISO9141 && gOBDList[gOBDListIndex].Protocol != ISO14230)
		{
			*ulSOMTimestamp = pRxMsg->Timestamp;    /* for J1850xxx */
		}

		ulResponseDelta = *ulSOMTimestamp - *ulTxTimestamp;

		/* Tally up the response statistics */
		gOBDAggregateResponseTimeMsecs += ulResponseDelta / 1000;
		gOBDAggregateResponses++;

		/* Display for non-class 2 */
		if (gOBDList[gOBDListIndex].Protocol == ISO9141 ||
		    gOBDList[gOBDListIndex].Protocol == ISO14230)
		{
			/* 6/7/04 - Print time stamps to log file for visual time verification. */
			LogPrint ("EOM: %lu usec, ",  *ulTxTimestamp);
			LogPrint ("SOM: %lu usec, ",  *ulSOMTimestamp);
			LogPrint ("DELTA: %lu usec\n", ulResponseDelta);
		}

		/* Check if response was too soon */
		/* Since under ISO9141 or ISO14230 we have saved off the ending timestamp
		** as ulTxTimestamp, we can now accurately calculate the difference between the
		** end of the last message to the beginning of this response.
		*/
		if ( (ulResponseDelta / 1000) < gOBDMinResponseTimeMsecs)
		{
			/* Exceeded maximum response time */
			LogPrint("FAILURE: OBD Response was sooner than allowed (< %dmsec)\n", (gOBDMinResponseTimeMsecs));
			   gRespTimeOutofRange++;
			   gRespTimeTooSoon++;

			/* Restart the periodic message if protocol determined and not in burst test */
			if (gOBDList[gOBDListIndex].TesterPresentID == -1)
			{
				StartPeriodicMsg ();
			}

			ERROR_RETURN;
		}

		/* Check if response was late */
		/* Since under ISO9141 or ISO14230 we have saved off the ending timestamp
		** as ulTxTimestamp, we can now accurately calculate the difference between the
		** end of the last message to the beginning of this response.
		*/
		if ( ( ulResponseDelta / 1000) >
			 ( gOBDMaxResponseTimeMsecs + 1	+ *ulExtendResponseTimeMsecs))
		{
			/* Exceeded maximum response time */
			LogPrint("FAILURE: OBD Response was later than allowed (> %dmsec)\n",
					 (gOBDMaxResponseTimeMsecs + *ulExtendResponseTimeMsecs));
			gRespTimeOutofRange++;
			gRespTimeTooLate++;

			/* Restart the periodic message if protocol determined and not in burst test */
			if ( (gOBDList[gOBDListIndex].TesterPresentID == -1) && (gDetermineProtocol != 1))
			{
				StartPeriodicMsg ();
			}

			ERROR_RETURN;
		}

		/* If response was not late, reset the start time */
		*ulStartTimeMsecs = GetTickCount();
		*ulTxTimestamp = pRxMsg->Timestamp;

		(*ulNumResponses)++;

		/* Save the response information */
		if (SidSaveResponseData (pRxMsg, pSidReq) != PASS)
		{
			LogPrint ("FAILURE: Cannot save SID response data\n");
			ERROR_RETURN;
		}
	}

	return PASS;
}

/******************************************************************************
**
**	Function:	ProcessISO15765Msg
**
**	Purpose:	Purpose is to process response based on connection to
**              ISO15765.
**
**  NOTE:       Considered consolidating ProcessISO15765Msg with
**              ProcessLegacyMsg.  The many unique difference would
**              complicate the 'streamling' of the logic.  Hence the
**              routines have intentionally remained separate.
**
*******************************************************************************
**
**	DATE		MODIFICATION
**	11/01/03	Isolated routine into a function.
**  02/23/04	Review of time evaluation logic for ISO15765 produced questions
**              related to the evaluation logic used for CAN.  Corrected logic
**              to perform a more absolute comparison of response time.
**  05/12/04    Enhanced routine to return a fail on any Mode $05 response.
*******************************************************************************
*/
STATUS ProcessISO15765Msg(	SID_REQ			*pSidReq,
							PASSTHRU_MSG	*pRxMsg,
							unsigned long   *ulStartTimeMsecs,
							unsigned long   *ulNumResponses,
							unsigned long   *ulNumFirstFrames,
							unsigned long	*ulTxTimestamp,
							unsigned long	*ulExtendResponseTimeMsecs )
{
	static unsigned char bPadErrorFound = FALSE;

	unsigned long EcuIndex;

	/* Check for padding error only once */
	if (bPadErrorFound == FALSE)
	{
		if (pRxMsg->RxStatus & ISO15765_PADDING_ERROR)
		{
			LogPrint ("FAILURE: ISO15765 message padding error - ECU ID: %02X%02X%02X%02X\n",
					  pRxMsg->Data[0], pRxMsg->Data[1], pRxMsg->Data[2], pRxMsg->Data[3]);
			bPadErrorFound = TRUE;
//*****************************************************************************
// Temporary work around to allow vehicles with padding errors to continue to test
//			ERROR_RETURN;
//*****************************************************************************
		}
	}

	/* Check for FirstFrame indication */
	if ( pRxMsg->RxStatus & ISO15765_FIRST_FRAME )
	{
		/* Extend the response time to the worst case for segmented responses */
		*ulExtendResponseTimeMsecs = 30000;
		(*ulNumFirstFrames)++;
		ScreenPrint ("Receiving segmented responses, please wait...\n");
	}

	/* Check for echoed request message */
	else if ( pRxMsg->RxStatus & TX_MSG_TYPE )
	{
		if ( ( pRxMsg->DataSize >= 5 ) &&
			 ( pRxMsg->Data[4]  == pSidReq->SID ) )
		{
			/* Save the timestamp */
			*ulTxTimestamp = pRxMsg->Timestamp;
		}
	}
	/* Check for NAK response message */
	else if ( ( pRxMsg->DataSize >= 7   ) &&
			  ( pRxMsg->Data[4]  == NAK ) &&
			  ( pRxMsg->Data[5]  == pSidReq->SID ) )
	{
		if ( pRxMsg->Data[6] != NAK_RESPONSE_PENDING )
		{
			(*ulNumResponses)++;
		}

		/*check the kind of response received for the vehicle*/
		if (pRxMsg->Data[6]  == NAK_NOT_SUPPORTED /*0x11*/)
		{
			LogPrint("\nINFORMATION: Service $%02X not supported.\n", pRxMsg->Data[5]);
		}
		else if (pRxMsg->Data[6]  == NAK_INVALID_FORMAT /*0x12*/)
		{
			LogPrint("\nINFORMATION: Service $%02X supported. Unsupported PID request.\n", pRxMsg->Data[5]);
		}
		else if (pRxMsg->Data[6]  == NAK_SEQUENCE_ERROR /*0x22*/)
		{
			LogPrint("\nINFORMATION: Service $%02X supported. Conditions not correct.\n", pRxMsg->Data[5]);
		}

		/*
		** NAK Truth table per J1699 Rev 11.6 Table B
		*/

		switch( pSidReq->SID )
		{
			case 0x01:
				/* If response pending, extend the wait time */
				LogPrint( "FAILURE: SID $01 NAK response error!\n" );
				ERROR_RETURN;
				break;

			case 0x02:
			case 0x03:
			case 0x06:
			case 0x07:
				/* If response pending, extend the wait time */
				/* if ( pRxMsg->Data[6] == NAK_RESPONSE_PENDING )
				**{
				**	*ulExtendResponseTimeMsecs = 5000;
				**}
				*/
				LogPrint( "FAILURE: SID $%02X NAK response error!\n", pSidReq->SID );
				ERROR_RETURN;
				break;

			case 0x04:
				if ( pRxMsg->Data[6] == NAK_RESPONSE_PENDING )
				{
					/* If response pending, extend the wait time */
					*ulExtendResponseTimeMsecs = ulResponsePendingDelay = 30000;

					/* set wait flag */
					EcuIndex = LookupEcuIndex (pRxMsg);
					if (EcuIndex < OBD_MAX_ECUS)
						ulEcuWaitFlags |= (1<<EcuIndex);
				}
				else if ( pRxMsg->Data[6] != NAK_SEQUENCE_ERROR )
				{
					LogPrint( "FAILURE: SID $04 NAK response error!\n" );
					ERROR_RETURN;
				}
				break;

			case 0x08:
				if ( pRxMsg->Data[6] != NAK_SEQUENCE_ERROR )
				{
					LogPrint( "FAILURE: SID $08 NAK response error!\n" );
					ERROR_RETURN;
				}
				break;

			case 0x09:
				if ( pRxMsg->Data[6] == NAK_RESPONSE_PENDING )
				{
					/* Don't allow extended response time for infotype 6
					** NOTE: this does not work for group requests,
					**       but there are no group requests for Sid $09
					*/
					if (pSidReq->Ids[0] != 0x06)
					{
						/* If response pending, extend the wait time */
						*ulExtendResponseTimeMsecs = ulResponsePendingDelay = 5000;

						/* set wait flag */
						EcuIndex = LookupEcuIndex (pRxMsg);
						if (EcuIndex < OBD_MAX_ECUS)
							ulEcuWaitFlags |= (1<<EcuIndex);
					}
				}
				else if ( pRxMsg->Data[6] != NAK_SEQUENCE_ERROR )
				{
					LogPrint( "FAILURE: SID $09 NAK response error!\n" );
					ERROR_RETURN;
				}
				break;

			default :
				LogPrint( "FAILURE: SID $%02X NAK response error!\n", pSidReq->SID );
				ERROR_RETURN;
				break;
		}
	}
	/* Check for response message */
	else if ( ( pRxMsg->DataSize >= 5 ) &&
			  ( pRxMsg->Data[4]  == ( pSidReq->SID + OBD_RESPONSE_BIT ) ) )
	{
		/* Tally up the response statistics */
		gOBDAggregateResponseTimeMsecs += (pRxMsg->Timestamp - *ulTxTimestamp) / 1000;
		gOBDAggregateResponses +=
			( ( ( pRxMsg->DataSize - 4 ) / ISO15765_MAX_BYTES_PER_FRAME ) + 1 );

		/* Check if response was late (compensate for segmented responses) */
		if ( ( ( pRxMsg->Timestamp - *ulTxTimestamp )   /  1000 ) >
			   ( gOBDMaxResponseTimeMsecs + 1 + *ulExtendResponseTimeMsecs ) )
		{
			/* Exceeded maximum response time */
			LogPrint("FAILURE: OBD Response was later than allowed (> %dmsec)\n",
					(gOBDMaxResponseTimeMsecs + *ulExtendResponseTimeMsecs));
			gRespTimeOutofRange++;
			gRespTimeTooLate++;
			LogPrint("INFORMATION: Calculated OBD Response time: %d msec\n",
				( ( pRxMsg->Timestamp - *ulTxTimestamp )   /  1000 ));

			ERROR_RETURN;
		}

		(*ulNumResponses)++;

		/* clear wait flag */
		EcuIndex = LookupEcuIndex (pRxMsg);
		if (EcuIndex < OBD_MAX_ECUS)
			ulEcuWaitFlags &= ~(1<<EcuIndex);

		/* Check if all ECUs with response pending have responded */
		if (ulEcuWaitFlags == 0)
		{
			ulResponsePendingDelay = 0;         /* no ECU response is pending */

			if (*ulNumFirstFrames == 0)
			{
				*ulExtendResponseTimeMsecs = 0; /* no First Frames, clear extended response time */
			}
		}

		/* Check if we can turn off the time extension for segmented frames */
		if ( pRxMsg->DataSize > ( 4 + ISO15765_MAX_BYTES_PER_FRAME ) )
		{
			if ( *ulNumFirstFrames > 0 )
			{
				if ( --(*ulNumFirstFrames) == 0 )
				{
					/*
					** If we have received as many segmented frames as
					** FirstFrame indications, restore 'response pending' time extension
					*/
					*ulExtendResponseTimeMsecs = ulResponsePendingDelay;
				}
			}
		}

		/* Save the response information */
		if (SidSaveResponseData(pRxMsg, pSidReq) != PASS)
		{
			LogPrint("FAILURE: Cannot save SID response data\n");
			ERROR_RETURN;
		}
	}

	return (PASS);
}

//*****************************************************************************
//
//	Function:	SetupRequestMSG
//
//	Purpose:	Purpose is to setup request message based on the protocol.
//              Routine will normalize the message data handling between
//				the regulated OBD protocol message structures.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	11/01/03	Isolated routine into a function.
//
//*****************************************************************************
STATUS SetupRequestMSG( SID_REQ *pSidReq, PASSTHRU_MSG *pTxMsg )
{

	/* Set message data information common to all protocols */
	pTxMsg->ProtocolID	= gOBDList[gOBDListIndex].Protocol;
	pTxMsg->RxStatus	= TX_MSG_TYPE;
	pTxMsg->TxFlags		= 0x00;	/*Change back to non-blocking*/

	/* Configure protocol specific message data. */
	switch(gOBDList[gOBDListIndex].Protocol)
	{
		case J1850VPW:
		case ISO9141:
		{
			pTxMsg->Data[0]  = 0x68;
			pTxMsg->Data[1]  = 0x6A;
			pTxMsg->Data[2]  = TESTER_NODE_ADDRESS;
			pTxMsg->Data[3]  = pSidReq->SID;

			memcpy( &pTxMsg->Data[4], &pSidReq->Ids[0], pSidReq->NumIds );
			pTxMsg->DataSize = 4 + pSidReq->NumIds;
		}
		break;
		case J1850PWM:
		{
			pTxMsg->Data[0]  = 0x61;
			pTxMsg->Data[1]  = 0x6A;
			pTxMsg->Data[2]  = TESTER_NODE_ADDRESS;
			pTxMsg->Data[3]  = pSidReq->SID;

			memcpy( &pTxMsg->Data[4], &pSidReq->Ids[0], pSidReq->NumIds );
			pTxMsg->DataSize = 4 + pSidReq->NumIds;
		}
		break;
		case ISO14230:
		{
			pTxMsg->Data[0]  = 0xC0 + pSidReq->NumIds + 1;
			pTxMsg->Data[1]  = 0x33;
			pTxMsg->Data[2]  = TESTER_NODE_ADDRESS;
			pTxMsg->Data[3]  = pSidReq->SID;

			memcpy( &pTxMsg->Data[4], &pSidReq->Ids[0], pSidReq->NumIds );
			pTxMsg->DataSize = 4 + pSidReq->NumIds;
		}
		break;
		case ISO15765:
		{
			pTxMsg->TxFlags |= ISO15765_FRAME_PAD;

			if (gOBDList[gOBDListIndex].InitFlags & CAN_29BIT_ID)
			{
				pTxMsg->TxFlags |= CAN_29BIT_ID;

				pTxMsg->Data[0]  = 0x18;
				pTxMsg->Data[1]  = 0xDB;
				pTxMsg->Data[2]  = 0x33;
				pTxMsg->Data[3]  = TESTER_NODE_ADDRESS;
				pTxMsg->Data[4]  = pSidReq->SID;

				memcpy( &pTxMsg->Data[5], &pSidReq->Ids[0], pSidReq->NumIds );
				pTxMsg->DataSize = 5 + pSidReq->NumIds;
			}
			else
			{
				pTxMsg->Data[0]  = 0x00;
				pTxMsg->Data[1]  = 0x00;
				pTxMsg->Data[2]  = 0x07;
				pTxMsg->Data[3]  = 0xDF;
				pTxMsg->Data[4]  = pSidReq->SID;

				memcpy( &pTxMsg->Data[5], &pSidReq->Ids[0], pSidReq->NumIds );
				pTxMsg->DataSize = 5 + pSidReq->NumIds;
			}
		}
		break;
		default:
		{
			return FAIL;
		}
	}

	return PASS;
}

//*****************************************************************************
//
//	Function:	LogTS
//
//	Purpose:	Common function to log timestamp for ISO9141 & ISO14230.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	05/17/04	Created common function for this logic.
//
//*****************************************************************************
void LogTS (PASSTHRU_MSG *pRxMsg, unsigned long *ulTxTimestamp)
{
	/*
	** For ISO9141 / ISO14230 calculate and note the time between
	** the previous message and the time stamp from current message.
	** Tx / Rx message, timestamp is accurate.
	** Rx / Rx message ( i.e. 2nd, 3rd, ... will only calculate
	**         Rx timestamp to Rx timestamp.
	*/
	if ( ( gOBDList[gOBDListIndex].Protocol == ISO9141 ) ||
		 ( gOBDList[gOBDListIndex].Protocol == ISO14230 ) )
	{
		LogPrint("INFORMATION: OBD Response time ( %dmsec )\n",
				( pRxMsg->Timestamp - *ulTxTimestamp ) / 1000 );
	}
}
