<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class VBEvent
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
      Me.CreateReceiver = New System.Windows.Forms.Button
      Me.ShutdownNodes = New System.Windows.Forms.Button
      Me.CreateTransmitterButton = New System.Windows.Forms.Button
      Me.SuspendLayout()
      '
      'CreateReceiver
      '
      Me.CreateReceiver.Location = New System.Drawing.Point(68, 33)
      Me.CreateReceiver.Name = "CreateReceiver"
      Me.CreateReceiver.Size = New System.Drawing.Size(148, 43)
      Me.CreateReceiver.TabIndex = 0
      Me.CreateReceiver.Text = "Create Receiving Node"
      Me.CreateReceiver.UseVisualStyleBackColor = True
      '
      'ShutdownNodes
      '
      Me.ShutdownNodes.Location = New System.Drawing.Point(68, 189)
      Me.ShutdownNodes.Name = "ShutdownNodes"
      Me.ShutdownNodes.Size = New System.Drawing.Size(148, 43)
      Me.ShutdownNodes.TabIndex = 1
      Me.ShutdownNodes.Text = "Shutdown Nodes"
      Me.ShutdownNodes.UseVisualStyleBackColor = True
      '
      'CreateTransmitterButton
      '
      Me.CreateTransmitterButton.Location = New System.Drawing.Point(68, 111)
      Me.CreateTransmitterButton.Name = "CreateTransmitterButton"
      Me.CreateTransmitterButton.Size = New System.Drawing.Size(148, 43)
      Me.CreateTransmitterButton.TabIndex = 2
      Me.CreateTransmitterButton.Text = "Create Transmitter"
      Me.CreateTransmitterButton.UseVisualStyleBackColor = True
      '
      'VBEvent
      '
      Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
      Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
      Me.ClientSize = New System.Drawing.Size(284, 264)
      Me.Controls.Add(Me.CreateTransmitterButton)
      Me.Controls.Add(Me.ShutdownNodes)
      Me.Controls.Add(Me.CreateReceiver)
      Me.MaximumSize = New System.Drawing.Size(300, 300)
      Me.MinimumSize = New System.Drawing.Size(300, 300)
      Me.Name = "VBEvent"
      Me.Text = "VBEvent Sample"
      Me.ResumeLayout(False)

   End Sub
    Friend WithEvents CreateReceiver As System.Windows.Forms.Button
   Friend WithEvents ShutdownNodes As System.Windows.Forms.Button
   Friend WithEvents CreateTransmitterButton As System.Windows.Forms.Button

End Class
