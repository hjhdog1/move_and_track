rem
rem Here's how to compile candump.c using Microsoft C (V2.0 or later)
rem This was tested using MSC 2.0.
rem 
cl -I..\..\INC  candump.c  ..\..\LIB\MS\canlib32.lib

rem And here's for Borland C++ 5.0. Uncomment it if you want to use it.
rem bcc32 -I..\..\INC candump.c ..\..\LIB\Borland\canlib32.lib