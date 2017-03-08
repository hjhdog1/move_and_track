/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _PCC82200_H_
#define _PCC82200_H_

extern BOOL PCC200_ProbeChannel(PPCCANcontext pCtx, IoAddress addr, int chan);
extern Vstatus PCC200_InitChannel(PPCCANcontext pCtx, int chan);
extern Vstatus PCC200_KillChannel(PPCCANcontext pCtx, int chan);
extern void PCC200_Interrupt(PPCCANcontext pCtx, int chan);


#endif
