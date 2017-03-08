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
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "ScreenOutput.h"

//-----------------------------------------------------------------------------
// Low-level screen functions
//-----------------------------------------------------------------------------
void gotoxy (short x, short y)
{
   HANDLE hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
   COORD position = { x, y }; 

   SetConsoleCursorPosition (hStdout, position);
}

void setrgb (int color)
{
   static WORD _attributes[] =
   {
      // 0 - White on Black
      FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,

      // 1 - Red on Black
      FOREGROUND_INTENSITY | FOREGROUND_RED,

      // 2 - Green on Black
      FOREGROUND_INTENSITY | FOREGROUND_GREEN, 

      // 3, Yellow on Black
      FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN, 

      // 4 - Blue on Black
      FOREGROUND_INTENSITY | FOREGROUND_BLUE, 

      // 5 - Magenta on Black
      FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE, 

      // 6 - Cyan on Black
      FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE, 

      // 7 - Black on Gray
      BACKGROUND_INTENSITY | FOREGROUND_INTENSITY, 

      // 8 - Black on White
      BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
      FOREGROUND_INTENSITY,

      // 9 - Red on White
      BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
      FOREGROUND_INTENSITY | FOREGROUND_RED,

      // 10 - Green on White
      BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
      FOREGROUND_INTENSITY | FOREGROUND_GREEN,

      // 11 - Yellow on White
      BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | 
      FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN, 

      // 12 - Blue on White
      BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
      FOREGROUND_INTENSITY | FOREGROUND_BLUE, 

      // 13 - Magenta on White
      BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | 
      FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE, 

      // 14 - Cyan on White
      BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | 
      FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE, 

      // 15 - White on White
      BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | 
      FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 
   };

   WORD attrib  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

   if (0 <= color && color < sizeof(_attributes)/sizeof(_attributes[0]))
      attrib = _attributes[color];

   SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), attrib);
}

void clrscr ()
{ 
   COORD coordScreen = { 0, 0 }; /* here's where we'll home the cursor */ 
   DWORD cCharsWritten; 
   CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */ 
   DWORD dwConSize; /* number of character cells in the current buffer */ 
 
   /* get the output console handle */
   HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);

   /* get the number of character cells in the current buffer */ 
   GetConsoleScreenBufferInfo (hConsole, &csbi); 
   dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 

   /* fill the entire screen with blanks */ 
   FillConsoleOutputCharacter (hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten); 

   /* get the current text attribute */
   setrgb (-1);
   GetConsoleScreenBufferInfo (hConsole, &csbi); 

   /* now set the buffer's attributes accordingly */ 
   FillConsoleOutputAttribute (hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten); 

   /* put the cursor at (0, 0) */ 
   SetConsoleCursorPosition (hConsole, coordScreen); 
}

void get_cursor_pos (short * x, short * y)
{
   CONSOLE_SCREEN_BUFFER_INFO csbi;

   GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi);

   *x = csbi.dwCursorPosition.X;
   *y = csbi.dwCursorPosition.Y;
}

//-----------------------------------------------------------------------------
// Dynamic screen handling functions
//-----------------------------------------------------------------------------
void init_screen (StaticTextElement elements[], int num_elements)
{
   int i;
   clrscr ();

   for (i=0; i<num_elements; i++)
   {
      gotoxy (elements[i].X, elements[i].Y);
      printf (elements[i].szLabel);
   }
}

void update_screen_dec (DynamicValueElement elements[], int num_elements, int index, int value)
{
   short x, y;

   if (0 <= index && index < num_elements)
   {
      get_cursor_pos (&x, &y);

      gotoxy (elements[index].X, elements[index].Y);
      printf ("%-*d", elements[index].Width, value);

      gotoxy (x, y);
   }
}

void update_screen_hex (DynamicValueElement elements[], int num_elements, int index, int value)
{
   short x, y;

   if (0 <= index && index < num_elements)
   {
      get_cursor_pos (&x, &y);

      gotoxy (elements[index].X, elements[index].Y);
      printf ("%-*X", elements[index].Width, value);

      gotoxy (x, y);
   }
}

void update_screen_text (DynamicValueElement elements[], int num_elements, int index, const char *value)
{
   short x, y;

   if (0 <= index && index < num_elements)
   {
      get_cursor_pos (&x, &y);

      gotoxy (elements[index].X, elements[index].Y);
      printf ("%-*s", elements[index].Width, value);

      gotoxy (x, y);
   }
}

