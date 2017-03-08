using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace CSharpSimpleWindowSample
{
  /// <summary>
  /// Summary description for Form1.
  /// </summary>
  public class Form1 : System.Windows.Forms.Form
  {
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.Button button1;
    private System.Windows.Forms.Button button2;
    private System.Windows.Forms.Button button3;
    private System.Windows.Forms.Button button4;
    private System.Windows.Forms.Button button5;
    private System.Windows.Forms.Button button6;
    private System.Windows.Forms.TextBox textBox1;
    private System.Windows.Forms.TextBox textBox2;
    private System.Windows.Forms.TextBox textBox3;
    private System.Windows.Forms.RadioButton radioButton1;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.Label label7;
    private System.Windows.Forms.Label label8;
    private System.Windows.Forms.Label label9;
    private System.Windows.Forms.Label label10;
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;
    
    int hnd0, hnd1; 
    canlibCLSNET.Canlib.canStatus can_status; 
    
    public Form1()
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      //
      // TODO: Add any constructor code after InitializeComponent call
      //
    }

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if (components != null) 
        {
          components.Dispose();
        }
      }
      base.Dispose( disposing );
    }

    #region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.label5 = new System.Windows.Forms.Label();
      this.label4 = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.textBox2 = new System.Windows.Forms.TextBox();
      this.textBox1 = new System.Windows.Forms.TextBox();
      this.button3 = new System.Windows.Forms.Button();
      this.button2 = new System.Windows.Forms.Button();
      this.button1 = new System.Windows.Forms.Button();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.label10 = new System.Windows.Forms.Label();
      this.label9 = new System.Windows.Forms.Label();
      this.label8 = new System.Windows.Forms.Label();
      this.label7 = new System.Windows.Forms.Label();
      this.label6 = new System.Windows.Forms.Label();
      this.radioButton1 = new System.Windows.Forms.RadioButton();
      this.textBox3 = new System.Windows.Forms.TextBox();
      this.button6 = new System.Windows.Forms.Button();
      this.button5 = new System.Windows.Forms.Button();
      this.button4 = new System.Windows.Forms.Button();
      this.groupBox1.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.SuspendLayout();
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.label5);
      this.groupBox1.Controls.Add(this.label4);
      this.groupBox1.Controls.Add(this.label3);
      this.groupBox1.Controls.Add(this.label2);
      this.groupBox1.Controls.Add(this.label1);
      this.groupBox1.Controls.Add(this.textBox2);
      this.groupBox1.Controls.Add(this.textBox1);
      this.groupBox1.Controls.Add(this.button3);
      this.groupBox1.Controls.Add(this.button2);
      this.groupBox1.Controls.Add(this.button1);
      this.groupBox1.Location = new System.Drawing.Point(24, 8);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(256, 240);
      this.groupBox1.TabIndex = 0;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Configurations";
      // 
      // label5
      // 
      this.label5.Location = new System.Drawing.Point(104, 192);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size(144, 24);
      this.label5.TabIndex = 9;
      this.label5.Text = "Bitrate";
      this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label4
      // 
      this.label4.Location = new System.Drawing.Point(104, 152);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(144, 24);
      this.label4.TabIndex = 8;
      this.label4.Text = "Gets the bitrate";
      this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label3
      // 
      this.label3.Location = new System.Drawing.Point(104, 108);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(144, 24);
      this.label3.TabIndex = 7;
      this.label3.Text = "Card type handle 1";
      this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label2
      // 
      this.label2.Location = new System.Drawing.Point(104, 72);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(144, 24);
      this.label2.TabIndex = 6;
      this.label2.Text = "Gets the card type";
      this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(104, 32);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(144, 24);
      this.label1.TabIndex = 5;
      this.label1.Text = "Initializes the hardware";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // textBox2
      // 
      this.textBox2.Location = new System.Drawing.Point(16, 192);
      this.textBox2.Name = "textBox2";
      this.textBox2.Size = new System.Drawing.Size(72, 20);
      this.textBox2.TabIndex = 4;
      this.textBox2.Text = "";
      // 
      // textBox1
      // 
      this.textBox1.Location = new System.Drawing.Point(16, 112);
      this.textBox1.Name = "textBox1";
      this.textBox1.Size = new System.Drawing.Size(72, 20);
      this.textBox1.TabIndex = 3;
      this.textBox1.Text = "";
      // 
      // button3
      // 
      this.button3.Location = new System.Drawing.Point(16, 152);
      this.button3.Name = "button3";
      this.button3.Size = new System.Drawing.Size(72, 24);
      this.button3.TabIndex = 2;
      this.button3.Text = "Bitrate";
      this.button3.Click += new System.EventHandler(this.button3_Click);
      // 
      // button2
      // 
      this.button2.Location = new System.Drawing.Point(16, 72);
      this.button2.Name = "button2";
      this.button2.Size = new System.Drawing.Size(72, 24);
      this.button2.TabIndex = 1;
      this.button2.Text = "Card type";
      this.button2.Click += new System.EventHandler(this.button2_Click);
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(16, 32);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(72, 24);
      this.button1.TabIndex = 0;
      this.button1.Text = "Initialize";
      this.button1.Click += new System.EventHandler(this.button1_Click);
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.label10);
      this.groupBox2.Controls.Add(this.label9);
      this.groupBox2.Controls.Add(this.label8);
      this.groupBox2.Controls.Add(this.label7);
      this.groupBox2.Controls.Add(this.label6);
      this.groupBox2.Controls.Add(this.radioButton1);
      this.groupBox2.Controls.Add(this.textBox3);
      this.groupBox2.Controls.Add(this.button6);
      this.groupBox2.Controls.Add(this.button5);
      this.groupBox2.Controls.Add(this.button4);
      this.groupBox2.Location = new System.Drawing.Point(304, 8);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(256, 240);
      this.groupBox2.TabIndex = 1;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Sending and receiving msgs";
      // 
      // label10
      // 
      this.label10.Location = new System.Drawing.Point(104, 192);
      this.label10.Name = "label10";
      this.label10.Size = new System.Drawing.Size(144, 24);
      this.label10.TabIndex = 11;
      this.label10.Text = "Numnber of received messages";
      this.label10.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label9
      // 
      this.label9.Location = new System.Drawing.Point(104, 152);
      this.label9.Name = "label9";
      this.label9.Size = new System.Drawing.Size(144, 24);
      this.label9.TabIndex = 10;
      this.label9.Text = "Reads the messages from the queue";
      this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label8
      // 
      this.label8.Location = new System.Drawing.Point(104, 112);
      this.label8.Name = "label8";
      this.label8.Size = new System.Drawing.Size(144, 24);
      this.label8.TabIndex = 9;
      this.label8.Text = "Indicates if there are any messages to read";
      this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label7
      // 
      this.label7.Location = new System.Drawing.Point(104, 72);
      this.label7.Name = "label7";
      this.label7.Size = new System.Drawing.Size(144, 24);
      this.label7.TabIndex = 8;
      this.label7.Text = "Sends 100 CAN msgs from channel 0";
      this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label6
      // 
      this.label6.Location = new System.Drawing.Point(104, 32);
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size(144, 24);
      this.label6.TabIndex = 7;
      this.label6.Text = "Sends a CAN msg from channel 0";
      this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // radioButton1
      // 
      this.radioButton1.Location = new System.Drawing.Point(16, 112);
      this.radioButton1.Name = "radioButton1";
      this.radioButton1.Size = new System.Drawing.Size(72, 24);
      this.radioButton1.TabIndex = 6;
      this.radioButton1.Text = "Receive";
      // 
      // textBox3
      // 
      this.textBox3.Location = new System.Drawing.Point(16, 192);
      this.textBox3.Name = "textBox3";
      this.textBox3.Size = new System.Drawing.Size(72, 20);
      this.textBox3.TabIndex = 5;
      this.textBox3.Text = "";
      // 
      // button6
      // 
      this.button6.Location = new System.Drawing.Point(16, 152);
      this.button6.Name = "button6";
      this.button6.Size = new System.Drawing.Size(72, 24);
      this.button6.TabIndex = 3;
      this.button6.Text = "Read msg";
      this.button6.Click += new System.EventHandler(this.button6_Click);
      // 
      // button5
      // 
      this.button5.Location = new System.Drawing.Point(16, 72);
      this.button5.Name = "button5";
      this.button5.Size = new System.Drawing.Size(72, 24);
      this.button5.TabIndex = 2;
      this.button5.Text = "Write msgs";
      this.button5.Click += new System.EventHandler(this.button5_Click);
      // 
      // button4
      // 
      this.button4.Location = new System.Drawing.Point(16, 32);
      this.button4.Name = "button4";
      this.button4.Size = new System.Drawing.Size(72, 24);
      this.button4.TabIndex = 1;
      this.button4.Text = "Write msg";
      this.button4.Click += new System.EventHandler(this.button4_Click);
      // 
      // Form1
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(584, 278);
      this.Controls.Add(this.groupBox1);
      this.Controls.Add(this.groupBox2);
      this.Name = "Form1";
      this.Text = "Kvaser C# Simple Window Sample";
      this.groupBox1.ResumeLayout(false);
      this.groupBox2.ResumeLayout(false);
      this.ResumeLayout(false);

    }
    #endregion

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main() 
    {
      Application.Run(new Form1());
    }
    
    private void button1_Click(object sender, System.EventArgs e)
    {
      canlibCLSNET.Canlib.canInitializeLibrary();
      
      hnd0 = canlibCLSNET.Canlib.canOpenChannel(0,0);
      hnd1 = canlibCLSNET.Canlib.canOpenChannel(1,0);
       
      canlibCLSNET.Canlib.canSetBusParams(hnd0, canlibCLSNET.Canlib.canBITRATE_125K, 0, 0, 0, 0, 0);
      canlibCLSNET.Canlib.canSetBusParams(hnd1, canlibCLSNET.Canlib.canBITRATE_125K, 0, 0, 0, 0, 0);

      canlibCLSNET.Canlib.canBusOn(hnd0);
      canlibCLSNET.Canlib.canBusOn(hnd1);
      this.button1.Enabled = false;

    }

    private void button2_Click(object sender, System.EventArgs e)
    {
      object hwinfo;
      canlibCLSNET.Canlib.canGetChannelData(hnd0, canlibCLSNET.Canlib.canCHANNELDATA_CARD_TYPE, out hwinfo);
      switch((int)hwinfo)
      {
        case canlibCLSNET.Canlib.canHWTYPE_NONE:
          this.textBox1.Text = "Unknown";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_LAPCAN:
          this.textBox1.Text = "LAPcan";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_USBCAN:
          this.textBox1.Text = "USBcan";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_USBCAN_II:
          this.textBox1.Text = "USBcanII";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_PCCAN:
          this.textBox1.Text = "PCcan";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_PCICAN:
          this.textBox1.Text = "PCIcan";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_PCICAN_II:
          this.textBox1.Text = "PCIcanII";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_SIMULATED:
          this.textBox1.Text = "Simulated";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_VIRTUAL:
          this.textBox1.Text = "Virtual";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_LEAF:
          this.textBox1.Text = " Kvaser Leaf";
          break;
        case canlibCLSNET.Canlib.canHWTYPE_USBCAN_LIGHT:
          this.textBox1.Text = "Kvaser USBcan Light";
          break;
        default:
          this.textBox1.Text ="Unknown";
          break;
      }

    }

    private void button3_Click(object sender, System.EventArgs e)
    {
      long freq;
      int tseg1, tseg2, sjw, nosamp, syncmode;
      can_status = canlibCLSNET.Canlib.canGetBusParams(hnd0, out freq, out tseg1, out tseg2, out sjw, out nosamp, out syncmode);
      this.textBox2.Text =  freq.ToString();

      // This is pointless in this context - it's here just to
      // demonstrate how to call canIoCtl..
      //
      // By the way, a customer reports that the following code:
      // canlibCLSNET.Canlib.canIoCtl(intHdlCanCh0,
      //   canlibCLSNET.Canlib.canIOCTL_SET_TIMER_SCALE,
      //   Convert.ToUInt32(100))
      // works fine in VB.NET.
      //
      
      object tmp = (UInt32) 100;
      canlibCLSNET.Canlib.canIoCtl(hnd0,
                                   canlibCLSNET.Canlib.canIOCTL_SET_TIMER_SCALE,
                                   ref tmp);
    }

    private void button4_Click(object sender, System.EventArgs e)
    {
      byte[] msg = {1,2,3,4,5,6};
      
      can_status = canlibCLSNET.Canlib.canWrite(hnd0, 123, msg, 6, 0);
      if (can_status != canlibCLSNET.Canlib.canStatus.canOK)
      {
        MessageBox.Show("Error sending messages");
      }
      else
      {
      this.radioButton1.Checked = true;
      }
    }

    private void button5_Click(object sender, System.EventArgs e)
    {
      byte[] msg = {1,2,3,4,5,6};
      int i;
      
      for (i =0; i<100; i++)
      {
        can_status = canlibCLSNET.Canlib.canWrite(hnd0, 123, msg, 6, 0);
      }
      if (can_status != canlibCLSNET.Canlib.canStatus.canOK)
      {
        MessageBox.Show("Error sending messages");
      }
      else
      {
        this.radioButton1.Checked = true;
      }
    }
  

    private void button6_Click(object sender, System.EventArgs e)
    {
      byte[] msg = {0, 0, 0, 0, 0, 0};
      int dlc, flag, i=0, id = 123;
      long time;

      can_status = canlibCLSNET.Canlib.canStatus.canOK;
      while (can_status == canlibCLSNET.Canlib.canStatus.canOK)
      {
        can_status = canlibCLSNET.Canlib.canRead(hnd1, out id, msg, out dlc, out flag, out time);
        if(can_status == canlibCLSNET.Canlib.canStatus.canOK)
        {
          i= i +1;
        }
      }
      this.radioButton1.Checked = false;
      this.textBox3.Text = i.ToString();
    }

  }  

}
