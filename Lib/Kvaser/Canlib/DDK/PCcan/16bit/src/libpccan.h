/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _LIBPCCAN_H_
#define _LIBPCCAN_H_

/*
** "extern" definitions of all PCCAN routines.
*/

#ifdef __cplusplus
extern "C" {
#endif

PUBLIC canStatus PCCANlibInit(void);
PUBLIC canStatus PCCANlibExit(void);
PUBLIC canStatus PCCANLocateHardware(char *cardname);
PUBLIC canStatus PCCANInitCircuit(circData* h);
PUBLIC canStatus PCCANkillCircuit(circData* h);
PUBLIC canStatus PCCANInitCard(cardData* h);
PUBLIC canStatus PCCANkillCard(cardData* h);
PUBLIC canStatus PCCANbusOn(circData *h);
PUBLIC canStatus PCCANbusOff(circData *h);
PUBLIC canStatus PCCANbusParams(circData* h,
                          unsigned long freq,
						  unsigned int tseg1,
						  unsigned int tseg2,
                          unsigned int sjw,
						  unsigned int nr_samp,
						  unsigned int syncmode);
PUBLIC canStatus PCCANbusOutputControl(circData *h,
								 unsigned int drivertype);
PUBLIC canStatus PCCANaccept(circData* h,
					   long envelope,
					   unsigned int flags);
PUBLIC canStatus PCCANstatus(circData* h,
					   unsigned long* flags);
PUBLIC canStatus PCCANstartWrite(circData* h,
						   long id,
						   void* msgPtr,
                           unsigned int dlc,
						   unsigned int flag);
PUBLIC int PCCANTXAvailable(circData* h);
PUBLIC canStatus PCCANEnableExtended(circData* h,
							   unsigned int onoff);

#ifdef __cplusplus
}
#endif



#endif
