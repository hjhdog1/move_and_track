rem
rem Here's how to compile simple.c using Microsoft C (V2.0 or later)
rem 
cl -I..\..\INC  swc.c  ..\..\LIB\MSC\canlib32.lib

rem And here's for Borland C++. Uncomment it if you want to use it.
rem bcc32 -I..\..\INC swc.c ..\..\LIB\Borland\canlib32.lib
