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
STATUS IDOBDProtocol (unsigned long *);
STATUS LogJ2534InterfaceVersion (void);
STATUS VerifyBatteryVoltage (void);
STATUS SearchTAfor29bit (void);  /* By Honda */

STATUS SaveConnectInfo (void);
STATUS VerifyConnectInfo (void);

/*-----------------------------------------------------------------------------
   Local data

   Note:  In order to reduce the global namespace, the following 
          definition/declaration(s) are made local since they are used by
          local functions only.

          They can be move to the global namespace if needed.
------------------------------------------------------------------------------*/
typedef struct
{
    unsigned long Protocol;
    unsigned long NumECUs;
    unsigned long HeaderSize;
    unsigned char Header[OBD_MAX_ECUS][4];
} INITIAL_CONNECT_INFO;

static INITIAL_CONNECT_INFO gInitialConnect = {0}; // initialize all fields to zero

static unsigned char gFirstConnectFlag = TRUE;

/*
*******************************************************************************
** DetermineProtocol - Function to see what protocol is used to support OBD
*******************************************************************************
*/
STATUS DetermineProtocol(void)
{
    unsigned long fOBDFound = FALSE;

    /* If already connected to a protocol, disconnect first */
    if (gOBDDetermined == TRUE)
    {
        DisconnectProtocol();
        gOBDDetermined = FALSE;

        /* Add a delay if disconnecting from ISO9141/ISO14230 to make sure the ECUs 
         * are out of any previous diagnostic session before determining protocol.
         */
        if ((gOBDList[gOBDListIndex].Protocol == ISO9141) ||
            (gOBDList[gOBDListIndex].Protocol == ISO14230))
        {
            Sleep (5000);
        }
    }

    /* Reset globals */
    gOBDListIndex = 0;
    gOBDNumEcus = 0;

    /* Initialize protocol list */
    InitProtocolList();

    gDetermineProtocol = 1;
    /* Connect to each protocol in the list and check if SID 1 PID 0 is supported */
    if (IDOBDProtocol (&fOBDFound) == PASS)
    {

		/* Connect to the OBD protocol */
		if (fOBDFound == TRUE)
		{
			gOBDListIndex  = gOBDFoundIndex;
			gOBDDetermined = TRUE;
			if (ConnectProtocol() != PASS)
			{
				LogPrint("FAILURE: Connect to protocol failed\n");
				return (FAIL);
			}

			/* Get the version information and log it */
			if (LogJ2534InterfaceVersion() != PASS)
			{
				/* If routine logged an error, pass error up! */
				return (FAIL);
			}

			/* Check if battery is within a resonable range */
			if (VerifyBatteryVoltage () != PASS)
            {
                return (FAIL);
            }

			gDetermineProtocol = 0;
			return (PASS);
		}

    }
    gDetermineProtocol = 0;
    return (FAIL);
}

