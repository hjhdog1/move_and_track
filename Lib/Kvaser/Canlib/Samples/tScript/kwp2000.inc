// KWP2000 constants and utility functions
//
// Not yet capable of dealing with multiple
// simultaneous connections.
variables {
  // KWP2000 constants

  const int SERVICE_DIAGNOSTIC_SESSION        = 0x10;  // Supports functional addr
  const int  SESSION_STANDARD                 = 0x81;
  const int  SESSION_PROGRAMMING              = 0x85;
  const int  SESSION_DEVELOPMENT              = 0x86;
  const int  SESSION_ADJUSTMENT               = 0x87;
  const int  SESSION_FLASH_PROGRAMMING        = 0x90;

  const int SERVICE_ECU_RESET                 = 0x11;
  const int  RESET_MODE_POWERON               = 0x01;
  const int  RESET_MODE_KEYON                 = 0x03;

  const int SERVICE_CLEAR_DIAGNOSTIC_INFO     = 0x14;

  const int SERVICE_READ_STATUS_OF_DTC        = 0x17;
  // groupOfDTC = 0xFFFF means all

  const int SERVICE_READ_DTC_BY_STATUS        = 0x18;
  const int  READ_DTC_STORED                  = 0x02;
  const int  READ_DTC_ALL                     = 0x03;
  const int  READ_DTC_PENDING                 = 0x11;
  const int  READ_DTC_STATUS_BITS             = 0xFF;
  // groupOfDTC = 0xFFFF means all
  // qqq Bits for statusOfDTC?
  // qqq Needed for DTC status reporting.

  const int SERVICE_READ_ECU_IDENTIFICATION   = 0x1A;
  const int  ECU_INFO_DATA_TABLE              = 0x80;
  const int  ECU_INFO_SCALING_TABLE           = 0x81;
  // Specific indentification option 0x87-0xbf.
  // 87-8A, 90-91, 97-99, A0, A5, B0, B1 defined.

  const int SERVICE_READ_DATA_BY_LOCAL_ID     = 0x21;

  const int SERVICE_READ_DATA_BY_COMMON_ID    = 0x22;

  const int SERVICE_DISABLE_NRML_MSG_TRANSM   = 0x28;  // Supports functional addr
  const int  RESPONSE_REQUIRED_YES            = 0x01;
  const int  RESPONSE_REQUIRED_NO             = 0x02;
  // No RESPONSE_REQUIRED_x byte is equivalent to _YES.

  const int SERVICE_ENABLE_NRML_MSG_TRANSM    = 0x29;  // Supports functional addr
  // Response required yes/no as above.

  const int SERVICE_WRITE_DATA_BY_COMMON_ID   = 0x2E;
  const int  COMMON_ID_VIN                    = 0x90;
  const int  COMMON_ID_SERIAL                 = 0x98;
  const int  COMMON_ID_DATE                   = 0x99;
  // There are more available ID:s.
  // See above under SERVICE_READ_ECU_IDENTIFICATION.

  const int SERVICE_WRITE_DATA_BY_LOCAL_ID    = 0x3B;
  const int  LOCAL_ID_SPEEDLIMIT              = 0x03;
  const int  LOCAL_ID_SPEEDLIMIT2             = 0x04;
  const int  LOCAL_CLEAR_SYSTEM_STATISTICS    = 0x70;
  const int  LOCAL_CLEAR_APPL_STATISTICS      = 0x71;

  const int SERVICE_TESTER_PRESENT            = 0x3E;  // Supports functional addr
  // Response required yes/no as above.

  const int SERVICE_CONTROL_DTC_SETTING       = 0x85;
  const int  DTC_SETTING_MODE_ON              = 0x01;
  const int  DTC_SETTING_MODE_OFF             = 0x02;

  const int SERVICE_FLASH_PROGRAMMING         = 0xB1;
  // Further information in other documents.


  // Scaling information

  const int SCALING_TYPE_MASK                 = 0xF0;
  const int SCALING_UNSIGNED_NUMERIC          = 0x00;
  const int SCALING_SIGNED_NUMERIC            = 0x10;
  const int SCALING_ASCII                     = 0x60;
  const int SCALING_ASCII_NUMERIC             = 0x80;
  const int SCALING_FORMULA                   = 0x90;
  const int SCALING_UNIT_FORMAT               = 0xA0;
  const int SCALING_UNSIGNED_NUM_W_INDICATION = 0xB0;
  const int SCALING_STATE                     = 0xD0;

  const int SCALING_DATA_END                  = 0xFF;


  // kwp2000 response handling
  const int SERVICE_RESPONSE_BIT              = 0x40;
  const int SERVICE_NEGATIVE_RESPONSE         = 0x7F;

  const int RESPONSE_MAX_LENGTH_TO_CHECK      = 3;
  const int RESPONSE_NONE_EXPECTED            = RESPONSE_MAX_LENGTH_TO_CHECK + 1;

  // Interpreted responses
  const int RESPONSE_POSITIVE                 =  0;
  const int RESPONSE_NEGATIVE                 = -1;
  const int RESPONSE_BAD                      = -2;
  const int RESPONSE_TIMEOUT                  = -3;


  // KWP2000 commands with no data
  // The first byte contains the number of bytes that are expected
  // to be returned equal (except |0x40 on the first one) for a
  // positive response.
  byte SetStandardSession[3]               = {2, SERVICE_DIAGNOSTIC_SESSION,
                                                 SESSION_STANDARD};

  byte SetAdjustmentSession[3]             = {2, SERVICE_DIAGNOSTIC_SESSION,
                                                 SESSION_ADJUSTMENT};

  byte ReadECUIdentification[3]            = {2, SERVICE_READ_ECU_IDENTIFICATION,
                                                 ECU_INFO_DATA_TABLE};

  byte ResetECU[3]                         = {2, SERVICE_ECU_RESET,
                                                 RESET_MODE_POWERON};

  byte ReadDTCs[5]                         = {1, SERVICE_READ_DTC_BY_STATUS,
                                                 READ_DTC_STORED,
                                                 0xFF, 0xFF};  // All DTCs

  byte ClearDTCs[4]                        = {1, SERVICE_CLEAR_DIAGNOSTIC_INFO,
                                                 0xFF, 0xFF};  // All DTCs

  byte ReadLocalIdentifierScalingTable[4]  = {3, SERVICE_READ_DATA_BY_LOCAL_ID,
                                                 0x00, 0x01};

  byte ReadECUIdentificationScalingTable[3] ={2, SERVICE_READ_ECU_IDENTIFICATION,
                                                 ECU_INFO_SCALING_TABLE};

  byte ReadCommonIdentifierScalingTable[4] = {3, SERVICE_READ_DATA_BY_COMMON_ID,
                                                 0x00, 0x01};

  // KWP2000 commands with data
  // These must be followed by a byte identifier and the actual data bytes.
  byte WriteDataByCommonId[3]              = {3, SERVICE_WRITE_DATA_BY_COMMON_ID,
                                                 0x00};

  byte WriteDataByLocalId[2]               = {2, SERVICE_WRITE_DATA_BY_LOCAL_ID};

  // ISO-15765
  CanTpHandle tp_handle;
  Timer timerNoResponse;

  // KWP2000 error checking
  byte expPosCount = RESPONSE_NONE_EXPECTED;
  byte expPosData[RESPONSE_MAX_LENGTH_TO_CHECK];
}


