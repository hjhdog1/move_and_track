Public Class Form1
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents Button1 As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents RadioButton1 As System.Windows.Forms.RadioButton
    Friend WithEvents Button2 As System.Windows.Forms.Button
    Friend WithEvents Button3 As System.Windows.Forms.Button
    Friend WithEvents Label4 As System.Windows.Forms.Label
  Friend WithEvents TextBox1 As System.Windows.Forms.TextBox
  Friend WithEvents Label5 As System.Windows.Forms.Label
  Friend WithEvents Label6 As System.Windows.Forms.Label
  Friend WithEvents Button4 As System.Windows.Forms.Button
  Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
  Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
  Friend WithEvents Button5 As System.Windows.Forms.Button
  Friend WithEvents Label7 As System.Windows.Forms.Label
  Friend WithEvents TextBox2 As System.Windows.Forms.TextBox
  Friend WithEvents Label8 As System.Windows.Forms.Label
  Friend WithEvents Label9 As System.Windows.Forms.Label
  Friend WithEvents TextBox3 As System.Windows.Forms.TextBox
  Friend WithEvents Button6 As System.Windows.Forms.Button
  Friend WithEvents Label10 As System.Windows.Forms.Label
  <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
    Me.Button1 = New System.Windows.Forms.Button
    Me.Label1 = New System.Windows.Forms.Label
    Me.Label2 = New System.Windows.Forms.Label
    Me.Label3 = New System.Windows.Forms.Label
    Me.RadioButton1 = New System.Windows.Forms.RadioButton
    Me.Button2 = New System.Windows.Forms.Button
    Me.Button3 = New System.Windows.Forms.Button
    Me.Label4 = New System.Windows.Forms.Label
    Me.TextBox1 = New System.Windows.Forms.TextBox
    Me.Label5 = New System.Windows.Forms.Label
    Me.Label6 = New System.Windows.Forms.Label
    Me.Button4 = New System.Windows.Forms.Button
    Me.GroupBox1 = New System.Windows.Forms.GroupBox
    Me.GroupBox2 = New System.Windows.Forms.GroupBox
    Me.Label9 = New System.Windows.Forms.Label
    Me.TextBox3 = New System.Windows.Forms.TextBox
    Me.Button6 = New System.Windows.Forms.Button
    Me.Label10 = New System.Windows.Forms.Label
    Me.Label8 = New System.Windows.Forms.Label
    Me.TextBox2 = New System.Windows.Forms.TextBox
    Me.Button5 = New System.Windows.Forms.Button
    Me.Label7 = New System.Windows.Forms.Label
    Me.GroupBox1.SuspendLayout()
    Me.GroupBox2.SuspendLayout()
    Me.SuspendLayout()
    '
    'Button1
    '
    Me.Button1.Location = New System.Drawing.Point(40, 48)
    Me.Button1.Name = "Button1"
    Me.Button1.TabIndex = 0
    Me.Button1.Text = "Initialize"
    '
    'Label1
    '
    Me.Label1.Location = New System.Drawing.Point(128, 48)
    Me.Label1.Name = "Label1"
    Me.Label1.Size = New System.Drawing.Size(128, 24)
    Me.Label1.TabIndex = 2
    Me.Label1.Text = "Initializes the hardware "
    Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
    '
    'Label2
    '
    Me.Label2.Location = New System.Drawing.Point(104, 24)
    Me.Label2.Name = "Label2"
    Me.Label2.Size = New System.Drawing.Size(144, 40)
    Me.Label2.TabIndex = 3
    Me.Label2.Text = "Sends a CAN message from channel 0"
    Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
    '
    'Label3
    '
    Me.Label3.Location = New System.Drawing.Point(104, 112)
    Me.Label3.Name = "Label3"
    Me.Label3.Size = New System.Drawing.Size(136, 32)
    Me.Label3.TabIndex = 4
    Me.Label3.Text = "Indicates if there are any messages to read"
    '
    'RadioButton1
    '
    Me.RadioButton1.Location = New System.Drawing.Point(16, 112)
    Me.RadioButton1.Name = "RadioButton1"
    Me.RadioButton1.Size = New System.Drawing.Size(80, 24)
    Me.RadioButton1.TabIndex = 5
    Me.RadioButton1.Text = "Recevied"
    '
    'Button2
    '
    Me.Button2.Location = New System.Drawing.Point(16, 32)
    Me.Button2.Name = "Button2"
    Me.Button2.TabIndex = 1
    Me.Button2.Text = "Write msg"
    '
    'Button3
    '
    Me.Button3.Location = New System.Drawing.Point(16, 152)
    Me.Button3.Name = "Button3"
    Me.Button3.TabIndex = 6
    Me.Button3.Text = "Read msg"
    '
    'Label4
    '
    Me.Label4.Location = New System.Drawing.Point(104, 152)
    Me.Label4.Name = "Label4"
    Me.Label4.Size = New System.Drawing.Size(136, 32)
    Me.Label4.TabIndex = 7
    Me.Label4.Text = "Reads  the messages from the queue"
    '
    'TextBox1
    '
    Me.TextBox1.Location = New System.Drawing.Point(16, 192)
    Me.TextBox1.Name = "TextBox1"
    Me.TextBox1.Size = New System.Drawing.Size(72, 20)
    Me.TextBox1.TabIndex = 8
    Me.TextBox1.Text = ""
    '
    'Label5
    '
    Me.Label5.Location = New System.Drawing.Point(104, 192)
    Me.Label5.Name = "Label5"
    Me.Label5.Size = New System.Drawing.Size(128, 32)
    Me.Label5.TabIndex = 9
    Me.Label5.Text = "Number of received messages"
    '
    'Label6
    '
    Me.Label6.Location = New System.Drawing.Point(104, 64)
    Me.Label6.Name = "Label6"
    Me.Label6.Size = New System.Drawing.Size(144, 40)
    Me.Label6.TabIndex = 12
    Me.Label6.Text = "Sends 100 CAN messages from channel 0 "
    Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
    '
    'Button4
    '
    Me.Button4.Location = New System.Drawing.Point(16, 72)
    Me.Button4.Name = "Button4"
    Me.Button4.TabIndex = 11
    Me.Button4.Text = "Write msg"
    '
    'GroupBox1
    '
    Me.GroupBox1.Controls.Add(Me.Label6)
    Me.GroupBox1.Controls.Add(Me.Button4)
    Me.GroupBox1.Controls.Add(Me.TextBox1)
    Me.GroupBox1.Controls.Add(Me.Label5)
    Me.GroupBox1.Controls.Add(Me.Label2)
    Me.GroupBox1.Controls.Add(Me.Label3)
    Me.GroupBox1.Controls.Add(Me.RadioButton1)
    Me.GroupBox1.Controls.Add(Me.Button2)
    Me.GroupBox1.Controls.Add(Me.Button3)
    Me.GroupBox1.Controls.Add(Me.Label4)
    Me.GroupBox1.Location = New System.Drawing.Point(296, 16)
    Me.GroupBox1.Name = "GroupBox1"
    Me.GroupBox1.Size = New System.Drawing.Size(256, 240)
    Me.GroupBox1.TabIndex = 10
    Me.GroupBox1.TabStop = False
    Me.GroupBox1.Text = "Sending and receiving msg"
    '
    'GroupBox2
    '
    Me.GroupBox2.Controls.Add(Me.Label9)
    Me.GroupBox2.Controls.Add(Me.TextBox3)
    Me.GroupBox2.Controls.Add(Me.Button6)
    Me.GroupBox2.Controls.Add(Me.Label10)
    Me.GroupBox2.Controls.Add(Me.Label8)
    Me.GroupBox2.Controls.Add(Me.TextBox2)
    Me.GroupBox2.Controls.Add(Me.Button5)
    Me.GroupBox2.Controls.Add(Me.Label7)
    Me.GroupBox2.Location = New System.Drawing.Point(24, 16)
    Me.GroupBox2.Name = "GroupBox2"
    Me.GroupBox2.Size = New System.Drawing.Size(256, 240)
    Me.GroupBox2.TabIndex = 11
    Me.GroupBox2.TabStop = False
    Me.GroupBox2.Text = "Configurations"
    '
    'Label9
    '
    Me.Label9.Location = New System.Drawing.Point(104, 192)
    Me.Label9.Name = "Label9"
    Me.Label9.Size = New System.Drawing.Size(128, 24)
    Me.Label9.TabIndex = 18
    Me.Label9.Text = "Bitrate"
    Me.Label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
    '
    'TextBox3
    '
    Me.TextBox3.Location = New System.Drawing.Point(16, 192)
    Me.TextBox3.Name = "TextBox3"
    Me.TextBox3.Size = New System.Drawing.Size(72, 20)
    Me.TextBox3.TabIndex = 17
    Me.TextBox3.Text = ""
    '
    'Button6
    '
    Me.Button6.Location = New System.Drawing.Point(16, 152)
    Me.Button6.Name = "Button6"
    Me.Button6.TabIndex = 15
    Me.Button6.Text = "Bitrate"
    '
    'Label10
    '
    Me.Label10.Location = New System.Drawing.Point(104, 152)
    Me.Label10.Name = "Label10"
    Me.Label10.Size = New System.Drawing.Size(128, 24)
    Me.Label10.TabIndex = 16
    Me.Label10.Text = "Gets the bitrate"
    Me.Label10.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
    '
    'Label8
    '
    Me.Label8.Location = New System.Drawing.Point(104, 112)
    Me.Label8.Name = "Label8"
    Me.Label8.Size = New System.Drawing.Size(128, 24)
    Me.Label8.TabIndex = 14
    Me.Label8.Text = "Card type"
    Me.Label8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
    '
    'TextBox2
    '
    Me.TextBox2.Location = New System.Drawing.Point(16, 112)
    Me.TextBox2.Name = "TextBox2"
    Me.TextBox2.Size = New System.Drawing.Size(72, 20)
    Me.TextBox2.TabIndex = 13
    Me.TextBox2.Text = ""
    '
    'Button5
    '
    Me.Button5.Location = New System.Drawing.Point(16, 72)
    Me.Button5.Name = "Button5"
    Me.Button5.TabIndex = 1
    Me.Button5.Text = "Card type"
    '
    'Label7
    '
    Me.Label7.Location = New System.Drawing.Point(104, 72)
    Me.Label7.Name = "Label7"
    Me.Label7.Size = New System.Drawing.Size(128, 24)
    Me.Label7.TabIndex = 12
    Me.Label7.Text = "Gets the card type of channel 0"
    Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
    '
    'Form1
    '
    Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
    Me.ClientSize = New System.Drawing.Size(584, 278)
    Me.Controls.Add(Me.Label1)
    Me.Controls.Add(Me.Button1)
    Me.Controls.Add(Me.GroupBox1)
    Me.Controls.Add(Me.GroupBox2)
    Me.Name = "Form1"
    Me.Text = "Kvaser VB Sample program"
    Me.GroupBox1.ResumeLayout(False)
    Me.GroupBox2.ResumeLayout(False)
    Me.ResumeLayout(False)

  End Sub

