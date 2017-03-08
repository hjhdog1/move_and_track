Public Class NodeDisplay
    'Allows thread to pass information to the Forms GUI Thread
    Public Delegate Sub AddTextDelegate(ByVal newText As String)
    Public Delegate Sub AddStatusDelegate(ByVal funcName As String, ByVal errorCode As canlibCLSNET.Canlib.canStatus)
    Public Delegate Sub AddMsgDelegate(ByVal identifier As Integer, ByVal message() As Byte, ByVal dlc As Integer, ByVal flags As Integer, ByVal time As Long)
    Public Delegate Sub AddErrFrameDelegate(ByVal time As Long)
    Public textDelegate As AddTextDelegate
    Public statusDelegate As AddStatusDelegate
    Public msgDelegate As AddMsgDelegate
    Public errDelegate As AddErrFrameDelegate

    Private myThread As System.Threading.Thread
    Private myId As Integer = -1
    Private reqShutdown = False

    Public Sub AddTextMethod(ByVal newText As String)
        'Thread will invoke this routine to just add some text to the form
        MsgText.AppendText(newText)
    End Sub

    Public Sub DisplayStatusInfo(ByVal funcName As String, ByVal errorCode As canlibCLSNET.Canlib.canStatus)
        'Thread will invoke this routine to add CANLib return code status information to the form
        Dim newText As String = " "
        Dim status As Integer

        MsgText.AppendText(funcName + " ")

        If (errorCode < canlibCLSNET.Canlib.canStatus.canOK) Then
            MsgText.AppendText("returned error code " + Str(errorCode) + ": ")
            status = canlibCLSNET.Canlib.canGetErrorText(errorCode, newText)
            If (status <> canlibCLSNET.Canlib.canStatus.canOK) Then
                MsgText.AppendText("Error Lookup Failed - " + Str(status))
            Else
                MsgText.AppendText(newText)
            End If
            MsgText.AppendText(ControlChars.NewLine)
        Else
            MsgText.AppendText("returned successfully" + ControlChars.NewLine)
        End If

    End Sub

    Private Sub DisplayErrorFrameText(ByVal time As Long)
        'Thread will invoke this routine to display error frame information in the form.
        MsgText.AppendText("canRead received Error Frame at Time = " + Str(time) + ControlChars.NewLine)

    End Sub

    Private Sub DisplayMessageInfo(ByVal identifier As Integer, ByVal message() As Byte, ByVal dlc As Integer, ByVal flags As Integer, ByVal time As Long)
        'Thread will invoke this routine to display the received CAN frame content in the form
        MsgText.AppendText("canRead received:  ID = " + identifier.ToString("X8") + "h   DLC = " + dlc.ToString("X1") + "   Data = ")
        For i As Integer = 0 To 7
            If (i < dlc) Then
                MsgText.AppendText(message(i).ToString("X2") + "h  ")
            Else
                MsgText.AppendText("     ")
            End If
        Next i
        MsgText.AppendText("Flags = " + flags.ToString("X8") + "h   Time = " + time.ToString("D") + ControlChars.NewLine)
    End Sub

    Public Sub New(ByVal NodeID As Integer)

        ' This call is required by the Windows Form Designer.
        InitializeComponent()

        myId = NodeID
        Text = "Receiver Node" + Str(NodeID)
        'Link the routines to the Delegate values
        textDelegate = New AddTextDelegate(AddressOf AddTextMethod)
        statusDelegate = New AddStatusDelegate(AddressOf DisplayStatusInfo)
        msgDelegate = New AddMsgDelegate(AddressOf DisplayMessageInfo)
        errDelegate = New AddErrFrameDelegate(AddressOf DisplayErrorFrameText)

        'Create and start the thread
        myThread = New System.Threading.Thread(New System.Threading.ThreadStart(AddressOf ThreadFunction))
        myThread.Start()

    End Sub

    Private Sub ThreadFunction()
        'Thread routine
        Dim myThreadClassObject As New ReceiveNode(Me)
        myThreadClassObject.Run()
    End Sub 'ThreadFunction

    Public Sub RequestShutdown()
        reqShutdown = True
    End Sub
    Public Function RequestShutdownStatus() As Boolean
        Return reqShutdown
    End Function
