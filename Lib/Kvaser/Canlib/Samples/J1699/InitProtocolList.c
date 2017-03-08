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
#include <string.h>
#include <windows.h>
#include "j2534.h"
#include "j1699.h"

/*
*******************************************************************************
** Protocol Init and Permutation data
*******************************************************************************
*/

typedef struct
{
	unsigned long Protocol;
	unsigned long InitFlags;
	unsigned long HeaderSize;
	unsigned long BaudRate;
	char *        Name;
} PROTOCOL_INIT;

static PROTOCOL_INIT ProtocolInitData[] =
{
  // Protocol  Flags                 HeaderSize  Baud    Name
	{J1850PWM, 0,                    3,          41600,  "J1850PWM"},
	{J1850VPW, 0,                    3,          10400,  "J1850VPW"},
	{ISO9141,  ISO9141_K_LINE_ONLY,  3,          10400,  "ISO9141"},
	{ISO14230, FAST_INIT,            3,          10400,  "ISO14230 Fast Init"},
	{ISO14230, 0,                    3,          10400,  "ISO14230"},
	{ISO15765, 0,                    4,          500000, "ISO15765"},
	{ISO15765, CAN_29BIT_ID,         4,          500000, "ISO15765 29 Bit"}
};

#define NUM_PROTOCOL_INIT       (sizeof(ProtocolInitData)/sizeof(ProtocolInitData[0]))

/* NUM_PROTOCOL_INIT must be equal to OBD_MAX_PROTOCOLS */

#define J1850PWM_I              0
#define J1850VPW_I              1
#define ISO9141_I               2
#define ISO14230_FAST_INIT_I    3
#define ISO14230_I              4
#define ISO15765_I              5
#define ISO15765_29_BIT_I       6

static unsigned int ProtocolPerm[][NUM_PROTOCOL_INIT] =
{
	{J1850PWM_I, J1850VPW_I, ISO9141_I, ISO14230_FAST_INIT_I, ISO14230_I, ISO15765_I, ISO15765_29_BIT_I},  // 5.2, 10.1
	{J1850PWM_I, J1850VPW_I, ISO14230_FAST_INIT_I, ISO9141_I, ISO14230_I, ISO15765_29_BIT_I, ISO15765_I},  // 5.8
	{J1850PWM_I, J1850VPW_I, ISO14230_FAST_INIT_I, ISO14230_I, ISO9141_I, ISO15765_I, ISO15765_29_BIT_I},  // 6.2
	{J1850PWM_I, J1850VPW_I, ISO9141_I, ISO14230_I, ISO14230_FAST_INIT_I, ISO15765_29_BIT_I, ISO15765_I},  // 7.2
	{J1850VPW_I, J1850PWM_I, ISO9141_I, ISO14230_I, ISO14230_FAST_INIT_I, ISO15765_29_BIT_I, ISO15765_I},  // 8.2
	{ISO15765_I, ISO15765_29_BIT_I, J1850PWM_I, J1850VPW_I, ISO9141_I, ISO14230_FAST_INIT_I, ISO14230_I},  // 9.2
	{ISO15765_29_BIT_I, ISO15765_I, J1850PWM_I, J1850VPW_I, ISO9141_I, ISO14230_I, ISO14230_FAST_INIT_I},  // 9.9
	{J1850PWM_I, J1850VPW_I, ISO9141_I, ISO14230_I, ISO14230_FAST_INIT_I, ISO15765_29_BIT_I, ISO15765_I},  // 9.13
	{ISO15765_29_BIT_I, ISO15765_I, J1850VPW_I, J1850PWM_I, ISO9141_I, ISO14230_I, ISO14230_FAST_INIT_I},  // 9.17
	{ISO15765_I, ISO15765_29_BIT_I, J1850PWM_I, J1850VPW_I, ISO9141_I, ISO14230_FAST_INIT_I, ISO14230_I}   // 9.21
};

#define NUM_PROTOCOL_PERM   (sizeof(ProtocolPerm)/sizeof(ProtocolPerm[0]))

/*
*******************************************************************************
** InitProtocolList - Function to initialize the protocol list
*******************************************************************************
*/
void InitProtocolList(void)
{
	int i, j;

	/* initialize gOBDList */
	for (i=0; i<NUM_PROTOCOL_INIT; i++)
	{
		j = ProtocolPerm[gOBDProtocolOrder][i];

		gOBDList[i].Protocol   = ProtocolInitData[j].Protocol;
		gOBDList[i].InitFlags  = ProtocolInitData[j].InitFlags;
		gOBDList[i].HeaderSize = ProtocolInitData[j].HeaderSize;
		gOBDList[i].BaudRate   = ProtocolInitData[j].BaudRate;
		strcpy(gOBDList[i].Name, ProtocolInitData[j].Name);

		gOBDList[i].ChannelID  = 0;
	}

	/* If at end of order, start over */
	if (++gOBDProtocolOrder >= NUM_PROTOCOL_PERM)
	{
		gOBDProtocolOrder = 0;
	}
}
