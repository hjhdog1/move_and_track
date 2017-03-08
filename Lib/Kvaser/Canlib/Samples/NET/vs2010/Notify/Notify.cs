using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using canlibCLSNET;

namespace NotifyTest
{
    public partial class Notify : Form
    {
        public Notify()
        {
            InitializeComponent();
            Canlib.canInitializeLibrary();
        }

        private void DisplayError(Canlib.canStatus status, String routineName)
        {
            String errText = "";
            if (status != Canlib.canStatus.canOK)
            {
                Canlib.canGetErrorText(status, out errText);
                errText += ".\nError code = " + status.ToString() + ".";
                MessageBox.Show(errText, routineName, MessageBoxButtons.OK);
                Environment.Exit(0);
            }
        }

        private void ConfigChannel_Click(object sender, EventArgs e)
        {
            Canlib.canStatus status;

            chanHandle = Canlib.canOpenChannel(0, Canlib.canOPEN_ACCEPT_VIRTUAL);
            DisplayError((Canlib.canStatus)chanHandle, "canOpenChannel");

            status = Canlib.canSetBusParams(chanHandle, Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
            DisplayError(status, "canSetBusParams");

            status = Canlib.canSetNotify(chanHandle, Handle, Canlib.canNOTIFY_RX);
            DisplayError(status, "canSetNotify");

            status = Canlib.kvSetNotifyCallback(chanHandle, new Canlib.kvCallbackDelegate(Callback), new IntPtr(0), Canlib.canNOTIFY_RX);
            DisplayError(status, "kvSetNotifyCallback");

            UInt32 bufValue = 1;
            object buf = bufValue;
            status = Canlib.canIoCtl(chanHandle, Canlib.canIOCTL_SET_TXACK, ref buf);
            DisplayError(status, "canIoCtl");

            status = Canlib.canBusOn(chanHandle);
            DisplayError(status, "canBusOn");

        }


        private void CloseChannel_Click(object sender, EventArgs e)
        {
            Canlib.canStatus status;

            status = Canlib.canBusOff(chanHandle);
            DisplayError(status, "canBusOff");

            status = Canlib.canClose(chanHandle);
            DisplayError(status, "canClose");

        }


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
            RxMsgsTbox.AppendText(s);
        }


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

            base.WndProc(ref m);
        }

        private void Callback(int handle, IntPtr context, UInt32 notifyEvent)
        {
          // qqq
          // Do something here, but please observe that this code is run in
          // the context of a high-priority thread.
        }


        private int chanHandle;
    }
}
