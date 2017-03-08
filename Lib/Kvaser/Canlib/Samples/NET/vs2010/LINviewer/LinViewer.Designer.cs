namespace LinViewer
{
   partial class LinViewer
   {
      /// <summary>
      /// Required designer variable.
      /// </summary>
      private System.ComponentModel.IContainer components = null;

      /// <summary>
      /// Clean up any resources being used.
      /// </summary>
      /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
      protected override void Dispose(bool disposing)
      {
         if (disposing && (components != null))
         {
            components.Dispose();
         }
         base.Dispose(disposing);
      }

      #region Windows Form Designer generated code

      /// <summary>
      /// Required method for Designer support - do not modify
      /// the contents of this method with the code editor.
      /// </summary>
      private void InitializeComponent()
      {
         this.ChannelTabCntl = new System.Windows.Forms.TabControl();
         this.StatPage = new System.Windows.Forms.TabPage();
         this.ChanSetGroup = new System.Windows.Forms.GroupBox();
         this.ChanNumLabel = new System.Windows.Forms.Label();
         this.label6 = new System.Windows.Forms.Label();
         this.OptionsLabel = new System.Windows.Forms.Label();
         this.BpsLabel = new System.Windows.Forms.Label();
         this.NodeTypeLabel = new System.Windows.Forms.Label();
         this.label3 = new System.Windows.Forms.Label();
         this.label2 = new System.Windows.Forms.Label();
         this.label1 = new System.Windows.Forms.Label();
         this.BusOffLED = new System.Windows.Forms.RadioButton();
         this.BusOnLED = new System.Windows.Forms.RadioButton();
         this.GoBusOff = new System.Windows.Forms.Button();
         this.GoBusOn = new System.Windows.Forms.Button();
         this.ConfigPage = new System.Windows.Forms.TabPage();
         this.BpsTBox = new System.Windows.Forms.MaskedTextBox();
         this.ChannelTBox = new System.Windows.Forms.MaskedTextBox();
         this.VarDlcCBox = new System.Windows.Forms.CheckBox();
         this.EnhancedCKsumCbox = new System.Windows.Forms.CheckBox();
         this.NodeTypeGBox = new System.Windows.Forms.GroupBox();
         this.GBSlave = new System.Windows.Forms.RadioButton();
         this.GBMaster = new System.Windows.Forms.RadioButton();
         this.label5 = new System.Windows.Forms.Label();
         this.label4 = new System.Windows.Forms.Label();
         this.ConfigApply = new System.Windows.Forms.Button();
         this.MainMenu = new System.Windows.Forms.MenuStrip();
         this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
         this.outputWindowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
         this.ChannelTabCntl.SuspendLayout();
         this.StatPage.SuspendLayout();
         this.ChanSetGroup.SuspendLayout();
         this.ConfigPage.SuspendLayout();
         this.NodeTypeGBox.SuspendLayout();
         this.MainMenu.SuspendLayout();
         this.SuspendLayout();
         // 
         // ChannelTabCntl
         // 
         this.ChannelTabCntl.Controls.Add(this.StatPage);
         this.ChannelTabCntl.Controls.Add(this.ConfigPage);
         this.ChannelTabCntl.Location = new System.Drawing.Point(17, 25);
         this.ChannelTabCntl.Name = "ChannelTabCntl";
         this.ChannelTabCntl.SelectedIndex = 0;
         this.ChannelTabCntl.Size = new System.Drawing.Size(260, 227);
         this.ChannelTabCntl.TabIndex = 0;
         // 
         // StatPage
         // 
         this.StatPage.BackColor = System.Drawing.SystemColors.ControlLight;
         this.StatPage.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
         this.StatPage.Controls.Add(this.ChanSetGroup);
         this.StatPage.Controls.Add(this.BusOffLED);
         this.StatPage.Controls.Add(this.BusOnLED);
         this.StatPage.Controls.Add(this.GoBusOff);
         this.StatPage.Controls.Add(this.GoBusOn);
         this.StatPage.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.StatPage.Location = new System.Drawing.Point(4, 22);
         this.StatPage.Name = "StatPage";
         this.StatPage.Padding = new System.Windows.Forms.Padding(3);
         this.StatPage.Size = new System.Drawing.Size(252, 201);
         this.StatPage.TabIndex = 0;
         this.StatPage.Text = "Control";
         // 
         // ChanSetGroup
         // 
         this.ChanSetGroup.Controls.Add(this.ChanNumLabel);
         this.ChanSetGroup.Controls.Add(this.label6);
         this.ChanSetGroup.Controls.Add(this.OptionsLabel);
         this.ChanSetGroup.Controls.Add(this.BpsLabel);
         this.ChanSetGroup.Controls.Add(this.NodeTypeLabel);
         this.ChanSetGroup.Controls.Add(this.label3);
         this.ChanSetGroup.Controls.Add(this.label2);
         this.ChanSetGroup.Controls.Add(this.label1);
         this.ChanSetGroup.Location = new System.Drawing.Point(14, 15);
         this.ChanSetGroup.Name = "ChanSetGroup";
         this.ChanSetGroup.Size = new System.Drawing.Size(222, 116);
         this.ChanSetGroup.TabIndex = 4;
         this.ChanSetGroup.TabStop = false;
         this.ChanSetGroup.Text = "Channel Settings";
         // 
         // ChanNumLabel
         // 
         this.ChanNumLabel.AutoSize = true;
         this.ChanNumLabel.Location = new System.Drawing.Point(88, 16);
         this.ChanNumLabel.Name = "ChanNumLabel";
         this.ChanNumLabel.Size = new System.Drawing.Size(13, 13);
         this.ChanNumLabel.TabIndex = 7;
         this.ChanNumLabel.Text = "0";
         // 
         // label6
         // 
         this.label6.AutoSize = true;
         this.label6.Location = new System.Drawing.Point(18, 37);
         this.label6.Name = "label6";
         this.label6.Size = new System.Drawing.Size(63, 13);
         this.label6.TabIndex = 6;
         this.label6.Text = "Node Type:";
         // 
         // OptionsLabel
         // 
         this.OptionsLabel.AutoSize = true;
         this.OptionsLabel.Location = new System.Drawing.Point(88, 83);
         this.OptionsLabel.Name = "OptionsLabel";
         this.OptionsLabel.Size = new System.Drawing.Size(109, 26);
         this.OptionsLabel.TabIndex = 5;
         this.OptionsLabel.Text = "Enhanced Checksum\r\nVariable DLC";
         // 
         // BpsLabel
         // 
         this.BpsLabel.AutoSize = true;
         this.BpsLabel.Location = new System.Drawing.Point(88, 60);
         this.BpsLabel.Name = "BpsLabel";
         this.BpsLabel.Size = new System.Drawing.Size(37, 13);
         this.BpsLabel.TabIndex = 4;
         this.BpsLabel.Text = "20000";
         // 
         // NodeTypeLabel
         // 
         this.NodeTypeLabel.AutoSize = true;
         this.NodeTypeLabel.Location = new System.Drawing.Point(88, 37);
         this.NodeTypeLabel.Name = "NodeTypeLabel";
         this.NodeTypeLabel.Size = new System.Drawing.Size(39, 13);
         this.NodeTypeLabel.TabIndex = 3;
         this.NodeTypeLabel.Text = "Master";
         // 
         // label3
         // 
         this.label3.AutoSize = true;
         this.label3.Location = new System.Drawing.Point(18, 83);
         this.label3.Name = "label3";
         this.label3.Size = new System.Drawing.Size(46, 13);
         this.label3.TabIndex = 2;
         this.label3.Text = "Options:";
         // 
         // label2
         // 
         this.label2.AutoSize = true;
         this.label2.Location = new System.Drawing.Point(18, 60);
         this.label2.Name = "label2";
         this.label2.Size = new System.Drawing.Size(49, 13);
         this.label2.TabIndex = 1;
         this.label2.Text = "Bits/sec:";
         // 
         // label1
         // 
         this.label1.AutoSize = true;
         this.label1.Location = new System.Drawing.Point(18, 16);
         this.label1.Name = "label1";
         this.label1.Size = new System.Drawing.Size(59, 13);
         this.label1.TabIndex = 0;
         this.label1.Text = "Channel #:";
         // 
         // BusOffLED
         // 
         this.BusOffLED.AutoSize = true;
         this.BusOffLED.Location = new System.Drawing.Point(151, 166);
         this.BusOffLED.Name = "BusOffLED";
         this.BusOffLED.Size = new System.Drawing.Size(60, 17);
         this.BusOffLED.TabIndex = 3;
         this.BusOffLED.TabStop = true;
         this.BusOffLED.Text = "Off Bus";
         this.BusOffLED.UseVisualStyleBackColor = true;
         // 
         // BusOnLED
         // 
         this.BusOnLED.AutoSize = true;
         this.BusOnLED.BackColor = System.Drawing.SystemColors.ControlLight;
         this.BusOnLED.ForeColor = System.Drawing.SystemColors.ControlText;
         this.BusOnLED.Location = new System.Drawing.Point(151, 143);
         this.BusOnLED.Name = "BusOnLED";
         this.BusOnLED.Size = new System.Drawing.Size(60, 17);
         this.BusOnLED.TabIndex = 2;
         this.BusOnLED.Text = "On Bus";
         this.BusOnLED.UseVisualStyleBackColor = false;
         // 
         // GoBusOff
         // 
         this.GoBusOff.Location = new System.Drawing.Point(14, 166);
         this.GoBusOff.Name = "GoBusOff";
         this.GoBusOff.Size = new System.Drawing.Size(75, 23);
         this.GoBusOff.TabIndex = 1;
         this.GoBusOff.Text = "Go Bus Off";
         this.GoBusOff.UseVisualStyleBackColor = true;
         this.GoBusOff.Click += new System.EventHandler(this.GoBusOff_Click);
         // 
         // GoBusOn
         // 
         this.GoBusOn.Location = new System.Drawing.Point(14, 137);
         this.GoBusOn.Name = "GoBusOn";
         this.GoBusOn.Size = new System.Drawing.Size(75, 23);
         this.GoBusOn.TabIndex = 0;
         this.GoBusOn.Text = "Go Bus On";
         this.GoBusOn.UseVisualStyleBackColor = true;
         this.GoBusOn.Click += new System.EventHandler(this.GoBusOn_Click);
         // 
         // ConfigPage
         // 
         this.ConfigPage.BackColor = System.Drawing.SystemColors.ControlLight;
         this.ConfigPage.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
         this.ConfigPage.Controls.Add(this.BpsTBox);
         this.ConfigPage.Controls.Add(this.ChannelTBox);
         this.ConfigPage.Controls.Add(this.VarDlcCBox);
         this.ConfigPage.Controls.Add(this.EnhancedCKsumCbox);
         this.ConfigPage.Controls.Add(this.NodeTypeGBox);
         this.ConfigPage.Controls.Add(this.label5);
         this.ConfigPage.Controls.Add(this.label4);
         this.ConfigPage.Controls.Add(this.ConfigApply);
         this.ConfigPage.Location = new System.Drawing.Point(4, 22);
         this.ConfigPage.Name = "ConfigPage";
         this.ConfigPage.Padding = new System.Windows.Forms.Padding(3);
         this.ConfigPage.Size = new System.Drawing.Size(252, 201);
         this.ConfigPage.TabIndex = 1;
         this.ConfigPage.Text = "Config";
         // 
         // BpsTBox
         // 
         this.BpsTBox.Location = new System.Drawing.Point(136, 34);
         this.BpsTBox.Mask = "99990";
         this.BpsTBox.Name = "BpsTBox";
         this.BpsTBox.PromptChar = ' ';
         this.BpsTBox.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
         this.BpsTBox.Size = new System.Drawing.Size(45, 20);
         this.BpsTBox.TabIndex = 9;
         this.BpsTBox.Text = "20000";
         // 
         // ChannelTBox
         // 
         this.ChannelTBox.Location = new System.Drawing.Point(136, 6);
         this.ChannelTBox.Mask = "90";
         this.ChannelTBox.Name = "ChannelTBox";
         this.ChannelTBox.PromptChar = ' ';
         this.ChannelTBox.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
         this.ChannelTBox.Size = new System.Drawing.Size(45, 20);
         this.ChannelTBox.TabIndex = 8;
         this.ChannelTBox.Text = "0";
         // 
         // VarDlcCBox
         // 
         this.VarDlcCBox.AutoSize = true;
         this.VarDlcCBox.Location = new System.Drawing.Point(61, 141);
         this.VarDlcCBox.Name = "VarDlcCBox";
         this.VarDlcCBox.Size = new System.Drawing.Size(88, 17);
         this.VarDlcCBox.TabIndex = 7;
         this.VarDlcCBox.Text = "Variable DLC";
         this.VarDlcCBox.UseVisualStyleBackColor = true;
         // 
         // EnhancedCKsumCbox
         // 
         this.EnhancedCKsumCbox.AutoSize = true;
         this.EnhancedCKsumCbox.Location = new System.Drawing.Point(61, 118);
         this.EnhancedCKsumCbox.Name = "EnhancedCKsumCbox";
         this.EnhancedCKsumCbox.Size = new System.Drawing.Size(128, 17);
         this.EnhancedCKsumCbox.TabIndex = 6;
         this.EnhancedCKsumCbox.Text = "Enhanced Checksum";
         this.EnhancedCKsumCbox.UseVisualStyleBackColor = true;
         // 
         // NodeTypeGBox
         // 
         this.NodeTypeGBox.Controls.Add(this.GBSlave);
         this.NodeTypeGBox.Controls.Add(this.GBMaster);
         this.NodeTypeGBox.Location = new System.Drawing.Point(25, 60);
         this.NodeTypeGBox.Name = "NodeTypeGBox";
         this.NodeTypeGBox.Size = new System.Drawing.Size(200, 51);
         this.NodeTypeGBox.TabIndex = 5;
         this.NodeTypeGBox.TabStop = false;
         this.NodeTypeGBox.Text = "Node Type";
         // 
         // GBSlave
         // 
         this.GBSlave.AutoSize = true;
         this.GBSlave.Location = new System.Drawing.Point(115, 24);
         this.GBSlave.Name = "GBSlave";
         this.GBSlave.Size = new System.Drawing.Size(52, 17);
         this.GBSlave.TabIndex = 1;
         this.GBSlave.TabStop = true;
         this.GBSlave.Text = "Slave";
         this.GBSlave.UseVisualStyleBackColor = true;
         // 
         // GBMaster
         // 
         this.GBMaster.AutoSize = true;
         this.GBMaster.Location = new System.Drawing.Point(33, 24);
         this.GBMaster.Name = "GBMaster";
         this.GBMaster.Size = new System.Drawing.Size(57, 17);
         this.GBMaster.TabIndex = 0;
         this.GBMaster.TabStop = true;
         this.GBMaster.Text = "Master";
         this.GBMaster.UseVisualStyleBackColor = true;
         // 
         // label5
         // 
         this.label5.AutoSize = true;
         this.label5.Location = new System.Drawing.Point(70, 37);
         this.label5.Name = "label5";
         this.label5.Size = new System.Drawing.Size(31, 13);
         this.label5.TabIndex = 4;
         this.label5.Text = "BPS:";
         // 
         // label4
         // 
         this.label4.AutoSize = true;
         this.label4.Location = new System.Drawing.Point(70, 10);
         this.label4.Name = "label4";
         this.label4.Size = new System.Drawing.Size(59, 13);
         this.label4.TabIndex = 1;
         this.label4.Text = "Channel #:";
         // 
         // ConfigApply
         // 
         this.ConfigApply.Location = new System.Drawing.Point(88, 170);
         this.ConfigApply.Name = "ConfigApply";
         this.ConfigApply.Size = new System.Drawing.Size(75, 23);
         this.ConfigApply.TabIndex = 0;
         this.ConfigApply.Text = "Apply";
         this.ConfigApply.UseVisualStyleBackColor = true;
         this.ConfigApply.Click += new System.EventHandler(this.ConfigApply_Click);
         // 
         // MainMenu
         // 
         this.MainMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1});
         this.MainMenu.Location = new System.Drawing.Point(0, 0);
         this.MainMenu.Name = "MainMenu";
         this.MainMenu.Size = new System.Drawing.Size(294, 24);
         this.MainMenu.TabIndex = 1;
         this.MainMenu.Text = "menuStrip1";
         // 
         // toolStripMenuItem1
         // 
         this.toolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.outputWindowToolStripMenuItem});
         this.toolStripMenuItem1.Name = "toolStripMenuItem1";
         this.toolStripMenuItem1.Size = new System.Drawing.Size(44, 20);
         this.toolStripMenuItem1.Text = "View";
         // 
         // outputWindowToolStripMenuItem
         // 
         this.outputWindowToolStripMenuItem.Checked = true;
         this.outputWindowToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
         this.outputWindowToolStripMenuItem.Name = "outputWindowToolStripMenuItem";
         this.outputWindowToolStripMenuItem.Size = new System.Drawing.Size(159, 22);
         this.outputWindowToolStripMenuItem.Text = "Output Window";
         this.outputWindowToolStripMenuItem.Click += new System.EventHandler(this.outputWindowToolStripMenuItem_Click);
         // 
         // LinViewer
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(294, 274);
         this.Controls.Add(this.ChannelTabCntl);
         this.Controls.Add(this.MainMenu);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
         this.MainMenuStrip = this.MainMenu;
         this.MaximumSize = new System.Drawing.Size(300, 300);
         this.MinimumSize = new System.Drawing.Size(300, 300);
         this.Name = "LinViewer";
         this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
         this.Text = "LinViewer";
         this.ChannelTabCntl.ResumeLayout(false);
         this.StatPage.ResumeLayout(false);
         this.StatPage.PerformLayout();
         this.ChanSetGroup.ResumeLayout(false);
         this.ChanSetGroup.PerformLayout();
         this.ConfigPage.ResumeLayout(false);
         this.ConfigPage.PerformLayout();
         this.NodeTypeGBox.ResumeLayout(false);
         this.NodeTypeGBox.PerformLayout();
         this.MainMenu.ResumeLayout(false);
         this.MainMenu.PerformLayout();
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.TabControl ChannelTabCntl;
      private System.Windows.Forms.TabPage StatPage;
      private System.Windows.Forms.TabPage ConfigPage;
      private System.Windows.Forms.Button GoBusOn;
      private System.Windows.Forms.Button GoBusOff;
      private System.Windows.Forms.RadioButton BusOnLED;
      private System.Windows.Forms.RadioButton BusOffLED;
      private System.Windows.Forms.GroupBox ChanSetGroup;
      private System.Windows.Forms.Label label3;
      private System.Windows.Forms.Label label2;
      private System.Windows.Forms.Label label1;
      private System.Windows.Forms.Label OptionsLabel;
      private System.Windows.Forms.Label BpsLabel;
      private System.Windows.Forms.Label NodeTypeLabel;
      private System.Windows.Forms.Label label4;
      private System.Windows.Forms.Button ConfigApply;
      private System.Windows.Forms.Label label5;
      private System.Windows.Forms.GroupBox NodeTypeGBox;
      private System.Windows.Forms.RadioButton GBMaster;
      private System.Windows.Forms.RadioButton GBSlave;
      private System.Windows.Forms.CheckBox VarDlcCBox;
      private System.Windows.Forms.CheckBox EnhancedCKsumCbox;
      private System.Windows.Forms.Label ChanNumLabel;
      private System.Windows.Forms.Label label6;
      private System.Windows.Forms.MaskedTextBox ChannelTBox;
      private System.Windows.Forms.MaskedTextBox BpsTBox;
      private System.Windows.Forms.MenuStrip MainMenu;
      private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
      private System.Windows.Forms.ToolStripMenuItem outputWindowToolStripMenuItem;
   }
}