#End Region

  Dim hnd0, hnd1 As Integer
    Dim can_status As canlibCLSNET.Canlib.canStatus

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Dim obj_buf As Object

        canlibCLSNET.Canlib.canInitializeLibrary()

        hnd0 = canlibCLSNET.Canlib.canOpenChannel(0, 0)
        hnd1 = canlibCLSNET.Canlib.canOpenChannel(1, 0)

        canlibCLSNET.Canlib.canGetChannelData(0, canlibCLSNET.Canlib.canCHANNELDATA_CHANNEL_NAME, obj_buf)

        Console.WriteLine("{0}: {1}", obj_buf.GetType, obj_buf)

        canlibCLSNET.Canlib.canSetBusParams(hnd0, canlibCLSNET.Canlib.canBITRATE_125K, 0, 0, 0, 0, 0)
        canlibCLSNET.Canlib.canSetBusParams(hnd1, canlibCLSNET.Canlib.canBITRATE_125K, 0, 0, 0, 0, 0)

        canlibCLSNET.Canlib.canBusOn(hnd0)
        canlibCLSNET.Canlib.canBusOn(hnd1)

        canlibCLSNET.Canlib.canSetNotify(hnd1, Me.Handle, canlibCLSNET.Canlib.canNOTIFY_RX)
        Me.Button1.Enabled = False

    End Sub

    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
    Dim msg(6) As Byte
    canlibCLSNET.Canlib.canWrite(hnd0, 123, msg, 6, 0)


  End Sub

  Private Sub Button3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button3.Click
    Dim id, dlc, flag, i As Integer
    Dim time As Long
    Dim msg(6) As Byte

    can_status = canlibCLSNET.Canlib.canStatus.canOK
    While can_status = canlibCLSNET.Canlib.canStatus.canOK
      can_status = canlibCLSNET.Canlib.canRead(hnd1, id, msg, dlc, flag, time)
      If (can_status = canlibCLSNET.Canlib.canStatus.canOK) Then
        i = i + 1
      End If
    End While
    Me.RadioButton1.Checked = False
    Me.TextBox1.Text = i

  End Sub
  Protected Overrides Sub WndProc( _
     ByRef m As Message _
  )
    If m.Msg = canlibCLSNET.Canlib.WM__CANLIB Then
      Console.WriteLine("WM__CANLIB received")
      Me.RadioButton1.Checked = True
    End If
    MyBase.WndProc(m)
  End Sub

  Private Sub Button4_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button4.Click
    Dim msg(6) As Byte
    Dim i As Integer
    For i = 0 To 99
      canlibCLSNET.Canlib.canWrite(hnd0, 123, msg, 6, 0)
    Next i

  End Sub

  Private Sub Button5_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button5.Click
    Dim chinfo As Object
    can_status = canlibCLSNET.Canlib.canGetChannelData(hnd0, canlibCLSNET.Canlib.canCHANNELDATA_CARD_TYPE, chinfo)
    Select Case chinfo
      Case canlibCLSNET.Canlib.canHWTYPE_NONE
        Me.TextBox2.Text = "Unknown"
      Case canlibCLSNET.Canlib.canHWTYPE_LAPCAN
        Me.TextBox2.Text = "LAPcan"
      Case canlibCLSNET.Canlib.canHWTYPE_USBCAN
        Me.TextBox2.Text = "USBcan"
      Case canlibCLSNET.Canlib.canHWTYPE_USBCAN_II
        Me.TextBox2.Text = "USBcanII"
      Case canlibCLSNET.Canlib.canHWTYPE_PCCAN
        Me.TextBox2.Text = "PCcan"
      Case canlibCLSNET.Canlib.canHWTYPE_PCICAN
        Me.TextBox2.Text = "PCIcan"
      Case canlibCLSNET.Canlib.canHWTYPE_PCICAN_II
        Me.TextBox2.Text = "PCIcanII"
      Case canlibCLSNET.Canlib.canHWTYPE_SIMULATED
        Me.TextBox2.Text = "Simulated"
      Case canlibCLSNET.Canlib.canHWTYPE_VIRTUAL
        Me.TextBox2.Text = "Virtual"
      Case canlibCLSNET.Canlib.canHWTYPE_LEAF
        Me.TextBox2.Text = "Kvaser Leaf"
      Case canlibCLSNET.Canlib.canHWTYPE_PC104_PLUS
        Me.TextBox2.Text = "PC104+"
      Case canlibCLSNET.Canlib.canHWTYPE_PCICANX_II
        Me.TextBox2.Text = "PCIcanx II"
      Case canlibCLSNET.Canlib.canHWTYPE_MEMORATOR_PRO
        Me.TextBox2.Text = "Memorator Professional"
      Case canlibCLSNET.Canlib.canHWTYPE_USBCAN_PRO
        Me.TextBox2.Text = "USBcan Professional"
      Case canlibCLSNET.Canlib.canHWTYPE_IRIS
        Me.TextBox2.Text = "Iris"
      Case canlibCLSNET.Canlib.canHWTYPE_USBCAN_LIGHT
        Me.TextBox2.Text = "USBcan Light"

    End Select

  End Sub

  Private Sub Button6_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button6.Click
    Dim freq, tseg1, tseg2, sjw, nosamp, syncmode As Integer
    canlibCLSNET.Canlib.canGetBusParams(hnd0, freq, tseg1, tseg2, sjw, nosamp, syncmode)
    Me.TextBox3.Text = freq
  End Sub
End Class
