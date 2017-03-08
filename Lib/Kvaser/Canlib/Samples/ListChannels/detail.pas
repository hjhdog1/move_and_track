unit detail;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ComCtrls, ExtCtrls;

type
  TDetailForm = class(TForm)
    ListView: TListView;
    procedure FormKeyPress(Sender: TObject; var Key: Char);
    procedure FormShow(Sender: TObject);
  private
    function TransceiverTypeToString(n: Integer): string;
    procedure UpdateTable;
  public
    channel: Integer;
  end;

var
  DetailForm: TDetailForm;

implementation

{$R *.DFM}

uses CANLIB;

procedure TDetailForm.FormKeyPress(Sender: TObject; var Key: Char);
begin
  if Key = chr(27) then Close;
end;

function TDetailForm.TransceiverTypeToString(n: Integer): string;
begin
  case n of
    canTRANSCEIVER_TYPE_UNKNOWN: Result := 'Undefined - defaults to 251';
    canTRANSCEIVER_TYPE_251: Result := '82c251';
    canTRANSCEIVER_TYPE_252: Result := '82c252';
    canTRANSCEIVER_TYPE_DNOPTO: Result := 'Optoisolated 82c251';
    canTRANSCEIVER_TYPE_W210: Result := 'W210';
    canTRANSCEIVER_TYPE_SWC_PROTO: Result := 'SWC prototype';
    canTRANSCEIVER_TYPE_SWC: Result := 'SWC';
    canTRANSCEIVER_TYPE_EVA: Result := 'EVA board';
    canTRANSCEIVER_TYPE_FIBER: Result := '82c251 w/ glass fibre';
    canTRANSCEIVER_TYPE_K251: Result := '82C251 + K-line';
    canTRANSCEIVER_TYPE_K: Result := 'K-Line';
    canTRANSCEIVER_TYPE_1054_OPTO: Result := '1054 optoisolated';
    canTRANSCEIVER_TYPE_SWC_OPTO: Result := 'SWC optoisolated';
    canTRANSCEIVER_TYPE_TT: Result := 'Truck&Trailer';
    canTRANSCEIVER_TYPE_1050: Result := '1050';
    canTRANSCEIVER_TYPE_1050_OPTO: Result := '1050 optoisolated';
    canTRANSCEIVER_TYPE_1041: Result := '1041';
    canTRANSCEIVER_TYPE_1041_OPTO: Result := '1041 optoisolated';
    canTRANSCEIVER_TYPE_RS485: Result := 'RS485';
    canTRANSCEIVER_TYPE_LIN: Result := 'LIN';
    canTRANSCEIVER_TYPE_KONE: Result := 'KONE';
  else
    Result := 'Unknown';
  end;
end;

procedure TDetailForm.FormShow(Sender: TObject);
begin
  try
    Screen.Cursor := crHourGlass;
    UpdateTable;
  finally
    Screen.Cursor := crDefault;
  end;
end;

procedure TDetailForm.UpdateTable;
var
  item: TListItem;
  s: string;
  p: packed array[0..64] of char;
  n: Integer;
  revno: array[0..3] of word;
  i64: int64;
