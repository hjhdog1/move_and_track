/*
**                   Copyright 2008 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**
** ---------------------------------------------------------------------------
*/
#include <stdio.h>
#include <windows.h>
#include <canlib.h>

int main(int argc, char **argv)
{
  int i, h;
  int stat;
  unsigned char buf[8];

  if (argc != 2) {
    printf("Usage: read_customer_data channel\n");
    exit(1);
  }
  
  canInitializeLibrary();
  h = canOpenChannel(atoi(argv[1]), 0);
  if (h < 0) {
    printf("Cannot open channel %s, error code %d\n", argv[1], h);
    exit(1);
  }

  stat = kvReadDeviceCustomerData(h,
                                  100 /*userNumber*/,
                                  0, /* reserved, MBZ */
                                  buf, sizeof(buf));

  if (stat != canOK) {
    printf("kvReadDeviceCustomerData returned error code %d\n", stat);
    exit(1);
  }

  printf("User data: ");
  for (i=0; i<(signed)sizeof(buf); i++) {
    printf("%02x ", buf[i]);
  }
  printf("\n");
  
  canClose(h);
  return 0;
}
