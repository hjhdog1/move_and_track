/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Use environment variables to load and start a specified script,
** in a specified slot and using a specified default CAN channel.
** Another environment variable can be used to stop the script again.
** ---------------------------------------------------------------------------
*/

variables {
  const int FILENAME_LENGTH = 80;
}

envvar {
  int  script_channel;
  int  script_slot;
  char script_file[FILENAME_LENGTH];
  int  script_quit;
}

on envvar script_file {
  int channel, slot;
  char filename[FILENAME_LENGTH];
  envvarGetValue(script_channel, &channel);
  envvarGetValue(script_slot,    &slot);
  envvarGetValue(script_file,    filename);

  scriptLoad(channel, slot, filename);
  scriptStart(slot);
}

on envvar script_quit {
  int slot;
  envvarGetValue(script_quit, &slot);
  scriptStop(slot);
  scriptUnload(slot);
}
