
rem Here's how to compile kvTimeStampTester.c using Microsoft C (V2.0 or later)
cl  -I..\..\INC  kvTimeStampTester.c  ..\..\LIB\MS\canlib32.lib winmm.lib

rem And here's for Borland C++ 5.0. Uncomment it if you want to use it.
rem bcc32 -I..\..\INC kvTimeStampTester.c ..\..\LIB\Borland\canlib32.lib