// This must be implemented by the code using this library!
void kwp2000CallbackHandleResponse(int response, const byte data[]);


void kwp2000ShutDown (void)
{
  canTpClose(tp_handle);

  expPosCount = RESPONSE_NONE_EXPECTED;
}


int kwp2000Init (int rx_id, int tx_id)
{
  if (canTpOpen(tp_handle, rx_id, tx_id, "ISO-15765",
                iso15765_Fixed | iso15765_Physical) < 0) {
    return -1;
  }

  if (canTpSetAttr(tp_handle, iso15765_STmin, 0)        < 0 ||
      canTpSetAttr(tp_handle, iso15765_BS, 0)           < 0 ||
      canTpSetAttr(tp_handle, iso15765_TxTimeout, 1000) < 0 ||
      canTpSetAttr(tp_handle, iso15765_RxTimeout, 1000) < 0 ||
      canTpSetHandler(tp_handle, "RXindication",
                      iso15765_RxIndication)            < 0 ||
      canTpSetHandler(tp_handle, "TXconfirmation",
                      iso15765_TxConfirmation)          < 0) {
    kwp2000ShutDown();
    return -1;
  }

  expPosCount = RESPONSE_NONE_EXPECTED;

  return 0;
}


int kwp2000SendRequest (byte data[])
{
  timerNoResponse.timeout = 2000;
  timerStart(timerNoResponse);

  // Remember expected response values
  expPosCount    = data[0];
  expPosData     = data[1, expPosCount];
  expPosData[0] |= SERVICE_RESPONSE_BIT;

  canTpTransmit(tp_handle, data + 1);

  return 0;
}


