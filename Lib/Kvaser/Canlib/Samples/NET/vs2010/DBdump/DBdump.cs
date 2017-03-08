// This sample program can dump the selected contents of a database,
// create a dummy database, delete entries from a database, and do
// conversions between message data and signal values according to
// a database.
//
// Example use:
// DBdump dbase.dbc -f motor -m rpm -s -v 12.6 -p -v
//   Reads the database dbase.dbc
//   Locates the signal "rpm" in the message "motor" and displays
//   its definition. Then inserts the value 12.6 for "rpm" and
//   displays the contents of the CAN datafield.
//
// The general parameter use is that a flag, beginning with '-',
// specifies an operation on previously selected items in the
// database. Parameters that do not begin with '-' are data for
// the following flag.
// In the example above, '-f' will use dbase.dbc as the filename
// of an existing database to open. "motor" will then be used to
// specify which message '-m' looks for, and in the same way "rpm"
// is the signal that '-s' looks for in that message.
// The information in the now selected signal is dumped using the
// '-v' flag (had it been used after the '-f' or '-m' instead, 
// the contents of the entire database, or the "motor" message
// would have been dumped).
// Finally, 12.6 is a floating point number that '-p' uses as a
// physical parameter value for the selected "rpm" signal, and
// the next '-v' dumps the contents of the CAN message in question.
//
// An initial '-v' turns on verbose mode.
//
// File operations
// -f  Loads an existing database (takes filename parameter)
// -c  Creates a test database
// -w  Writes the current database (takes filename parameter)
//
// Search
// -m  Finds message by name (name parameter)
// -i  Finds message by id (id parameter)
// -s  Finds signal by name (name parameter)
// -N  Finds node by name (name parameter) for removal from signal
// -n  Finds node by name (name parameter) for removal from database
// -a  Finds node attribute by name (name parameter)
// 
// Data
// -z  Clears CAN message
// -h  Sets CAN message to hex data (hex data parameter)
//
// Signal
// -P  Fetches physical value from CAN message
// -R  Fetches raw value from CAN message
// -p  Store physical value in CAN message (value parameter)
// -r  Store raw value in CAN message (value parameter)
//
// Delete
// -d  Deletes last selected database item
//
// View
// -v  Dumps last selected database item
//

using System;
using System.Collections.Generic;
using System.Text;

using Kvaser.Kvadblib;

namespace DBdump {
  class dumper {
    static void DisplayError (Kvadblib.Status status, String routineName)
    {
      switch (status) {
      case Kvadblib.Status.OK:
        break;
      default:
        Console.WriteLine("{0} failed: {1}", routineName, status);
        Environment.Exit(1);
        break;
      }
    }

    static void dumpData (byte[] data)
    {
      for(int n = 0; n < data.Length; n++) {
        Console.Write("{0:x2}", data[n]);
      }
      Console.WriteLine();
    }

    static void dumpNode (Kvadblib.NodeHnd nh)
    {
      Console.WriteLine("No way to get node data");
    }

    static void dumpSignal (Kvadblib.SignalHnd sh)
    {
      double minval, maxval, factor, offset;
      DisplayError(Kvadblib.GetSignalValueLimits(sh, out minval, out maxval),
                   "GetSignalValueLimits");
      DisplayError(Kvadblib.GetSignalValueScaling(sh, out factor, out offset),
                   "GetSignalValueScaling");
      int startbit, length;
      DisplayError(Kvadblib.GetSignalValueSize(sh, out startbit, out length),
                   "GetSignalValueScaling");
      string name, qname;
      DisplayError(Kvadblib.GetSignalName(sh, out name), "GetSignalName");
      DisplayError(Kvadblib.GetSignalQualifiedName(sh, out qname),
                   "GetSignalQualifiedName");
      Console.WriteLine("{0} / {1}: {2} - {3}  * {4} + {5}  @ {6}, {7}",
                        name, qname,
                        minval, maxval,
                        factor, offset,
                        startbit, length);
      string comment, unit;
      DisplayError(Kvadblib.GetSignalComment(sh, out comment),
                   "GetSignalComment");
      DisplayError(Kvadblib.GetSignalUnit(sh, out unit),
                   "GetSignalUnit");
      Kvadblib.SignalEncoding encoding;
      DisplayError(Kvadblib.GetSignalEncoding(sh, out encoding),
                   "GetSignalEncoding");
      Kvadblib.SignalType rtype, ptype;
      DisplayError(Kvadblib.GetSignalRepresentationType(sh, out rtype),
                   "GetSignalRepresentationType");
      DisplayError(Kvadblib.GetSignalPresentationType(sh, out ptype),
                   "GetSignalPresentationType");
      Console.WriteLine("  {0} {1} {2} {3} {4}",
                        comment, unit, encoding, rtype, ptype);
    }

