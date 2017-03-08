using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows;
using System.Runtime.CompilerServices;
using canlibCLSNET;
using linlibCLSNET;

namespace LinViewer
{
   class LinManipulator
   {
      private LinViewer mainForm;
      private OutputView outForm;
      private Int32 channelNum = -1;
      private Int32 linHandle = -1;
      private UInt32 canHandle = 0;
      private Int32 nodeType = Linlib.LIN_MASTER;
      private Int32 varLength = 0;
      private Int32 enhanceCKSum = 0;
      private UInt32 bps = 20000;
      private Boolean CurOnBus = false;
      private Boolean notShutdown = true;
      private EventWaitHandle busOnEvent;
      private EventWaitHandle busOffEvent;
      private EventWaitHandle shutdownEvent;
      private EventWaitHandle adjustParamsEvent;
      private CanlibWaitHandle canEvent;
      private object winHandle;
 


      public LinManipulator(LinViewer lvForm, OutputView ovForm)
      {
         mainForm = lvForm;
         outForm = ovForm;
         busOnEvent = new EventWaitHandle(false, EventResetMode.ManualReset);
         busOffEvent = new EventWaitHandle(false, EventResetMode.ManualReset);
         shutdownEvent = new EventWaitHandle(false, EventResetMode.ManualReset);
         adjustParamsEvent = new EventWaitHandle(false, EventResetMode.AutoReset);

      } // constructor

      public void BusOn(Int32 chanNum, Int32 nType, Int32 varLen, Int32 enhCKSum, UInt32 bitsPerSec)
      {
         if (!CurOnBus)
         {
            channelNum = chanNum;
            nodeType = nType;
            varLength = varLen;
            enhanceCKSum = enhCKSum;
            bps = bitsPerSec;

            // Signal bus on event
            busOnEvent.Set();
         }
      } // BusOn

      public void BusOff()
      {
         if (CurOnBus)
         {
            // Signal bus off request
            busOffEvent.Set();
         }
      } // BusOn

      public void ShutdownNow()
      {
         shutdownEvent.Set();
         Thread.Sleep(250);
      }

      public void AdjustBusParameters(Int32 varLen, Int32 enhCKSum, UInt32 bitsPerSec)
      {
         if (CurOnBus)
         {
            varLength = varLen;
            enhanceCKSum = enhCKSum;
            bps = bitsPerSec;
            adjustParamsEvent.Set();
         }
      }

      private void DisplayError(Linlib.LinStatus status, String funcName)
      {
         System.Windows.Forms.MessageBox.Show(funcName + " failed with error code " + status,
                         funcName + " Error", 
                         System.Windows.Forms.MessageBoxButtons.OK,
                         System.Windows.Forms.MessageBoxIcon.Error);
      } // DisplayError

      private void InitChannel()
      {
         Linlib.LinStatus status;

         // try to setup the channel
         if ((linHandle = Linlib.linOpenChannel(channelNum, nodeType)) != (Int32)(Linlib.LinStatus.linOK))
         {
            status = (Linlib.LinStatus)linHandle;
            DisplayError(status, "linOpenChannel");
            return;
         }

         if ((status = Linlib.linSetBitrate(linHandle, bps)) != Linlib.LinStatus.linOK)
         {
            DisplayError(status, "linSetBitrate");
            status = Linlib.linClose(linHandle);
            return;
         }

         if ((status = Linlib.linGetCanHandle(linHandle, ref canHandle)) != Linlib.LinStatus.linOK)
         {
            DisplayError(status, "linGetCanHandle");
            status = Linlib.linClose(linHandle);
            return;
         }

         winHandle = RuntimeHelpers.GetObjectValue(new object());
         Canlib.canStatus cStatus = Canlib.canIoCtl((Int32)canHandle, 14, ref winHandle);
         if (cStatus != Canlib.canStatus.canOK)
         {
            DisplayError((Linlib.LinStatus)cStatus, "canIoCtl");
            status = Linlib.linClose(linHandle);
         }
         canEvent = new CanlibWaitHandle(RuntimeHelpers.GetObjectValue(winHandle));
         
         // bring the channel on the bus
         if ((status = Linlib.linBusOn(linHandle)) != Linlib.LinStatus.linOK)
         {
            DisplayError(status, "linBusOn");
            status = Linlib.linClose(linHandle);
            return;
         }

         if ((status = Linlib.linSetupLIN(linHandle, (UInt32)(varLength & enhanceCKSum), bps)) != 
              Linlib.LinStatus.linOK)
         {
            DisplayError(status, "linSetupLIN");
            status = Linlib.linClose(linHandle);
            return;
         }
         CurOnBus = true;
      } // InitChannel

