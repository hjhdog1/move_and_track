/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _PCCAN_H_
#define _PCCAN_H_

#define PHILIPS_OFFSET      0x4000U
#define XILINX_OFFSET       0x6000U
#define INTEL_OFFSET        0x2000U


/*
** Registers in 1.0B-3:
** 00 IRQ control.
** 01 Reset.
** 02 Interrupt status and clock control.
** 03 Formerly T-CANnector, used for Dallas on PCcan 2.0.
**
*/

#define XREG_IRQ            0

#define XREG_CTRL           1
#define XREG_CTRL_NORESET   1

// "2" is valid for 1.0B-3
// Older PCCAN cards have the interrupt register at address 0.
#define XREG_INTERRUPT      2
#define XREG_CLOCK          XREG_INTERRUPT
#define XREG_CLOCK_ONBOARD  0x80

#define XREG_MISCBITS       2
#define XREG_MISC_SDOUT     0x00
#define XREG_MISC_SDIN      0x10


#endif

