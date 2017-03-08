#include "kwp2000.inc"


variables {
  // Start "addresses" for the various operations
  const int CONNECT     = 0x100;
  const int PROGRAMMING = 0x200;
  const int READ_DTC    = 0x300;
  const int DISCONNECT  = 0x400;

  const int IDLE        = 0x000;   // Not executing


  // From host
  char xml_file_name[13];
  char FingerPrintSerial[16];
  char FingerPrintDate[8];

  // From XML file
  char VIN[32];
  char SpeedLimit[16];
  char SpeedLimit2[16];


  // Execution engine
  int pc  = IDLE;
  int dpc = 0;


  char error_text[80];
}

envvar {
  // Outgoing
  char text[4095];

  // Incoming
  int connect;
  char programming[32];
  char xmlfilename[13];
  int readDTC;
  int disconnect;
}


void Report(const char msg[]);
int  HandleError(int result, const char text[]);
void ReportECUInfo(const byte data[]);
void DTCsToText(char outbuf[], const byte data[]);
int  ReadValuesFromXMLFile(void);


void HaltExecution (void)
{
  pc = IDLE;
}


// qqq If we can find a nice way to break out the four modes into separate
//     functions, that would likely be better.
// Continue execution of current communication operation.
// Called when the last sub-operation was successfully completed.
// 
void Execute (const byte data[])
{
  static char buf[4096];
  static int estab_length, lstab_length;

  switch (pc++) {
  // ************* Connect ***************
  case     CONNECT:
    HandleError(kwp2000SendRequest(SetStandardSession),
                "set session");
    break;
  case 1 + CONNECT:
    HandleError(kwp2000SendRequest(ReadECUIdentification),
                "read ECU info");
    break;
  case 2 + CONNECT:
    ReportECUInfo(data);
    HaltExecution();
    break;


  // ************* Program ***************
  case     PROGRAMMING:
    if (HandleError(ReadValuesFromXMLFile(),
                    "read XML file") >= 0) {
      // qqq Programming session didn't work, so go with adjustment instead.
      HandleError(kwp2000SendRequest(SetAdjustmentSession),
                  "set adjustment session");
    }
    break;
  case 1 + PROGRAMMING:
    HandleError(kwp2000SendRequest(ReadECUIdentificationScalingTable),
                "read ECU ID scaling table");
    break;
  case 2 + PROGRAMMING:
    estab_length         = data.count;
    buf[0, estab_length] = data;
    HandleError(kwp2000WriteByCommonIdBytes(COMMON_ID_SERIAL, FingerPrintSerial,
                                            buf[0, estab_length]),
                "write finger print serial");
    break;
  case 3 + PROGRAMMING:
    HandleError(kwp2000WriteByCommonIdBytes(COMMON_ID_DATE, FingerPrintDate,
                                            buf[0, estab_length]),
                "write finger print data");
    break;
  case 4 + PROGRAMMING:
    HandleError(kwp2000SendRequest(ReadLocalIdentifierScalingTable),
                "read local ID scaling table");
    break;
  case 5 + PROGRAMMING:
    lstab_length = data.count;
    buf[estab_length, lstab_length] = data;
    HandleError(kwp2000WriteByLocalIdFloat(LOCAL_ID_SPEEDLIMIT, atof(SpeedLimit),
                                           buf[estab_length, lstab_length]),
                "scale and write SpeedLimit");
    break;
  case 6 + PROGRAMMING:
    HandleError(kwp2000WriteByLocalIdFloat(LOCAL_ID_SPEEDLIMIT2, atof(SpeedLimit2),
                                           buf[estab_length, lstab_length]),
                "scale and write SpeedLimit2");
    break;
  case 7 + PROGRAMMING:
    HandleError(kwp2000WriteByCommonIdBytes(COMMON_ID_VIN, VIN,
                                            buf[0, estab_length]),
                "write VIN number");
    break;
  case 8 + PROGRAMMING:
    HandleError(kwp2000SendRequest(ResetECU),
                "reset ECU");
    break;
  case 9 + PROGRAMMING:
    Report("Programming OK.\n");
    HaltExecution();
    break;


  // ************** Read DTCs **************
  case    READ_DTC:
    HandleError(kwp2000SendRequest(ReadDTCs),
                "read DTCs");
    break;
  case 1 + READ_DTC:
    strcpy(buf, "DTCs set before EraseDTC: ");
    DTCsToText(buf, data);
    // qqq What about ClearDiagnosticInformation?
    HandleError(kwp2000SendRequest(ClearDTCs),
                "clear DTCs");
    break;
  case 2 + READ_DTC:
    HandleError(kwp2000SendRequest(ReadDTCs),
                "reread DTCs");
    break;
  case 3 + READ_DTC:
    strcat(buf, "DTCs set after EraseDTC: ");
    DTCsToText(buf, data);
    Report(buf);
    HaltExecution();
    break;


  // ************** Disconnect **************
  case     DISCONNECT:
    HandleError(kwp2000SendRequest(ResetECU),
                "reset ECU");
    break;

  case 1 + DISCONNECT:
    Report("Disconnected.\n");
    HaltExecution();
    break;


  // ************** Idle **************
  case     IDLE:
    // This should never happen!
    HaltExecution();
  }
}