    static void dumpMessage (Kvadblib.MessageHnd mh)
    {
      string name, qname, comment;
      DisplayError(Kvadblib.GetMsgName(mh, out name), "GetMsgName");
      DisplayError(Kvadblib.GetMsgQualifiedName(mh, out qname),
                   "GetMsgQualifiedName");
      DisplayError(Kvadblib.GetMsgComment(mh, out comment),
                   "GetMsgComment");
      int id, dlc;
      Kvadblib.MESSAGE flags;
      DisplayError(Kvadblib.GetMsgId(mh, out id, out flags), "GetMsgId");
      DisplayError(Kvadblib.GetMsgDlc(mh, out dlc), "GetMsgDcl");
      Console.WriteLine("Message {0} / {1} ({2}): {3}/{4} {5}",
                        name, qname, comment, id, flags, dlc);

      Kvadblib.SignalHnd sh;
      Kvadblib.Status status = Kvadblib.GetFirstSignal(mh, out sh);
      while (status == Kvadblib.Status.OK) {
        DisplayError(status, "GetFirst/NextSignal");
        dumpSignal(sh);
        status = Kvadblib.GetNextSignal(mh, out sh);
      }
    }

    static void dumpDatabase (Kvadblib.Hnd dh)
    {
      Kvadblib.DATABASE flags;
      DisplayError(Kvadblib.GetFlags(dh, out flags), "GetFlags");
      Console.WriteLine("Flags: {0}", flags);

      Kvadblib.MessageHnd mh;
      Kvadblib.Status status = Kvadblib.GetFirstMsg(dh, out mh);
      while (status == Kvadblib.Status.OK)
      {
        DisplayError(status, "GetFirst/NextMsg");
        dumpMessage(mh);
        status = Kvadblib.GetNextMsg(dh, out mh);
      }

      Kvadblib.NodeHnd nh;
      status = Kvadblib.GetFirstNode(dh, out nh);
      while (status == Kvadblib.Status.OK) {
        DisplayError(status, "GetFirst/NextNode");
        dumpNode(nh);
        status = Kvadblib.GetNextNode(dh, out nh);
      }
    }

    static void createDatabase (Kvadblib.Hnd dh)
    {
      DisplayError(Kvadblib.SetFlags(dh, Kvadblib.DATABASE.J1939), "SetFlags");

      var nh = new List<Kvadblib.NodeHnd>();
      for(int n = 0; n < 4; n++) {
        Kvadblib.NodeHnd nhx;
        DisplayError(Kvadblib.AddNode(dh, out nhx), "AddNode");
        nh.Add(nhx);
        DisplayError(Kvadblib.SetNodeName(nhx, "node" + n.ToString()),
                      "SetNodeName");
        DisplayError(Kvadblib.SetNodeComment(nhx, "comment" + n.ToString()),
                     "SetNodeComment");
      }

      for (int m = 0; m < 4; m++) {
        Kvadblib.MessageHnd mh;
        DisplayError(Kvadblib.AddMsg(dh, out mh), "AddMsg");
        DisplayError(Kvadblib.SetMsgName(mh, "msg" + m.ToString()),
                     "SetMsgName");
        DisplayError(Kvadblib.SetMsgComment(mh, "comment" + m.ToString()),
                     "SetMsgComment");
        DisplayError(Kvadblib.SetMsgId(mh, 0x42 + m, 0), "SetMsgId");
        DisplayError(Kvadblib.SetMsgDlc(mh, 8 - m), "SetMsgDlc");

        for (int n = 0; n < 4; n++) {
          Kvadblib.SignalHnd sh;
          DisplayError(Kvadblib.AddSignal(mh, out sh), "AddSignal");
          DisplayError(Kvadblib.SetSignalValueLimits(sh, n, n + 10),
                       "SetSignalValueLimits");
          DisplayError(Kvadblib.SetSignalValueScaling(sh, n / 2.0, n),
                       "SetSignalValueScaling");
          DisplayError(Kvadblib.SetSignalValueSize(sh, n * 16, 16),
                       "SetSignalValueSize");
          DisplayError(Kvadblib.SetSignalName(sh, "sig" + m.ToString() + "_" + n.ToString()),
                       "SetSignalName");
          DisplayError(Kvadblib.SetSignalComment(sh, "comment" + n.ToString()),
                       "SetSignalComment");
          DisplayError(Kvadblib.SetSignalUnit(sh, "m/s"),
                       "SetSignalUnit");
          DisplayError(Kvadblib.SetSignalEncoding(sh, (Kvadblib.SignalEncoding)n),
                       "SetSignalEncoding");
          DisplayError(Kvadblib.SetSignalRepresentationType(sh, Kvadblib.SignalType.Signed + n),
                       "SetSignalRepresentationType");

          if (n >= 1 && n <= nh.Count) {
            DisplayError(Kvadblib.AddReceiveNodeToSignal(sh, nh[n - 1]),
                         "AddReceiveNodeToSignal");
          }
        }
      }
    }

