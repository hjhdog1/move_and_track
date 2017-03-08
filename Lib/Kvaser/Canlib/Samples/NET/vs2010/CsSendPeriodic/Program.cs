using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using canlibCLSNET;
using MMTimers;

namespace CsSendPeriodic
{
   class Program
   {
      // used in all methods
      static int txMethod = 1;
      static int rxChanHndl = -1;
      static int txChanHndl = -1;
      static int msgIdx1 = -1;
      static int msg1Id = 0x200;
      static byte[] msg1Data = new byte[8] {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
      static int msg1Dlc = 8;
      static int msg1Flags = Canlib.canMSG_EXT;
      static int msgIdx2 = -1;
      static int msg2Id = 0x250;
      static byte[] msg2Data = new byte[8] {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x080};
      static int msg2Dlc = 8;
      static int msg2Flags = Canlib.canMSG_EXT;

      // Used in Method 2
      static ManualResetEvent shutdown;
      static Thread txThread;

      // used in Method 3
      static MMTimer timer1;
      static MMTimer timer2;
      static int txChanHndl2 = -1;

      /*
      **  Print out how to use this application
      */
      static void Usage()
      {
         Console.WriteLine("");
         Console.WriteLine("Send Periodic Program");
         Console.WriteLine("");
         Console.WriteLine("This sample program demonstrates three methods of transmitting periodic");
         Console.WriteLine("messages using the Kvaser Canlib API.  The user may select the channel");
         Console.WriteLine("to be used and the transmit method demonstrated.  The three methods that");
         Console.WriteLine("can be demonstrated are Canlib Object Buffers, periodic thread, and");
         Console.WriteLine("multimedia timer.");
         Console.WriteLine("");
         Console.WriteLine("Usage: sendperiodic [flags]");
         Console.WriteLine("   -O<value>   Use CAN channel number <value> to transmit.");
         Console.WriteLine("   -R<value>   Use CAN channel number <value> to receive.");
         Console.WriteLine("   -T<value>   Transmission method is <value>.");
         Console.WriteLine("               Default method is 1.");
         Console.WriteLine("               1 = Canlib Object Buffers");
         Console.WriteLine("               2 = periodic thread");
         Console.WriteLine("               3 = multimedia timer");
         Console.WriteLine("   -m          Set multimedia timer precision.");
         Console.WriteLine("               This can be used in combination with method 2.");
         Console.WriteLine("   -h          Print this help text.");
         Console.WriteLine("");
         Console.WriteLine("Example:");
         Console.WriteLine("sendperiodic -O0 -R1 -T1");
         Console.WriteLine("   transmissions will be on channel 0 using Canlib Object Buffers");

         Environment.Exit(1);
      }  // end of Usage()

      /*
      ** Check a status code and issue an error message if the code isn't canOK.
      */
      static void ErrorDump(string id, Canlib.canStatus stat, bool quit)
      {
         string buf = "";
         if (stat != Canlib.canStatus.canOK) {
            Canlib.canGetErrorText(stat, out buf);
            Console.WriteLine("{0}: failed, stat={1} ({2})", id, (int)stat, buf);
            Thread.Sleep(5000);
		      if (quit) Environment.Exit(1);
         }
      }  // end of ErrorDump()

      /*
      **  Prepare channel regardless of method
      */
      static void InitChannel(int chNum, ref int chHndl)
      {
         Canlib.canStatus status;

         // Because we are working on timing we want the channel exclusive
         chHndl = Canlib.canOpenChannel(chNum, Canlib.canOPEN_ACCEPT_VIRTUAL);
         if (chHndl < 0)
            ErrorDump("canOpenChannel", (Canlib.canStatus)(chHndl), true);

         // decided to use the default 250 kBits/sec
         status = Canlib.canSetBusParams(chHndl, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
         ErrorDump("canSetBusParams", status, true);
      }  // end of InitChannel()
      
      /*
      **  Shuts down an open channel
      */
      static void CloseChannel(int chHndl)
      {
         Canlib.canStatus status;

         // take the channel offline
         status = Canlib.canBusOff(chHndl);
         ErrorDump("canBusOff", status, false);

         // free the channel handle
         status = Canlib.canClose(chHndl);
         ErrorDump("canClose", status, false);
      }  // end of CloseChannel()

      /*
      **  This routine completes the configuration necessary to perform
      **  transmit method 1.  This method uses the Canlib Object Buffers.
      **  This will include the configuration of two periodic messages.
      **  One of the messages will be sent every 1 ms.  The second
      **  message will be sent every 10 ms.
      */
      static void TranmitMethod1(int channel)
      {
	      Canlib.canStatus status;

	      // get basic setup done.
	      InitChannel(channel, ref txChanHndl);

	      // get a buffer to configure msg 1 transmission
	      msgIdx1 = (int)Canlib.canObjBufAllocate(txChanHndl, Canlib.canOBJBUF_TYPE_PERIODIC_TX);
	      if (msgIdx1 < 0)
		      ErrorDump("canObjBufAllocate", (Canlib.canStatus)msgIdx1, true);

	      /* configure the buffer to transmit every 1 ms
	      ** (period is defined in micro seconds)
	      */
	      status = Canlib.canObjBufSetPeriod(txChanHndl, msgIdx1, 1000);
	      ErrorDump("canObjBufSetPeriod", status, true);

	      // establish the data to be transmitted
	      status = Canlib.canObjBufWrite(txChanHndl, msgIdx1, msg1Id, msg1Data,
                                        msg1Dlc, msg1Flags);
	      ErrorDump("canObjBufWrite", status, true);


	      // get a buffer to configure msg 2 transmission
	      msgIdx2 = (int)Canlib.canObjBufAllocate(txChanHndl, Canlib.canOBJBUF_TYPE_PERIODIC_TX);
	      if (msgIdx2 < 0)
		      ErrorDump("canObjBufAllocate", (Canlib.canStatus)msgIdx2, true);

	      /* configure the buffer to transmit every 10 ms 
	      ** (period is defined in micro seconds)
	      */
	      status = Canlib.canObjBufSetPeriod(txChanHndl, msgIdx2, 10000);
	      ErrorDump("canObjBufSetPeriod", status, true);

	      // establish the data to be transmitted
	      status = Canlib.canObjBufWrite(txChanHndl, msgIdx2, msg2Id, msg2Data,
                                        msg2Dlc, msg2Flags);
	      ErrorDump("canObjBufWrite", status, true);


	      // Go on bus so messages can be transmitted
	      status = Canlib.canBusOn(txChanHndl);
	      ErrorDump("canBusOn", status, true);

	      // allow the buffer to transmit
	      status = Canlib.canObjBufEnable(txChanHndl, msgIdx1);
	      ErrorDump("canObjBufEnable", status, true);

	      status = Canlib.canObjBufEnable(txChanHndl, msgIdx2);
	      ErrorDump("canObjBufEnable", status, true);
      }  // end of TransmitMethod1()

      /*
      ** if the user changes the desired buffer content,
      ** this routine is called to change the content to
      ** the updated values.
      */
      static void UpdateObjBuffer()
      {
	      Canlib.canStatus status;

	      // establish the data to be transmitted
	      status = Canlib.canObjBufWrite(txChanHndl, msgIdx1, msg1Id, msg1Data,
                                        msg1Dlc, msg1Flags);
	      ErrorDump("canObjBufWrite", status, true);
      }  // end of UpdateObjBuffer()
      
      //
      //  Do a proper shutdown for transmission method 1
      //
      static void CloseMethod1()
      {
	      Canlib.canStatus status;

	      // Stops the buffer from transmitting
	      status = Canlib.canObjBufDisable(txChanHndl, msgIdx1);
	      ErrorDump("canObjBufDisable", status, false);

	      // Free the buffers
	      status = Canlib.canObjBufFreeAll(txChanHndl);
	      ErrorDump("canObjBufFreeAll", status, false);

	      // close the transmit channel
	      CloseChannel(txChanHndl);
      }  // end of CloseMethod1()

      /*
      **  Transmit thread launched when running transmit method 2.  This
      **  thread gets a handle to the channel then waits for 1 millisecond
      **  for a shutdown event.  If that event does not occur, then the 
      **  thread transmits the first message and increments a counter.
      **  When the counter indicates that 10 interations have passed (10
      **  milliseconds), the second messages is also transmitted.
      **  The accuracy of the timing of transmissions is purely in the hands
      **  of the Window's scheduler.  Therefore, the timing will not be
      **  accurate.
      */
      static void Transmitter2(object chNum)
      {
	      Canlib.canStatus status;
	      int count = 0;
	      bool done = false;
	      int channel = (int)chNum;

	      // get basic setup done.
	      InitChannel(channel, ref txChanHndl);

         // Make the channel handle active on the bus
	      if ((status = Canlib.canBusOn(txChanHndl)) == Canlib.canStatus.canOK) {

		      // loop until an error occurs or the user chooses to stop
		      while (!done) {
			      // check for the user shutdown event
			      if (!shutdown.WaitOne(1)) {

				      // transmit the first message
				      status = Canlib.canWrite(txChanHndl, msg1Id, msg1Data, msg1Dlc, msg1Flags);
				      if (status != Canlib.canStatus.canOK) {
					      // something went wrong with the transmit so abort
					      ErrorDump("canWrite", status, false);
					      done = true;
				      }

				      // increment the 10 millisecond counter
				      count++;

				      // check if 10 milliseconds have passed
				      if (count >= 10) {

					      // reset the counter
					      count = 0;

					      // transmit the second message
					      status = Canlib.canWrite(txChanHndl, msg2Id, msg2Data, msg2Dlc, msg2Flags);
					      if (status != Canlib.canStatus.canOK) {
						      // something went wrong with the transmit so abort
						      ErrorDump("canWrite", status, false);
						      done = true;
					      }
				      } // end of count large enough for second message transmit
			      }  // end of timeout met without shutdown.
			      else {
				      // the user shutdown event has occurred so exit
				      done = true;
			      }
		      } // end of while loop
	      }
	      else {
		      ErrorDump("canBusOn", status, false);
	      }

	      // close the transmit channel
	      CloseChannel(txChanHndl);

      }  // end of Transmitter2()

      /*
      **  This routine completes the configuration necessary to perform
      **  transmit method 2.  This method uses the periodic thread.
      **  This will include the configuration of a periodic thread that
      **  transmits two periodic messages.  One of the messages will be 
      **  sent every 1 ms.  The second message will be sent every 10 ms.
      */
      static void TranmitMethod2(int channel)
      {
	      // create a shutdown event
	      shutdown = new ManualResetEvent(false);

	      // create a thread - cheat on passing the channel number and pass as a value hidden in a pointer
	      txThread = new Thread(Transmitter2);
 
         txThread.Priority = ThreadPriority.Highest;

	      // start the thread
	      txThread.Start(channel);
      }  // end of TransmitMethod2()

      /*
      **  Do a proper shutdown for transmission method 2
      */
      static void CloseMethod2()
      {
	      // try to stop the thread
	      if (shutdown.Set()) {
		      // wait for the thread to terminate itself
		      Thread.Sleep(2000);
	      }
      }  // end of CloseMethod2()
 
      // The routine called by the multimedia timer 1 event created in transmission method 3.
      static void Timer1Handler(Object sender, EventArgs e)
      {
	      Canlib.canStatus status;

	      // transmit the first message
	      status = Canlib.canWrite(txChanHndl, msg1Id, msg1Data, msg1Dlc, msg1Flags);
	      if (status != Canlib.canStatus.canOK) {
		      // something went wrong with the transmit so abort
		      ErrorDump("canWrite", status, false);
	      }
      } // end of multimedia timer 1 callback


      // The routine called by the multimedia timer 2 event created in transmission method 3.
      static void Timer2Handler(Object sender, EventArgs e)
      {
	      Canlib.canStatus status;

	      // transmit the first message
	      status = Canlib.canWrite(txChanHndl2, msg2Id, msg2Data, msg2Dlc, msg2Flags);
	      if (status != Canlib.canStatus.canOK) {
		      // something went wrong with the transmit so abort
		      ErrorDump("canWrite", status, false);
	      }
      } // end of multimedia timer 2 callback

      /*
      **  This routine completes the configuration necessary to perform
      **  transmit method 3.  This method uses a multimedia timer.
      */
      static void TranmitMethod3(int channel)
      {
	      Canlib.canStatus status;

	      // get basic setup done.
	      InitChannel(channel, ref txChanHndl);
	      InitChannel(channel, ref txChanHndl2);

	      /*                       ****** WARNING ******
	      ** You must not use txChanHndl anywhere outside of the multimedia timer
	      ** handler once you start the timer.  Windows create a thread for the 
	      ** timer and the callback is executed inside that thread.  You must not
	      ** violate the Kvaser rule of one handle per thread.  We are able to
	      ** bend the rule by handing off the handle, but if we continue to use
	      ** the handle in this thread we will break the rule and possibly have
	      ** erratic runtime errors.
	      */
	      // have to go on bus before the periodic timer starts.
	      status = Canlib.canBusOn(txChanHndl);
	      ErrorDump("canBusOn", status, true);
	      status = Canlib.canBusOn(txChanHndl2);
	      ErrorDump("canBusOn", status, true);
         
	      // create a multimedia timer and attach to a callback
	      timer1 = new MMTimer();
         timer1.Timer += new EventHandler(Timer1Handler);
	      timer2 = new MMTimer();
         timer2.Timer += new EventHandler(Timer2Handler);
         timer1.Start(1, true);
         timer2.Start(10, true);
      }

      //  Do a proper shutdown for transmission method 3
      static void CloseMethod3()
      {
	      // try to kill the periodic multimedia timer
	      timer1.Stop();
	      timer2.Stop();
      	
	      // close the transmit channel
	      CloseChannel(txChanHndl);
	      CloseChannel(txChanHndl2);
      }
     
      //
      //  Print incoming data as it comes in and watch for key presses.
      //  Print the difference between in coming message timestamps also.
      //
      static void ProcessIncoming()
      {
         bool finished = false;
         Canlib.canStatus status;
         int msgId;
         byte[] data = new byte[8] {0, 0, 0, 0, 0, 0, 0, 0};
         int dlc;
         int flags;
         long time;
         long lastTime1 = 0;
         long lastTime2 = 0;

	      /* print a header for the output data.  It is important to notice that the
	      ** time placed in the final column is the time since the last reception of the
	      ** same message.  This timestamp is in 1/10 of a millisecond resolution.
	      */
	      Console.Write("   ID    DLC DATA                      Time since last msg (1/10 msec)");

	      // loop until we need to exit.
         while (!finished) {

		      // Get the next message in the queue or wait for one
		      if ((status = Canlib.canReadWait(rxChanHndl, out msgId, data, out dlc, out flags,
                                             out time, 10)) == Canlib.canStatus.canOK) {
               // check for error frame
			      if ((flags & Canlib.canMSG_ERROR_FRAME) != 0) {
				      Console.Write("Error Frame received ****");
			      }
               // real message so dump to the screen
			      else {
				      if (msgId == msg1Id) {
                     Console.WriteLine("{0:x8}  {1}  {2:x2} {3:x2} {4:x2} {5:x2} {6:x2} {7:x2} {8:x2} {9:x2}   {10}",
						                     msgId, dlc, data[0], data[1], data[2], data[3], data[4],
						                     data[5], data[6], data[7], time - lastTime1);
					      lastTime1 = time;
				      }
				      else if (msgId == msg2Id) {
                     Console.WriteLine("{0:x8}  {1}  {2:x2} {3:x2} {4:x2} {5:x2} {6:x2} {7:x2} {8:x2} {9:x2}   {10}",
                                       msgId, dlc, data[0], data[1], data[2], data[3], data[4],
						                     data[5], data[6], data[7], time - lastTime2);
					      lastTime2 = time;
				      }
			      }  // end of else it was a real message
		      }  // end of canReadWait returned with a message.
            // a message was not received so make sure an error has not occurred.
		      else if (status != Canlib.canStatus.canERR_NOMSG) {
			      // if we received some other error then exit
			      finished = true;
			      ErrorDump("canReadWait", status, false);
		      }
              
		      // check if the user has pressed a key
		      if (Console.KeyAvailable) {
			      // React to whatever the user's input
		         switch (Console.ReadKey(true).Key) {

                  // Escape key to exit program
			         case ConsoleKey.Escape:
				         finished = true;
					      break;

				      // Increment the first byte of data in message 1
				      case ConsoleKey.OemPlus:
					      msg1Data[0] += 1;
					      if (txMethod == 1)
						      UpdateObjBuffer();
					      break;
			      }  //end of switch on Key
		      }  // end of if KeyAvailable
	      }  // end of finished loop

      }  // end of ProcessIncoming


      static void Main(string[] args)
      {
         Canlib.canStatus status;
         int txChannel = 0;
         int rxChannel = 1;
         bool mmPrecision = false;

         /* Get the channel and transmit method from the command line or display
         ** help on how to use this program.
         */
         foreach (string s in args)
         {
             // check for channel to be used for transmitting
             if (s.StartsWith("-O"))
             {
                txChannel = Convert.ToInt32(s.Remove(0, 2));
             }
             // check for channel to be used for transmitting
             else if (s.StartsWith("-R"))
             {
                rxChannel = Convert.ToInt32(s.Remove(0, 2));
             }
             // check for tranmit method to be used
             else if (s.StartsWith("-T"))
             {
                txMethod = Convert.ToInt32(s.Remove(0, 2));
             }
             else if (s.Equals("-m"))
             {
                 mmPrecision = true;
             }
             else if (s.Equals("-h"))
             {
                 Usage();
             }
             else
             {
                 Usage();
             }
         }  // end of foreach command line argument

         // do one final sanity check on the command line options
         if ((txChannel == rxChannel) || (txChannel < 0) || (rxChannel < 0) ||
             (txMethod < 0) || (txMethod > 3)) {
            Usage();
         }
         
         // set the multimedia timer precision
         if (mmPrecision || (txMethod == 3))
         {
            // Lets try setting the multimedia timer precision to see if it boost performance.
            // Try setting to
            if (MMTimer.timeBeginPeriod(1) != 0)
            {
               Console.WriteLine("Failed to set the multimedia timer resolution.");
               Environment.Exit(1);
            }
         }

         // prepare the library
         Canlib.canInitializeLibrary();

         // open the receive channel
         InitChannel(rxChannel, ref rxChanHndl);

         /* for the receive channel we want to up the resolution returned on
         ** on the time stamp to 1/10 of a millisecond.  This changes the resolution,
         ** but the accuracy still depends on the Kvaser hardware used.
         */
         object timeObj = (UInt32)100;
         status = Canlib.canIoCtl(rxChanHndl, Canlib.canIOCTL_SET_TIMER_SCALE, ref timeObj);
         ErrorDump("canIoCtl", status, true);

         // go ahead and start receiving messages into the buffer
         status = Canlib.canBusOn(rxChanHndl);
         ErrorDump("canBusOn", status, true);

         // transmission method changes based on command line input
         switch (txMethod)
         {
             case 1:
                 TranmitMethod1(txChannel);
                 break;
             case 2:
                 TranmitMethod2(txChannel);
                 break;
             case 3:
                 TranmitMethod3(txChannel);
                 break;
             default:
                 Console.WriteLine("Invalid transmission method selected");
                 Environment.Exit(1);
                 break;
         }

         // loop reading the CAN bus and checking for keyboard entry
         ProcessIncoming();

         // transmission method changes based on command line input
         switch (txMethod)
         {
             case 1:
                 CloseMethod1();
                 break;
             case 2:
                 CloseMethod2();
                 break;
             case 3:
                 CloseMethod3();
                 break;
             default:
                 break;
         }

         // Release the channel
         CloseChannel(rxChanHndl);

      } // end of Main()
   }  // end of class Program 
}  // end of namespace CsSendPeriodic
