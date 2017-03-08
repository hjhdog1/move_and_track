/*
**                         Copyright 1998 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#include <stdinc.h>

#include "hermes_res.h"
#include "herm_hw.h"
#include "vcan_msg.h"

// One area for the 5920, one for the CAN circuit(s), one for the Xilinx.
#define HERMES_IOAREAS 3



void HERMESFreeResources(PHERMEScontext pCtx)
{
    CM_RESOURCE_LIST NullResourceList;
    BOOLEAN ResourceConflict;

#ifdef DEBUG
    DEBUG1("[HERMESFreeResources]\n");
#endif

    if (pCtx->InterruptObject) IoDisconnectInterrupt(pCtx->InterruptObject);
    pCtx->InterruptObject= NULL;

    //
    // Deallocate the resources.
    //
    RtlZeroMemory((PVOID)&NullResourceList, sizeof(NullResourceList));
    IoReportResourceUsage(NULL,
                          pCtx->DriverObject,
                          &NullResourceList,
                          sizeof(ULONG),
                          NULL,
                          NULL,
                          0,
                          FALSE,
                          &ResourceConflict);
}
