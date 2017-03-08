Attribute VB_Name = "Notify"
'(c) Ordina Technical Automation B.V. Deventer, The Netherlands
'Author: Ing. J.C. Muller
'Phone: (+31)570-504500
'fax: (+31)570-504591
'E-mail: hasse.muller@ordina.nl
Option Explicit
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'
' This module contains all code needed to get event notifications
' from the CAN Driver. It works closely related to INotify and CNotify.
'
' Notify.bas
'
' We install one MessageHandler and we only call the correct object that has
' the same (CAN) Handle.
'
'

Private Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hwnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Private Declare Function CallWindowProc Lib "user32" Alias "CallWindowProcA" (ByVal lpPrevWndFunc As Long, ByVal hwnd As Long, ByVal msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Private Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (ByRef Destination As Any, ByRef Source As Any, ByVal length As Long)
Private Const GWL_WNDPROC = (-4)

Private colNotify As Collection   'Collection holding pointer of INotify Interface or corresponding handle
Private OldNotify As Long         'Old message Handler function
Private CurrentNotify As INotify  '

Public Sub StartWdwMessageHandler(lCNotifyPtr As Long, CanHandle As Long)
    
       
    Set CurrentNotify = GetObjectByPtr(lCNotifyPtr)
    
    'create collection if not yet created
    If colNotify Is Nothing Then Set colNotify = New Collection
    
    'add to collection
    On Error GoTo Err: 'key may already exist....
    colNotify.Add lCNotifyPtr, Trim(Str$(CanHandle))
    
    ' Install the new WindowProc.
    ' We install our own handler for window messages. This handler
    ' is placed inbetween. It needs a hWnd of a form, we use the frmNotify for
    ' this.
    If OldNotify = 0 Then
        'only install one messagehandler
        OldNotify = SetWindowLong(frmNotify.hwnd, GWL_WNDPROC, AddressOf EventNotify)
    End If
    Exit Sub
Err:
    'Add your own error handler here
    Debug.Print Err.Description
End Sub
Public Sub StopWdwMessageHandler(CanHandle As Long)
    'remove from collection
    On Error GoTo Err 'key may already be deleted....
    colNotify.Remove Trim(Str$(CanHandle))
    
    'if no more handles in collection then remove message handler
    If colNotify.Count = 0 Then
        ' Restore the original window proc.
        SetWindowLong frmNotify.hwnd, GWL_WNDPROC, OldNotify
        
        ' Reset OldNotify value
        OldNotify = 0
    End If
    Exit Sub
Err:
    'Add your own error handler here
    Debug.Print Err.Description
    Resume Next
End Sub
Public Function EventNotify(ByVal hwnd As Long, ByVal msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Dim Ptr As Long
On Error GoTo Err:
    If msg = WM__CANLIB Then
        Log CANLIBCALLS, "Notify new message for handle: " & wParam
        'lParam lowest 2 bytes = canEVENT_RX (most significant 2 bytes are 0)
        'wParam is the handle of the circuit
                
        'we only want to tell the correct channel/CanBus so we
        'take the correct object from our collection
        If CurrentNotify.CurrentHandle <> wParam Then
            'current object not the correct object
            'kill old one and re-create the old one
            'from the pointer stored in our collection
            DestroyObject CurrentNotify
            Ptr = colNotify(Trim(Str$(wParam)))
            Set CurrentNotify = GetObjectByPtr(Ptr)
        End If
        
        Select Case lParam
            Case canEVENT_RX
                'notification of queue no longer empty
                'new notification after queue is completely emptied => Edges only!
                'Thus: Msg arrived in previously empty queue
                'call function in correct object
                CurrentNotify.CANQueueNoLongerEmpty
            Case canEVENT_TX
                'message was sent
                CurrentNotify.CANMsgTransmitted
            Case canEVENT_ERROR
                'error
                CurrentNotify.CANError
            Case canEVENT_STATUS
                'status changed
                CurrentNotify.CANStatusChanged
            Case Else
                'Add your own error handler here
        End Select
    Else
        ' Send other messages to the original
        ' window proc.
        EventNotify = CallWindowProc( _
                        OldNotify, hwnd, msg, _
                        wParam, lParam)
    End If
    
    Exit Function
Err:
    'Add your own error handler here
    Debug.Print Err.Description
    Resume Next
End Function

