VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "CNotify"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
'(c) Ordina Technical Automation B.V. Deventer, The Netherlands
'Author: Ing. J.C. Muller
'Phone: (+31)570-504500
'fax: (+31)570-504591
'E-mail: hasse.muller@ordina.nl
Option Explicit

Implements INotify

Private mHandle As Long    'we store the handle of the channel we have to
                           'send a notify event for

Public Event DataReceived()
Public Event MessageSent()
Public Event BusError(Active As Boolean)

Public Sub SetUpEventNotification(Handle As Long)
    ' Install the new WindowProc.
    ' We install our own handler for window messages. This handler
    ' is placed inbetween. It needs a hWnd of a form, we use the frmNotify for
    ' this.
    
    'store handle (local)
    mHandle = Handle
    
    'setup windows message handler
    StartWdwMessageHandler ObjPtr(Me), mHandle

    'setup CANLIB to get events
    canSetNotify mHandle, frmNotify.hwnd, canNOTIFY_STATUS + _
                                          canNOTIFY_ERROR + _
                                          canNOTIFY_TX + _
                                          canNOTIFY_RX
End Sub
Public Sub CancelEventNotification()
    'setup CANLIB to get events
    canSetNotify mHandle, frmNotify.hwnd, 0
    
    'Uninstall handler
    StopWdwMessageHandler mHandle
    
    'clear local storage of Handle
    mHandle = canINVALID_HANDLE
End Sub
Private Sub INotify_CANMsgTransmitted()
    RaiseEvent MessageSent
End Sub
Private Sub INotify_CANQueueNoLongerEmpty()
    RaiseEvent DataReceived
End Sub
Private Sub INotify_CANError()
    'No implementation in this demo.
    'problem: no good description of these events found yet.
End Sub
Private Sub INotify_CANStatusChanged()
Dim Flags As Long
    canReadStatus mHandle, Flags
    
    If ((Flags And canSTAT_ERROR_PASSIVE) = canSTAT_ERROR_PASSIVE) Or _
        (Flags And canSTAT_BUS_OFF) = canSTAT_BUS_OFF Then
        'Debug.Print "BUS Errors ACTIVE"
        RaiseEvent BusError(True)
    Else
        'Debug.Print "BUS Errors INACTIVE"
        RaiseEvent BusError(False)
    End If
End Sub
Private Function INotify_CurrentHandle() As Long
    INotify_CurrentHandle = mHandle
End Function
