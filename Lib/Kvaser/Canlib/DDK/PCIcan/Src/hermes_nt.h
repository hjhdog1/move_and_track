/*
**                         Copyright 1999 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
**
** Description:
**
**
*/
#ifndef _HERMES_NT_H_
#define _HERMES_NT_H_

extern NTSTATUS CreateChannelsHermes(IN PDRIVER_OBJECT  DriverObject,
                              IN PUNICODE_STRING RegistryPath);

extern void HERMESDisconnectInterrupt(void *p);


#endif
