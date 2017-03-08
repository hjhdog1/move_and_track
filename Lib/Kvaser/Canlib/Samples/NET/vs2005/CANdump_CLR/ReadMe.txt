This sample demonstrates how to build a program that uses managed C++
code.

Visual Studio 2005 (or later) is required.

This is a command-line program. It works in the same way as its
non-managed C counterpart, candump.

You will probably need to adjust the canlibCLSNET assembly reference
in the project file.  To do this, 
* select Project | CLRcandump Properies... from the Visual Studio menu.  
* Select canlibCLSNET in the list of references, 
  o  remove it, and 
  o  add it using the Add New Reeference... button. 
     A dialog appears; select the Browse tab and point VS2005 to the place 
     where canlibCLSNET.dll is installed (e.g. in the bin directory of canlib SDK).