    static int Main(string[] args)
    {
      // Dummy initializations to make the compiler happy.
      var dh = new Kvadblib.Hnd();
      var mh = new Kvadblib.MessageHnd();
      var sh = new Kvadblib.SignalHnd();
      var nh = new Kvadblib.NodeHnd();
      bool verbose = false;
      char last = '\0';
      int ival;
      double fval;
      byte[] data = new byte[8];

      DisplayError(Kvadblib.Open(out dh), "Open");

      for(int n = 0; n < args.Length; n++) {
        if (verbose) {
          Console.WriteLine("{0}: {1}", n, args[n]);
        }

        if (args[n][0] == '-') {
          if ((n == 0) && (args[n][1] != 'v')) {
            DisplayError(Kvadblib.Status.Err_Param, "Argument fetch");
          }
          switch (args[n][1]) {
          // File operations
          case 'f':
            DisplayError(Kvadblib.ReadFile(dh, args[n - 1]),
                         "ReadFile");
            break;
          case 'w':
            DisplayError(Kvadblib.WriteFile(dh, args[n - 1]),
                         "WriteFile");
            break;
          case 'c':
            DisplayError(Kvadblib.Create(dh, args[n - 1], ""),
                         "Create");
            createDatabase(dh);
            break;

          // Locate item
          case 'm':
            DisplayError(Kvadblib.GetMsgByName(dh, args[n - 1], out mh),
                         "GetMsgByName");
            break;
          case 'i':
            DisplayError(Kvadblib.GetMsgById(dh, int.Parse(args[n - 1]), out mh),
                         "GetMsgById");
            break;
          case 's':
            DisplayError(Kvadblib.GetSignalByName(mh, args[n - 1], out sh),
                         "GetSignalByName");
            break;
          case 'N':
          case 'n':
            DisplayError(Kvadblib.GetNodeByName(dh, args[n - 1], out nh),
                         "GetNodeByName");
            break;
          case 'a':
            DisplayError(Kvadblib.GetNodeAttributeIntByName(nh, args[n - 1], out ival),
                         "GetNodeAttributeIntByName");
            Console.WriteLine("Node attribute: {1}", ival);
            break;

          // Message data
          case 'z':
            for (int i = 0; i < data.Length; i++) {
              data[i] = 0;
            }
            break;
          case 'h':
            if (((args[n - 1].Length % 2) == 1) || (args[n - 1].Length > 16)) {
              DisplayError(Kvadblib.Status.Err_Param, "Hex input");
            }
            for(int i = 0; i < args[n - 1].Length; i += 2) {
              data[i / 2] = (byte)Convert.ToInt32(args[n - 1].Substring(i, 2), 16);
            }
            break;

          // Signal data
          case 'P':
            DisplayError(Kvadblib.GetSignalValueFloat(sh, out fval, data, 8),
                         "GetSignalValueFloat");
            Console.WriteLine("Physical: {0}", fval);
            break;
          case 'R':
            DisplayError(Kvadblib.GetSignalValueInteger(sh, out ival, data, 8),
                         "GetSignalValueInteger");
            Console.WriteLine("Raw: {0}", ival);
            break;
          case 'p':
            if (!double.TryParse(args[n - 1], out fval)) {
              DisplayError(Kvadblib.Status.Err_Param, "double.TryParse");
            }
            DisplayError(Kvadblib.StoreSignalValuePhys(sh, data, 8, fval),
                         "StoreSignalValuePhys");
            break;
          case 'r':
            if (!int.TryParse(args[n - 1], out ival)) {
              DisplayError(Kvadblib.Status.Err_Param, "int.TryParse");
            }
            DisplayError(Kvadblib.StoreSignalValueRaw(sh, data, 8, ival),
                         "StoreSignalValueRaw");
            break;

          // Delete
          case 'd':
            switch (last) {
            case 'm':
              DisplayError(Kvadblib.DeleteMsg(dh, mh), "DeleteMsg");
              break;
            case 's':
              DisplayError(Kvadblib.DeleteSignal(mh, sh), "DeleteSignal");
              break;
            case 'n':
              DisplayError(Kvadblib.DeleteNode(dh, nh), "DeleteNode");
              break;
            case 'N':
              DisplayError(Kvadblib.RemoveReceiveNodeFromSignal(sh, nh),
                           "RemoveReceiveNodeFromSignal");
              break;
            default:
              DisplayError(Kvadblib.Status.Err_Param, args[n]);
              break;
            }
            break;

          // View
          case 'v':
            switch (last) {
            case 'f':
            case 'c':
              dumpDatabase(dh);
              break;
            case 'm':
              dumpMessage(mh);
              break;
            case 's':
              dumpSignal(sh);
              break;
            case 'n':
              dumpNode(nh);
              break;
            case 'h':
            case 'p':
            case 'r':
              dumpData(data);
              break;
            case '\0':
              verbose = true;
              break;
            default:
              DisplayError(Kvadblib.Status.Err_Param, args[n]);
              break;
            }
            break;
          default:
            DisplayError(Kvadblib.Status.Err_Param, args[n]);
            break;
          }
          last = args[n][1];
        }
      }

      
      DisplayError(Kvadblib.Close(dh), "Close");

      return 0;
    }
  }
}