void StartOperation (int operation)
{
  // If we can't switch mode now, defer for later.
  if (pc != IDLE) {
    dpc = operation;
    return;
  }

  pc = operation;
  Execute("");
}


int HandleError (int result, const char text[])
{
  if (result < 0) {
    char buf[80];
    sprintf(buf, "Failed to %s (%d).\n", text, result);
    Report(buf);
    HaltExecution();
  } else {
    // Remember for use at callback failure.
    error_text = text;
  }

  return result;
}


// Called on completed receive from the kwp2000 library.
void kwp2000CallbackHandleResponse (int response, const byte data[])
{
  char buf[200];

  switch (response) {
  case RESPONSE_POSITIVE:
    Execute(data);
    break;

  // ************* Error reporting ***************

  case RESPONSE_NEGATIVE:
    sprintf(buf, "Operation \"%s\" failed. Service 0x%02x: ",
            error_text, data[1]);
    kwp2000AppendNegativeResponseText(buf, data[2]);
    strcat(buf, "\n");
    Report(buf);
    HaltExecution(); 
    break;

  case RESPONSE_TIMEOUT:
    sprintf(buf, "Operation \"%s\" failed. No response from ECU.\n", error_text);
    Report(buf);
    HaltExecution();
    break;

  case RESPONSE_BAD:
    sprintf(buf, "Operation \"%s\" failed. Bad response from ECU.\n", error_text);
    Report(buf);
    HaltExecution();
    break;
  }

  if ((pc == IDLE) && dpc) {
    StartOperation(dpc);
    dpc = 0;
  }
}


on start {
  canSetBitrate(0, canBITRATE_250K);
  canSetBusOutputControl(0, canDRIVER_NORMAL);
  canBusOn(0);
  if (kwp2000Init(0x11, 0xF1) < 0) {
    // qqq Stop script?
    Report("Failed to initialize kwp2000 library.\n");
  }
}


on exception {
  Report("**Exception**\n");
}


on stop {
  kwp2000ShutDown();
  canBusOff(0);
}


// Commands from the host

on envvar connect {
  StartOperation(CONNECT);
}

on envvar programming {
  char finger_print[32];
  envvarGetValue(programming, finger_print);
  FingerPrintDate   = finger_print[0, 6];
  FingerPrintSerial = finger_print[6, 10];

  envvarGetValue(xmlfilename, xml_file_name);

  StartOperation(PROGRAMMING);
}

on envvar readDTC {
  StartOperation(READ_DTC);
}

on envvar disconnect {
  StartOperation(DISCONNECT);
}


int ReadValuesFromXMLFile (void)
{
  XmlHandle xi;
  if (xmlOpen(xi, xml_file_name) < 0) {
    return -1;
  }

  int ret = 0;
  if (xmlGet(xi, SpeedLimit, "VehicleData", "Parameters", "SpeedLimit") < 0) {
    ret--;
  }
  if (xmlGet(xi, SpeedLimit2, "VehicleData", "Parameters", "SpeedLimit2") < 0) {
    ret--;
  }
  if (xmlGet(xi, VIN, "VehicleData", "VehicleID", "VINnumber") < 0) {
    ret--;
  }

  xmlClose(xi);

  return ret;
}


