VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Kvaser"
   ClientHeight    =   6135
   ClientLeft      =   1710
   ClientTop       =   1590
   ClientWidth     =   8640
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   6135
   ScaleWidth      =   8640
   Begin VB.TextBox ChannelEdit 
      Height          =   288
      Left            =   2880
      TabIndex        =   11
      Text            =   "0"
      Top             =   1320
      Width           =   732
   End
   Begin VB.Frame Frame1 
      Caption         =   "Status"
      Height          =   1332
      Left            =   4200
      TabIndex        =   9
      Top             =   120
      Width           =   3612
      Begin VB.Label Label2 
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   9.75
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   492
         Left            =   960
         TabIndex        =   10
         Top             =   480
         Width           =   2172
      End
   End
   Begin VB.CommandButton Command8 
      Caption         =   "Close"
      Height          =   732
      Left            =   240
      TabIndex        =   8
      Top             =   5280
      Width           =   2532
   End
   Begin VB.CommandButton Command7 
      Caption         =   "Read a message"
      Height          =   612
      Left            =   240
      TabIndex        =   7
      Top             =   3840
      Width           =   2532
   End
   Begin VB.CommandButton Command6 
      Caption         =   "Send a message"
      Height          =   612
      Left            =   240
      TabIndex        =   6
      Top             =   3120
      Width           =   2532
   End
   Begin VB.CommandButton Command5 
      Caption         =   "Go Off Bus"
      Height          =   612
      Left            =   240
      TabIndex        =   5
      Top             =   4560
      Width           =   2532
   End
   Begin VB.CommandButton Command4 
      Caption         =   "Go On Bus"
      Height          =   492
      Left            =   240
      TabIndex        =   4
      Top             =   2520
      Width           =   2532
   End
   Begin VB.CommandButton Command3 
      Caption         =   "Set Bus Parameters"
      Height          =   612
      Left            =   240
      TabIndex        =   3
      Top             =   1800
      Width           =   2532
   End
   Begin VB.CommandButton Command2 
      Caption         =   "Open a circuit"
      Height          =   732
      Left            =   240
      TabIndex        =   1
      Top             =   960
      Width           =   2532
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Init Library"
      Height          =   732
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   2532
   End
   Begin VB.Label Label3 
      Caption         =   "Channel #"
      Height          =   252
      Left            =   2880
      TabIndex        =   12
      Top             =   1080
      Width           =   732
   End
   Begin VB.Label Label1 
      Height          =   372
      Left            =   3120
      TabIndex        =   2
      Top             =   3960
      Width           =   1932
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim Handle As Long

Private Sub DisplayStatus(stat As Long)
    ' Label2.Caption = Str(stat)
    Label2.Caption = GetErrorTextVB(stat)
End Sub


Private Sub Command1_Click()
    canInitializeLibrary
End Sub


'
'
Private Sub Command2_Click()
    Handle = canOpenChannel(Val(ChannelEdit.Text), canOPEN_EXCLUSIVE)
    If (Handle < 0) Then
        DisplayStatus (Handle)
    Else
        Label2.Caption = "OK."
    End If
    
End Sub


'
'
'
Private Sub Command3_Click()
Dim stat As Long
    stat = canSetBusParams(Handle, canBITRATE_125K, 0, 0, 0, 0, 0)
    DisplayStatus (stat)
    stat = canSetBusOutputControl(Handle, canPUSHPULL)
    DisplayStatus (stat)

End Sub


Private Sub Command4_Click()
    Dim stat As Long
    stat = canBusOn(Handle)
    DisplayStatus (stat)
End Sub


Private Sub Command5_Click()
    Dim stat As Long
    stat = canBusOff(Handle)
    DisplayStatus (stat)

End Sub


Private Sub Command6_Click()
    Dim stat As Long
    Dim msg(0 To 7) As Byte
    msg(0) = &H63
    msg(1) = &H53
    msg(2) = &H21
    msg(3) = &H84
    msg(4) = &H72
    msg(5) = &H19
    msg(6) = &H23
    msg(7) = &H44
    ' Send a message with id=123, dlc=8,
    ' first byte = &H63
    ' second byte = &H53
    ' third byte = &H21
    ' fourth byte = &H84
    ' fifth byte = &H72
    ' sixth byte = &H19
    ' seventh byte = &H23
    ' eighth byte = &H44
    stat = canWrite(Handle, 123, msg(0), 8, 0)
    DisplayStatus (stat)
End Sub


Private Sub Command7_Click()
    Dim stat As Long
    Dim id As Long
    Dim msg(0 To 7) As Byte
    Dim dlc As Long
    Dim Flags As Long
    Dim tmp As String
    Dim time As Long
    Dim i As Integer
    stat = canRead(Handle, id, msg(0), dlc, Flags, time)
    If stat = canOK Then
        tmp = ""
        tmp = tmp + "Id=" + Str(id)
        tmp = tmp + " Data="
        For i = 0 To dlc - 1
            tmp = tmp + " " + Str(msg(i))
        Next i
        tmp = tmp + " DLC=" + Str(dlc)
        tmp = tmp + " Flags=" + Str(Flags)
        Label1.Caption = tmp
    ElseIf stat = canERR_NOMSG Then
       Label1.Caption = "No message"
    Else
       ' an error
       Label1.Caption = "An error."
    End If
    DisplayStatus (stat)
    
End Sub


Private Sub Command8_Click()
    canClose (Handle)
    Handle = canINVALID_HANDLE
End Sub



