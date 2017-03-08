/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/

#ifndef _PCCHW_H_
#define _PCCHW_H_

#ifdef __WIN_NT__
BOOLEAN PCCANInterruptService(IN PKINTERRUPT interrupt,
                              IN OUT PVOID context);
Vstatus PCCANHwInitContext(PPCCANcontext pCtx,
                           PUCHAR portBase,
                           PVchannel* chan,
                           unsigned int numberOfChannels);
#endif


#ifdef __WIN_95__
Vstatus PCCANHwInitContext(PPCCANcontext pCtx,
                           WORD portBase,
                           PVchannel* chan,
                           unsigned int numberOfChannels);
void PCCANInterruptService(PPCCANcontext pCtx);
#endif


/*Vstatus PCCANVerify(PPCCANcontext pCtx);
Vstatus PCCANInit(PPCCANcontext pCtx);
Vstatus PCCANShutdown(PPCCANcontext pCtx);
Vstatus PCCANSetup( PPCCANcontext pHwContext );*/

Vstatus PCCANTransmitEvent(PVchannel pChan, PVevent pEvent, unsigned long tout);

#endif
