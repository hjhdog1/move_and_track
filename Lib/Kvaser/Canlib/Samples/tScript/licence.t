/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a licence and may be used and copied
** only in accordance with the terms of such licence.
**
** Description:
** Demonstrate how licences can be created and checked.
**
** Licences can be created locked to EAN and serial number, or to the
** CID identifier of an SD-card. Optionally, the licences can have a "timeout".
**
** By default, the code is set up for licence verification, causing the
** script to abort if no licence valid for the Eagle is found.
** The code can also be used to create and examine licences, by setting the
** CREATE_LICENCES constant to 1.

** The default password and licence file are set in a variables block below.
**
** When creating licences, the password, as well as EAN, serial number, CID
** and timeout can be set via environment variables.
** Key events as follows are used for creation/test:
** c - Create a new licence and add it to the licence file.
** d - Dump the contents of all licences in the licence file.
** v - Validate licence, i.e. check that the licence file contains an OK one.
** i - Information about the Eagle and SD card.
** z - Zero (revert to initial values) the EAN, serial number and CID.
** f - Free the licences, i.e. delete the licence file.
*/

variables {
  const int CREATE_LICENCES = 0;   // Set to 1 for licence creation

  const char default_pwd[]  = "My default pwd!";  // Must be 16 bytes!
  const char licence_file[] = "licence.dat";

  const int LICENCE_TYPE = 0;
  const int LICENCE_PAD1 = LICENCE_TYPE + 1;
  const int LICENCE_END  = LICENCE_PAD1 + 3;
  const int LICENCE_CID  = LICENCE_END  + 4;
  const int LICENCE_EAN  = LICENCE_END  + 4;  // Overlaps CID
  const int LICENCE_SER  = LICENCE_EAN  + 8;  // Overlaps CID
  const int LICENCE_PAD2 = LICENCE_SER  + 4;  // Overlaps CID
  const int LICENCE_PAD3 = LICENCE_CID  + SYS_SD_CID_SIZE;
  const int LICENCE_HASH = LICENCE_PAD3 + 4;
  const int LICENCE_SIZE = LICENCE_HASH + CRYPTO_SHA1_SIZE;
  // Verify licence format and size
  const int LICENCE_CHK1 = 1 / !(LICENCE_PAD3 - (LICENCE_PAD2 + 4));
  const int LICENCE_CHK2 = 1 / !(LICENCE_SIZE % CRYPTO_AES128_BLOCK);
}



variables {
  byte cid[SYS_SD_CID_SIZE];
  int ean_high;
  int ean_low;
  int serno;
}

void restore()
{
  if (sysGetValue(SYS_SD_CID, cid) < 0) {
    cid = 0;
  }
  sysGetValue(SYS_EAN_HIGH,  &ean_high);
  sysGetValue(SYS_EAN_LOW,   &ean_low);
  sysGetValue(SYS_SERIAL_NO, &serno);
}

on start {
  restore();
}


// Validate licence when the script is started
int licenceOK();
#if CREATE_LICENCES
#else
on start {
  if (!licenceOK()) {
    printf("Trying to stop myself!\n");
    scriptStop(0);   // qqq Stop myself!
    int a = 1 / 0;   // Throw an exception!
  }
}
#endif



#if CREATE_LICENCES
// *****
// Various stuff for the creation of licences (and for testing).
// Do not include this in code sent to customers!
// *****

variables {
  byte password[CRYPTO_AES128_BLOCK] = default_pwd;
  int type     = 0;
  int days     = 0;
}

envvar {
  char password_e[CRYPTO_AES128_BLOCK];
  int  type_e;
  int  days_e;
}

envvar {
  int  ean_high_e;
  int  ean_low_e;
  int  serno_e;
  char cid_e[SYS_SD_CID_SIZE];
}

on envvar type_e {
  envvarGetValue(type_e, &type);
}

on envvar days_e {
  envvarGetValue(days_e, &days);
}

on envvar ean_high_e {
  envvarGetValue(ean_high_e, &ean_high);
}

on envvar ean_low_e {
  envvarGetValue(ean_low_e, &ean_low);
}

on envvar serno_e {
  envvarGetValue(serno_e, &serno);
}

on envvar password_e {
  envvarGetValue(password_e, password);
}

on envvar cid_e {
  envvarGetValue(cid_e, cid);
}


int hex2bytes(byte data[]);
int bytes2int(const byte data[]);

