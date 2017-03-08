rem
rem Here's how to compile using Microsoft C
rem 
cl -I..\..\INC  envvar.c  ..\..\LIB\MSC\canlib32.lib

rem The script must also be compiled, using scc.exe in canlib sdk.
..\..\bin\scc test1.t

rem And here's for Borland/CodeGear/Embarcadero C++. Uncomment it if you want to use it.
rem bcc32 -I..\..\INC envvar.cpp ..\..\LIB\Borland\canlib32.lib
