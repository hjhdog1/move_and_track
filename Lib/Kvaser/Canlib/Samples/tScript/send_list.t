/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**
** Send according to a transmit list file, given by an environment variable.
** Another environment variable is used to halt the transmission.
** The file data will be sorted on transmission time before sending begins.
**
** The file contains lines of the following form:
**   transmission time, counting from the start of transmission
**     in decimal milliseconds.
**   :
**   [space]
**   message ID, with possibility for extended(x) and remote request(r)
**     in decimal
**        binary(%)
**        hexadecimal($)
**   [space]
**   message data
**     in decimal, a 32 bit integer 
**        hexadecimal($), 1-8 bytes
**        string(""), 1-8 bytes
**  or (if remote request)
**   message dlc in decimal
**
** Example:
**  010: 12 "text"
**  012: x%10010100011101100 232124
**  100: $a1c $c045bd24
**  250: rx345 5
** ---------------------------------------------------------------------------
*/


variables {
  const int DATA_COUNT       = 100;
  const int LINE_LENGTH      = 80;
  const int SORT_KEY_LENGTH  = 4;
  const int FILE_NAME_LENGTH = 80;

  // Create a two dimensional array of characters.
  // Note that there is (currently) no way of doing this in t,
  // except as an array of a struct.
  // Also note that structs can't (currently) contain structs.
  typedef struct {
    char line[LINE_LENGTH];
  } Line;
  Line data[DATA_COUNT];

  int txtime[DATA_COUNT];
  CanMessage txmsg[DATA_COUNT];

  Timer transmitter;
}

envvar {
  char transmit_list[FILE_NAME_LENGTH];
  int transmit_list_halt;
}

on start {
  canBusOff(0);
  canSetBitrate(0, 1000000);
  canSetBusOutputControl(0, canDRIVER_NORMAL);
  canBusOn(0);
}

on envvar transmit_list_halt {
  timerCancel(transmitter);
}

on Timer transmitter {
  // Send all messages that are scheduled for this time.
  int time;
  do {
    time = txtime[this.id];
    if (canWrite(txmsg[this.id]) < 0) {   // If transmission fails (queue full),
      txtime[this.id] += 1;               //   try again in a short time.
      break;
    }
  } while (txtime[++this.id] <= time);

  // If there are more messages to send,
  // set up the timer event for the next one.
  if (txtime[this.id] != 0x7fffffff) {
    transmitter.timeout = txtime[this.id] - time;
    timerStart(transmitter);
  } else {
    char text[80] = "Transmission finished: ";
    char time[80];
    timeGetDate(time);
    strcat(text, time);
    printf("%s",text);
  }
}

void to_msgdata(const char text[], CanMessage msg);
int read(const char filename[], Line data[]);
void sort(Line data[], int significant);

on envvar transmit_list {
  char filename[FILE_NAME_LENGTH];
  envvarGetValue(transmit_list, filename);

  int count = read(filename, data);

  // Return if the file was empty
  if (count <= 0) {
    return;
  }

  // Only sort by the initial four characters (the transmission time).
  sort(data[0, count], SORT_KEY_LENGTH);

  // Convert into data suitable for the timer handler.
  int i;
  for(i = 0; i < count; i++) {
    txtime[i] = atoi(data[i].line);
    to_msgdata(data[i].line + 5, txmsg[i]);
  }
  txtime[i] = 0x7fffffff;     // Mark end

  // For debug purposes, dump the read data to another file.
// /*  
  FileHandle out;
  if (fileOpen(out, "dump.txt") >= 0) {
    for(int n = 0; n < i; n++) {
      char line[LINE_LENGTH], buf[LINE_LENGTH];
      sprintf(line, "%04d: ", txtime[n]);
      if (txmsg[n].flags & canMSG_RTR) {
        strcat(line, "r");
      }
      if (txmsg[n].flags & canMSG_EXT) {
        strcat(line, "x");
      }
      itoa(txmsg[n].id, buf, 2);
      int pos = strlen(line);
      pos += sprintf(line + pos, "%%%s $", buf);
      for(int i = 0; i < txmsg[n].dlc; i++) {
        sprintf(line + pos + i * 2, "%02x ", txmsg[n].data[i]);
      }
      filePuts(out, line);
      filePuts(out, "\n");
    }
    fileClose(out);
  }
// */

  if (count) {
    transmitter.id      = 0;
    transmitter.timeout = txtime[0];
    timerStart(transmitter);
  }
}