//*****************************************************************************
//
//	Function:	IDOBDProtocol
//
//	Purpose:	Purpose is to attempt to communicate on each specified OBDII
//				protocol channel and attempt to communicate.  If the current
//				link communicate then return return result.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	10/21/03	Isolated routine into a function.
//	05/21/04	Per SAEJ1699 v11.5, removed check for support of PID$01.
//
//*****************************************************************************
STATUS IDOBDProtocol (unsigned long *pfOBDFound)
{
    SID_REQ			SidReq;

    /* Setup initial conditions */
    *pfOBDFound = FALSE;

    /* Connect to each protocol in the list and check if SID 1 PID 0 is supported */
    for (gOBDListIndex = 0; gOBDListIndex < OBD_MAX_PROTOCOLS; gOBDListIndex++)
    {
        /* Connect to the protocol */
        LogPrint("INFORMATION: Checking for OBD on %s protocol\n", gOBDList[gOBDListIndex].Name);
        if ((gOBDList[gOBDListIndex].Protocol == ISO15765) &&       /* By Honda */
            (gOBDList[gOBDListIndex].InitFlags & CAN_29BIT_ID))
        {
            if (SearchTAfor29bit() != PASS)
		    {
                /* can't quit if there are more protocols to check */
                if ((gOBDListIndex >= OBD_MAX_PROTOCOLS-1) && (gOBDNumEcus == 0))
                {
                    LogPrint("FAILURE: CAN 29 bit address search failed.\n");
                    return (FAIL);
                }
			}
        }

        if (ConnectProtocol() == PASS)
        {
            /* Check if SID 1 PID 0 supported */
            SidReq.SID		= 1;
            SidReq.NumIds	= 1;
            SidReq.Ids[0]	= 0;

            if (SidRequest(&SidReq, SID_REQ_NORMAL) == PASS)
            {
                /* We've found an OBD supported protocol */
                LogPrint("INFORMATION: OBD on %s protocol detected\n", gOBDList[gOBDListIndex].Name);
                if (*pfOBDFound == TRUE)
                {
                    /* Check if protocol is the same (e.g. ISO14230 fast and 5-baud init) */
                    if ((gOBDList[gOBDListIndex].Protocol  != gOBDList[gOBDFoundIndex].Protocol) ||
                        (gOBDList[gOBDListIndex].Protocol  == ISO15765) ||
                        (gOBDList[gOBDFoundIndex].Protocol == ISO15765) )
                    {
                        LogPrint("FAILURE: Multiple protocols supporting OBD detected\n");
                        return (FAIL);
                    }
                }

                /* Save connect parameters (protocol, # ECUs, ECU IDs) on very first connect.
                 * Connect parameters should match on each subsequent connect.
                 */
                if (gFirstConnectFlag == TRUE)
                {
                    SaveConnectInfo();
                    gFirstConnectFlag = FALSE;
                }
                else
                {
                    if (VerifyConnectInfo () == FAIL)
                    {
                        return (FAIL);
                    }
                }

                /* Set the found flag and globals */
                *pfOBDFound = TRUE;
                gOBDFoundIndex = gOBDListIndex;
            }
        }

        /* Disconnect from the protocol */
        DisconnectProtocol ();

        /*
        ** Add a delay for ISO9141/ISO14230 to make sure the ECUs are out of
        ** any previous diagnostic session.
        */
        if ((gOBDList[gOBDListIndex].Protocol == ISO9141) ||
            (gOBDList[gOBDListIndex].Protocol == ISO14230))
        {
            Sleep (5000);
        }
    }

    return (PASS);
}

//
//*****************************************************************************
//
//	Function:	LogJ2534InterfaceVersion
//
//	Purpose:	Purpose is to read J2534 interface version information
//				then log it to the current log file.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	10/11/03	Isolated routine into a function.
//
//*****************************************************************************
STATUS LogJ2534InterfaceVersion (void)
{
    static char FirmwareVersion[80];
    static char DllVersion[80];
    static char ApiVersion[80];

    STATUS result = PASS;

    if (PassThruReadVersion (DeviceID, FirmwareVersion, DllVersion, ApiVersion) != STATUS_NOERROR)
    {
        LogPrint("FAILURE: Version information not available\n");
        fclose (ghLogFile);
        result = FAIL;
    }
    LogPrint("INFORMATION: Firmware Version: %s\n", FirmwareVersion);
    LogPrint("INFORMATION: DLL Version: %s\n", DllVersion);
    LogPrint("INFORMATION: API Version: %s\n", ApiVersion);

    return (result);
}

