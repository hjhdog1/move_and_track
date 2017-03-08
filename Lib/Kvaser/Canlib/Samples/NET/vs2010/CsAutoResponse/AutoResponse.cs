using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using canlibCLSNET;


namespace CsAutoResponse
{
   public partial class AutoResponse : Form
   {
      public AutoResponse()
      {
         InitializeComponent();

         // initialize the API library
         Canlib.canInitializeLibrary();

         // create the response message data bytes storage
         obData = new byte[8] { 0, 0, 0, 0, 0, 0, 0, 0 };
      }

      private void DisplayError(Canlib.canStatus status, String routineName, Boolean exitApp)
      {
         String errText = "";
         if (status != Canlib.canStatus.canOK)
         {
            Canlib.canGetErrorText(status, out errText);
            errText += ".\nError code = " + status.ToString() + ".";
            MessageBox.Show(errText, routineName, MessageBoxButtons.OK);

            // Only exit when signaled
            if (exitApp)
            {
               Environment.Exit(0);
            }
         }
      }

      private void GetResponseMessage()
      {
         obId = IdentifierMTB.TranslatedHexValue();
         if (ExtendedIdCB.Checked)
         {
            obFlag = Canlib.canMSG_EXT;
         }
         else
         {
            obFlag = Canlib.canMSG_STD;
         }
         obDlc = DlcMTB.TranslatedHexValue();
         obData[0] = (byte)Data0MTB.TranslatedHexValue();
         obData[1] = (byte)Data1MTB.TranslatedHexValue();
         obData[2] = (byte)Data2MTB.TranslatedHexValue();
         obData[3] = (byte)Data3MTB.TranslatedHexValue();
         obData[4] = (byte)Data4MTB.TranslatedHexValue();
         obData[5] = (byte)Data5MTB.TranslatedHexValue();
         obData[6] = (byte)Data6MTB.TranslatedHexValue();
         obData[7] = (byte)Data7MTB.TranslatedHexValue();
      }  // GetResponseMessage

