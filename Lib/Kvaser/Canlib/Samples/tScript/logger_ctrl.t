/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Interact with logging from the script.
** This assumes the logger is armed and ready (not using "Log All")
** See t user guide for more information about loggerStart and loggerStop.
** ---------------------------------------------------------------------------
*/

variables {
  int trig_value = -1;
}

// Complex logger start and stop logic.
on CanMessage 0x4242x {
  if ((trig_value < 0) && (loggerStatus() == LOGGER_STATE_IDLE)) {
    trig_value = this.data[2];
    loggerStart();
  } else if (this.data[2] > trig_value + 10) {
    trig_value = -1;
    loggerStop();
  }
}

// Remove some 0x4243x messages before the rest of the
// trigger machinery has a chance to look at them.
on prefilter CanMessage 0x4243x {
  if ((trig_value > 0) && (trig_value & 1)) {
    filterDropMessage();
  }
}
