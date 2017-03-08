Visual Basic sample program
===========================

This program demostrates how to initialize the hardware, send and receive messages and how to get hardware information and bus parameter information. 

The program is just intended as an example of how to use CANLIB. It's not
performance optimized in any way. It may, or may not, be useful
for you.

The program is written in Microsoft Visual Studio.NET 2003.

Building the sample program
---------------------------
Open the VBSample.vbproj file in Visual Studio.NET.

To be able to build the sample program you must make sure that the path to "canlibCLSNET.dll" in Visual Studio is correct. Do so by using:
Project->VBSample Properties->Common Properties->Reference Path

Remember to make sure canlibCLSNET.dll is somewhere in the path as it is required to run the application.

It is now posible to build the project by using: 
Build->Build solution

You can now run the program by using:

Debug->Start


