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
** LogMsg - Function to log a message
*******************************************************************************
*/
void LogMsg (PASSTHRU_MSG *Msg, unsigned long Flags)
{
	char LogBuffer[1024];
	unsigned long ByteIndex;
	unsigned long BufferIndex;
	unsigned long tsNow;

	tsNow = GetTickCount();
	BufferIndex = 0;

	if (Flags & LOG_REQ_MSG)
	{
		/*
		** For local messages, set ExtraDataIndex to message size.
		*/
		Msg->ExtraDataIndex = Msg->DataSize;
		BufferIndex += sprintf(&LogBuffer[BufferIndex], "+%06ldms REQ MSG:  ", tsNow - gLastLogTime);
	}
	else if (Msg->RxStatus & TX_MSG_TYPE)
	{
		BufferIndex += sprintf(&LogBuffer[BufferIndex], "+%06ldms TX MSG: %10luusec ", tsNow - gLastLogTime, Msg->Timestamp);
	}
	else
	{
		BufferIndex += sprintf(&LogBuffer[BufferIndex], "+%06ldms RX MSG: %10luusec ", tsNow - gLastLogTime, Msg->Timestamp);
	}

	gLastLogTime = tsNow;

	switch (Msg->ProtocolID)
	{
		case J1850VPW:
		{
			BufferIndex += sprintf(&LogBuffer[BufferIndex], "J1850VPW ");
		}
		break;
		case J1850PWM:
		{
			BufferIndex += sprintf(&LogBuffer[BufferIndex], "J1850PWM ");
		}
		break;
		case ISO9141:
		{
			BufferIndex += sprintf(&LogBuffer[BufferIndex], "ISO9141 ");
		}
		break;
		case ISO14230:
		{
			BufferIndex += sprintf(&LogBuffer[BufferIndex], "ISO14230 ");
		}
		break;
		case ISO15765:
		{
			BufferIndex += sprintf(&LogBuffer[BufferIndex], "ISO15765 ");
		}
		break;
		default:
		{
			/* Unsupported protocol */
		}
		break;
	}

	if ((Msg->ProtocolID == ISO15765) && (Msg->RxStatus & ISO15765_FIRST_FRAME))
	{
		BufferIndex += sprintf(&LogBuffer[BufferIndex], "FirstFrame Indication ");
	}
	if ((Msg->ProtocolID == ISO9141 || Msg->ProtocolID == ISO14230) && (Msg->RxStatus & START_OF_MESSAGE))
	{
		BufferIndex += sprintf(&LogBuffer[BufferIndex], "Start of Message ");
	}

	if (Msg->RxStatus & RX_BREAK)
	{
		BufferIndex += sprintf(&LogBuffer[BufferIndex], "BREAK Indication ");
	}
	for (ByteIndex = 0; (ByteIndex < Msg->ExtraDataIndex) && (ByteIndex < 128); ByteIndex++)
	{
		BufferIndex += sprintf(&LogBuffer[BufferIndex], "%02X ", Msg->Data[ByteIndex]);
	}

	if ((Msg->ProtocolID == ISO9141) || (Msg->ProtocolID == ISO14230))
	{
		if (Msg->ExtraDataIndex < Msg->DataSize)
		{
			// log ISO9141/ISO14230 checksum
			BufferIndex += sprintf(&LogBuffer[BufferIndex], "(%02X) ", Msg->Data[Msg->ExtraDataIndex]);
		}
	}

	BufferIndex += sprintf(&LogBuffer[BufferIndex], "\n");

	if (gSuspendLogOutput == TRUE)
	{
		if (gLogBufferCopyIndex + BufferIndex < sizeof (gszLogBufferCopy))
		{
			strcpy (&gszLogBufferCopy[gLogBufferCopyIndex], LogBuffer);
			gLogBufferCopyIndex += BufferIndex;
		}
	}
	else
	{
		fputs(LogBuffer, ghLogFile);
		fflush(ghLogFile);
	}
}

void LogMsgCopy (void)
{
	fwrite (gszLogBufferCopy, 1, gLogBufferCopyIndex, ghLogFile);

	fflush(ghLogFile);

	ClearLogMsgCopy ();
}

void ClearLogMsgCopy (void)
{
	gszLogBufferCopy[0] = 0;
	gLogBufferCopyIndex = 0;
}

/*
*******************************************************************************
** ERROR_RETURN functions
*******************************************************************************
*/
unsigned long gErrorCount = 0;
unsigned long gERFlags = 0;

STATUS ErrorReturn (void)
{
char Ret;

	gErrorCount++;

	// if haven't selected continue on all failures, do normal prompt
	if ((gERFlags & ER_BYPASS_USER_PROMPT) == 0x00)
	{
		Ret = LogUserPrompt("Failure detected, Continue?\n", YES_NO_ALL_PROMPT);

		// No Continue
		if ( Ret == 'N' )
		{
			return FAIL;
		}

		// All Continue
		else if ( Ret == 'A' )
		{
			gERFlags |= ER_BYPASS_USER_PROMPT;
		}

		return PASS;
	}
	// if have selected to continued on All failures 
	// and not temporarily disabled prompts (ie SidRequest),
	// don't wait for a user response
	else if ( (gERFlags & ER_CONTINUE) == 0x00 )
	{
		Ret = LogUserPrompt( "Failure detected, Continue?\n", NO_WAIT_PROMPT);
	}

	if ( (gERFlags & ER_CONTINUE) != 0x00 )
	{
		return PASS;
	}

	return FAIL;
}


unsigned long ErrorCount (void)
{
	unsigned long count = gErrorCount;
	if ( (gERFlags & ER_DONT_CLEAR_ERROR_COUNT) == 0 )
	{
		gErrorCount = 0;
	}
	return count;
}


unsigned long ErrorFlags (unsigned long newflags)
{
	unsigned long flags = gERFlags;
	gERFlags = newflags;
	return flags;
}


unsigned char TestContinue ( char *PromptString )
{
char Ret;

	gOBDTestFailed = TRUE;

	// if haven't selected continue on all failures, do normal prompt
	if ((gERFlags & ER_BYPASS_USER_PROMPT) == 0)
	{
		Ret = LogUserPrompt( PromptString, YES_NO_ALL_PROMPT);
	}
	// if have selected to continued on All failures, don't wait for a user response
	else
	{
		Ret = LogUserPrompt( PromptString, NO_WAIT_PROMPT);
	}

	// if Yes or All Continue
	if ( Ret == 'A' || Ret == 'Y' )
	{
		if ( Ret == 'A' )
		{
			gERFlags |= ER_BYPASS_USER_PROMPT;
			Ret = 'Y';
		}
		gOBDFailureBypassed = TRUE;
	}

	return Ret;
}


unsigned char WarnContinue ( char *PromptString )
{
char Ret;

	// if haven't selected continue on All failures, do normal prompt
	if ((gERFlags & ER_BYPASS_USER_PROMPT) == 0)
	{
		Ret = LogUserPrompt( PromptString, YES_NO_ALL_PROMPT);
	}
	// if have selected to continued on All failures, don't wait for a user response
	else
	{
		Ret = LogUserPrompt( PromptString, NO_WAIT_PROMPT);
	}

	// if Yes or All Continue
	if ( Ret == 'A' )
	{
		gERFlags |= ER_BYPASS_USER_PROMPT;
		Ret = 'Y';
	}

	return Ret;
}
