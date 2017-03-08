/*
**                         Copyright 2000 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
**
** Description:
**
**
*/
#ifndef _HERMES_XILINX_H_
#define _HERMES_XILINX_H_


//
// These register values are valid for revision 14 of the Xilinx logic.
//
#define XILINX_OUTA         0   // Bit 7 used to write bits to serial memory DS2430.
#define XILINX_INA          1   // Bit 7 used to read back bits from serial memory DS2430.
#define XILINX_CTRLA        2	// Sets the function of the Xilinx pins normally set to zero.
#define XILINX_UNUSED       3
#define XILINX_OUTB         4   // Bit 7 used to reset the SJA-1000.
#define XILINX_INB          5   // Bit 7 used to read back the reset line value.
#define XILINX_CTRLB		6   // Sets the function of the Xilinx pins normally set to zero.
#define XILINX_VERINT       7   // Lower nibble simulate interrupts, high nibble version number.


#define XILINX_PRESUMED_VERSION     14



#define 

#endif
