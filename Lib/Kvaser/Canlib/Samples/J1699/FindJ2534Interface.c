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
STATUS J2534List (char DeviceList[MAX_J2534_DEVICES][300], char LibraryList[MAX_J2534_DEVICES][300], unsigned long *ListIndex);
STATUS J2534LoadApi ( char *szLibrary);
unsigned long J2534Select (char DeviceList[MAX_J2534_DEVICES][300], char LibraryList[MAX_J2534_DEVICES][300], unsigned long ListIndex);

/*
*******************************************************************************
** FindJ2534Interface -
** Function to find appropriate J2534 interface and load the DLL
*******************************************************************************
*/
STATUS FindJ2534Interface (void)
{
    unsigned long DeviceIndex;
    unsigned long ListIndex;
    char DeviceList[MAX_J2534_DEVICES][300];
    char LibraryList[MAX_J2534_DEVICES][300];

	/* Acquire installed J2534 interface list. */
	if ( J2534List( DeviceList, LibraryList, &ListIndex ) != PASS )
	{
		return ( FAIL );
	}

    /* Present list to user to choose appropriate tool (if more than one) */
    if (ListIndex == 0)
    {
        LogPrint("\n\nFAILURE: No interfaces found in registry\n");
        return(FAIL);
    }
	else
	{
		/* Select J2534 interface */
		DeviceIndex = J2534Select( DeviceList, LibraryList, ListIndex );
	}

    /* Load appropriate DLL and attach to API functions "*/
    LogPrint("\nINFORMATION: Loading %s library\n\n", LibraryList[DeviceIndex]);

	/* Attach & load vendor supplied J2534 interface. */
	return ( J2534LoadApi( LibraryList[DeviceIndex] ) );  
}

