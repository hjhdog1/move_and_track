/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Demonstrate calculations using signal values.
**
** When an Engine message with correct sequence number
** arrives on channel 0/1, reports its values using environment
** variables and passes a "recalculated" version on to channel 1/0.
** Reports messages with bad sequence numbers.
** ---------------------------------------------------------------------------
*/

envvar {
  int   rpm;
  int   temp;
  float xfactor;
}

on CanMessage<*> Engine {
  static int seqno = 0;

  if (this.seqno.Phys == seqno) {
    seqno++;
    envvarSetValue(rpm,     this.rpm.Phys);
    envvarSetValue(temp,    this.temp.Phys);
    envvarSetValue(xfactor, this.xfactor.Phys);

    this.rpm.Phys   <<= 2;
    this.temp.Phys   *= 1.2;
    const float k     = (3 * M_E) / 1.00351e-4;
    this.xfactor.Phys = k / (this.xfactor.Phys + 0.45);
    canWrite(!this.channel, this);
  } else {
    printf("Engine - bad sequence number (%d/%d)\n", this.seqno.Phys, seqno);
    seqno = this.seqno.Phys + 1;
  }
}
