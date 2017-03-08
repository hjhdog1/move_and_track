/*
**                         Copyright 2010 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This is a sample program that uses the CANdB to create and access CAN
** databases. The purpuse of this program is to demonstrate certain
** programming techniques. It may or may not be useful to you.
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CANDB requires the following #include.
#include "kvaDblib.h"

// Forward declarations
KvaDbStatus createNewDatabase(char *filename);
KvaDbStatus dumpDatabase(char *filename);
KvaDbStatus editDatabase(char *filename);

// -----------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------
int main (int argc, char **argv) {
  KvaDbStatus status = kvaDbOK;
  char filename[] = "temp.dlc";
  
  printf("\nSample program for the CANdb-API DLL:\n\n");
  
  // Create a new database from scratch
  printf("1. Create a new data base with two messages and three signals...");
  status = createNewDatabase(filename);
  if (status != kvaDbOK) {
    printf("\nError: Could not create a new database: %d\n", status);
    return 0;
  }
  printf("done.\n\n");
  
  // Dump the contents of the database to stdout
  printf("2. Display the contents of the database:\n");
  status = dumpDatabase(filename);  
  if (status != kvaDbOK) {
    printf("\nError: Could not dump the contents of the database: %d\n", status);
    return 0;
  }

  printf("3. Remove the first message and change a signal in the second...");
  // Edit the new database
  status = editDatabase(filename);
  if (status != kvaDbOK) {
    printf("\nError: Could not edit a new database: %d\n", status);
    return 0;
  }
  printf("done.\n\n");

  // Dump the contents of the updated database to stdout
  printf("4. Display the contents of the edited database:\n");
  status = dumpDatabase(filename);  
  if (status != kvaDbOK) {
    printf("\nError: Could not dump the contents of the updated database: %d\n", status);
    return 0;
  }
  
  printf("\nDone.\n");
  
  return 0;
}


// -----------------------------------------------------------------------
// Convert signal encoding to a string
// -----------------------------------------------------------------------
const char* sigEncToStr(KvaDbSignalEncoding sigEnc)
{
  if (sigEnc == kvaDb_Intel) {
    return "Intel";
  } else if (sigEnc == kvaDb_Motorola) {
    return "Motorola";
  } else {
    return "Undefined";
  }
}

// -----------------------------------------------------------------------
// Convert signal representation to a string
// -----------------------------------------------------------------------
const char* sigRepToStr(KvaDbSignalType sigType)
{
  if (sigType == kvaDb_Invalid) {
    return "Invalid";
  } else if (sigType == kvaDb_Signed) {
    return "Signed";
  } else if (sigType == kvaDb_Unsigned) {
    return "Unsigned";
  } else if (sigType == kvaDb_Float) {
    return "Float";
  } else if (sigType == kvaDb_Double) {
    return "Double";        
  } else {
    return "Undefined";
  }
}

// -----------------------------------------------------------------------
// 
// Create a new database with two messages and three signals
//
// -----------------------------------------------------------------------
KvaDbStatus createNewDatabase (char *filename) {
  KvaDbStatus status = kvaDbOK;
  KvaDbHnd dh;
  KvaDbMessageHnd mh;
  KvaDbSignalHnd sh;
    
  unsigned char data[8];
  int dlc = 8;
  int t_raw = 0;
  double t_phys = 0;
  memset(data, 0, sizeof(data));
  
  // Open a database handle
  status = kvaDbOpen(&dh);
  if (status != kvaDbOK) {
    printf ("kvaDbOpen falied: %d\n", status);
    return status;
  }
  
  // Create a new empty database
  status = kvaDbCreate(dh, "T-base", NULL);
    if (status != kvaDbOK) {
    printf ("kvaDbCreate failed: %d\n", status);
    return status;
  }
  
  // Add a new message to the database
  // ---------------------------------
  status = kvaDbAddMsg(dh, &mh);
  if (status != kvaDbOK) {
    printf("kvaDbAddMsg failed: %d\n", status);
    return status;
  }
  
  // Set the message properties
  status = kvaDbSetMsgName(mh, "TempMessage01");
  if (status != kvaDbOK) {
    printf("kvaDbSetMsgName failed: %d\n", status);
    return status;
  }  
  
  // Set the message id
  status = kvaDbSetMsgId(mh, 340, 0);
  if (status != kvaDbOK) {
    printf("kvaDbSetMsgId failed: %d\n", status);
    return status;
  }

  // Set the message comment
  status = kvaDbSetMsgComment(mh, "Temperature message 02.");
  if (status != kvaDbOK) {
    printf("kvaDbSetMsgComment failed: %d\n", status);
    return status;
  }

  // Set the message dlc  
  status = kvaDbSetMsgDlc(mh, dlc);
  if (status != kvaDbOK) {
    printf("kvaDbSetMsgDlc failed: %d\n", status);
    return status;
  }
    
  // Add a signal to the new message
  // -------------------------------
  status = kvaDbAddSignal(mh, &sh);
  if (status != kvaDbOK) {
    printf("kvaDbAddSignal failed: %d\n", status);
    return status;
  }
    
  // Set signal name
  status = kvaDbSetSignalName(sh, "Status");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalName failed: %d\n", status);
    return status;
  }
  
  // Set signal comment
  status = kvaDbSetSignalComment(sh,"The status signal.");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalComment failed: %d\n", status);
    return status;
  }

  // Set signal unit
  status = kvaDbSetSignalUnit(sh, "-");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalUnit failed: %d\n", status);
    return status;
  }

  // Set signal encoding
  status = kvaDbSetSignalEncoding(sh, kvaDb_Intel);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalEncoding failed: %d\n", status);
    return status;
  }

  // Set signal representation
  status = kvaDbSetSignalRepresentationType(sh, kvaDb_Unsigned);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalRepresentationType failed: %d\n", status);
    return status;
  }
  
  // Set signal value limits
  status = kvaDbSetSignalValueLimits(sh, 0, 0xFFFFFFFF);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalValueLimits failed: %d\n", status);
    return status;
  }

  // Set signal value size
  status = kvaDbSetSignalValueSize(sh, 0, 32);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalValueSize failed: %d\n", status);
    return status;
  }

  
  // Add a second message with two signals to the database
  // -----------------------------------------------------
  status = kvaDbAddMsg(dh, &mh);
  if (status != kvaDbOK) {
    printf("kvaDbAddMsg failed: %d\n", status);
    return status;
  }
  
  // Set the message name
  status = kvaDbSetMsgName(mh, "TempMessage02");
  if (status != kvaDbOK) {
    printf("kvaDbSetMsgName failed: %d\n", status);
    return status;
  }
  
  // Set the message id
  status = kvaDbSetMsgId(mh, 345, 0);
  if (status != kvaDbOK) {
    printf("kvaDbSetMsgId failed: %d\n", status);
    return status;
  }
  
  // Set the message comment
  status = kvaDbSetMsgComment(mh, "Temperature message 01.");
  if (status != kvaDbOK) {
    printf("kvaDbSetMsgComment failed: %d\n", status);
    return status;
  }
  
  // Set the message dlc
  status = kvaDbSetMsgDlc(mh, dlc);
  if (status != kvaDbOK) {
    printf("kvaDbSetMsgDlc failed: %d\n", status);
    return status;
  }

  // Add a signal to the second message
  // ----------------------------------
  status = kvaDbAddSignal(mh, &sh);
  if (status != kvaDbOK) {
    printf("kvaDbAddSignal failed: %d\n", status);
    return status;
  }
    
  // Set signal name
  status = kvaDbSetSignalName(sh, "T01");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalName failed: %d\n", status);
    return status;
  }
  
  // Set signal comment
  status = kvaDbSetSignalComment(sh,"The first temperature signal.");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalComment failed: %d\n", status);
    return status;
  }

  // Set signal unit
  status = kvaDbSetSignalUnit(sh, "C");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalUnit failed: %d\n", status);
    return status;
  }

  // Set signal encoding
  status = kvaDbSetSignalEncoding(sh, kvaDb_Intel);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalEncoding failed: %d\n", status);
    return status;
  }

  // Set signal representation
  status = kvaDbSetSignalRepresentationType(sh, kvaDb_Signed);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalRepresentationType failed: %d\n", status);
    return status;
  }
  
  // Set signal value limits
  status = kvaDbSetSignalValueLimits(sh, -100, 1000);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalValueLimits failed: %d\n", status);
    return status;
  }

  // Set signal scale factor and offset
  status = kvaDbSetSignalValueScaling(sh, 0.02, 10);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalValueScaling failed: %d\n", status);
    return status;
  }
  
  // Set signal value size
  status = kvaDbSetSignalValueSize(sh, 0, 32);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalValueSize failed: %d\n", status);
    return status;
  }

  // Add another signal to the second message
  // ----------------------------------------
  status = kvaDbAddSignal(mh, &sh);
  if (status != kvaDbOK) {
    printf("kvaDbAddSignal failed: %d\n", status);
    return status;
  }
  
  // Set signal name
  status = kvaDbSetSignalName(sh, "T02");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalName failed: %d\n", status);
    return status;
  }
  
  // Set signal comment
  status = kvaDbSetSignalComment(sh,"The second temperature signal.");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalComment failed: %d\n", status);
    return status;
  }

  // Set signal unit
  status = kvaDbSetSignalUnit(sh, "C");
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalUnit failed: %d\n", status);
    return status;
  }

  // Set signal encoding
  status = kvaDbSetSignalEncoding(sh, kvaDb_Intel);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalEncoding failed: %d\n", status);
    return status;
  }

  // Set signal representation
  status = kvaDbSetSignalRepresentationType(sh, kvaDb_Signed);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalRepresentationType failed: %d\n", status);
    return status;
  }
  
  // Set signal value limits
  status = kvaDbSetSignalValueLimits(sh, -100, 1000);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalValueLimits failed: %d\n", status);
    return status;
  }

  // Set signal scale factor and offset
  status = kvaDbSetSignalValueScaling(sh, 0.01, -10);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalValueScaling failed: %d\n", status);
    return status;
  }
  
  // Set signal value size
  status = kvaDbSetSignalValueSize(sh, 31, 32);
  if (status != kvaDbOK) {
    printf("kvaDbSetSignalValueSize failed: %d\n", status);
    return status;
  }
    
  // Use the signal data conversion functions
  // ----------------------------------------
  status = kvaDbGetSignalByName(mh, "T01", &sh);
  if (status != kvaDbOK) {
    printf("kvaDbGetSignalByName failed: %d\n", status);
    return status;
  }
  
  // Convert the physical temperature value 31.5 to CAN data
  status = kvaDbStoreSignalValuePhys(sh, &data[0], sizeof(data), 31.5);
  if (status != kvaDbOK) {
    printf("kvaDbStoreSignalValuePhys failed: %d\n", status);
    return status;
  }
  
  // Convert the CAN data back to physical value as float 
  status = kvaDbGetSignalValueFloat(sh, &t_phys, data, sizeof(data));
  if (status != kvaDbOK) {
    printf("kvaDbGetSignalValueFloat failed: %d\n", status);
    return status;
  }
  
  // Convert the CAN data back to physical value as float
  status = kvaDbGetSignalValueInteger(sh, &t_raw, data, sizeof(data));
  if (status != kvaDbOK) {
    printf("kvaDbGetSignalValueInteger failed: %d\n", status);
    return status;
  }  
  
  // Save the database to file
  status = kvaDbWriteFile(dh, filename);
  if (status != kvaDbOK) {
    printf("kvaDbWriteFile failed: %d\n", status);
    return status;
  }  
  
  // Close and free allocated memory
  status = kvaDbClose(dh);
  if (status != kvaDbOK) {
    printf("kvaDbClose failed: %d\n", status);
    return status;
  }
  
  return status;
}


// -----------------------------------------------------------------------
//
// Dump the contents of a database to stdout
//
// -----------------------------------------------------------------------
KvaDbStatus dumpDatabase (char *filename)
{
  KvaDbStatus status = kvaDbOK;
  KvaDbHnd dh = 0;
  KvaDbMessageHnd mh = 0;
  KvaDbSignalHnd sh = 0;
  unsigned int flags = 0;
  int i = 0;
  
  // Open a database handle
  status = kvaDbOpen (&dh);
  if (status != kvaDbOK) {
    printf("Could not create a database handle: %d\n", status);
    return status;
  }
  
  // Load the database file 
  status = kvaDbReadFile (dh, filename);
  if (status != kvaDbOK) {
    printf("Could not load '%s': %d\n", filename, status);
    return status;
  }
  
  status = kvaDbGetFlags(dh, &flags);
  if (status != kvaDbOK) {
    printf ("kvaDbGetFlags failed: %d\n", status);
    return status;
  }
  printf ("  Database: flags=0x%x\n", flags);
  
  
  // Get the first message in the database
  status = kvaDbGetFirstMsg (dh, &mh);
  if (status != kvaDbOK) {
    printf ("kvaDbGetFirstMsg failed: %d\n", status);
    return status;
  }

  // Go through all messages in the database
  while (status == kvaDbOK) {
    char msg_name[100];
    char msg_qname[200];
    char msg_comment[200];
    int  dlc = 0;
    unsigned int id = 0;
    
    // Clear the strings
    memset(msg_name,    0, sizeof(msg_name));
    memset(msg_qname,   0, sizeof(msg_qname));
    memset(msg_comment, 0, sizeof(msg_comment));
    
    // Get the properties for each message
    status = kvaDbGetMsgName (mh, msg_name, sizeof (msg_name));
    if (status != kvaDbOK) {
      printf ("kvaDbGetMsgName failed: %d\n", status);
      return status;
    }

    status = kvaDbGetMsgQualifiedName (mh, msg_qname, sizeof (msg_qname));
    if (status != kvaDbOK) {
      printf ("kvaDbGetMsgQualifiedName failed: %d\n", status);
      return status;
    }

    status = kvaDbGetMsgComment (mh, msg_comment, sizeof (msg_comment));
    if (status != kvaDbOK) {
      printf ("kvaDbGetMsgComment failed: %d\n", status);
      return status;
    }

    status = kvaDbGetMsgId (mh, &id, &flags);
    if (status != kvaDbOK) {
      printf ("kvaDbGetMsgId failed: %d\n", status);
      return status;
    }

    status = kvaDbGetMsgDlc (mh, &dlc);
    if (status != kvaDbOK) {
      printf ("kvaDbGetMsgDlc failed: %d\n", status);
      return status;
    }
    
    // Print the properties for each message
    printf ("    Msg: name='%s'\n", msg_name);
    printf ("         qname='%s', comment='%s'\n", msg_qname, msg_comment);
    printf ("         id=%x, dlc=%d, flags=%x\n", id, dlc, flags);

    
    // Go through all signals for this message
    status = kvaDbGetFirstSignal (mh, &sh);
    while (status == kvaDbOK) {
      char signal_name[100];
      char signal_qname[200];
      char signal_comment[200];
      char signal_unit[100];
      KvaDbSignalEncoding sigEnc;
      KvaDbSignalType sigType;
      int startbit = 0;
      int length = 0;
      double minval = 0;
      double maxval = 0;
      double factor = 0;
      double offset = 0;
      
      // Reset the strings
      memset(signal_name,    0, sizeof(signal_name));
      memset(signal_qname,   0, sizeof(signal_qname));
      memset(signal_comment, 0, sizeof(signal_comment));
      memset(signal_unit,    0, sizeof(signal_unit));
      
      // Get the properties for each signal.
      status = kvaDbGetSignalName (sh, signal_name, sizeof (signal_name));
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalName failed: %d\n", status);
        return status;
      }
      
      status = kvaDbGetSignalQualifiedName (sh, signal_qname, sizeof (signal_qname));
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalQualifiedName failed: %d\n", status);
        return status;
      }
      
      status = kvaDbGetSignalComment (sh, signal_comment, sizeof (signal_comment));
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalComment failed: %d\n", status);
        return status;
      }

      status = kvaDbGetSignalUnit (sh, signal_unit, sizeof (signal_unit));
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalUnit failed: %d\n", status);
        return status;
      }

      status = kvaDbGetSignalEncoding (sh, &sigEnc);
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalEncoding failed: %d\n", status);
        return status;
      }

      status = kvaDbGetSignalRepresentationType(sh, &sigType);
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalRepresentationType failed: %d\n", status);
        return status;
      }

      status = kvaDbGetSignalValueLimits(sh, &minval, &maxval);
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalValueLimits failed: %d\n", status);
        return status;
      }

      status = kvaDbGetSignalValueScaling(sh, &factor, &offset);
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalValueScaling failed: %d\n", status);
        return status;
      }

      status = kvaDbGetSignalValueSize(sh, &startbit, &length);
      if (status != kvaDbOK) {
        printf ("kvaDbGetSignalValueSize failed: %d\n", status);
        return status;
      }

      printf("      Signal: name='%s', unit='%s'\n", signal_name, signal_unit);
      printf("              qname='%s'\n", signal_qname);
      printf("              comment='%s'\n", signal_comment);
      printf("              encoding='%s'\n", sigEncToStr(sigEnc)); 
      printf("              representation='%s'\n", sigRepToStr(sigType));
      printf("              value min=%.02f, value max=%.02f\n", minval, maxval);
      printf("              scale faktor=%.02f, offset=%.02f\n", factor, offset);
      printf("              startbit=%d, length=%d\n", startbit, length);
      status = kvaDbGetNextSignal (mh, &sh);
    }

    status = kvaDbGetNextMsg (dh, &mh);
  }

  status = kvaDbClose (dh);
  if (status != kvaDbOK) {
      printf ("kvaDbClose failed: %d\n", status);
      return status;
  }
  printf("\n\n");
  return kvaDbOK;
}

// -----------------------------------------------------------------------
//
// Edit the new database by removing the first message and updating
// the scale factor for a signal in the second message.
//
// -----------------------------------------------------------------------

KvaDbStatus editDatabase (char* filename) {

  KvaDbStatus status = kvaDbOK;
  KvaDbHnd dh=0;
  KvaDbMessageHnd mh;
  KvaDbSignalHnd sh;
  
  // Open a database handle
  status = kvaDbOpen (&dh);
  if (status != kvaDbOK) {
    printf("kvaDbOpen falied: %d\n", status);
    return status;
  }
  
  // Load the database file 
  status = kvaDbReadFile (dh, filename);
  if (status != kvaDbOK) {
    printf("Could not load '%s': %d\n", filename, status);
    return status;
  }

  // Search for a message by indentifier
  status = kvaDbGetMsgById(dh, 340, &mh);
  if (status != kvaDbOK) {
    printf("Could not find the message with identifier '340':%d\n", status);
    return status;
  }
  
  // Delete all signals in this message
  status = kvaDbGetFirstSignal (mh, &sh);
  while (status == kvaDbOK) {
    status = kvaDbDeleteSignal(mh, sh);
    if (status != kvaDbOK) {
      printf("Could not delete a signal: %d\n", status);
      break;
    }
    status = kvaDbGetNextSignal (mh, &sh);
  }
  
  // Delete the message
  status = kvaDbDeleteMsg(dh, mh);
    if (status != kvaDbOK) {
      printf("Could not delete the message with identifier '340':%d\n", status);
      return status;
    }
  
  // Search for a message by name
  status = kvaDbGetMsgByName(dh, "TempMessage02", &mh);
  if (status != kvaDbOK) {
    printf("Could not find the message with name 'TempMessage02':%d\n", status);
    return status;
  }
  
  // Search for a signal in this message
  status = kvaDbGetSignalByName(mh, "T01", &sh);
  if (status != kvaDbOK) {
    printf("Could not find the signal with name 'T01':%d\n", status);
    return status;
  }

  // Update the scaling
  status = kvaDbSetSignalValueScaling(sh, 0.04, 5);
  if (status != kvaDbOK) {
    printf("Could not change the scale factor for signal 'T01':%d\n", status);
    return status;
  }
  
  // Save the modified database to file
  status = kvaDbWriteFile(dh, filename);
  if (status != kvaDbOK) {
    printf("kvaDbWriteFile failed:%d\n", status);
    return status;
  }
  
  // Close and free allocated memory
  status = kvaDbClose(dh);
  if (status != kvaDbOK) {
    printf("kvaDbClose falied:%d\n", status);
    return status;
  }

  return status;
}
