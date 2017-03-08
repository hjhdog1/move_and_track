object MainForm: TMainForm
  Left = 350
  Top = 290
  AutoScroll = False
  Caption = 'CAN Bus Status'
  ClientHeight = 474
  ClientWidth = 625
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 609
    Height = 229
    Caption = 'Channel A'
    TabOrder = 0
    object Label1: TLabel
      Left = 20
      Top = 120
      Width = 66
      Height = 13
      Caption = 'Bus Status:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object BusStatusLabel: TLabel
      Left = 116
      Top = 120
      Width = 74
      Height = 13
      Caption = 'BusStatusLabel'
    end
    object Label4: TLabel
      Left = 20
      Top = 148
      Width = 86
      Height = 13
      Caption = 'Error Counters:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object ErrorCounterLabel: TLabel
      Left = 116
      Top = 148
      Width = 85
      Height = 13
      Caption = 'ErrorCounterLabel'
    end
    object ChannelCombo1: TComboBox
      Left = 24
      Top = 20
      Width = 317
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
    end
    object OnBusBtn: TButton
      Left = 24
      Top = 64
      Width = 75
      Height = 25
      Caption = 'Go OnBus'
      TabOrder = 1
      OnClick = OnBusBtnClick
    end
    object OffBusBtn: TButton
      Left = 144
      Top = 64
      Width = 75
      Height = 25
      Caption = 'Go Off Bus'
      TabOrder = 2
      OnClick = OffBusBtnClick
    end
    object ReadMsgBtn: TButton
      Left = 24
      Top = 188
      Width = 165
      Height = 25
      Caption = 'Read all pending messages'
      TabOrder = 3
      OnClick = ReadMsgBtnClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 248
    Width = 609
    Height = 217
    Caption = 'Channel B'
    TabOrder = 1
    object ChannelCombo2: TComboBox
      Left = 24
      Top = 20
      Width = 317
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
    end
    object OnBus2Btn: TButton
      Left = 24
      Top = 64
      Width = 75
      Height = 25
      Caption = 'Go OnBus'
      TabOrder = 1
      OnClick = OnBus2BtnClick
    end
    object OffBus2Btn: TButton
      Left = 144
      Top = 64
      Width = 75
      Height = 25
      Caption = 'Go Off Bus'
      TabOrder = 2
      OnClick = OffBus2BtnClick
    end
    object ErrBtn: TButton
      Left = 28
      Top = 120
      Width = 185
      Height = 25
      Caption = 'Send error frame'
      TabOrder = 3
      OnClick = ErrBtnClick
    end
    object Err10_Btn: TButton
      Left = 224
      Top = 120
      Width = 185
      Height = 25
      Caption = 'Send 10 error frames'
      TabOrder = 4
      OnClick = Err10_BtnClick
    end
    object TxMsgBtn: TButton
      Left = 28
      Top = 152
      Width = 185
      Height = 25
      Caption = 'Send one normal message'
      TabOrder = 5
      OnClick = TxMsgBtnClick
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 388
    Top = 24
  end
end
