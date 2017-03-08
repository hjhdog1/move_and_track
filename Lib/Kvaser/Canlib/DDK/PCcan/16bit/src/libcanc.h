/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _LIBCANC_H_
#define _LIBCANC_H_

#ifdef __cplusplus
extern "C" {
#endif

PUBLIC canStatus CANClibInit(void);
PUBLIC canStatus CANClibExit(void);
PUBLIC canStatus CANCLocateHardware(char *cardname);
PUBLIC canStatus CANCInitCircuit(circData* h);
PUBLIC canStatus CANCkillCircuit(circData* h);
PUBLIC canStatus CANCInitCard(cardData* h);
PUBLIC canStatus CANCkillCard(cardData* h);
PUBLIC canStatus CANCbusOn(circData *h);
PUBLIC canStatus CANCbusOff(circData *h);
PUBLIC canStatus CANCbusParams(circData* h,
                         unsigned long freq, unsigned int tseg1,
						 unsigned int tseg2, unsigned int sjw,
						 unsigned int nr_samp, unsigned int syncmode);
PUBLIC canStatus CANCbusOutputControl(circData *h, unsigned int drivertype);
PUBLIC canStatus CANCaccept(circData* h, long envelope, unsigned int flags);
PUBLIC canStatus CANCstatus(circData* h, unsigned long* flags);
PUBLIC canStatus CANCstartWrite(circData* h, long id, void* msgPtr,
                          unsigned int dlc, unsigned int flag);
PUBLIC int CANCTXAvailable(circData* h);
PUBLIC canStatus CANCEnableExtended(circData* h, unsigned int onoff);



#endif