begin
  Caption := Format('Details for channel %d', [channel]);
  ListView.Items.Clear;

  item := ListView.Items.Add;
  item.Caption := 'Channel';
  item.SubItems.Add(Format('%d', [channel]));

  item := ListView.Items.Add;
  item.Caption := 'Channel Name';
  canGetChannelData(channel, canCHANNELDATA_CHANNEL_NAME, p, sizeof(p));
  item.SubItems.Add(string(p));

  item := ListView.Items.Add;
  item.Caption := 'Channel on Card';
  canGetChannelData(channel, canCHANNELDATA_CHAN_NO_ON_CARD, n, sizeof(n));
  item.SubItems.Add(IntToStr(n));

  item := ListView.Items.Add;
  item.Caption := 'Card Number';
  canGetChannelData(channel, canCHANNELDATA_CARD_NUMBER, n, sizeof(n));
  item.SubItems.Add(IntToStr(n));

  canGetChannelData(channel, canCHANNELDATA_CARD_TYPE, n, sizeof(n));
  case n of
        canHWTYPE_VIRTUAL: s := 'Virtual';
        canHWTYPE_LAPCAN: s := 'LAPcan';
        canHWTYPE_PCCAN: s := 'PCcan';
        canHWTYPE_PCICAN: s := 'PCIcan';
        canHWTYPE_USBCAN: s := 'USBcan';
        canHWTYPE_PCICAN_II: s := 'PCIcan II';
        canHWTYPE_USBCAN_II: s := 'USBcan II';
        canHWTYPE_SIMULATED: s := 'Simulated';
        canHWTYPE_ACQUISITOR: s := 'Acquisitor';
        canHWTYPE_BLACKBIRD_V2: s:= 'BBv2';
        canHWTYPE_EAGLE: s:= 'Eagle';
        canHWTYPE_USBCAN_KLINE: s:= 'K-line';
        canHWTYPE_LEAF: s := 'Kvaser Leaf';
        canHWTYPE_PC104_PLUS: s := 'Kvaser PC104+';
        canHWTYPE_PCICANX_II: s := 'Kvaser PCIcanx II';
        canHWTYPE_MEMORATOR_PRO: s := 'Kvaser Memorator Professional';
        canHWTYPE_USBCAN_PRO: s := 'Kvaser USBcan Professional';
        canHWTYPE_IRIS: s := 'Iris';
        canHWTYPE_MINIPCIE: s := 'Kvaser Mini PCI Express';
        canHWTYPE_ETHERCAN: s := 'Kvaser Ethercan';
        canHWTYPE_USBCAN_LIGHT: s := 'Kvaser USBcan Light';
        canHWTYPE_UVP: s :=  'UVP';
  else
    s := 'Unknown type';
  end;
  s := s + Format(', %d', [n]);
  item := ListView.Items.Add;
  item.Caption := 'Hardware Type';
  item.SubItems.Add(s);

  item := ListView.Items.Add;
  item.Caption := 'Serial Number';
  canGetChannelData(channel, canCHANNELDATA_CARD_SERIAL_NO, n, sizeof(n));
  item.SubItems.Add(IntToStr(n));

  item := ListView.Items.Add;
  item.Caption := 'Display Name';
  canGetChannelData(channel, canCHANNELDATA_DEVDESCR_ASCII, p, sizeof(p));
  item.SubItems.Add(string(p));

  item := ListView.Items.Add;
  item.Caption := 'Manufacturer';
  canGetChannelData(channel, canCHANNELDATA_MFGNAME_ASCII, p, sizeof(p));
  item.SubItems.Add(string(p));

  item := ListView.Items.Add;
  item.Caption := 'Firmware Revision';
  canGetChannelData(channel, canCHANNELDATA_CARD_FIRMWARE_REV, revno, sizeof(revno));
  item.SubItems.Add(Format('%d.%d.%d', [revno[3], revno[2], revno[0]]));

  item := ListView.Items.Add;
  item.Caption := 'Hardware Revision';
  canGetChannelData(channel, canCHANNELDATA_CARD_HARDWARE_REV, revno, sizeof(revno));
  item.SubItems.Add(Format('%d.%d', [revno[1], revno[0]]));

  item := ListView.Items.Add;
  item.Caption := 'Card EAN';
  canGetChannelData(channel, canCHANNELDATA_CARD_UPC_NO, revno, sizeof(revno));
  item.SubItems.Add(Format('%4.4x%4.4x%4.4x%4.4x', [revno[3], revno[2], revno[1], revno[0]]));

  item := ListView.Items.Add;
  item.Caption := 'Transceiver EAN';
  canGetChannelData(channel, canCHANNELDATA_TRANS_UPC_NO, revno, sizeof(revno));
  item.SubItems.Add(Format('%4.4x%4.4x%4.4x%4.4x', [revno[3], revno[2], revno[1], revno[0]]));

  item := ListView.Items.Add;
  item.Caption := 'Transceiver S/N';
  canGetChannelData(channel, canCHANNELDATA_TRANS_SERIAL_NO, i64, sizeof(i64));
  item.SubItems.Add(IntToStr(i64));

  item := ListView.Items.Add;
  item.Caption := 'DLL File Version';
  canGetChannelData(channel, canCHANNELDATA_DLL_FILE_VERSION, revno, sizeof(revno));
  item.SubItems.Add(Format('%d.%d.%d.%d', [revno[3], revno[2], revno[1], revno[0]]));

  item := ListView.Items.Add;
  item.Caption := 'DLL Product Version';
  canGetChannelData(channel, canCHANNELDATA_DLL_PRODUCT_VERSION, revno, sizeof(revno));
  item.SubItems.Add(Format('%d.%d.%d.%d', [revno[3], revno[2], revno[1], revno[0]]));

  item := ListView.Items.Add;
  item.Caption := 'Transciver Type';
  canGetChannelData(channel, canCHANNELDATA_TRANS_TYPE, n, sizeof(n));
  item.SubItems.Add(Format('%s (%d)', [TransceiverTypeToString(n), n]));

  item := ListView.Items.Add;
  item.Caption := 'Physical Position';
  canGetChannelData(channel, canCHANNELDATA_DEVICE_PHYSICAL_POSITION, n, sizeof(n));
  item.SubItems.Add(Format('%d (0x%8.8x)', [n, n]));

  item := ListView.Items.Add;
  item.Caption := 'UI Number';
  canGetChannelData(channel, canCHANNELDATA_UI_NUMBER, n, sizeof(n));
  item.SubItems.Add(Format('%d (0x%8.8x)', [n, n]));

  item := ListView.Items.Add;
  item.Caption := 'Timesync Enabled';
  canGetChannelData(channel, canCHANNELDATA_TIMESYNC_ENABLED, n, sizeof(n));
  item.SubItems.Add(Format('%d', [n]));

  item := ListView.Items.Add;
  item.Caption := 'DLL Type';
  canGetChannelData(channel, canCHANNELDATA_DLL_FILETYPE, n, sizeof(n));
  item.SubItems.Add(Format('%d', [n]));

  item := ListView.Items.Add;
  item.Caption := 'Driver File Version';
  canGetChannelData(channel, canCHANNELDATA_DRIVER_FILE_VERSION, revno, sizeof(revno));
  item.SubItems.Add(Format('%d.%d.%d.%d', [revno[3], revno[2], revno[1], revno[0]]));

  item := ListView.Items.Add;
  item.Caption := 'Driver Product Version';
  canGetChannelData(channel, canCHANNELDATA_DRIVER_PRODUCT_VERSION, revno, sizeof(revno));
  item.SubItems.Add(Format('%d.%d.%d.%d', [revno[3], revno[2], revno[1], revno[0]]));


end;

end.

