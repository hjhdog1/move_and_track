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
** SidResetResponseData - Function to reset SID response data
*******************************************************************************
*/
STATUS SidResetResponseData(PASSTHRU_MSG *TxMsg)
{
	unsigned long HeaderSize;
	unsigned long EcuIndex;

	/* Set the response header size based on the protocol */
	HeaderSize = gOBDList[gOBDListIndex].HeaderSize;

	/* Reset SID response data for all ECUs */
	for (EcuIndex = 0; EcuIndex < OBD_MAX_ECUS; EcuIndex++)
	{
		/* Save the data in the appropriate SID/PID/MID/TID/InfoType */
		switch(TxMsg->Data[HeaderSize])
		{
			/* SID 1 (Mode 1) */
			case 0x01:
			{
				switch (TxMsg->Data[HeaderSize + 1])
				{
					case 0x00:
					{
						/******************************************************
						* Partial fix for SF# 1719664
						* DO NOT uncomment until remainer of bug is fixed
						if ( !gVerifyLink )
						{
						******************************************************/
							/* Clear out the support data when PID 0x00 is requested */
							gOBDResponse[EcuIndex].Sid1PidSupportSize = 0;

						/******************************************************
						* Partial fix for SF# 1719664
						* DO NOT uncomment until remainer of bug is fixed
							memset( gOBDResponse[EcuIndex].Sid1PidSupport,
							        0,
							        sizeof(gOBDResponse[EcuIndex].Sid1PidSupport) );
						}
						******************************************************/
					}

					default:
					{
						/* All other requests reset the PID data */
						gOBDResponse[EcuIndex].Sid1PidSize = 0;
						memset( gOBDResponse[EcuIndex].Sid1Pid,
						        0,
						        sizeof(gOBDResponse[EcuIndex].Sid1Pid) );
					}
					break;
				}
			}
			break;
			/* SID 2 (Mode 2) */
			case 0x02:
			{
				switch (TxMsg->Data[HeaderSize + 1])
				{
					case 0x00:
					{
						/* Clear out the support data when PID 0x00 is requested */
						gOBDResponse[EcuIndex].Sid2PidSupportSize = 0;
						memset(gOBDResponse[EcuIndex].Sid2PidSupport, 0,
						sizeof(gOBDResponse[EcuIndex].Sid2PidSupport));
					}
					break;
					case 0x20:
					case 0x40:
					case 0x60:
					case 0x80:
					case 0xA0:
					case 0xC0:
					case 0xE0:
					{
						/* Don't do anything with other PID support requests */
					}
					break;
					default:
					{
						/* All other requests reset the PID data */
						gOBDResponse[EcuIndex].Sid2PidSize = 0;
						memset(gOBDResponse[EcuIndex].Sid2Pid, 0,
						sizeof(gOBDResponse[EcuIndex].Sid2Pid));
					}
					break;
				}
			}
			break;
			/* SID 3 (Mode 3) */
			case 0x03:
			{
                gOBDResponse[EcuIndex].Sid3Supported = FALSE;
				gOBDResponse[EcuIndex].Sid3Size = 0;
				memset( gOBDResponse[EcuIndex].Sid3,
				        0,
				        sizeof(gOBDResponse[EcuIndex].Sid3) );
			}
			break;
			/* SID 4 (Mode 4) */
			case 0x04:
			{
				gOBDResponse[EcuIndex].Sid4Size = 0;
			}
			break;
			/* SID 5 (Mode 5) */
			case 0x05:
			{
				switch (TxMsg->Data[HeaderSize + 1])
				{
					case 0x00:
					{
						/* Clear out the support data when PID 0x00 is requested */
						gOBDResponse[EcuIndex].Sid5TidSupportSize = 0;
						memset(gOBDResponse[EcuIndex].Sid5TidSupport, 0,
						sizeof(gOBDResponse[EcuIndex].Sid5TidSupport));
					}
					break;
					case 0x20:
					case 0x40:
					case 0x60:
					case 0x80:
					case 0xA0:
					case 0xC0:
					case 0xE0:
					{
						/* Don't do anything with other PID support requests */
					}
					break;
					default:
					{
						/* All other requests reset the PID data */
						gOBDResponse[EcuIndex].Sid5TidSize = 0;
						memset(gOBDResponse[EcuIndex].Sid5Tid, 0,
						sizeof(gOBDResponse[EcuIndex].Sid5Tid));
					}
					break;
				}
			}
			break;
			/* SID 6 (Mode 6) */
			case 0x06:
			{
				switch (TxMsg->Data[HeaderSize + 1])
				{
					case 0x00:
					{
						/* Clear out the support data when PID 0x00 is requested */
						gOBDResponse[EcuIndex].Sid6MidSupportSize = 0;
						memset(gOBDResponse[EcuIndex].Sid6MidSupport, 0,
						sizeof(gOBDResponse[EcuIndex].Sid6MidSupport));
					}
					break;
					case 0x20:
					case 0x40:
					case 0x60:
					case 0x80:
					case 0xA0:
					case 0xC0:
					case 0xE0:
					{
						/* Don't do anything with other PID support requests */
					}
					break;
					default:
					{
						/* All other requests reset the PID data */
						gOBDResponse[EcuIndex].Sid6MidSize = 0;
						memset(gOBDResponse[EcuIndex].Sid6Mid, 0,
						sizeof(gOBDResponse[EcuIndex].Sid6Mid));
					}
					break;
				}
			}
			break;
			/* SID 7 (Mode 7) */
			case 0x07:
			{
                gOBDResponse[EcuIndex].Sid7Supported = FALSE;
				gOBDResponse[EcuIndex].Sid7Size = 0;
				memset( gOBDResponse[EcuIndex].Sid7,
				        0,
				        sizeof(gOBDResponse[EcuIndex].Sid7) );
			}
			break;
			/* SID 8 (Mode 8) */
			case 0x08:
			{
				switch (TxMsg->Data[HeaderSize + 1])
				{
					case 0x00:
					{
						/* Clear out the support data when PID 0x00 is requested */
						gOBDResponse[EcuIndex].Sid8TidSupportSize = 0;
						memset(gOBDResponse[EcuIndex].Sid8TidSupport, 0,
						sizeof(gOBDResponse[EcuIndex].Sid8TidSupport));
					}
					break;
					case 0x20:
					case 0x40:
					case 0x60:
					case 0x80:
					case 0xA0:
					case 0xC0:
					case 0xE0:
					{
						/* Don't do anything with other PID support requests */
					}
					break;
					default:
					{
						/* All other requests reset the PID data */
						gOBDResponse[EcuIndex].Sid8TidSize = 0;
						memset(gOBDResponse[EcuIndex].Sid8Tid, 0,
						sizeof(gOBDResponse[EcuIndex].Sid8Tid));
					}
					break;
				}
			}
			break;
			/* SID 9 (Mode 9) */
			case 0x09:
			{
				switch (TxMsg->Data[HeaderSize + 1])
				{
					case 0x00:
					{
						/* Clear out the support data when PID 0x00 is requested */
						gOBDResponse[EcuIndex].Sid9InfSupportSize = 0;
						memset(gOBDResponse[EcuIndex].Sid9InfSupport, 0,
						sizeof(gOBDResponse[EcuIndex].Sid9InfSupport));
					}
					break;
					case 0x20:
					case 0x40:
					case 0x60:
					case 0x80:
					case 0xA0:
					case 0xC0:
					case 0xE0:
					{
						/* Don't do anything with other PID support requests */
					}
					break;
					default:
					{
						/* All other requests reset the PID data */
						gOBDResponse[EcuIndex].Sid9InfSize = 0;
						memset(gOBDResponse[EcuIndex].Sid9Inf, 0,
						sizeof(gOBDResponse[EcuIndex].Sid9Inf));
					}
					break;
				}
			}
			break;


			/* SID A (Mode A) */
			case 0x0A:
			{
				/* reset the PID data */
                gOBDResponse[EcuIndex].Sid3Supported = FALSE;
				gOBDResponse[EcuIndex].SidASize = 0;
				memset( gOBDResponse[EcuIndex].SidA,
				        0,
				        sizeof(gOBDResponse[EcuIndex].SidA) );
			}
			break;


			/* SID B-F (Mode B-F) */
			case 0x0B:
			case 0x0C:
			case 0x0D:
			case 0x0E:
			case 0x0F:
			{
			}
			break;


			default:
			{
				/* Unexpected reset of SID data */
				LogPrint("FAILURE: Unexpected reset of data for SID%d\n",
				TxMsg->Data[HeaderSize]);
				return(FAIL);

			}
		}
	}
	return(PASS);
}

