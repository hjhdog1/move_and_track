object DetailForm: TDetailForm
  Left = 333
  Top = 319
  Width = 598
  Height = 390
  Caption = 'DetailForm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ListView: TListView
    Left = 0
    Top = 0
    Width = 590
    Height = 363
    Align = alClient
    Columns = <
      item
        Caption = 'Item'
        Width = 130
      end
      item
        Caption = 'Value'
        Width = -2
        WidthType = (
          -2)
      end>
    ReadOnly = True
    TabOrder = 0
    ViewStyle = vsReport
  end
end
