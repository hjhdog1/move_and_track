using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using linlibCLSNET;

namespace LinViewer
{
   public partial class OutputView : Form
   {
      private Boolean shutDown = false;
      private LinViewer mainForm;
      public AddMsgDataDelegate addMsgDelegate;

      public delegate void AddMsgDataDelegate(Int32 chNum, UInt32 msgId, UInt32 dlc, Byte[] msgData,
                                              UInt32 flags, Linlib.LinMessageInfo msgInfo);

      // constructor
      public OutputView(LinViewer lvForm)
      {
         InitializeComponent();

         addMsgDelegate = new AddMsgDataDelegate(this.AddMessageData);
         this.FormClosing += new FormClosingEventHandler(this.FormClosingHandler);
         this.Resize += new EventHandler(this.ResizeHandler);

         HeaderLabel.Text = "Dir   Ch    Msg Id    Flags   Len  CkSum  Parity  Data: 1   2   3   4   5   6   7   8     Time";

         mainForm = lvForm;
      } // OutputView constructor
      
      // Allows the thread to place data in the RichEditBox control.
      private void AddMessageData(Int32 chNum, UInt32 msgId, UInt32 dlc, Byte [] msgData,
                                  UInt32 flags, Linlib.LinMessageInfo msgInfo)
      {
         if ((flags & Linlib.LIN_RX) != 0)
         {
            OutputRTB.AppendText("  RX   " + chNum.ToString("00") + "   ");
         }
         else
         {
            OutputRTB.AppendText("  TX   " + chNum.ToString("00") + "   ");
         }
         OutputRTB.AppendText(msgId.ToString("X8") + "   ");

         OutputRTB.AppendText(
            (((flags & Linlib.LIN_BIT_ERROR) != 0) ? "B" : " ") +
            (((flags & Linlib.LIN_SYNCH_ERROR) != 0) ? "S" : " ") +
            (((flags & Linlib.LIN_PARITY_ERROR) != 0) ? "P" : " ") +
            (((flags & Linlib.LIN_CSUM_ERROR) != 0) ? "C" : " ") +
            (((flags & Linlib.LIN_NODATA) != 0) ? "H" : " ") +
            (((flags & Linlib.LIN_WAKEUP_FRAME) != 0) ? "W" : " ") + "   ");

         OutputRTB.AppendText(dlc.ToString("D") + "     " +
                              msgInfo.checkSum.ToString("X2") + "     " + 
                              msgInfo.idPar.ToString("X2") + 
                              "         ");
         for (int i = 0; i < 8; i++)
         {
            if (i < dlc)
            {
               OutputRTB.AppendText(msgData[i].ToString("X2") + "  ");
            }
            else
            {
               OutputRTB.AppendText("    ");
            }
         }
         // time stamp
         double modifiedTime;
         modifiedTime = ((double)(msgInfo.timestamp)) / 1000.0;
         OutputRTB.AppendText(modifiedTime.ToString(" " + "000000.000") + "\n");

         // Need scroll bar to move down as data is added.
         // qqq - Something needs to be fixed here so scroll bar prevents this call.
         OutputRTB.ScrollToCaret();
      } // AddMessageData

      // Allows closing the main form to properly close this form.
      public void ShutdownNow()
      {
         shutDown = true;
      } // ShutdownNow

      // Makes sure that the form close button does not actually close the form.
      private void FormClosingHandler(object sender, CancelEventArgs e)
      {
         if (!shutDown)
         {
            e.Cancel = true;
            Hide();
            mainForm.Invoke(mainForm.ovStatusDelegate, new object[] { false });
         }
      } // FormClosingHandler

      // Resize event handler that ensures the window is hidden.  You can only restore
      // through main window menu view options.
      private void ResizeHandler(object sender, System.EventArgs e)
      {
         // just update the view info if the minimize button was pressed.
         if (this.WindowState == FormWindowState.Minimized)
         {
            Hide();
            mainForm.Invoke(mainForm.ovStatusDelegate, new object[] { false });
         }
      }

      // Event handler for the RichTextBox control's Context menu Clear menu item
      private void ClearAll_Click(object sender, EventArgs e)
      {
         OutputRTB.Clear();
      }

      // Event handler for the RichTextBox control's Context menu Copy to clipboard menu item.
      private void CopyAllToClipBoard_Click(object sender, EventArgs e)
      {
         OutputRTB.SelectAll();
         OutputRTB.Copy();
         OutputRTB.Select(0, 0);
      } // ResizeHandler

   } // OutputView class

} // LinViewer namespace
