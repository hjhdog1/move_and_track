namespace CsAutoResponse
{
    partial class AutoResponse
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
           this.InitChannelB = new System.Windows.Forms.Button();
           this.BusOnB = new System.Windows.Forms.Button();
           this.BusOffB = new System.Windows.Forms.Button();
           this.CloseChannelB = new System.Windows.Forms.Button();
           this.OutputRTB = new System.Windows.Forms.RichTextBox();
           this.AutoConfigGB = new System.Windows.Forms.GroupBox();
           this.groupBox2 = new System.Windows.Forms.GroupBox();
           this.label14 = new System.Windows.Forms.Label();
           this.label13 = new System.Windows.Forms.Label();
           this.label12 = new System.Windows.Forms.Label();
           this.label11 = new System.Windows.Forms.Label();
           this.label10 = new System.Windows.Forms.Label();
           this.label9 = new System.Windows.Forms.Label();
           this.label8 = new System.Windows.Forms.Label();
           this.label7 = new System.Windows.Forms.Label();
           this.label6 = new System.Windows.Forms.Label();
           this.label5 = new System.Windows.Forms.Label();
           this.ExtendedIdCB = new System.Windows.Forms.CheckBox();
           this.label4 = new System.Windows.Forms.Label();
           this.groupBox1 = new System.Windows.Forms.GroupBox();
           this.RTROnlyCB = new System.Windows.Forms.CheckBox();
           this.label2 = new System.Windows.Forms.Label();
           this.label1 = new System.Windows.Forms.Label();
           this.RemoveB = new System.Windows.Forms.Button();
           this.UpdateContentsB = new System.Windows.Forms.Button();
           this.CreateB = new System.Windows.Forms.Button();
           this.label3 = new System.Windows.Forms.Label();
           this.label15 = new System.Windows.Forms.Label();
           this.ChannelNumMTB = new System.Windows.Forms.MaskedTextBox();
           this.label16 = new System.Windows.Forms.Label();
           this.Data7MTB = new CsAutoResponse.HexTextBox();
           this.Data6MTB = new CsAutoResponse.HexTextBox();
           this.Data5MTB = new CsAutoResponse.HexTextBox();
           this.Data4MTB = new CsAutoResponse.HexTextBox();
           this.Data3MTB = new CsAutoResponse.HexTextBox();
           this.Data2MTB = new CsAutoResponse.HexTextBox();
           this.Data1MTB = new CsAutoResponse.HexTextBox();
           this.Data0MTB = new CsAutoResponse.HexTextBox();
           this.DlcMTB = new CsAutoResponse.HexTextBox();
           this.IdentifierMTB = new CsAutoResponse.HexTextBox();
           this.MFMaskMTB = new CsAutoResponse.HexTextBox();
           this.MFCodeMTB = new CsAutoResponse.HexTextBox();
           this.AutoConfigGB.SuspendLayout();
           this.groupBox2.SuspendLayout();
           this.groupBox1.SuspendLayout();
           this.SuspendLayout();
           // 
           // InitChannelB
           // 
           this.InitChannelB.Location = new System.Drawing.Point(20, 12);
           this.InitChannelB.Name = "InitChannelB";
           this.InitChannelB.Size = new System.Drawing.Size(91, 52);
           this.InitChannelB.TabIndex = 0;
           this.InitChannelB.Text = "Initial Channel";
           this.InitChannelB.UseVisualStyleBackColor = true;
           this.InitChannelB.Click += new System.EventHandler(this.InitChannelB_Click);
           // 
           // BusOnB
           // 
           this.BusOnB.Location = new System.Drawing.Point(20, 322);
           this.BusOnB.Name = "BusOnB";
           this.BusOnB.Size = new System.Drawing.Size(91, 52);
           this.BusOnB.TabIndex = 1;
           this.BusOnB.Text = "Bus On";
           this.BusOnB.UseVisualStyleBackColor = true;
           this.BusOnB.Click += new System.EventHandler(this.BusOnB_Click);
           // 
           // BusOffB
           // 
           this.BusOffB.Location = new System.Drawing.Point(20, 390);
           this.BusOffB.Name = "BusOffB";
           this.BusOffB.Size = new System.Drawing.Size(91, 52);
           this.BusOffB.TabIndex = 2;
           this.BusOffB.Text = "Bus Off";
           this.BusOffB.UseVisualStyleBackColor = true;
           this.BusOffB.Click += new System.EventHandler(this.BusOffB_Click);
           // 
           // CloseChannelB
           // 
           this.CloseChannelB.Location = new System.Drawing.Point(20, 459);
           this.CloseChannelB.Name = "CloseChannelB";
           this.CloseChannelB.Size = new System.Drawing.Size(91, 52);
           this.CloseChannelB.TabIndex = 3;
           this.CloseChannelB.Text = "Close Channel";
           this.CloseChannelB.UseVisualStyleBackColor = true;
           this.CloseChannelB.Click += new System.EventHandler(this.CloseChannelB_Click);
           // 
           // OutputRTB
           // 
           this.OutputRTB.Location = new System.Drawing.Point(138, 322);
           this.OutputRTB.Name = "OutputRTB";
           this.OutputRTB.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
           this.OutputRTB.Size = new System.Drawing.Size(659, 218);
           this.OutputRTB.TabIndex = 4;
           this.OutputRTB.Text = "";
           // 
           // AutoConfigGB
           // 
           this.AutoConfigGB.Controls.Add(this.groupBox2);
           this.AutoConfigGB.Controls.Add(this.groupBox1);
           this.AutoConfigGB.Controls.Add(this.RemoveB);
           this.AutoConfigGB.Controls.Add(this.UpdateContentsB);
           this.AutoConfigGB.Controls.Add(this.CreateB);
           this.AutoConfigGB.Location = new System.Drawing.Point(20, 82);
           this.AutoConfigGB.Name = "AutoConfigGB";
           this.AutoConfigGB.Size = new System.Drawing.Size(654, 234);
           this.AutoConfigGB.TabIndex = 5;
           this.AutoConfigGB.TabStop = false;
           this.AutoConfigGB.Text = "Auto Response Object Buffer Configuration";
           // 
           // groupBox2
           // 
           this.groupBox2.Controls.Add(this.Data7MTB);
           this.groupBox2.Controls.Add(this.Data6MTB);
           this.groupBox2.Controls.Add(this.Data5MTB);
           this.groupBox2.Controls.Add(this.Data4MTB);
           this.groupBox2.Controls.Add(this.Data3MTB);
           this.groupBox2.Controls.Add(this.Data2MTB);
           this.groupBox2.Controls.Add(this.Data1MTB);
           this.groupBox2.Controls.Add(this.Data0MTB);
           this.groupBox2.Controls.Add(this.label14);
           this.groupBox2.Controls.Add(this.label13);
           this.groupBox2.Controls.Add(this.label12);
           this.groupBox2.Controls.Add(this.label11);
           this.groupBox2.Controls.Add(this.label10);
           this.groupBox2.Controls.Add(this.label9);
           this.groupBox2.Controls.Add(this.label8);
           this.groupBox2.Controls.Add(this.label7);
           this.groupBox2.Controls.Add(this.label6);
           this.groupBox2.Controls.Add(this.DlcMTB);
           this.groupBox2.Controls.Add(this.label5);
           this.groupBox2.Controls.Add(this.ExtendedIdCB);
           this.groupBox2.Controls.Add(this.IdentifierMTB);
           this.groupBox2.Controls.Add(this.label4);
           this.groupBox2.Location = new System.Drawing.Point(145, 109);
           this.groupBox2.Name = "groupBox2";
           this.groupBox2.Size = new System.Drawing.Size(472, 119);
           this.groupBox2.TabIndex = 4;
           this.groupBox2.TabStop = false;
           this.groupBox2.Text = "Response Message";
           // 
           // label14
           // 
           this.label14.AutoSize = true;
           this.label14.Location = new System.Drawing.Point(363, 65);
           this.label14.Name = "label14";
           this.label14.Size = new System.Drawing.Size(13, 13);
           this.label14.TabIndex = 13;
           this.label14.Text = "7";
           // 
           // label13
           // 
           this.label13.AutoSize = true;
           this.label13.Location = new System.Drawing.Point(321, 65);
           this.label13.Name = "label13";
           this.label13.Size = new System.Drawing.Size(13, 13);
           this.label13.TabIndex = 12;
           this.label13.Text = "6";
           // 
           // label12
           // 
           this.label12.AutoSize = true;
           this.label12.Location = new System.Drawing.Point(279, 65);
           this.label12.Name = "label12";
           this.label12.Size = new System.Drawing.Size(13, 13);
           this.label12.TabIndex = 11;
           this.label12.Text = "5";
           // 
           // label11
           // 
           this.label11.AutoSize = true;
           this.label11.Location = new System.Drawing.Point(237, 65);
           this.label11.Name = "label11";
           this.label11.Size = new System.Drawing.Size(13, 13);
           this.label11.TabIndex = 10;
           this.label11.Text = "4";
           // 
           // label10
           // 
           this.label10.AutoSize = true;
           this.label10.Location = new System.Drawing.Point(196, 65);
           this.label10.Name = "label10";
           this.label10.Size = new System.Drawing.Size(13, 13);
           this.label10.TabIndex = 9;
           this.label10.Text = "3";
           // 
           // label9
           // 
           this.label9.AutoSize = true;
           this.label9.Location = new System.Drawing.Point(154, 65);
           this.label9.Name = "label9";
           this.label9.Size = new System.Drawing.Size(13, 13);
           this.label9.TabIndex = 8;
           this.label9.Text = "2";
           // 
           // label8
           // 
           this.label8.AutoSize = true;
           this.label8.Location = new System.Drawing.Point(113, 65);
           this.label8.Name = "label8";
           this.label8.Size = new System.Drawing.Size(13, 13);
           this.label8.TabIndex = 7;
           this.label8.Text = "1";
           // 
           // label7
           // 
           this.label7.AutoSize = true;
           this.label7.Location = new System.Drawing.Point(68, 65);
           this.label7.Name = "label7";
           this.label7.Size = new System.Drawing.Size(13, 13);
           this.label7.TabIndex = 6;
           this.label7.Text = "0";
           // 
           // label6
           // 
           this.label6.AutoSize = true;
           this.label6.Location = new System.Drawing.Point(20, 65);
           this.label6.Name = "label6";
           this.label6.Size = new System.Drawing.Size(30, 13);
           this.label6.TabIndex = 5;
           this.label6.Text = "Data";
           // 
           // label5
           // 
           this.label5.AutoSize = true;
           this.label5.Location = new System.Drawing.Point(326, 29);
           this.label5.Name = "label5";
           this.label5.Size = new System.Drawing.Size(28, 13);
           this.label5.TabIndex = 3;
           this.label5.Text = "DLC";
           // 
           // ExtendedIdCB
           // 
           this.ExtendedIdCB.AutoSize = true;
           this.ExtendedIdCB.Location = new System.Drawing.Point(172, 27);
           this.ExtendedIdCB.Name = "ExtendedIdCB";
           this.ExtendedIdCB.Size = new System.Drawing.Size(114, 17);
           this.ExtendedIdCB.TabIndex = 2;
           this.ExtendedIdCB.Text = "Extended Identifier";
           this.ExtendedIdCB.UseVisualStyleBackColor = true;
           // 
           // label4
           // 
           this.label4.AutoSize = true;
           this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.label4.Location = new System.Drawing.Point(20, 29);
           this.label4.Name = "label4";
           this.label4.Size = new System.Drawing.Size(47, 13);
           this.label4.TabIndex = 0;
           this.label4.Text = "Identifier";
           // 
           // groupBox1
           // 
           this.groupBox1.Controls.Add(this.MFMaskMTB);
           this.groupBox1.Controls.Add(this.RTROnlyCB);
           this.groupBox1.Controls.Add(this.label2);
           this.groupBox1.Controls.Add(this.label1);
           this.groupBox1.Controls.Add(this.MFCodeMTB);
           this.groupBox1.Location = new System.Drawing.Point(142, 31);
           this.groupBox1.Name = "groupBox1";
           this.groupBox1.Size = new System.Drawing.Size(475, 67);
           this.groupBox1.TabIndex = 3;
           this.groupBox1.TabStop = false;
           this.groupBox1.Text = "Message To Respond To";
           // 
           // RTROnlyCB
           // 
           this.RTROnlyCB.AutoSize = true;
           this.RTROnlyCB.Location = new System.Drawing.Point(326, 27);
           this.RTROnlyCB.Name = "RTROnlyCB";
           this.RTROnlyCB.Size = new System.Drawing.Size(129, 17);
           this.RTROnlyCB.TabIndex = 3;
           this.RTROnlyCB.Text = "Respond to RTR only";
           this.RTROnlyCB.UseVisualStyleBackColor = true;
           // 
           // label2
           // 
           this.label2.AutoSize = true;
           this.label2.Location = new System.Drawing.Point(172, 28);
           this.label2.Name = "label2";
           this.label2.Size = new System.Drawing.Size(32, 13);
           this.label2.TabIndex = 2;
           this.label2.Text = "Code";
           // 
           // label1
           // 
           this.label1.AutoSize = true;
           this.label1.Location = new System.Drawing.Point(20, 29);
           this.label1.Name = "label1";
           this.label1.Size = new System.Drawing.Size(33, 13);
           this.label1.TabIndex = 1;
           this.label1.Text = "Mask";
           // 
           // RemoveB
           // 
           this.RemoveB.Location = new System.Drawing.Point(8, 139);
           this.RemoveB.Name = "RemoveB";
           this.RemoveB.Size = new System.Drawing.Size(91, 52);
           this.RemoveB.TabIndex = 2;
           this.RemoveB.Text = "Remove";
           this.RemoveB.UseVisualStyleBackColor = true;
           this.RemoveB.Click += new System.EventHandler(this.RemoveB_Click);
           // 
           // UpdateContentsB
           // 
           this.UpdateContentsB.Location = new System.Drawing.Point(8, 81);
           this.UpdateContentsB.Name = "UpdateContentsB";
           this.UpdateContentsB.Size = new System.Drawing.Size(91, 52);
           this.UpdateContentsB.TabIndex = 1;
           this.UpdateContentsB.Text = "Update Contents";
           this.UpdateContentsB.UseVisualStyleBackColor = true;
           this.UpdateContentsB.Click += new System.EventHandler(this.UpdateContentsB_Click);
           // 
           // CreateB
           // 
           this.CreateB.Location = new System.Drawing.Point(8, 23);
           this.CreateB.Name = "CreateB";
           this.CreateB.Size = new System.Drawing.Size(91, 52);
           this.CreateB.TabIndex = 0;
           this.CreateB.Text = "Create";
           this.CreateB.UseVisualStyleBackColor = true;
           this.CreateB.Click += new System.EventHandler(this.CreateB_Click);
           // 
           // label3
           // 
           this.label3.AutoSize = true;
           this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.label3.Location = new System.Drawing.Point(402, 16);
           this.label3.Name = "label3";
           this.label3.Size = new System.Drawing.Size(283, 13);
           this.label3.TabIndex = 6;
           this.label3.Text = "All Numeric Values (Except For Channel Number)";
           // 
           // label15
           // 
           this.label15.AutoSize = true;
           this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.label15.Location = new System.Drawing.Point(441, 38);
           this.label15.Name = "label15";
           this.label15.Size = new System.Drawing.Size(204, 13);
           this.label15.TabIndex = 7;
           this.label15.Text = "Should Be Entered In Hexadecimal";
           // 
           // ChannelNumMTB
           // 
           this.ChannelNumMTB.Location = new System.Drawing.Point(162, 32);
           this.ChannelNumMTB.Mask = "90";
           this.ChannelNumMTB.Name = "ChannelNumMTB";
           this.ChannelNumMTB.PromptChar = ' ';
           this.ChannelNumMTB.Size = new System.Drawing.Size(24, 20);
           this.ChannelNumMTB.TabIndex = 8;
           this.ChannelNumMTB.Text = "0";
           // 
           // label16
           // 
           this.label16.AutoSize = true;
           this.label16.Location = new System.Drawing.Point(131, 16);
           this.label16.Name = "label16";
           this.label16.Size = new System.Drawing.Size(86, 13);
           this.label16.TabIndex = 9;
           this.label16.Text = "Channel Number";
           // 
           // Data7MTB
           // 
           this.Data7MTB.Location = new System.Drawing.Point(356, 87);
           this.Data7MTB.Mask = "CC";
           this.Data7MTB.Name = "Data7MTB";
           this.Data7MTB.PromptChar = ' ';
           this.Data7MTB.Size = new System.Drawing.Size(29, 20);
           this.Data7MTB.TabIndex = 21;
           this.Data7MTB.Text = "00";
           // 
           // Data6MTB
           // 
           this.Data6MTB.Location = new System.Drawing.Point(314, 87);
           this.Data6MTB.Mask = "CC";
           this.Data6MTB.Name = "Data6MTB";
           this.Data6MTB.PromptChar = ' ';
           this.Data6MTB.Size = new System.Drawing.Size(29, 20);
           this.Data6MTB.TabIndex = 20;
           this.Data6MTB.Text = "00";
           // 
           // Data5MTB
           // 
           this.Data5MTB.Location = new System.Drawing.Point(272, 87);
           this.Data5MTB.Mask = "CC";
           this.Data5MTB.Name = "Data5MTB";
           this.Data5MTB.PromptChar = ' ';
           this.Data5MTB.Size = new System.Drawing.Size(29, 20);
           this.Data5MTB.TabIndex = 19;
           this.Data5MTB.Text = "00";
           // 
           // Data4MTB
           // 
           this.Data4MTB.Location = new System.Drawing.Point(230, 87);
           this.Data4MTB.Mask = "CC";
           this.Data4MTB.Name = "Data4MTB";
           this.Data4MTB.PromptChar = ' ';
           this.Data4MTB.Size = new System.Drawing.Size(29, 20);
           this.Data4MTB.TabIndex = 18;
           this.Data4MTB.Text = "00";
           // 
           // Data3MTB
           // 
           this.Data3MTB.Location = new System.Drawing.Point(188, 87);
           this.Data3MTB.Mask = "CC";
           this.Data3MTB.Name = "Data3MTB";
           this.Data3MTB.PromptChar = ' ';
           this.Data3MTB.Size = new System.Drawing.Size(29, 20);
           this.Data3MTB.TabIndex = 17;
           this.Data3MTB.Text = "00";
           // 
           // Data2MTB
           // 
           this.Data2MTB.Location = new System.Drawing.Point(146, 87);
           this.Data2MTB.Mask = "CC";
           this.Data2MTB.Name = "Data2MTB";
           this.Data2MTB.PromptChar = ' ';
           this.Data2MTB.Size = new System.Drawing.Size(29, 20);
           this.Data2MTB.TabIndex = 16;
           this.Data2MTB.Text = "00";
           // 
           // Data1MTB
           // 
           this.Data1MTB.Location = new System.Drawing.Point(104, 87);
           this.Data1MTB.Mask = "CC";
           this.Data1MTB.Name = "Data1MTB";
           this.Data1MTB.PromptChar = ' ';
           this.Data1MTB.Size = new System.Drawing.Size(29, 20);
           this.Data1MTB.TabIndex = 15;
           this.Data1MTB.Text = "00";
           // 
           // Data0MTB
           // 
           this.Data0MTB.Location = new System.Drawing.Point(62, 87);
           this.Data0MTB.Mask = "CC";
           this.Data0MTB.Name = "Data0MTB";
           this.Data0MTB.PromptChar = ' ';
           this.Data0MTB.Size = new System.Drawing.Size(29, 20);
           this.Data0MTB.TabIndex = 14;
           this.Data0MTB.Text = "00";
           // 
           // DlcMTB
           // 
           this.DlcMTB.Location = new System.Drawing.Point(364, 25);
           this.DlcMTB.Mask = "CC";
           this.DlcMTB.Name = "DlcMTB";
           this.DlcMTB.PromptChar = ' ';
           this.DlcMTB.Size = new System.Drawing.Size(31, 20);
           this.DlcMTB.TabIndex = 4;
           this.DlcMTB.Text = "0";
           // 
           // IdentifierMTB
           // 
           this.IdentifierMTB.Location = new System.Drawing.Point(77, 25);
           this.IdentifierMTB.Mask = "CCCCCCCC";
           this.IdentifierMTB.Name = "IdentifierMTB";
           this.IdentifierMTB.PromptChar = ' ';
           this.IdentifierMTB.Size = new System.Drawing.Size(75, 20);
           this.IdentifierMTB.TabIndex = 1;
           this.IdentifierMTB.Text = "0";
           // 
           // MFMaskMTB
           // 
           this.MFMaskMTB.Location = new System.Drawing.Point(62, 25);
           this.MFMaskMTB.Mask = "CCCCCCCC";
           this.MFMaskMTB.Name = "MFMaskMTB";
           this.MFMaskMTB.PromptChar = ' ';
           this.MFMaskMTB.Size = new System.Drawing.Size(75, 20);
           this.MFMaskMTB.TabIndex = 4;
           this.MFMaskMTB.Text = "00000000";
           // 
           // MFCodeMTB
           // 
           this.MFCodeMTB.Location = new System.Drawing.Point(210, 25);
           this.MFCodeMTB.Mask = "CCCCCCCC";
           this.MFCodeMTB.Name = "MFCodeMTB";
           this.MFCodeMTB.PromptChar = ' ';
           this.MFCodeMTB.Size = new System.Drawing.Size(75, 20);
           this.MFCodeMTB.TabIndex = 0;
           this.MFCodeMTB.Text = "00000000";
           // 
           // AutoResponse
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.ClientSize = new System.Drawing.Size(809, 552);
           this.Controls.Add(this.label16);
           this.Controls.Add(this.ChannelNumMTB);
           this.Controls.Add(this.label15);
           this.Controls.Add(this.label3);
           this.Controls.Add(this.AutoConfigGB);
           this.Controls.Add(this.OutputRTB);
           this.Controls.Add(this.CloseChannelB);
           this.Controls.Add(this.BusOffB);
           this.Controls.Add(this.BusOnB);
           this.Controls.Add(this.InitChannelB);
           this.Name = "AutoResponse";
           this.Text = "Auto Response Example";
           this.AutoConfigGB.ResumeLayout(false);
           this.groupBox2.ResumeLayout(false);
           this.groupBox2.PerformLayout();
           this.groupBox1.ResumeLayout(false);
           this.groupBox1.PerformLayout();
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button InitChannelB;
        private System.Windows.Forms.Button BusOnB;
        private System.Windows.Forms.Button BusOffB;
        private System.Windows.Forms.Button CloseChannelB;
        private System.Windows.Forms.RichTextBox OutputRTB;
        private System.Windows.Forms.GroupBox AutoConfigGB;
        private System.Windows.Forms.Button CreateB;
        private System.Windows.Forms.Button UpdateContentsB;
        private System.Windows.Forms.Button RemoveB;
        private System.Windows.Forms.GroupBox groupBox1;
//        private System.Windows.Forms.MaskedTextBox MFCodeMTB;
        private HexTextBox MFCodeMTB;
       private System.Windows.Forms.CheckBox RTROnlyCB;
       private System.Windows.Forms.Label label2;
       private System.Windows.Forms.Label label1;
       private HexTextBox MFMaskMTB;
       private System.Windows.Forms.GroupBox groupBox2;
       private System.Windows.Forms.Label label3;
       private HexTextBox IdentifierMTB;
       private System.Windows.Forms.Label label4;
       private System.Windows.Forms.CheckBox ExtendedIdCB;
       private HexTextBox DlcMTB;
       private System.Windows.Forms.Label label5;
       private System.Windows.Forms.Label label7;
       private System.Windows.Forms.Label label6;
       private System.Windows.Forms.Label label14;
       private System.Windows.Forms.Label label13;
       private System.Windows.Forms.Label label12;
       private System.Windows.Forms.Label label11;
       private System.Windows.Forms.Label label10;
       private System.Windows.Forms.Label label9;
       private System.Windows.Forms.Label label8;
       private HexTextBox Data2MTB;
       private HexTextBox Data1MTB;
       private HexTextBox Data0MTB;
       private HexTextBox Data6MTB;
       private HexTextBox Data5MTB;
       private HexTextBox Data4MTB;
       private HexTextBox Data3MTB;
       private HexTextBox Data7MTB;
       private System.Windows.Forms.Label label15;
       private System.Windows.Forms.MaskedTextBox ChannelNumMTB;
       private System.Windows.Forms.Label label16;

    }
}