//*****************************************************************************
//
//	Function:	VerifyBatteryVoltage
//
//	Purpose:	Purpose is to read current vehicle system voltage then
//				verify if the voltage is within specified limits.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	10/11/03	Isolated routine into a function.
//
//*****************************************************************************
STATUS VerifyBatteryVoltage (void)
{
	long			RetVal;
	unsigned long	BatteryVoltage;
    STATUS          RetCode = FAIL;

	/* Check if battery is within a resonable range */
	RetVal = PassThruIoctl (DeviceID, READ_VBATT, NULL, &BatteryVoltage);
	if (RetVal != STATUS_NOERROR && RetVal != ERR_NOT_SUPPORTED)
	{
		Log2534Error("", RetVal, "PassThruIoctl(READ_VBATT)");
	}
	// Added by JS@BE
	else if (RetVal == ERR_NOT_SUPPORTED)
	{
		Log2534Error("", RetVal, "PassThruIoctl(READ_VBATT)");
		LogPrint("FAILURE: Battery voltage reading is not supported by the external test equipment");
		if ( WarnContinue ( "Do you wish to continue? " ) == 'Y' )
            RetCode = PASS;
	}
	else if (BatteryVoltage < OBD_BATTERY_MINIMUM)
	{
		LogPrint("WARNING: Battery voltage is LOW (%ld.%03ldV)\n", (BatteryVoltage / 1000),
            (BatteryVoltage % 1000));

        if ( WarnContinue ( "Do you wish to continue? " ) == 'Y' )
            RetCode = PASS;
	}
	else if (BatteryVoltage > OBD_BATTERY_MAXIMUM)
	{
		LogPrint("WARNING: Battery voltage is HIGH (%ld.%03ldV)\n", (BatteryVoltage / 1000),
            (BatteryVoltage % 1000));

        if ( WarnContinue ( "Do you wish to continue? " ) == 'Y' )
            RetCode = PASS;
	}
	else
	{
		LogPrint("INFORMATION: Battery = %ld.%03ldV\n", (BatteryVoltage / 1000),
            (BatteryVoltage % 1000));
        RetCode = PASS;
	}

    return RetCode;
}

