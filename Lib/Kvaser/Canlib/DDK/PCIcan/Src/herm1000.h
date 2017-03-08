/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _HERMES_SJA1000_H_
#define _HERMES_SJA1000_H_

#include "hermes.h"

extern BOOL HERMES_ProbeChannel(PHERMEScontext pCtx, IoAddress addr, int chan);
extern Vstatus HERMES_InitChannel(PHERMEScontext pCtx, int chan);
extern Vstatus HERMES_KillChannel(PHERMEScontext pCtx, int chan);
extern int HERMES_ChannelInterrupt(PHERMEScontext pCtx, int chan);


#endif
