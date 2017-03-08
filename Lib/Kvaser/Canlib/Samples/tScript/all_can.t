/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Demonstrates the usage of 'on CanMessage [*]'
**
** This code is intended to be inserted into any script that already have 
** some 'on CanMessage' statements.
** ---------------------------------------------------------------------------
*/

variables {
  int min_delta_time = 0x7fffffff;
}

envvar {
  int delta_time;
  int id1;
  int id2;
}

// Deals with all received CAN messages.
// Look at all received CAN messages and keep track of the two
// consecutive ones that arrive closest together in time.
//
// Note: The 32 bit integers used mean that if the time between two
//       messages is larger than ~71 minutes, this simple code will
//       not work as intended (canGetTimestamp(this, 0) can be
//       used to get the high part of the timestamps).
// For now, assume that messages are never further appart than ~35 minutes,
// so that delta_time is guaranteed to be positive!

on CanMessage<*> [*] {
  // Get current time in us (up to 2^32 - 1).
  // Replace 1 by 1000 to get the time in ms instead.
  int current_time = canGetTimestamp(this, 1);

  printf("%d ", current_time);

  static int last_time;
  static int last_id = -1;
  if (last_id >= 0) {
    // 2's complement arithmetic means that we don't have to worry about
    // which of the two numbers is larger. The result will be correct, anyway.
    int dt = current_time - last_time;
    if (dt < min_delta_time) {
      min_delta_time = dt;
      envvarSetValue(delta_time, min_delta_time);
      envvarSetValue(id1, last_id);
      envvarSetValue(id2, this.id);
    }
  }
  last_id   = this.id;
  last_time = current_time;

}
