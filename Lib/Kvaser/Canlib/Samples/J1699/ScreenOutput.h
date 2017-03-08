/*
********************************************************************************
** SAE J1699-3 Test Source Code
**
**  Copyright (C) 2005 EnGenius. http://j1699-3.sourceforge.net/
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

//-----------------------------------------------------------------------------
// Screen data structures
//-----------------------------------------------------------------------------
typedef struct _StaticTextElement
{
   char * szLabel;
   short  X;
   short  Y;
} StaticTextElement;

typedef struct _DynamicValueElement
{
   short  X;
   short  Y;
   int    Width;
} DynamicValueElement;

//-----------------------------------------------------------------------------
// Dynamic screen handling functions
//-----------------------------------------------------------------------------
void init_screen (StaticTextElement elements[], int num_elements);
void update_screen_dec (DynamicValueElement elements[], int num_elements, int index, int value);
void update_screen_hex (DynamicValueElement elements[], int num_elements, int index, int value);
void update_screen_text (DynamicValueElement elements[], int num_elements, int index, const char *value);


//-----------------------------------------------------------------------------
// low-level screen functions
//-----------------------------------------------------------------------------
void gotoxy (short x, short y);
void setrgb (int color);
void clrscr ();
void get_cursor_pos (short * x, short * y);