// In-place conversion from byte values to ASCII-hex string.
void bytes2hex(byte data[], int length)
{
  for(int i = length - 1; i >= 0; i--) {
    char buf[3];
    sprintf(buf, "%02x", data[i]);
    data[i * 2, 2] = buf[0, 2];
  }
  data[length * 2] = '\0';
}

// Splits integer into four bytes.
void int2bytes(byte data[], int value)
{
  for(int i = 0; i < 4; i++) {
    data[i] = value >> (i * 8);
  }
}


void licenceDecipher(byte data[], const byte pwd[]);
int licenceIsAuthentic(const byte data[]);
int licenceFind(const char file[], byte licence[]);

void licenceSign(byte data[], const byte pwd[])
{
  cryptoHash(CRYPTO_SHA1, data + LICENCE_HASH, data[0, LICENCE_HASH]);
  cryptoEncipher(CRYPTO_AES128, data, data[0, LICENCE_SIZE], pwd);
}

void licenceCreate(byte data[])
{
  data = 0;
  data[0] = type;

  // If wanted, add time limit to licence.
  int licence_end = 0;
  const int day_seconds  = 24 * 60 * 60;
  if (days) {
    int current_time;
    timeGetDate(&current_time);
    int current_day = current_time / day_seconds;
    licence_end     = current_day + days + 1;
  }
  int2bytes(data + LICENCE_END, licence_end * day_seconds);

  switch (type) {
  case 0:
    data[LICENCE_CID, SYS_SD_CID_SIZE] = cid;
    break;
  case 1:
    int2bytes(data + LICENCE_EAN, ean_high);
    int2bytes(data + LICENCE_EAN + 4, ean_low);
    int2bytes(data + LICENCE_SER, serno);
    break;
  default:    // Anything else is an invalid licence
    data = 0xff;
    break;
  }
}

void licenceDump(const byte data[], char str[])
{
  char date[32] = "unlimited";
  int time = bytes2int(data + LICENCE_END);
  if (time) {
    strcpy(date, "until ");
    timeGetDate(time, date + strlen(date));
  }

  switch (data[0]) {
  case 0:
    char cid[SYS_SD_CID_SIZE * 2 + 1];
    cid = data[LICENCE_CID, SYS_SD_CID_SIZE];
    bytes2hex(cid, SYS_SD_CID_SIZE);
    sprintf(str, "CID = %s, %s", cid, date);
    break;
  case 1:
    sprintf(str, "EAN = %08x-%08x SER = %d,       %s",
            bytes2int(data + LICENCE_EAN),
            bytes2int(data + LICENCE_EAN + 4),
            bytes2int(data + LICENCE_SER),
            date);
    break;
  default:
    strcpy(str, "invalid");
    break;
  }
}

int add_licence(const char file[])
{
  byte data[LICENCE_SIZE * 2 + 1 + 1];   // Room for ASCII version!
  licenceCreate(data);
  licenceSign(data, password);
  bytes2hex(data, LICENCE_SIZE);
  strcat(data, "\n");

  FileHandle lic;
  if (fileOpen(lic, file, OPEN_APPEND) < 0) {
    return -1;
  }
  if (filePuts(lic, data) < 0) {
    fileClose(lic);
    return -2;
  }
  fileClose(lic);

  return 0;
}


on key 'c' {
  switch (add_licence(licence_file)) {
  case 0:
    printf("Successfully added to %s.\n", licence_file);
    break;
  case -1:
    printf("Unable to open %s.\n", licence_file);
    break;
  case -2:
    printf("Unable to write %s.\n", licence_file);
    break;
  default:
    printf("Unknown problem when adding licence.\n");
    break;
  }
}

on key 'd' {
  FileHandle lic;
  if (fileOpen(lic, licence_file, OPEN_READ) < 0) {
    printf("Unable to open %s.\n", licence_file);
    return;
  }

  byte data[128];
  int n = 1;
  while (fileGets(lic, data) > 0) {
    // Skip comments and empty lines.
    if ((data[0] == '#') || (data[0] == '\n')) {
      continue;
    }
    if (hex2bytes(data) != LICENCE_SIZE) {
      printf("Bad data in %s.\n", licence_file);
      break;
    }
    licenceDecipher(data, password);
    if (licenceIsAuthentic(data)) {
      char buf[80];
      licenceDump(data, buf);
      printf("Licence %d: %s\n", n, buf);
    } else {
      printf("Licence %d: invalid\n", n);
    }
    n++;
  }

  fileClose(lic);  
}

