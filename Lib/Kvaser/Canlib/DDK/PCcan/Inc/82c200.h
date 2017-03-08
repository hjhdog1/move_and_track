/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _82C200_H_
#define _82C200_H_

#define CCNTRL          0                   /* Control register */
#define SPD             0x40                /* Speed mode; set = slow */
#define SPEED_SLOW      0x40                /* Alternate name */
#define OIE             0x10
#define EIE             0x08
#define TIE             0x04
#define RIE             0x02
#define RR              0x01

#define CCOM            1
#define RX0             0x80
#define RX1             0x40
#define COMPSEL         0x20
#define SLEEP           0x10
#define COS             0x08
#define RRB             0x04
#define AT              0x02
#define TR              0x01

#define CSTAT           2
#define BS              0x80
#define ES              0x40
#define TS              0x20
#define RS              0x10
#define TCS             0x08
#define TBA             0x04
#define DO              0x02
#define RBS             0x01

#define CINT            3
#define WIF             0x10
#define OIF             0x08
#define EIF             0x04
#define TIF             0x02
#define RIF             0x01

#define CACC            4
#define CACM            5
#define CBT0            6
#define CBT1            7
#define COCNTRL         8

#define OCTP1           0x80
#define OCTN1           0x40
#define OCPOL1          0x20
#define OCTP0           0x10
#define OCTN0           0x08
#define OCPOL0          0x04
#define OCM1            0x02
#define OCM0            0x01

#define TBI             0x0A
#define TRTDL           0x0B
#define TDS0            0x0C
#define TDS1            0x0D
#define TDS2            0x0E
#define TDS3            0x0F
#define TDS4            0x10
#define TDS5            0x11
#define TDS6            0x12
#define TDS7            0x13

#define RBI             0x14
#define RRTDL           0x15
#define RDS0            0x16
#define RDS1            0x17
#define RDS2            0x18
#define RDS3            0x19
#define RDS4            0x1A
#define RDS5            0x1B
#define RDS6            0x1C
#define RDS7            0x1D

#endif