float kwp2000GetReal (byte data[])
{
  int e = (data[0] >> 4) & 0x07;
  if (data[0] & 0x80) {
    e = -e;
  }

  int m = ((data[0] << 8) | data[1]) & 0x7FF;
  if (data[0] & 0x08) {
    m = -m;
  }

  return m * exp10((float)e);
}


void kwp2000GetReals (byte data[], float C[])
{
  for(int i = 0; i < C.count; i++) {
    C[i] = kwp2000GetReal(data + i * 2);
  }
}


int kwp2000GetConstants (byte data[], float C[])
{
  byte constants[10] = {2, 2, 3, 2, 2, 3, 1, 1, 1, 2};
  int formula = data[0];

  if (formula >= constants.count) {
    return -1;   // Bad formula
  }

  kwp2000GetReals(data + 1, C[0, constants[formula]]);

  return constants[formula];
}


int kwp2000ScalePhysToRaw (float num, byte data[], int &result)
{
  float C[3];
  int ret = kwp2000GetConstants(data, C);
  if (ret < 0) {
    return ret;
  }
  
  // Calculate raw value
  float scaled_num;
  switch (data[0]) {
  case 0x00:
    scaled_num = (num - C[1]) / C[0];
    break;
  case 0x01:
    scaled_num = (num / C[0]) - C[1];
    break;
  case 0x02:
    scaled_num = (C[0] / (num - C[2])) - C[1];
    break;
  case 0x03:
    scaled_num = (num - C[1]) * C[0];
    break;
  case 0x04:
    scaled_num = (num * C[1]) - C[0];
    break;
  case 0x05:
    scaled_num = C[1] * (num - C[2]) - C[0];
    break;
  case 0x06:
    scaled_num = num / C[0];
    break;
  case 0x07:
    scaled_num = num * C[0];
    break;
  case 0x08:
    scaled_num = num - C[0];
    break;
  case 0x09:
    scaled_num = (num * C[1]) / C[0];
    break;
  }

  result = round(scaled_num);

  return 0;
}


int kwp2000ScaleRawToPhys (int num, byte data[], float &result)
{
  float C[3];
  int ret = kwp2000GetConstants(data, C);
  if (ret < 0) {
    return ret;
  }
  
  // Calculate physical value
  switch (data[0]) {
  case 0x00:
    result = (C[0] * num) + C[1];
    break;
  case 0x01:
    result = C[0] * (num + C[1]);
    break;
  case 0x02:
    result = (C[0] / (C[1] + num)) + C[2];
    break;
  case 0x03:
    result = (num / C[0]) + C[1];
    break;
  case 0x04:
    result = (num + C[0]) / C[1];
    break;
  case 0x05:
    result = ((num + C[0]) / C[1]) + C[2];
    break;
  case 0x06:
    result = num * C[0];
    break;
  case 0x07:
    result = num / C[0];
    break;
  case 0x08:
    result = num + C[0];
    break;
  case 0x09:
    result = (num * C[0]) / C[1];
    break;
  }

  return 0;
}


int kwp2000FindScalingData (int id, byte data[])
{
  int i = 0;
  while ((i + 1 < data.count) && (data[i] != SCALING_DATA_END)) {
    if (data[i + 1] == id) {
      return i;
    }
    i += data[i];
  }

  return -1;
}


