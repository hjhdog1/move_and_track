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
   public partial class LinViewer : Form
   {
      private Int32 nodeType = Linlib.LIN_MASTER;
      private Int32 varLength = 0;
      private Int32 enhanceCKSum = 0;
      private UInt32 bps = 20000;
      private Boolean CurOnBus = false;
      private Int32 channelNumber = 0;
      private Boolean wasMinimized = false;
      private OutputView outForm;
      private LinManipulator linWorker;
      private System.Threading.Thread myThread;
      public FixBusStatus busStatusDelegate;
      public OutputViewStatus ovStatusDelegate;

      // delegate prototype
      public delegate void FixBusStatus(Boolean curStatus);
      public delegate void OutputViewStatus(Boolean Visible);

      // Constructor
      public LinViewer()
      {
         InitializeComponent();

         // init event handlers
         this.ChannelTabCntl.Click += new System.EventHandler(this.TabCntl_Click);
         this.FormClosing += new FormClosingEventHandler(this.FormClosingHandler);
         this.Resize += new EventHandler(this.ResizeHandler);

         // initialize controls on form
         BusOnLED.Checked = false;
         BusOffLED.Checked = true;
         UpdateChanSettingGroup();

         ChannelTBox.Text = channelNumber.ToString();
         BpsTBox.Text = bps.ToString();
         GBMaster.Checked = (nodeType == Linlib.LIN_MASTER);
         GBSlave.Checked = (nodeType == Linlib.LIN_SLAVE);
         EnhancedCKsumCbox.Checked = (enhanceCKSum == Linlib.LIN_ENHANCED_CHECKSUM);
         VarDlcCBox.Checked = (varLength == Linlib.LIN_VARIABLE_DLC);

         // setup delegates
         busStatusDelegate = new FixBusStatus(this.UpdateBusStatus);
         ovStatusDelegate = new OutputViewStatus(this.FixOutputViewStatus);

         // prepare linlib for use
         Linlib.linInitializeLibrary();

         // create the output display
         outForm = new OutputView(this);
         outForm.Show();
         Point startLocation = new Point(100, 100);
         Location = startLocation;
         startLocation.X = 450;
         outForm.Location = startLocation;
         
         // Chreate the thread
         linWorker = new LinManipulator(this, outForm);
         myThread = new System.Threading.Thread(new System.Threading.ThreadStart(linWorker.Run));
         myThread.Start();

      } // LinViewer constructor

      private void DisplayError(String title, String msgText)
      {
         MessageBox.Show(msgText, title, MessageBoxButtons.OK,
                         MessageBoxIcon.Error);
      } // DisplayError

      // A single routine for updating the desired current bus configuration on the Config tab.
      private void UpdateChanSettingGroup()
      {
         ChanNumLabel.Text = channelNumber.ToString();
         if (nodeType == Linlib.LIN_MASTER) {
            NodeTypeLabel.Text = "Master";
         } else {
            NodeTypeLabel.Text = "Slave";
         }
         BpsLabel.Text = bps.ToString();
         OptionsLabel.Text = ((enhanceCKSum == Linlib.LIN_ENHANCED_CHECKSUM) ? ("Enhanced Checksum\n") : "Standard Checksum\n") +
                             ((varLength == Linlib.LIN_VARIABLE_DLC) ? ("Variable DLC") : "");
      } // UpdateChanSettingGroup

      // Signals the thread to become active on the bus when the Bus On button is pressed.
      private void GoBusOn_Click(object sender, EventArgs e)
      {
         // don't allow the bus on to be called if the bus is already on
         if (!CurOnBus)
            linWorker.BusOn(channelNumber, nodeType, varLength, enhanceCKSum, bps);

      } // GoBusOn_Click

      // Allows the worker thread to update this form's bus status based on actual calls
      // to the API.
      private void UpdateBusStatus(Boolean newStatus)
      {
         CurOnBus = newStatus;
         BusOffLED.Checked = !CurOnBus;
         BusOnLED.Checked = CurOnBus;
      } // UpdateBusStatus

      // Allows the Output Window Form to update the menu state when that form's minimize
      // button is used.
      private void FixOutputViewStatus(Boolean visible)
      {
         outputWindowToolStripMenuItem.Checked = visible;
      } // FixOutputViewStatus

      // Signals the thread to become inactive on the bus when the Bus Off button is pressed.
      private void GoBusOff_Click(object sender, EventArgs e)
      {
         // don't do anything if you are already off bus
         if (CurOnBus) linWorker.BusOff();

      } // GoBusOff_Click

      // Handles the Accept button press on the Config tab.
      private void ConfigApply_Click(object sender, EventArgs e)
      {
         Int32 tmpChNum = Convert.ToInt32(ChannelTBox.Text);
         UInt32 tmpBps = Convert.ToUInt32(BpsTBox.Text);

         if ((tmpBps < 1000) || (tmpBps > 20000))
         {
            DisplayError("BPS Out of Range", "Bit rate entered is not within allowed range.  " +
                         "Enter a value in the range of 1000 to 20000.");
            return;
         }
         if (CurOnBus)
         {
            bps = Convert.ToUInt32(BpsTBox.Text);
            enhanceCKSum = (EnhancedCKsumCbox.Checked) ? Linlib.LIN_ENHANCED_CHECKSUM : 0;
            varLength = (VarDlcCBox.Checked) ? Linlib.LIN_VARIABLE_DLC : 0;
            UpdateChanSettingGroup();
         }
         else
         {
            // update the bus configuration data
            channelNumber = Convert.ToInt32(ChannelTBox.Text);
            bps = Convert.ToUInt32(BpsTBox.Text);
            nodeType = (GBMaster.Checked) ? Linlib.LIN_MASTER : Linlib.LIN_SLAVE;
            enhanceCKSum = (EnhancedCKsumCbox.Checked) ? Linlib.LIN_ENHANCED_CHECKSUM : 0;
            varLength = (VarDlcCBox.Checked) ? Linlib.LIN_VARIABLE_DLC : 0;
            UpdateChanSettingGroup();
         }
      } // ConfigApply_Click

      // Ensures panel controls are updated to the correct information when tabs are changed.
      private void TabCntl_Click(object sender, EventArgs e)
      {
         ChannelTBox.Text = channelNumber.ToString();
         BpsTBox.Text = bps.ToString();
         GBMaster.Checked = (nodeType == Linlib.LIN_MASTER);
         GBSlave.Checked = (nodeType == Linlib.LIN_SLAVE);
         EnhancedCKsumCbox.Checked = (enhanceCKSum == Linlib.LIN_ENHANCED_CHECKSUM);
         VarDlcCBox.Checked = (varLength == Linlib.LIN_VARIABLE_DLC);

         if (CurOnBus)
         {
            ChannelTBox.Enabled = false;
            GBMaster.Enabled = false;
            GBSlave.Enabled = false;
         }
         else
         {
            ChannelTBox.Enabled = true;
            GBMaster.Enabled = true;
            GBSlave.Enabled = true;
         }
      } // TabCntl_Click

      // ensures all related forms and threads are properly shutdown.
      private void FormClosingHandler(object sender, CancelEventArgs e)
      {
         // shutdown the thread
         linWorker.ShutdownNow();

         // allow Output View to close
         outForm.ShutdownNow();
      } // FormClosingHandler

      // Event handler for the View->Output Window menu item being clicked.
      private void outputWindowToolStripMenuItem_Click(object sender, EventArgs e)
      {
         if (outputWindowToolStripMenuItem.Checked)
         {
            outForm.Hide();
            outputWindowToolStripMenuItem.Checked = false;
         }
         else
         {
            outForm.Show();
            // this is necessary if the minimize box was used to hide the output view form.
            outForm.WindowState = FormWindowState.Normal;
            outputWindowToolStripMenuItem.Checked = true;
         }
      } // outputWindowToolStripMenuItem_Click

      // Resize event handler to ensure all associated forms are hidden and restored when this
      // form is minimized and restored.  
      private void ResizeHandler(object sender, System.EventArgs e)
      {
         // just update the view info if the minimize button was pressed.
         if (this.WindowState == FormWindowState.Minimized)
         {
            outForm.Hide();
            wasMinimized = true;
         }
         else if (wasMinimized & outputWindowToolStripMenuItem.Checked)
         {
            wasMinimized = false;
            outForm.Show();
         }
      } // ResizeHandler

   } // LinViewer Class

} // LinViewer namespace
