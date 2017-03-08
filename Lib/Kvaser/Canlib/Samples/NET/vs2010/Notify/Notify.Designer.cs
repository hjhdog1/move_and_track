namespace NotifyTest
{
    partial class Notify
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
            this.ConfigChannel = new System.Windows.Forms.Button();
            this.CloseChannel = new System.Windows.Forms.Button();
            this.RxMsgsTbox = new System.Windows.Forms.TextBox();
            this.OutputLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // ConfigChannel
            // 
            this.ConfigChannel.Location = new System.Drawing.Point(32, 34);
            this.ConfigChannel.Name = "ConfigChannel";
            this.ConfigChannel.Size = new System.Drawing.Size(115, 33);
            this.ConfigChannel.TabIndex = 0;
            this.ConfigChannel.Text = "Open Channel";
            this.ConfigChannel.UseVisualStyleBackColor = true;
            this.ConfigChannel.Click += new System.EventHandler(this.ConfigChannel_Click);
            // 
            // CloseChannel
            // 
            this.CloseChannel.Location = new System.Drawing.Point(545, 34);
            this.CloseChannel.Name = "CloseChannel";
            this.CloseChannel.Size = new System.Drawing.Size(115, 33);
            this.CloseChannel.TabIndex = 1;
            this.CloseChannel.Text = "Close Channel";
            this.CloseChannel.UseVisualStyleBackColor = true;
            this.CloseChannel.Click += new System.EventHandler(this.CloseChannel_Click);
            // 
            // RxMsgsTbox
            // 
            this.RxMsgsTbox.Font = new System.Drawing.Font("Courier New", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.RxMsgsTbox.Location = new System.Drawing.Point(32, 134);
            this.RxMsgsTbox.Multiline = true;
            this.RxMsgsTbox.Name = "RxMsgsTbox";
            this.RxMsgsTbox.ReadOnly = true;
            this.RxMsgsTbox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.RxMsgsTbox.Size = new System.Drawing.Size(628, 203);
            this.RxMsgsTbox.TabIndex = 2;
            this.RxMsgsTbox.WordWrap = false;
            // 
            // OutputLabel
            // 
            this.OutputLabel.AutoSize = true;
            this.OutputLabel.Font = new System.Drawing.Font("Courier New", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.OutputLabel.Location = new System.Drawing.Point(29, 116);
            this.OutputLabel.Name = "OutputLabel";
            this.OutputLabel.Size = new System.Drawing.Size(618, 18);
            this.OutputLabel.TabIndex = 3;
            this.OutputLabel.Text = "   ID    Flag DLC  Data                             Timestamp";
            // 
            // Notify
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(693, 357);
            this.Controls.Add(this.OutputLabel);
            this.Controls.Add(this.RxMsgsTbox);
            this.Controls.Add(this.CloseChannel);
            this.Controls.Add(this.ConfigChannel);
            this.Name = "Notify";
            this.Text = "Test Notify";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button ConfigChannel;
        private System.Windows.Forms.Button CloseChannel;
        private System.Windows.Forms.TextBox RxMsgsTbox;
        private System.Windows.Forms.Label OutputLabel;
    }
}

