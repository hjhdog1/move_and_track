/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Send data from an environment variable once per second.
** ---------------------------------------------------------------------------
*/


variables {
  Timer periodic;
}

envvar {
  char periodic_data[8];
}

on Timer periodic {
  CanMessage msg;
  msg.id    = 123;
  msg.flags = 0;
  msg.dlc   = 8;
  envvarGetValue(periodic_data, msg.data);
  canWrite(msg);
}

on start {
  periodic.timeout = 1000;  // Once per second
  timerStart(periodic, FOREVER);
}