/* Additinal Function By Honda */
//*****************************************************************************
//
//	Function:	SearchTAfor29bit
//
//	Purpose:	Purpose is to research ECU's TargetAddress and numer of ECUs.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	07/05/04	additional by Honda.
//
//*****************************************************************************
STATUS SearchTAfor29bit (void)
{
    PROTOCOL_LIST Can;            /* for CAN protcol */
    unsigned long RetVal;         /* Return Value */
    unsigned long NumMsgs;        /* Number of Messages */
    PASSTHRU_MSG  MaskMsg;
    PASSTHRU_MSG  PatternMsg;
    PASSTHRU_MSG  TxMsg;
    PASSTHRU_MSG  RxMsg;

    STATUS        RetCode = FAIL;

    /* Data Initalize */
    gOBDNumEcusCan = 0;
    Can.ChannelID  = 0;

    /* Connect to protocol */
    RetVal = PassThruConnect (DeviceID, CAN, CAN_29BIT_ID, gOBDList[gOBDListIndex].BaudRate, &Can.ChannelID);
    if (RetVal != STATUS_NOERROR)
    {
        Log2534Error ("", RetVal, "PassThruConnect");
        return (FAIL);
    }

	/* Clear the message filters */
    RetVal = PassThruIoctl (Can.ChannelID, CLEAR_MSG_FILTERS, NULL, NULL);
    if (RetVal != STATUS_NOERROR)
    {
        Log2534Error ("", RetVal, "PassThruIoctl(CLEAR_MSG_FILTERS)");
        goto search29bit_disconnect_exit;
    }

	/* Setup pass filter to read only OBD requests / responses */
    MaskMsg.ProtocolID    = CAN;
    MaskMsg.TxFlags       = CAN_29BIT_ID;
    MaskMsg.DataSize      = 4;
    MaskMsg.Data[0]       = 0xFF;
    MaskMsg.Data[1]       = 0xFF;
    MaskMsg.Data[2]       = 0xFF;
    MaskMsg.Data[3]       = 0x00;

    PatternMsg.ProtocolID = CAN;
    PatternMsg.TxFlags    = CAN_29BIT_ID;
    PatternMsg.DataSize   = 4;
    PatternMsg.Data[0]    = 0x18;
    PatternMsg.Data[1]    = 0xDA;
    PatternMsg.Data[2]    = 0xF1;
    PatternMsg.Data[3]    = 0x00;

    RetVal = PassThruStartMsgFilter (Can.ChannelID, PASS_FILTER, &MaskMsg, &PatternMsg, NULL, &Can.FilterID);
    if (RetVal != STATUS_NOERROR)
    {
        Log2534Error ("", RetVal, "PassThruStartMsgFilter");
        goto search29bit_err_exit;
    }

    /* Clear the receive queue before sending request */
    RetVal = PassThruIoctl (Can.ChannelID, CLEAR_RX_BUFFER, NULL, NULL);
    if (RetVal != STATUS_NOERROR)
    {
        Log2534Error ("", RetVal, "PassThruIoctl(CLEAR_RX_BUFFER)");
        goto search29bit_err_exit;
    }

    /* Send the request by CAN Format as ISO15765 29bits */
    TxMsg.ProtocolID = CAN;
//  TxMsg.RxStatus   = TX_MSG_TYPE;
    TxMsg.TxFlags    = CAN_29BIT_ID;
    TxMsg.Data[0]    = 0x18;
    TxMsg.Data[1]    = 0xDB;
    TxMsg.Data[2]    = 0x33;
    TxMsg.Data[3]    = 0xF1;
    TxMsg.Data[4]    = 0x02;	/* Service ID */
    TxMsg.Data[5]    = 0x01;	/* Service ID */
    TxMsg.Data[6]    = 0x00;	/* PID */
    TxMsg.Data[6]    = 0x00;	/* Padding Data */
    TxMsg.Data[7]    = 0x00;	/* Padding Data */
    TxMsg.Data[8]    = 0x00;	/* Padding Data */
    TxMsg.Data[9]    = 0x00;	/* Padding Data */
    TxMsg.Data[10]   = 0x00;	/* Padding Data */
    TxMsg.Data[11]   = 0x00;	/* Padding Data */
    TxMsg.DataSize   = 12;

    NumMsgs = 1;
    RetVal  = PassThruWriteMsgs (Can.ChannelID, &TxMsg, &NumMsgs, 500);
    if (RetVal != STATUS_NOERROR)
    {
        /*  don't log timeouts during DetermineProtocol */
        if (!(gDetermineProtocol == 1 && RetVal == ERR_TIMEOUT))
            Log2534Error("", RetVal, "PassThruWriteMsgs");
        goto search29bit_err_exit;
    }

    NumMsgs = 1;
    RetVal  = PassThruReadMsgs (Can.ChannelID, &RxMsg, &NumMsgs, 500);

    /* All OBD ECUs are respond by 50msec */
    while (NumMsgs == 1)
    {
        if ( (RxMsg.DataSize >= 5) && (RxMsg.Data[5] == 0x41) )
        {
            LogPrint("INFORMATION: ECU#%d: Target Address %02X(hex)\n",gOBDNumEcusCan+1, RxMsg.Data[3]);
            gOBDResponseTA[gOBDNumEcusCan] = RxMsg.Data[3]; /* ECU TA */
            gOBDNumEcusCan++;       /* Increment number of positive respond ECUs */
        }
        NumMsgs = 1;
        RetVal  = PassThruReadMsgs (Can.ChannelID, &RxMsg, &NumMsgs, 500);
    }

    /* so far so good */
    RetCode = PASS;

search29bit_err_exit:

    /* Turn off all filters before disconnecting */
    RetVal = PassThruIoctl (Can.ChannelID, CLEAR_MSG_FILTERS, NULL, NULL);
    if (RetVal != STATUS_NOERROR)
    {
        Log2534Error ("", RetVal, "PassThruIoctl(CLEAR_MSG_FILTERS)");
        RetCode = FAIL;
    }

search29bit_disconnect_exit:

    /* DisconnectProtocol */
    RetVal = PassThruDisconnect (Can.ChannelID);
    if (RetVal != STATUS_NOERROR)
    {
        Log2534Error ("", RetVal, "PassThruDisconnect");
        RetCode = FAIL;
    }

    return RetCode;
}