End Class

Public Class ReceiveNode
    'The worker thread code is in this class
    Private myForm As NodeDisplay

    Public Sub New(ByVal assignedForm As NodeDisplay)
        myForm = assignedForm
    End Sub

    Public Sub Run()
        'The real work performed by the Thread is in this routine
        Dim ChannelHandle As Integer
        Dim status As canlibCLSNET.Canlib.canStatus
        Dim winHandle As New Object

        'Open the channel (The flags will allow a virtual channel to be used)
        ChannelHandle = canlibCLSNET.Canlib.canOpenChannel(0, canlibCLSNET.Canlib.canOPEN_ACCEPT_VIRTUAL)
        myForm.Invoke(myForm.statusDelegate, New Object() {"canOpenChannel", ChannelHandle})

        'Set the bit params (In this case 250 kbits/sec)
        status = canlibCLSNET.Canlib.canSetBusParams(ChannelHandle, canlibCLSNET.Canlib.canBITRATE_250K, 0, 0, 0, 0, 0)
        myForm.Invoke(myForm.statusDelegate, New Object() {"canSetBusParams", status})

        status = canlibCLSNET.Canlib.canIoCtl(ChannelHandle, canlibCLSNET.Canlib.canIOCTL_GET_EVENTHANDLE, winHandle)
        myForm.Invoke(myForm.statusDelegate, New Object() {"canIoCtl", status})

        'Make the channel active
        status = canlibCLSNET.Canlib.canBusOn(ChannelHandle)
        myForm.Invoke(myForm.statusDelegate, New Object() {"canBusOn", status})

        'Put the event handle into a usuable variable form
        Dim kvEvent As New CanlibWaitEvent(winHandle)
        Dim waitHandles As System.Threading.WaitHandle() = New System.Threading.WaitHandle() {kvEvent}
        Dim NotShutdown As Boolean = True

        Do While (NotShutdown And (Not myForm.RequestShutdownStatus()))
            'Wait for 1 second or the occurence of a Kvaser event
            If (System.Threading.WaitHandle.WaitAny(waitHandles, 1000, False) = 0) Then
                Dim id As Integer = 0
                Dim data As Byte() = New Byte(7) {}
                Dim dlc As Integer = 0
                Dim flag As Integer = 0
                Dim time As Long = 0
                'Loop until the receive buffer is empty
                Do
                    status = canlibCLSNET.Canlib.canRead(ChannelHandle, id, data, dlc, flag, time)
                    If (status = canlibCLSNET.Canlib.canStatus.canOK) Then
                        If ((flag And canlibCLSNET.Canlib.canMSG_ERROR_FRAME) = 0) Then
                            'Message data has been returned
                     myForm.Invoke(myForm.msgDelegate, New Object() {id, data, dlc, flag, time})
                        Else
                            'Error Frame data has been returned.
                            myForm.Invoke(myForm.errDelegate, New Object() {time})
                        End If
                    End If
                Loop Until status <> 0
                'Check that we exited the loop because the incoming message buffer was empty
                If (status <> canlibCLSNET.Canlib.canStatus.canERR_NOMSG) Then
                    myForm.Invoke(myForm.statusDelegate, New Object() {"canRead", status})
                    myForm.Invoke(myForm.textDelegate, New Object() {"Halting Receive Node" + +ControlChars.NewLine})
                    NotShutdown = False
                End If
            End If 'WaitAny returned because of a CAN event
        Loop  'While NotShutdown

        'Make the channel inactive
        status = canlibCLSNET.Canlib.canBusOff(ChannelHandle)
        myForm.Invoke(myForm.statusDelegate, New Object() {"canBusOff", status})

        status = canlibCLSNET.Canlib.canClose(ChannelHandle)
        myForm.Invoke(myForm.statusDelegate, New Object() {"canClose", status})

        myForm.Invoke(myForm.textDelegate, New Object() {"Thread has completed execution...  Goodbye!"})
    End Sub 'Run

End Class