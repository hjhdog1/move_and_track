/*
**                   Copyright 2012 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a licence and may be used and copied
** only in accordance with the terms of such licence.
**
** Description:
** Demonstrate how user parameters can be read.
** Write data to i.e. eagle with hydra_setparam_userdata.exe
**
*/

on start {
  printf("Starting user_param\n");
}

on key 'c' {
  byte user_param[USER_PARAM_MAX_SIZE];
  int user_int;
  int user_param_size;
  int userId   = 100;
  int paramId  = 0; // Must be zero (presently not implemented) 
  int stat;
  
  printf("Reading user_param \n");
  
  // Read user parameter array size
  stat = customerdataGetLength(userId, paramId, &user_param_size); 
  printf("\nstat = %d Size = %d userId = %d\n",
         stat, user_param_size, userId);

  // Read user parameter array 
  stat = customerdataGetValue(userId, paramId, user_param);
  printf("stat = %d \n", stat);
  for (int i = 0; i < user_param_size; i++) {
    printf("%02x ", user_param[i]);
  }
}

