Public Class VBEvent
   Dim FirstTime As Boolean = True

   Const MAX_RX_NODES As Integer = 3
   Const MAX_TX_NODES As Integer = 1
   Dim RxNodeCount As Integer = 0
   Dim TxNodeCount As Integer = 0
   Dim myRxNodes(MAX_RX_NODES) As NodeDisplay
   Dim myTxNode As Transmitter



    Private Sub CreateReceiver_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CreateReceiver.Click

        'Prepare CANLib for the individual threads
        If (FirstTime) Then
            canlibCLSNET.Canlib.canInitializeLibrary()
            FirstTime = False
        End If

        'Limit the number of nodes that can be created
        If (RxNodeCount < MAX_RX_NODES) Then
            'Create a new thread
            myRxNodes(RxNodeCount) = New NodeDisplay(RxNodeCount + 1)
            myRxNodes(RxNodeCount).Show()
            RxNodeCount = RxNodeCount + 1
        Else
            MessageBox.Show("You have already created the maximum number of Receive Nodes.  You cannot create more.", _
                            "Node Creation Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
        End If

    End Sub

   Private Sub ShutdownNodes_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ShutdownNodes.Click
      myTxNode.Close()
      TxNodeCount = 0
      For i As Integer = 0 To (RxNodeCount - 1) Step 1
         myRxNodes(i).RequestShutdown()
      Next i
      RxNodeCount = 0
   End Sub

   Private Sub CreateTransmitterButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CreateTransmitterButton.Click

      'Prepare CANLib for the individual threads
      If (FirstTime) Then
         canlibCLSNET.Canlib.canInitializeLibrary()
         FirstTime = False
      End If

      'Limit the number of nodes that can be created
      If (TxNodeCount < MAX_TX_NODES) Then
         'Create a new thread
         myTxNode = New Transmitter()
         myTxNode.Show()
         TxNodeCount = TxNodeCount + 1
      Else
         MessageBox.Show("You can only create one Transmitter.  You cannot create more.", _
                         "Transmitter Creation Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
      End If

   End Sub
End Class
