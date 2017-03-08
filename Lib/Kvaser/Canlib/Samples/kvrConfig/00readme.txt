CANLIB Sample Program : kvrConfig.exe
=====================================

This is a 32-bit Windows console mode program that can scan for available
networks, configure a local (USB) device, and use the new configuration
to establish a connection to WLAN. The program requires the dll and 
dtd files located in the bin directory of canlib SDK.

The program is just intended as an example of how to use KVRLIB. It's not
performance optimized in any way. It may, or may not, be useful for you.

The program is written in C and has been tested with Borland and
Microsoft compilers.

Type 'kvrConfig.exe' to list all available devices and type 'kvrConfig.exe 2'
to configure the device on CAN channel 2.
