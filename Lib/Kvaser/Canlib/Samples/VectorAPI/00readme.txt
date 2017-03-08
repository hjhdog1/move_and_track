The following files are provided to facilitate porting your
application from Vector's hardware to Kvaser hardware, if your
application already uses Vector's driver API.

To make your application run on Kvaser hardware instead,
observe the following points:


- Windows 95 is not supported.

- Look through your code and search for any occurrences of code that
  test the CAN hardware type. Such code should be reviewed and modified
  if needed (Kvaser hardware has other HWTYPE_xxx types - with the
  exception for LAPcan which has the same HWTYPE_xxx as CANcardX.)

- Include the vcand.h provided here, recompile and relink using the
  import libraries found here - vcandm32.lib for VC++ and vcand32.lib
  for Borland compilers.

- Place the vcand32.dll provided in CANLIB SDK in the same directory
  as your application.

- Your application should now run without errors.



A simple sample, cancount.c, is included for your reference. For more information see cancount.txt.
