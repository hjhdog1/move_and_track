User data in Kvaser Leaf etc.
=============================

Setting user data works with Kvaser Leaf (of all types), Kvaser USBcan
Professional, Kvaser Memorator Professional and Kvaser Memorator
Light.


Drivers and firmware
====================

You will need canlib 4.1 (or later) and firmware 1.8 (or later).


Reading user data
=================

Use

  kvReadDeviceCustomerData(int hnd, 
                           int userNumber,
                           int itemNumber,
                           void *data, 
                           size_t bufsiz)

to read user data.

userNumber is assigned by Kvaser.

itemnumber is presently not implemented. Set to zero.

data and bufsiz points to a buffer of bufsiz bytes (max 8 bytes) where the result
will be placed.

Note: anyone can read the user data. The writing is protected by
a password but reading is not.


Writing user data
=================

To write user data, use filo_setparam_userdata.exe. It's a command-line program.

c:\>filo_setparam_userdata.exe -?

Kvaser Leaf Flash Programming Utility
(C) 2002-2008 KVASER AB, SWEDEN -  www.kvaser.com
-----------------------------------------------------------------------
Usage:
  setparam -[flagX] -[flagX] ...
  Flags:
  -Cn          |  Program device number n.
  -data=XXXX   |  XXXX is a 16-character hexadecimal string.
  -v           |  Verbose mode.
  -s           |  Silent mode.
  -pwdX!xxxxxx |  xxxxxx is a six character password for user X

Notes:
  * The order of the flags doesn't matter.

Example:

filo_setparam_userdata.exe -C0 -data=0102030405060708 -v -pwd100!upHXy1


Maximum number of bytes to write
================================
Presently you cannot write more than 8 bytes of data.


Maximum number of write cycles
==============================

The user data is stored in a flash memory and the number of write cycles is limited.
The ideal application is to write the data once, or a few times. A license key
that your software validates would be a typical way of using this feature.



User number and password for test and development
=================================================

Use user number = 100 and password = upHXy1 (uniform papa hotel xray yankee one)

If you decide to use this user number and password for "production
use", please remember that the password is known to everyone.


Obtaining your own password
===========================

You have to sign an NDA with Kvaser. Please contact sales@kvaser.com.
