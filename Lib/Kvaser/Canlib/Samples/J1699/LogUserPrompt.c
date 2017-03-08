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
#include <conio.h>
#include <time.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

/*
*******************************************************************************
** LogUserPrompt - Function to instruct the user and prompt for a response
*******************************************************************************
*/
char LogUserPrompt(char *PromptString, unsigned long PromptType)
{
	char PrintBuffer[1024];
	char UserResponse[1024];
	char LogBuffer[1024];
	unsigned long PrintIndex;

	/* Send the prompt to the user */
	switch(PromptType)
	{
		break;
		case YES_NO_PROMPT:
		{
			sprintf (PrintBuffer, "\n\nPROMPT: %s (Enter Yes or No): ", PromptString);
			printf ("%s", PrintBuffer);
		}
		break;
		case YES_NO_ALL_PROMPT:
		case NO_WAIT_PROMPT:
		{
			sprintf (PrintBuffer, "\n\nPROMPT: %s (Enter Yes, No or All yes): ", PromptString);
			printf ("%s", PrintBuffer);
		}
		break;
		case ENTER_PROMPT:
		default:
		{
			sprintf (PrintBuffer, "\n\nPROMPT: %s (Press Enter): ", PromptString);
			printf ("%s", PrintBuffer);
		}
	}

	/* Get the user response and log it */
	if ( PromptType == NO_WAIT_PROMPT )
	{
		sprintf( UserResponse, "All" );
	}
	else
	{
		clear_keyboard_buffer ();
		gets (UserResponse);
	}

	sprintf (&PrintBuffer[strlen(PrintBuffer)], "%s\n", UserResponse);

	/* Get rid of any control characters or spaces at the beginning of the string before logging */
	for ( PrintIndex = 0;
	      (PrintIndex < sizeof(PrintBuffer)) &&
	      (PrintBuffer[PrintIndex] <= ' ') &&
	      (PrintBuffer[PrintIndex] != '\0');
	      PrintIndex++ ) {}

	/* Add the timestamp and put the string in the log file */
	sprintf (LogBuffer, &PrintBuffer[PrintIndex]);
	fputs (LogBuffer, ghLogFile);
	fflush (ghLogFile);

	/* Return first character of user response and remove ASCII lowercase bit */
	return (UserResponse[0] & 0xDF);
}

/*
*******************************************************************************
** clear_keyboard_buffer - remove all pending data from input buffer
*******************************************************************************
*/
void clear_keyboard_buffer (void)
{
	while (_kbhit()) {_getch();}
	fflush(stdin);
}
