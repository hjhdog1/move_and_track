<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Transmitter
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
      Me.IdGroupbox = New System.Windows.Forms.GroupBox
      Me.ExtendedRB = New System.Windows.Forms.RadioButton
      Me.StandardRB = New System.Windows.Forms.RadioButton
      Me.CanIdTB = New System.Windows.Forms.MaskedTextBox
      Me.DlcTB = New System.Windows.Forms.MaskedTextBox
      Me.Label1 = New System.Windows.Forms.Label
      Me.Label2 = New System.Windows.Forms.Label
      Me.Label3 = New System.Windows.Forms.Label
      Me.Label4 = New System.Windows.Forms.Label
      Me.Label5 = New System.Windows.Forms.Label
      Me.Label6 = New System.Windows.Forms.Label
      Me.Label7 = New System.Windows.Forms.Label
      Me.Label8 = New System.Windows.Forms.Label
      Me.Label9 = New System.Windows.Forms.Label
      Me.DataTB1 = New System.Windows.Forms.MaskedTextBox
      Me.DataTB2 = New System.Windows.Forms.MaskedTextBox
      Me.DataTB3 = New System.Windows.Forms.MaskedTextBox
      Me.DataTB4 = New System.Windows.Forms.MaskedTextBox
      Me.DataTB5 = New System.Windows.Forms.MaskedTextBox
      Me.DataTB6 = New System.Windows.Forms.MaskedTextBox
      Me.DataTB7 = New System.Windows.Forms.MaskedTextBox
      Me.DataTB8 = New System.Windows.Forms.MaskedTextBox
      Me.SendButton = New System.Windows.Forms.Button
      Me.IdGroupbox.SuspendLayout()
      Me.SuspendLayout()
      '
      'IdGroupbox
      '
      Me.IdGroupbox.BackColor = System.Drawing.SystemColors.Control
      Me.IdGroupbox.Controls.Add(Me.ExtendedRB)
      Me.IdGroupbox.Controls.Add(Me.StandardRB)
      Me.IdGroupbox.Controls.Add(Me.CanIdTB)
      Me.IdGroupbox.Location = New System.Drawing.Point(29, 12)
      Me.IdGroupbox.Name = "IdGroupbox"
      Me.IdGroupbox.Size = New System.Drawing.Size(227, 75)
      Me.IdGroupbox.TabIndex = 0
      Me.IdGroupbox.TabStop = False
      Me.IdGroupbox.Text = "CAN Identifier"
      '
      'ExtendedRB
      '
      Me.ExtendedRB.AutoSize = True
      Me.ExtendedRB.Location = New System.Drawing.Point(126, 46)
      Me.ExtendedRB.Name = "ExtendedRB"
      Me.ExtendedRB.Size = New System.Drawing.Size(70, 17)
      Me.ExtendedRB.TabIndex = 2
      Me.ExtendedRB.TabStop = True
      Me.ExtendedRB.Text = "Extended"
      Me.ExtendedRB.UseVisualStyleBackColor = True
      '
      'StandardRB
      '
      Me.StandardRB.AutoSize = True
      Me.StandardRB.Location = New System.Drawing.Point(31, 46)
      Me.StandardRB.Name = "StandardRB"
      Me.StandardRB.Size = New System.Drawing.Size(68, 17)
      Me.StandardRB.TabIndex = 1
      Me.StandardRB.TabStop = True
      Me.StandardRB.Text = "Standard"
      Me.StandardRB.UseVisualStyleBackColor = True
      '
      'CanIdTB
      '
      Me.CanIdTB.Location = New System.Drawing.Point(31, 20)
      Me.CanIdTB.Mask = "999999990"
      Me.CanIdTB.Name = "CanIdTB"
      Me.CanIdTB.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.CanIdTB.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.CanIdTB.Size = New System.Drawing.Size(165, 20)
      Me.CanIdTB.TabIndex = 0
      Me.CanIdTB.Text = "0"
      '
      'DlcTB
      '
      Me.DlcTB.Location = New System.Drawing.Point(137, 98)
      Me.DlcTB.Mask = "0"
      Me.DlcTB.Name = "DlcTB"
      Me.DlcTB.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DlcTB.Size = New System.Drawing.Size(44, 20)
      Me.DlcTB.TabIndex = 1
      Me.DlcTB.Text = "0"
      '
      'Label1
      '
      Me.Label1.AutoSize = True
      Me.Label1.Location = New System.Drawing.Point(103, 102)
      Me.Label1.Name = "Label1"
      Me.Label1.Size = New System.Drawing.Size(28, 13)
      Me.Label1.TabIndex = 2
      Me.Label1.Text = "DLC"
      '
      'Label2
      '
      Me.Label2.AutoSize = True
      Me.Label2.Location = New System.Drawing.Point(35, 130)
      Me.Label2.Name = "Label2"
      Me.Label2.Size = New System.Drawing.Size(37, 13)
      Me.Label2.TabIndex = 3
      Me.Label2.Text = "Byte 1"
      '
      'Label3
      '
      Me.Label3.AutoSize = True
      Me.Label3.Location = New System.Drawing.Point(35, 157)
      Me.Label3.Name = "Label3"
      Me.Label3.Size = New System.Drawing.Size(37, 13)
      Me.Label3.TabIndex = 4
      Me.Label3.Text = "Byte 2"
      '
      'Label4
      '
      Me.Label4.AutoSize = True
      Me.Label4.Location = New System.Drawing.Point(35, 184)
      Me.Label4.Name = "Label4"
      Me.Label4.Size = New System.Drawing.Size(37, 13)
      Me.Label4.TabIndex = 5
      Me.Label4.Text = "Byte 3"
      '
      'Label5
      '
      Me.Label5.AutoSize = True
      Me.Label5.Location = New System.Drawing.Point(35, 211)
      Me.Label5.Name = "Label5"
      Me.Label5.Size = New System.Drawing.Size(37, 13)
      Me.Label5.TabIndex = 6
      Me.Label5.Text = "Byte 4"
      '
      'Label6
      '
      Me.Label6.AutoSize = True
      Me.Label6.Location = New System.Drawing.Point(168, 157)
      Me.Label6.Name = "Label6"
      Me.Label6.Size = New System.Drawing.Size(37, 13)
      Me.Label6.TabIndex = 7
      Me.Label6.Text = "Byte 6"
      '
      'Label7
      '
      Me.Label7.AutoSize = True
      Me.Label7.Location = New System.Drawing.Point(168, 184)
      Me.Label7.Name = "Label7"
      Me.Label7.Size = New System.Drawing.Size(37, 13)
      Me.Label7.TabIndex = 8
      Me.Label7.Text = "Byte 7"
      '
      'Label8
      '
      Me.Label8.AutoSize = True
      Me.Label8.Location = New System.Drawing.Point(168, 211)
      Me.Label8.Name = "Label8"
      Me.Label8.Size = New System.Drawing.Size(37, 13)
      Me.Label8.TabIndex = 9
      Me.Label8.Text = "Byte 8"
      '
      'Label9
      '
      Me.Label9.AutoSize = True
      Me.Label9.Location = New System.Drawing.Point(168, 130)
      Me.Label9.Name = "Label9"
      Me.Label9.Size = New System.Drawing.Size(37, 13)
      Me.Label9.TabIndex = 10
      Me.Label9.Text = "Byte 5"
      '
      'DataTB1
      '
      Me.DataTB1.Location = New System.Drawing.Point(78, 127)
      Me.DataTB1.Mask = "990"
      Me.DataTB1.Name = "DataTB1"
      Me.DataTB1.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.DataTB1.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DataTB1.Size = New System.Drawing.Size(39, 20)
      Me.DataTB1.TabIndex = 11
      Me.DataTB1.Text = "0"
      '
      'DataTB2
      '
      Me.DataTB2.Location = New System.Drawing.Point(78, 154)
      Me.DataTB2.Mask = "990"
      Me.DataTB2.Name = "DataTB2"
      Me.DataTB2.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.DataTB2.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DataTB2.Size = New System.Drawing.Size(39, 20)
      Me.DataTB2.TabIndex = 12
      Me.DataTB2.Text = "0"
      '
      'DataTB3
      '
      Me.DataTB3.Location = New System.Drawing.Point(78, 181)
      Me.DataTB3.Mask = "990"
      Me.DataTB3.Name = "DataTB3"
      Me.DataTB3.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.DataTB3.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DataTB3.Size = New System.Drawing.Size(39, 20)
      Me.DataTB3.TabIndex = 13
      Me.DataTB3.Text = "0"
      '
      'DataTB4
      '
      Me.DataTB4.Location = New System.Drawing.Point(78, 208)
      Me.DataTB4.Mask = "990"
      Me.DataTB4.Name = "DataTB4"
      Me.DataTB4.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.DataTB4.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DataTB4.Size = New System.Drawing.Size(39, 20)
      Me.DataTB4.TabIndex = 14
      Me.DataTB4.Text = "0"
      '
      'DataTB5
      '
      Me.DataTB5.Location = New System.Drawing.Point(211, 127)
      Me.DataTB5.Mask = "990"
      Me.DataTB5.Name = "DataTB5"
      Me.DataTB5.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.DataTB5.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DataTB5.Size = New System.Drawing.Size(39, 20)
      Me.DataTB5.TabIndex = 15
      Me.DataTB5.Text = "0"
      '
      'DataTB6
      '
      Me.DataTB6.Location = New System.Drawing.Point(211, 154)
      Me.DataTB6.Mask = "990"
      Me.DataTB6.Name = "DataTB6"
      Me.DataTB6.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.DataTB6.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DataTB6.Size = New System.Drawing.Size(39, 20)
      Me.DataTB6.TabIndex = 16
      Me.DataTB6.Text = "0"
      '
      'DataTB7
      '
      Me.DataTB7.Location = New System.Drawing.Point(211, 181)
      Me.DataTB7.Mask = "990"
      Me.DataTB7.Name = "DataTB7"
      Me.DataTB7.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.DataTB7.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DataTB7.Size = New System.Drawing.Size(39, 20)
      Me.DataTB7.TabIndex = 17
      Me.DataTB7.Text = "0"
      '
      'DataTB8
      '
      Me.DataTB8.Location = New System.Drawing.Point(211, 208)
      Me.DataTB8.Mask = "990"
      Me.DataTB8.Name = "DataTB8"
      Me.DataTB8.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
      Me.DataTB8.RightToLeft = System.Windows.Forms.RightToLeft.Yes
      Me.DataTB8.Size = New System.Drawing.Size(39, 20)
      Me.DataTB8.TabIndex = 18
      Me.DataTB8.Text = "0"
      '
      'SendButton
      '
      Me.SendButton.Location = New System.Drawing.Point(105, 240)
      Me.SendButton.Name = "SendButton"
      Me.SendButton.Size = New System.Drawing.Size(75, 23)
      Me.SendButton.TabIndex = 19
      Me.SendButton.Text = "SEND"
      Me.SendButton.UseVisualStyleBackColor = True
      '
      'Transmitter
      '
      Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
      Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
      Me.ClientSize = New System.Drawing.Size(284, 274)
      Me.ControlBox = False
      Me.Controls.Add(Me.SendButton)
      Me.Controls.Add(Me.DataTB8)
      Me.Controls.Add(Me.DataTB7)
      Me.Controls.Add(Me.DataTB6)
      Me.Controls.Add(Me.DataTB5)
      Me.Controls.Add(Me.DataTB4)
      Me.Controls.Add(Me.DataTB3)
      Me.Controls.Add(Me.DataTB2)
      Me.Controls.Add(Me.DataTB1)
      Me.Controls.Add(Me.Label9)
      Me.Controls.Add(Me.Label8)
      Me.Controls.Add(Me.Label7)
      Me.Controls.Add(Me.Label6)
      Me.Controls.Add(Me.Label5)
      Me.Controls.Add(Me.Label4)
      Me.Controls.Add(Me.Label3)
      Me.Controls.Add(Me.Label2)
      Me.Controls.Add(Me.Label1)
      Me.Controls.Add(Me.DlcTB)
      Me.Controls.Add(Me.IdGroupbox)
      Me.Name = "Transmitter"
      Me.Text = "Transmitter"
      Me.IdGroupbox.ResumeLayout(False)
      Me.IdGroupbox.PerformLayout()
      Me.ResumeLayout(False)
      Me.PerformLayout()

   End Sub
   Friend WithEvents IdGroupbox As System.Windows.Forms.GroupBox
   Friend WithEvents CanIdTB As System.Windows.Forms.MaskedTextBox
   Friend WithEvents ExtendedRB As System.Windows.Forms.RadioButton
   Friend WithEvents StandardRB As System.Windows.Forms.RadioButton
   Friend WithEvents DlcTB As System.Windows.Forms.MaskedTextBox
   Friend WithEvents Label1 As System.Windows.Forms.Label
   Friend WithEvents Label2 As System.Windows.Forms.Label
   Friend WithEvents Label3 As System.Windows.Forms.Label
   Friend WithEvents Label4 As System.Windows.Forms.Label
   Friend WithEvents Label5 As System.Windows.Forms.Label
   Friend WithEvents Label6 As System.Windows.Forms.Label
   Friend WithEvents Label7 As System.Windows.Forms.Label
   Friend WithEvents Label8 As System.Windows.Forms.Label
   Friend WithEvents Label9 As System.Windows.Forms.Label
   Friend WithEvents DataTB1 As System.Windows.Forms.MaskedTextBox
   Friend WithEvents DataTB2 As System.Windows.Forms.MaskedTextBox
   Friend WithEvents DataTB3 As System.Windows.Forms.MaskedTextBox
   Friend WithEvents DataTB4 As System.Windows.Forms.MaskedTextBox
   Friend WithEvents DataTB5 As System.Windows.Forms.MaskedTextBox
   Friend WithEvents DataTB6 As System.Windows.Forms.MaskedTextBox
   Friend WithEvents DataTB7 As System.Windows.Forms.MaskedTextBox
   Friend WithEvents DataTB8 As System.Windows.Forms.MaskedTextBox
   Friend WithEvents SendButton As System.Windows.Forms.Button
End Class