/*****************************************************************************/
//
//	Function:	J2534LoadApi
//
//	Purpose:	This function will load user specified J2534 api & assign
//				usage to global data defined w/in this module.  When applied
//				to an API (DLL) these references will be contained w/in the
//				DLL.
//
/*****************************************************************************/
//
//	Date		Modification
//	10/25/03	Created function
//
/*****************************************************************************/
STATUS J2534LoadApi (char *szLibrary)
{
	HINSTANCE hDLL;

	/**********************************************************************/
	//
	//	Once user selects the specifie API & hardware, load the vendor
	//	supplied API calls.
	//
	/**********************************************************************/

    if ((hDLL = LoadLibrary( szLibrary ) ) == NULL)
    {
        LogPrint("FAILURE: Cannot load %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruConnect = (PTCONNECT)GetProcAddress(hDLL, "PassThruConnect")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruConnect function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruDisconnect = (PTDISCONNECT)GetProcAddress(hDLL, "PassThruDisconnect")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruDisconnect function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruReadMsgs = (PTREADMSGS)GetProcAddress(hDLL, "PassThruReadMsgs")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruReadMsgs function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruWriteMsgs = (PTWRITEMSGS)GetProcAddress(hDLL, "PassThruWriteMsgs")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruWriteMsgs function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruStartPeriodicMsg = (PTSTARTPERIODICMSG)GetProcAddress(hDLL, "PassThruStartPeriodicMsg")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruStartPeriodicMsg function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruStopPeriodicMsg = (PTSTOPPERIODICMSG)GetProcAddress(hDLL, "PassThruStopPeriodicMsg")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruStopPeriodicMsg function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruStartMsgFilter = (PTSTARTMSGFILTER)GetProcAddress(hDLL, "PassThruStartMsgFilter")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruStartMsgFilter function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruStopMsgFilter = (PTSTOPMSGFILTER)GetProcAddress(hDLL, "PassThruStopMsgFilter")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruStopMsgFilter function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruSetProgrammingVoltage = (PTSETPROGRAMMINGVOLTAGE)GetProcAddress(hDLL, "PassThruSetProgrammingVoltage")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruSetProgrammingVoltage function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruReadVersion = (PTREADVERSION)GetProcAddress(hDLL, "PassThruReadVersion")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruReadVersion function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruGetLastError = (PTGETLASTERROR)GetProcAddress(hDLL, "PassThruGetLastError")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruGetLastError function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruIoctl = (PTIOCTL)GetProcAddress(hDLL, "PassThruIoctl")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruIoctl function in %s\n", szLibrary );
        return(FAIL);
    }

    if ((PassThruOpen = (PTOPEN)GetProcAddress(hDLL, "PassThruOpen")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruOpen function in %s\n", szLibrary );
        return(FAIL);
    }
    if ((PassThruClose = (PTCLOSE)GetProcAddress(hDLL, "PassThruClose")) == NULL)
    {
        LogPrint("FAILURE: Cannot find PassThruClose function in %s\n", szLibrary );
        return(FAIL);
    }

	return( PASS );
}

/*****************************************************************************/
//
//	Function:	J2534List
//
//	Purpose:	Read system regestry and populate list of available 
//				interfaces.
//
/*****************************************************************************/
//
//	Date		Modification
//	10/25/03	Created function
//  1/10/06     Modified to support J2543 v4.04 registry spec.
//
/*****************************************************************************/
static char* szPROTOCOLS[] = {"CAN", "ISO9141", "ISO14230", "ISO15765", "J1850VPW", "J1850PWM"};
#define NUM_PROTOCOLS  sizeof(szPROTOCOLS)/sizeof(szPROTOCOLS[0])

STATUS J2534List (char DeviceList[MAX_J2534_DEVICES][300],
				  char LibraryList[MAX_J2534_DEVICES][300], unsigned long *ListIndex)
{
    unsigned long DeviceIndex, ProtocolIndex;
    HKEY          hKey1, hKey2, hKey3;
    DWORD         KeySize, ProtocolFlag;
    BYTE          KeyValue[240];
    FILETIME      lastTimeWritten;

    *ListIndex = 0;

    /* Find all available interfaces in the registry */
    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Software", 0, KEY_READ, &hKey1) != ERROR_SUCCESS)
    {
        LogPrint ("FAILURE: Cannot open HKEY_LOCAL_MACHINE->Software key\n");
        return (FAIL);
    }
    if (RegOpenKeyEx (hKey1, "PassThruSupport.04.04", 0, KEY_READ, &hKey2) != ERROR_SUCCESS)
    {
        LogPrint ("FAILURE:  Cannot open HKEY_LOCAL_MACHINE->Software->PassThruSupport key\n");
        return (FAIL);
    }

    /* Check for up to MAX_J2534_DEVICES vendors */
    for (DeviceIndex = 0; DeviceIndex < MAX_J2534_DEVICES; DeviceIndex++)
    {
        KeySize = sizeof(KeyValue);
        if (RegEnumKeyEx (hKey2, DeviceIndex, KeyValue, &KeySize, NULL, NULL, NULL, &lastTimeWritten) != ERROR_SUCCESS)
        {
            /* If no more vendors, stop looking */
            break;
        }

        if (RegOpenKeyEx(hKey2, KeyValue, 0, KEY_READ, &hKey3) != ERROR_SUCCESS)
        {
            break;
        }

        KeySize = sizeof(KeyValue);
        if (RegQueryValueEx (hKey3, "Name", 0, 0, KeyValue, &KeySize) == ERROR_SUCCESS)
        {
            strcpy (DeviceList[DeviceIndex], KeyValue);
            LogPrint ("INFORMATION: Device = %s\n", KeyValue);
        }

        KeyValue[0] = 0;
        for (ProtocolIndex = 0; ProtocolIndex < NUM_PROTOCOLS; ProtocolIndex++)
        {
            KeySize = sizeof(ProtocolFlag);
            if (RegQueryValueEx (hKey3, szPROTOCOLS[ProtocolIndex], 0, 0, (unsigned char *)&ProtocolFlag, &KeySize) == ERROR_SUCCESS)
            {
                if (ProtocolFlag != 0)
                {
                    strcat (KeyValue, szPROTOCOLS[ProtocolIndex]);
                    strcat (KeyValue, ",");
                }
            }
        }
        LogPrint("INFORMATION: ProtocolsSupported = %s\n", KeyValue);

        KeySize = sizeof(KeyValue);
        if (RegQueryValueEx(hKey3, "FunctionLibrary", 0, 0, KeyValue, &KeySize) == ERROR_SUCCESS)
        {
            strcpy (LibraryList[DeviceIndex], KeyValue);
        }

        RegCloseKey (hKey3);
    }

    *ListIndex = DeviceIndex;

    RegCloseKey(hKey2);
    RegCloseKey(hKey1);

	return (PASS);
}

/*****************************************************************************/
//
//	Function:	J2534Select
//
//	Purpose:	This function will prompt user to select from the installed
//				J2534 choices.
//
/*****************************************************************************/
//
//	Date		Modification
//	10/25/03	Created function
//
/*****************************************************************************/
unsigned long J2534Select (char DeviceList[MAX_J2534_DEVICES][300], 
						   char LibraryList[MAX_J2534_DEVICES][300], unsigned long ListIndex)
{
	unsigned long DeviceIndex;

    /* Present list to user to choose appropriate tool (if more than one) */
	if (ListIndex > 1)
    {
        LogPrint("\n\nPROMPT: Select tool from following list:\n\n");
        for (DeviceIndex = 0; DeviceIndex < ListIndex; DeviceIndex++)
        {
            LogPrint("    %d %s (%s)\n", (DeviceIndex + 1), DeviceList[DeviceIndex],
            LibraryList[DeviceIndex]);
        }
        LogPrint("\nEnter device number (1-%d): ", DeviceIndex);

        scanf ("%u", &DeviceIndex);
        LogPrint("%d\n", DeviceIndex);

        DeviceIndex -= 1;   // make zero-based index
    }
    else
    {
        DeviceIndex = 0;
    }

	/* Define default value, in the event out of range. */
    if (DeviceIndex >= ListIndex)
    {
        DeviceIndex = 0;
    }

	return (DeviceIndex);
}
