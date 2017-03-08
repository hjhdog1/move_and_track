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
**    DATE		MODIFICATION
**    06/14/04  Removed Mode $01 PID $01 / PID $41 comparison.  Implement
**              PID $01 check logic defined in specification
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

#define MAX_PIDS 0x100
STATUS VerifyM01P01 (SID1 *pSid1, unsigned long SidIndex, unsigned long EcuIndex);
STATUS GetPid4FArray (void);
unsigned char Pid4F[OBD_MAX_ECUS][4];

SID1 Sid1Pid1[OBD_MAX_ECUS];    // capture the response from SID01 PID01 response.


/*
*******************************************************************************
** VerifyDiagnosticSupportAndData -
** Function to verify SID1 diagnostic support and data
*******************************************************************************
*/
STATUS VerifyDiagnosticSupportAndData(void)
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	SID_REQ SidReq;
	SID1 *pSid1;
	unsigned long SidIndex;
	long temp_data_long;
	float temp_data_float;
	unsigned int BSIndex;
	unsigned char fPid13Supported = FALSE;	/* Added per V11.7 */
	unsigned char fPid1DSupported = FALSE;	/* Added per V11.7 */
	unsigned char fPid4FSupported = FALSE;
	unsigned char fReqPidNotSupported = FALSE;  // set if a required PID is not supported
	unsigned char fPidSupported[MAX_PIDS];      // an array of PIDs (TRUE if PID is supported)
	unsigned long ErrCntTmp;                    // stores the error count when starting this function
	unsigned long ErrFlgTmp;                    // stores the state of the error flags when starting this function


	// Clear Error Count for ERROR_RETURN
	ErrorCount();

	/* Read SID 1 PID support PIDs */
	if (RequestSID1SupportData () != PASS)
	{
		ERROR_RETURN;
	}

	/* Per J1699 rev 11.5 TC# 5.10.5 - Verify ECU did not drop out.	*/
	if (VerifyLinkActive() != PASS)
	{
		ERROR_RETURN;
	}

	/* Determine size of PIDs $06, $07, $08, $09 */
	if (DetermineVariablePidSize () != PASS)
	{
		ERROR_RETURN;
	}

	if (GetPid4FArray() != PASS)
	{
		ERROR_RETURN;
	}

	/* For each PID group */
	for (IdIndex = 0x01; IdIndex < MAX_PIDS; IdIndex++)
	{
		/* clear PID supported indicator */
		fPidSupported[IdIndex] = FALSE;

		/* skip PID supported PIDs */
		if (IdIndex == 0x20 || IdIndex == 0x40 || IdIndex == 0x60 || IdIndex == 0x80 ||
			IdIndex == 0xA0 || IdIndex == 0xC0 || IdIndex == 0xE0)
			continue;


		if (IsSid1PidSupported (-1, IdIndex) == FALSE)
		{
			continue;
		}

		SidReq.SID = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = (unsigned char)IdIndex;
		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			/* There must be a response for ISO15765 protocol */
			if (gOBDList[gOBDListIndex].Protocol == ISO15765)
			{
				LogPrint( "FAILURE: SID $1 PID $%02X request failed\n", IdIndex );
				ERROR_RETURN;
				continue;
			}
		}

		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid1PidSize != 0)
			{
				break;
			}
		}

		if (EcuIndex >= gOBDNumEcus)
		{
			LogPrint( "FAILURE: No SID $1 PID $%02X data\n", IdIndex );
			ERROR_RETURN;
			continue;
		}

		// Prepare Error Flags and Error Count for ERROR_RETURN
		ErrFlgTmp = ErrorFlags (ER_BYPASS_USER_PROMPT | ER_CONTINUE | ER_DONT_CLEAR_ERROR_COUNT);
		ErrCntTmp = ErrorCount();

		/* Verify that all SID 1 PID data is valid */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			/* Set ECU dependent PID flags */
			fPid13Supported = IsSid1PidSupported (EcuIndex, 0x13);
			fPid1DSupported = IsSid1PidSupported (EcuIndex, 0x1D);
			fPid4FSupported = IsSid1PidSupported (EcuIndex, 0x4F);

			/* If PID is supported, check it */
			if (IsSid1PidSupported (EcuIndex, IdIndex) == TRUE)
			{
				/* save indication that PID is supported */
				fPidSupported[IdIndex] = TRUE;

				/* Check the data to see if it is valid */
				pSid1 = (SID1 *)&gOBDResponse[EcuIndex].Sid1Pid[0];
				for (SidIndex = 0; SidIndex < (gOBDResponse[EcuIndex].Sid1PidSize / sizeof(SID1)); SidIndex++)
				{
					/* Check various PID values for validity based on vehicle state */
					switch(pSid1[SidIndex].PID)
					{
						case 0x01:
						{
							// Capture the response from PID $01!
							// for use with SID 01 PID 41 and
							// SID $06 MID verification logic
							memcpy( &Sid1Pid1[EcuIndex], &pSid1[SidIndex], sizeof( SID1 ) );

							if ( VerifyM01P01( pSid1, SidIndex, EcuIndex ) != PASS )
							{
								ERROR_RETURN;
							}
						}
						break;
						case 0x02:
						{
							temp_data_long = (pSid1[SidIndex].Data[0] * 256) +
							                 pSid1[SidIndex].Data[1];
							if (gOBDEngineWarm == FALSE)  // test 5.6 & 5.10
							{
								if ((pSid1[SidIndex].Data[0] != 0x00) ||
									(pSid1[SidIndex].Data[1] != 0x00))
								{
									LogPrint( "FAILURE: ECU %X  DTCFRZF = $%04X, Freeze frames available\n",
									          GetEcuId(EcuIndex),
									          temp_data_long );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x03:
						{
							LogPrint( "INFORMATION: FUELSYS1 = $%02X  FUELSYS2 = $%02X\n",
							          pSid1[SidIndex].Data[0],
							          pSid1[SidIndex].Data[1] );
							if (((pSid1[SidIndex].Data[0] & 0xE0) != 0x00) ||
								((pSid1[SidIndex].Data[1] & 0xE0) != 0x00))
							{
								LogPrint( "FAILURE: ECU %X  FUELSYS  Reserved bits set\n",
								          GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x04:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: LOAD_PCT = %d %%\n", temp_data_long);
							if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
							{
								if (temp_data_long > 60)
								{
									/* Load should be 60% or less with engine running */
									LogPrint( "FAILURE: ECU %X  LOAD_PCT > 60 %%\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else  // test 5.6
							{
								if (temp_data_long != 0)
								{
									/* There should be no load with the engine OFF */
									LogPrint( "FAILURE: ECU %X  LOAD_PCT > 0 %%\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x05:
						{
							temp_data_long = pSid1[SidIndex].Data[0] - 40;
							LogPrint("INFORMATION: ECT = %d C\n", temp_data_long);
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								if ((temp_data_long < 65) || (temp_data_long > 120))
								{
									LogPrint("FAILURE: ECU %X  ECT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else  // test 5.6 & 5.10
							{
								if ((temp_data_long < -20) || (temp_data_long > 120))
								{
									LogPrint( "FAILURE: ECU %X  ECT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x06:
						{
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								temp_data_float = (float)(pSid1[SidIndex].Data[0]) * (float)(100.0/128.0) - (float)100.0;
								LogPrint("INFORMATION: SHRTFT1 = %f %%\n", temp_data_float);
								if (temp_data_float < -50.0 || temp_data_float > 50.0)
								{
									LogPrint ( "FAILURE: ECU %X  SHRTFT1 out of range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}

								if (gSid1VariablePidSize == 2)
								{
									temp_data_float = (float)(pSid1[SidIndex].Data[1]) * (float)(100.0/128.0) - (float)100.0;
									LogPrint("INFORMATION: SHRTFT3 = %f %%\n", temp_data_float);
									if (temp_data_float < -50.0 || temp_data_float > 50.0)
									{
										LogPrint ( "FAILURE: ECU %X  SHRTFT3 out of range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}
						}
						break;
						case 0x07:
						{
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								temp_data_float = (float)(pSid1[SidIndex].Data[0]) * (float)(100.0/128.0) - (float)100.0;
								LogPrint("INFORMATION: LONGFT1 = %f %%\n", temp_data_float);
								if (temp_data_float < -50.0 || temp_data_float > 50.0)
								{
									LogPrint ( "FAILURE: ECU %X  LONGFT1 out of range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}

								if (gSid1VariablePidSize == 2)
								{
									temp_data_float = (float)(pSid1[SidIndex].Data[1]) * (float)(100.0/128.0) - (float)100.0;
									LogPrint("INFORMATION: LONGFT3 = %f %%\n", temp_data_float);
									if (temp_data_float < -50.0 || temp_data_float > 50.0)
									{
										LogPrint (" FAILURE: ECU %X  LONGFT3 out of range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}
						}
						break;
						case 0x08:
						{
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								temp_data_float = (float)(pSid1[SidIndex].Data[0]) * (float)(100.0/128.0) - (float)100.0;
								LogPrint("INFORMATION: SHRTFT2 = %f %%\n", temp_data_float);
								if (temp_data_float < -50.0 || temp_data_float > 50.0)
								{
									LogPrint ( "FAILURE: ECU %X  SHRTFT2 out of range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}

								if (gSid1VariablePidSize == 2)
								{
									temp_data_float = (float)(pSid1[SidIndex].Data[1]) * (float)(100.0/128.0) - (float)100.0;
									LogPrint("INFORMATION: SHRTFT4 = %f %%\n", temp_data_float);
									if (temp_data_float < -50.0 || temp_data_float > 50.0)
									{
										LogPrint ( "FAILURE: ECU %X  SHRTFT4 out of range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}
						}
						break;
						case 0x09:
						{
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								temp_data_float = (float)(pSid1[SidIndex].Data[0]) * (float)(100.0/128.0) - (float)100.0;
								LogPrint("INFORMATION: LONGFT2 = %f %%\n", temp_data_float);
								if (temp_data_float < -50.0 || temp_data_float > 50.0)
								{
									LogPrint ("FAILURE: ECU %X  LONGFT2 out of range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}

								if (gSid1VariablePidSize == 2)
								{
									temp_data_float = (float)(pSid1[SidIndex].Data[1]) * (float)(100.0/128.0) - (float)100.0;
									LogPrint("INFORMATION: LONGFT4 = %f %%\n", temp_data_float);
									if (temp_data_float < -50.0 || temp_data_float > 50.0)
									{
										LogPrint ( "FAILURE: ECU %X  LONGFT4 out of range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}
						}
						break;
						case 0x0A:
						{
							temp_data_long = pSid1[SidIndex].Data[0] * 3;
							LogPrint ("INFORMATION: FRP = %u kPa\n", temp_data_long);
							if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
							{
								if (temp_data_long == 0)
								{
									LogPrint( "FAILURE: ECU %X  FRP must be greater than 0\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x0B:
						{
							temp_data_float = (float)(pSid1[SidIndex].Data[0]);
							if ( fPid4FSupported == TRUE && Pid4F[EcuIndex][3]!=0 )
							{
								temp_data_float = temp_data_float * ((float)(Pid4F[EcuIndex][3]*10)/255);
							}
							LogPrint ("INFORMATION: MAP = %f kPa\n", temp_data_float);
							if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
							{
								if (temp_data_float == 0)
								{
									LogPrint( "FAILURE: ECU %X  MAP must be greater than 0\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x0C:
						{
							temp_data_long = (((pSid1[SidIndex].Data[0] * 256) + pSid1[SidIndex].Data[1]) / 4);

							LogPrint("INFORMATION: RPM = %d rpm\n", temp_data_long);
							if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
							{
								/* Per J1699 rev 11.6 - table 41 */
								if ( ( temp_data_long > 2000 ) || ( temp_data_long < 300 ) )
								{
									/* Idle RPM is outside the reasonable range */
									LogPrint( "FAILURE: ECU %X  RPM exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else  // test 5.6
							{
								if (temp_data_long != 0)
								{
									/* There should be no RPM with the engine OFF */
									LogPrint( "FAILURE: ECU %X  RPM > 0 rpm\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x0D:
						{
							LogPrint("INFORMATION: VSS = %d km/h\n", pSid1[SidIndex].Data[0]);
							if (pSid1[SidIndex].Data[0] != 0x00)
							{
								/* There should be no vehicle speed when not moving */
								LogPrint( "FAILURE: ECU %X  VSS > 0 km/h\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x0E:
						{
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								temp_data_long = (pSid1[SidIndex].Data[0] / 2) - 64;
								LogPrint("INFORMATION: SPARKADV = %d deg\n", temp_data_long);
								if ((temp_data_long < -25) || (temp_data_long > 40))
								{
									LogPrint( "FAILURE: ECU %X  SPARKADV exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x0F:
						{
							temp_data_long = pSid1[SidIndex].Data[0] - 40;
							LogPrint("INFORMATION: IAT = %d C\n", temp_data_long);
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								if ( (temp_data_long < 0) || (temp_data_long > 120) )
								{
									LogPrint( "FAILURE: ECU %X  IAT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else  // test 5.6 & 5.10
							{
								if ( (temp_data_long < -20) || (temp_data_long > 120) )
								{
									LogPrint( "FAILURE: ECU %X  IAT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x10:
						{
							temp_data_float = (float)((pSid1[SidIndex].Data[0] * 256) +
							pSid1[SidIndex].Data[1])/(float)100.0;
							LogPrint("INFORMATION: MAF = %f gm/s\n", temp_data_float);

							if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
							{
								if (temp_data_float == 0.0)
								{
									/* MAF should not be zero with the engine running */
									LogPrint( "FAILURE: ECU %X  MAF = 0 gm/s\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else  // test 5.6
							{
								/*
								** J1699 version 11.6 table 23, engine off update.
								*/
								if (temp_data_float > 5.0)
								{
									/* MAF should be zero with the engine OFF */
									LogPrint( "FAILURE: ECU %X  MAF > 5 gm/s\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x11:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: TP = %d %%\n", temp_data_long);
							if (gOBDDieselFlag == 0)
							{
								// non-diesel
								if (temp_data_long > 40)
								{
									/*
									** Throttle position should be
									** 40% or less when not driving
									*/
									LogPrint("FAILURE: ECU %X  TP > 40 %%\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else
							{
								// diesel
								if (temp_data_long > 100)
								{
									/*
									** Throttle position should be
									** 100% or less when not driving
									*/
									LogPrint("FAILURE: ECU %X  TP > 100 %%\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x1C:
						{
							/* Make sure value is in the valid range before the lookup */
							if (pSid1[SidIndex].Data[0] > 13)
							{
								pSid1[SidIndex].Data[0] = 0;
							}

							/* Updated the following supported IDs per J1699 V11.5 */
							/* Vehicle should support OBD-II */
							LogPrint("INFORMATION: OBD_TYPE = %s\n", OBD_TYPE[pSid1[SidIndex].Data[0]]);
							if (pSid1[SidIndex].Data[0] != 0x01	&&     /* CARB OBDII */
								pSid1[SidIndex].Data[0] != 0x03	&&     /* OBDI and OBDII */
								pSid1[SidIndex].Data[0] != 0x07	&&     /* EOBD and OBDII */
								pSid1[SidIndex].Data[0] != 0x09	&&     /* EOBD, OBD and OBDII */
								pSid1[SidIndex].Data[0] != 0x0B	&&     /* JOBD and OBD II */
								pSid1[SidIndex].Data[0] != 0x0D)       /* JOBD, EOBD, and OBD II */
							{
								LogPrint("FAILURE: ECU %X  Not an OBD-II ECU\n");
								ERROR_RETURN;
							}
						}
						break;
						case 0x13:		/* J1699 V11.5, Added per table 41 */
						case 0x1D:
						{
							unsigned short O2Bit;
							unsigned short O2Count;

							/* Identify support for PID 0x13 / 0x1D */
							if ( pSid1[SidIndex].PID == 0x13 )
							{
								fPid13Supported = TRUE;
							}
							else
							{
								fPid1DSupported = TRUE;
							}

							/* Evaluate for dual PID / Spark engine support */
							if ( ( fPid13Supported == TRUE ) &&
								 ( fPid1DSupported == TRUE ) )
							{
								LogPrint( "FAILURE: ECU %X  PID 13 & 1D both indicated as supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* Count the number of O2 sensors */
							for (O2Bit = 0x01, O2Count = 0; O2Bit != 0x100; O2Bit = O2Bit << 1)
							{
								if (pSid1[SidIndex].Data[0] & O2Bit)
								{
									O2Count++;
								}
							}
							LogPrint("INFORMATION: %d O2 Sensors\n", O2Count);

							/* At least 2 O2 sensors required for spark ignition enges */
							if ( gOBDDieselFlag == FALSE &&
								 O2Count < 2 )
							{
								LogPrint( "FAILURE: ECU %X  O2S < 2\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x1F:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 256) +
							pSid1[SidIndex].Data[1]);
							LogPrint("INFORMATION: RUNTM = %d sec\n", temp_data_long);
							if (gOBDEngineRunning == TRUE)
							{
								if (gOBDEngineWarm == TRUE)  // test 10.12
								{
									if (temp_data_long <= 300)
									{
										/* Run time should greater than 300 seconds */
										LogPrint( "FAILURE: ECU %X  RUNTM < 300 sec\n", GetEcuId(EcuIndex) , temp_data_long);
										ERROR_RETURN;
									}
								}
								else  // test 5.10
								{
									if (temp_data_long == 0)
									{
										/* Run time should not be zero if engine is running */
										LogPrint("FAILURE: ECU %X  RUNTM = 0 sec\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}
							else  // test 5.6
							{
								if (temp_data_long != 0)
								{
									/* Run time should be zero if engine is OFF */
									LogPrint("FAILURE: ECU %X  RUNTM > 0 sec\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x21:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 256) +
							pSid1[SidIndex].Data[1]);
							LogPrint("INFORMATION: MIL_DIST = %d km\n", temp_data_long);
							if (temp_data_long != 0x00)
							{
								if ( (gOBDEngineWarm == TRUE)     ||            // test 10.12
									 (gOBDEngineRunning == FALSE) ||            // test 5.6
									((gOBDEngineRunning == TRUE)  &&            // test 5.10
									 (gOBDEngineWarm == FALSE)    &&
									 (gOBDResponse[EcuIndex].Sid4Size > 0) &&
									 (gOBDResponse[EcuIndex].Sid4[0] == 0x44)))
								{
									LogPrint("FAILURE: ECU %X  MIL_DIST > 0 after clearing DTCs\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x22:
						{
							temp_data_float = (float)((pSid1[SidIndex].Data[0] * 256) +
							pSid1[SidIndex].Data[1]) * (float)0.079;
							LogPrint("INFORMATION: FRP (relative to manifold) = %f kPa\n"   , temp_data_float);

							/* 06/16/04; Correct logic error.  Previously following check compared
							**           temp_data_long but should compare to actual calculation
							**           stored to temp_data_float.
							*/
							if ( gOBDEngineRunning == TRUE &&  // test 5.10 &10.12
							     temp_data_float == 0)
							{
								LogPrint( "FAILURE: ECU %X  FRP = 0 kPa\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x23:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 256) +
							pSid1[SidIndex].Data[1]) * 10;
							LogPrint("INFORMATION: FRP = %ld kPa\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE &&  // test 5.10 & 10.12
							     temp_data_long == 0)
							{
								LogPrint("FAILURE: ECU %X  FRP = 0 kPa\n");
								ERROR_RETURN;
							}
						}
						break;
						case 0x24:
						case 0x25:
						case 0x26:
						case 0x27:
						case 0x28:
						case 0x29:
						case 0x2A:
						case 0x2B:
						{
							switch (pSid1[SidIndex].PID)
							{
								case 0x24:
									BSIndex = 0x11;
								break;
								case 0x25:
									BSIndex = 0x12;
								break;
								case 0x26:
									BSIndex = fPid13Supported ? 0x13 : 0x21;
								break;
								case 0x27:
									BSIndex = fPid13Supported ? 0x14 : 0x22;
								break;
								case 0x28:
									BSIndex = fPid13Supported ? 0x21 : 0x31;
								break;
								case 0x29:
									BSIndex = fPid13Supported ? 0x22 : 0x32;
								break;
								case 0x2A:
									BSIndex = fPid13Supported ? 0x23 : 0x41;
								break;
								case 0x2B:
									BSIndex = fPid13Supported ? 0x24 : 0x42;
								break;
							}

							temp_data_float = (float)(((unsigned long)(pSid1[SidIndex].Data[0] << 8) |
							pSid1[SidIndex].Data[1]));
							if ( fPid4FSupported == FALSE || Pid4F[EcuIndex][0] == 0 )
							{
								temp_data_float = temp_data_float * (float)0.0000305;
							}
							else
							{
								temp_data_float = temp_data_float * ((float)(Pid4F[EcuIndex][0])/65535);
							}

							LogPrint("INFORMATION: EQ_RAT%x = % f %%\n", BSIndex, temp_data_float);

							temp_data_float = (float)(((unsigned long)(pSid1[SidIndex].Data[2] << 8) |
							pSid1[SidIndex].Data[3]));
							if ( fPid4FSupported == FALSE || Pid4F[EcuIndex][1] == 0 )
							{
								temp_data_float = temp_data_float * (float)0.000122;
							}
							else
							{
								temp_data_float = temp_data_float * ((float)(Pid4F[EcuIndex][1])/65535);
							}

							LogPrint("INFORMATION: O2S%x = %f V\n", BSIndex, temp_data_float);
						}
						break;
						case 0x2C:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: EGR_PCT = %d %%\n", temp_data_long);
							if ( gOBDDieselFlag == TRUE ||  // compression ignition or
							     gOBDHybridFlag == TRUE )   // hybrid engines
							{
								if ( gOBDEngineRunning == FALSE )  // test 5.6
								{
									if (temp_data_long > 10)
									{
										LogPrint( "FAILURE: ECU %X  EGR_PCT > 10 %\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
								else  // test 5.10 & 10.12
								{
									if (temp_data_long > 100)
									{
										LogPrint( "FAILURE: ECU %X  EGR_PCT > 100 %\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}
							else  // spark ignition engine
							{
								if (temp_data_long > 10)
								{
									LogPrint( "FAILURE: ECU %X  EGR_PCT > 10 %\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x2F:
						{
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								temp_data_float = ((float)pSid1[SidIndex].Data[0]) * (float)(100. / 255.);
								LogPrint("INFORMATION: FLI = %f\n", temp_data_float);
								if (temp_data_float < 1.0 || temp_data_float > 100.0)
								{
									LogPrint( "FAILURE: ECU %X  FLI should be between 1 and 100 %%\n", GetEcuId(EcuIndex), temp_data_float);
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x30:
						{
							temp_data_long = pSid1[SidIndex].Data[0];
							LogPrint("INFORMATION: WARM_UPS = %d\n", temp_data_long);

							/* 06/04/04 - Do not check during Drive cycle */
							/* 08/08/04 - Update per spec 11.7;evaluate
							**            to 0 or 1
							*/
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								if (temp_data_long > 4)
								{
									LogPrint( "FAILURE: ECU %X  WARM_UPS > 4\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else if ( (gOBDEngineRunning == FALSE) ||       // test 5.6
									 ((gOBDEngineRunning == TRUE)  &&       // test 5.10
									  (gOBDResponse[EcuIndex].Sid4Size > 0) &&
									  (gOBDResponse[EcuIndex].Sid4[0] == 0x44)))
							{
								if (temp_data_long != 0)
								{
									LogPrint("FAILURE: ECU %X  WARM_UPS > 0\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x31:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 256) +
							                  pSid1[SidIndex].Data[1]);
							LogPrint("INFORMATION: CLR_DIST = %d km\n", temp_data_long);
							if (gOBDEngineWarm == TRUE)                     // test 10.12
							{
								if (temp_data_long >= 50)
								{
									LogPrint("FAILURE: ECU %X  CLR_DIST >= 50km after CARB drive cycle\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else if ( (gOBDEngineRunning == FALSE) ||       // test 5.6
									 ((gOBDEngineRunning == TRUE)  &&       // test 5.10
									  (gOBDResponse[EcuIndex].Sid4Size > 0) &&
									  (gOBDResponse[EcuIndex].Sid4[0] == 0x44)))
							{
								if (temp_data_long != 0)
								{
									LogPrint("FAILURE: ECU %X  CLR_DIST > 0 km after clearing DTCs\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x33:
						{
							LogPrint("INFORMATION: BARO = %d kPa\n", pSid1[SidIndex].Data[0]);
							if ((pSid1[SidIndex].Data[0] < 71) || (pSid1[SidIndex].Data[0] > 110))
							{
								LogPrint("FAILURE: ECU %X  BARO exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x34:
						case 0x35:
						case 0x36:
						case 0x37:
						case 0x38:
						case 0x39:
						case 0x3A:
						case 0x3B:
						{
							switch (pSid1[SidIndex].PID)
							{
								case 0x34:
									BSIndex = 0x11;
								break;
								case 0x35:
									BSIndex = 0x12;
								break;
								case 0x36:
									BSIndex = fPid13Supported ? 0x13 : 0x21;
								break;
								case 0x37:
									BSIndex = fPid13Supported ? 0x14 : 0x22;
								break;
								case 0x38:
									BSIndex = fPid13Supported ? 0x21 : 0x31;
								break;
								case 0x39:
									BSIndex = fPid13Supported ? 0x22 : 0x32;
								break;
								case 0x3A:
									BSIndex = fPid13Supported ? 0x23 : 0x41;
								break;
								case 0x3B:
									BSIndex = fPid13Supported ? 0x24 : 0x42;
								break;
							}

							// scale Equivalence Ratio
							temp_data_float = (float)(((unsigned long)(pSid1[SidIndex].Data[0] << 8) |
							                                           pSid1[SidIndex].Data[1]));
							if ( fPid4FSupported == FALSE || Pid4F[EcuIndex][0] == 0 )
							{
								temp_data_float = temp_data_float * (float)0.0000305;
							}
							else
							{
								temp_data_float = temp_data_float * ((float)(Pid4F[EcuIndex][0])/65535);
							}

							LogPrint("INFORMATION: EQ_RAT%x = %f %%\n", BSIndex, temp_data_float);

							// offset and scale Oxygen Sensor Current
							temp_data_float = (float)((((signed short)(pSid1[SidIndex].Data[2]) << 8) |
							                                           pSid1[SidIndex].Data[3]) - 0x8000);
							if ( fPid4FSupported == FALSE || Pid4F[EcuIndex][2] == 0 )
							{
								temp_data_float = temp_data_float * (float)0.00390625;
							}
							else
							{
								temp_data_float = temp_data_float * ((float)(Pid4F[EcuIndex][2])/32768);
							}

							LogPrint("INFORMATION: O2S%x = %f mA\n", BSIndex, temp_data_float);
						}
						break;
						case 0x41:
						{
							/* Diesel Ignition bit */
							/* only check if MY 2010 and beyond and ECU does not only supports CCM requirements (SID 1 PID 1 Data B bit 2==1) */
							if ( gModelYear >= 2010 && (Sid1Pid1[EcuIndex].Data[1] & 0x04) == 0 )
							{
								if ( (pSid1[SidIndex].Data[1] & 0x08) == 0 &&
								     gOBDDieselFlag == 1 )
								{
									LogPrint("FAILURE: ECU %X  SID $1 PID $41 Data B bit 3 clear (Spark Ignition) does not match user input (Diesel)\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
								else if ( (pSid1[SidIndex].Data[1] & 0x08) == 0x08 &&
								          gOBDDieselFlag == 0 )
								{
									LogPrint("FAILURE: ECU %X  SID $1 PID $41 Data B bit 3 set (Compression Ignition) does not match user input (Non Diesel)\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							/* Catalyst monitor */
							if  ( ( ( Sid1Pid1[EcuIndex].Data[2] & 0x01 ) == 0x00 ) &&
							        ( pSid1[SidIndex].Data[3]    & 0x01 ) == 0x01 )
							{
								LogPrint("FAILURE: ECU %X  Cat Mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* Heated CAT Mon   */
							if  ( ( ( Sid1Pid1[EcuIndex].Data[2] & 0x02 ) == 0x00 ) &&
							        ( pSid1[SidIndex].Data[3]    & 0x02 ) == 0x02 )
							{
								LogPrint("FAILURE: ECU %X  Heated Cat Mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* EVAP sys         */
							if  ( ( ( Sid1Pid1[EcuIndex].Data[2] & 0x04 ) == 0x00 ) &&
							        ( pSid1[SidIndex].Data[3]    & 0x04 ) == 0x04 )
							{
								LogPrint("FAILURE: ECU %X  EVAP sys: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* Secondary AIR    */
							if  ( ( ( Sid1Pid1[EcuIndex].Data[2] & 0x08 ) == 0x00 ) &&
							        ( pSid1[SidIndex].Data[3]     & 0x08 ) == 0x08 )
							{
								LogPrint("FAILURE: ECU %X  Secondary AIR: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* A/C Sys mon      */
							if  ( ( ( Sid1Pid1[EcuIndex].Data[2] & 0x10 ) == 0x00 ) &&
							        ( pSid1[SidIndex].Data[3]    & 0x10 ) == 0x10 )
							{
								LogPrint("FAILURE: ECU %X  AC Sys mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* O2 sens mon      */
							if  ( ( ( Sid1Pid1[EcuIndex].Data[2] & 0x20 ) == 0x00 ) &&
							        ( pSid1[SidIndex].Data[3]    & 0x20 ) == 0x20 )
							{
								LogPrint("FAILURE: ECU %X  O2 sens mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* O2 sens htr Mon  */
							if  ( ( ( Sid1Pid1[EcuIndex].Data[2] & 0x40 ) == 0x00 ) &&
							        ( pSid1[SidIndex].Data[3]    & 0x40 ) == 0x40 )
							{
								LogPrint("FAILURE: ECU %X  O2 sens htr Mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* EGR sys mon      */
							if  ( ( ( Sid1Pid1[EcuIndex].Data[2] & 0x80 ) == 0x00 ) &&
							        ( pSid1[SidIndex].Data[3]    & 0x80 ) == 0x80 )
							{
								LogPrint("FAILURE: ECU %X  EGR sys mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x43:
						{
							temp_data_long = ( ( ( (pSid1[SidIndex].Data[0] * 256) +
							                      pSid1[SidIndex].Data[1] ) * 100 ) / 255);
							LogPrint("INFORMATION: LOAD_ABS = %d %%\n", temp_data_long);
							if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
							{
								if (temp_data_long == 0)
								{
									LogPrint("FAILURE: ECU %X  LOAD_ABS = 0%\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else  // test 5.6
							{
								if (temp_data_long != 0)
								{
									LogPrint("FAILURE: ECU %X  LOAD_ABS > 0%\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x44:
						{
							if (gOBDEngineWarm == TRUE)  // test 10.12
							{
								temp_data_float = (float)(((unsigned long)(pSid1[SidIndex].Data[0]) << 8)
								 | pSid1[SidIndex].Data[1]);
								LogPrint("INFORMATION: EQ_RAT = %f %%\n", temp_data_float);
								if ( fPid4FSupported == FALSE || Pid4F[EcuIndex][0] == 0 )
								{
									temp_data_float = temp_data_float * (float)0.0000305;

									if ( gOBDDieselFlag == FALSE )
									{
										if ( temp_data_float < 0.5 || temp_data_float > 1.5  )
										{
											LogPrint("FAILURE: ECU %X  EQ_RAT exceeded normal range\n", GetEcuId(EcuIndex) );
											ERROR_RETURN;
										}
									}
									else
									{
										if ( temp_data_float > 1.99  )
										{
											LogPrint("FAILURE: ECU %X  EQ_RAT exceeded normal range\n", GetEcuId(EcuIndex) );
											ERROR_RETURN;
										}
									}
								}
								else
								{
									temp_data_float = temp_data_float * ((float)(Pid4F[EcuIndex][0])/65535);
								}
							}
						}
						break;
						case 0x45:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: TP_R = %d%%\n", temp_data_long);
							if ((gOBDDieselFlag == FALSE) && (temp_data_long > 50) ||
								(gOBDDieselFlag == TRUE)  && (temp_data_long > 100))
							{
								LogPrint("FAILURE: ECU %X  TP_R exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x46:
						{
							temp_data_long = pSid1[SidIndex].Data[0] - 40;
							LogPrint("INFORMATION: AAT = %d C\n", temp_data_long);
							if ((temp_data_long < -20) || (temp_data_long > 85))
							{
								LogPrint("FAILURE: ECU %X  AAT exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x47:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION:TP_B = %d %%\n", temp_data_long);
							/*
							** J1699 V11.5 calls for value to be between 0 - 60 %
							** for non-diesel and 0 - 100 % for diesel
							*/
							if (gOBDDieselFlag == 0)
							{
								// non-diesel
								if ( ( temp_data_long > 60 ) || ( temp_data_long < 0 ) )
								{
									LogPrint("FAILURE: ECU %X  TP_B exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else
							{
								// diesel
								if ( ( temp_data_long > 100 ) || ( temp_data_long < 0 ) )
								{
									LogPrint("FAILURE: ECU %X  TP_B exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x48:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: TP_C = %d %%\n", temp_data_long);
							/*
							** J1699 V11.5 calls for value to be between 0 - 60 %
							** for non-diesel and 0 - 100 % for diesel
							*/
							if (gOBDDieselFlag == 0)
							{
								// non-diesel
								if ( ( temp_data_long > 60 ) || ( temp_data_long < 0 ) )
								{
									LogPrint("FAILURE: ECU %X  TP_C exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else
							{
								// diesel
								if ( ( temp_data_long > 100 ) || ( temp_data_long < 0 ) )
								{
									LogPrint("FAILURE: ECU %X  TP_C exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x49:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: APP_D = %d %%\n", temp_data_long);
							if (temp_data_long > 40)
							{
								LogPrint("FAILURE: ECU %X  APP_D exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x4A:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: APP_E = %d %%\n", temp_data_long);
							if (temp_data_long > 40)
							{
								LogPrint("FAILURE: ECU %X  APP_E exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x4B:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: APP_F = %d %%\n", temp_data_long);
							if (temp_data_long > 40)
							{
								LogPrint("FAILURE: ECU %X  APP_F exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;
						case 0x4D:
						{
							temp_data_long = ( pSid1[SidIndex].Data[0] * 256 ) +
							                 pSid1[SidIndex].Data[1];
							LogPrint("INFORMATION: MIL_TIME = %d min\n", temp_data_long);

							/* 06/04/04 - Do not check during IM drive cycle. */
							if (temp_data_long != 0x00)
							{
								if ( (gOBDEngineWarm == TRUE)     ||            // test 10.12
									 (gOBDEngineRunning == FALSE) ||            // test 5.6
									((gOBDEngineRunning == TRUE)  &&            // test 5.10
									 (gOBDEngineWarm == FALSE)    &&
									 (gOBDResponse[EcuIndex].Sid4Size > 0) &&
									 (gOBDResponse[EcuIndex].Sid4[0] == 0x44)))
								{
									LogPrint("FAILURE: ECU %X  MIL_TIME > 0\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;
						case 0x4E:
						{
							temp_data_long = ( pSid1[SidIndex].Data[0] * 256 ) +
							                 pSid1[SidIndex].Data[1];
							LogPrint("INFORMATION: CLR_TIME = %d min\n", temp_data_long);

							/* 06/04/04 - Do not check during IM drive cycle. */
							if (gOBDEngineWarm == TRUE)                     // test 10.12
							{
								if (temp_data_long > 30)
								{
									LogPrint("FAILURE: ECU %X  CLR_TIME > 30 min\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else if ( (gOBDEngineRunning == FALSE) ||       // test 5.6
									 ((gOBDEngineRunning == TRUE)  &&       // test 5.10
									  (gOBDResponse[EcuIndex].Sid4Size > 0) &&
									  (gOBDResponse[EcuIndex].Sid4[0] == 0x44)))
							{
								if (temp_data_long != 0)
								{
									LogPrint("FAILURE: ECU %X  CLR_TIME > 0 min\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x51:
						{
							temp_data_long = pSid1[SidIndex].Data[0];
							LogPrint("INFORMATION: FUEL_TYPE = $%02X\n", temp_data_long);

							if ( temp_data_long < 1 || temp_data_long > 16 )
							{
								LogPrint("FAILURE: ECU %X  FUEL_TYPE must be $01 to $16\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x52:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: ALCH_PCT = %d %%\n", temp_data_long);
						}
						break;

						case 0x53:
						{
							temp_data_float = ( (pSid1[SidIndex].Data[0] * 256) +
							                   pSid1[SidIndex].Data[1] ) * (float)0.005;
							LogPrint("INFORMATION: EVAP_VPA = %f kPa\n", temp_data_float);
						}
						break;

						case 0x54:
						{
							temp_data_long = ( ((signed short)pSid1[SidIndex].Data[0] * 256) +
							                   pSid1[SidIndex].Data[1] ) - 0x8000;
							LogPrint("INFORMATION: EVAP_VP = %d Pa\n", temp_data_long);
						}
						break;

						case 0x55:
						{
							temp_data_float = ( (float)( pSid1[SidIndex].Data[0]  - 128 ) * 100 ) / 128;
							LogPrint("INFORMATION: STSO2FT1 = %f %%\n", temp_data_float);
							if (temp_data_float < -100.0 || temp_data_float > 99.2)
							{
								LogPrint ( "FAILURE: ECU %X  STSO2FT1 out of range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if (gSid1VariablePidSize == 2)
							{
								temp_data_float = ( (float)( pSid1[SidIndex].Data[1] - 128 ) * 100 ) / 128;
								LogPrint("INFORMATION: STSO2FT3 = %f %%\n", temp_data_float);
								if (temp_data_float < -100.0 || temp_data_float > 99.2)
								{
									LogPrint ( "FAILURE: ECU %X  STSO2FT3 out of range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x56:
						{
							temp_data_float = ( (float)( pSid1[SidIndex].Data[0] - 128 ) * 100 ) / 128;
							LogPrint("INFORMATION: LGSO2FT1 = %f %%\n", temp_data_float);
							if (temp_data_float < -100.0 || temp_data_float > 99.2)
							{
								LogPrint ( "FAILURE: ECU %X  LGSO2FT1 out of range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if (gSid1VariablePidSize == 2)
							{
								temp_data_float = ( (float)( pSid1[SidIndex].Data[1] - 128 ) * 100 ) / 128;
								LogPrint("INFORMATION: LGSO2FT3 = %f %%\n", temp_data_float);
								if (temp_data_float < -100.0 || temp_data_float > 99.2)
								{
									LogPrint ( "FAILURE: ECU %X  LGSO2FT3 out of range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x57:
						{
							temp_data_float = ( (float)( pSid1[SidIndex].Data[0] - 128 ) * 100 ) / 128;
							LogPrint("INFORMATION: STSO2FT2 = %f %%\n", temp_data_float);
							if (temp_data_float < -100.0 || temp_data_float > 99.2)
							{
								LogPrint ( "FAILURE: ECU %X  STSO2FT2 out of range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if (gSid1VariablePidSize == 2)
							{
								temp_data_float = ( (float)( pSid1[SidIndex].Data[1] - 128 ) * 100 ) / 128;
								LogPrint("INFORMATION: STSO2FT4 = %f %%\n", temp_data_float);
								if (temp_data_float < -100.0 || temp_data_float > 99.2)
								{
									LogPrint ( "FAILURE: ECU %X  STSO2FT4 out of range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x58:
						{
							temp_data_float = ( (float)( pSid1[SidIndex].Data[0] - 128 ) * 100 ) / 128;
							LogPrint("INFORMATION: LGSO2FT2 = %f %%\n", temp_data_float);
							if (temp_data_float < -100.0 || temp_data_float > 99.2)
							{
								LogPrint ( "FAILURE: ECU %X  LGSO2FT2 out of range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if (gSid1VariablePidSize == 2)
							{
								temp_data_float = ( (float)( pSid1[SidIndex].Data[1] - 128 ) * 100 ) / 128;
								LogPrint("INFORMATION: LGSO2FT4 = %f %%\n", temp_data_float);
								if (temp_data_float < -100.0 || temp_data_float > 99.2)
								{
									LogPrint ( "FAILURE: ECU %X  LGSO2FT4 out of range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x59:
						{
							temp_data_long = ( (pSid1[SidIndex].Data[0] * 256) +
							                    pSid1[SidIndex].Data[1] ) * 10;
							LogPrint("INFORMATION: FRP = %d kPa\n", temp_data_long);
							if (  gOBDEngineRunning == TRUE && temp_data_long == 0 )
							{
								LogPrint("FAILURE: ECU %X  FRP exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x5A:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: APP_R = %d %%\n", temp_data_long);
							if ( temp_data_long > 40 )
							{
								LogPrint("FAILURE: ECU %X  APP_R exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x5B:
						{
							temp_data_long = ((pSid1[SidIndex].Data[0] * 100) / 255);
							LogPrint("INFORMATION: BAT_PWR = %d %%\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE && temp_data_long == 0 )
							{
								LogPrint("FAILURE: ECU %X  BAT_PWR exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x5C:
						{
							temp_data_long = pSid1[SidIndex].Data[0] - 40;
							LogPrint("INFORMATION: EOT = %d C\n", temp_data_long);
							if ( temp_data_long  < -20 || temp_data_long > 150 )
							{
								LogPrint("FAILURE: ECU %X  EOT exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x5D:
						{
							temp_data_long = ( ( (pSid1[SidIndex].Data[0] * 256) +
							                      pSid1[SidIndex].Data[1] ) - 26880 ) / 128;
							LogPrint("INFORMATION: FUEL_TIMING = %d\n", temp_data_long);
							if ( temp_data_long < -210 || temp_data_long >= 302 )
							{
								LogPrint("FAILURE: ECU %X  FUEL_TIMING exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x5E:
						{
							temp_data_float = ( (pSid1[SidIndex].Data[0] * 256) +
							                    pSid1[SidIndex].Data[1] ) * (float)0.05;
							LogPrint("INFORMATION: FUEL_RATE = %f L/h\n", temp_data_float);
							if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
							{
								if ( temp_data_float == 0 )
								{
									LogPrint("FAILURE: ECU %X  FUEL_RATE exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
							else // test 5.6
							{
								if ( temp_data_float != 0 )
								{
									LogPrint("FAILURE: ECU %X  FUEL_RATE exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x5F:
						{
							temp_data_long = pSid1[SidIndex].Data[0];
							LogPrint("INFORMATION: EMIS_SUP = $%02X\n", temp_data_long);
							if ( temp_data_long  < 0x0E || temp_data_long > 0x10 )
							{
								LogPrint("FAILURE: ECU %X  EMIS_SUP must be $0E to $10\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x61:
						{
							temp_data_long = pSid1[SidIndex].Data[0] - 125;
							LogPrint("INFORMATION: TQ_DD = %d %%\n", temp_data_long);
						}
						break;

						case 0x62:
						{
							temp_data_long = pSid1[SidIndex].Data[0] - 125;
							LogPrint("INFORMATION: TQ_ACT = %d %%\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE )  // test 5.10 & 10.12
							{
								if ( temp_data_long <= 0 )
								{
									LogPrint("FAILURE: ECU %X  TQ_ACT exceeded normal range\n", GetEcuId(EcuIndex) );
								    ERROR_RETURN;
								}
							}
							else if ( temp_data_long < 0 )  // test 5.6
							{
								LogPrint("FAILURE: ECU %X  TQ_ACT exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x63:
						{
							temp_data_long = (pSid1[SidIndex].Data[0] * 256) +
							                 pSid1[SidIndex].Data[1];
							LogPrint("INFORMATION: TQ_REF = %d Nm\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE && temp_data_long < 0 )  // test 5.10 & 10.12
							{
								LogPrint("FAILURE: ECU %X  TQ_REF exceeded normal range\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x64:
						{
							temp_data_long = pSid1[SidIndex].Data[0] - 125;
							LogPrint("INFORMATION: TQ_MAX1 = %d %%\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE )  // test 5.10 & 10.12
							{
								if ( temp_data_long <= 0 )
								{
									LogPrint("FAILURE: ECU %X  TQ_MAX1 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							temp_data_long = pSid1[SidIndex].Data[1] - 125;
							LogPrint("INFORMATION: TQ_MAX2 = %d %%\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE )  // test 5.10 & 10.12
							{
								if ( temp_data_long <= 0 )
								{
									LogPrint("FAILURE: ECU %X  TQ_MAX2 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							temp_data_long = pSid1[SidIndex].Data[2] - 125;
							LogPrint("INFORMATION: TQ_MAX3 = %d %%\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE )  // test 5.10 & 10.12
							{
								if ( temp_data_long <= 0 )
								{
									LogPrint("FAILURE: ECU %X  TQ_MAX3 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							temp_data_long = pSid1[SidIndex].Data[3] - 125;
							LogPrint("INFORMATION: TQ_MAX4 = %d %%\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE )  // test 5.10 & 10.12
							{
								if ( temp_data_long <= 0 )
								{
									LogPrint("FAILURE: ECU %X  TQ_MAX4 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							temp_data_long = pSid1[SidIndex].Data[4] - 125;
							LogPrint("INFORMATION: TQ_MAX5 = %d %%\n", temp_data_long);
							if ( gOBDEngineRunning == TRUE )  // test 5.10 & 10.12
							{
								if ( temp_data_long <= 0 )
								{
									LogPrint("FAILURE: ECU %X  TQ_MAX5 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x65:
						{
							LogPrint( "INFORMATION: Auxillary I/O A = $%02X  Auxillary I/O B = $%02X\n",
							          pSid1[SidIndex].Data[0],
							          pSid1[SidIndex].Data[1] );

							if ( (pSid1[SidIndex].Data[0] & 0xF0) != 0 ||
							     (pSid1[SidIndex].Data[1] & 0xF0) != 0)
							{
								LogPrint("FAILURE: ECU %X  Auxillary I/O  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x66:
						{
							LogPrint( "INFORMATION: MAF Sensor support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x03) == 0 )
							{
								LogPrint("FAILURE: ECU %X  MAF Sensor support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xFC )
							{
								LogPrint("FAILURE: ECU %X  MAF Sensor support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( (pSid1[SidIndex].Data[1] * 256) +
								                    pSid1[SidIndex].Data[2] ) * (float)0.03125;
								LogPrint("INFORMATION: MAFA = %f g/s\n", temp_data_float);
								if ( gOBDEngineRunning == TRUE )  // test 5.10 & 10.12
								{
									if ( temp_data_float < 0 )
									{
										LogPrint("FAILURE: ECU %X  MAFA exceeded normal range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
								else  // test 5.6
								{
									if ( temp_data_float > 5 )
									{
										LogPrint("FAILURE: ECU %X  MAFA exceeded normal range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( (pSid1[SidIndex].Data[3] * 256) +
								                    pSid1[SidIndex].Data[4] ) * (float)0.03125;
								LogPrint("INFORMATION: MAFB = %f g/s\n", temp_data_float);
								if ( gOBDEngineRunning == TRUE )  // test 5.10 & 10.12
								{
									if ( temp_data_float < 0 )
									{
										LogPrint("FAILURE: ECU %X  MAFB exceeded normal range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
								else
								{
									if ( temp_data_float > 5 )
									{
										LogPrint("FAILURE: ECU %X  MAFB exceeded normal range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}
						}
						break;

						case 0x67:
						{
							LogPrint( "INFORMATION: ECT Sensor support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x03) == 0 )
							{
								LogPrint("FAILURE: ECU %X  ECT Sensor support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xFC )
							{
								LogPrint("FAILURE: ECU %X  ECT Sensor support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = (pSid1[SidIndex].Data[1] ) - 40;
								LogPrint("INFORMATION: ECT1 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  ECT1 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = (pSid1[SidIndex].Data[2] ) - 40;
								LogPrint("INFORMATION: ECT2 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  ECT2 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x68:
						{
							LogPrint( "INFORMATION: IAT Sensor support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x3F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  IAT Sensor support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xC0 )
							{
								LogPrint("FAILURE: ECU %X  IAT Sensor support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] ) - 40;
								LogPrint("INFORMATION: IAT11 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  IAT11 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] ) - 40;
								LogPrint("INFORMATION: IAT12 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  IAT12 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = (pSid1[SidIndex].Data[3] ) - 40;
								LogPrint("INFORMATION: IAT11 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  IAT13 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[4] ) - 40;
								LogPrint("INFORMATION: IAT21 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  IAT21 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x10) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[5] ) - 40;
								LogPrint("INFORMATION: IAT22 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  IAT22 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x20) != 0 )
							{
								temp_data_long =  (pSid1[SidIndex].Data[6] ) - 40;
								LogPrint("INFORMATION: IAT23 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  IAT23 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x69:
						{
							LogPrint( "INFORMATION: EGR data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x3F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  EGR data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xC0 )
							{
								LogPrint("FAILURE: ECU %X  EGR data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] * 100 ) / 255;
								LogPrint("INFORMATION: EGR_A_CMD = %d %%\n", temp_data_long);
								if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
								{
									if ( gOBDDieselFlag == FALSE && temp_data_long > 10 )
									{
										LogPrint("FAILURE: ECU %X  EGR_A_CMD exceeded normal range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
								else // test 5.6
								{
									if ( temp_data_long > 10 )
									{
										LogPrint("FAILURE: ECU %X  EGR_A_CMD exceeded normal range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] * 100 ) / 255;
								LogPrint("INFORMATION: EGR_A_ACT = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( (pSid1[SidIndex].Data[3] - 128 ) * 100 ) / 128;
								LogPrint("INFORMATION: EGR_A_ERR = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[4] ) * 100 ) / 255;
								LogPrint("INFORMATION: EGR_B_CMD = %d %%\n", temp_data_long);

								if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
								{
									if ( gOBDDieselFlag == FALSE && temp_data_long > 10 )
									{
										LogPrint("FAILURE: ECU %X  EGR_B_CMD exceeded normal range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}
								else // test 5.6
								{
									if ( temp_data_long > 10 )
									{
										LogPrint("FAILURE: ECU %X  EGR_B_CMD exceeded normal range\n", GetEcuId(EcuIndex) );
										ERROR_RETURN;
									}
								}

							if ( (pSid1[SidIndex].Data[0] & 0x10) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[5] * 100 ) / 255;
								LogPrint("INFORMATION: EGR_B_ACT = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x20) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[6] - 128) * 100 ) / 128;
								LogPrint("INFORMATION: EGR_B_ERR = %d %%\n", temp_data_long);
							}
						}
						break;

						case 0x6A:
						{
							LogPrint( "INFORMATION: IAF data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  IAF data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  IAF data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] * 100 ) / 255;
								LogPrint("INFORMATION: IAF_A_CMD = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] * 100 ) / 255;
								LogPrint("INFORMATION: IAF_A_REL = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[3] * 100 ) / 255;
								LogPrint("INFORMATION: IAF_B_CMD = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[4] * 100 ) / 255;
								LogPrint("INFORMATION: IAF_B_REL = %d %%\n", temp_data_long);
							}
						}
						break;

						case 0x6B:
						{
							LogPrint( "INFORMATION: EGRT sensor support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  EGRT sensor support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  EGRT sensor support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] ) -40;
								LogPrint("INFORMATION: EGRT11 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  EGRT11 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] ) -40;
								LogPrint("INFORMATION: EGRT12 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  EGRT12 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[3] ) -40;
								LogPrint("INFORMATION: EGRT21 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  EGRT21 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[4] ) -40;
								LogPrint("INFORMATION: EGRT22 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  EGRT22 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x6C:
						{
							LogPrint( "INFORMATION: TAC data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  TAC data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  TAC data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] * 100 ) / 255;
								LogPrint("INFORMATION: TAC_A_CMD = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] * 100 ) / 255;
								LogPrint("INFORMATION: TP_A_REL = %d %%\n", temp_data_long);
								if ( gOBDDieselFlag == FALSE && temp_data_long > 50 )
								{
									LogPrint("FAILURE: ECU %X  TP_A_REL exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[3] * 100 ) / 255;
								LogPrint("INFORMATION: TAC_B_CMD = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[4] * 100 ) / 255;
								LogPrint("INFORMATION: TP_B_REL = %d %%\n", temp_data_long);
								if ( gOBDDieselFlag == FALSE && temp_data_long > 50 )
								{
									LogPrint("FAILURE: ECU %X  TP_B_REL exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x6D:
						{
							LogPrint( "INFORMATION: FRP data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x3F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  FRP data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xC0 )
							{
								LogPrint("FAILURE: ECU %X  FRP data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[1] * 256 ) +
								                   pSid1[SidIndex].Data[2] ) * 10;
								LogPrint("INFORMATION: FRP_A_CMD = %d kPa\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[3] * 256 ) +
								                   pSid1[SidIndex].Data[4] ) * 10;
								LogPrint("INFORMATION: FRP_A = %d kPa\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[5] ) - 40;
								LogPrint("INFORMATION: FRT_A = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  FRT_A exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[6] * 256 ) +
								                   pSid1[SidIndex].Data[7] ) * 10;
								LogPrint("INFORMATION: FRP_B_CMD = %d kPa\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x10) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[8] * 256 ) +
								                   pSid1[SidIndex].Data[9] ) * 10;
								LogPrint("INFORMATION: FRP_B = %d kPa\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x20) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[10] ) - 40;
								LogPrint("INFORMATION: FRT_B = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  FRT_B exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x6E:
						{
							LogPrint( "INFORMATION: ICP system data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  ICP data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  ICP data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[1] * 256 ) +
								                   pSid1[SidIndex].Data[2] ) * 10;
								LogPrint("INFORMATION: ICP_A_CMD = %d kPa\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[3] * 256 ) +
								                   pSid1[SidIndex].Data[4] ) * 10;
								LogPrint("INFORMATION: ICP_A = %d kPa\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[5] * 256 ) +
								                   pSid1[SidIndex].Data[6] ) * 10;
								LogPrint("INFORMATION: ICP_B_CMD = %d kPa\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( ( pSid1[SidIndex].Data[7] * 256 ) +
								                   pSid1[SidIndex].Data[8] ) * 10;
								LogPrint("INFORMATION: ICP_B = %d kPa\n", temp_data_long);
							}
						}
						break;

						case 0x6F:
						{
							LogPrint( "INFORMATION: TCA sensor support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x03) == 0 )
							{
								LogPrint("FAILURE: ECU %X  TCA sensor support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xFC )
							{
								LogPrint("FAILURE: ECU %X  TCA sensor support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = pSid1[SidIndex].Data[1];
								LogPrint("INFORMATION: TCA_CINP = %d kPa\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = pSid1[SidIndex].Data[2];
								LogPrint("INFORMATION: TCB_CINP = %d kPa\n", temp_data_long);
							}
						}
						break;

						case 0x70:
						{
							LogPrint( "INFORMATION: BP data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x3F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  BP data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xC0 )
							{
								LogPrint("FAILURE: ECU %X  BP data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( ( pSid1[SidIndex].Data[1] * 256 ) +
								                    pSid1[SidIndex].Data[2] ) * (float)0.03125;
								LogPrint("INFORMATION: BP_A_CMD = %f kPa\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( ( pSid1[SidIndex].Data[3] * 256 ) +
								                    pSid1[SidIndex].Data[4] ) * (float)0.03125;
								LogPrint("INFORMATION: BP_A_ACT = %f kPa\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_float = ( ( pSid1[SidIndex].Data[5] * 256 ) +
								                    pSid1[SidIndex].Data[6] ) * (float)0.03125;
								LogPrint("INFORMATION: BP_B_CMD = %f kPa\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x10) != 0 )
							{
								temp_data_float = ( ( pSid1[SidIndex].Data[7] * 256 ) +
								                    pSid1[SidIndex].Data[8] ) * (float)0.03125;
								LogPrint("INFORMATION: BP_B_ACT = %f kPa\n", temp_data_float);
							}

							/* bit 2 (0x04) BP_A Status and bit 5 (0x20) BP_B Status */
							if ( (pSid1[SidIndex].Data[0] & 0x24) != 0 )
							{
								LogPrint( "INFORMATION: BP control status = $%02X\n", pSid1[SidIndex].Data[9] );
								/* check that reserved bits are not set */
								if ( pSid1[SidIndex].Data[9] & 0xF0 )
								{
									LogPrint("FAILURE: ECU %X  BP control status  Reserved bits set\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x71:
						{
							LogPrint( "INFORMATION: VGT data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x3F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  VGT data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xC0 )
							{
								LogPrint("FAILURE: ECU %X  VGT data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] * 100 ) / 255;
								LogPrint("INFORMATION: VGT_A_CMD = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
								temp_data_long = ( pSid1[SidIndex].Data[2] * 100 ) / 255;
								LogPrint("INFORMATION: VGT_A_ACT = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[3] * 100 ) / 255;
								LogPrint("INFORMATION: VGT_B_CMD = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x10) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[4] * 100 ) / 255;
								LogPrint("INFORMATION: VGT_B_ACT = %d %%\n", temp_data_long);
							}

							/* bit 2 (0x04) VGT_A Status and bit 5 (0x20) VGT_B Status */
							if ( (pSid1[SidIndex].Data[0] & 0x24) != 0 )
							{
								LogPrint( "INFORMATION: VGT control status = $%02X\n", pSid1[SidIndex].Data[5] );
								/* check that reserved bits are not set */
								if ( pSid1[SidIndex].Data[5] & 0xF0 )
								{
									LogPrint("FAILURE: ECU %X  VGT control status  Reserved bits set\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x72:
						{
							LogPrint( "INFORMATION: WG data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  WG data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  WG data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] * 100 ) / 255;
								LogPrint("INFORMATION: WG_A_CMD = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] * 100 ) / 255;
								LogPrint("INFORMATION: WG_A_ACT = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[3] * 100 ) / 255;
								LogPrint("INFORMATION: WG_B_CMD = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[4] * 100 ) / 255;
								LogPrint("INFORMATION: WG_B_ACT = %d %%\n", temp_data_long);
							}
						}
						break;

						case 0x73:
						{
							LogPrint( "INFORMATION: EP data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x03) == 0 )
							{
								LogPrint("FAILURE: ECU %X  EP data support  No sensors supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xFC )
							{
								LogPrint("FAILURE: ECU %X  EP data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( (pSid1[SidIndex].Data[1] * 256) +
								                    pSid1[SidIndex].Data[2] ) * (float)0.01;
								LogPrint("INFORMATION: EP1 = %f kPa\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( (pSid1[SidIndex].Data[3] * 256) +
								                    pSid1[SidIndex].Data[4] ) * (float)0.01;
								LogPrint("INFORMATION: EP2 = %f kPa\n", temp_data_float);
							}
						}
						break;

						case 0x74:
						{
							LogPrint( "INFORMATION: TC_RPM data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x03) == 0 )
							{
								LogPrint("FAILURE: ECU %X  TC_RPM data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xFC )
							{
								LogPrint("FAILURE: ECU %X  TC_RPM data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = (pSid1[SidIndex].Data[1] * 256) +
								                  pSid1[SidIndex].Data[2];
								LogPrint("INFORMATION: TCA_RPM = %d RPM\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = (pSid1[SidIndex].Data[3] * 256) +
								                  pSid1[SidIndex].Data[4];
								LogPrint("INFORMATION: TCB_RPM = %d RPM\n", temp_data_long);
							}
						}
						break;

						case 0x75:
						{
							LogPrint( "INFORMATION: TC A Temp data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  TC A Temp data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  TC A Temp data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] ) - 40;
								LogPrint("INFORMATION: TCA_CINT = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  TCA_CINT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] ) - 40;
								LogPrint("INFORMATION: TCA_COUTT = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  TCA_COUTT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[3] * 256) +
								                      pSid1[SidIndex].Data[4] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: TCA_TINT = %f C\n", temp_data_float);
								if ( temp_data_float < -20 || temp_data_float > 120 )
								{
									LogPrint("FAILURE: ECU %X  TCA_TINT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[5] * 256) +
								                      pSid1[SidIndex].Data[6] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: TCA_TOUTT = %f C\n", temp_data_float);
								if ( temp_data_float < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  TCA_TOUTT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x76:
						{
							LogPrint( "INFORMATION: TC B Temp data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  TC B Temp data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  TC B Temp data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] ) - 40;
								LogPrint("INFORMATION: TCB_CINT = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  TCB_CINT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] ) - 40;
								LogPrint("INFORMATION: TCB_COUTT = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  TCB_COUTT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[3] * 256) +
								                      pSid1[SidIndex].Data[4] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: TCB_TINT = %f C\n", temp_data_float);
								if ( temp_data_float < -20 || temp_data_float > 120 )
								{
									LogPrint("FAILURE: ECU %X  TCB_TINT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[5] * 256) +
								                      pSid1[SidIndex].Data[6] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: TCB_TOUTT = %f C\n", temp_data_float);
								if ( temp_data_float < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  TCB_TOUTT exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x77:
						{
							LogPrint( "INFORMATION: CACT data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  CACT data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  CACT data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] ) - 40;
								LogPrint("INFORMATION: CACT11 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  CACT11 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[2] ) - 40;
								LogPrint("INFORMATION: CACT12 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  CACT12 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[3] ) - 40;
								LogPrint("INFORMATION: CACT21 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  CACT21 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[4] ) - 40;
								LogPrint("INFORMATION: CACT22 = %d C\n", temp_data_long);
								if ( temp_data_long < -20 || temp_data_long > 120 )
								{
									LogPrint("FAILURE: ECU %X  CACT22 exceeded normal range\n", GetEcuId(EcuIndex) );
									ERROR_RETURN;
								}
							}
						}
						break;

						case 0x78:
						{
							LogPrint( "INFORMATION: EGT Bank 1 data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  EGT Bank 1 data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  EGT Bank 1 data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[1] * 256) +
								                      pSid1[SidIndex].Data[2] ) * (float).1 ) - 40;
								LogPrint("INFORMATION: EGT11 = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[3] * 256) +
								                      pSid1[SidIndex].Data[4] ) * (float).1 ) - 40;
								LogPrint("INFORMATION: EGT12 = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[5] * 256) +
								                      pSid1[SidIndex].Data[6] ) * (float).1 ) - 40;
								LogPrint("INFORMATION: EGT13 = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[7] * 256) +
								                      pSid1[SidIndex].Data[8] ) * (float).1 ) - 40;
								LogPrint("INFORMATION: EGT14 = %f C\n", temp_data_float);
							}
						}
						break;

						case 0x79:
						{
							LogPrint( "INFORMATION: EGT Bank 2 data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  EGT Bank 2 data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  EGT Bank 2 data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[1] * 256) +
								                      pSid1[SidIndex].Data[2] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: EGT21 = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[3] * 256) +
								                      pSid1[SidIndex].Data[4] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: EGT22 = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[5] * 256) +
								                      pSid1[SidIndex].Data[6] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: EGT23 = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[7] * 256) +
								                      pSid1[SidIndex].Data[8] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: EGT24 = %f C\n", temp_data_float);
							}
						}
						break;

						case 0x7A:
						{
							LogPrint( "INFORMATION: DPF Bank 1 data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x07) == 0 )
							{
								LogPrint("FAILURE: ECU %X  DPF Bank 1 data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF8 )
							{
								LogPrint("FAILURE: ECU %X  DPF Bank 1 data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = (float)((((signed short)( pSid1[SidIndex].Data[1] ) * 256 ) +
								                                          pSid1[SidIndex].Data[2] ) * 0.1);
								LogPrint("INFORMATION: DPF1_DP = %f kPa\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( ( ( pSid1[SidIndex].Data[3] ) * 256 ) +
								                      pSid1[SidIndex].Data[4] ) * (float)0.1;
								LogPrint("INFORMATION: DPF1_INP = %f kPa\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_float = ( ( ( pSid1[SidIndex].Data[5] ) * 256 ) +
								                      pSid1[SidIndex].Data[6] ) * (float)0.1;
								LogPrint("INFORMATION: DPF1_OUTP = %f kPa\n", temp_data_float);
							}
						}
						break;

						case 0x7B:
						{
							LogPrint( "INFORMATION: DPF Bank 2 data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x07) == 0 )
							{
								LogPrint("FAILURE: ECU %X  DPF Bank 2 data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF8 )
							{
								LogPrint("FAILURE: ECU %X  DPF Bank 2 data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = (float)((((signed short)( pSid1[SidIndex].Data[1] ) * 256 ) +
								                                          pSid1[SidIndex].Data[2] ) * 0.1);
								LogPrint("INFORMATION: DPF2_DP = %f kPa\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( ( ( pSid1[SidIndex].Data[3] ) * 256 ) +
								                      pSid1[SidIndex].Data[4] ) * (float)0.1;
								LogPrint("INFORMATION: DPF2_INP = %f kPa\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_float = ( ( ( pSid1[SidIndex].Data[5] ) * 256 ) +
								                      pSid1[SidIndex].Data[6] ) * (float)0.1;
								LogPrint("INFORMATION: DPF2_OUTP = %f kPa\n", temp_data_float);
							}
						}
						break;

						case 0x7C:
						{
							LogPrint( "INFORMATION: DPF Temp data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  DPF Temp data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  DPF Temp data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[1] * 256) +
								                      pSid1[SidIndex].Data[2] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: DPF1_INT = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[3] * 256) +
								                      pSid1[SidIndex].Data[4] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: DPF1_OUTT = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[5] * 256) +
								                      pSid1[SidIndex].Data[6] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: DPF2_INT = %f C\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_float = ( ( (pSid1[SidIndex].Data[7] * 256) +
								                      pSid1[SidIndex].Data[8] ) * (float)0.1 ) - 40;
								LogPrint("INFORMATION: DPF2_OUTT = %f C\n", temp_data_float);
							}
						}
						break;

						case 0x7D:
						{
							LogPrint( "INFORMATION: NOx NTE control area status = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  NOx NTE control area status  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x7E:
						{
							LogPrint( "INFORMATION: PM NTE control area status = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  PM NTE control area status  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}
						}
						break;

						case 0x7F:
						{
							LogPrint( "INFORMATION: Run Time support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x07) == 0 )
							{
								LogPrint("FAILURE: ECU %X  Run Time data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF8 )
							{
								LogPrint("FAILURE: ECU %X  Run Time support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[2] * 65536 ) +
								                 ( pSid1[SidIndex].Data[3] * 256 ) +
								                 pSid1[SidIndex].Data[4];
								LogPrint("INFORMATION: RUN_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[5] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[6] * 65536 ) +
								                 ( pSid1[SidIndex].Data[7] * 256 ) +
								                 pSid1[SidIndex].Data[8];
								LogPrint("INFORMATION: IDLE_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[9] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[10] * 65536 ) +
								                 ( pSid1[SidIndex].Data[11] * 256 ) +
								                 pSid1[SidIndex].Data[12];
								LogPrint("INFORMATION: PTO_TIME = %d sec\n", temp_data_long);
							}
						}
						break;

						case 0x81:
						{
							LogPrint( "INFORMATION: Run Time for AECD #1-#5 support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x1F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  Run Time for AECD #1-#5 support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xE0 )
							{
								LogPrint("FAILURE: ECU %X  Run Time for AECD #1-#5 support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[2] * 65536 ) +
								                 ( pSid1[SidIndex].Data[3] * 256 ) +
								                 pSid1[SidIndex].Data[4];
								LogPrint("INFORMATION: AECD1_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[5] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[6] * 65536 ) +
								                 ( pSid1[SidIndex].Data[7] * 256 ) +
								                 pSid1[SidIndex].Data[8];
								LogPrint("INFORMATION: AECD2_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[9] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[10] * 65536 ) +
								                 ( pSid1[SidIndex].Data[11] * 256 ) +
								                 pSid1[SidIndex].Data[12];
								LogPrint("INFORMATION: AECD3_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[13] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[14] * 65536 ) +
								                 ( pSid1[SidIndex].Data[15] * 256 ) +
								                 pSid1[SidIndex].Data[16];
								LogPrint("INFORMATION: AECD4_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x10) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[17] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[18] * 65536 ) +
								                 ( pSid1[SidIndex].Data[19] * 256 ) +
								                 pSid1[SidIndex].Data[20];
								LogPrint("INFORMATION: AECD5_TIME = %d sec\n", temp_data_long);
							}
						}
						break;

						case 0x82:
						{
							LogPrint( "INFORMATION: Run Time for AECD #6-#10 support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xE0 )
							{
								LogPrint("FAILURE: ECU %X  Run Time for AECD #6-#10 support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[1] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[2] * 65536 ) +
								                 ( pSid1[SidIndex].Data[3] * 256 ) +
								                 pSid1[SidIndex].Data[4];
								LogPrint("INFORMATION: AECD6_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[5] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[6] * 65536 ) +
								                 ( pSid1[SidIndex].Data[7] * 256 ) +
								                 pSid1[SidIndex].Data[8];
								LogPrint("INFORMATION: AECD7_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[9] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[10] * 65536 ) +
								                 ( pSid1[SidIndex].Data[11] * 256 ) +
								                 pSid1[SidIndex].Data[12];
								LogPrint("INFORMATION: AECD8_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[13] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[14] * 65536 ) +
								                 ( pSid1[SidIndex].Data[15] * 256 ) +
								                 pSid1[SidIndex].Data[16];
								LogPrint("INFORMATION: AECD9_TIME = %d sec\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x10) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[17] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[18] * 65536 ) +
								                 ( pSid1[SidIndex].Data[19] * 256 ) +
								                 pSid1[SidIndex].Data[20];
								LogPrint("INFORMATION: AECD10_TIME = %d sec\n", temp_data_long);
							}
						}
						break;

						case 0x83:
						{
							LogPrint( "INFORMATION: NOx Sensor Data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x03) == 0 )
							{
								LogPrint("FAILURE: ECU %X  NOx Sensor Data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xFC )
							{
								LogPrint("FAILURE: ECU %X  NOx Sensor Data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_long = (pSid1[SidIndex].Data[1] * 256) +
								                  pSid1[SidIndex].Data[2];
								LogPrint("INFORMATION: NOX11 = %d ppm\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_long = (pSid1[SidIndex].Data[3] * 256) +
								                  pSid1[SidIndex].Data[4];
								LogPrint("INFORMATION: NOX21 = %d ppm\n", temp_data_long);
							}
						}
						break;

						case 0x84:
						{
							LogPrint( "INFORMATION: MST = %d C\n", ( pSid1[SidIndex].Data[0] ) -40 );
						}
						break;

						case 0x85:
						{
							LogPrint( "INFORMATION: NOx Reagent data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x0F) == 0 )
							{
								LogPrint("FAILURE: ECU %X  NOx Reagent data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xF0 )
							{
								LogPrint("FAILURE: ECU %X  NOx Reagent data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( ( pSid1[SidIndex].Data[1] * 256 ) +
								                    pSid1[SidIndex].Data[2] ) * (float)0.005;
								LogPrint("INFORMATION: REAG_RATE = %f L/h\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( (pSid1[SidIndex].Data[3] * 256) +
								                    pSid1[SidIndex].Data[4] ) * (float)0.005;
								LogPrint("INFORMATION: REAG_DEMD = %f L/h\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x04) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[5] * 100) / 255;
								LogPrint("INFORMATION: REAG_LVL = %d %%\n", temp_data_long);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x08) != 0 )
							{
								temp_data_long = ( pSid1[SidIndex].Data[6] * 16777216 ) +
								                 ( pSid1[SidIndex].Data[7] * 65536 ) +
								                 ( pSid1[SidIndex].Data[8] * 256 ) +
								                 pSid1[SidIndex].Data[9];
								LogPrint("INFORMATION: NWI_TIME = %d sec\n", temp_data_long);
							}
						}
						break;

						case 0x86:
						{
							LogPrint( "INFORMATION: PM data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x03) == 0 )
							{
								LogPrint("FAILURE: ECU %X  PM data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xFC )
							{
								LogPrint("FAILURE: ECU %X  PM data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( ( pSid1[SidIndex].Data[1] * 256 ) +
								                    pSid1[SidIndex].Data[2] ) * (float)0.0125;
								LogPrint("INFORMATION: PM11 = %f mg/m3\n", temp_data_float);
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( (pSid1[SidIndex].Data[3] * 256) +
								                    pSid1[SidIndex].Data[4] ) * (float)0.0125;
								LogPrint("INFORMATION: PM21 = %f mg/m3\n", temp_data_float);
							}
						}
						break;

						case 0x87:
						{
							LogPrint( "INFORMATION: MAP data support = $%02X\n", pSid1[SidIndex].Data[0] );
							/* check that required bit is set */
							if ( (pSid1[SidIndex].Data[0] & 0x03) == 0 )
							{
								LogPrint("FAILURE: ECU %X  MAP data support  No data supported\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							/* check that reserved bits are not set */
							if ( pSid1[SidIndex].Data[0] & 0xFC )
							{
								LogPrint("FAILURE: ECU %X  MAP data support  Reserved bits set\n", GetEcuId(EcuIndex) );
								ERROR_RETURN;
							}

							if ( (pSid1[SidIndex].Data[0] & 0x01) != 0 )
							{
								temp_data_float = ( ( pSid1[SidIndex].Data[1] * 256 ) +
								                    pSid1[SidIndex].Data[2] ) * (float)0.03125;
								LogPrint("INFORMATION: MAP_A = %f kPa\n", temp_data_float);
								if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
								{
									if ( temp_data_float == 0 )
									{
										LogPrint("FAILURE: ECU %X  MAP_A must be non zero\n", GetEcuId(EcuIndex) );
									}
								}
							}

							if ( (pSid1[SidIndex].Data[0] & 0x02) != 0 )
							{
								temp_data_float = ( (pSid1[SidIndex].Data[3] * 256) +
								                    pSid1[SidIndex].Data[4] ) * (float)0.03125;
								LogPrint("INFORMATION: MAP_B = %f kPa\n", temp_data_float);
								if (gOBDEngineRunning == TRUE)  // test 5.10 & 10.12
								{
									if ( temp_data_float == 0 )
									{
										LogPrint("FAILURE: ECU %X  MAP_B must be non zero\n", GetEcuId(EcuIndex) );
									}
								}
							}
						}
						break;

						default:
						{
							/* Non-OBD PID */
						}
						break;

					}  // end switch(pSid1[SidIndex].PID)

				}  // end for (SidIndex

			}  // end if (IsSid1PidSupported (EcuIndex, IdIndex) == TRUE)

		}  // end for (EcuIndex

		// If there where any errors in the data, fail
		if ( ErrorCount() != ErrCntTmp )
		{
			ErrorFlags (ErrFlgTmp);  //reset the Error Flags
			LogPrint("FAILURE: Invalid SID $1 PID $%02X Data\n", IdIndex);
			ERROR_RETURN;
		}
		ErrorFlags (ErrFlgTmp);  //reset the Error Flags

	}  // end for (IdIndex


	// Prepare Error Flags and Error Count for ERROR_RETURN
	ErrFlgTmp = ErrorFlags (ER_BYPASS_USER_PROMPT | ER_CONTINUE | ER_DONT_CLEAR_ERROR_COUNT);
	ErrCntTmp = ErrorCount();


	/* All vehicles require Pid $05 or $67 */
	if ( fPidSupported[0x05] == FALSE &&
	     fPidSupported[0x67] == FALSE )
	{
		fReqPidNotSupported = TRUE;
		LogPrint("WARNING: Neither SID $1 PID $05 nor $67 supported (Support of one required for all vehicles)\n");
	}

	/* Gasoline vehicles require Pid $0B, $87, $10 or $66 */
	if ( gOBDDieselFlag == FALSE &&
	     ( fPidSupported[0x0B] == FALSE &&
	       fPidSupported[0x87] == FALSE &&
	       fPidSupported[0x10] == FALSE &&
	       fPidSupported[0x66] == FALSE ) )
	{
		fReqPidNotSupported = TRUE;
		LogPrint("WARNING: SID $1 PID $0B, $87, $10 nor $66 supported (Support of one required for gasoline vehicles)\n");
	}

	/* Gasoline vehicles require Pid $0F or $68 */
	if ( gOBDDieselFlag == FALSE &&
	     ( fPidSupported[0x0F] == FALSE &&
	       fPidSupported[0x66] == FALSE ) )
	{
		fReqPidNotSupported = TRUE;
		LogPrint("WARNING: Neither SID $1 PID $0F nor $66 supported (Support of one required for gasoline vehicles)\n");
	}

	/* Gasoline vehicles require Pid $13 or $1D */
	if ( gOBDDieselFlag == FALSE &&
	     ( fPidSupported[0x13] == FALSE &&
	       fPidSupported[0x1D] == FALSE ) )
	{
		fReqPidNotSupported = TRUE;
		LogPrint("WARNING: Neither SID $1 PID $13 nor $1D supported (Support of one required for gasoline vehicles)\n");
	}

	/* Gasoline vehicles require Pid $13 or $1D */
	if ( gOBDList[gOBDListIndex].Protocol == ISO15765 &&
	     gOBDDieselFlag == FALSE &&
	     ( fPidSupported[0x45] == FALSE &&
	       fPidSupported[0x6C] == FALSE ) )
	{
		fReqPidNotSupported = TRUE;
		LogPrint( "FAILURE: Neither SID $1 PID $45 nor PID $6C supported (Support required for ISO15765 gasoline vehicles)\n", IdIndex );
		ERROR_RETURN;
	}

	for ( IdIndex = 0x01; IdIndex < MAX_PIDS; IdIndex++ )
	{
		if ( fPidSupported[IdIndex] == FALSE )
		{
			switch ( IdIndex )
			{
				case 0x01:
				case 0x04:
				case 0x0C:
				case 0x0D:
				case 0x1C:
				{
					/* Must be supported for all vehicles */
					fReqPidNotSupported = TRUE;
					LogPrint( "FAILURE: SID $1 PID $%02X not supported (Support required for all vehicles)\n", IdIndex );
					ERROR_RETURN;
				}
				break;

				case 0x03:
				case 0x06:
				case 0x07:
				case 0x0E:
				{
					/* Warn if not supported for gasoline vehicles */
					if ( gOBDDieselFlag == FALSE )
					{
						fReqPidNotSupported = TRUE;
						LogPrint( "WARNING: SID $1 PID $%02X not supported (Support required for gasoline vehicles)\n", IdIndex );
					}
				}
				break;

				case 0x11:
				{
					/* Must be supported for gasoline vehicles */
					if ( gOBDDieselFlag == FALSE )
					{
						fReqPidNotSupported = TRUE;
						LogPrint( "FAILURE: SID $1 PID $%02X not supported (Support required for gasoline vehicles)\n", IdIndex );
						ERROR_RETURN;
					}
				}
				break;

				case 0x1F:
				case 0x21:
				case 0x30:
				case 0x31:
				case 0x33:
				case 0x41:
				{
					/* Must be supported for ISO15765 protocol */
					if (gOBDList[gOBDListIndex].Protocol == ISO15765)
					{
						fReqPidNotSupported = TRUE;
						LogPrint( "FAILURE: SID $1 PID $%02X not supported (Support required for ISO15765 protocol vehicles)\n", IdIndex );
						ERROR_RETURN;
					}
				}
				break;

				case 0x2E:
				case 0x2F:
				{
					/* Warn if not supported for ISO15765 gasoline vehicles */
					if ( gOBDList[gOBDListIndex].Protocol == ISO15765 && gOBDDieselFlag == FALSE )
					{
						fReqPidNotSupported = TRUE;
						LogPrint( "WARNING: SID $1 PID $%02X not supported (Support required for ISO15765 gasoline vehicles)\n", IdIndex );
					}
				}
				break;

				case 0x42:
				case 0x43:
				case 0x44:
				{
					/* Must be supported for ISO15765 gasoline vehicles */
					if ( gOBDList[gOBDListIndex].Protocol == ISO15765 && gOBDDieselFlag == FALSE )
					{
						fReqPidNotSupported = TRUE;
						LogPrint( "FAILURE: SID $1 PID $%02X not supported (Support required for ISO15765 gasoline vehicles)\n", IdIndex );
						ERROR_RETURN;
					}
				}
				break;

				case 0x4D:
				case 0x4E:
				{
					/* Must be supported for ISO15765 gasoline vehicles */
					if ( gModelYear >= 2010 && gOBDDieselFlag == TRUE )
					{
						fReqPidNotSupported = TRUE;
						LogPrint( "WARNING: SID $1 PID $%02X not supported (Support required for MY2010 and beyond diesel vehicles)\n", IdIndex );
						ERROR_RETURN;
					}
				}
				break;

			}  /* end switch ( IdIndex ) */

		}  /* end if ( fPidSupported[IdIndex] == FALSE ) */

	}  /* end for ( IdIndex */


	// If there where any missing required PIDs, fail
	if ( ErrorCount() != ErrCntTmp )
	{
		ErrorFlags (ErrFlgTmp);  //reset the Error Flags
		LogPrint( "FAILURE: Not all required SID $1 PIDs are supported\n" );
		ERROR_RETURN;
	}
	ErrorFlags (ErrFlgTmp);  //reset the Error Flags


	if ( fReqPidNotSupported == FALSE )
	{
		LogPrint("INFORMATION: All required SID $1 PIDs supported!\n");
	}


	/* Try group support if ISO15765 */
	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{
		/*return(VerifyGroupDiagnosticSupport());*/
		/* 4/30/04: Link active test to verify communication remained active for ALL protocols
		 */
		if ( VerifyGroupDiagnosticSupport() == FAIL )
		{
			ERROR_RETURN;
		}
	}

	/* Link active test to verify communication remained active for ALL protocols
	 */
	if (VerifyLinkActive() != PASS)
	{
		ERROR_RETURN;
	}

	if ( ErrorCount() != 0 )
	{
		return(FAIL);
	}

	return(PASS);
}

/*
********************************************************************************
**	FUNCTION	VerifyM01P01
**
**	Purpose		Isolated function an align to specification SAEJ1699 rev 11.5.
********************************************************************************
**    DATE		MODIFICATION
**	  05/14/04  Created function / aligned test to SAEJ1699 rev 11.5.
********************************************************************************
*/
STATUS VerifyM01P01( SID1 *pSid1, unsigned long SidIndex, unsigned long EcuIndex )
{

	BOOL bTestFailed = FALSE;


	/* Check if the MIL light is ON (SID 1 PID 1 Data A Bit 7) */
	if (pSid1[SidIndex].Data[0] & 0x80)
	{
		if (gOBDDTCStored == FALSE)
		{
			/* MIL is ON when there should not be a stored DTC */
			LogPrint("FAILURE: ECU %X  MIL status failure.\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
	}

	/* Check if any DTC status bits (SID 1 PID 1 Data A Bits 0-6) */
	if (pSid1[SidIndex].Data[0] & 0x7F)
	{
		if (gOBDDTCStored == FALSE)
		{
			/*
			** DTC status bit(s) set when there should not
			** be a stored DTC
			*/
			LogPrint("FAILURE: ECU %X  DTC status failure.\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
	}

	/* Evaluate Data B, BIT 4 */
	if ( (pSid1[SidIndex].Data[1] & 0x01) == 0x00  ||    /* Check if ECU does NOT support misfire monitor */
	     gOBDDieselFlag == FALSE )                     /* Check for non-compression ignition            */
	{
		if ( ( pSid1[SidIndex].Data[1] & 0x10 ) != 0x00 )
		{
			LogPrint("FAILURE: ECU %X  Misfire Monitor must be '0' for spark ignition vehicles or controllers that do not support misfire.\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
	}

	/* Evaluate Data B, BIT 4 'or' condition. */
	if ( ( (pSid1[SidIndex].Data[1] & 0x01) == 0x01) &&     /* Check if ECU does NOT support misfire monitor */
		   (gOBDDieselFlag == TRUE) )                       /* Check for non-compression ignition            */
	{
		/* Bit 4 may be 0 or 1 for compression ignition w/Engine running. */
		if (TestPhase == eTestPerformanceCounters)
		{
			if ((pSid1[SidIndex].Data[1] & 0x10) != 0)
			{
				LogPrint("FAILURE: ECU %X  Misfire Monitor must be '0' for compression ignition vehicles.\n", GetEcuId(EcuIndex) );
				bTestFailed = TRUE;
			}
		}
		else if ( (gOBDEngineRunning == FALSE) &&  // test 5.6
			      ((pSid1[SidIndex].Data[1] & 0x10) != 0x10) )
		{
			LogPrint("FAILURE: ECU %X  Misfire Monitor must be '1' for compression ignition vehicles.\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
	}

	/* Evaluate Data B, BIT 5 */
	if ( (pSid1[SidIndex].Data[1] & 0x20 ) == 0x20)
	{
		LogPrint("FAILURE: ECU %X  Fuel system must indicate complete.\n", GetEcuId(EcuIndex) );
		bTestFailed = TRUE;
	}

	/* Evaluate Data B, BIT 6 */
	if ( (pSid1[SidIndex].Data[1] & 0x40) == 0x40 )
	{
		LogPrint("FAILURE: ECU %X  CCM must indicate complete.\n", GetEcuId(EcuIndex) );
		bTestFailed = TRUE;
	}

	/* Evaluate unsupported monitor status */
	/* If misfire unsupported it must indicate complete */
	if ( ( ( pSid1[SidIndex].Data[1] & 0x01 ) == 0x00 ) &&
		   ( pSid1[SidIndex].Data[1] & 0x10 ) == 0x10 )
	{
		LogPrint("FAILURE: ECU %X  Misfire test: unsupported monitor must indicate complete.\n", GetEcuId(EcuIndex) );
		bTestFailed = TRUE;
	}

	/* If fuel system monitor unsupported it must indicate complete */
	if ( ( ( pSid1[SidIndex].Data[1] & 0x02 ) == 0x00 ) &&
		   ( pSid1[SidIndex].Data[1] & 0x20 ) == 0x20 )
	{
		LogPrint("FAILURE: ECU %X  Fuel system: unsupported monitor must indicate complete.\n", GetEcuId(EcuIndex) );
		bTestFailed = TRUE;
	}

	/* If CCM monitor unsupported it must indicate complete */
	if ( ( ( pSid1[SidIndex].Data[1] & 0x04 ) == 0x00 ) &&
		   ( pSid1[SidIndex].Data[1] & 0x40 ) == 0x40 )
	{
		LogPrint("FAILURE: ECU %X  CCM unsupported monitor must indicate complete.\n", GetEcuId(EcuIndex) );
		bTestFailed = TRUE;
	}

	/* Check bit 3 state */
	/* only check if MY 2010 and beyond and ECU does not only supports CCM requirements (SID 1 PID 1 Data B bit 2==1) */
	if ( gModelYear >= 2010 &&
	     (Sid1Pid1[EcuIndex].Data[1] & 0x04) == 0 )
	{
		if ( (pSid1[SidIndex].Data[1] & 0x08) == 0 &&
		     gOBDDieselFlag == 1  )
		{
		    LogPrint("FAILURE: ECU %X  SID $1 PID $1 Data B bit 3 clear (Spark Ignition) does not match user input (Diesel)\n", GetEcuId(EcuIndex) );
		    bTestFailed = TRUE;
		}
		else if ( (pSid1[SidIndex].Data[1] & 0x08) == 0x08 &&
		     gOBDDieselFlag == 0  )
		{
		    LogPrint("FAILURE: ECU %X  SID $1 PID $1 Data B bit 3 set (Compression Ignition) does not match user input (Non Diesel)\n", GetEcuId(EcuIndex) );
		    bTestFailed = TRUE;
		}
	}

	/* Check if reserved bits 7 is set */
	if (pSid1[SidIndex].Data[1] & 0x80)
	{
		LogPrint("FAILURE: ECU %X  Reserved I/M readiness status bit 7 set\n", GetEcuId(EcuIndex) );
		bTestFailed = TRUE;
	}

	/* If ECU supports M01 P01 then at lease one bit must be set */
	if ( ( ( pSid1[SidIndex].Data[1] & 0x07 ) == 0x00 ) &&
		 ( ( pSid1[SidIndex].Data[2]        ) == 0x00 ) )
	{
		LogPrint("FAILURE: ECU %X  At lease one monitor must be supported by ECU!\n", GetEcuId(EcuIndex) );
		bTestFailed = TRUE;
	}

	if ( gOBDEngineRunning == FALSE )  // test 5.6
	{
		/* Don't check O2 sensor heater monitor */
		if ( (pSid1[SidIndex].Data[2] & ~0x40) != (pSid1[SidIndex].Data[3] & ~0x40))
		{
			LogPrint("FAILURE: ECU %X  Supported monitors are indicating 'complete' or unsupported monitors are indicating incomplete.\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
	}
	else  // test 5.10 & 10.12
	{
		/* Catalyst monitor */
		if  ( ( ( pSid1[SidIndex].Data[2] & 0x01 ) == 0x00 ) &&
				( pSid1[SidIndex].Data[3] & 0x01 ) == 0x01 )
		{
			LogPrint("FAILURE: ECU %X  Cat Mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}

		/* Heated CAT Mon   */
		if  ( ( ( pSid1[SidIndex].Data[2] & 0x02 ) == 0x00 ) &&
				( pSid1[SidIndex].Data[3] & 0x02 ) == 0x02 )
		{
			LogPrint("FAILURE: ECU %X  Heated Cat Mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}

		/* EVAP sys         */
		if ( ( ( pSid1[SidIndex].Data[2] & 0x04 ) == 0x00 ) &&
			   ( pSid1[SidIndex].Data[3] & 0x04 ) == 0x04 )
		{
			LogPrint("FAILURE: ECU %X  EVAP sys: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}

		/* Secondary AIR    */
		if ( ( ( pSid1[SidIndex].Data[2] & 0x08 ) == 0x00 ) &&
			   ( pSid1[SidIndex].Data[3] & 0x08 ) == 0x08 )
		{
			LogPrint("FAILURE: ECU %X  Secondary AIR: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}

		/* A/C Sys mon      */
		if ( ( ( pSid1[SidIndex].Data[2] & 0x10 ) == 0x00 ) &&
			   ( pSid1[SidIndex].Data[3] & 0x10 ) == 0x10 )
		{
			LogPrint("FAILURE: ECU %X  AC Sys mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}

		/* O2 sens mon      */
		if ( ( ( pSid1[SidIndex].Data[2] & 0x20 ) == 0x00 ) &&
			   ( pSid1[SidIndex].Data[3] & 0x20 ) == 0x20 )
		{
			LogPrint("FAILURE: ECU %X  O2 sens mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}

		/* O2 sens htr Mon  */
		if ( ( ( pSid1[SidIndex].Data[2] & 0x40 ) == 0x00 ) &&
			   ( pSid1[SidIndex].Data[3] & 0x40 ) == 0x40 )
		{
			LogPrint("FAILURE: ECU %X  O2 sens htr Mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}


		/* EGR sys mon      */
		if ( ( ( pSid1[SidIndex].Data[2] & 0x80 ) == 0x00 ) &&
			   ( pSid1[SidIndex].Data[3] & 0x80 ) == 0x80 )
		{
			LogPrint("FAILURE: ECU %X  EGR sys mon: Unsupported monitor must indicate 'complete'\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}

	}

	if (gOBDIMDriveCycle == TRUE)
	{
		if (pSid1[SidIndex].Data[3] != 0)
		{
			LogPrint("FAILURE: ECU %X  Supported monitors not complete after I/M drive cycle\n", GetEcuId(EcuIndex) );
			bTestFailed = TRUE;
		}
	}

	if (bTestFailed == TRUE)
	{
		return ( FAIL );
	}
	else
	{
		return ( PASS );
	}
}

/*
********************************************************************************
**	FUNCTION	VerifyIM_Ready
**
**	Purpose		Isolated function to request SID 1 PID 1 and check IM Ready
********************************************************************************
*/
STATUS VerifyIM_Ready (void)
{
	unsigned long EcuIndex;
	unsigned long SidIndex;

	SID_REQ SidReq;
	SID1 *pSid1;

	if (IsSid1PidSupported (-1, 1) == FALSE)
	{
		LogPrint("FAILURE: SID $1 PID $01 not supported\n");
		return (FAIL);
	}

	SidReq.SID = 1;
	SidReq.NumIds = 1;
	SidReq.Ids[0] = 1;
	if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
	{
		/* There must be a response for ISO15765 protocol */
		if (gOBDList[gOBDListIndex].Protocol == ISO15765)
		{
			LogPrint("FAILURE: SID $1 PID $01 request failed\n");
			return (FAIL);
		}
	}

	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		if (gOBDResponse[EcuIndex].Sid1PidSize != 0)
		{
			break;
		}
	}

	if (EcuIndex >= gOBDNumEcus)
	{
		LogPrint("FAILURE: No SID $1 PID $01 data\n");
		return (FAIL);
	}

	/* Verify that all SID 1 PID data is valid */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		/* If PID is supported, check it */
		if (IsSid1PidSupported (EcuIndex, 1) == TRUE)
		{
			/* Check the data to see if it is valid */
			pSid1 = (SID1 *)&gOBDResponse[EcuIndex].Sid1Pid[0];
			for (SidIndex = 0; SidIndex < (gOBDResponse[EcuIndex].Sid1PidSize / sizeof(SID1)); SidIndex++)
			{
				if ( VerifyM01P01( pSid1, SidIndex, EcuIndex ) != PASS )
				{
					return( FAIL );
				}
			}
		}
	}

	return (PASS);
}

/*
********************************************************************************
**	FUNCTION	RequestSID1SupportData
**
**	Purpose		Isolated function to identify support for SID 1 PID x
********************************************************************************
*/
STATUS RequestSID1SupportData (void)
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	unsigned long ulPIDSupport;	/* Evaluate $E0 PID support indication. */
	SID_REQ SidReq;

	/* Request SID 1 support data */
	for (IdIndex = 0x00; IdIndex < MAX_PIDS; IdIndex += 0x20)
	{
		SidReq.SID = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = (unsigned char)IdIndex;
		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			/* There must be a response to PID 0x00 */
			if (IdIndex == 0x00)
			{
				LogPrint("FAILURE: SID $1 support request failed\n");
				return (FAIL);
			}
		}

		/* Check if we need to request the next group */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid1PidSupport[IdIndex >> 5].IDBits[3] & 0x01)
			{
				break;
			}
		}
		if (EcuIndex >= gOBDNumEcus)
		{
			break;
		}
	}

	/* Enhance logic to verify support information if request is at upper limit of $E0 */
	if ( IdIndex == 0xE0 )
	{
		/* Init variable to no-support */
		ulPIDSupport = 0;

		/* For each ECU */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			/* If MID is supported, keep looking */
			if ( ( gOBDResponse[EcuIndex].Sid1PidSupport[IdIndex >> 5].IDBits[0]        ||
				   gOBDResponse[EcuIndex].Sid1PidSupport[IdIndex >> 5].IDBits[1]        ||
				   gOBDResponse[EcuIndex].Sid1PidSupport[IdIndex >> 5].IDBits[2]        ||
				(  gOBDResponse[EcuIndex].Sid1PidSupport[IdIndex >> 5].IDBits[3] & 0xFE ) ) != 0x00)
			{
				/* Flag as support indicated! */
				ulPIDSupport = 1;
			}
		}

		/* Flag as error if no support indicated in $E0 */
		if (ulPIDSupport == 0x00)
		{
			LogPrint("FAILURE: SID $1 PID $E0 support failure.  No PID support indicated!\n");
			return (FAIL);
		}
	}
	else
	{
		/*
		** Per J1699 rev 11.5 TC# 5.10.5 - Request request next
		** unsupported OBDMID-support OBDMID
		*/
		SidReq.SID = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = (unsigned char)IdIndex += 0x20;

		gIgnoreNoResponse = TRUE;

		if ( SidRequest(&SidReq, SID_REQ_NORMAL) == PASS )
		{
			/* J1850 & ISO9141 - No response preferred, but positive response
			** allowed
			*/
			if ( ( gOBDList[gOBDListIndex].Protocol == ISO15765 ) )
			{
				gIgnoreNoResponse = FALSE;
				LogPrint("FAILURE: TC# 5.10.5 - Unexpected response from ECU!\n");
				return (FAIL);
			}
		}

		gIgnoreNoResponse = FALSE;
	}

	return (PASS);
}

//*****************************************************************************
//
//	Function:	IsSid1PidSupported
//
//	Purpose:	Determine if SID 1 PID x is supported on specific ECU.
//              Need to have called RequestSID1SupportData() previously.
//              If EcuIndex < 0 then check all ECUs.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	02/10/05	Created common function for this logic.
//
//*****************************************************************************
unsigned int IsSid1PidSupported (unsigned int EcuIndex, unsigned int PidIndex)
{
	int index1;
	int index2;
	int mask;

	if (PidIndex == 0)
		return TRUE;            // all modules must support SID 01 PID 00

	PidIndex--;

	index1 =  PidIndex >> 5;
	index2 = (PidIndex >> 3) & 0x03;
	mask   = 0x80 >> (PidIndex & 0x07);

	if ((signed int)EcuIndex < 0)
	{
		for (EcuIndex = 0; EcuIndex < gUserNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid1PidSupport[index1].IDBits[index2] & mask)
				return TRUE;
		}
	}
	else
	{
		if (gOBDResponse[EcuIndex].Sid1PidSupport[index1].IDBits[index2] & mask)
			return TRUE;
	}

	return FALSE;
}

//*****************************************************************************
//
//	Function:	DetermineVariablePidSize
//
//	Purpose:	Determine number of data bytes in PIDs $06 - $09, $55 - $58
//
//*****************************************************************************
STATUS DetermineVariablePidSize (void)
{
	SID_REQ SidReq;

	SID1    *pPid1;
	unsigned char pid[OBD_MAX_ECUS];
	unsigned long EcuIdIndex[OBD_MAX_ECUS];

	unsigned long EcuIndex, numResp;

	/* only need to check once */
	if (gSid1VariablePidSize != 0)
		return PASS;

	/* -1 ==> cannot determine the PID size */
	gSid1VariablePidSize = -1;

	/* only check if needed */
	if ((IsSid1PidSupported (-1, 0x06) == FALSE) &&
		(IsSid1PidSupported (-1, 0x07) == FALSE) &&
		(IsSid1PidSupported (-1, 0x08) == FALSE) &&
		(IsSid1PidSupported (-1, 0x09) == FALSE) &&
		(IsSid1PidSupported (-1, 0x55) == FALSE) &&
		(IsSid1PidSupported (-1, 0x56) == FALSE) &&
		(IsSid1PidSupported (-1, 0x57) == FALSE) &&
		(IsSid1PidSupported (-1, 0x55) == FALSE) )
		return PASS;

	/* cannot support both PID $13 and $1D */
	if (IsSid1PidSupported (-1, 0x13) == TRUE &&
		IsSid1PidSupported (-1, 0x1D) == TRUE)
	{
		LogPrint ("FAILURE: Both PID $13 and $1D are supported\n");
		return FAIL;
	}

	/* check PID $13 first */
	if (IsSid1PidSupported (-1, 0x13) == TRUE)
	{
		SidReq.SID = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = 0x13;

		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			/* There must be a response for ISO15765 protocol */
			if (gOBDList[gOBDListIndex].Protocol == ISO15765)
			{
				LogPrint("FAILURE: SID $1 PID $13 request failed\n");
				return FAIL;
			}
		}

		numResp = 0;
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (IsSid1PidSupported (EcuIndex, 0x13) == TRUE)
			{
				if (gOBDResponse[EcuIndex].Sid1PidSize > 0)
				{
					pPid1 = (SID1 *)(gOBDResponse[EcuIndex].Sid1Pid);
					if (pPid1->PID == 0x13)
					{
						EcuIdIndex[numResp] = EcuIndex;
						pid[numResp++] = pPid1->Data[0];
					}
				}
			}
		}

		if (numResp == 0 || numResp > 2)
		{
			LogPrint("FAILURE: SID $1 PID $13 supported by %d ECUs\n", numResp);
			return FAIL;
		}

		gSid1VariablePidSize = 1;
		return PASS;
	}

	/* check PID $1D second */
	if (IsSid1PidSupported (-1, 0x1D) == TRUE)
	{
		SidReq.SID = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = 0x1D;

		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			/* There must be a response for ISO15765 protocol */
			if (gOBDList[gOBDListIndex].Protocol == ISO15765)
			{
				LogPrint("FAILURE: SID $1 PID $1D request failed\n");
				return FAIL;
			}
		}

		numResp = 0;
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (IsSid1PidSupported (EcuIndex, 0x1D) == TRUE)
			{
				if (gOBDResponse[EcuIndex].Sid1PidSize > 0)
				{
					pPid1 = (SID1 *)(gOBDResponse[EcuIndex].Sid1Pid);
					if (pPid1->PID == 0x1D)
					{
						EcuIdIndex[numResp] = EcuIndex;
						pid[numResp++] = pPid1->Data[0];
					}
				}
			}
		}

		if (numResp == 1)
		{
			gSid1VariablePidSize = (pid[0] == 0xff) ? 2 : 1;
			return PASS;
		}

		if (numResp == 2)
		{
			if ( (pid[0] | pid[1]) != 0xff)
			{
				gSid1VariablePidSize = 1;
				return PASS;
			}

			if ( (pid[0] == 0xf0 && pid[1] == 0x0f) ||
				 (pid[0] == 0x0f && pid[1] == 0xf0) )
			{
				gSid1VariablePidSize = 2;
				return PASS;
			}

			if ( (pid[0] == 0xcc && pid[1] == 0x33) ||
				 (pid[0] == 0x33 && pid[1] == 0xcc) )
			{
				gSid1VariablePidSize = 2;
				return PASS;
			}

			LogPrint ("FAILURE: Unable to determine number of data bytes in PIDs $06 - $09\n");
			LogPrint ("INFORMATION: ECU %X  SID $01 PID $1D -> %02X\n", GetEcuId (EcuIdIndex[0]), pid[0]);
			LogPrint ("INFORMATION: ECU %X  SID $01 PID $1D -> %02X\n", GetEcuId (EcuIdIndex[1]), pid[1]);
			return FAIL;
		}

		/* numResp == 0 or numResp > 2 */
		LogPrint("FAILURE:  SID $1 PID $1D supported by %d ECUs\n", numResp);
		return FAIL;
	}

	LogPrint ("FAILURE: Neither SID $1 PID $13 nor $1D are supported\n");
	return FAIL;
}

//*****************************************************************************
//
//	Function:	GetPid4FArray
//
//	Purpose:	copy to PID $4F values into an array.
//
//*****************************************************************************
STATUS GetPid4FArray (void)
{
	SID_REQ SidReq;
	SID1 *pPid1;
	unsigned int EcuIndex;

	if (IsSid1PidSupported (-1, 0x4F) == TRUE)
	{
		SidReq.SID = 1;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = 0x4F;

		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			LogPrint("FAILURE: SID $1 PID $4F request failed\n");
			return (FAIL);
		}

		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (IsSid1PidSupported (EcuIndex, 0x4F) == TRUE)
			{
				if (gOBDResponse[EcuIndex].Sid1PidSize > 0)
				{
					pPid1 = (SID1 *)(gOBDResponse[EcuIndex].Sid1Pid);
					if (pPid1->PID == 0x4F)
					{
						Pid4F[EcuIndex][0] = pPid1->Data[0];
						Pid4F[EcuIndex][1] = pPid1->Data[1];
						Pid4F[EcuIndex][2] = pPid1->Data[2];
						Pid4F[EcuIndex][3] = pPid1->Data[3];
					}
				}
			}
		}
	}
	return (PASS);
}
