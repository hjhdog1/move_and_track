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

int          VerifySid6PidSupportData (void);
STATUS       RequestSID6SupportData (void);
unsigned int IsSid6MidSupported (unsigned int EcuIndex, unsigned int MidIndex);



/*
*******************************************************************************
** VerifyMonitorTestSupportAndResults -
** Function to verify SID6 monitor test support and results
**
**  DATE		MODIFICATION
**  07/15/04    Correct logic error associated with the evaluation of $E0
**              support.
*******************************************************************************
*/
STATUS VerifyMonitorTestSupportAndResults(void)
{
	unsigned long  EcuIndex;
	unsigned long  IdIndex;
	unsigned long  SidIndex;
	unsigned short u_tmp;
	signed short   s_tmp;
	long           TestValue;
	long           TestLimitMax;
	long           TestLimitMin;

	unsigned char  fError = FALSE;              // set if an error occured during test
	unsigned char  fSparkIgnMonSup = FALSE;     // set if Spark Ignition Monitor is supported (otherwise Compression)

	unsigned char  fDataBBit0Supported = FALSE; // set if SID1 PID1 Data B bit 0 is supported by the vehicle
	unsigned char  fDataCBit0Supported = FALSE; // set if SID1 PID1 Data C bit 0 is supported by the vehicle
	unsigned char  fDataCBit1Supported = FALSE; // set if SID1 PID1 Data C bit 1 is supported by the vehicle
	unsigned char  fDataCBit2Supported = FALSE; // set if SID1 PID1 Data C bit 2 is supported by the vehicle
	unsigned char  fDataCBit3Supported = FALSE; // set if SID1 PID1 Data C bit 3 is supported by the vehicle
	unsigned char  fDataCBit4Supported = FALSE; // set if SID1 PID1 Data C bit 4 is supported by the vehicle
	unsigned char  fDataCBit5Supported = FALSE; // set if SID1 PID1 Data C bit 5 is supported by the vehicle
	unsigned char  fDataCBit6Supported = FALSE; // set if SID1 PID1 Data C bit 6 is supported by the vehicle
	unsigned char  fDataCBit7Supported = FALSE; // set if SID1 PID1 Data C bit 7 is supported by the vehicle

	unsigned char  f01Supported = FALSE;        // set if MID 01 is supported by the vehicle
	unsigned char  f21Supported = FALSE;        // set if MID 21 is supported by the vehicle
	unsigned char  fA20BSupported = FALSE;      // set if MID A2 SDTID 0B is supported by the vehicle
	unsigned char  fA20CSupported = FALSE;      // set if MID A2 SDTID 0C is supported by the vehicle
	unsigned char  fB2Supported = FALSE;        // set if MID B2 is supported by the vehicle

	unsigned char  f01_10Supported = FALSE;     // set if a MID between 01 and 10 is supported by the ECU
	unsigned char  f21_24Supported = FALSE;     // set if a MID between 21 and 24 is supported by the ECU
	unsigned char  f31_38Supported = FALSE;     // set if a MID between 31 and 38 is supported by the ECU
	unsigned char  f39_3DSupported = FALSE;     // set if a MID between 39 and 3D is supported by the ECU
	unsigned char  f61_64Supported = FALSE;     // set if a MID between 61 and 64 is supported by the ECU
	unsigned char  f71_74Supported = FALSE;     // set if a MID between 71 and 74 is supported by the ECU
	unsigned char  f81_84Supported = FALSE;     // set if a MID between 81 and 84 is supported by the ECU
	unsigned char  fA1_B1Supported = FALSE;     // set if a MID between A1 and B1 is supported by the ECU
	unsigned char  fB2_B3Supported = FALSE;     // set if a MID between B2 and B3 is supported by the ECU
	unsigned char  f85_86Supported = FALSE;     // set if a MID between 85 and 86 is supported by the ECU
	unsigned char  f90919899Supported = FALSE;  // set if MID 90,91,98 or 99 is supported by the ECU

	SID_REQ        SidReq;
	SID6          *pSid6;

	// Clear Error Count for ERROR_RETURN
	ErrorCount();

	/* Request SID 6 support data */
	if (RequestSID6SupportData() != PASS)
		return FAIL;

	/* Verify that all SID 6 test data is reset or within limits */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		/* Test 5.14.3 */
		if ( gOBDList[gOBDListIndex].Protocol == ISO15765 &&
		     TestPhase == eTestNoDTC && TestSubsection == 14 )
		{
			// if Spark Ignition Module (SID $1 PID $1 Data B bit 3 == 0)
			if ( (Sid1Pid1[EcuIndex].Data[1] & 0x08) == 0x00 )
			{
				fSparkIgnMonSup = TRUE;
			}
			else
			{
				fSparkIgnMonSup = FALSE;
			}


			// if ECU does not only support CCM requirements (SID 1 PID 1 Data B bit 2 == 1)
			if ( (Sid1Pid1[EcuIndex].Data[1] & 0x04) == 0 )
			{
				/* Test SID 1 PID 1 Data B bit 3 for each ECU */
				if ( gModelYear >= 2010 )
				{
					if ( gOBDDieselFlag == TRUE && fSparkIgnMonSup == TRUE )
					{
						LogPrint("FAILURE: ECU %X  SID $1 PID $1 Data B bit 3 must = 1 for Compression Ignition Vehicles\n", GetEcuId(EcuIndex) );
						fError = TRUE;
					}
					else if ( gOBDDieselFlag == FALSE && fSparkIgnMonSup == FALSE )
					{
						LogPrint("FAILURE: ECU %X  SID $1 PID $1 Data B bit 3 must = 0 for Spark Ignition Vehicles\n", GetEcuId(EcuIndex) );
						fError = TRUE;
					}
				}
				// prior to MY 2010, warn for compression only
				else
				{
					if ( gOBDDieselFlag == TRUE && fSparkIgnMonSup == TRUE )
					{
						LogPrint("WARNING: ECU %X  SID $1 PID $1 Data B bit 3 must = 1 for Compression Ignition Vehicles\n", GetEcuId(EcuIndex) );
						ERROR_RETURN;
					}
				}
			}


			if ( (Sid1Pid1[EcuIndex].Data[2] & 0x10) != 0 )
			{
				LogPrint("FAILURE: ECU %X  SID $1 PID $01 Data C bit 4 is set (Must be 0 for All Vehicles)\n", GetEcuId(EcuIndex) );
				fError = TRUE;
			}

			if ( fSparkIgnMonSup == FALSE && (Sid1Pid1[EcuIndex].Data[2] & 0x04) != 0 )
			{
				LogPrint("FAILURE: ECU %X  SID $1 PID $1 Data C bit 2 is set (Must be 0 for Compression Ignition Vehicles)\n", GetEcuId(EcuIndex) );
				fError = TRUE;
			}

			/* Test Sid1 Data B bit 1 for the vehicle */
			if ( Sid1Pid1[EcuIndex].Data[1] & 0x01 )
			{
				fDataBBit0Supported = TRUE;
			}

			/* Test Sid1 Data C bit 0 for the vehicle */
			if ( Sid1Pid1[EcuIndex].Data[2] & 0x01 )
			{
				fDataCBit0Supported = TRUE;
			}

			/* Test Sid1 Data C bit 1 for the vehicle */
			if ( Sid1Pid1[EcuIndex].Data[2] & 0x02 )
			{
				fDataCBit1Supported = TRUE;
			}

			/* Test Sid1 Data C bit 2 for the vehicle */
			if ( Sid1Pid1[EcuIndex].Data[2] & 0x04 )
			{
				fDataCBit2Supported = TRUE;
			}

			/* Test Sid1 Data C bit 3 for the vehicle */
			if ( Sid1Pid1[EcuIndex].Data[2] & 0x08 )
			{
				fDataCBit3Supported = TRUE;
			}

			/* Test Sid1 Data C bit 5 for the vehicle */
			if ( Sid1Pid1[EcuIndex].Data[2] & 0x20 )
			{
				fDataCBit5Supported = TRUE;
			}

			/* Test Sid1 Data C bit 6 for the vehicle */
			if ( Sid1Pid1[EcuIndex].Data[2] & 0x40 )
			{
				fDataCBit6Supported = TRUE;
			}

			/* Test Sid1 Data C bit 7 for the vehicle */
			if ( Sid1Pid1[EcuIndex].Data[2] & 0x80 )
			{
				fDataCBit7Supported = TRUE;
			}

		} /* end Test5.14.3 */

		/* For each MID group */
		for (IdIndex = 0x01; IdIndex < 0x100; IdIndex++)
		{
			/* skip PID supported PIDs */
			if (IdIndex == 0x20 || IdIndex == 0x40 || IdIndex == 0x60 || IdIndex == 0x80 ||
			    IdIndex == 0xA0 || IdIndex == 0xC0 || IdIndex == 0xE0)
				continue;

			/* If MID is supported, request it */
			if (IsSid6MidSupported (EcuIndex, IdIndex) == TRUE)
			{
				SidReq.SID = 6;
				SidReq.NumIds = 1;
				SidReq.Ids[0] = (unsigned char)IdIndex;
				if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
				{
					LogPrint("FAILURE: ECU %X  SID $6 MID $%02X request failed\n", GetEcuId(EcuIndex) , IdIndex);
					fError = TRUE;
					continue;
				}

				if (gOBDResponse[EcuIndex].Sid6MidSize == 0)
				{
					LogPrint("FAILURE: ECU %X  No SID $6 MID $%02X data\n", GetEcuId(EcuIndex) , IdIndex);
					fError = TRUE;
					continue;
				}

				/* Check the data that should be reset and / or within limits */
				pSid6 = (SID6 *)&gOBDResponse[EcuIndex].Sid6Mid[0];
				for (SidIndex = 0; SidIndex < (gOBDResponse[EcuIndex].Sid6MidSize / sizeof(SID6)); SidIndex++)
				{
					/*
					** If ISO15765 protocol...
					*/
					if ( gOBDList[gOBDListIndex].Protocol == ISO15765 )
					{
						/* IF not test 11.4 (tests 5.5, 5.14 and 10.6)*/
						if ( gOBDIMDriveCycle == FALSE )
						{
							/* If MID 0x01 - 0x10 TID 1 - 4 OR */
							/* Test 5.14.3 AND MID $A1 - $B1, don't check for reset */
							/* Otherwise, values should be zero after a code clear */
							if ( !(pSid6[SidIndex].OBDMID <= 0x10 && pSid6[SidIndex].SDTID <= 4) &&
							     !( TestPhase == eTestNoDTC && TestSubsection == 14 &&
							        (pSid6[SidIndex].OBDMID >= 0xA1 && pSid6[SidIndex].OBDMID <= 0xB1) ) )
							{
								if (pSid6[SidIndex].TVHI    != 0 || pSid6[SidIndex].TVLO    != 0 ||
									pSid6[SidIndex].MINTLHI != 0 || pSid6[SidIndex].MINTLLO != 0 ||
									pSid6[SidIndex].MAXTLHI != 0 || pSid6[SidIndex].MAXTLLO != 0)
								{
									LogPrint("WARNING: ECU %X  SID $6 MID %02X test value/limits not reset\n", GetEcuId(EcuIndex) , pSid6[SidIndex].OBDMID);
								}
							}


							/* Test 5.14.3 */
							if ( TestPhase == eTestNoDTC && TestSubsection == 14 )
							{
								/* Check for prohibited MIDs */
								if ( ( pSid6[SidIndex].OBDMID >= 0x11 && pSid6[SidIndex].OBDMID <= 0x1F ) ||
								     ( pSid6[SidIndex].OBDMID >= 0x25 && pSid6[SidIndex].OBDMID <= 0x30 ) ||
								     pSid6[SidIndex].OBDMID == 0x3E || pSid6[SidIndex].OBDMID == 0x3F ||
								     ( pSid6[SidIndex].OBDMID >= 0x51 && pSid6[SidIndex].OBDMID <= 0x5F ) ||
								     ( pSid6[SidIndex].OBDMID >= 0x65 && pSid6[SidIndex].OBDMID <= 0x70 ) ||
								     ( pSid6[SidIndex].OBDMID >= 0x75 && pSid6[SidIndex].OBDMID <= 0x7F ) ||
								     ( pSid6[SidIndex].OBDMID >= 0x87 && pSid6[SidIndex].OBDMID <= 0x8F ) ||
								     ( pSid6[SidIndex].OBDMID >= 0x92 && pSid6[SidIndex].OBDMID <= 0x97 ) ||
								     ( pSid6[SidIndex].OBDMID >= 0x9A && pSid6[SidIndex].OBDMID <= 0x9F ) ||
								     ( pSid6[SidIndex].OBDMID >= 0xB4 && pSid6[SidIndex].OBDMID <= 0xBF ) ||
								     ( pSid6[SidIndex].OBDMID >= 0xC1 && pSid6[SidIndex].OBDMID <= 0xDF ) ||
								     ( pSid6[SidIndex].OBDMID >= 0xE1 && pSid6[SidIndex].OBDMID <= 0xFF ) )
								{
									LogPrint( "FAILURE: ECU %X  SID $6 MID $%02X support prohibited\n", GetEcuId(EcuIndex) , pSid6[SidIndex].OBDMID );
									fError = TRUE;
								}

								/* Check MID ranges common to spark and Compression Ignition vehicles */
								else if ( pSid6[SidIndex].OBDMID <= 0x10 )
								{
									f01_10Supported = TRUE;

									/* Test 5.14.3 - MID $01 must be supported by all vehicles */
									if ( pSid6[SidIndex].OBDMID == 0x01 )
									{
										f01Supported = TRUE;
									}
								}

								else if ( pSid6[SidIndex].OBDMID >= 0x31 && pSid6[SidIndex].OBDMID <= 0x38 )
								{
									f31_38Supported = TRUE;
								}

								else if ( pSid6[SidIndex].OBDMID >= 0x81 && pSid6[SidIndex].OBDMID <= 0x84 )
								{
									f81_84Supported = TRUE;
								}

								/* Check MID ranges for Spark Ignition Vehicles */
								else if ( fSparkIgnMonSup == TRUE )
								{
									if ( pSid6[SidIndex].OBDMID >= 0x21 && pSid6[SidIndex].OBDMID <= 0x24 )
									{
										f21_24Supported = TRUE;

										if ( pSid6[SidIndex].OBDMID == 0x21 )
										{
											f21Supported = TRUE;
										}
									}

									else if ( pSid6[SidIndex].OBDMID >= 0x39 && pSid6[SidIndex].OBDMID <= 0x3D )
									{
										f39_3DSupported = TRUE;
									}

									else if ( pSid6[SidIndex].OBDMID >= 0x61 && pSid6[SidIndex].OBDMID <= 0x64 )
									{
										f61_64Supported = TRUE;
									}

									else if ( pSid6[SidIndex].OBDMID >= 0x71 && pSid6[SidIndex].OBDMID <= 0x74 )
									{
										f71_74Supported = TRUE;
									}
								}

								/* Check MID ranges for Compression Ignition Vehicles */
								else
								{
									if ( pSid6[SidIndex].OBDMID >= 0x21 && pSid6[SidIndex].OBDMID <= 0x24 )
									{
										f21_24Supported = TRUE;
									}

									else if ( pSid6[SidIndex].OBDMID >= 0x85 && pSid6[SidIndex].OBDMID <= 0x86 )
									{
										f85_86Supported = TRUE;
									}

									else if ( pSid6[SidIndex].OBDMID == 0x90 ||
									          pSid6[SidIndex].OBDMID == 0x91 ||
									          pSid6[SidIndex].OBDMID == 0x98 ||
									          pSid6[SidIndex].OBDMID == 0x99 )
									{
										f90919899Supported = TRUE;
									}

									if ( pSid6[SidIndex].OBDMID >= 0xB2 && pSid6[SidIndex].OBDMID <= 0xB3 )
									{
										fB2_B3Supported = TRUE;

										if ( pSid6[SidIndex].OBDMID == 0xB2 )
										{
											fB2Supported = TRUE;
										}
									}
								}

							}  /* end Test 5.14.3 */

						}  /* end if (gOBDIMDriveCycle == FALSE) */

						/* Tests 5.5, 5.14, 10.6 and 11.4 */
						/* If MID $A1 thru $B1, MID $A2 SDTID $0B or $0C must be supported */
						if ( pSid6[SidIndex].OBDMID >= 0xA1 && pSid6[SidIndex].OBDMID <= 0xB1 )
						{
							fA1_B1Supported = TRUE;

							if ( pSid6[SidIndex].OBDMID == 0xA2 )
							{
								if ( pSid6[SidIndex].SDTID == 0x0B )
								{
									fA20BSupported = TRUE;
								}

								if ( pSid6[SidIndex].SDTID == 0x0C)
								{
									fA20CSupported = TRUE;
								}
							}
						}

					}  /* end if (gOBDList[gOBDListIndex].Protocol == ISO15765) */

					/* IF protocol other than ISO15765 OR in drive cycle (Test 11.4) */
					if ( gOBDList[gOBDListIndex].Protocol != ISO15765 ||
					     gOBDIMDriveCycle == TRUE )
					{
						/* Check the value against the limits */
						if ( pSid6[SidIndex].UASID & 0x80 )
						{
							/*
							** Signed values
							*/
							s_tmp = (pSid6[SidIndex].TVHI << 8) + pSid6[SidIndex].TVLO;
							TestValue = s_tmp;

							s_tmp = (pSid6[SidIndex].MINTLHI << 8) + pSid6[SidIndex].MINTLLO;
							TestLimitMin = s_tmp;

							s_tmp = (pSid6[SidIndex].MAXTLHI << 8 ) + pSid6[SidIndex].MAXTLLO;
							TestLimitMax = s_tmp;
						}
						else
						{
							/*
							** Unsigned values
							*/
							u_tmp = (pSid6[SidIndex].TVHI << 8) + pSid6[SidIndex].TVLO;
							TestValue = u_tmp;

							u_tmp = (pSid6[SidIndex].MINTLHI << 8) + pSid6[SidIndex].MINTLLO;
							TestLimitMin = u_tmp;

							u_tmp = (pSid6[SidIndex].MAXTLHI << 8 ) + pSid6[SidIndex].MAXTLLO;
							TestLimitMax = u_tmp;
						}

						if (TestValue < TestLimitMin)
						{
							LogPrint("FAILURE: ECU %X  SID $6 MID %02X test value exceeded min\n", GetEcuId(EcuIndex) , pSid6[SidIndex].OBDMID);
							fError = TRUE;
						}

						if (TestValue > TestLimitMax)
						{
							LogPrint("FAILURE: ECU %X  SID $6 MID %02X test value exceeded max\n", GetEcuId(EcuIndex) , pSid6[SidIndex].OBDMID);
							fError = TRUE;
						}
					} /* end protocol other than ISO15765 */

				} /* end for (SidIndex . . . */

			} /* end if (IsSid6MidSupported (EcuIndex, IdIndex) == TRUE) */

		} /* end for (IdIndex . . . */

	} /* end for (EcuIndex . . . */


	/* Check for vehicle required MIDs */
	if ( gOBDList[gOBDListIndex].Protocol == ISO15765 )
	{
		/* Test 5.14.3 */
		if ( TestPhase == eTestNoDTC && TestSubsection == 14 )
		{
			if ( fDataBBit0Supported == TRUE &&
			     ( fA20BSupported == FALSE ||
			       fA20CSupported == FALSE ) )
			{
				LogPrint("FAILURE: SID $6 MID $A2 SDTID $0B or $0C not supported (Required for All Vehicles with SID $1 PID $1 Data B bit 0 set)\n");
				fError = TRUE;
			}

			if ( fDataCBit5Supported == TRUE && f01Supported == FALSE )
			{
				LogPrint("FAILURE: SID $6 MID $01 not supported (Required for All Vehicles with SID $1 PID $1 Data C bit 5 set)\n");
				fError = TRUE;
			}

			if ( fDataCBit7Supported == TRUE && f31_38Supported == FALSE )
			{
				LogPrint("FAILURE: SID $6 MID $31-$38 not supported (At least one required for All Vehicles with SID $1 PID $1 Data C bit 7 set)\n" );
				fError = TRUE;
			}

			if ( gOBDDieselFlag == FALSE )
			{
				if ( fDataCBit0Supported == TRUE && f21Supported == FALSE )
				{
					LogPrint("FAILURE: SID $6 MID $21 not supported (Required for Spark Ignition Vehicles with SID $1 PID $1 Data C bit 0 set)\n");
					fError = TRUE;
				}

				if ( fDataCBit1Supported == TRUE && f61_64Supported == FALSE )
				{
					LogPrint("FAILURE: SID $6 MID $61-$64 not supported (At least one required for Spark Ignition Vehicles with SID $1 PID $1 Data C bit 1 set)\n" );
					fError = TRUE;
				}

				if ( fDataCBit2Supported == TRUE != 0 && f39_3DSupported == FALSE )
				{
					LogPrint("FAILURE: SID $6 MID $39-$3D not supported (At least one required for Spark Ignition Vehicles with SID $1 PID $1 Data C bit 2 set)\n" );
					fError = TRUE;
				}

				if ( fDataCBit3Supported == TRUE != 0 && f71_74Supported == FALSE )
				{
					LogPrint("FAILURE: SID $6 MID $71-$74 not supported (At least one required for Spark Ignition Vehicles with SID 1 PID 1 Data C bit 3 set)\n" );
					fError = TRUE;
				}
			}

			else
			{
				if ( fDataCBit6Supported == TRUE && fB2Supported == FALSE )
				{
					LogPrint("FAILURE: SID $6 MID $B2 not supported (Required for Compression Ignition Vehicles with SID $1 PID $1 Data C bit 6 set)\n");
					fError = TRUE;
				}

				if ( fDataCBit1Supported == TRUE && f90919899Supported == FALSE )
				{
					LogPrint("FAILURE: SID $6 MID $90,$91,$98,$99 not supported (At least one required for Compression Ignition Vehicles with SID $1 PID $1 Data C bit 1 set)\n" );
					fError = TRUE;
				}

				if ( fDataCBit3Supported == TRUE && f85_86Supported == FALSE )
				{
					LogPrint("FAILURE: SID $6 MID $85-$86 not supported (At least one required for Compression Ignition Vehicles with SID $1 PID $1 Data C bit 3 set)\n" );
					fError = TRUE;
				}
			}
		}  // end Test 5.14

		else if ( fA20BSupported == FALSE ||
		          fA20CSupported == FALSE )
		{
			LogPrint("FAILURE: SID $6 MID $A2 SDTID $0B or $0C not supported by the vehicle\n");
			fError = TRUE;
		}
	}

	/* if any required MIDs not supported OR a reserved bit is set, alert user of failure */
	if ( fError == TRUE)
	{
		ERROR_RETURN;
	}

	/*
	** If ISO15765 protocol, make sure the required OBDMID/SDTID values are supported
	** and try group support
	*/
	if (gOBDList[gOBDListIndex].Protocol == ISO15765)
	{
		if (gOBDIMDriveCycle == FALSE)
		{
			/* 4/30/04 - Restructure logic to allow for Link Active test after completion of
			 *           group request
			*/
			if(VerifyGroupMonitorTestSupport()==FAIL)
			{
				return(FAIL);
			}
		}
	}

	if ( ErrorCount() != 0 )
	{
		return(FAIL);
	}

	return(PASS);
}

