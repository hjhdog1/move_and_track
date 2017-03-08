namespace KvaserHardwareTester
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            this.comboBoxInterface1 = new System.Windows.Forms.ComboBox();
            this.comboBoxInterface2 = new System.Windows.Forms.ComboBox();
            this.buttonInterface1BusOn = new System.Windows.Forms.Button();
            this.buttonInterface2GoOnBus = new System.Windows.Forms.Button();
            this.buttonTestAutoBuffers = new System.Windows.Forms.Button();
            this.textBoxResult = new System.Windows.Forms.TextBox();
            this.buttonSendWakeUp = new System.Windows.Forms.Button();
            this.timerRead = new System.Windows.Forms.Timer(this.components);
            this.buttonSendErrorframe = new System.Windows.Forms.Button();
            this.buttonStartTimer = new System.Windows.Forms.Button();
            this.buttonSendMessage = new System.Windows.Forms.Button();
            this.buttonGetBusConfig = new System.Windows.Forms.Button();
            this.textBoxIncommingFrames = new System.Windows.Forms.TextBox();
            this.textBoxCode = new System.Windows.Forms.TextBox();
            this.textBoxMask = new System.Windows.Forms.TextBox();
            this.buttonSetFilter = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.buttonGetChannelData = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.buttonReadTimer = new System.Windows.Forms.Button();
            this.buttonCanIOCTL = new System.Windows.Forms.Button();
            this.buttonRxqueueSize = new System.Windows.Forms.Button();
            this.radioButtonOverRun = new System.Windows.Forms.RadioButton();
            this.buttonReadOne = new System.Windows.Forms.Button();
            this.buttonClearIncomming = new System.Windows.Forms.Button();
            this.textBoxRxQueueSize = new System.Windows.Forms.TextBox();
            this.buttonReadSpecific = new System.Windows.Forms.Button();
            this.textBoxSpecificID = new System.Windows.Forms.TextBox();
            this.labelSpecificID = new System.Windows.Forms.Label();
            this.buttonReadSpecificSkip = new System.Windows.Forms.Button();
            this.buttonSilentMode = new System.Windows.Forms.Button();
            this.buttonGetBusStatistics = new System.Windows.Forms.Button();
            this.buttonMessageBurst = new System.Windows.Forms.Button();
            this.buttonReadErrorCounters = new System.Windows.Forms.Button();
            this.buttonInterface1BusOff = new System.Windows.Forms.Button();
            this.buttonSendRemoteFrame = new System.Windows.Forms.Button();
            this.buttonStartReadThread = new System.Windows.Forms.Button();
            this.buttonStopReadThread = new System.Windows.Forms.Button();
            this.buttonPingPongStart = new System.Windows.Forms.Button();
            this.buttonWaitForPingPong = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.panelIf1 = new System.Windows.Forms.Panel();
            this.panelIf2 = new System.Windows.Forms.Panel();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panelIf1.SuspendLayout();
            this.panelIf2.SuspendLayout();
            this.SuspendLayout();
            // 
            // comboBoxInterface1
            // 
            this.comboBoxInterface1.FormattingEnabled = true;
            this.comboBoxInterface1.Location = new System.Drawing.Point(14, 6);
            this.comboBoxInterface1.Name = "comboBoxInterface1";
            this.comboBoxInterface1.Size = new System.Drawing.Size(202, 21);
            this.comboBoxInterface1.TabIndex = 0;
            this.comboBoxInterface1.SelectedIndexChanged += new System.EventHandler(this.comboBoxInterface1_SelectedIndexChanged);
            // 
            // comboBoxInterface2
            // 
            this.comboBoxInterface2.FormattingEnabled = true;
            this.comboBoxInterface2.Location = new System.Drawing.Point(12, 7);
            this.comboBoxInterface2.Name = "comboBoxInterface2";
            this.comboBoxInterface2.Size = new System.Drawing.Size(218, 21);
            this.comboBoxInterface2.TabIndex = 1;
            this.comboBoxInterface2.SelectedIndexChanged += new System.EventHandler(this.comboBoxInterface2_SelectedIndexChanged);
            // 
            // buttonInterface1BusOn
            // 
            this.buttonInterface1BusOn.Location = new System.Drawing.Point(14, 62);
            this.buttonInterface1BusOn.Name = "buttonInterface1BusOn";
            this.buttonInterface1BusOn.Size = new System.Drawing.Size(115, 23);
            this.buttonInterface1BusOn.TabIndex = 2;
            this.buttonInterface1BusOn.Text = "Go BusOn";
            this.buttonInterface1BusOn.UseVisualStyleBackColor = true;
            this.buttonInterface1BusOn.Click += new System.EventHandler(this.ButtonInterface1GoOnBus_Click);
            // 
            // buttonInterface2GoOnBus
            // 
            this.buttonInterface2GoOnBus.Location = new System.Drawing.Point(12, 36);
            this.buttonInterface2GoOnBus.Name = "buttonInterface2GoOnBus";
            this.buttonInterface2GoOnBus.Size = new System.Drawing.Size(131, 23);
            this.buttonInterface2GoOnBus.TabIndex = 3;
            this.buttonInterface2GoOnBus.Text = "Go OnBus";
            this.buttonInterface2GoOnBus.UseVisualStyleBackColor = true;
            this.buttonInterface2GoOnBus.Click += new System.EventHandler(this.buttonInterface2GoOnBus_Click);
            // 
            // buttonTestAutoBuffers
            // 
            this.buttonTestAutoBuffers.Location = new System.Drawing.Point(14, 209);
            this.buttonTestAutoBuffers.Name = "buttonTestAutoBuffers";
            this.buttonTestAutoBuffers.Size = new System.Drawing.Size(117, 23);
            this.buttonTestAutoBuffers.TabIndex = 4;
            this.buttonTestAutoBuffers.Text = "Test autobuffers";
            this.buttonTestAutoBuffers.UseVisualStyleBackColor = true;
            this.buttonTestAutoBuffers.Click += new System.EventHandler(this.buttonTestAutoBuffers_Click);
            // 
            // textBoxResult
            // 
            this.textBoxResult.Location = new System.Drawing.Point(9, 554);
            this.textBoxResult.Multiline = true;
            this.textBoxResult.Name = "textBoxResult";
            this.textBoxResult.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBoxResult.Size = new System.Drawing.Size(684, 98);
            this.textBoxResult.TabIndex = 5;
            // 
            // buttonSendWakeUp
            // 
            this.buttonSendWakeUp.Location = new System.Drawing.Point(14, 180);
            this.buttonSendWakeUp.Name = "buttonSendWakeUp";
            this.buttonSendWakeUp.Size = new System.Drawing.Size(117, 23);
            this.buttonSendWakeUp.TabIndex = 7;
            this.buttonSendWakeUp.Text = "Send Wakeup";
            this.buttonSendWakeUp.UseVisualStyleBackColor = true;
            this.buttonSendWakeUp.Click += new System.EventHandler(this.buttonSendWakeUp_Click);
            // 
            // timerRead
            // 
            this.timerRead.Interval = 50;
            this.timerRead.Tick += new System.EventHandler(this.timerRead_Tick);
            // 
            // buttonSendErrorframe
            // 
            this.buttonSendErrorframe.Location = new System.Drawing.Point(14, 149);
            this.buttonSendErrorframe.Name = "buttonSendErrorframe";
            this.buttonSendErrorframe.Size = new System.Drawing.Size(116, 23);
            this.buttonSendErrorframe.TabIndex = 8;
            this.buttonSendErrorframe.Text = "Send Errorframe";
            this.buttonSendErrorframe.UseVisualStyleBackColor = true;
            this.buttonSendErrorframe.Click += new System.EventHandler(this.buttonSendErrorframe_Click);
            // 
            // buttonStartTimer
            // 
            this.buttonStartTimer.Location = new System.Drawing.Point(12, 190);
            this.buttonStartTimer.Name = "buttonStartTimer";
            this.buttonStartTimer.Size = new System.Drawing.Size(131, 23);
            this.buttonStartTimer.TabIndex = 9;
            this.buttonStartTimer.Text = "Start readtimer";
            this.buttonStartTimer.UseVisualStyleBackColor = true;
            this.buttonStartTimer.Click += new System.EventHandler(this.buttonStartTimerRead_Click);
            // 
            // buttonSendMessage
            // 
            this.buttonSendMessage.Location = new System.Drawing.Point(14, 119);
            this.buttonSendMessage.Name = "buttonSendMessage";
            this.buttonSendMessage.Size = new System.Drawing.Size(115, 23);
            this.buttonSendMessage.TabIndex = 10;
            this.buttonSendMessage.Text = "Send Message";
            this.buttonSendMessage.UseVisualStyleBackColor = true;
            this.buttonSendMessage.Click += new System.EventHandler(this.buttonSendMessage_Click);
            // 
            // buttonGetBusConfig
            // 
            this.buttonGetBusConfig.Location = new System.Drawing.Point(14, 238);
            this.buttonGetBusConfig.Name = "buttonGetBusConfig";
            this.buttonGetBusConfig.Size = new System.Drawing.Size(115, 23);
            this.buttonGetBusConfig.TabIndex = 11;
            this.buttonGetBusConfig.Text = "Get busconfig";
            this.buttonGetBusConfig.UseVisualStyleBackColor = true;
            this.buttonGetBusConfig.Click += new System.EventHandler(this.buttonGetBusConfig_Click);
            // 
            // textBoxIncommingFrames
            // 
            this.textBoxIncommingFrames.Location = new System.Drawing.Point(153, 161);
            this.textBoxIncommingFrames.Multiline = true;
            this.textBoxIncommingFrames.Name = "textBoxIncommingFrames";
            this.textBoxIncommingFrames.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBoxIncommingFrames.Size = new System.Drawing.Size(242, 202);
            this.textBoxIncommingFrames.TabIndex = 12;
            this.textBoxIncommingFrames.Text = "\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n";
            // 
            // textBoxCode
            // 
            this.textBoxCode.Location = new System.Drawing.Point(153, 117);
            this.textBoxCode.Name = "textBoxCode";
            this.textBoxCode.Size = new System.Drawing.Size(119, 20);
            this.textBoxCode.TabIndex = 13;
            // 
            // textBoxMask
            // 
            this.textBoxMask.Location = new System.Drawing.Point(278, 117);
            this.textBoxMask.Name = "textBoxMask";
            this.textBoxMask.Size = new System.Drawing.Size(117, 20);
            this.textBoxMask.TabIndex = 14;
            // 
            // buttonSetFilter
            // 
            this.buttonSetFilter.Location = new System.Drawing.Point(12, 117);
            this.buttonSetFilter.Name = "buttonSetFilter";
            this.buttonSetFilter.Size = new System.Drawing.Size(131, 23);
            this.buttonSetFilter.TabIndex = 15;
            this.buttonSetFilter.Text = "Set filter";
            this.buttonSetFilter.UseVisualStyleBackColor = true;
            this.buttonSetFilter.Click += new System.EventHandler(this.buttonSetFilter_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(153, 98);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 13);
            this.label1.TabIndex = 16;
            this.label1.Text = "Code";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(275, 98);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(33, 13);
            this.label2.TabIndex = 17;
            this.label2.Text = "Mask";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(153, 142);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(84, 13);
            this.label3.TabIndex = 18;
            this.label3.Text = "Incoming frames";
            // 
            // buttonGetChannelData
            // 
            this.buttonGetChannelData.Location = new System.Drawing.Point(14, 267);
            this.buttonGetChannelData.Name = "buttonGetChannelData";
            this.buttonGetChannelData.Size = new System.Drawing.Size(115, 23);
            this.buttonGetChannelData.TabIndex = 19;
            this.buttonGetChannelData.Text = "Get Ch.data";
            this.buttonGetChannelData.UseVisualStyleBackColor = true;
            this.buttonGetChannelData.Click += new System.EventHandler(this.buttonGetChannelData_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(10, 538);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(39, 13);
            this.label4.TabIndex = 20;
            this.label4.Text = "Output";
            // 
            // buttonReadTimer
            // 
            this.buttonReadTimer.Location = new System.Drawing.Point(14, 296);
            this.buttonReadTimer.Name = "buttonReadTimer";
            this.buttonReadTimer.Size = new System.Drawing.Size(115, 23);
            this.buttonReadTimer.TabIndex = 23;
            this.buttonReadTimer.Text = "ReadTimer";
            this.buttonReadTimer.UseVisualStyleBackColor = true;
            this.buttonReadTimer.Click += new System.EventHandler(this.buttonReadTimer_Click);
            // 
            // buttonCanIOCTL
            // 
            this.buttonCanIOCTL.Location = new System.Drawing.Point(14, 325);
            this.buttonCanIOCTL.Name = "buttonCanIOCTL";
            this.buttonCanIOCTL.Size = new System.Drawing.Size(115, 23);
            this.buttonCanIOCTL.TabIndex = 25;
            this.buttonCanIOCTL.Text = "CanIOCtl";
            this.buttonCanIOCTL.UseVisualStyleBackColor = true;
            this.buttonCanIOCTL.Click += new System.EventHandler(this.buttonCanIOCTL_Click);
            // 
            // buttonRxqueueSize
            // 
            this.buttonRxqueueSize.Location = new System.Drawing.Point(14, 32);
            this.buttonRxqueueSize.Name = "buttonRxqueueSize";
            this.buttonRxqueueSize.Size = new System.Drawing.Size(117, 23);
            this.buttonRxqueueSize.TabIndex = 26;
            this.buttonRxqueueSize.Text = "RxqueueSize";
            this.buttonRxqueueSize.UseVisualStyleBackColor = true;
            this.buttonRxqueueSize.Click += new System.EventHandler(this.buttonRxqueueSize_Click);
            // 
            // radioButtonOverRun
            // 
            this.radioButtonOverRun.AutoSize = true;
            this.radioButtonOverRun.Location = new System.Drawing.Point(153, 369);
            this.radioButtonOverRun.Name = "radioButtonOverRun";
            this.radioButtonOverRun.Size = new System.Drawing.Size(63, 17);
            this.radioButtonOverRun.TabIndex = 27;
            this.radioButtonOverRun.TabStop = true;
            this.radioButtonOverRun.Text = "Overrun";
            this.radioButtonOverRun.UseVisualStyleBackColor = true;
            // 
            // buttonReadOne
            // 
            this.buttonReadOne.Location = new System.Drawing.Point(12, 161);
            this.buttonReadOne.Name = "buttonReadOne";
            this.buttonReadOne.Size = new System.Drawing.Size(131, 23);
            this.buttonReadOne.TabIndex = 28;
            this.buttonReadOne.Text = "Read one";
            this.buttonReadOne.UseVisualStyleBackColor = true;
            this.buttonReadOne.Click += new System.EventHandler(this.buttonReadOne_Click);
            // 
            // buttonClearIncomming
            // 
            this.buttonClearIncomming.Location = new System.Drawing.Point(12, 340);
            this.buttonClearIncomming.Name = "buttonClearIncomming";
            this.buttonClearIncomming.Size = new System.Drawing.Size(131, 23);
            this.buttonClearIncomming.TabIndex = 30;
            this.buttonClearIncomming.Text = "Clear";
            this.buttonClearIncomming.UseVisualStyleBackColor = true;
            this.buttonClearIncomming.Click += new System.EventHandler(this.buttonClearIncomming_Click);
            // 
            // textBoxRxQueueSize
            // 
            this.textBoxRxQueueSize.Location = new System.Drawing.Point(139, 35);
            this.textBoxRxQueueSize.Name = "textBoxRxQueueSize";
            this.textBoxRxQueueSize.Size = new System.Drawing.Size(77, 20);
            this.textBoxRxQueueSize.TabIndex = 31;
            this.textBoxRxQueueSize.Text = "20";
            // 
            // buttonReadSpecific
            // 
            this.buttonReadSpecific.Location = new System.Drawing.Point(3, 29);
            this.buttonReadSpecific.Name = "buttonReadSpecific";
            this.buttonReadSpecific.Size = new System.Drawing.Size(124, 23);
            this.buttonReadSpecific.TabIndex = 32;
            this.buttonReadSpecific.Text = "Read specific";
            this.buttonReadSpecific.UseVisualStyleBackColor = true;
            this.buttonReadSpecific.Click += new System.EventHandler(this.buttonReadSpecific_Click);
            // 
            // textBoxSpecificID
            // 
            this.textBoxSpecificID.Location = new System.Drawing.Point(27, 4);
            this.textBoxSpecificID.Name = "textBoxSpecificID";
            this.textBoxSpecificID.Size = new System.Drawing.Size(100, 20);
            this.textBoxSpecificID.TabIndex = 33;
            this.textBoxSpecificID.Text = "123";
            // 
            // labelSpecificID
            // 
            this.labelSpecificID.AutoSize = true;
            this.labelSpecificID.Location = new System.Drawing.Point(3, 7);
            this.labelSpecificID.Name = "labelSpecificID";
            this.labelSpecificID.Size = new System.Drawing.Size(18, 13);
            this.labelSpecificID.TabIndex = 34;
            this.labelSpecificID.Text = "ID";
            // 
            // buttonReadSpecificSkip
            // 
            this.buttonReadSpecificSkip.Location = new System.Drawing.Point(3, 58);
            this.buttonReadSpecificSkip.Name = "buttonReadSpecificSkip";
            this.buttonReadSpecificSkip.Size = new System.Drawing.Size(124, 23);
            this.buttonReadSpecificSkip.TabIndex = 35;
            this.buttonReadSpecificSkip.Text = "ReadSpecificSkip";
            this.buttonReadSpecificSkip.UseVisualStyleBackColor = true;
            this.buttonReadSpecificSkip.Click += new System.EventHandler(this.buttonReadSpecificSkip_Click);
            // 
            // buttonSilentMode
            // 
            this.buttonSilentMode.Location = new System.Drawing.Point(14, 355);
            this.buttonSilentMode.Name = "buttonSilentMode";
            this.buttonSilentMode.Size = new System.Drawing.Size(116, 23);
            this.buttonSilentMode.TabIndex = 36;
            this.buttonSilentMode.Text = "Set silent mode";
            this.buttonSilentMode.UseVisualStyleBackColor = true;
            this.buttonSilentMode.Click += new System.EventHandler(this.buttonSilentMode_Click);
            // 
            // buttonGetBusStatistics
            // 
            this.buttonGetBusStatistics.Location = new System.Drawing.Point(14, 384);
            this.buttonGetBusStatistics.Name = "buttonGetBusStatistics";
            this.buttonGetBusStatistics.Size = new System.Drawing.Size(116, 23);
            this.buttonGetBusStatistics.TabIndex = 37;
            this.buttonGetBusStatistics.Text = "Bus Statistics";
            this.buttonGetBusStatistics.UseVisualStyleBackColor = true;
            this.buttonGetBusStatistics.Click += new System.EventHandler(this.buttonGetBusStatistics_Click);
            // 
            // buttonMessageBurst
            // 
            this.buttonMessageBurst.Location = new System.Drawing.Point(14, 414);
            this.buttonMessageBurst.Name = "buttonMessageBurst";
            this.buttonMessageBurst.Size = new System.Drawing.Size(116, 23);
            this.buttonMessageBurst.TabIndex = 38;
            this.buttonMessageBurst.Text = "Send burst";
            this.buttonMessageBurst.UseVisualStyleBackColor = true;
            this.buttonMessageBurst.Click += new System.EventHandler(this.buttonMessageBurst_Click);
            // 
            // buttonReadErrorCounters
            // 
            this.buttonReadErrorCounters.Location = new System.Drawing.Point(14, 443);
            this.buttonReadErrorCounters.Name = "buttonReadErrorCounters";
            this.buttonReadErrorCounters.Size = new System.Drawing.Size(115, 23);
            this.buttonReadErrorCounters.TabIndex = 39;
            this.buttonReadErrorCounters.Text = "Read error counters";
            this.buttonReadErrorCounters.UseVisualStyleBackColor = true;
            this.buttonReadErrorCounters.Click += new System.EventHandler(this.buttonReadErrorCounters_Click);
            // 
            // buttonInterface1BusOff
            // 
            this.buttonInterface1BusOff.Location = new System.Drawing.Point(14, 91);
            this.buttonInterface1BusOff.Name = "buttonInterface1BusOff";
            this.buttonInterface1BusOff.Size = new System.Drawing.Size(115, 23);
            this.buttonInterface1BusOff.TabIndex = 40;
            this.buttonInterface1BusOff.Text = "Go BusOff";
            this.buttonInterface1BusOff.UseVisualStyleBackColor = true;
            this.buttonInterface1BusOff.Click += new System.EventHandler(this.buttonInterface1BusOff_Click);
            // 
            // buttonSendRemoteFrame
            // 
            this.buttonSendRemoteFrame.Location = new System.Drawing.Point(139, 62);
            this.buttonSendRemoteFrame.Name = "buttonSendRemoteFrame";
            this.buttonSendRemoteFrame.Size = new System.Drawing.Size(110, 23);
            this.buttonSendRemoteFrame.TabIndex = 41;
            this.buttonSendRemoteFrame.Text = "Send remote frames";
            this.buttonSendRemoteFrame.UseVisualStyleBackColor = true;
            this.buttonSendRemoteFrame.Click += new System.EventHandler(this.buttonSendRemoteFrame_Click);
            // 
            // buttonStartReadThread
            // 
            this.buttonStartReadThread.Location = new System.Drawing.Point(12, 399);
            this.buttonStartReadThread.Name = "buttonStartReadThread";
            this.buttonStartReadThread.Size = new System.Drawing.Size(131, 23);
            this.buttonStartReadThread.TabIndex = 42;
            this.buttonStartReadThread.Text = "Start Readthread";
            this.buttonStartReadThread.UseVisualStyleBackColor = true;
            this.buttonStartReadThread.Click += new System.EventHandler(this.buttonStartReadThread_Click);
            // 
            // buttonStopReadThread
            // 
            this.buttonStopReadThread.Location = new System.Drawing.Point(12, 428);
            this.buttonStopReadThread.Name = "buttonStopReadThread";
            this.buttonStopReadThread.Size = new System.Drawing.Size(131, 23);
            this.buttonStopReadThread.TabIndex = 43;
            this.buttonStopReadThread.Text = "Stop Readthread";
            this.buttonStopReadThread.UseVisualStyleBackColor = true;
            this.buttonStopReadThread.Click += new System.EventHandler(this.buttonStopReadThread_Click);
            // 
            // buttonPingPongStart
            // 
            this.buttonPingPongStart.Location = new System.Drawing.Point(139, 91);
            this.buttonPingPongStart.Name = "buttonPingPongStart";
            this.buttonPingPongStart.Size = new System.Drawing.Size(110, 23);
            this.buttonPingPongStart.TabIndex = 44;
            this.buttonPingPongStart.Text = "Start pingpong";
            this.buttonPingPongStart.UseVisualStyleBackColor = true;
            this.buttonPingPongStart.Click += new System.EventHandler(this.buttonPingPongStart_Click);
            // 
            // buttonWaitForPingPong
            // 
            this.buttonWaitForPingPong.Location = new System.Drawing.Point(139, 119);
            this.buttonWaitForPingPong.Name = "buttonWaitForPingPong";
            this.buttonWaitForPingPong.Size = new System.Drawing.Size(110, 23);
            this.buttonWaitForPingPong.TabIndex = 45;
            this.buttonWaitForPingPong.Text = "Wait for pingpong";
            this.buttonWaitForPingPong.UseVisualStyleBackColor = true;
            this.buttonWaitForPingPong.Click += new System.EventHandler(this.buttonWaitForPingPong_Click);
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Controls.Add(this.buttonReadSpecific);
            this.panel1.Controls.Add(this.buttonReadSpecificSkip);
            this.panel1.Controls.Add(this.textBoxSpecificID);
            this.panel1.Controls.Add(this.labelSpecificID);
            this.panel1.Location = new System.Drawing.Point(12, 220);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(135, 100);
            this.panel1.TabIndex = 46;
            // 
            // panel2
            // 
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel2.Controls.Add(this.button1);
            this.panel2.Controls.Add(this.button2);
            this.panel2.Controls.Add(this.textBox1);
            this.panel2.Controls.Add(this.label5);
            this.panel2.Location = new System.Drawing.Point(-1, -1);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(135, 100);
            this.panel2.TabIndex = 47;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(3, 29);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(124, 23);
            this.button1.TabIndex = 32;
            this.button1.Text = "Read specific";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(3, 58);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(124, 23);
            this.button2.TabIndex = 35;
            this.button2.Text = "ReadSpecificSkip";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(27, 4);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(100, 20);
            this.textBox1.TabIndex = 33;
            this.textBox1.Text = "123";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(3, 7);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(18, 13);
            this.label5.TabIndex = 34;
            this.label5.Text = "ID";
            // 
            // panelIf1
            // 
            this.panelIf1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelIf1.Controls.Add(this.comboBoxInterface1);
            this.panelIf1.Controls.Add(this.buttonInterface1BusOn);
            this.panelIf1.Controls.Add(this.buttonWaitForPingPong);
            this.panelIf1.Controls.Add(this.buttonTestAutoBuffers);
            this.panelIf1.Controls.Add(this.buttonPingPongStart);
            this.panelIf1.Controls.Add(this.buttonSendWakeUp);
            this.panelIf1.Controls.Add(this.buttonSendErrorframe);
            this.panelIf1.Controls.Add(this.buttonSendMessage);
            this.panelIf1.Controls.Add(this.buttonSendRemoteFrame);
            this.panelIf1.Controls.Add(this.buttonGetBusConfig);
            this.panelIf1.Controls.Add(this.buttonInterface1BusOff);
            this.panelIf1.Controls.Add(this.buttonGetChannelData);
            this.panelIf1.Controls.Add(this.buttonReadErrorCounters);
            this.panelIf1.Controls.Add(this.buttonReadTimer);
            this.panelIf1.Controls.Add(this.buttonMessageBurst);
            this.panelIf1.Controls.Add(this.buttonCanIOCTL);
            this.panelIf1.Controls.Add(this.buttonGetBusStatistics);
            this.panelIf1.Controls.Add(this.buttonRxqueueSize);
            this.panelIf1.Controls.Add(this.buttonSilentMode);
            this.panelIf1.Controls.Add(this.textBoxRxQueueSize);
            this.panelIf1.Location = new System.Drawing.Point(9, 12);
            this.panelIf1.Name = "panelIf1";
            this.panelIf1.Size = new System.Drawing.Size(264, 501);
            this.panelIf1.TabIndex = 47;
            // 
            // panelIf2
            // 
            this.panelIf2.Controls.Add(this.buttonSetFilter);
            this.panelIf2.Controls.Add(this.buttonStartTimer);
            this.panelIf2.Controls.Add(this.panel1);
            this.panelIf2.Controls.Add(this.textBoxIncommingFrames);
            this.panelIf2.Controls.Add(this.buttonInterface2GoOnBus);
            this.panelIf2.Controls.Add(this.buttonStopReadThread);
            this.panelIf2.Controls.Add(this.comboBoxInterface2);
            this.panelIf2.Controls.Add(this.textBoxCode);
            this.panelIf2.Controls.Add(this.buttonStartReadThread);
            this.panelIf2.Controls.Add(this.textBoxMask);
            this.panelIf2.Controls.Add(this.buttonClearIncomming);
            this.panelIf2.Controls.Add(this.label1);
            this.panelIf2.Controls.Add(this.buttonReadOne);
            this.panelIf2.Controls.Add(this.label2);
            this.panelIf2.Controls.Add(this.radioButtonOverRun);
            this.panelIf2.Controls.Add(this.label3);
            this.panelIf2.Location = new System.Drawing.Point(279, 12);
            this.panelIf2.Name = "panelIf2";
            this.panelIf2.Size = new System.Drawing.Size(414, 501);
            this.panelIf2.TabIndex = 48;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(714, 698);
            this.Controls.Add(this.panelIf2);
            this.Controls.Add(this.panelIf1);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBoxResult);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panelIf1.ResumeLayout(false);
            this.panelIf1.PerformLayout();
            this.panelIf2.ResumeLayout(false);
            this.panelIf2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox comboBoxInterface1;
        private System.Windows.Forms.ComboBox comboBoxInterface2;
        private System.Windows.Forms.Button buttonInterface1BusOn;
        private System.Windows.Forms.Button buttonInterface2GoOnBus;
        private System.Windows.Forms.Button buttonTestAutoBuffers;
        private System.Windows.Forms.TextBox textBoxResult;
        private System.Windows.Forms.Button buttonSendWakeUp;
        private System.Windows.Forms.Timer timerRead;
        private System.Windows.Forms.Button buttonSendErrorframe;
        private System.Windows.Forms.Button buttonStartTimer;
        private System.Windows.Forms.Button buttonSendMessage;
        private System.Windows.Forms.Button buttonGetBusConfig;
        private System.Windows.Forms.TextBox textBoxIncommingFrames;
        private System.Windows.Forms.TextBox textBoxCode;
        private System.Windows.Forms.TextBox textBoxMask;
        private System.Windows.Forms.Button buttonSetFilter;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button buttonGetChannelData;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button buttonReadTimer;
        private System.Windows.Forms.Button buttonCanIOCTL;
        private System.Windows.Forms.Button buttonRxqueueSize;
        private System.Windows.Forms.RadioButton radioButtonOverRun;
        private System.Windows.Forms.Button buttonReadOne;
        private System.Windows.Forms.Button buttonClearIncomming;
        private System.Windows.Forms.TextBox textBoxRxQueueSize;
        private System.Windows.Forms.Button buttonReadSpecific;
        private System.Windows.Forms.TextBox textBoxSpecificID;
        private System.Windows.Forms.Label labelSpecificID;
        private System.Windows.Forms.Button buttonReadSpecificSkip;
        private System.Windows.Forms.Button buttonSilentMode;
        private System.Windows.Forms.Button buttonGetBusStatistics;
        private System.Windows.Forms.Button buttonMessageBurst;
        private System.Windows.Forms.Button buttonReadErrorCounters;
        private System.Windows.Forms.Button buttonInterface1BusOff;
        private System.Windows.Forms.Button buttonSendRemoteFrame;
        private System.Windows.Forms.Button buttonStartReadThread;
        private System.Windows.Forms.Button buttonStopReadThread;
        private System.Windows.Forms.Button buttonPingPongStart;
        private System.Windows.Forms.Button buttonWaitForPingPong;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Panel panelIf1;
        private System.Windows.Forms.Panel panelIf2;
    }
}

