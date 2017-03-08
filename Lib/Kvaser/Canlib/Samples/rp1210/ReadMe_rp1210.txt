About the RP1210 ini files
===========================================================================

Here are two sample ini files, kv121032.ini and RP121032.ini, that
must be adapted to your installation before the RP1210 DLL can be
used.

One of each of these .ini files must exist in the directory Windows
was installed in (which is, typically, C:\Windows.)

The RP121032.ini file may have other vendor's information in it.
Kvaser information should be added to any existing version of the
file.  A new file with the Kvaser information should be created if no
previous version of the file exists.

The KV121032.ini file represents Kvaser specific device information.
The file should contain a Device entry for each channel provided by
the hardware (eg. LAPcan card should provide 2 device entries, one for
each channel).  There should also be a Protocol entry for each
protocol supported.

We have used the DeviceParams field to assign a Device Id to a
specific physical channel on the device.  This is done with the
parameter "Chan" which can have values of 0 upto the maximum channels
on the card minus 1.  This means the first channel on a LAPcan card is
"Chan=0" and the second channel on the same card is "Chan=1".

We have also used the DeviceParams field to assign a Device Id to a
specific card of the same type.  This is done with the parameter
"Serial" which must match the serial number value returned from the
card.  This number can be seen through the Control Panel "Kvaser
Hardware" applet by highlighting one of the card's channels in the
tree pane.  The serial number is then presented in the right pane,
together with other useful information.

