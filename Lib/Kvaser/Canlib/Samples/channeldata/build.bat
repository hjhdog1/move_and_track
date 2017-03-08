rem
rem Here's how to compile channeldata.c using Microsoft C (V2.0 or later)
rem This was tested using MSC 6.0.
rem 
cl -I..\..\INC  channeldata.c  ..\..\LIB\MS\canlib32.lib

rem And here's for Borland C++ 5.0 or later. Uncomment it if you want to use it.
rem bcc32 -I..\..\INC channeldata.c ..\..\LIB\Borland\canlib32.lib