/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Set up CAN buses for use, and allow for bitrate
** changes via environment variable.
** ---------------------------------------------------------------------------
*/

envvar {
  int bitrate;
}

on start {
  canSetBitrate(0, canBITRATE_1M);
  canSetBitrate(1, canBITRATE_1M);
  canSetBusOutputControl(0, canDRIVER_NORMAL);
  canSetBusOutputControl(1, canDRIVER_NORMAL);
  canBusOn(0);
  canBusOn(1);
}

on stop {
  canBusOff(0);
  canBusOff(1);
}

// Allow for bitrate change by external software.
on envvar bitrate {
  canBusOff(0);
  canBusOff(1);
  int new_bitrate;
  envvarGetValue(bitrate, &new_bitrate);
  canSetBitrate(0, new_bitrate);
  canSetBitrate(1, new_bitrate);
  canBusOn(0);
  canBusOn(1);
}
