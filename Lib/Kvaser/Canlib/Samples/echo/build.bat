rem
rem Here's how to compile canecho.c using Microsoft C (V2.0 or later)
rem 
cl -I..\..\INC  canecho.c  ..\..\LIB\MSC\canlib32.lib

rem And here's for Borland C++ 5.0. Uncomment it if you want to use it.
rem bcc32 -I..\..\INC canecho.c ..\..\LIB\Borland\canlib32.lib