int kwp2000FindScalingByte (int type, byte data[])
{
  byte constants[10] = {2, 2, 3, 2, 2, 3, 1, 1, 1, 2};
  int i = 0;
  while (i < data.count) {
    if ((data[i] & SCALING_TYPE_MASK) == type) {
      return i;
    }
    switch (data[i] & SCALING_TYPE_MASK) {
    case SCALING_UNSIGNED_NUMERIC:
    case SCALING_SIGNED_NUMERIC:
    case SCALING_ASCII:
    case SCALING_ASCII_NUMERIC:
    case SCALING_UNSIGNED_NUM_W_INDICATION:
      i += 1;
      break;
    case SCALING_UNIT_FORMAT:
    case SCALING_STATE:
      i += 2;
      break;
    case SCALING_FORMULA:
      if (data[i + 1] >= constants.count) {
        return -1;
      }
      i += 2 + 2 * constants[data[i + 1]];
      break;
    default:
      return -1;
    }
  }

  return -1;
}


int kwp2000FindValueSize (byte data[])
{
  byte constants[10] = {2, 2, 3, 2, 2, 3, 1, 1, 1, 2};
  int size = 0;
  int i = 0;
  while (i < data.count) {
    switch (data[i] & SCALING_TYPE_MASK) {
    case SCALING_UNSIGNED_NUMERIC:
    case SCALING_SIGNED_NUMERIC:
    case SCALING_UNSIGNED_NUM_W_INDICATION:
      return data[i] & 0x0F;
    case SCALING_ASCII:
    case SCALING_ASCII_NUMERIC:
      size += data[i] & 0x0F;
      i += 1;
      break;
    case SCALING_UNIT_FORMAT:
      i += 2;
      break;
    case SCALING_STATE:
      return 1;
    case SCALING_FORMULA:
      if (data[i + 1] >= constants.count) {
        return -1;
      }
      i += 2 + 2 * constants[data[i + 1]];
      break;
    default:
      return -1;
    }
  }

  if (size) {
    return size;
  }

  return -1;
}


int kwp2000ScaleValue (float value, int id, byte data[], int &value_raw)
{
  int first = kwp2000FindScalingData(id, data);
  if (first < 0) {
    return first;
  }

  int length = data[first];
  if (first + length > data.count) {
    return -1;
  }

  int formula = kwp2000FindScalingByte(SCALING_FORMULA,
                                       data[first + 2, length - 2]);
  if (formula < 0) {
    return formula;
  }
  formula += first + 2;
  
  int ret = kwp2000ScalePhysToRaw(value, data + formula + 1, &value_raw);
  if (ret < 0) {
    return ret;
  }

  return kwp2000FindValueSize(data[first + 2, length - 2]);
}


int kwp2000WriteByLocalIdFloat (int id, float value, byte data[])
{
  int raw;
  int bytes = kwp2000ScaleValue(value, id, data, &raw);
  if (bytes < 0) {
    return bytes;
  }

  byte buf[4095];
  buf = WriteDataByLocalId;
  buf[2] = id;
  for(int i = bytes - 1; i >= 0; i--) {
    buf[3 + i] = raw & 0xFF;
    raw >>= 8;
  }

  kwp2000SendRequest(buf[0, 3 + bytes]);

  return bytes;
}


int kwp2000WriteByCommonIdBytes (int id, char text[], byte data[])
{
  int first = kwp2000FindScalingData(id, data);
  if (first < 0) {
    return first;
  }
  
  int length = data[first];
  if (first + length > data.count) {
    return -1;
  }
  
  int bytes = kwp2000FindValueSize(data[first + 2, length - 2]);
  if (bytes < 0) {
    return -1;
  }

  byte buf[4095];
  buf = WriteDataByCommonId;
  buf[3] = id;
  buf[4, bytes] = text;
  // Clear any unused bytes
  for(int i = text.count; i < bytes; i++) {
    buf[4 + i] = 0;
  }

  kwp2000SendRequest(buf[0, 4 + bytes]);

  return bytes;
}


void kwp2000AppendDTCDescription (char buf[], const char data[])
{
  int buflen = strlen(buf);

  sprintf(buf + buflen, "(%02x%02x %02x)",
          data[0], data[1], data[2]);
}


