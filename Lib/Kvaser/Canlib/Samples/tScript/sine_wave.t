/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:

** Using a database defined signal, send out a sine wave data at
** at irregular time intervals (but using real time to define the wave).
** At random intervals the frequency will switch between four values.
**
** The transmission begins when the message 4567x is received, with the
** number 12 in its first data byte.
** The offset of the sine wave is also controlled by 4567x, by adding
** the quote of the second (interpreted as signed data) and third data byte,
** up to a maximum of 10.
** ---------------------------------------------------------------------------
*/

float wave (float frequency, float amplitude, float offset)
{
  float time = (float)timeGetLocal(1000) / 1000;
  time = time - floor(time);
  float val = amplitude * sin(2 * M_PI * time * frequency) + offset;
  return val;
}

variables {
  const float AMPLITUDE = 2;
  float offset = AMPLITUDE + 0.6;
  Timer intermittent;
}

on start {
  canBusOff(0);
  intermittent.timeout = 1;
  canSetBitrate(0, 1000000);
  canSetBusOutputControl(0, canDRIVER_NORMAL);
  canBusOn(0);
}

on Timer intermittent {
  static float frequency[4] = {1.0 / 7, 2.2, 0.5, 2e-3};
  static int i = 0;
  CanMessage_Wave msg;  // Cast below is not required, but quitens compiler.
  msg.wave.Phys = wave(frequency[i], (float) AMPLITUDE, offset);
  canWrite(msg);

  if (random(100) == 0) {
    i++;
    if (i == frequency.count) {
      i = 0;
    }
  }
  intermittent.timeout = random(99) + 1;
  timerStart(intermittent);
}

on CanMessage 4567x {
  if ((this.data[0] == 12) && !timerIsPending(intermittent)) {
    intermittent.timeout = 1;
    timerStart(intermittent);
  }
  // The message data is unsigned (byte), but interpret as signed here.
  char data1 = this.data[1];
  offset += (float)data1 / this.data[2];
  if (offset > 10) {
    offset = 10;
  }
}
