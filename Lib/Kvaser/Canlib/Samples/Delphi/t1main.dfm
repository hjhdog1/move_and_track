object Form1: TForm1
  Left = 338
  Top = 120
  Width = 435
  Height = 300
  Caption = 'Kvaser'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 130
    Top = 33
    Width = 32
    Height = 13
    Caption = ''
  end
  object Label2: TLabel
    Left = 130
    Top = 72
    Width = 32
    Height = 13
    Caption = ''
  end
  object OpenBtn: TButton
    Left = 26
    Top = 26
    Width = 72
    Height = 27
    Caption = 'Open'
    TabOrder = 0
    OnClick = OpenBtnClick
  end
  object BusOnBtn: TButton
    Left = 26
    Top = 65
    Width = 72
    Height = 27
    Caption = 'BusOn'
    TabOrder = 1
    OnClick = BusOnBtnClick
  end
  object SendBtn: TButton
    Left = 26
    Top = 104
    Width = 72
    Height = 27
    Caption = 'Send'
    TabOrder = 2
    OnClick = SendBtnClick
  end
end