// Print conents of can message
void dumpCanMessage (CanMessage msg)
{
  int i;
  int pos;
  char temp[LINE_LENGTH];

  pos = sprintf(temp, "CAN id: %d", msg.id);
  pos += sprintf(temp + pos, ", flags: 02%04x", msg.id);
  pos += sprintf(temp + pos, ", dlc: %d", msg.dlc);

  pos += sprintf(temp + pos, ", data: 0x");
  for(int i = 0; i < msg.dlc; i++) {
    pos += sprintf(temp + pos, "%02x ",msg.data[i]);
  }
  printf("%s\n",temp);

}

// Extracts CanMessage data from the given ASCII text.
void to_msgdata (const char text[], CanMessage msg)
{
  msg.flags = 0;

  int pos  = 0;
  int base = 10;
  int done = 0;
  do {
    switch (text[++pos]) {
    case 'x':
      msg.flags |= canMSG_EXT;
      break;
    case 'r':
      msg.flags |= canMSG_RTR;
      break;
    case '$':
      base = 16;
      break;
    case '%':
      base = 2;
      break;
    default:
      done = 1;
      break;
    }
  } while (!done);

  msg.id = atoi(text + pos, base);

  while (text[pos++] != ' ') {
    // Skip to next space
  }

  switch (text[pos++]) {
  case '"':
    msg.dlc  = strlen(text + pos) - 1;
    msg.data = text[pos, msg.dlc];
    break;
  case '$':
    msg.dlc  = strlen(text + pos) / 2;
    for(int i = 0; i < msg.dlc; i++) {
      msg.data[i] = atoi(text[pos + i * 2, 2], 16);
    }
    break;
  default:
    int val = atoi(text + (pos - 1));
    if (msg.flags & canMSG_RTR) {
      msg.dlc = val;
    } else {
      msg.dlc = 4;
      for(int i = 0; i < 4; i++) {
        msg.data[i] = val;
        val       >>= 8;
      }
    }
  }
  // For debug purposes, print the CAN message.
  // dumpCanMessage(msg);
}

// Sort array of strings, by the beginning signficant characters of each string.
// Uses the Shell sort algorithm (from Wikipedia pseudocode).
void sort (Line data[], int significant)
{
  int inc = round(data.count / 2.0);
  while (inc > 0) {
    for(int i = inc; i < data.count; i++) {
      char temp[LINE_LENGTH];
      strcpy(temp, data[i].line);
      // temp = data[i].line;  // Works too, but LINE_LENGTH bytes will be copied.
      int j = i;
      while (j >= inc) {
        if (strcmp(data[j - inc].line, temp, significant) <= 0) {
          break;
        }
        strcpy(data[j].line, data[j - inc].line);
        j = j - inc;
      }
      strcpy(data[j].line, temp);
    }
    inc = round(inc / 2.2);
  }
}

// Reads text lines from a file into a two dimensional character array.
int read (const char filename[], Line data[])
{
  FileHandle in;
  int result = fileOpen(in, filename, OPEN_READ);
  if (result < 0) {
    printf("Failed to open \"%s\". Error code %d\n", filename, result);
    return -1;
  }

  int i = 0;
  int length;
  do {
    length = fileGets(in, data[i].line);
    // There should really be different error message for different problems.
    if (length < 0) {
      printf("Failed to read from \"%s\". Error code: %d\n", filename, length);
      return -1;
    }

    // End of file?
    if (length == 0) {
      break;
    }

    // Make sure the line really fit in our line array.
    if ((length == data[i].line.count - 1) &&
        (data[i].line[length - 1] != '\n')) {
      printf("Line too long in \"%s\". Length: %d\n", filename, length);
      return -1;
    }

    data[i].line[length - 1] = '\0';   // Remove final '\n'.
  } while (++i < data.count);

  fileClose(in);
  
  return i;
}
