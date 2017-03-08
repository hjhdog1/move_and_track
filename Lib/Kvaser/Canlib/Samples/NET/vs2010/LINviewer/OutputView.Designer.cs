namespace LinViewer
{
   partial class OutputView
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
         this.OutputRTB = new System.Windows.Forms.RichTextBox();
         this.HeaderLabel = new System.Windows.Forms.Label();
         this.RTBContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
         this.CopyAllToClipBoard = new System.Windows.Forms.ToolStripMenuItem();
         this.ClearAll = new System.Windows.Forms.ToolStripMenuItem();
         this.RTBContextMenu.SuspendLayout();
         this.SuspendLayout();
         // 
         // OutputRTB
         // 
         this.OutputRTB.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.OutputRTB.ContextMenuStrip = this.RTBContextMenu;
         this.OutputRTB.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.OutputRTB.Location = new System.Drawing.Point(-1, 30);
         this.OutputRTB.Name = "OutputRTB";
         this.OutputRTB.ReadOnly = true;
         this.OutputRTB.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedBoth;
         this.OutputRTB.Size = new System.Drawing.Size(742, 224);
         this.OutputRTB.TabIndex = 0;
         this.OutputRTB.TabStop = false;
         this.OutputRTB.Text = "";
         this.OutputRTB.WordWrap = false;
         // 
         // HeaderLabel
         // 
         this.HeaderLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.HeaderLabel.AutoSize = true;
         this.HeaderLabel.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.HeaderLabel.Location = new System.Drawing.Point(4, 11);
         this.HeaderLabel.Name = "HeaderLabel";
         this.HeaderLabel.Size = new System.Drawing.Size(204, 16);
         this.HeaderLabel.TabIndex = 1;
         this.HeaderLabel.Text = "Time   Identifier   DLC Data";
         // 
         // RTBContextMenu
         // 
         this.RTBContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.CopyAllToClipBoard,
            this.ClearAll});
         this.RTBContextMenu.Name = "RTBContextMenu";
         this.RTBContextMenu.ShowImageMargin = false;
         this.RTBContextMenu.Size = new System.Drawing.Size(150, 70);
         // 
         // CopyAllToClipBoard
         // 
         this.CopyAllToClipBoard.Name = "CopyAllToClipBoard";
         this.CopyAllToClipBoard.Size = new System.Drawing.Size(149, 22);
         this.CopyAllToClipBoard.Text = "Copy To Clipboard";
         this.CopyAllToClipBoard.Click += new System.EventHandler(this.CopyAllToClipBoard_Click);
         // 
         // ClearAll
         // 
         this.ClearAll.Name = "ClearAll";
         this.ClearAll.Size = new System.Drawing.Size(149, 22);
         this.ClearAll.Text = "Clear";
         this.ClearAll.Click += new System.EventHandler(this.ClearAll_Click);
         // 
         // OutputView
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(742, 254);
         this.Controls.Add(this.HeaderLabel);
         this.Controls.Add(this.OutputRTB);
         this.Name = "OutputView";
         this.ShowIcon = false;
         this.ShowInTaskbar = false;
         this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
         this.Text = "Output";
         this.RTBContextMenu.ResumeLayout(false);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.RichTextBox OutputRTB;
      private System.Windows.Forms.Label HeaderLabel;
      private System.Windows.Forms.ContextMenuStrip RTBContextMenu;
      private System.Windows.Forms.ToolStripMenuItem CopyAllToClipBoard;
      private System.Windows.Forms.ToolStripMenuItem ClearAll;

   }
}