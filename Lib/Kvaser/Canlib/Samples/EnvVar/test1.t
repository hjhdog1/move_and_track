/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**  Simple script that demonstrates evironment variables.
** ---------------------------------------------------------------------------
*/

envvar {
  int evInt;
  int evUpdateCount;

  char evBigOne[64];
  int evChecksum;
}

//
// When the host program updates evInt, increment evUpdateCount.
//
on envvar evInt {
  int tmp;
  envvarGetValue(evUpdateCount, &tmp);
  envvarSetValue(evUpdateCount, tmp + 1);
}

//
// When the host program updates evBigOne, calculate the sum of all
// bytes in it and write the sum into evChecksum.
//
on envvar evBigOne {
  int i, tmp;
  char b[64];

  envvarGetValue(evBigOne, b, 64);
 
  tmp = 0;
  for (i=0; i<64; i++) {
    tmp += b[i];
  }
  envvarSetValue(evChecksum, tmp);
}