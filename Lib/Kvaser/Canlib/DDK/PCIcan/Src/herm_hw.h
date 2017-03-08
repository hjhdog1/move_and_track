/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/

#ifndef _HERMES_HW_H_
#define _HERMES_HW_H_

#ifdef __WIN_NT__
BOOLEAN HERMESInterruptService(IN PKINTERRUPT interrupt,
                              IN OUT PVOID context);
Vstatus HERMESHwInitContext(PHERMEScontext pCtx,
                           PUCHAR portBase,
                           PVchannel* chan,
                           unsigned int numberOfChannels);
#endif


#ifdef __WIN_95__
Vstatus HERMESHwInitContext(PHERMEScontext pCtx,
                           WORD portBase,
                           PVchannel* chan,
                           unsigned int numberOfChannels);
int     HERMESInterruptService(PHERMEScontext pCtx);
#endif


Vstatus HERMESTransmitEvent(PVchannel pChan, PVevent pEvent, unsigned long tout);

void    HERMES_ConnectIRQ(PHERMEScontext pCtx, int chan, unsigned connect);
Vstatus HERMES_DetectCard(PHERMEScontext pCtx);
Vstatus HERMES_InitCard(PHERMEScontext pCtx);
Vstatus HERMES_KillCard(PHERMEScontext pCtx);
void    HERMES_EmergencyExit(PHERMEScontext pCtx, int chan);
int     HERMES_Interrupt(PHERMEScontext pCtx);


#endif
