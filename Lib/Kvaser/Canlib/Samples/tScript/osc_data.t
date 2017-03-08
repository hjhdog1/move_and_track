/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**
** Read and write binary files, in this case containing "oscilloscope" data.
** Uses a "trick" to convert from/to int/float to/from bytes:
**   A special message in a data base is set up to have 32 bit integer data
**   as well as 32 bit floating point data, in both big and little endian
**   format, in the first four data bytes.
**   Conversion is then simply a case of writing to the correct Raw element
**   and fetching the first four data bytes, and vice versa.
**
** Fetches old data from disk (oscdata.dat) at script start.
** Stores data at script stop (appends if not cleared).
** Appends incoming OscData signals to array.
** On remote request 701, immediately sends out the next stored data item.
** On message 702, sends out the requested (data[0..1]) data item.
** On message 703, clears array and starts over from the beginning.
**
** OscData is defined to 700 in the examples.dbc database
** Even when no data is recorded, it will still send item 0.
**
*/

variables {
  const int LITTLE_ENDIAN = 0;
  const int BIG_ENDIAN    = 1;
  const int INTEL         = LITTLE_ENDIAN;
  const int MOTOROLA      = BIG_ENDIAN;
}

// Converts from four data bytes to a float value.
// Big or little endian byte order.
void from_bytes (float &val, const byte data[], int order)
{
  CanMessage_Convert msg;
  msg.data[0, 4] = data[0, 4];
  if (order == INTEL) {
    val = msg.real_intel.Raw;
  } else {
    val = msg.real_motorola.Raw;
  }
}

// Converts from four data bytes to a float value.
// Little endian byte order.
void from_bytes (float &val, const byte data[])
{
  from_bytes(&val, data, INTEL);
}

// Converts from four data bytes to an int value.
// Big or little endian byte order.
void from_bytes (int &val, const byte data[], int order)
{
  CanMessage_Convert msg;
  msg.data[0, 4] = data[0, 4];
  if (order == INTEL) {
    val = msg.int_intel.Raw;
  } else {
    val = msg.int_motorola.Raw;
  }
}

// Converts from four data bytes to an int value.
// Little endian byte order.
void from_bytes (int &val, const byte data[])
{
  from_bytes(&val, data, INTEL);
}

// Converts from a float value to four data bytes.
// Big or little endian byte order.
void to_bytes (float val, byte data[], int order)
{
  CanMessage_Convert msg;
  if (order == INTEL) {
    msg.real_intel.Raw = val;
  } else {
    msg.real_motorola.Raw = val;
  }
  data[0, 4] = msg.data[0, 4];
}

// Converts from a float value to four data bytes.
// Little endian byte order.
void to_bytes (float val, byte data[])
{
  to_bytes(val, data, INTEL);
}

// Converts from an int value to four data bytes.
// Big or little endian byte order.
void to_bytes (int val, byte data[], int order)
{
  CanMessage_Convert msg;
  if (order == INTEL) {
    msg.int_intel.Raw = val;
  } else {
    msg.int_motorola.Raw = val;
  }
  data[0, 4] = msg.data[0, 4];
}

// Converts from an int value to four data bytes.
// Little endian byte order.
void to_bytes (int val, byte data[])
{
  to_bytes(val, data, INTEL);
}


variables {
  const int OSC_DATA_ITEMS  = 1024;
  const char osc_filename[] = "oscdata.dat";

  typedef struct {
    int   time;
    float voltage;
  } OscData;
  OscData osc_data[OSC_DATA_ITEMS];

  CanMessage osc_reply;
  int osc_reply_idx = 0;

  int old_used = 0;
  int used     = 0;
  int cleared  = 0;
}

// "Deserializes" OscData from a file and puts it in an array.
// "Deserialization" is done manually (not yet supported by t).
int read_osc_data (FileHandle in, OscData data[])
{
  int i;
  for(i = 0; i < data.count; i++) {
    byte buf[8];
    int length = fileReadBlock(in, buf);
    if (length == 0) {
      break;
    }
    int   time;     // References to struct elements
    float voltage;  // of base types are not allowed!
    from_bytes(&time,    buf[0, 4]);
    from_bytes(&voltage, buf[4, 4]);
    data[i].time    = time;
    data[i].voltage = voltage;
  }
  return i;
}

// Writes "serialized" OscData from an array to a file.
// "Serialization" is done manually (not yet supported by t).
void write_osc_data (FileHandle out, OscData data[])
{
  int i;
  for(i = 0; i < data.count; i++) {
    byte buf[8];
    to_bytes(data[i].time,    buf[0, 4]);
    to_bytes(data[i].voltage, buf[4, 4]);
    fileWriteBlock(out, buf);
  }
}

on start {
  FileHandle in;
  if (fileOpen(in, osc_filename) < 0) {
    old_used = 0;
    used     = 0;
    return;
  }

  old_used = read_osc_data(in, osc_data);
  used     = old_used;

  // Set up for first 701r request.
  osc_reply.id    = 701;
  osc_reply.flags = 0;
  osc_reply.dlc   = 8;
  if (old_used) {
    to_bytes(osc_data[0].time,    osc_reply.data[0, 4]);
    to_bytes(osc_data[0].voltage, osc_reply.data[0, 4]);
  } else {
    osc_reply.data = 0;   // Set to all zeroes
  }

  fileClose(in);
}

on stop {
  if (!used) {
    return;
  }

  FileHandle out;
  if (cleared) {
    if (fileOpen(out, osc_filename, OPEN_TRUNCATE) < 0) {
      return;
    }
    old_used = 0;
  } else {
    if (fileOpen(out, osc_filename, OPEN_APPEND) < 0) {
      return;
    }
  }

  write_osc_data(out, osc_data[old_used .. used - 1]);

  fileClose(out);
}

on CanMessage OscData {
  if (used < osc_data.count) {
    osc_data[used].time    = this.time.Phys;
    osc_data[used].voltage = this.voltage.Phys;
    used++;
  }
}

on CanMessage 701r {
  // Reply with a previously setup message, to be as quick as possible.
  canWrite(osc_reply);

  // Prepare for the next remote request.
  osc_reply_idx++;
  if (osc_reply_idx >= used) {
    osc_reply_idx = 0;
  }
  to_bytes(osc_data[osc_reply_idx].time,    osc_reply.data[0, 4]);
  to_bytes(osc_data[osc_reply_idx].voltage, osc_reply.data[4, 4]);
}

on CanMessage 702 {
  CanMessage msg;

  msg.id    = 710;
  msg.dlc   = 8;
  msg.flags = 0;
  int msg_reply_idx = (this.data[1] << 8) | this.data[0];
  to_bytes(osc_data[msg_reply_idx].time,    msg.data[0, 4]);
  to_bytes(osc_data[msg_reply_idx].voltage, msg.data[4, 4]);
  canWrite(msg);
}

on CanMessage 703 {
  cleared = 1;
  used    = 0;
}

// If someone sends too big data in the on CanMessage 702 above, it can
// generate an array index exception. Take care of this here in order
// to continue executing the script.

on exception {
  printf("OOOooops...\n");
  printf("Err: %d, Thread: %d, Pc: %d, cycles: %d\n",
         this.error, this.current_thread, this.pc, this.cycle);
}
