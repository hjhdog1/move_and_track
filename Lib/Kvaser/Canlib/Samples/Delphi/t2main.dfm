object Form1: TForm1
  Left = 374
  Top = 124
  Width = 580
  Height = 443
  Caption = 'Delphi Test #2'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = True
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 16
  object GroupBox1: TGroupBox
    Left = 312
    Top = 12
    Width = 249
    Height = 393
    Caption = 'Channel 1'
    TabOrder = 0
    object Label3: TLabel
      Left = 136
      Top = 44
      Width = 41
      Height = 16
      Caption = ''
    end
    object Label4: TLabel
      Left = 136
      Top = 88
      Width = 41
      Height = 16
      Caption = ''
    end
    object Button4: TButton
      Left = 28
      Top = 36
      Width = 89
      Height = 33
      Caption = 'Open'
      TabOrder = 0
      OnClick = Button4Click
    end
    object Button5: TButton
      Left = 28
      Top = 80
      Width = 89
      Height = 33
      Caption = 'On bus'
      TabOrder = 1
      OnClick = Button5Click
    end
    object Button6: TButton
      Left = 28
      Top = 124
      Width = 89
      Height = 33
      Caption = 'Send'
      TabOrder = 2
      OnClick = Button6Click
    end
  end
  object GroupBox2: TGroupBox
    Left = 4
    Top = 4
    Width = 289
    Height = 401
    Caption = 'Channel 0'
    TabOrder = 1
    object Label1: TLabel
      Left = 128
      Top = 52
      Width = 41
      Height = 16
      Caption = ''
    end
    object Label2: TLabel
      Left = 128
      Top = 92
      Width = 41
      Height = 16
      Caption = ''
    end
    object Label5: TLabel
      Left = 20
      Top = 192
      Width = 126
      Height = 16
      Caption = 'Received messages'
    end
    object Button1: TButton
      Left = 16
      Top = 44
      Width = 89
      Height = 33
      Caption = 'Open'
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 16
      Top = 84
      Width = 89
      Height = 33
      Caption = 'On bus'
      TabOrder = 1
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 16
      Top = 124
      Width = 89
      Height = 33
      Caption = 'Send'
      TabOrder = 2
      OnClick = Button3Click
    end
    object ListBox1: TListBox
      Left = 16
      Top = 216
      Width = 241
      Height = 97
      ItemHeight = 16
      TabOrder = 3
    end
    object Button7: TButton
      Left = 16
      Top = 324
      Width = 241
      Height = 33
      Caption = 'Read all messages in the queue'
      TabOrder = 4
      OnClick = Button7Click
    end
  end
end