/*
*******************************************************************************
**	Function:	VerifySid6PidSupportData
**
**	Purpose:	Verify each controller supports at a minimum one PID.
**              Any ECU that responds that does not support at least
**              one PID is flagged as an error.
**
*******************************************************************************
*/
int VerifySid6PidSupportData (void)
{
	int             bReturn = PASS;
	int             bEcuResult;
	unsigned long	EcuIndex;
	unsigned long   Index;

	/* For each ECU */
	for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
	{
		bEcuResult = FAIL;
		for (Index = 0; Index < gOBDResponse[EcuIndex].Sid6MidSupportSize; Index++)
		{
			/* If MID is supported, keep looking */
			if ( ( gOBDResponse[EcuIndex].Sid6MidSupport[Index].IDBits[0]		||
			       gOBDResponse[EcuIndex].Sid6MidSupport[Index].IDBits[1]		||
			       gOBDResponse[EcuIndex].Sid6MidSupport[Index].IDBits[2]		||
			     ( gOBDResponse[EcuIndex].Sid6MidSupport[Index].IDBits[3] & 0xFE ) ) != 0x00)
			{
				bEcuResult = PASS;
				break;
			}
		}

		if ((bEcuResult == FAIL) && (gOBDResponse[EcuIndex].Sid6MidSupportSize > 0))
		{
			LogPrint ("INFORMATION: ECU %X  SID $6 invalid PID supported PIDs", GetEcuId(EcuIndex));
			bReturn = FAIL;
		}
	}

	return bReturn;
}

