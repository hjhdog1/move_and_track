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
** DATE          MODIFICATION
** 04/26/04      Commented out logic to check SID9 INF8.  It is not specified to
**               be cleared via mode $04 in ISO15031-5.
** 05/01/04      Altered logic to flag non-support of INFOTYPE $08 as
**               failure.  OEM must present phase-in plan to CARB if
**               unsupported.
** 05/11/04      Added logic to verify J1699 Spec 11.5 test case #5.17.1
**               Verify ECU support of INFOTYPE $04.
** 05/11/04      Added logic to verify J1699 Spec 11.5 test case #5.17.1
**               Verify ECU support of INFOTYPE $06.
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include "j2534.h"
#include "j1699.h"

/* Function Prototypes */
STATUS VerifyUnsupportedPID (void);
STATUS VerifyVINFormat (void);
STATUS VerifyECUNameFormat (unsigned long  EcuIndex);
int    VerifySid9PidSupportData (void);

/*
*******************************************************************************
** VerifyVehicleInformationSupportAndData -
** Function to verify SID9 vehicle info support and data
*******************************************************************************
*/
//*****************************************************************************
//
//
//
//*****************************************************************************
//	DATE		Modification
//	07/16/03	SF#760692:	Mode 9 model year modifier.
//							Eleminated check for gVIN[9] for determination
//							of vehicle model year with respect to INFOTYPE
//							2, 4, or 6 determination.
//*****************************************************************************
STATUS VerifyVehicleInformationSupportAndData (void)
{
	unsigned long  EcuIndex;
	unsigned long  ENIndex;
	unsigned long  IdIndex;
	unsigned long  Inf2NumResponses = 0;
	unsigned long  Inf3NumItems[OBD_MAX_ECUS] = {0};
	unsigned long  Inf5NumItems[OBD_MAX_ECUS] = {0};
	unsigned long  Inf7NumItems[OBD_MAX_ECUS] = {0};
	unsigned long  NumCalIds = 0;
	SID_REQ        SidReq;
	SID9          *pSid9;
	unsigned long  SidIndex;
	unsigned long  Sid9Limit;
	unsigned char  fInf3Responded = FALSE;
	unsigned char  fInf4Responded = FALSE;
	unsigned char  fInf5Responded = FALSE;
	unsigned char  fInf6Responded = FALSE;
	unsigned char  fInf7Responded = FALSE;
	unsigned char  fInf8Responded = FALSE;
	unsigned char  fInfAResponded = FALSE;
	unsigned char  fInfBResponded = FALSE;
	PASSTHRU_MSG   RxMsg;
	unsigned long  NumMsgs;
	unsigned long  NumCALID = 0;
	unsigned long  NumCVN = 0;
	unsigned long  StartTimeMsecs;
	unsigned long  INF2SupportCount;
	unsigned long  INF4SupportCount;
	unsigned long  INF6SupportCount;
	unsigned long  INF8SupportCount;
	unsigned long  INFASupportCount;
	unsigned long  INFBSupportCount;

	BOOL bTestFailed = FALSE;

	SID9           CALIDCount[OBD_MAX_ECUS];    /* J1699 Rev 11.5 TC# 5.17.7 call for each ECU to report */
	SID9           CVNCount[OBD_MAX_ECUS];      /* J1699 Rev 11.5 TC# 5.17.9 call for each ECU to report */


	memset (CALIDCount, 0x00, sizeof(SID9) * OBD_MAX_ECUS);
	memset (CVNCount,   0x00, sizeof(SID9) * OBD_MAX_ECUS);


	// Clear Error Count for ERROR_RETURN
	ErrorCount();

	/* Request SID 9 support data */
	if (RequestSID9SupportData () != PASS)
	{
		return FAIL;
	}

	/*
	** Find a INF that is not supported by any ECU and request it to see if causes
	** vehicle to drop out of diagnostic mode.
	*/
	if (VerifyUnsupportedPID () != PASS)
	{
		return FAIL;
	}

	/*
	** Determine number of ECUs supporting required INFs
	*/
	INF2SupportCount = 0;
	INF4SupportCount = 0;
	INF6SupportCount = 0;
	INF8SupportCount = 0;
	INFASupportCount = 0;
	INFBSupportCount = 0;
	for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
	{
		if (IsSid9InfSupported (EcuIndex, 0x02) == TRUE)
		{
			INF2SupportCount++;
		}

		if (IsSid9InfSupported (EcuIndex, 0x04) == TRUE)
		{
			INF4SupportCount++;
		}

		if (IsSid9InfSupported (EcuIndex, 0x06) == TRUE)
		{
			INF6SupportCount++;
		}

		if (IsSid9InfSupported (EcuIndex, 0x08) == TRUE)
		{
			// Spark Ignition IPT support
			INF8SupportCount++;

			// if ECU does not only supports CCM requirements (SID 1 PID 1 Data B bit 2==1)
			if ( (Sid1Pid1[EcuIndex].Data[1] & 0x04) == 0 )
			{
				// Spark Ignition operator selection
				if ( gOBDDieselFlag == FALSE )
				{
					// Compression Ignition SID1 PID1 DATA_B Bit_3
					if ( (Sid1Pid1[EcuIndex].Data[1] & 0x08) != 0 )
					{
						if ( gModelYear >= 2010 )
						{
							LogPrint ("FAILURE: ECU %X  SID $1 PID $1 DATA_B Bit_3 does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
							bTestFailed = TRUE;
						}
						else
						{
							LogPrint ("WARNING: ECU %X  SID $1 PID $1 DATA_B Bit_3 does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
						}
					}
				}
				// Compression Ignition operator selection
				else
				{
					if ( gModelYear >= 2010 )
					{
						LogPrint ("FAILURE: ECU %X  SID $9 IPT (INF8) support does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
						bTestFailed = TRUE;
					}
					else
					{
						LogPrint ("WARNING: ECU %X  SID $9 IPT (INF8) support does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
					}

					// Spark Ignition SID1 PID1 DATA_B Bit_3
					if ( (Sid1Pid1[EcuIndex].Data[1] & 0x08) == 0 )
					{
						if ( gModelYear >= 2010 )
						{
							LogPrint ("FAILURE: ECU %X  SID $1 PID $1 DATA_B Bit_3 does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
							bTestFailed = TRUE;
						}
						else
						{
							LogPrint ("WARNING: ECU %X  SID $1 PID $1 DATA_B Bit_3 does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
						}
					}
				}
			} // end if ECU does not only supports CCM requirements (SID 1 PID 1 Data B bit 2==1)
		} // end if (IsSid9InfSupported (EcuIndex, 0x08) == TRUE)

		if (IsSid9InfSupported (EcuIndex, 0x0A) == TRUE)
		{
			INFASupportCount++;
		}

		if (IsSid9InfSupported (EcuIndex, 0x0B) == TRUE)
		{
			// Compression Ignition IPT support
			INFBSupportCount++;

			// if ECU does not only supports CCM requirements (SID 1 PID 1 Data B bit 2==1)
			if ( (Sid1Pid1[EcuIndex].Data[1] & 0x04) == 0 )
			{
				// Compression Ignition operator selection
				if ( gOBDDieselFlag == TRUE )
				{
					// Compression Ignition SID1 PID1 DATA_B Bit_3
					if ( (Sid1Pid1[EcuIndex].Data[1] & 0x08) == 0 )
					{
						if ( gModelYear >= 2010 )
						{
							LogPrint ("FAILURE: ECU %X  SID $1 PID $1 DATA_B Bit_3 does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
							bTestFailed = TRUE;
						}
						else
						{
							LogPrint ("WARNING: ECU %X  SID $1 PID $1 DATA_B Bit_3 does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
						}
					}
				}
				// Spark Ignition operator selection
				else
				{
					if ( gModelYear >= 2010 )
					{
						LogPrint ("FAILURE: ECU %X  SID $9 IPT (INFB) support does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
						bTestFailed = TRUE;
					}
					else
					{
						LogPrint ("WARNING: ECU %X  SID $9 IPT (INFB) support does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n", GetEcuId(EcuIndex) );
					}

					// Compression Ignition SID1 PID1 DATA_B Bit_3
					if ( (Sid1Pid1[EcuIndex].Data[1] & 0x08) != 0 )
					{
						if ( gModelYear >= 2010 )
						{
							LogPrint ("FAILURE: ECU %X  SID1 PID1 DATA_B Bit_3 does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n");
							bTestFailed = TRUE;
						}
						else
						{
							LogPrint ("WARNING: ECU %X  SID1 PID1 DATA_B Bit_3 does not match Operator Selected Ignition Type (Required for MY 2010 and later vehicles)\n");
						}
					}
				} // end gOBDDieselFlag == FALSE
			} // end if ECU does not only supports CCM requirements (SID 1 PID 1 Data B bit 2==1)
		} // end if (IsSid9InfSupported (EcuIndex, 0x0B) == TRUE)
	} // end for ( EcuIndex


	// Verify that one and only one ECU supports INF $2 (VIN)
	if ( INF2SupportCount > 1 )
	{
		LogPrint ("FAILURE: SID $9 INF $2 supported by multiple controllers!\n");
		bTestFailed = TRUE;
	}

	// Verify that INF $4 (CALID) is supported by the expected number of ECUs (from user prompt)
	if ( INF4SupportCount != gUserNumEcus )
	{
		LogPrint ("FAILURE: SID $9 INF $4 not supported by expected number of controllers!\n");
		bTestFailed = TRUE;
	}

	// Verify that the number of ECUs that support INF $6 (CVN) matches or exceeds the number of ECUs (from user prompt)
	if ( INF6SupportCount < gUserNumEcusReprgm )
	{
		LogPrint ("FAILURE: SID $9 INF $6 not supported by expected number of controllers!\n");
		bTestFailed = TRUE;
	}

	// Verify that INF $8 and INF $B are not both supported
	if ( INF8SupportCount != 0 && INFBSupportCount != 0 )
	{
		LogPrint ("FAILURE: Both SID $9 INF $8 and INF $B are supported!\n");
		bTestFailed = TRUE;
	}

	 // Verify that INF $8 or INF $B (IPT) is supported by at least one ECU for MY 2007 and later
	if ( INF8SupportCount == 0 && INFBSupportCount == 0 && gModelYear >= 2007 )
	{
		LogPrint ("FAILURE: Neither SID $9 INF $8 nor INF $B is supported! (Support for one required for MY 2007 and later vehicles)\n");
		bTestFailed = TRUE;
	}

	// Verify that INF $A is supported for MY 2010 and later
	if ( gModelYear >= 2010 &&
	     INFASupportCount != gUserNumEcus )
	{
		LogPrint ("FAILURE: SID $9 INF $A is not supported by all ECUs! (Required for MY 2010 and later vehicles)\n");
		bTestFailed = TRUE;
	}

	// If there where any errors in the data, fail
	if ( bTestFailed == TRUE )
	{
		ERROR_RETURN;
		bTestFailed = FALSE;
	}


	/*
	** For each INF group
	** Verify that all SID 9 INF data is valid
	** Request user supplied expected OBD ECU responses
	*/
	for ( IdIndex = 0x01; IdIndex <= 0x0B /*0x100*/; IdIndex++ )
	{
		/* Mode 9 INF's 1,3,5,7 should be ignored for ISO15765
		   and INF's $0A and $0B should be handled only for ISO15765 */
		if ( gOBDList[gOBDListIndex].Protocol == ISO15765 )
		{
			if ( IdIndex ==  0x01 || IdIndex ==  0x03 || IdIndex ==  0x05 || IdIndex ==  0x07 )
				continue;
		}
		else
		{
			if ( IdIndex ==  0x0A || IdIndex ==  0x0B )
				continue;
		}

		/* If INF is supported by any ECU, request it */
		if ( IsSid9InfSupported (-1, IdIndex) == TRUE )
		{
			SidReq.SID    = 9;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = (unsigned char)IdIndex;

			if ( SidRequest( &SidReq, SID_REQ_NORMAL ) != PASS)
			{
				/*
				** If request for CVN fails on M/Y 2004 or less, ignore it because it
				** could be due to the extended (up to 30sec) response time that is
				** not allowed in 2005 and beyond.
				*/
				if ( (IdIndex == INF_TYPE_CVN) && (gModelYear < 2005) )
				{
					StartTimeMsecs = GetTickCount ();
					while ( (GetTickCount() - StartTimeMsecs) < 30000 )
					{
						NumMsgs = 1;
						PassThruReadMsgs (gOBDList[gOBDListIndex].ChannelID,
						                  &RxMsg,
						                  &NumMsgs,
						                  gOBDMaxResponseTimeMsecs);
						/* If a message was received, process it */
						if (NumMsgs == 1)
						{
							/* Save all read messages in the log file */
							LogMsg(&RxMsg, LOG_NORMAL_MSG);
						}
					}
					break;
				}

				LogPrint ("FAILURE: SID $9 INF $%02X request failed\n", IdIndex );
				ERROR_RETURN;
			}

			for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
			{
				/* If INF is not supported, skip to next ECU */
				if ( IsSid9InfSupported (EcuIndex, IdIndex) == FALSE )
					continue;

				/* Check the data to see if it is valid */
				pSid9 = (SID9 *)&gOBDResponse[EcuIndex].Sid9Inf[0];

				if ( gOBDResponse[EcuIndex].Sid9InfSize == 0 )
				{
					LogPrint ("FAILURE: ECU %X  No SID $9 INF $%02X data\n", GetEcuId(EcuIndex), IdIndex );
					ERROR_RETURN;
				}

				Sid9Limit = ( gOBDList[gOBDListIndex].Protocol == ISO15765) ?
				              1 : (gOBDResponse[EcuIndex].Sid9InfSize / sizeof (SID9));

				for (SidIndex = 0; SidIndex < Sid9Limit; SidIndex++)
				{
					/* Check various INF values for validity */
					switch(pSid9[SidIndex].INF)
					{
						case INF_TYPE_VIN_COUNT:
						{
							if ( (gOBDList[gOBDListIndex].Protocol != ISO15765) &&
							     (pSid9[SidIndex].NumItems         != 0x05) )
							{
								LogPrint ( "FAILURE: ECU %X  SID $9 INF $1 (VIN Count) NumItems = %d (should be 5)\n",
								           GetEcuId(EcuIndex),
								           pSid9[SidIndex].NumItems );
								ERROR_RETURN;
							}
						}
						break;

						case INF_TYPE_VIN:
						{
							// Copy the VIN into the global array
							if (gOBDList[gOBDListIndex].Protocol == ISO15765)
							{
								memcpy (&gVIN[0], &pSid9[0].Data[0], 17);

								/// Check that there are no pad bytes included in message
								if (pSid9[0].Data[20] != 0x00)
								{
									LogPrint ("FAILURE: ECU %X  SID $9 INF $2 format error! (Must be 17 chars and no pad bytes)\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else
							{
								if (SidIndex == 0)
								{
									gVIN[0] = pSid9[SidIndex].Data[3];
								}
								else if (SidIndex < 5)
								{
									memcpy(&gVIN[SidIndex*4 - 3], &pSid9[SidIndex].Data[0], 4);
								}
							}

							if ( (gOBDList[gOBDListIndex].Protocol == ISO15765) ||
							     (SidIndex == 4) )
							{
								// Check for INF2 (VIN) support from multiple controllers
								if ( ++Inf2NumResponses != 0x01 )
								{
									LogPrint ( "FAILURE: SID $9 INF $2 response from multiple controllers!\n" );
									ERROR_RETURN;
								}

								/* Isolated VIN verification logic */
								if (VerifyVINFormat () != PASS)
								{
									ERROR_RETURN;
								}
							}
						}
						break;

						case INF_TYPE_CALID_COUNT:
						{
							fInf3Responded = TRUE;

							/* Response should be a multiple of four if not ISO15765 */
							if ( (gOBDList[gOBDListIndex].Protocol != ISO15765) &&
							     (pSid9[SidIndex].NumItems & 0x03) )
							{
								LogPrint ( "FAILURE: ECU %X  SID $9 INF $3 (CALID Count) NumItems = %d (should be a multiple of 4)\n",
								           GetEcuId(EcuIndex),
								           pSid9[SidIndex].NumItems );
								ERROR_RETURN;
							}

							Inf3NumItems[EcuIndex] = pSid9[SidIndex].NumItems;

							/* Add to number of CALIDs */
							NumCalIds += Inf3NumItems[EcuIndex];
						}
						break;

						case INF_TYPE_CALID:
						{
							fInf4Responded = TRUE;

							CALIDCount[EcuIndex].INF = INF_TYPE_CALID;
							CALIDCount[EcuIndex].NumItems = (unsigned char)Sid9Limit;

							NumCALID += pSid9[SidIndex].NumItems;

							if (VerifyCALIDFormat (EcuIndex, Inf3NumItems[EcuIndex]) != PASS)
							{
								ERROR_RETURN;
							}

							/* Response should match INF3 if not ISO15765 */
							if ((gOBDList[gOBDListIndex].Protocol != ISO15765) &&
							    pSid9[Inf3NumItems[EcuIndex]-1].NumItems != Inf3NumItems[EcuIndex])
							{
								LogPrint ( "FAILURE: ECU %X  SID $9 INF $4 (CALID) NumItems = %d (should match INF3 CALID Count %d)\n",
								           GetEcuId(EcuIndex),
								           pSid9[Inf3NumItems[EcuIndex]-1].NumItems, Inf3NumItems[EcuIndex]);
								ERROR_RETURN;
							}

							if ( gModelYear >= 2009 && pSid9[SidIndex].NumItems != 1)
							{
								LogPrint ( "WARNING: ECU %X  SID $9 INF $4 (CALID) NumItems = %d (should be 1 for MY 2009 and later)\n",
								           GetEcuId(EcuIndex),
								           pSid9[SidIndex].NumItems );
							}

							SidIndex = Sid9Limit;       /* continue with next ECU */
						}
						break;

						case INF_TYPE_CVN_COUNT:
						{
							fInf5Responded = TRUE;
							Inf5NumItems[EcuIndex] = pSid9[SidIndex].NumItems;
						}
						break;

						case INF_TYPE_CVN:
						{
							fInf6Responded = TRUE;

							CVNCount[EcuIndex].INF = INF_TYPE_CVN;
							CVNCount[EcuIndex].NumItems++;

							NumCVN += pSid9[SidIndex].NumItems;

							/* Response should match INF5 if not ISO15765 */
							if ((gOBDList[gOBDListIndex].Protocol != ISO15765) &&
							    (pSid9[Inf5NumItems[EcuIndex] - 1].NumItems != Inf5NumItems[EcuIndex]))
							{
								LogPrint ( "FAILURE: ECU %X  SID $9 INF $6 (CVN) NumItems = %d (should match INF5 CVN Count)\n",
								           GetEcuId(EcuIndex),
								           pSid9[Inf5NumItems[EcuIndex] - 1].NumItems );
								ERROR_RETURN;
							}

							if ( gModelYear >= 2009 && pSid9[SidIndex].NumItems != 1)
							{
								LogPrint ( "WARNING: ECU %X  SID $9 INF $6 (CVN) NumItems = %d (should be 1 for MY 2009 and later)\n",
								           GetEcuId(EcuIndex),
								           pSid9[SidIndex].NumItems );
							}
						}
						break;

						case INF_TYPE_IPT_COUNT:
						{
							fInf7Responded = TRUE;

							/* non-ISO15765 should report 0x08 */
							if ( (gOBDList[gOBDListIndex].Protocol != ISO15765) &&
							     (pSid9[SidIndex].NumItems != 0x08) )
							{
								LogPrint ( "FAILURE: ECU %X  SID $9 INF $7 (IPT Count) NumItems = %d (should be 0x08)\n",
								           GetEcuId(EcuIndex),
								           pSid9[SidIndex].NumItems);
								ERROR_RETURN;
							}

							Inf7NumItems[EcuIndex] = pSid9[SidIndex].NumItems;
						}
						break;

						case INF_TYPE_IPT:
						{
							fInf8Responded = TRUE;

							if ( VerifyINF8Data (EcuIndex) != PASS )
							{
								ERROR_RETURN;
							}

							/* Response should match INF7 if not ISO15765 */
							if ((gOBDList[gOBDListIndex].Protocol != ISO15765) &&
							    (pSid9[Inf7NumItems[EcuIndex] - 1].NumItems != Inf7NumItems[EcuIndex]))
							{
								LogPrint ( "FAILURE: ECU %X  SID $9 INF $8 (IPT) NumItems = %d (should match INF7 IPT Count)\n",
								           GetEcuId(EcuIndex),
								           pSid9[Inf7NumItems[EcuIndex] - 1].NumItems );
								ERROR_RETURN;
							}
						}
						break;

						case INF_TYPE_ECUNAME:
						{
							fInfAResponded = TRUE;

							if (gOBDList[gOBDListIndex].Protocol == ISO15765)
							{
								// ECU Name Data must contain 20 bytes of data
								if ( gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 20 )
								{
									LogPrint ( "FAILURE: ECU %X  SID $9 INF $A (ECU NAME) Data Size Error = %d (Must be 20 bytes!)\n",
									           GetEcuId(EcuIndex),
									           (gOBDResponse[EcuIndex].Sid9InfSize - 0x02) );
									ERROR_RETURN;
								}

								// SID9 INFA NODI must equal $01
								if ( pSid9[SidIndex].NumItems != 0x01 )
								{
									LogPrint ( "FAILURE: ECU %X  SID $9 INF $A NODI = %d (Must be 1)\n",
									           GetEcuId(EcuIndex),
									           pSid9[SidIndex].NumItems );
									ERROR_RETURN;
								}

								if ( VerifyECUNameFormat (EcuIndex) != PASS )
								{
									ERROR_RETURN;
								}

								// Check for duplicate ECU Names
								for ( ENIndex = 0; ENIndex < EcuIndex; ENIndex++ )
								{
									if ( strcmp(&gOBDResponse[EcuIndex].Sid9Inf[2], &gOBDResponse[ENIndex].Sid9Inf[2]) == 0 )
									{
										LogPrint ( "FAILURE: ECU %X and ECU %X  SID $9 INF $A, Duplicate ECU names\n",
										           GetEcuId(EcuIndex),
										           GetEcuId(ENIndex) );
										ERROR_RETURN;
									}
								}
							}
						}
						break;

						case INF_TYPE_IPD:
						{
							fInfBResponded = TRUE;

							if ( VerifyINFBData (EcuIndex) != PASS )
							{
								ERROR_RETURN;
							}
						}
						break;

						default:
						{
							/* Non-OBD INF type */
						}
						break;
					}
				}
			}
		}
	}


	/* Verify ECU support for INFOTYPE $04 & $06 */
	/* Mode 9 Prompt 2 must be at least equal to or less than the number of CVNs. */
	for ( EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++ )
	{
		/* J1699 Rev 11.5 TC# 5.17.7 call for each ECU to report */
		if (CALIDCount[EcuIndex].NumItems == 0x00)
		{
			LogPrint ("FAILURE: ECU %X  SID $9 INF $4, ECU did not report\n", GetEcuId(ENIndex) );
			bTestFailed = TRUE;
		}

		/* All msgs must be accounted for as specified by CALIDCount */
		NumCalIds -= CALIDCount[EcuIndex].NumItems;

		/* Increment CVN count and evaluate later...*/
		if (CVNCount[EcuIndex].NumItems == 0x00)
		{
			LogPrint ("FAILURE: ECU %X  SID $9 INF $6, ECU did not report\n", GetEcuId(ENIndex) );
			bTestFailed = TRUE;
		}
	}

	/* Verify there are at least as many CALIDs as OBD ECUs */
	if ( NumCALID < gUserNumEcus )
	{
		LogPrint ( "FAILURE: SID $9 INF $4, the number of CALID responses (%d) must be at least the number of OBD ECUs (%d)\n",
		           NumCALID,
		           gUserNumEcus );
		bTestFailed = TRUE;
	}

	/* Verify there are at least as many CVNs as Reprogrammable ECUs */
	if ( NumCVN < gUserNumEcusReprgm )
	{
		LogPrint ( "FAILURE: SID $9 INF $6, the number of CVN responses (%d) must be at least the number of Reprogrammable ECUs (%d)\n",
		           NumCVN,
		           gUserNumEcusReprgm );
		bTestFailed = TRUE;
	}

	/* reordered test to allow execution prior to other data checks for INF support */
	/* Try group support if ISO15765 */
	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{
		if (VerifyGroupVehicleInformationSupport() == FAIL)
		{
			bTestFailed = TRUE;
		}
	}

	/* If M/Y 2005 or greater, INFO type 8 must be supported */
	/* If not supported then OEM will present Infotype phase in plan */
	if ( gOBDDieselFlag == FALSE &&
	     fInf8Responded == FALSE &&
	     gModelYear >= 2005 )
	{
		LogPrint ("FAILURE: No SID $9 INF $8 response (Required for Spark Ignition vehicles MY 2005 and later)\n");
		bTestFailed = TRUE;
	}

	/* If M/Y 2010 or greater, INFO type B must be supported */
	/* If not supported then OEM will present Infotype phase in plan */
	if ( gOBDDieselFlag == TRUE &&
	     fInfBResponded == FALSE &&
	     gModelYear >= 2010 )
	{
		LogPrint ("FAILURE: No SID $9 INF $B response (Required for Compression Ignition vehicles MY 2010 and later)\n");
		bTestFailed = TRUE;
	}

	// If there where any errors in the data, fail
	if ( bTestFailed == TRUE )
	{
		ERROR_RETURN;
	}

	if ( ErrorCount() != 0 )
	{
		return(FAIL);
	}

	return PASS;
}

/******************************************************************************
**
**	Function:	RequestSID9SupportData
**
**	Purpose:	Purpose of this function is to Request SID 9 support data
**
*******************************************************************************
**
**	DATE		MODIFICATION
**	10/22/03	Isolated Request SID 9 support data
**	05/11/04	Added logic, per J1699 ver 11.5 TC 5.17.3, request next
**              unsupported INFOTYPE-support INFOTYPE and verify ECU did
**              not drop out.
**  06/16/04    Added logic to account for upper $E0 limit and validate
**              support.
**  07/15/04    Correct logic error associated with the evaluation of $E0
**              support.
**
*******************************************************************************
*/
STATUS RequestSID9SupportData (void)
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	unsigned long ulInfSupport;  /* used to determine $E0 support indication */

	SID_REQ SidReq;

	for (IdIndex = 0x00; IdIndex < 0x100; IdIndex += 0x20)
	{
		SidReq.SID = 9;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = (unsigned char)IdIndex;
		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			/* There must be a response to PID 0x00 */
			if (IdIndex == 0x00)
			{
				LogPrint ("FAILURE: SID $9 support request failed\n");
				return FAIL;
			}
		}

		/* Check if we need to request the next group */
		for (EcuIndex = 0; EcuIndex < gUserNumEcusReprgm; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid9InfSupport[IdIndex >> 5].IDBits[3] & 0x01)
			{
				break;
			}
		}
		if (EcuIndex >= gUserNumEcusReprgm)
		{
			break;
		}
	}

	/* Flag error if ECU indicates no support */
	if (VerifySid9PidSupportData() == FAIL)
	{
		ERROR_RETURN;
	}

	/* Enhance logic to verify support information if request is at upper limit of $E0 */
	if ( IdIndex == 0xE0 )
	{
		/* Init variable to no-support */
		ulInfSupport = 0;

		/* For each ECU */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			/* If MID is supported, keep looking */
			if ( ( gOBDResponse[EcuIndex].Sid9InfSupport[IdIndex >> 5].IDBits[0]		||
			       gOBDResponse[EcuIndex].Sid9InfSupport[IdIndex >> 5].IDBits[1]		||
			       gOBDResponse[EcuIndex].Sid9InfSupport[IdIndex >> 5].IDBits[2]		||
			     ( gOBDResponse[EcuIndex].Sid9InfSupport[IdIndex >> 5].IDBits[3] & 0xFE ) ) != 0x00)
			{
				/* Flag as support indicated! */
				ulInfSupport = 1;
			}
		}

		/* If no ECU indicated support, flag as error. */
		if ( ulInfSupport == 0 )
		{
			LogPrint ("FAILURE: SID $9 INF $E0 support failure.  No INF support indicated!\n");
			ERROR_RETURN;
		}
	}
	else
	{
		/*
		** Per J1699 rev 11.5 TC# 5.17.3 - Request request next
		**              unsupported INFOTYPE-support INFOTYPE
		*/
		SidReq.SID = 9;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = (unsigned char)IdIndex += 0x20;

		gIgnoreNoResponse = TRUE;

		if ( SidRequest(&SidReq, SID_REQ_NORMAL) == PASS )
		{
			if (gOBDList[gOBDListIndex].Protocol == ISO15765)
			{
				LogPrint ("FAILURE: TC# 5.17.3 - Unexpected response from ECU!\n");
				gIgnoreNoResponse = FALSE;
				return FAIL;
			}
		}

		gIgnoreNoResponse = FALSE;
	}

	/*
	** Per J1699 rev 11.5 TC# 5.17.3 - Verify ECU did not drop out.
	*/
	if (VerifyLinkActive() != PASS)
	{
		return FAIL;
	}

	return PASS;
}

//*****************************************************************************
//
//	Function:	VerifyUnsupportedPID
//
//	Purpose:	Purpose of this function is to Request SID 9 unsupported PIDs
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	10/22/03	Isolated Request SID 9 support data
//
//*****************************************************************************
STATUS VerifyUnsupportedPID (void)
{
	unsigned long IdIndex;

	SID_REQ SidReq;

	/*
	** Find a INF that is not supported by any ECU and request it to see if causes
	** vehicle to drop out of diagnostic mode.
	*/

	/* For each INF group */
	for (IdIndex = 0x01; IdIndex < 0x100; IdIndex++)
	{
		/* If not supported by any ECUs, request it */
		if (IsSid9InfSupported (-1, IdIndex) == FALSE)
		{
			gIgnoreNoResponse = TRUE;
			SidReq.SID = 9;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = (unsigned char)IdIndex;
			SidRequest(&SidReq, SID_REQ_NORMAL);
			gIgnoreNoResponse = FALSE;

			/* Done */
			break;
		}
	}

	return PASS;
}

/******************************************************************************
**
**	Function:	VerifyVINFormat
**
**	Purpose:	Purpose of this function is to verify the VIN format
**				for correct format.  In the event the format fails
**				defined criteria, an error is returned.
**
*******************************************************************************
**
**	DATE		MODIFICATION
**	10/22/03	Isolated VIN verification logic
**	06/17/04	Update VIN character validation as documented in J1699 version
**              11.6, table 79.
**
******************************************************************************/
STATUS VerifyVINFormat (void)
{
	unsigned long VinIndex;

	// Print VIN string to log file
	LogPrint ("VIN = %s\n", gVIN);

	/* Check all VIN characters for validity */
	for (VinIndex = 0; VinIndex < 17; VinIndex++)
	{
		if ((gVIN[VinIndex] <  '0') || (gVIN[VinIndex] >  'Z') ||
		    (gVIN[VinIndex] == 'I') || (gVIN[VinIndex] == 'O') ||
		    (gVIN[VinIndex] == 'Q') || (gVIN[VinIndex] == ':') ||
		    (gVIN[VinIndex] == ';') || (gVIN[VinIndex] == '<') ||
		    (gVIN[VinIndex] == '>') || (gVIN[VinIndex] == '=') ||
		    (gVIN[VinIndex] == '?') || (gVIN[VinIndex] == '@'))
		{
			break;
		}
	}

	if (VinIndex != 17)
	{
		LogPrint ("FAILURE: Invalid VIN information\n");
		return FAIL;
	}


	/* Check that VIN model year character matches user input model year */
	if ( ( gVIN[9] == '1' && gModelYear != 2001 ) ||
	     ( gVIN[9] == '2' && gModelYear != 2002 ) ||
	     ( gVIN[9] == '3' && gModelYear != 2003 ) ||
	     ( gVIN[9] == '4' && gModelYear != 2004 ) ||
	     ( gVIN[9] == '5' && gModelYear != 2005 ) ||
	     ( gVIN[9] == '6' && gModelYear != 2006 ) ||
	     ( gVIN[9] == '7' && gModelYear != 2007 ) ||
	     ( gVIN[9] == '8' && gModelYear != 2008 ) ||
	     ( gVIN[9] == '9' && gModelYear != 2009 ) ||
	     ( gVIN[9] == 'A' && gModelYear != 2010 ) ||
	     ( gVIN[9] == 'B' && gModelYear != 1981 && gModelYear != 2011 ) ||
	     ( gVIN[9] == 'C' && gModelYear != 1982 && gModelYear != 2012 ) ||
	     ( gVIN[9] == 'D' && gModelYear != 1983 && gModelYear != 2013 ) ||
	     ( gVIN[9] == 'E' && gModelYear != 1984 && gModelYear != 2014 ) ||
	     ( gVIN[9] == 'F' && gModelYear != 1985 && gModelYear != 2015 ) ||
	     ( gVIN[9] == 'G' && gModelYear != 1986 && gModelYear != 2016 ) ||
	     ( gVIN[9] == 'H' && gModelYear != 1987 && gModelYear != 2017 ) ||
	     ( gVIN[9] == 'J' && gModelYear != 1988 && gModelYear != 2018 ) ||
	     ( gVIN[9] == 'K' && gModelYear != 1989 && gModelYear != 2019 ) ||
	     ( gVIN[9] == 'L' && gModelYear != 1990 && gModelYear != 2020 ) ||
	     ( gVIN[9] == 'M' && gModelYear != 1991 && gModelYear != 2021 ) ||
	     ( gVIN[9] == 'N' && gModelYear != 1992 && gModelYear != 2022 ) ||
	     ( gVIN[9] == 'P' && gModelYear != 1993 && gModelYear != 2023 ) ||
	     ( gVIN[9] == 'R' && gModelYear != 1994 && gModelYear != 2024 ) ||
	     ( gVIN[9] == 'S' && gModelYear != 1995 && gModelYear != 2025 ) ||
	     ( gVIN[9] == 'T' && gModelYear != 1996 ) ||
	     ( gVIN[9] == 'V' && gModelYear != 1997 ) ||
	     ( gVIN[9] == 'W' && gModelYear != 1998 ) ||
	     ( gVIN[9] == 'X' && gModelYear != 1999 ) ||
	     ( gVIN[9] == 'Y' && gModelYear != 2000 ) )
	{
		LogPrint ( "FAILURE: VIN year character (%c) does not match user entry (%d)\n", gVIN[9], gModelYear );
		return FAIL;
	}

	else if ( gVIN[9] == 'Z' )
	{
		LogPrint ( "FAILURE: Invalid VIN year character (%c) not currently defined)\n", gVIN[9] );
		return FAIL;
	}

	else if ( gModelYear < 1981 || gModelYear > 2025 )
	{
		LogPrint ( "FAILURE: Model Year %d does not currently have a VIN character defined)\n", gModelYear );
		return FAIL;
	}


	return PASS;
}

//*****************************************************************************
//
//	Function:	VerifyCALIDFormat
//
//	Purpose:	Purpose of this function is to verify the CALID format
//				for correct format.  In the event the format fails
//				defined criteria, an error is returned.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	11/01/03	Isolated CALID verification logic
//
//*****************************************************************************
STATUS VerifyCALIDFormat (unsigned long  EcuIndex, unsigned long  Inf3NumItems)
{
	unsigned long  SidIndex;
	unsigned long  Sid9Limit;
	unsigned long  Inf4NumItems;
	unsigned long  ItemIndex;
	unsigned long  ByteIndex;
	char           buffer[20];
	SID9          *pSid9;

	if (gOBDResponse[EcuIndex].Sid9InfSize == 0)
	{
		LogPrint ("FAILURE: ECU %X  No SID $9 INF $4 data\n", GetEcuId(EcuIndex) );
		return FAIL;
	}

	pSid9 = (SID9 *)&gOBDResponse[EcuIndex].Sid9Inf[0];

	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{
		Inf4NumItems = pSid9->NumItems;

		for (ItemIndex = 0; ItemIndex < Inf4NumItems; ItemIndex++)
		{
			memcpy (buffer, &(pSid9->Data[ItemIndex * 16]), 16);

			for (ByteIndex = 0; ByteIndex < 16; ByteIndex++)
			{
				if ( (buffer[ByteIndex] < ' ') || (buffer[ByteIndex] > 'z') )
				{
					break;
				}
			}

			for (; ByteIndex < 16; ByteIndex++)
			{
				if (buffer[ByteIndex] != 0)
				{
					LogPrint ( "FAILURE: ECU %X  CALID not zero padded on right\n", GetEcuId(EcuIndex) );
					return FAIL;
				}
			}

			buffer[16] = 0;
			LogPrint ( "INFORMATION: ECU %X  CALID: %s\n", GetEcuId(EcuIndex), buffer );
		}
	}
	else
	{
		Sid9Limit = gOBDResponse[EcuIndex].Sid9InfSize / sizeof (SID9);

		for (SidIndex = 0; SidIndex < Sid9Limit; SidIndex += 4)
		{
			for (ItemIndex = 0, ByteIndex = 0; ItemIndex < 4; ItemIndex++, pSid9++)
			{
				buffer[ByteIndex++] = pSid9->Data[0];
				buffer[ByteIndex++] = pSid9->Data[1];
				buffer[ByteIndex++] = pSid9->Data[2];
				buffer[ByteIndex++] = pSid9->Data[3];
			}

			for (ByteIndex = 0; ByteIndex < 16; ByteIndex++)
			{
				if ( (buffer[ByteIndex] < ' ') || (buffer[ByteIndex] > 'z') )
				{
					break;
				}
			}

			for (; ByteIndex < 16; ByteIndex++)
			{
				if (buffer[ByteIndex] != 0)
				{
					LogPrint ( "FAILURE: ECU %X  CALID not zero padded on right\n", GetEcuId(EcuIndex) );
					return FAIL;
				}
			}

			buffer[ByteIndex] = 0;
			LogPrint ( "INFORMATION: ECU %X  CALID: %s\n", GetEcuId (EcuIndex), buffer );
		}
	}

	return PASS;
}

/******************************************************************************
**
**	Function:	VerifyECUNameFormat
**
**	Purpose:	Purpose of this function is to verify the ECU Name format
**				for correct format.  In the event the format fails
**				defined criteria, an error is returned.
**
*******************************************************************************
**
**	DATE		MODIFICATION
**	07/07/07	Isolated ECU Name verification logic
**
******************************************************************************/
#define TABLE_SIZE  25
#define ACRONYM_STRING_SIZE 15
#define TEXTNAME_STRING_SIZE 16
const char szECUAcronym[TABLE_SIZE][ACRONYM_STRING_SIZE] =
	{ "ABS ABS1 ABS2",
	  "AFCM AFC1 AFC2",
	  "AHCM AHC1 AHC2",
	  "AWDC AWD1 AWD2",
	  "BECM BEC1 BEC2",
	  "BSCM BSC1 BSC2",
	  "CRCM CRC1 CRC2",
	  "CTCM CTC1 CTC2",
	  "DMCM DMC1 DMC2",
	  "ECCI ECC1 ECC2",
	  "ECM ECM1 ECM2",
	  "FACM FAC1 FAC2",
	  "FICM FIC1 FIC2",
	  "FPCM FPC1 FPC2",
	  "4WDC 4WD1 4WD2",
	  "GPCM GPC1 GPC2",
	  "GSM GSM1 GSM2",
	  "HPCM HPC1 HPC2",
	  "IPC IPC1 IPC2",
	  "PCM PCM1 PCM2",
	  "RDCM RDC1 RDC2",
	  "SGCM SGC1 SGC2",
	  "TACM TAC1 TAC2",
	  "TCCM TCC1 TCC2",
	  "TCM TCM1 TCM2" };

const char szECUTextName[TABLE_SIZE][16] =
	{ "AntiLockBrake",
	  "AltFuelCtrl",
	  "AuxHeatCtrl",
	  "AllWhlDrvCtrl",
	  "B+EnergyCtrl",
	  "BrakeSystem",
	  "CruiseControl",
	  "CoolTempCtrl",
	  "DriveMotorCtrl",
	  "EmisCritInfo",
	  "EngineControl",
	  "FuelAddCtrl",
	  "FuelInjCtrl",
	  "FuelPumpCtrl",
	  "4WhlDrvClCtrl",
	  "GlowPlugCtrl",
	  "GearShiftCtrl",
	  "HybridPtCtrl",
	  "InstPanelClust",
	  "PowertrainCtrl",
	  "ReductantCtrl",
	  "Start/GenCtrl",
	  "ThrotActCtrl",
	  "TransfCaseCtrl",
	  "TransmisCtrl" };

STATUS VerifyECUNameFormat ( unsigned long  EcuIndex )
{
	SID9           *pSid9;
	unsigned int    ByteIndex;
	char            buffer[21];
	char           *pString;
	char            AcronymBuffer[ACRONYM_STRING_SIZE];
	char            AcronymNumber = 0;
	char            TextNameNumber = 0;
	unsigned int    CompSize;
	unsigned int    TableIndex;
	unsigned int    AcronymTableIndex;
	unsigned char   bTestFailed = FALSE;
	unsigned char   bMatchFound = FALSE;

	if (gOBDResponse[EcuIndex].Sid9InfSize == 0)
	{
		LogPrint ( "FAILURE: ECU %X  No SID $9 INF $A data\n", GetEcuId(EcuIndex) );
		return FAIL;
	}

	pSid9 = (SID9 *)&gOBDResponse[EcuIndex].Sid9Inf[0];

	memcpy (buffer, &(pSid9->Data[0]), 20);

	// Check all ECU Acronym characters for validity
	for ( ByteIndex = 0; ByteIndex < 4; ByteIndex++ )
	{
		if ( buffer[ByteIndex] == 0x00 )
		{
			break;
		}
		else if ( !( buffer[ByteIndex] >= '1' && buffer[ByteIndex] <= '9' ) &&
		          !( buffer[ByteIndex] >= 'A' && buffer[ByteIndex] <= 'Z' ) )
		{
			if ( gModelYear >= 2010 )
			{
				LogPrint ( "FAILURE: ECU %X  Invalid ECU acronym/number character (byte %d)\n",
				           GetEcuId(EcuIndex),
				           ByteIndex );
				bTestFailed = TRUE;
			}
			else
			{
				LogPrint ( "WARNING: ECU %X  Invalid ECU acronym/number character (byte %d)\n",
				           GetEcuId(EcuIndex),
				           ByteIndex );
			}
		}
	}

	// Check that ECU Acronym is approved
	for ( TableIndex = 0; TableIndex < TABLE_SIZE; TableIndex++ )
	{
		memcpy ( AcronymBuffer, &(szECUAcronym[TableIndex]), ACRONYM_STRING_SIZE );

		// get pointer to first string
		pString = strtok ( (char*)&(AcronymBuffer), " " );
		do
		{
			// if found match
			if ( strncmp ( pString, &buffer[0], ByteIndex ) == 0 )
			{
				bMatchFound = TRUE;
				break;
			}

			// get pointer to next string
			pString = strtok ('\0', " " );
		} while ( pString && bMatchFound != TRUE );

		if ( bMatchFound == TRUE )
		{
			break;
		}
	}

	if ( TableIndex == TABLE_SIZE )
	{
		if ( gModelYear >= 2010 )
		{
			LogPrint ( "FAILURE: ECU %X  Not an approved ECU acronym/number\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
		else
		{
			LogPrint ( "WARNING: ECU %X  Not an approved ECU acronym/number\n", GetEcuId(EcuIndex) );
		}
	}

	// if last character was a number
	if ( buffer[ByteIndex-1] >= '1' && buffer[ByteIndex-1] <= '9' )
	{
		// save the number for Acronym/Text Name verification
		AcronymNumber = buffer[ByteIndex-1];
	}

	// save index for Acronym/Text Name verification
	AcronymTableIndex = TableIndex;

	// Check for zero padding of ECU Acronym
	for ( ; ByteIndex < 4; ByteIndex++ )
	{
		if ( buffer[ByteIndex] != 0x00 )
		{
			if ( gModelYear >= 2010 )
			{
				LogPrint ( "FAILURE: ECU %X  ECU acronym/number not zero padded on right\n", GetEcuId(EcuIndex) );
				bTestFailed = TRUE;
			}
			else
			{
				LogPrint ( "WARNING: ECU %X  ECU acronym/number not zero padded on right\n", GetEcuId(EcuIndex) );
			}
		}
		else
		{
			buffer[ByteIndex] = ' ';    // replace zero with space for printing to log file
		}
	}

	// Check for invalid delimiter
	if ( buffer[ByteIndex++] != '-' )
	{
		if ( gModelYear >= 2010 )
		{
			LogPrint ( "FAILURE: ECU %X  Invalid ECU Name delimiter character (byte %d)\n",
			           GetEcuId(EcuIndex),
			           ByteIndex );
			bTestFailed = TRUE;
		}
		else
		{
			LogPrint ( "WARNING: ECU %X  Invalid ECU Name delimiter character (byte %d)\n",
			           GetEcuId(EcuIndex),
			           ByteIndex );
		}
	}

	// Check all ECU Text Name characters for validity
	for ( ; ByteIndex < 20; ByteIndex++ )
	{
		if ( buffer[ByteIndex] == 0x00 )
		{
			break;
		}

		if ( !( buffer[ByteIndex] >= '1' && buffer[ByteIndex] <= '9' ) &&
		     !( buffer[ByteIndex] >= 'A' && buffer[ByteIndex] <= 'Z' ) &&
		     !( buffer[ByteIndex] >= 'a' && buffer[ByteIndex] <= 'z' ) &&
		     buffer[ByteIndex] != '+' && buffer[ByteIndex] != '/' )
		{
			if ( gModelYear >= 2010 )
			{
				LogPrint ( "FAILURE: ECU %X  Invalid ECU text name character (byte %d)\n",
				           GetEcuId(EcuIndex),
				           ByteIndex );
				bTestFailed = TRUE;
			}
			else
			{
				LogPrint ( "WARNING: ECU %X  Invalid ECU text name character (byte %d)\n",
				           GetEcuId(EcuIndex),
				           ByteIndex );
			}
		}
	}

	// if last character was a number, don't use it in ECU Text Name search
	if ( buffer[ByteIndex-1] >= '1' && buffer[ByteIndex-1] <= '9' )
	{
		// comparison size is buffer size less the Acronym, the Delimiter and the Text Name number
		CompSize = ByteIndex - 6;
		// save the number for Acronym/Text Name verification
		TextNameNumber = buffer[ByteIndex-1];
	}
	else
	{
		// comparison size is buffer size less the Acronym and the Delimiter
		CompSize = ByteIndex - 5;
	}

	// Check that ECU Text Name is approved
	for ( TableIndex = 0; TableIndex < TABLE_SIZE; TableIndex++ )
	{
		// if found match
		if ( strncmp ( &buffer[5], szECUTextName[TableIndex], CompSize ) == 0 )
		{
			break;
		}
	}

	if ( TableIndex == TABLE_SIZE )
	{
		if ( gModelYear >= 2010 )
		{
			LogPrint ( "FAILURE: ECU %X  Not an approved ECU text name\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
		else
		{
			LogPrint ( "WARNING: ECU %X  Not an approved ECU text name\n", GetEcuId(EcuIndex) );
		}
	}

	// Check for zero padding of ECU Text Name
	for ( ; ByteIndex < 20; ByteIndex++ )
	{
		if ( buffer[ByteIndex] != 0 )
		{
			if ( gModelYear >= 2010 )
			{
				LogPrint ( "FAILURE: ECU %X  ECU text name not zero padded on right\n", GetEcuId(EcuIndex) );
				bTestFailed = TRUE;
			}
			else
			{
				LogPrint ( "WARNING: ECU %X  ECU text name not zero padded on right\n", GetEcuId(EcuIndex) );
			}
		}
	}

	// Check for ECU Acronym and Text Name match
	if ( AcronymTableIndex != TableIndex ||
	     AcronymNumber != TextNameNumber )
	{
		if ( gModelYear >= 2010 )
		{
			LogPrint ( "FAILURE: ECU %X  ECU acronym and text name do not match\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
		else
		{
			LogPrint ( "WARNING: ECU %X  ECU acronym and text name do not match\n", GetEcuId(EcuIndex) );
		}
	}

	// insert NULL terminator at end of string
	buffer[ByteIndex] = 0;

	// Print ECU Name to Log file
	LogPrint ( "INFORMATION: ECU %X  NAME: %s\n", GetEcuId(EcuIndex), buffer );


	if ( bTestFailed == TRUE )
	{
		return FAIL;
	}
	else
	{
		return PASS;
	}
}


/******************************************************************************
**
**	Function:	VerifyINF8Data
**
**	Purpose:	Purpose of this function is to verify the INF 8 data
**				for correct format.  In the event the format fails
**				defined criteria, an error is returned.
**
*******************************************************************************
**
**	DATE		MODIFICATION
**	07/07/07	Isolated INF 8 verification logic
**
******************************************************************************/
STATUS VerifyINF8Data ( unsigned long  EcuIndex )
{
	// Compression Ignition vehicles MY 2010 and later should not support INF8
	if ( gModelYear >= 2010 && gOBDDieselFlag == TRUE )
	{
		LogPrint ("FAILURE: ECU %X  SID $9 INF $8 response! (Not allowed for MY 2010 and later compression ignition vehicles)\n", GetEcuId(EcuIndex) );
		ERROR_RETURN;
	}

	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{
		// If MY 2010 and later spark ignition vehicle
		if ( gModelYear >= 2010 )
		{
			// In-Use Performance Data must contain 40 bytes of data
			if ( gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 40 )
			{
				LogPrint ( "FAILURE: ECU %X  SID $9 INF $8 (IPT) Data Size = %d (Must be 40 bytes!)\n",
				           GetEcuId(EcuIndex),
				           (gOBDResponse[EcuIndex].Sid9InfSize - 0x02) );
				ERROR_RETURN;
			}

			// SID9 INF8 NODI must equal $14
			if ( gOBDResponse[EcuIndex].Sid9Inf[1] != 0x14 )
			{
				LogPrint ( "FAILURE: ECU %X  SID $9 INF $8 NODI = %d (Must be 20 ($14)\n",
				           GetEcuId(EcuIndex),
				           gOBDResponse[EcuIndex].Sid9Inf[1] );
				ERROR_RETURN;
			}
		}
		else
		{
			// In-Use Performance Data must contain 32 or 40 bytes of data
			if ( gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 32 &&
			     gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 40 )
			{
				LogPrint ( "FAILURE: ECU %X  SID $9 INF $8 (IPT) Data Size = %d (Must be 32 or 40 bytes!)\n",
				           GetEcuId(EcuIndex),
				           (gOBDResponse[EcuIndex].Sid9InfSize - 0x02) );
				ERROR_RETURN;
			}

			// SID9 INF8 NODI must equal $10 or $14
			if ( gOBDResponse[EcuIndex].Sid9Inf[1] != 0x10 &&
			     gOBDResponse[EcuIndex].Sid9Inf[1] != 0x14 )
			{
				LogPrint ( "FAILURE: ECU %X  SID $9 INF $8 NODI = %d (Must be 16 ($10) or 20 ($14)\n",
				           GetEcuId(EcuIndex),
				           gOBDResponse[EcuIndex].Sid9Inf[1] );
				ERROR_RETURN;
			}
		}
	}
	else
	{
		/* For non-CAN, calculate the number of responses.  Expected is
		** 0x08 records.  Each response from controller held in
		** data structure SID9.
		*/
		if (gOBDResponse[EcuIndex].Sid9InfSize / sizeof(SID9) != 0x08)
		{
			LogPrint ("FAILURE: ECU %X  SID $9 INF $8 (IPT) Data Size Error, Must be 32 bytes!\n", GetEcuId(EcuIndex) );
			ERROR_RETURN;
		}
	}

	return PASS;
}


/******************************************************************************
**
**	Function:	VerifyINFBData
**
**	Purpose:	Purpose of this function is to verify the INF B data
**				for correct format.  In the event the format fails
**				defined criteria, an error is returned.
**
*******************************************************************************
**
**	DATE		MODIFICATION
**	07/07/07	Isolated INF B verification logic
**
******************************************************************************/
STATUS VerifyINFBData ( unsigned long  EcuIndex )
{
	// Spark Ignition vehicles MY 2010 and later should not support INFB
	if ( gModelYear >= 2010 && gOBDDieselFlag == FALSE )
	{
		LogPrint ( "FAILURE: ECU %X  SID $9 INF $B response! (Not allowed for MY 2010 and later spark ignition vehicles)\n", GetEcuId(EcuIndex) );
		ERROR_RETURN;
	}

	/* 32 bytes of data returned */
	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{
		// In-Use Performance Data must contain 32 bytes of data
		if ( gOBDResponse[EcuIndex].Sid9InfSize - 0x02 != 32 )
		{
			LogPrint ( "FAILURE: ECU %X  SID $9 INF $B (IPT) Data Size Error = %d (Must be 32 bytes!)\n",
			           GetEcuId(EcuIndex),
			           (gOBDResponse[EcuIndex].Sid9InfSize - 0x02) );
			ERROR_RETURN;
		}

		// SID9 INFB NODI must equal $10
		if ( gOBDResponse[EcuIndex].Sid9Inf[1] != 0x10 )
		{
			LogPrint ( "FAILURE: ECU %X  SID $9 INF $B NODI = %d (Must be 16 ($10)\n",
			           GetEcuId(EcuIndex),
			           gOBDResponse[EcuIndex].Sid9Inf[1] );
			ERROR_RETURN;
		}
	}

	return PASS;
}


//*****************************************************************************
//  Function:	VerifySid9PidSupportData
//
//	Purpose:	Verify each controller supports at a minimum one PID.
//              Any ECU that responds that does not support at least
//              one PID is flagged as an error.
//
//*****************************************************************************
int VerifySid9PidSupportData (void)
{
	int             bReturn = PASS;
	int             bEcuResult;
	unsigned long   EcuIndex;
	unsigned long   Index;

	/* For each ECU */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		bEcuResult = FAIL;
		for (Index = 0; Index < gOBDResponse[EcuIndex].Sid9InfSupportSize; Index++)
		{
			/* If MID is supported, keep looking */
			if ( ( gOBDResponse[EcuIndex].Sid9InfSupport[Index].IDBits[0]		||
			       gOBDResponse[EcuIndex].Sid9InfSupport[Index].IDBits[1]		||
			       gOBDResponse[EcuIndex].Sid9InfSupport[Index].IDBits[2]		||
			     ( gOBDResponse[EcuIndex].Sid9InfSupport[Index].IDBits[3] & 0xFE ) ) != 0x00)
			{
				bEcuResult = PASS;
				break;
			}
		}

		if ((bEcuResult == FAIL) && (gOBDResponse[EcuIndex].Sid9InfSupportSize > 0))
		{
			LogPrint ("INFORMATION: ECU %X  SID $9 invalid PID supported PIDs", GetEcuId(EcuIndex) );
			bReturn = FAIL;
		}
	}

	return bReturn;
}

//*****************************************************************************
//
//	Function:	IsSid9InfSuported
//
//	Purpose:	Determine if SID 9 INF is supported on specific ECU.
//              Need to have called RequestSID9SupportData() previously.
//              If EcuIndex < 0 then check all ECUs.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	02/10/05	Created common function for this logic.
//
//*****************************************************************************
unsigned int IsSid9InfSupported (unsigned int EcuIndex, unsigned int InfIndex)
{
	unsigned int index0;
	unsigned int index1;
	unsigned int index2;
	unsigned int mask;

	if (InfIndex == 0)
		return TRUE;            // all modules must support SID 09 INF 00

	InfIndex--;

	index1 =  InfIndex >> 5;
	index2 = (InfIndex >> 3) & 0x03;
	mask   = 0x80 >> (InfIndex & 0x07);

	if ((signed int)EcuIndex < 0)
	{
		for (index0 = 0; index0 < gUserNumEcus; index0++)
		{
			if (gOBDResponse[index0].Sid9InfSupport[index1].IDBits[index2] & mask)
				return TRUE;
		}
	}
	else
	{
		if (gOBDResponse[EcuIndex].Sid9InfSupport[index1].IDBits[index2] & mask)
			return TRUE;
	}

	return FALSE;
}
