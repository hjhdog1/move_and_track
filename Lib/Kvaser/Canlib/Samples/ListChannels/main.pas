unit main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ComCtrls, ImgList, ToolWin, ExtCtrls;

type
  TMainForm = class(TForm)
    ListView: TListView;
    ToolBar1: TToolBar;
    RefreshBtn: TToolButton;
    ImageList1: TImageList;
    Panel1: TPanel;
    procedure FormShow(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure RefreshBtnClick(Sender: TObject);
    procedure ListViewDblClick(Sender: TObject);
    procedure ListViewKeyPress(Sender: TObject; var Key: Char);
    procedure FormKeyPress(Sender: TObject; var Key: Char);
  private
    { Private declarations }
  public
    procedure PopulateList;
    procedure DisplayDetails;
    procedure WM__DeviceChange(var msg: TMessage); message WM_DEVICECHANGE;
  end;

var
  MainForm: TMainForm;

implementation

{$R *.DFM}
{$R ICON.RES}

uses CANLIB, detail;

procedure TMainForm.WM__DeviceChange(var msg: TMessage);
begin
  PopulateList;
  msg.Result := 1;
end;

procedure TMainForm.PopulateList;
var i: Integer;
  numChannels: Integer;
  item: TListItem;
  s: string;
  p: packed array[0..64] of char;
  n: Integer;
  revno: array[0..3] of word;
begin
  canUnloadLibrary;
  canInitializeLibrary;

  canGetNumberOfChannels(numChannels);
  ListView.Items.Clear;
  try
    Screen.Cursor := crHourGlass;
    ListView.Items.BeginUpdate;
    for i := 0 to numChannels - 1 do begin
      item := ListView.Items.Add;

      item.Caption := Format('%d', [i]);

      canGetChannelData(i, canCHANNELDATA_CHAN_NO_ON_CARD, n, sizeof(n));
      item.SubItems.Add(IntToStr(n));

      canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, p, sizeof(p));
      item.SubItems.Add(string(p));

      canGetChannelData(i, canCHANNELDATA_CHANNEL_NAME, p, sizeof(p));
      item.SubItems.Add(string(p));

      canGetChannelData(i, canCHANNELDATA_CARD_TYPE, n, sizeof(n));
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
        s := '???';
      end;
      item.SubItems.Add(s);

      canGetChannelData(i, canCHANNELDATA_CARD_SERIAL_NO, n, sizeof(n));
      item.SubItems.Add(IntToStr(n));

      canGetChannelData(i, canCHANNELDATA_CARD_FIRMWARE_REV, revno, sizeof(revno));
      item.SubItems.Add(Format('%d.%d.%d', [revno[3], revno[2], revno[0]]));
    end;
  finally
    Screen.Cursor := crDefault;
  end;
  ListView.Items.EndUpdate;
end;

procedure TMainForm.FormShow(Sender: TObject);
begin
  PopulateList;
end;

procedure TMainForm.FormCreate(Sender: TObject);
begin
  canInitializeLibrary;
end;

procedure TMainForm.RefreshBtnClick(Sender: TObject);
begin
  PopulateList;
end;

procedure TMainForm.ListViewDblClick(Sender: TObject);
begin
  DisplayDetails;
end;

procedure TMainForm.ListViewKeyPress(Sender: TObject; var Key: Char);
begin
  if Key = chr(13) then DisplayDetails;
end;

procedure TMainForm.DisplayDetails;
begin
  if ListView.SelCount <> 1 then Exit;
  DetailForm.channel := ListView.Selected.Index;
  DetailForm.ShowModal;
end;

procedure TMainForm.FormKeyPress(Sender: TObject; var Key: Char);
begin
  if Key = chr(27) then Close;
end;

end.

