object Form1: TForm1
  Left = 330
  Top = 116
  Caption = 'Kvaser'
  ClientHeight = 611
  ClientWidth = 830
  Color = clBtnFace
  Constraints.MaxWidth = 846
  Constraints.MinHeight = 600
  Constraints.MinWidth = 846
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 416
    Height = 592
    Align = alLeft
    Caption = 'Channel1'
    TabOrder = 0
    ExplicitLeft = -4
    ExplicitTop = -6
    DesignSize = (
      416
      592)
    object Channel1Combo: TComboBox
      Left = 13
      Top = 20
      Width = 361
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = Channel1ComboChange
    end
    object RadioGroup1: TRadioGroup
      Left = 13
      Top = 44
      Width = 169
      Height = 78
      Caption = 'Speed'
      ItemIndex = 0
      Items.Strings = (
        '125k'
        '250k'
        '500k')
      TabOrder = 1
      OnClick = RadioGroup1Click
    end
    object Panel1: TPanel
      Left = 2
      Top = 549
      Width = 412
      Height = 41
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 2
      ExplicitLeft = 48
      ExplicitTop = 504
      ExplicitWidth = 185
      object Button1: TButton
        Left = 93
        Top = 11
        Width = 61
        Height = 20
        Caption = 'On Bus'
        TabOrder = 0
        OnClick = Button1Click
      end
      object Button2: TButton
        Left = 160
        Top = 11
        Width = 60
        Height = 20
        Caption = 'Off Bus'
        Enabled = False
        TabOrder = 1
        OnClick = Button2Click
      end
      object Button5: TButton
        Left = 290
        Top = 11
        Width = 60
        Height = 20
        Caption = 'Send'
        Enabled = False
        TabOrder = 2
        OnClick = Button5Click
      end
      object Button7: TButton
        Left = 11
        Top = 11
        Width = 61
        Height = 20
        Caption = 'clear'
        TabOrder = 3
        OnClick = Button7Click
      end
    end
    object ListView2: TListView
      Left = 13
      Top = 140
      Width = 361
      Height = 403
      Anchors = [akLeft, akTop, akRight, akBottom]
      Columns = <
        item
          Caption = 'Id'
        end
        item
          Caption = 'dlc'
        end
        item
          Caption = 'time'
        end>
      TabOrder = 3
      ViewStyle = vsReport
    end
  end
  object GroupBox2: TGroupBox
    Left = 416
    Top = 0
    Width = 414
    Height = 592
    Align = alClient
    Caption = 'Channel2'
    TabOrder = 1
    ExplicitLeft = 410
    ExplicitWidth = 321
    ExplicitHeight = 557
    DesignSize = (
      414
      592)
    object Channel2Combo: TComboBox
      Left = 13
      Top = 20
      Width = 364
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = Channel2ComboChange
    end
    object RadioGroup2: TRadioGroup
      Left = 13
      Top = 44
      Width = 163
      Height = 78
      Caption = 'Speed'
      ItemIndex = 0
      Items.Strings = (
        '125k'
        '250k'
        '500k')
      TabOrder = 1
      OnClick = RadioGroup2Click
    end
    object Panel2: TPanel
      Left = 2
      Top = 549
      Width = 410
      Height = 41
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 2
      ExplicitLeft = 160
      ExplicitTop = 536
      ExplicitWidth = 185
      object onBus2: TButton
        Left = 106
        Top = 11
        Width = 61
        Height = 20
        Caption = 'On Bus'
        TabOrder = 0
        OnClick = onBus2Click
      end
      object OffBus2: TButton
        Left = 178
        Top = 11
        Width = 60
        Height = 20
        Caption = 'Off Bus'
        Enabled = False
        TabOrder = 1
        OnClick = OffBus2Click
      end
      object Send2: TButton
        Left = 310
        Top = 11
        Width = 60
        Height = 20
        Caption = 'Send'
        Enabled = False
        TabOrder = 2
        OnClick = Send2Click
      end
      object Button8: TButton
        Left = 14
        Top = 11
        Width = 61
        Height = 20
        Caption = 'clear'
        TabOrder = 3
        OnClick = Button8Click
      end
    end
    object ListView1: TListView
      Left = 16
      Top = 140
      Width = 361
      Height = 403
      Anchors = [akLeft, akTop, akRight, akBottom]
      Columns = <
        item
          Caption = 'Id'
        end
        item
          Caption = 'dlc'
        end
        item
          Caption = 'time'
        end>
      TabOrder = 3
      ViewStyle = vsReport
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 592
    Width = 830
    Height = 19
    Panels = <
      item
        Text = 'Channel1: Off bus'
        Width = 300
      end
      item
        Text = 'Channel2: Off bus'
        Width = 50
      end>
    ExplicitTop = 549
    ExplicitWidth = 879
  end
end
