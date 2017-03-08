rem
rem Here's how to compile simple.c using Microsoft C (V2.0 or later)
rem This was tested using MSC 2.0.
rem 
rc main.rc
cl -I..\..\..\INC  simple.c main.c ..\..\..\LIB\MS\canlib32.lib user32.lib gdi32.lib main.res
