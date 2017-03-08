
ABOUT THE SOURCE CODE
=====================

The code here are snippets from our
16-bit (DOS, Windows 3.1) and 32-bit (Windows 95, NT) drivers for the
PCcan card.

There are also snippets from the driver for the PCIcan card.

The sources are not complete but should serve as an example on how to
program the card and the circuits.  The necessary include files are
in the INC directory at the topmost level in the CANLIB SDK.

Let us repeat this: the code is NOT complete and will NOT compile
right out of the box, but you will most likely find useful code
fragments here.

In addition to the source code, you will also need the following documents:

For PCcan:
- The PCcan manual (www.kvaser.com)
- the SJA1000 data booklet (Philips, but also at www.kvaser.com)

For PCIcan:
- The PCIcan manual (www.kvaser.com)
- the SJA1000 data booklet (Philips, but also at www.kvaser.com)
- The AMCC S5920 data book
- General PCI knowledge

You will probably start with the 16-bit source where the major files are:
* libpccan.c which contains all the PCcan specific code
* canlib.c which contains (mostly) hardware-independent code such as queues and so on.

The 16-bit driver runs the SJA1000 in 82c200 mode. Have a look in
32bit/src/pcc1000.c for examples on how to program the SJA1000 in
pelican mode.

Include files of particular interest are
- canlib.h
- canstat.h
- sja1000.h
- pccan.h
- 82c200.h
