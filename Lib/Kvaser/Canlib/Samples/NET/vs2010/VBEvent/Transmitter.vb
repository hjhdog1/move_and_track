Public Class Transmitter

   Dim ChannelHandle As Integer

   Private Sub DisplayError(ByVal errorCode As canlibCLSNET.Canlib.canStatus, ByVal msgTitle As String)
      Dim msgDescr As String = " "
      Dim status As canlibCLSNET.Canlib.canStatus

      If (errorCode < canlibCLSNET.Canlib.canStatus.canOK) Then
         status = canlibCLSNET.Canlib.canGetErrorText(errorCode, msgDescr)
         If (status <> canlibCLSNET.Canlib.canStatus.canOK) Then
            msgDescr = "Unknown error"
         End If
         MessageBox.Show(msgTitle, msgTitle + " returned error code " + Str(errorCode) _
                         + " - " + msgDescr, _
                         MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
      End If
   End Sub

   Public Sub New()

      ' This call is required by the Windows Form Designer.
      InitializeComponent()

      ' restrict control entry
      StandardRB.Checked = True

      ' Add any initialization after the InitializeComponent() call.
      Dim status As canlibCLSNET.Canlib.canStatus

      'Open the channel (The flags will allow a virtual channel to be used)
      ChannelHandle = canlibCLSNET.Canlib.canOpenChannel(1, canlibCLSNET.Canlib.canOPEN_ACCEPT_VIRTUAL)
      DisplayError(ChannelHandle, "canOpenChannel")

      'Set the bit params (In this case 250 kbits/sec)
      status = canlibCLSNET.Canlib.canSetBusParams(ChannelHandle, canlibCLSNET.Canlib.canBITRATE_250K, 0, 0, 0, 0, 0)
      DisplayError(status, "canSetBusParams")

      'Make the channel active
      status = canlibCLSNET.Canlib.canBusOn(ChannelHandle)
      DisplayError(status, "canBusOn")

   End Sub

   Private Sub SendButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SendButton.Click
      Dim id As Integer = 0
      Dim data As Byte() = New Byte(7) {}
      Dim dlc As Integer = 0
      Dim flag As Integer = 0
      Dim status As canlibCLSNET.Canlib.canStatus

      id = Val(CanIdTB.Text)
      dlc = Val(DlcTB.Text)
      data(0) = Val(DataTB1.Text)
      data(1) = Val(DataTB2.Text)
      data(2) = Val(DataTB3.Text)
      data(3) = Val(DataTB4.Text)
      data(4) = Val(DataTB5.Text)
      data(5) = Val(DataTB6.Text)
      data(6) = Val(DataTB7.Text)
      data(7) = Val(DataTB8.Text)
      If (StandardRB.Checked) Then
         flag = canlibCLSNET.Canlib.canMSG_STD
      Else
         flag = canlibCLSNET.Canlib.canMSG_EXT
      End If

      status = canlibCLSNET.Canlib.canWrite(ChannelHandle, id, data, dlc, flag)

   End Sub

   Private Sub Transmitter_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
      Dim status As canlibCLSNET.Canlib.canStatus

      'Make the channel inactive
      status = canlibCLSNET.Canlib.canBusOff(ChannelHandle)
      DisplayError(status, "canBusOff")

      status = canlibCLSNET.Canlib.canClose(ChannelHandle)
      DisplayError(status, "canClose")

   End Sub
End Class