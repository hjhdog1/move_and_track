<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class MainForm
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
        Me.StartButton = New System.Windows.Forms.Button
        Me.StopButton = New System.Windows.Forms.Button
        Me.PeriodMTB = New System.Windows.Forms.MaskedTextBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.Label4 = New System.Windows.Forms.Label
        Me.UpdateButton = New System.Windows.Forms.Button
        Me.IdentifierNUD = New System.Windows.Forms.NumericUpDown
        Me.Label5 = New System.Windows.Forms.Label
        Me.IdentifierGB = New System.Windows.Forms.GroupBox
        Me.ExtendedIdRB = New System.Windows.Forms.RadioButton
        Me.StandardIdRB = New System.Windows.Forms.RadioButton
        Me.DataGB = New System.Windows.Forms.GroupBox
        Me.Label14 = New System.Windows.Forms.Label
        Me.Label13 = New System.Windows.Forms.Label
        Me.Label12 = New System.Windows.Forms.Label
        Me.Label11 = New System.Windows.Forms.Label
        Me.Label10 = New System.Windows.Forms.Label
        Me.Label9 = New System.Windows.Forms.Label
        Me.Label8 = New System.Windows.Forms.Label
        Me.Label7 = New System.Windows.Forms.Label
        Me.Label6 = New System.Windows.Forms.Label
        Me.Data7NUD = New System.Windows.Forms.NumericUpDown
        Me.Data6NUD = New System.Windows.Forms.NumericUpDown
        Me.Data5NUD = New System.Windows.Forms.NumericUpDown
        Me.Data4NUD = New System.Windows.Forms.NumericUpDown
        Me.Data3NUD = New System.Windows.Forms.NumericUpDown
        Me.Data2NUD = New System.Windows.Forms.NumericUpDown
        Me.Data1NUD = New System.Windows.Forms.NumericUpDown
        Me.Data0NUD = New System.Windows.Forms.NumericUpDown
        Me.DlcNUD = New System.Windows.Forms.NumericUpDown
        CType(Me.IdentifierNUD, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.IdentifierGB.SuspendLayout()
        Me.DataGB.SuspendLayout()
        CType(Me.Data7NUD, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Data6NUD, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Data5NUD, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Data4NUD, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Data3NUD, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Data2NUD, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Data1NUD, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.Data0NUD, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.DlcNUD, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'StartButton
        '
        Me.StartButton.Location = New System.Drawing.Point(15, 12)
        Me.StartButton.Name = "StartButton"
        Me.StartButton.Size = New System.Drawing.Size(83, 50)
        Me.StartButton.TabIndex = 0
        Me.StartButton.Text = "Activate Channel 0"
        Me.StartButton.UseVisualStyleBackColor = True
        '
        'StopButton
        '
        Me.StopButton.Location = New System.Drawing.Point(15, 225)
        Me.StopButton.Name = "StopButton"
        Me.StopButton.Size = New System.Drawing.Size(83, 50)
        Me.StopButton.TabIndex = 1
        Me.StopButton.Text = "Deactivate Channel 0"
        Me.StopButton.UseVisualStyleBackColor = True
        '
        'PeriodMTB
        '
        Me.PeriodMTB.Location = New System.Drawing.Point(133, 29)
        Me.PeriodMTB.Mask = "#####"
        Me.PeriodMTB.Name = "PeriodMTB"
        Me.PeriodMTB.PromptChar = Global.Microsoft.VisualBasic.ChrW(32)
        Me.PeriodMTB.Size = New System.Drawing.Size(51, 20)
        Me.PeriodMTB.TabIndex = 2
        Me.PeriodMTB.Text = "10"
        Me.PeriodMTB.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.PeriodMTB.ValidatingType = GetType(Integer)
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label1.Location = New System.Drawing.Point(136, 15)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(43, 13)
        Me.Label1.TabIndex = 3
        Me.Label1.Text = "Period"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.Location = New System.Drawing.Point(189, 32)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(75, 13)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "milliseconds"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label3.Location = New System.Drawing.Point(301, 32)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(220, 13)
        Me.Label3.TabIndex = 5
        Me.Label3.Text = "Set Period before Activating channel."
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.Location = New System.Drawing.Point(130, 244)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(332, 13)
        Me.Label4.TabIndex = 6
        Me.Label4.Text = "Deactivate channel before closing and to change Period."
        '
        'UpdateButton
        '
        Me.UpdateButton.Location = New System.Drawing.Point(15, 115)
        Me.UpdateButton.Name = "UpdateButton"
        Me.UpdateButton.Size = New System.Drawing.Size(83, 50)
        Me.UpdateButton.TabIndex = 7
        Me.UpdateButton.Text = "Update"
        Me.UpdateButton.UseVisualStyleBackColor = True
        '
        'IdentifierNUD
        '
        Me.IdentifierNUD.Hexadecimal = True
        Me.IdentifierNUD.Location = New System.Drawing.Point(133, 112)
        Me.IdentifierNUD.Maximum = New Decimal(New Integer() {536870911, 0, 0, 0})
        Me.IdentifierNUD.Name = "IdentifierNUD"
        Me.IdentifierNUD.Size = New System.Drawing.Size(90, 20)
        Me.IdentifierNUD.TabIndex = 8
        Me.IdentifierNUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label5.Location = New System.Drawing.Point(146, 93)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(57, 13)
        Me.Label5.TabIndex = 9
        Me.Label5.Text = "Identifier"
        '
        'IdentifierGB
        '
        Me.IdentifierGB.Controls.Add(Me.ExtendedIdRB)
        Me.IdentifierGB.Controls.Add(Me.StandardIdRB)
        Me.IdentifierGB.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.IdentifierGB.Location = New System.Drawing.Point(270, 93)
        Me.IdentifierGB.Name = "IdentifierGB"
        Me.IdentifierGB.Size = New System.Drawing.Size(209, 43)
        Me.IdentifierGB.TabIndex = 10
        Me.IdentifierGB.TabStop = False
        Me.IdentifierGB.Text = "Identifier Type"
        '
        'ExtendedIdRB
        '
        Me.ExtendedIdRB.AutoSize = True
        Me.ExtendedIdRB.Location = New System.Drawing.Point(116, 22)
        Me.ExtendedIdRB.Name = "ExtendedIdRB"
        Me.ExtendedIdRB.Size = New System.Drawing.Size(78, 17)
        Me.ExtendedIdRB.TabIndex = 1
        Me.ExtendedIdRB.TabStop = True
        Me.ExtendedIdRB.Text = "Extended"
        Me.ExtendedIdRB.UseVisualStyleBackColor = True
        '
        'StandardIdRB
        '
        Me.StandardIdRB.AutoSize = True
        Me.StandardIdRB.Checked = True
        Me.StandardIdRB.Location = New System.Drawing.Point(15, 22)
        Me.StandardIdRB.Name = "StandardIdRB"
        Me.StandardIdRB.Size = New System.Drawing.Size(76, 17)
        Me.StandardIdRB.TabIndex = 0
        Me.StandardIdRB.TabStop = True
        Me.StandardIdRB.Text = "Standard"
        Me.StandardIdRB.UseVisualStyleBackColor = True
        '
        'DataGB
        '
        Me.DataGB.Controls.Add(Me.Label14)
        Me.DataGB.Controls.Add(Me.Label13)
        Me.DataGB.Controls.Add(Me.Label12)
        Me.DataGB.Controls.Add(Me.Label11)
        Me.DataGB.Controls.Add(Me.Label10)
        Me.DataGB.Controls.Add(Me.Label9)
        Me.DataGB.Controls.Add(Me.Label8)
        Me.DataGB.Controls.Add(Me.Label7)
        Me.DataGB.Controls.Add(Me.Label6)
        Me.DataGB.Controls.Add(Me.Data7NUD)
        Me.DataGB.Controls.Add(Me.Data6NUD)
        Me.DataGB.Controls.Add(Me.Data5NUD)
        Me.DataGB.Controls.Add(Me.Data4NUD)
        Me.DataGB.Controls.Add(Me.Data3NUD)
        Me.DataGB.Controls.Add(Me.Data2NUD)
        Me.DataGB.Controls.Add(Me.Data1NUD)
        Me.DataGB.Controls.Add(Me.Data0NUD)
        Me.DataGB.Controls.Add(Me.DlcNUD)
        Me.DataGB.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.DataGB.Location = New System.Drawing.Point(133, 142)
        Me.DataGB.Name = "DataGB"
        Me.DataGB.Size = New System.Drawing.Size(475, 74)
        Me.DataGB.TabIndex = 11
        Me.DataGB.TabStop = False
        Me.DataGB.Text = "Data"
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label14.Location = New System.Drawing.Point(428, 27)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(14, 13)
        Me.Label14.TabIndex = 17
        Me.Label14.Text = "7"
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label13.Location = New System.Drawing.Point(380, 27)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(14, 13)
        Me.Label13.TabIndex = 16
        Me.Label13.Text = "6"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label12.Location = New System.Drawing.Point(332, 27)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(14, 13)
        Me.Label12.TabIndex = 15
        Me.Label12.Text = "5"
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label11.Location = New System.Drawing.Point(284, 27)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(14, 13)
        Me.Label11.TabIndex = 14
        Me.Label11.Text = "4"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label10.Location = New System.Drawing.Point(236, 27)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(14, 13)
        Me.Label10.TabIndex = 13
        Me.Label10.Text = "3"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label9.Location = New System.Drawing.Point(188, 27)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(14, 13)
        Me.Label9.TabIndex = 12
        Me.Label9.Text = "2"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label8.Location = New System.Drawing.Point(140, 27)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(14, 13)
        Me.Label8.TabIndex = 11
        Me.Label8.Text = "1"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label7.Location = New System.Drawing.Point(92, 27)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(14, 13)
        Me.Label7.TabIndex = 10
        Me.Label7.Text = "0"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label6.Location = New System.Drawing.Point(18, 27)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(31, 13)
        Me.Label6.TabIndex = 9
        Me.Label6.Text = "DLC"
        '
        'Data7NUD
        '
        Me.Data7NUD.Hexadecimal = True
        Me.Data7NUD.Location = New System.Drawing.Point(418, 45)
        Me.Data7NUD.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Data7NUD.Name = "Data7NUD"
        Me.Data7NUD.Size = New System.Drawing.Size(36, 20)
        Me.Data7NUD.TabIndex = 8
        Me.Data7NUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Data6NUD
        '
        Me.Data6NUD.Hexadecimal = True
        Me.Data6NUD.Location = New System.Drawing.Point(370, 45)
        Me.Data6NUD.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Data6NUD.Name = "Data6NUD"
        Me.Data6NUD.Size = New System.Drawing.Size(36, 20)
        Me.Data6NUD.TabIndex = 7
        Me.Data6NUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Data5NUD
        '
        Me.Data5NUD.Hexadecimal = True
        Me.Data5NUD.Location = New System.Drawing.Point(322, 45)
        Me.Data5NUD.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Data5NUD.Name = "Data5NUD"
        Me.Data5NUD.Size = New System.Drawing.Size(36, 20)
        Me.Data5NUD.TabIndex = 6
        Me.Data5NUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Data4NUD
        '
        Me.Data4NUD.Hexadecimal = True
        Me.Data4NUD.Location = New System.Drawing.Point(274, 45)
        Me.Data4NUD.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Data4NUD.Name = "Data4NUD"
        Me.Data4NUD.Size = New System.Drawing.Size(36, 20)
        Me.Data4NUD.TabIndex = 5
        Me.Data4NUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Data3NUD
        '
        Me.Data3NUD.Hexadecimal = True
        Me.Data3NUD.Location = New System.Drawing.Point(226, 45)
        Me.Data3NUD.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Data3NUD.Name = "Data3NUD"
        Me.Data3NUD.Size = New System.Drawing.Size(36, 20)
        Me.Data3NUD.TabIndex = 4
        Me.Data3NUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Data2NUD
        '
        Me.Data2NUD.Hexadecimal = True
        Me.Data2NUD.Location = New System.Drawing.Point(178, 45)
        Me.Data2NUD.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Data2NUD.Name = "Data2NUD"
        Me.Data2NUD.Size = New System.Drawing.Size(36, 20)
        Me.Data2NUD.TabIndex = 3
        Me.Data2NUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Data1NUD
        '
        Me.Data1NUD.Hexadecimal = True
        Me.Data1NUD.Location = New System.Drawing.Point(130, 45)
        Me.Data1NUD.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Data1NUD.Name = "Data1NUD"
        Me.Data1NUD.Size = New System.Drawing.Size(36, 20)
        Me.Data1NUD.TabIndex = 2
        Me.Data1NUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Data0NUD
        '
        Me.Data0NUD.Hexadecimal = True
        Me.Data0NUD.Location = New System.Drawing.Point(82, 45)
        Me.Data0NUD.Maximum = New Decimal(New Integer() {255, 0, 0, 0})
        Me.Data0NUD.Name = "Data0NUD"
        Me.Data0NUD.Size = New System.Drawing.Size(36, 20)
        Me.Data0NUD.TabIndex = 1
        Me.Data0NUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'DlcNUD
        '
        Me.DlcNUD.Location = New System.Drawing.Point(16, 45)
        Me.DlcNUD.Maximum = New Decimal(New Integer() {15, 0, 0, 0})
        Me.DlcNUD.Name = "DlcNUD"
        Me.DlcNUD.Size = New System.Drawing.Size(36, 20)
        Me.DlcNUD.TabIndex = 0
        Me.DlcNUD.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(628, 287)
        Me.Controls.Add(Me.DataGB)
        Me.Controls.Add(Me.IdentifierGB)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.IdentifierNUD)
        Me.Controls.Add(Me.UpdateButton)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.PeriodMTB)
        Me.Controls.Add(Me.StopButton)
        Me.Controls.Add(Me.StartButton)
        Me.Name = "MainForm"
        Me.Text = "Auto Transmit Example"
        CType(Me.IdentifierNUD, System.ComponentModel.ISupportInitialize).EndInit()
        Me.IdentifierGB.ResumeLayout(False)
        Me.IdentifierGB.PerformLayout()
        Me.DataGB.ResumeLayout(False)
        Me.DataGB.PerformLayout()
        CType(Me.Data7NUD, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Data6NUD, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Data5NUD, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Data4NUD, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Data3NUD, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Data2NUD, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Data1NUD, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.Data0NUD, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.DlcNUD, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents StartButton As System.Windows.Forms.Button
    Friend WithEvents StopButton As System.Windows.Forms.Button
    Friend WithEvents PeriodMTB As System.Windows.Forms.MaskedTextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents UpdateButton As System.Windows.Forms.Button
    Friend WithEvents IdentifierNUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents IdentifierGB As System.Windows.Forms.GroupBox
    Friend WithEvents StandardIdRB As System.Windows.Forms.RadioButton
    Friend WithEvents ExtendedIdRB As System.Windows.Forms.RadioButton
    Friend WithEvents DataGB As System.Windows.Forms.GroupBox
    Friend WithEvents Data0NUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents DlcNUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Data4NUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Data3NUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Data2NUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Data1NUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Data7NUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Data6NUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Data5NUD As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label

End Class
