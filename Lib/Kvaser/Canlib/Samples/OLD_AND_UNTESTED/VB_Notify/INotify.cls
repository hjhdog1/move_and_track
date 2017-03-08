VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "INotify"
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

'Interface INotify of CNotify Class holding some friend procedures that should not
'be public to user. (No need to give the users any unneeded info)


'method called by Notify procedure function to fire event of CNotify Class
Public Sub CANQueueNoLongerEmpty()
End Sub

Public Sub CANStatusChanged()
End Sub

Public Sub CANError()
End Sub

Public Sub CANMsgTransmitted()
End Sub

'Get current handle
Public Function CurrentHandle() As Long
End Function

 
