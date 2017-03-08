/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
** Respond to a remote request with data from an environment variable.
** ---------------------------------------------------------------------------
*/

variables {
  CanMessage remote_reply;
}

envvar {
  char reply_data[8];
}

on envvar reply_data {
  envvarGetValue(reply_data, remote_reply.data);
}

on start {
  remote_reply.id    = 1000;
  remote_reply.flags = canMSG_EXT;
  remote_reply.dlc   = 4;
  remote_reply.data  = 0;
}

// Quick reply to remote request message from the default channel.
on CanMessage 1000xr {
  canWrite(this.channel, remote_reply);
}
