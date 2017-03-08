
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

void ChkIFRAdjust( PASSTHRU_MSG *RxMsg );	/* Logic ajustment for PID Group Reverse order request. */

/*
*******************************************************************************
** SidSaveResponseData -
** Function to save SID response data
*******************************************************************************
**	DATE		Modification
**	08/28/03	SF#790547:	Mode 6 MY Check not required.
**				Testing 2005 model year vehicle has uncovered an issue with
**				the source codes handling of mode 6 data.  Modification of
**				routine, "SidSaveResponseData", are required to complete
**				this change request.
*******************************************************************************
*/
STATUS SidSaveResponseData(PASSTHRU_MSG *RxMsg, SID_REQ *SidReq)
{
	unsigned long HeaderSize;
	unsigned long EcuIndex;
	unsigned long ByteIndex;
	unsigned char bElementOffset;
	unsigned long ulInx;	//SF#790547:	Added to coordinate mode 6 data respones
							//				for multiple data response.

	/* Set the response header size based on the protocol */
	HeaderSize = gOBDList[gOBDListIndex].HeaderSize;

	/* Get index into gOBDResponse struct */
	EcuIndex = LookupEcuIndex (RxMsg);

	/* Check if we exceeded the maximum allowable OBD ECUs */
	if (EcuIndex >= OBD_MAX_ECUS)
	{
		LogPrint("FAILURE: Maximum number of OBD ECU responses exceeded\n");
		return(FAIL);
	}

	ChkIFRAdjust( RxMsg );

	/* Save the data in the appropriate SID/PID/MID/TID/InfoType */
	switch(RxMsg->Data[HeaderSize])
	{
		/* SID 1 (Mode 1) */
		case 0x41:
		{
			/* Process message based on PID number */
			switch (RxMsg->Data[HeaderSize + 1])
			{
				case 0x00:
				case 0x20:
				case 0x40:
				case 0x60:
				case 0x80:
				case 0xA0:
				case 0xC0:
				case 0xE0:
				{
					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize < (HeaderSize + 1 + sizeof(ID_SUPPORT)))
					{
						LogPrint("FAILURE: ECU %X  Not enough data in SID $1 support response message to process\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Save the data in the buffer and set the new size */
					for ( ulInx = HeaderSize + 1; ulInx < RxMsg->DataSize; ulInx += sizeof(ID_SUPPORT) )
					{
						/* find the element number (for the array of structures) */
						bElementOffset = (unsigned char)(RxMsg->Data[ulInx]) >> 5;

						/* move the data into the element */
						memcpy( &gOBDResponse[EcuIndex].Sid1PidSupport[bElementOffset],
							&RxMsg->Data[ulInx],
							sizeof( ID_SUPPORT ));

						/* increase the element count if the element in the array was previously unused */
						if ( (bElementOffset + 1) > gOBDResponse[EcuIndex].Sid1PidSupportSize)
						{
							gOBDResponse[EcuIndex].Sid1PidSupportSize = (bElementOffset + 1);
						}
					}
				}
				/*
				 * FALL THROUGH AND TREAT LIKE OTHER PIDS
				 */

				default:
				{
					/* If not PID 0, then check if this is a response to an unsupported PID */
					if ( (RxMsg->Data[HeaderSize + 1] != 0) &&
					     (gOBDResponse[EcuIndex].Sid1PidSupport[
					       (RxMsg->Data[HeaderSize + 1] - 1) >> 5].IDBits[
					       ((RxMsg->Data[HeaderSize + 1] - 1) >> 3) & 0x03]
					       & (0x80 >> ((RxMsg->Data[HeaderSize + 1] - 1) & 0x07))) == 0
					   )
					{
						LogPrint( "WARNING: ECU %X  Unsupported SID $1 PID $%02X detected\n",
						          GetEcuId(EcuIndex),
						          RxMsg->Data[HeaderSize + 1]);
					}

					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize <= (HeaderSize + 1))
					{
						LogPrint("FAILURE: ECU %X  Not enough data in SID $1 response message to process\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/*
					** If only one PID requested, make sure the
					** PID has the correct amount of data
					*/
					if (SidReq->NumIds == 1)
					{
						switch (RxMsg->Data[HeaderSize + 1])
						{
							/* Single byte PIDs */
							case 0x04:
							case 0x05:
							case 0x0A:
							case 0x0B:
							case 0x0D:
							case 0x0E:
							case 0x0F:
							case 0x11:
							case 0x12:
							case 0x13:
							case 0x1C:
							case 0x1D:
							case 0x1E:
							case 0x2C:
							case 0x2D:
							case 0x2E:
							case 0x2F:
							case 0x30:
							case 0x33:
							case 0x45:
							case 0x46:
							case 0x47:
							case 0x48:
							case 0x49:
							case 0x4A:
							case 0x4B:
							case 0x4C:
							case 0x4F:
							case 0x51:
							case 0x52:
							case 0x5A:
							case 0x5B:
							case 0x5C:
							case 0x5F:
							case 0x61:
							case 0x62:
							case 0x7D:
							case 0x7E:
							case 0x84:
							{
								/* Check for one data byte */
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 1)
								{
									LogPrint("FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 1)\n",
									GetEcuId(EcuIndex),
									RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Two byte PIDs */
							case 0x02:
							case 0x03:
							case 0x0C:
							case 0x10:
							case 0x14:
							case 0x15:
							case 0x16:
							case 0x17:
							case 0x18:
							case 0x19:
							case 0x1A:
							case 0x1B:
							case 0x1F:
							case 0x21:
							case 0x22:
							case 0x23:
							case 0x31:
							case 0x32:
							case 0x3C:
							case 0x3D:
							case 0x3E:
							case 0x3F:
							case 0x42:
							case 0x43:
							case 0x44:
							case 0x4D:
							case 0x4E:
							case 0x53:
							case 0x54:
							case 0x59:
							case 0x5D:
							case 0x5E:
							case 0x63:
							case 0x65:
							{
								/* Check for two data bytes */
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 2)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 2)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)) );
									return(FAIL);
								}
							}
							break;

							/* Three byte PIDs */
							case 0x67:
							case 0x6F:
							{
								/* Check for four data bytes */
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 3)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 3)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Four byte PIDs */
							case 0x00:
							case 0x20:
							case 0x40:
							case 0x60:
							case 0x80:
							case 0xA0:
							case 0xC0:
							case 0xE0:
							case 0x01:
							case 0x24:
							case 0x25:
							case 0x26:
							case 0x27:
							case 0x28:
							case 0x29:
							case 0x2A:
							case 0x2B:
							case 0x34:
							case 0x35:
							case 0x36:
							case 0x37:
							case 0x38:
							case 0x39:
							case 0x3A:
							case 0x3B:
							case 0x41:
							{
								/* Check for four data bytes */
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 4)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 4)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Five byte PIDs */
							case 0x64:
							case 0x66:
							case 0x6A:
							case 0x6B:
							case 0x6C:
							case 0x72:
							case 0x73:
							case 0x74:
							case 0x77:
							case 0x83:
							case 0x86:
							case 0x87:
							{
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 5)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 5)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Six byte PIDs */
							case 0x71:
							{
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 6)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 6)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Seven byte PIDs */
							case 0x68:
							case 0x69:
							case 0x75:
							case 0x76:
							case 0x7A:
							case 0x7B:
							{
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 7)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 7)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Nine byte PIDs */
							case 0x6E:
							case 0x78:
							case 0x79:
							case 0x7C:
							{
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 9)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 9)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Ten byte PIDs */
							case 0x70:
							case 0x85:
							{
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 10)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 10)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Eleven byte PIDs */
							case 0x6D:
							{
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 11)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 11)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Thirteen byte PIDs */
							case 0x7F:
							{
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 13)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 14)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* Twenty-One byte PIDs */
							case 0x81:
							case 0x82:
							{
								if ((RxMsg->DataSize - (HeaderSize + 2)) != 21)
								{
									LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be 21)\n",
									          GetEcuId(EcuIndex),
									          RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 2)));
									return(FAIL);
								}
							}
							break;

							/* 1 or 2 byte PIDs */
							case 0x06:		/* PID $06 1 / 2-Byte definition */
							case 0x07:		/* PID $07 1 / 2-Byte definition */
							case 0x08:		/* PID $08 1 / 2-Byte definition */
							case 0x09:		/* PID $09 1 / 2-Byte definition */
							case 0x55:		/* PID $55 1 / 2-Byte definition */
							case 0x56:		/* PID $56 1 / 2-Byte definition */
							case 0x57:		/* PID $57 1 / 2-Byte definition */
							case 0x58:		/* PID $58 1 / 2-Byte definition */
							{
								/* Only check if successfully determined the (variable) PID size */
								if (gSid1VariablePidSize > 0)
								{
									/* Check for one or two data bytes */
									if ((RxMsg->DataSize - (HeaderSize + 2)) != (unsigned)gSid1VariablePidSize)
									{
										LogPrint( "FAILURE: ECU %X  SID $1 PID $%02X has %d data bytes (should be %u)\n",
										          GetEcuId(EcuIndex),
										          RxMsg->Data[HeaderSize + 1],
										          (RxMsg->DataSize - (HeaderSize + 2)),
										          gSid1VariablePidSize);
										return(FAIL);
									}
								}
							}
							default:
							{
								/* Non-OBD PID */
							}
							break;
						}
					}

					/* Determine if there is enough room in the buffer to store the data */
					if ( (RxMsg->DataSize - HeaderSize - 1) >
					     (sizeof(gOBDResponse[EcuIndex].Sid1Pid) - gOBDResponse[EcuIndex].Sid1PidSize))
					{
						LogPrint( "FAILURE: ECU %X  SID $1 response data exceeded buffer size\n",GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Save the data in the buffer and set the new size */
					memcpy( &gOBDResponse[EcuIndex].Sid1Pid[gOBDResponse[EcuIndex].Sid1PidSize],
					        &RxMsg->Data[HeaderSize + 1],
					        sizeof(SID1) );
					gOBDResponse[EcuIndex].Sid1PidSize += sizeof(SID1);
				}
				break;
			}
		}
		break;


		/* SID 2 (Mode 2) */
		case 0x42:
		{
			/* Process message based on PID number */
			switch (RxMsg->Data[HeaderSize + 1])
			{
				case 0x00:
				case 0x20:
				case 0x40:
				case 0x60:
				case 0x80:
				case 0xA0:
				case 0xC0:
				case 0xE0:
				{
					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize < (HeaderSize + 1 + sizeof(ID_SUPPORT)))
					{
						LogPrint( "FAILURE: ECU %X  Not enough data in SID $2 support response message to process\n",GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Save the data in the buffer and set the new size                                 */
					for ( ulInx = HeaderSize + 1; ulInx < RxMsg->DataSize; ulInx += sizeof(FF_SUPPORT) )
					{
						/* find the element number (for the array of structures) */
						bElementOffset = (unsigned char)(RxMsg->Data[ulInx]) >> 5;

						/* move the data into the element */
						memcpy( &gOBDResponse[EcuIndex].Sid2PidSupport[bElementOffset],
							&RxMsg->Data[ulInx],
							sizeof( FF_SUPPORT ));

						/* increase the element count if the element in the array was previously unused */
						if ( (bElementOffset + 1) > gOBDResponse[EcuIndex].Sid2PidSupportSize)
						{
							gOBDResponse[EcuIndex].Sid2PidSupportSize = (bElementOffset + 1);
						}
					}
				}
				break;
				default:
				{
					/* Check if this is a response to an unsupported PID */
					if ( gIgnoreUnsupported == FALSE &&
						 IsSid2PidSupported(EcuIndex, RxMsg->Data[HeaderSize + 1]) == FALSE )
					{
						LogPrint( "WARNING: ECU %X  Unsupported SID $2 PID $%02X detected\n",
						          GetEcuId(EcuIndex),
						          RxMsg->Data[HeaderSize + 1] );
					}

					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize <= (HeaderSize + 1))
					{
						LogPrint( "FAILURE: ECU %X  Not enough data in SID $2 response message to process\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Determine if there is enough room in the buffer to store the data */
					if ((RxMsg->DataSize - HeaderSize - 1) > (sizeof(
					gOBDResponse[EcuIndex].Sid2Pid) - gOBDResponse[EcuIndex].Sid2PidSize))
					{
						LogPrint( "FAILURE: ECU %X  SID $2 response data exceeded buffer size\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Save the data in the buffer and set the new size */
					memcpy( &gOBDResponse[EcuIndex].Sid2Pid[gOBDResponse[EcuIndex].Sid2PidSize],
					        &RxMsg->Data[HeaderSize + 1],
					        (RxMsg->DataSize - HeaderSize - 1) );
					gOBDResponse[EcuIndex].Sid2PidSize += (unsigned short)(RxMsg->DataSize - HeaderSize - 1);
				}
				break;
			}
		}
		break;


		/* SID 3 (Mode 3) */
		case 0x43:
		{
            gOBDResponse[EcuIndex].Sid3Supported = TRUE;

			if (gOBDList[gOBDListIndex].Protocol != ISO15765)
			{
				/* Just copy the DTC bytes */
				memcpy( &gOBDResponse[EcuIndex].Sid3[gOBDResponse[EcuIndex].Sid3Size],
				        &RxMsg->Data[HeaderSize + 1],
				        RxMsg->DataSize - HeaderSize - 1);
				gOBDResponse[EcuIndex].Sid3Size += (unsigned short)(RxMsg->DataSize - HeaderSize - 1);
			}
			else
			{
				/* If ISO15765, skip the number of DTCs byte */
				if (RxMsg->DataSize <= (HeaderSize + 2) || (RxMsg->Data[HeaderSize + 1] == 0))
				{
					/* If no DTCs, set to zero */
					memset(&gOBDResponse[EcuIndex].Sid3[gOBDResponse[EcuIndex].Sid3Size], 0, 2);
					gOBDResponse[EcuIndex].Sid3Size = 2;
				}
				else
				{
					/* Otherwise copy the DTC data */
					memcpy( &gOBDResponse[EcuIndex].Sid3[gOBDResponse[EcuIndex].Sid3Size],
					        &RxMsg->Data[HeaderSize + 2],
					        (RxMsg->DataSize - HeaderSize - 2));
					gOBDResponse[EcuIndex].Sid3Size = (unsigned char)(RxMsg->DataSize - HeaderSize - 2);

					/* Per J1699 rev 11.6- TC# 6.3 & 7.3 verify that reported number of DTCs
					** matches that of actual DTC count.
					*/
					if ( RxMsg->Data[HeaderSize + 1] != ( (RxMsg->DataSize - HeaderSize - 2) / 2 ) )
					{
						LogPrint( "FAILURE: ECU %X  SID $3 DTC Count must match the # of DTC's reported.\n", GetEcuId(EcuIndex) );
						return (FAIL);
					}
				}
			}
		}
		break;


		/* SID 4 (Mode 4) */
		case 0x44:
		{
			/* Indicate positive response */
			gOBDResponse[EcuIndex].Sid4[gOBDResponse[EcuIndex].Sid4Size] = 0x44;
			gOBDResponse[EcuIndex].Sid4Size++;
		}
		break;


		/* SID 5 (Mode 5) */
		case 0x45:
		{
			/* Indicate positive response */
			gOBDResponse[EcuIndex].Sid5Tid[gOBDResponse[EcuIndex].Sid5TidSize] = 0x45;
			gOBDResponse[EcuIndex].Sid5TidSize++;
		}
		break;


		/* SID 6 (Mode 6) */
		case 0x46:
		{
			/* Process message based on PID number */
			switch (RxMsg->Data[HeaderSize + 1])
			{
				case 0x00:
				case 0x20:
				case 0x40:
				case 0x60:
				case 0x80:
				case 0xA0:
				case 0xC0:
				case 0xE0:
				{
					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize < (HeaderSize + 1 + sizeof(ID_SUPPORT)))
					{
						LogPrint("FAILURE: ECU %X  Not enough data in SID $6 support response message to process\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* If not ISO15765 protocol, get rid of 0xFF to make data look the same */
					if (gOBDList[gOBDListIndex].Protocol != ISO15765)
					{
						for (ByteIndex = (HeaderSize + 2); ByteIndex < (RxMsg->DataSize - 1);
						ByteIndex++)
						{
							RxMsg->Data[ByteIndex] = RxMsg->Data[ByteIndex + 1];
						}
						RxMsg->DataSize -= 2;
					}

					/* Save the data in the buffer and set the new size */
					for ( ulInx = HeaderSize + 1; ulInx < RxMsg->DataSize; ulInx += sizeof(ID_SUPPORT) )
					{
						/* find the element number (for the array of structures) */
						bElementOffset = (unsigned char)(RxMsg->Data[ulInx]) >> 5;

						/* move the data into the element */
						memcpy( &gOBDResponse[EcuIndex].Sid6MidSupport[bElementOffset],
						        &RxMsg->Data[ulInx],
						        sizeof(ID_SUPPORT) );

						/* increase the element count if the element in the array was previously unused */
						if ( (bElementOffset + 1) > gOBDResponse[EcuIndex].Sid6MidSupportSize)
						{
							gOBDResponse[EcuIndex].Sid6MidSupportSize = (bElementOffset + 1);
						}
					}
				}
				break;
				default:
				{
					/* Check if this is a response to an unsupported MID */
					if ((gOBDResponse[EcuIndex].Sid6MidSupport[
					(RxMsg->Data[HeaderSize + 1] - 1) >> 5].IDBits[
					((RxMsg->Data[HeaderSize + 1] - 1) >> 3) & 0x03]
					& (0x80 >> ((RxMsg->Data[HeaderSize + 1] - 1) & 0x07))) == 0)
					{
						LogPrint( "WARNING: ECU %X  Unsupported SID $6 MID $%02X detected\n",
						          GetEcuId(EcuIndex),
						          RxMsg->Data[HeaderSize + 1]);
					}

					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize <= (HeaderSize + 1))
					{
						LogPrint( "FAILURE: ECU %X  Not enough data in SID $6 response message to process\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* If not ISO15765 protocol, make the data look like it */
					if (gOBDList[gOBDListIndex].Protocol != ISO15765)
					{
						/* Check if it is a max or min limit */
						if (RxMsg->Data[HeaderSize + 2] & 0x80)
						{
							/* Move the limit to the minimum location */
							RxMsg->Data[HeaderSize + 7] = RxMsg->Data[HeaderSize + 6];
							RxMsg->Data[HeaderSize + 6] = RxMsg->Data[HeaderSize + 5];

							/* Set the maximum to 0xFFFF */
							RxMsg->Data[HeaderSize + 9] = 0xFF;
							RxMsg->Data[HeaderSize + 8] = 0xFF;
						}
						else
						{
							/* Move the limit to the maximum location */
							RxMsg->Data[HeaderSize + 9] = RxMsg->Data[HeaderSize + 6];
							RxMsg->Data[HeaderSize + 8] = RxMsg->Data[HeaderSize + 5];

							/* Set the minimum to 0x0000 */
							RxMsg->Data[HeaderSize + 7] = 0x00;
							RxMsg->Data[HeaderSize + 6] = 0x00;
						}

						/* Move the value to the new location */
						RxMsg->Data[HeaderSize + 5] = RxMsg->Data[HeaderSize + 4];
						RxMsg->Data[HeaderSize + 4] = RxMsg->Data[HeaderSize + 3];

						/* Set the unit and scaling ID to zero */
						RxMsg->Data[HeaderSize + 3] = 0x00;

						/* Adjust the message size */
						RxMsg->DataSize += 3;
					}

					/* Determine if there is enough room in the buffer to store the data */
					if ((RxMsg->DataSize - HeaderSize - 1) > (sizeof(
					gOBDResponse[EcuIndex].Sid6Mid) - gOBDResponse[EcuIndex].Sid6MidSize))
					{
						LogPrint("FAILURE: ECU %X  SID $6 response data exceeded buffer size\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					//SF#790547:	Logic calculation added to determine the response
					//				size of the mode 6 data.   Previously only sizeof(SID6)
					//				data was moved from the received queue to the mode 6
					//				data structure.  The result was only the first
					//				'data record of supported OBDMID' was available for
					//				analysis.
					ulInx = ((RxMsg->DataSize - (HeaderSize + 1))/sizeof(SID6));
					/* Save the data in the buffer and set the new size */
					memcpy( &gOBDResponse[EcuIndex].Sid6Mid[gOBDResponse[EcuIndex].Sid6MidSize],
					        &RxMsg->Data[HeaderSize + 1],
					        (sizeof(SID6)*ulInx));
					gOBDResponse[EcuIndex].Sid6MidSize += (unsigned short)(sizeof(SID6)*ulInx);
				}
				break;
			}
		}
		break;


		/* SID 7 (Mode 7) */
		case 0x47:
		{
            gOBDResponse[EcuIndex].Sid7Supported = TRUE;

			if (gOBDList[gOBDListIndex].Protocol != ISO15765)
			{
				/* Just copy the DTC bytes */
				memcpy(&gOBDResponse[EcuIndex].Sid7[gOBDResponse[EcuIndex].Sid7Size],
				&RxMsg->Data[HeaderSize + 1], RxMsg->DataSize - HeaderSize - 1);
				gOBDResponse[EcuIndex].Sid7Size += (unsigned short)(RxMsg->DataSize - HeaderSize - 1);
			}
			else
			{
				/* If ISO15765, skip the number of DTCs byte */
				if (RxMsg->DataSize <= (HeaderSize + 2) || (RxMsg->Data[HeaderSize + 1] == 0))
				{
					/* If no DTCs, set to zero */
					memset(&gOBDResponse[EcuIndex].Sid7[gOBDResponse[EcuIndex].Sid7Size], 0, 2);
					gOBDResponse[EcuIndex].Sid7Size = 2;
				}
				else
				{
					/* Otherwise copy the DTC data */
					memcpy(&gOBDResponse[EcuIndex].Sid7[gOBDResponse[EcuIndex].Sid7Size],
					&RxMsg->Data[HeaderSize + 2], (RxMsg->DataSize - HeaderSize - 2));
					gOBDResponse[EcuIndex].Sid7Size = (unsigned char)(RxMsg->DataSize - HeaderSize - 2);

					/* Per J1699 rev 11.6- TC# 6.3 & 7.3 verify that reported number of DTCs
					** matches that of actual DTC count.
					*/
					if ( RxMsg->Data[HeaderSize + 1] != ( (RxMsg->DataSize - HeaderSize - 2) / 2 ) )
					{
						LogPrint("FAILURE: ECU %X  SID $7 DTC Count must match the # of DTC's reported.\n", GetEcuId(EcuIndex) );
						return (FAIL);
					}
				}
			}
		}
		break;


		/* SID 8 (Mode 8) */
		case 0x48:
		{
			/* Process message based on PID number */
			switch (RxMsg->Data[HeaderSize + 1])
			{
				case 0x00:
				case 0x20:
				case 0x40:
				case 0x60:
				case 0x80:
				case 0xA0:
				case 0xC0:
				case 0xE0:
					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize < (HeaderSize + 1 + sizeof(ID_SUPPORT)))
					{
						LogPrint( "FAILURE: ECU %X  Not enough data in SID $8 support response message to process\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Save the data in the buffer and set the new size */
					for ( ulInx = HeaderSize + 1; ulInx < RxMsg->DataSize; ulInx += sizeof(ID_SUPPORT) )
					{
						/* find the element number (for the array of structures) */
						bElementOffset = (unsigned char)(RxMsg->Data[ulInx]) >> 5;

						/* move the data into the element */
						memcpy( &gOBDResponse[EcuIndex].Sid8TidSupport[bElementOffset],
						        &RxMsg->Data[ulInx],
						        sizeof( ID_SUPPORT ));

						/* increase the element count if the element in the array was previously unused */
						if ( (bElementOffset + 1) > gOBDResponse[EcuIndex].Sid8TidSupportSize)
						{
							gOBDResponse[EcuIndex].Sid8TidSupportSize = (bElementOffset + 1);
						}
					}
					break;

				default:
					/* Check if this is a response to an unsupported TID */
					if (IsSid8TidSupported(EcuIndex, RxMsg->Data[HeaderSize + 1]) == FALSE)
					{
						LogPrint( "WARNING: ECU %X  Unsupported SID $8 TID $%02X detected\n",
						          GetEcuId(EcuIndex),
						          RxMsg->Data[HeaderSize + 1]);
					}

					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize <= (HeaderSize + 2))
					{
						LogPrint("FAILURE: ECU %X  Not enough data in SID $8 response message to process\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Determine if there is enough room in the buffer to store the data */
					if ((RxMsg->DataSize - HeaderSize - 1) > (sizeof(gOBDResponse[EcuIndex].Sid8Tid) - gOBDResponse[EcuIndex].Sid8TidSize))
					{
						LogPrint("FAILURE: ECU %X  SID $8 response data exceeded buffer size\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					memcpy( &gOBDResponse[EcuIndex].Sid8Tid[gOBDResponse[EcuIndex].Sid8TidSize],
					        &RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 1)) );

					gOBDResponse[EcuIndex].Sid8TidSize += (unsigned char)(RxMsg->DataSize - (HeaderSize + 1));

					break;
			}
		}
		break;


		/* SID 9 (Mode 9) */
		case 0x49:
		{
			/* Process message based on PID number */
			switch (RxMsg->Data[HeaderSize + 1])
			{
				case 0x00:
				case 0x20:
				case 0x40:
				case 0x60:
				case 0x80:
				case 0xA0:
				case 0xC0:
				case 0xE0:
				{
					if (gOBDList[gOBDListIndex].Protocol != ISO15765)
					{
						ulInx = HeaderSize + 2;
					}
					else
					{
						ulInx = HeaderSize + 1;
					}

					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize < (ulInx + sizeof(ID_SUPPORT)))
					{
						LogPrint( "FAILURE: ECU %X  Not enough data in SID $9 support response message to process.\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Save the data in the buffer and set the new size */
					for ( ; ulInx < RxMsg->DataSize; ulInx += sizeof(ID_SUPPORT) )
					{
						/* find the element number (for the array of structures) */
						bElementOffset = (unsigned char)(RxMsg->Data[ulInx]) >> 5;

						/* move the data into the element */
						memcpy( &gOBDResponse[EcuIndex].Sid9InfSupport[bElementOffset],
						        &RxMsg->Data[ulInx],
						        sizeof( ID_SUPPORT ));

						/* increase the element count if the element in the array was previously unused */
						if ( (bElementOffset + 1) > gOBDResponse[EcuIndex].Sid9InfSupportSize)
						{
							gOBDResponse[EcuIndex].Sid9InfSupportSize = (bElementOffset + 1);
						}
					}
				}
				break;

				default:
				{
					/* Check if this is a response to an unsupported INF */
					if (IsSid9InfSupported(EcuIndex, RxMsg->Data[HeaderSize + 1]) == FALSE)
					{
						LogPrint( "WARNING: ECU %X  Unsupported SID $9 INF $%02X detected\n",
						          GetEcuId(EcuIndex),
						          RxMsg->Data[HeaderSize + 1]);
					}

					/* Make sure there is enough data in the message to process */
					if (RxMsg->DataSize <= (HeaderSize + 2))
					{
						LogPrint( "FAILURE: ECU %X  Not enough data in SID $9 response message to process\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Determine if there is enough room in the buffer to store the data */
					if ( (RxMsg->DataSize - HeaderSize - 1) >
					     (sizeof(gOBDResponse[EcuIndex].Sid9Inf) - gOBDResponse[EcuIndex].Sid9InfSize) )
					{
						LogPrint( "FAILURE: ECU %X  SID $9 response data exceeded buffer size\n", GetEcuId(EcuIndex) );
						return(FAIL);
					}

					/* Save the data in the buffer and set the new size */
					if (gOBDList[gOBDListIndex].Protocol == ISO15765)
					{
						memcpy(&gOBDResponse[EcuIndex].Sid9Inf[gOBDResponse[EcuIndex].Sid9InfSize],
						&RxMsg->Data[HeaderSize + 1], (RxMsg->DataSize - (HeaderSize + 1)));
						gOBDResponse[EcuIndex].Sid9InfSize += (unsigned char)(RxMsg->DataSize - (HeaderSize + 1));

						/* SAEJ1699 V11.6; Verify CVNs contain 4 HEX bytes */
						if ( RxMsg->Data[0x05] == 0x06 )
						{

							/* HeaderSize is 4; 0x49 0x06 0x(CVN SIZE) 0x(DATA) ... */
							if ( ( ( RxMsg->DataSize - ( HeaderSize + 3 ) ) % 4 ) != 0 )
							{
								LogPrint( "FAILURE: ECU %X  SID $9 INF $6, All CVNs must contain 4 HEX bytes.\n", GetEcuId(EcuIndex) );

								/* Restart the periodic message if protocol determined and not in burst test */
								if ( gOBDList[gOBDListIndex].TesterPresentID == -1 )
								{
									LogPrint("INFORMATION: Tester present active!\n" );
									StartPeriodicMsg ();
								}
								return (FAIL);
							}
						}
					}
					else
					{
						memcpy( &gOBDResponse[EcuIndex].Sid9Inf[gOBDResponse[EcuIndex].Sid9InfSize],
						        &RxMsg->Data[HeaderSize + 1],
						        sizeof(SID9) );
						gOBDResponse[EcuIndex].Sid9InfSize += sizeof(SID9);

						/* SAEJ1699 V11.6; Verify CVNs contain 4 HEX bytes */
						if ( ( RxMsg->Data[0x04] == 0x06 ) &&
							 ( RxMsg->DataSize   != 0x0a ) )
						{
							LogPrint( "FAILURE: ECU %X  SID $9 response, All CVNs must contain 4 bytes of HEX data!\n", GetEcuId(EcuIndex) );

							/* Restart the periodic message if protocol determined and not in burst test */
							if ( gOBDList[gOBDListIndex].TesterPresentID == -1 )
							{
								LogPrint("INFORMATION: Tester present active!\n");
								StartPeriodicMsg ();
							}
							return (FAIL);
						}
					}
				}
				break;
			}
		}
		break;


		/* SID A (Mode A) */
		case 0x4A:
		{
            gOBDResponse[EcuIndex].SidASupported = TRUE;

			/* Copy the DTC data */
			memcpy( &gOBDResponse[EcuIndex].SidA[gOBDResponse[EcuIndex].SidASize],
			        &RxMsg->Data[HeaderSize + 2],
			        (RxMsg->DataSize - HeaderSize - 2) );
			gOBDResponse[EcuIndex].SidASize = (unsigned char)(RxMsg->DataSize - HeaderSize - 2);

			/* Verify that reported number of DTCs matches that of actual DTC count. */
			if ( RxMsg->Data[HeaderSize + 1] != ( (RxMsg->DataSize - HeaderSize - 2) / 2 ) )
			{
				LogPrint( "FAILURE: ECU %X  SID $A DTC Count must match the # of DTC's reported.\n",GetEcuId(EcuIndex) );
				return (FAIL);
			}
		}
		break;

		default:
		{
			/* Check for a negative response code */
			if (RxMsg->Data[HeaderSize] == NAK)
			{
				LogPrint("INFORMATION: Received negative response code\n");
			}
			else
			{
				/* Unexpected SID response */
				LogPrint( "FAILURE: ECU %X  Unexpected SID $%02X response%s\n",
				          GetEcuId(EcuIndex),
				          (RxMsg->Data[HeaderSize] - OBD_RESPONSE_BIT),
				          (RxMsg->Data[HeaderSize] == NAK) ? " (NAK)" : "" );
				return(FAIL);
			}
		}
	}
	return(PASS);
}

/*
********************************************************************************
**
**	FUNCTION	ChkIFRAdjust
**
**	PURPOSE		Account for checksum or IFR in data size.
**
********************************************************************************
*/
void ChkIFRAdjust (PASSTHRU_MSG *RxMsg)
{
	/* If extra data was returned (e.g. checksum, IFR, ...), remove it */
	if ( ( RxMsg->ExtraDataIndex != 0 ) && ( RxMsg->DataSize != RxMsg->ExtraDataIndex ) )
	{
		/* Adjust the data size to ignore the extra data */
		RxMsg->DataSize = RxMsg->ExtraDataIndex;
	}
}

/*
********************************************************************************
**
**	FUNCTION	LookupEcuIndex
**
**	PURPOSE		Return index into gOBDResponse struct for this message
**
********************************************************************************
*/
unsigned long LookupEcuIndex (PASSTHRU_MSG *RxMsg)
{
	unsigned long HeaderSize;
	unsigned long EcuIndex;
	unsigned long ByteIndex;

	/* Set the response header size based on the protocol */
	HeaderSize = gOBDList[gOBDListIndex].HeaderSize;

	/* Find the appropriate EcuIndex based on the header information */
	for (EcuIndex = 0; EcuIndex < OBD_MAX_ECUS; EcuIndex++)
	{
		/* Check if we have a header match */
		for (ByteIndex = 0; ByteIndex < HeaderSize; ByteIndex++)
		{
			if (RxMsg->Data[ByteIndex] != gOBDResponse[EcuIndex].Header[ByteIndex])
			{
				/*
				** If ISO14230 protocol,
				** ignore the length bits in the first byte of header
				*/
				if ( (gOBDList[gOBDListIndex].Protocol == ISO14230) &&
					 (ByteIndex == 0))
				{
					if ( (RxMsg->Data[ByteIndex] & 0xC0) !=
						 (gOBDResponse[EcuIndex].Header[ByteIndex] & 0xC0) )
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		/* If no match, check if EcuIndex is empty */
		if (ByteIndex != HeaderSize)
		{
			if (gOBDResponse[EcuIndex].Header[0] == 0x00 &&
				gOBDResponse[EcuIndex].Header[1] == 0x00 &&
				gOBDResponse[EcuIndex].Header[2] == 0x00 &&
				gOBDResponse[EcuIndex].Header[3] == 0x00)
			{
				/* if not currently in the process of determining the protocol
				 * then check every Rx msg's ID
				 */
				if (gOBDDetermined == TRUE)
				{
					if (VerifyEcuID (&RxMsg->Data[0]) == FAIL)
					{
						LogPrint( "FAILURE: Response from ECU %X not in initial list\n", GetEcuId(EcuIndex) );
						return (FAIL);
					}
				}

				/* If empty, add the new response */
				memcpy(&gOBDResponse[EcuIndex].Header[0], &RxMsg->Data[0], HeaderSize);
				break;
			}
		}
		else
		{
			/* We have a header match */
			break;
		}
	}

	return EcuIndex;
}