on key 'v' {
  byte licence[LICENCE_SIZE];
  int ret = licenceFind(licence_file, licence);
  switch (ret) {
  case 0:
    printf("No valid licence in %s.\n", licence_file);
    break;
  case -1:
    printf("Unable to open %s.\n", licence_file);
    break;
  case -2:
    printf("Bad data in %s.\n", licence_file);
    break;
  default:
    printf("Licence %d: OK\n", ret);
    break;
  }
}

on key 'i' {
  char buf[SYS_SD_CID_SIZE * 2 + 1];
  buf = cid;
  bytes2hex(buf, SYS_SD_CID_SIZE);
  printf("CID = %s\n", buf);
  printf("EAN = %08x-%08x\n", ean_high, ean_low);
  printf("SER = %d\n", serno);
  timeGetDate(buf);
  printf("Now = %s\n", buf);
}

on key 'z' {
  restore();
}

on key 'f' {
  fileDelete(licence_file);
}
#endif


// Is the byte a hexadecimal ASCII character?
int isHex(byte b)
{
  if (b >= 'a') {
    if (b > 'f') {
      return 0;
    }
  } else if (b >= 'A') {
    if (b > 'F') {
      return 0;
    }
  } else if (b < '0') {
    return 0;
  } else if (b > '9') {
    return 0;
  }

  return 1;
}

// In-place conversion from ASCII-hex string to byte values.
// Returns number of bytes converted.
int hex2bytes(byte data[])
{
  int i;
  for(i = 0; i < data.count; i++) {
    if (!isHex(data[i * 2]) || !isHex(data[i * 2 + 1])) {
      break;
    }
    data[i] = atoi(data[i * 2, 2], 16);
  }

  return i;
}

// Combines four bytes into an integer.
int bytes2int(const byte data[])
{
  int value = 0;
  for(int i = 0; i < 4; i++) {
    value |= data[i] << (i * 8);
  }

  return value;
}

// Compares two memory areas, like C/C++ memcmp().
int memcmp(const byte src1[], const byte src2[])
{
  // Throw exception if unequal lengths!
  if (src1.count != src2.count) {
    return 1 / 0;
  }

  for(int i = 0; i < src1.count; i++) {
    if (src1[i] != src2[i]) {
      return src1[i] - src2[i];
    }
  }

  return 0;
}


// Licence checking functions

void licenceDecipher(byte data[], const byte pwd[])
{
  cryptoDecipher(CRYPTO_AES128, data, data[0, LICENCE_SIZE], pwd);
}

int licenceIsAuthentic(const byte data[])
{
  byte digest[CRYPTO_SHA1_SIZE];
  cryptoHash(CRYPTO_SHA1, digest, data[0, LICENCE_HASH]);

  return memcmp(digest, data[LICENCE_HASH, CRYPTO_SHA1_SIZE]) == 0;
}

int licenceValid(const byte data[])
{
  if (!licenceIsAuthentic(data)) {
    return 0;
  }

  int valid;
  switch (data[0]) {
  case 0:
    valid = memcmp(cid, data[LICENCE_CID, SYS_SD_CID_SIZE]) == 0;
    break;
  case 1:
    valid = (bytes2int(data + LICENCE_EAN)     == ean_high) &&
            (bytes2int(data + LICENCE_EAN + 4) == ean_low) &&
            (bytes2int(data + LICENCE_SER)     == serno);
    break;
  default:
    valid = 0;
    break;
  }

  if (!valid) {
    return 0;
  }

  int licence_end = bytes2int(data + LICENCE_END);
  int time;
  timeGetDate(&time);

  return !licence_end || (time < licence_end);
}

int licenceFind(const char file[], byte licence[])
{
  FileHandle lic;
  if (fileOpen(lic, file, OPEN_READ) < 0) {
    return -1;
  }

  byte data[128];
  int n = 1;
  int ret = 0;
  while (fileGets(lic, data) > 0) {
    // Skip comments and empty lines.
    if ((data[0] == '#') || (data[0] == '\n')) {
      continue;
    }
    if (hex2bytes(data) != LICENCE_SIZE) {
      ret = -2;
      break;
    }
#if CREATE_LICENCES
    licenceDecipher(data, password);
#else
    licenceDecipher(data, default_pwd);
#endif
    if (licenceValid(data)) {
      licence = data[0, LICENCE_SIZE];
      ret = n;
      break;
    }
    n++;
  }

  fileClose(lic);

  return ret;
}

int licenceOK()
{
  byte licence[LICENCE_SIZE];
  return licenceFind(licence_file, licence) > 0;  
}