//*****************************************************************************
//
//	Function:	SaveConnectInfo
//
//	Purpose:	Save connection information from initial connect for comparison 
//              during subsequent connects.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	1/5/05	initial
//
//*****************************************************************************
STATUS SaveConnectInfo (void)
{
    unsigned long EcuIndex;

    memset (&gInitialConnect, 0, sizeof(gInitialConnect));

    gInitialConnect.Protocol   = gOBDList[gOBDListIndex].Protocol;
    gInitialConnect.NumECUs    = gOBDNumEcusResp;
    gInitialConnect.HeaderSize = gOBDList[gOBDListIndex].HeaderSize;

    for (EcuIndex=0; EcuIndex < gOBDNumEcusResp; EcuIndex++)
    {
        memcpy (gInitialConnect.Header[EcuIndex], gOBDResponse[EcuIndex].Header, 
                gOBDList[gOBDListIndex].HeaderSize);
    }

    return (PASS);
}

//*****************************************************************************
//
//	Function:	VerifyConnectInfo
//
//	Purpose:	Compare connection information for current connect with initial connect.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	1/5/05	initial
//
//*****************************************************************************
STATUS VerifyConnectInfo (void)
{
    unsigned long EcuIndex;

    /* same protocol ? */
    if (gOBDList[gOBDListIndex].Protocol != gInitialConnect.Protocol)
    {
        LogPrint("FAILURE: Different protocol than initial connect\n");
        return (FAIL);
    }

    /* same number of ECUs ? */
    if (gOBDNumEcusResp != gInitialConnect.NumECUs)
    {
        LogPrint("FAILURE: Number of ECUs responding different than number of ECUs from initial connect\n");
        return (FAIL);
    }

    /* check each ECU ID */
    for (EcuIndex=0; EcuIndex<gOBDNumEcusResp; EcuIndex++)
    {
        if (VerifyEcuID (gOBDResponse[EcuIndex].Header) == FAIL)
        {
            LogPrint("FAILURE: ECU IDs don't match IDs from initial connect\n");
            return (FAIL);
        }
    }

    return (PASS);
}

//*****************************************************************************
//
//	Function:	VerifyEcuID
//
//	Purpose:	Compare ECU ID with those from initial connect.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	1/5/05	initial
//
//*****************************************************************************
STATUS VerifyEcuID (unsigned char EcuId[])
{
    unsigned long Index;

    for (Index=0; Index<gInitialConnect.NumECUs; Index++)
    {
        if (memcmp (EcuId, gInitialConnect.Header[Index], gInitialConnect.HeaderSize) == 0)
           return (PASS);
    }

    return (FAIL);
}

//*****************************************************************************
//
//	Function:	GetEcuId
//
//	Purpose:	return ECU ID independent of protocol.
//
//*****************************************************************************
//
//	DATE		MODIFICATION
//	3/1/05	initial
//
//*****************************************************************************
unsigned int GetEcuId (unsigned int EcuIndex)
{
    unsigned int HeaderIndex, ID = 0;

    if (EcuIndex < gUserNumEcus)
    {
        if (gOBDList[gOBDListIndex].Protocol == ISO15765)
        {
            // ISO15765
            for (HeaderIndex=0; HeaderIndex < gOBDList[gOBDListIndex].HeaderSize; ++HeaderIndex)
            {
                ID = (ID << 8) | gOBDResponse[EcuIndex].Header[HeaderIndex];
            }
        }
        else
        {
            // J1850PWM
            // J1850VPW
            // ISO9141
            // ISO14230
            ID = gOBDResponse[EcuIndex].Header[2];
        }
    }

    return ID;
}
