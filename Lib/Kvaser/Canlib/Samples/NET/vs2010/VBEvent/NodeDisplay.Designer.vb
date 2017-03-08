<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class NodeDisplay
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
      Me.MsgText = New System.Windows.Forms.RichTextBox
      Me.SuspendLayout()
      '
      'MsgText
      '
      Me.MsgText.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
      Me.MsgText.Location = New System.Drawing.Point(13, 13)
      Me.MsgText.Name = "MsgText"
      Me.MsgText.Size = New System.Drawing.Size(560, 239)
      Me.MsgText.TabIndex = 0
      Me.MsgText.Text = ""
      '
      'NodeDisplay
      '
      Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
      Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
      Me.ClientSize = New System.Drawing.Size(585, 264)
      Me.ControlBox = False
      Me.Controls.Add(Me.MsgText)
      Me.Name = "NodeDisplay"
      Me.Text = "NodeDisplay"
      Me.ResumeLayout(False)

   End Sub
    Friend WithEvents MsgText As System.Windows.Forms.RichTextBox
End Class