void kwp2000AppendNegativeResponseText (char buf[], int code)
{
  switch (code) {
  case 0x10:
    strcat(buf, "GeneralReject");
    break;
  case 0x11:
    strcat(buf, "ServiceNotSupported");
    break;
  case 0x12:
    strcat(buf, "subFunctionNotSupported-invalidFormat");
    break;
  case 0x22:
    strcat(buf, "ConditionsNotCorrectOrRequestSequenceError");
    break;
  case 0x23:
    strcat(buf, "RoutineNotCompleteOrServiceInProgress");
    break;
  case 0x31:
    strcat(buf, "RequestOutOfRange");
    break;
  case 0x78:
    strcat(buf, "requestCorrectlyReceived-ResponsePending");
    break;
  case 0x80:
    strcat(buf, "serviceNotSupportedInActiveDiagnosticSession");
    break;
  default:
    {
      char tmp[40];
      sprintf(tmp, "Unknown negative response: 0x%02x", code);
      strcat(buf, tmp);
    }
    break;
  }
}


void kwp2000AppendUnit (char buf[], int unit)
{
  char units[72] = "\x08\x06Second"
                   "\x0D\x06Ampere"
                   "\x0E\x04Volt"
                   "\x17\x07Celsius"
                   "\x1C\x05Hertz"
                   "\x24\x06Pascal"
                   "\x25\x03Bar"
                   "\x30\x04Km/h"
                   "\x33\x03Rpm"
                   "\x35\x07Percent";
  int i = 0;
  while (units[i]) {
    if (units[i] == unit) {
      strcat(buf, units[i + 2, units[i + 1]]);
      return;
    }
    i += units[i + 1] + 2;
  }

  strcat(buf, "Unknown");
}


// Event handlers

on CanTpMessage "TXconfirmation" {
  if (this.result == iso15765_OK) {
    return;
  }

  timerCancel(timerNoResponse);
  canTpAbort(tp_handle);

  if (this.result & (iso15765_TIMEOUT_A | iso15765_TIMEOUT_Bs |
                     iso15765_WFT_OVRN)) {
    kwp2000CallbackHandleResponse(RESPONSE_TIMEOUT, "");
  } else {
    kwp2000CallbackHandleResponse(RESPONSE_BAD, "");
  }
}


// qqq Dummy so far
on CanTpMessage "FFindication" {
}

// qqq Perhaps use different receivers for different data?
//     (Impossible without reference variables?)
on CanTpMessage "RXindication" {
  timerCancel(timerNoResponse);

  // If the ECU says something to us when we are
  // not expecting an answer, just ignore it.
  if (expPosCount == RESPONSE_NONE_EXPECTED) {
    canTpAbort(tp_handle);
    return;
  }

  if (this.result != canTp_OK) {
    canTpAbort(tp_handle);
    if (this.result & (iso15765_TIMEOUT_A | iso15765_TIMEOUT_Cr)) {
      kwp2000CallbackHandleResponse(RESPONSE_TIMEOUT, "");
    } else {
      kwp2000CallbackHandleResponse(RESPONSE_BAD, "");
    }
    return;
  }

  // Check response
  byte buf[RESPONSE_MAX_LENGTH_TO_CHECK];

  if (this.data.count < expPosCount) {
    kwp2000CallbackHandleResponse(RESPONSE_BAD, "");
    return;
  }

  if (this.data[0] == SERVICE_NEGATIVE_RESPONSE) {
    kwp2000CallbackHandleResponse(RESPONSE_NEGATIVE, this.data[0 .. 2]);
    return;
  }

  for(int i = 0; i < expPosCount; i++) {
    if (this.data[i] != expPosData[i]) {
      kwp2000CallbackHandleResponse(RESPONSE_BAD, "");
      return;
    }
  }

  // Any data received before the next send is bad!
  int first = expPosCount;
  expPosCount = RESPONSE_NONE_EXPECTED;

  if (this.data.count > first) {
    kwp2000CallbackHandleResponse(RESPONSE_POSITIVE,
                                  this.data[first .. this.data.count - 1]);
  } else {
    kwp2000CallbackHandleResponse(RESPONSE_POSITIVE, "");
  }
}


on Timer timerNoResponse {
  canTpAbort(tp_handle);

  kwp2000CallbackHandleResponse(RESPONSE_TIMEOUT, "");
}