      private void InitChannelB_Click(object sender, EventArgs e)
      {
         Canlib.canStatus status;
         Int32 channelNumber;

         // get the channel number from user
         if (!(Int32.TryParse(ChannelNumMTB.Text, out channelNumber))) {
            channelNumber = 0;
            ChannelNumMTB.Text = " 0";
         }

         // try to open the channel
         chHandle = Canlib.canOpenChannel(channelNumber, Canlib.canOPEN_ACCEPT_VIRTUAL);
         DisplayError((Canlib.canStatus)chHandle, "canOpenChannel", true);

         // setup the bit rate
         status = Canlib.canSetBusParams(chHandle, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
         DisplayError(status, "canSetBusParams", true);

         // Setup notification so we can trigger WndProc to empty the receive buffer
         status = Canlib.canSetNotify(chHandle, Handle, Canlib.canNOTIFY_RX);
         DisplayError(status, "canSetNotify", true);

      }

      private void CreateB_Click(object sender, EventArgs e)
      {
         Canlib.canStatus status;

         // only allow one object buffer to be created in this example
         if (obExists)
         {
            MessageBox.Show("This example only allows one auto response buffer to be created",
                            "Example Limitation", MessageBoxButtons.OK);
         }
         else
         {
            // get an object buffer of the correct type
            obHandle = (Int32)Canlib.canObjBufAllocate(chHandle, Canlib.canOBJBUF_TYPE_AUTO_RESPONSE);
            DisplayError((Canlib.canStatus)obHandle, "canObjBufAllocate", false);

            if (chHandle >= 0)
            {
               // set the filter
               status = Canlib.canObjBufSetFilter(chHandle, obHandle,
                                                  MFCodeMTB.TranslatedHexValue(),
                                                  MFMaskMTB.TranslatedHexValue());
               DisplayError(status, "canObjBufSetFilter", false);

               // if selected, set to only reply to RTR messages
               if (RTROnlyCB.Checked)
               {
                  status = Canlib.canObjBufSetFlags(chHandle, obHandle,
                                                    Canlib.canOBJBUF_AUTO_RESPONSE_RTR_ONLY);
                  DisplayError(status, "canObjBufSetFlags", false);
               }

               // load the message content
               GetResponseMessage();
               status = Canlib.canObjBufWrite(chHandle, obHandle, obId, obData, obDlc, obFlag);
               DisplayError(status, "canObjBufWrite", false);

               obExists = true;
            }
         }
      }

      private void UpdateContentsB_Click(object sender, EventArgs e)
      {
         Canlib.canStatus status;

         // Only update the contents if the buffer exists
         if (obExists)
         {
            // get the latest message content
            GetResponseMessage();

            // place it in the buffer
            status = Canlib.canObjBufWrite(chHandle, obHandle, obId, obData, obDlc, obFlag);
            DisplayError(status, "canObjBufWrite", false);
         }
      }  // UpdateContentsB_Click

      private void RemoveB_Click(object sender, EventArgs e)
      {
         Canlib.canStatus status;

         if (obExists)
         {
            // disable the buffer
            if (obEnabled)
            {
               status = Canlib.canObjBufDisable(chHandle, obHandle);
               DisplayError(status, "canObjBufDisable", false);
               obEnabled = false;
            }

            // free the auto response object buffer
            status = Canlib.canObjBufFree(chHandle, obHandle);
            DisplayError(status, "canObjBufFree", false);
            obExists = false;
         }
      }

      private void BusOnB_Click(object sender, EventArgs e)
      {
         Canlib.canStatus status;

         if ((chHandle >= 0) && (!onLine))
         {
            // make the channel active on the bus
            status = Canlib.canBusOn(chHandle);
            DisplayError(status, "canBusOn", true);
            onLine = true;

            // Turn the auto response buffer on.
            if (obExists && !obEnabled)
            {
               status = Canlib.canObjBufEnable(chHandle, obHandle);
               DisplayError(status, "canObjBufEnable", false);
               obEnabled = true;
            }
         }
      }  // BusOnB_Click

      private void BusOffB_Click(object sender, EventArgs e)
      {
         Canlib.canStatus status;

         if (onLine)
         {
            // turn the auto response buffer off
            if (obEnabled)
            {
               status = Canlib.canObjBufDisable(chHandle, obHandle);
               DisplayError(status, "canObjBufDisable", false);
               obEnabled = false;
            }

            // make the channel inactive on the bus
            status = Canlib.canBusOff(chHandle);
            DisplayError(status, "canBusOff", false);
            onLine = false;
         }
      }  // BusOffB_Click

      private void CloseChannelB_Click(object sender, EventArgs e)
      {
         Canlib.canStatus status;

         if (onLine)
         {
            // make the channel inactive on the bus
            status = Canlib.canBusOff(chHandle);
            DisplayError(status, "canBusOff", false);
            onLine = false;
         }

         // our bus off routine should handle disabling the object buffer
         // but we still need to free the object buffer
         if (obExists)
         {
            status = Canlib.canObjBufFreeAll(chHandle);
            DisplayError(status, "canObjBufFreeAll", false);
            obExists = false;
         }

         // free the handle
         status = Canlib.canClose(chHandle);
         DisplayError(status, "canClose", false);
      }  // CloseChannelB_Click

      private void DisplayMessage(int id, int dlc, byte[] data, int flags, long time)
      {
         String s;
         if ((flags & Canlib.canMSG_ERROR_FRAME) == Canlib.canMSG_ERROR_FRAME)
            s = String.Format("ErrorFrame                                          {0}", time) + Environment.NewLine;
         else
         {
            s = String.Format("{0:x8} ", id);
            if ((flags & Canlib.canMSG_EXT) == Canlib.canMSG_EXT)
               s += "X";
            else
               s += " ";
            if ((flags & Canlib.canMSG_RTR) == Canlib.canMSG_RTR)
               s += "R";
            else
               s += " ";
            if ((flags & Canlib.canMSG_TXACK) == Canlib.canMSG_TXACK)
               s += "A";
            else
               s += " ";
            if ((flags & Canlib.canMSG_WAKEUP) == Canlib.canMSG_WAKEUP)
               s += "W";
            else
               s += " ";
            s += String.Format("  {0:x1} ", dlc);
            for (int i = 0; i < 8; i++)
            {
               if (i < dlc)
                  s += String.Format("  {0:x2}", data[i]);
               else
                  s += "    ";
            }
            s += String.Format("   {0}", time) + Environment.NewLine;
         }
         OutputRTB.AppendText(s);
      }  // DisplayMessage

      protected override void WndProc(ref Message m)
      {
         Canlib.canStatus status;

         if (m.Msg == Canlib.WM__CANLIB)
         {
            // You can check m.LParam low word for the notification type

            // regardless of notification type you must empty the receive buffer
            int id = 0;
            byte[] data = new byte[8] { 0, 0, 0, 0, 0, 0, 0, 0 };
            int dlc = 0;
            int flag = 0;
            long time = 0;

            // empty the receive buffer before waiting again
            while ((status = Canlib.canRead(chHandle, out id, data, out dlc, out flag, out time))
                    == Canlib.canStatus.canOK)
            {
               DisplayMessage(id, dlc, data, flag, time);
            }

            if (status != Canlib.canStatus.canERR_NOMSG)
            {
               // an error communicating with the hardware detected so shutdown
               onLine = false;
               Canlib.canBusOff(chHandle);
               Canlib.canClose(chHandle);
               chHandle = -1;
               DisplayError(status, "canRead", true);
            }

         }

         base.WndProc(ref m);
      }  // WndProc
 
      private Int32 chHandle = -1;
      private Int32 obHandle = -1;
      private Boolean onLine = false;
      private Boolean obExists = false;
      private Boolean obEnabled = false;
      private Int32 obId;
      private Int32 obDlc;
      private Int32 obFlag;
      private byte[] obData;

   }  // end of AutoResponse Class
}  // end of CsAutoResponse namespace
