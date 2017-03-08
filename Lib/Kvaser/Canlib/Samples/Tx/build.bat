rem
rem Here's how to compile tx.c using Microsoft C (V2.0 or later)
rem This was tested using MSC 2.0.
rem 
cl -I..\..\INC  tx.c  ..\..\LIB\MS\canlib32.lib winmm.lib

rem And here's for Borland C++ 5.0. Uncomment it if you want to use it.
rem bcc32 -I..\..\INC tx.c ..\..\LIB\Borland\canlib32.lib