void Report (const char msg[])
{
  envvarSetValue(text, msg);

  printf("---------------\n%s---------------\n", msg);
}


void DTCsToText (char outbuf[], const byte data[])
{
  int count = data[0];
  if (count == 0) {
    strcat(outbuf, "none\n");
    return;
  }

  sprintf(outbuf + strlen(outbuf), "%d\n", count);

  for(int i = 0; i < count; i++) {
    kwp2000AppendDTCDescription(outbuf, data[1 + i * 3, 3]);
    strcat(outbuf, " ");
  }
  strcat(outbuf, "\n");
}


void ReportECUInfo (const byte data[])
{
  char buf[4096];

  strcpy(buf, "ECUSparePartNumber: ");
  strcat(buf, data[0,  12]);
  strcat(buf, "\nECUSoftwareNumber: ");
  strcat(buf, data[12, 12]);
  strcat(buf, "\nECUVersionNumber: ");
  strcat(buf, data[24,  9]);
  strcat(buf, "\nECUManufacturer: ");
  strcat(buf, data[33, 32]);
  strcat(buf, "\nECUManufacturingDate: ");
  strcat(buf, data[65,  6]);
  strcat(buf, "\nVIN: ");
  strcat(buf, data[71, 17]);
  strcat(buf, "\nECUHardwareNumber: ");
  strcat(buf, data[88, 12]);
  strcat(buf, "\nSystemName: ");
  strcat(buf, data[100,  4]);
  strcat(buf, "\nTesterSerialNumber: ");
  strcat(buf, data[104, 10]);
  strcat(buf, "\nProgrammingDate: ");
  strcat(buf, data[114,  6]);
  strcat(buf, "\nScaniaECUBootProgramNumber: ");
  strcat(buf, data[120,  7]);
  strcat(buf, "\nScaniaECUHardwareWithoutBootNumber: ");
  strcat(buf, data[127,  12]);
  strcat(buf, "\nServiceToolECUDiagnosticNumber: ");
  strcat(buf, data[139,  6]);
  strcat(buf, "\nApplicationInfoString: ");
  strcat(buf, data[145,  6]);
  strcat(buf, "\n");

  Report(buf);
}



//  ********* Remove these later


on key 'c' {
  StartOperation(CONNECT);
}

on key 'f' {
  FileHandle f;
  fileOpen(f, "xmlfile");
  filePuts(f, "<VehicleData>\n");
  filePuts(f, "    <VehicleID>\n");
  filePuts(f, "        <VINnumber>YS2P4X20001234567</VINnumber>\n");
  filePuts(f, "    </VehicleID>\n");
  filePuts(f, "    <Parameters>\n");
  filePuts(f, "        <SpeedLimit>72.8</SpeedLimit>\n");
  filePuts(f, "        <SpeedLimit2>25</SpeedLimit2>\n");
  filePuts(f, "    </Parameters>\n");
  filePuts(f, "</VehicleData>\n");
  fileClose(f);
}

on key 'p' {
  strcpy(FingerPrintSerial, "0123456789");
  strcpy(FingerPrintDate, "080908");
  strcpy(xml_file_name, "xmlfile");
  StartOperation(PROGRAMMING);
}

on key 't' {
  StartOperation(READ_DTC);
}

on key 'd' {
  StartOperation(DISCONNECT);
}

on key 'o' {
  byte data[4] = {0x01, 0x17, 0xFF, 0xFF};
  kwp2000SendRequest(data);
}

// qqq This is for testing
on key 's' {                  // Read SpeedLimit
  byte buf[3] = {1,  SERVICE_READ_DATA_BY_LOCAL_ID, LOCAL_ID_SPEEDLIMIT};
  kwp2000SendRequest(buf);
}

on key 'S' {                  // Read SpeedLimit2
  byte buf[3] = {1,  SERVICE_READ_DATA_BY_LOCAL_ID, LOCAL_ID_SPEEDLIMIT2};
  kwp2000SendRequest(buf);
}
