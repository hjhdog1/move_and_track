/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _PCCXILINX_H_
#define _PCCXILINX_H_

void    PCCAN_ConnectIRQ(PPCCANcontext pCtx, int chan, unsigned connect);
Vstatus PCCAN_DetectCard(PPCCANcontext pCtx);
Vstatus PCCAN_InitCard(PPCCANcontext pCtx);
Vstatus PCCAN_KillCard(PPCCANcontext pCtx);
void    PCCAN_Interrupt(PPCCANcontext pCtx);
Vstatus PCCAN_InitChannel(PPCCANcontext pCtx, int chan);
Vstatus PCCAN_KillChannel(PPCCANcontext pCtx, int chan);
void PCCAN_EmergencyExit(PPCCANcontext pCtx, int chan);

#endif