      private void ChangeBusParameters()
      {
         Linlib.LinStatus status;
         if ((status = Linlib.linSetupLIN(linHandle, (UInt32)(varLength & enhanceCKSum), bps)) !=
              Linlib.LinStatus.linOK)
         {
            DisplayError(status, "linSetupLIN");
            status = Linlib.linClose(linHandle);
            CurOnBus = false;
            return;
         }

      } // ChangeBusParameters

      private void TurnOffChannel()
      {
         Linlib.LinStatus status;

         // bring the channel off the bus
         if ((status = Linlib.linBusOff(linHandle)) != Linlib.LinStatus.linOK)
         {
            DisplayError(status, "linBusOff");
         }

         if ((status = Linlib.linClose(linHandle)) != Linlib.LinStatus.linOK)
         {
            DisplayError(status, "linClose");
         }
         CurOnBus = false;
      } // TurnOffChannel

      public void Run()
      {
         WaitHandle[] waitHandles;
         WaitHandle[] busOffHandles = { busOnEvent, busOffEvent, shutdownEvent, adjustParamsEvent };
         WaitHandle[] busOnHandles = { busOnEvent, busOffEvent, shutdownEvent, adjustParamsEvent, 
                                       canEvent };
         Int32 waitIndex;

         waitHandles = busOffHandles;
         while (notShutdown)
         {
            waitIndex = WaitHandle.WaitAny(waitHandles, 200);

            switch (waitIndex)
            {
               case 0:                    // busOnEvent
                  InitChannel();
                  busOnEvent.Reset();
                  mainForm.Invoke(mainForm.busStatusDelegate, new object[] { CurOnBus });
                  if (CurOnBus)
                  {
                     busOnHandles[4] = canEvent;
                     waitHandles = busOnHandles;
                  }
                  break;
               case 1:                    // busOffEvent
                  TurnOffChannel();
                  busOffEvent.Reset();
                  mainForm.Invoke(mainForm.busStatusDelegate, new object[] { CurOnBus });
                  waitHandles = busOffHandles;
                  break;
               case 2:                    // shutdownEvent
                  if (CurOnBus) TurnOffChannel();
                  waitHandles = busOffHandles;
                  notShutdown = false;
                  break;
               case 3:
                  ChangeBusParameters();
                  adjustParamsEvent.Reset();
                  mainForm.Invoke(mainForm.busStatusDelegate, new object[] { CurOnBus });
                  if (!CurOnBus) waitHandles = busOffHandles;
                  break;
               case 4:
                  ProcessReceivedMessages();
                  break;
               case WaitHandle.WaitTimeout:   // timeout occurred
                  break;
               default:
                  // should never get here
                  break;
            } // waitIndex case block

         } // while not shutdown

      } // Run

      private void ProcessReceivedMessages()
      {
         UInt32 msgId = 0;
         UInt32 dlc = 0;
         Linlib.LinStatus status = Linlib.LinStatus.linOK;
         UInt32 flags = 0;
         Byte[] msgData = { 0, 0, 0, 0, 0, 0, 0, 0 };
         Linlib.LinMessageInfo msgInfo = new Linlib.LinMessageInfo();

         while ((status = Linlib.linReadMessage(linHandle, ref msgId, msgData, ref dlc, ref flags, msgInfo))
                  == Linlib.LinStatus.linOK)
         {
            // Process data
            outForm.Invoke(outForm.addMsgDelegate, new object[] {channelNum, msgId, dlc, msgData, flags, msgInfo});
         }
         if (status != Linlib.LinStatus.linERR_NOMSG)
         {
            DisplayError(status, "linReadMessage");
            TurnOffChannel();
            mainForm.Invoke(mainForm.busStatusDelegate, new object[] { CurOnBus });
         }
      } // ProcessReceivedMessages

   } // LinManipulator
} // LinViewer namespace
