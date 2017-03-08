using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

using canlibCLSNET;

namespace CSdump
{
  class CSdumpall
  {
    static void DisplayError(Canlib.canStatus status, String routineName)
    {
      String errText = "";
      if (status != Canlib.canStatus.canOK)
      {
        Canlib.canGetErrorText(status, out errText);
        Console.WriteLine("{2} failed: {0} = {1}", status, errText, routineName);
        Environment.Exit(0);
      }
      else
        Console.WriteLine("{0} succeeded", routineName);
    }

    static void DisplayMessage(int id, int dlc, byte[] data, int flags, long time)
    {
      if ((flags & Canlib.canMSGERR_OVERRUN) > 0)
        Console.WriteLine("****  RECEIVE OVERRUN ****");
      if ((flags & Canlib.canMSG_ERROR_FRAME) == Canlib.canMSG_ERROR_FRAME)
        Console.WriteLine("ErrorFrame                                          {0}", time);
      else
      {
        Console.Write("{0:x8} ", id);
        if ((flags & Canlib.canMSG_EXT) == Canlib.canMSG_EXT)
          Console.Write("X");
        else
          Console.Write(" ");
        if ((flags & Canlib.canMSG_RTR) == Canlib.canMSG_RTR)
          Console.Write("R");
        else
          Console.Write(" ");
        if ((flags & Canlib.canMSG_TXACK) == Canlib.canMSG_TXACK)
          Console.Write("A");
        else
          Console.Write(" ");
        if ((flags & Canlib.canMSG_WAKEUP) == Canlib.canMSG_WAKEUP)
          Console.Write("W");
        else
          Console.Write(" ");
        Console.Write("  {0:x1} ", dlc);
        for (int i = 0; i < 8; i++)
        {
          if (i < dlc)
            Console.Write("  {0:x2}", data[i]);
          else
            Console.Write("    ");
        }
        Console.WriteLine("   {0}", time);
      }
    }

    static void Main(string[] args)
    {
      Canlib.canStatus status;
      int chanHandle;

      Canlib.canInitializeLibrary();
      Console.WriteLine("CAN Interface Library Initialized");

      chanHandle = Canlib.canOpenChannel(0, Canlib.canOPEN_ACCEPT_VIRTUAL);
      DisplayError((Canlib.canStatus)chanHandle, "canOpenChannel");

      status = Canlib.canSetBusParams(chanHandle, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
      DisplayError(status, "canSetBusParams");

      Object winHandle = new IntPtr(-1);
      status = Canlib.canIoCtl(chanHandle, Canlib.canIOCTL_GET_EVENTHANDLE, ref winHandle);

      DisplayError(status, "canIoCtl");

      object bufLevel = (UInt32)2048;
      status = Canlib.canIoCtl(chanHandle, Canlib.canIOCTL_SET_RX_QUEUE_SIZE, ref bufLevel);
      DisplayError(status, "canIoCtl");

      status = Canlib.canBusOn(chanHandle);
      DisplayError(status, "canBusOn");

      Console.WriteLine("Press Escape Key to exit");
      Console.WriteLine("   ID    Flag DLC  Data                             Timestamp");

      CanLibWaitEvent kvEvent = new CanLibWaitEvent(winHandle);
      WaitHandle[] waitHandles = new WaitHandle[] { kvEvent };

      bool notFinished = true;

      while (notFinished)
      {
        int index = WaitHandle.WaitAny(waitHandles, 1000, false);

        if (index == 0)
        {
          // Didn't timeout... we received a CAN event
          int id = 0;
          byte[] data = new byte[8] { 0, 0, 0, 0, 0, 0, 0, 0 };
          int dlc = 0;
          int flag = 0;
          long time = 0;

          // empty the receive buffer before waiting again
          while ((status = Canlib.canRead(chanHandle, out id, data, out dlc, out flag, out time))
                  == Canlib.canStatus.canOK)
          {
            DisplayMessage(id, dlc, data, flag, time);
          }

          if (status != Canlib.canStatus.canERR_NOMSG)
          {
            // an error communicating with the hardware detected so shutdown
            Canlib.canBusOff(chanHandle);
            Canlib.canClose(chanHandle);
            DisplayError(status, "canRead");
          }
        }

        if (Console.KeyAvailable == true)
        {
          ConsoleKeyInfo cki = new ConsoleKeyInfo();
          cki = Console.ReadKey(true);
          if (cki.Key == ConsoleKey.Escape)
            notFinished = false;
        }
      }

      status = Canlib.canBusOff(chanHandle);
      DisplayError(status, "canBusOff");

      status = Canlib.canClose(chanHandle);
      DisplayError(status, "canClose");

    }
  }
}
