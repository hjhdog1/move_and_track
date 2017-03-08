rem
rem Here's how to compile kvrConnect.c using Microsoft C (V2.0 or later)
rem This was tested using Microsoft Visual Studio .NET 2003.
rem 
cl -I..\..\INC  kvrConnect.c  ..\..\LIB\MS\kvrlib.lib ..\..\LIB\MS\canlib32.lib

rem And here's for Borland C++ 5.0. Uncomment it if you want to use it.
rem bcc32 -I..\..\INC kvrConnect.c ..\..\LIB\Borland\kvrlib.lib ..\..\LIB\Borland\canlib32.lib