//*****************************************************************************
//
//	Function:	RequestSID6SupportData
//
//	Purpose:	Purpose of this routine is to verify that SID 6 PID 00
//              returns a support record. Continue requesting support
//              PIDs thru the highest supported group.
//
//*****************************************************************************
STATUS RequestSID6SupportData (void)
{
	unsigned long EcuIndex;
	unsigned long IdIndex;
	unsigned long ulMidSupport;  /* used to determine $E0 support indication */

	SID_REQ       SidReq;

	/* Request SID 6 support data */
	for (IdIndex = 0x00; IdIndex < 0x100; IdIndex += 0x20)
	{
		SidReq.SID = 6;
		SidReq.NumIds = 1;
		SidReq.Ids[0] = (unsigned char)IdIndex;
		if (SidRequest(&SidReq, SID_REQ_NORMAL) != PASS)
		{
			/* There must be a response to MID 0x00 */
			if (IdIndex == 0x00)
			{
				LogPrint("FAILURE: SID $6 support request failed\n");
				return FAIL;
			}
		}

		/* Check if we need to request the next group */
		for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
		{
			if (gOBDResponse[EcuIndex].Sid6MidSupport[IdIndex >> 5].IDBits[3] & 0x01)
			{
				break;
			}
		}
		if (EcuIndex >= gOBDNumEcus)
		{
			break;
		}
	}

	/* Flag error if ECU indicates no support */
	if (VerifySid6PidSupportData() == FAIL)
	{
		ERROR_RETURN;
	}

	if (gOBDIMDriveCycle == FALSE)
	{
		/* Enhance logic to verify support information if request is at upper limit of $E0 */
		if (IdIndex == 0xE0)
		{
			/* Init variable to no-support */
			ulMidSupport = 0;

			/* For each ECU */
			for (EcuIndex = 0; EcuIndex < gOBDNumEcus; EcuIndex++)
			{
				/* If MID is supported, keep looking */
				if ( ( gOBDResponse[EcuIndex].Sid6MidSupport[IdIndex >> 5].IDBits[0]		||
				       gOBDResponse[EcuIndex].Sid6MidSupport[IdIndex >> 5].IDBits[1]		||
				       gOBDResponse[EcuIndex].Sid6MidSupport[IdIndex >> 5].IDBits[2]		||
				     ( gOBDResponse[EcuIndex].Sid6MidSupport[IdIndex >> 5].IDBits[3] & 0xFE ) ) != 0x00)
				{
					/* Flag as support indicated! */
					ulMidSupport = 1;
				}
			}

			/* If no ECU indicated support, flag as error. */
			if ( ulMidSupport == 0 )
			{
				LogPrint("FAILURE: SID $6 MID $E0 support failure.  No OBDMID support indicated!\n");
				ERROR_RETURN;
			}
		}
		else
		{
			/*
			** Per J1699 rev 11.5 TC# 5.14.5 - Request request next
			**              unsupported OBDMID-support OBDMID
			*/
			SidReq.SID = 6;
			SidReq.NumIds = 1;
			SidReq.Ids[0] = (unsigned char)IdIndex += 0x20;

			gIgnoreNoResponse = TRUE;

			if ( SidRequest(&SidReq, SID_REQ_NORMAL) == PASS )
			{
				if ( gOBDList[gOBDListIndex].Protocol == ISO15765 )
				{
					LogPrint("FAILURE: TC# 5.14.5 - Unexpected response from ECU!\n");
					gIgnoreNoResponse = FALSE;
					ERROR_RETURN;	/* 8/9/04 - Allow user to continue */
				}
			}

			gIgnoreNoResponse = FALSE;
		}

		/*
		** Per J1699 rev 11.5 TC# 5.14.5 - Verify ECU did not
		** drop out.
		*/
		if (VerifyLinkActive() != PASS)
		{
			return FAIL;
		}
	}

	return PASS;
}

/*
*******************************************************************************
**	Function:	IsSid6MidSupported
**
**	Purpose:	Determine if SID 6 MID is supported on specific ECU.
**              If EcuIndex < 0 then check all ECUs.
**
*******************************************************************************
*/
unsigned int IsSid6MidSupported (unsigned int EcuIndex, unsigned int MidIndex)
{
	unsigned int index0;
	unsigned int index1;
	unsigned int index2;
	unsigned int mask;

	if (MidIndex == 0)
		return TRUE;            // all modules must support SID 06 MID 00

	MidIndex--;

	index1 =  MidIndex >> 5;
	index2 = (MidIndex >> 3) & 0x03;
	mask   = 0x80 >> (MidIndex & 0x07);

	if ((signed int)EcuIndex < 0)
	{
		for (index0 = 0; index0 < gUserNumEcus; index0++)
		{
			if (gOBDResponse[index0].Sid6MidSupport[index1].IDBits[index2] & mask)
				return TRUE;
		}
	}
	else
	{
		if (gOBDResponse[EcuIndex].Sid6MidSupport[index1].IDBits[index2] & mask)
			return TRUE;
	}

	return FALSE;
}

