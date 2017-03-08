/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Demonstrates the usage of 'on CanMessage *'
**
** This code is intended to be inserted into any script that already have 
** some more specific 'on CanMessage' statements.
** ---------------------------------------------------------------------------
*/

// CAN messages not matched by anything more specific will end up here.

on CanMessage * {
  char text[80];
  sprintf(text, "Unknown message: %d", this.id);
  if (this.flags & canMSG_EXT) {
    strcat(text, "x");
  }
  if (this.flags & canMSG_RTR) {
    strcat(text, "r");
  }

  printf(text);
}
