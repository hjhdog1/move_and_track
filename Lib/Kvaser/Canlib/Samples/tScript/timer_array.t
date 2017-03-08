/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Use array of timers to delay the message response.
** ---------------------------------------------------------------------------
*/

variables {
  const int DELAY_TIME = 10;   // Delay in ms
  typedef struct {
    int dlc;
    byte data[8];
  } DelayedData;
  const int MAX_DELAYS = 16;
  Timer delay_timer[MAX_DELAYS];
  DelayedData delayed_data[MAX_DELAYS];
  int next_timer = 0;
}

on Timer delay_handler {
  CanMessage msg;
  int index = this.id;
  msg.id    = 42;

  msg.flags = 0;
  msg.dlc   = delayed_data[index].dlc;
  msg.data  = delayed_data[index].data;
  canWrite(0, msg);
}

on start {
  // Initialize a bunch of delay timers.
  for(int i = 0; i < delay_timer.count; i++) {
    delay_timer[i].timeout = 10;
    delay_timer[i].id      = i;
    timerSetHandler(delay_timer[i], "delay_handler");
  }
}

// Send a message on after a delay (from channel 1 to 0).
// Assume that we never exceed MAX_DELAYS simultaneous delays.
on CanMessage<1> 42 {
  delayed_data[next_timer].dlc  = this.dlc;
  delayed_data[next_timer].data = this.data;

  timerStart(delay_timer[next_timer]);

  next_timer++;
  if (next_timer == delay_timer.count) {
    next_timer = 0;
  }
}
