{
| This program demonstrates how the CAN bus status changes with various
| events on the CAN bus.
}
unit main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, CANLIB, ExtCtrls;

type
  TMainForm = class(TForm)
    GroupBox1: TGroupBox;
    Label1: TLabel;
    BusStatusLabel: TLabel;
    Label4: TLabel;
    ErrorCounterLabel: TLabel;
    ChannelCombo1: TComboBox;
    OnBusBtn: TButton;
    OffBusBtn: TButton;
    GroupBox2: TGroupBox;
    ChannelCombo2: TComboBox;
    OnBus2Btn: TButton;
    OffBus2Btn: TButton;
    ErrBtn: TButton;
    Err10_Btn: TButton;
    TxMsgBtn: TButton;
    ReadMsgBtn: TButton;
    Timer1: TTimer;
    procedure FormShow(Sender: TObject);
    procedure OnBusBtnClick(Sender: TObject);
    procedure OffBusBtnClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure OnBus2BtnClick(Sender: TObject);
    procedure OffBus2BtnClick(Sender: TObject);
    procedure ErrBtnClick(Sender: TObject);
    procedure Err10_BtnClick(Sender: TObject);
    procedure TxMsgBtnClick(Sender: TObject);
    procedure ReadMsgBtnClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
  private
    channel_A: Integer;
    hnd_A: Integer;
    channel_B: Integer;
    hnd_B: Integer;
  public
    procedure WMDeviceChange(var msg: TMessage); message WM_DEVICECHANGE;
    procedure WMCanLib(var msg: TMessage); message WM__CANLIB;
    procedure BuildChannelList;
    procedure UpdateBusStatus(stat: Longint);
    procedure UpdateErrorCounters;

  end;

var
  MainForm: TMainForm;

implementation

{$R *.DFM}


procedure TMainForm.WMDeviceChange(var msg: TMessage);
begin
  { If the hardware disappears, rebuild the channel list }
  canClose(hnd_A);
  canClose(hnd_B);
  BuildChannelList;
end;

procedure TMainForm.BuildChannelList;
{ Enumerate all CAN channels and populate the drop-down boxes }
var
  i, maxChan: Integer;
  numChannels: Integer;
  s: string;
  p: packed array[0..64] of char;
  n: Integer;
begin
  canUnloadLibrary;
  canInitializeLibrary;

  canGetNumberOfChannels(numChannels);
  ChannelCombo1.Items.Clear;
  ChannelCombo2.Items.Clear;
  try
    Screen.Cursor := crHourGlass;
    ChannelCombo1.Items.BeginUpdate;
    ChannelCombo2.Items.BeginUpdate;
    maxChan := 0;
    for i := 0 to numChannels - 1 do begin
      canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, p, sizeof(p));
      canGetChannelData(i, canCHANNELDATA_CHAN_NO_ON_CARD, n, sizeof(n));
      s := Format('%s channel %d', [string(p), n + 1]);
      ChannelCombo1.Items.Add(s);
      ChannelCombo2.Items.Add(s);
      maxChan := i;
    end;
  finally
    Screen.Cursor := crDefault;
  end;
  ChannelCombo1.Items.EndUpdate;
  ChannelCombo1.ItemIndex := 0;
  ChannelCombo2.Items.EndUpdate;
  if maxChan > 0 then ChannelCombo2.ItemIndex := 1 else ChannelCombo2.ItemIndex := 0;
end;


procedure TMainForm.FormShow(Sender: TObject);
{ When the main window is displayed for the first time, rebuild
the channel lists. }
begin
  BuildChannelList;
end;

procedure TMainForm.OnBusBtnClick(Sender: TObject);
{ Go on bus on the first channel. }
begin
  channel_A := ChannelCombo1.ItemIndex;
  hnd_A := canOpenChannel(channel_A, canOPEN_ACCEPT_VIRTUAL);
  if hnd_A < 0 then begin
    MessageDlg('Failed to open the selected channel (A)', mtError, [mbOK], 0);
  end;
  canSetNotify(hnd_A, Self.Handle, $FFFFFFFF);
  canBusOn(hnd_A);
end;

procedure TMainForm.WMCanLib(var msg: TMessage);
{ This routine is called when "something" happens
on the CAN bus. Here we handle error frames
and status events on the first channel. }
var
  stat: Longint;
  id: Integer;
  dlc, flags: Cardinal;
  time: longint;
  data: array[0..7] of byte;
begin
  if msg.WParam = hnd_A then begin
    case (msg.LParam and $FFFF) of
      canEVENT_RX:
        begin
          { Read and throw away all available messages on the first channel }
          while canRead(hnd_A, id, @data, dlc, flags, time) = canOK do ;
          canReadStatus(hnd_A, stat);
          UpdateBusStatus(stat);
          UpdateErrorCounters;
        end;
      canEVENT_TX:
        begin
        end;
      canEVENT_ERROR:
        begin
          { An error frame on the first channel }
          UpdateErrorCounters;
        end;
      canEVENT_STATUS:
        begin
          { A status event on the first channel }
          canReadStatus(hnd_A, stat);
          UpdateBusStatus(stat);
          UpdateErrorCounters;
        end;
    end;
  end else begin
    case (msg.LParam and $FFFF) of
      canEVENT_RX:
        begin
        end;
      canEVENT_TX:
        begin
        end;
      canEVENT_ERROR:
        begin
          // UpdateErrorCounters;
        end;
      canEVENT_STATUS:
        begin
          // canReadStatus(hnd, stat);
          // UpdateBusStatus(stat);
          // UpdateErrorCounters;
        end;
    end;
  end;
end;

procedure TMainForm.UpdateBusStatus(stat: Longint);
{ Express the bus status flags as a (more or less) readable string }
var
  s: string;
begin
  s := '';
  if ((stat and canSTAT_ERROR_PASSIVE) <> 0) then s := s + 'EP ';
  if ((stat and canSTAT_BUS_OFF) <> 0) then s := s + 'Boff ';
  if ((stat and canSTAT_ERROR_WARNING) <> 0) then s := s + 'EW ';
  if ((stat and canSTAT_ERROR_ACTIVE) <> 0) then s := s + 'EAct ';
  if ((stat and canSTAT_TX_PENDING) <> 0) then s := s + 'TxPend ';
  if ((stat and canSTAT_RX_PENDING) <> 0) then s := s + 'RxPend ';
  if ((stat and canSTAT_TXERR) <> 0) then s := s + 'TxErr ';
  if ((stat and canSTAT_RXERR) <> 0) then s := s + 'RxErr ';
  if ((stat and canSTAT_HW_OVERRUN) <> 0) then s := s + 'HwOvr ';
  if ((stat and canSTAT_SW_OVERRUN) <> 0) then s := s + 'SwOvr ';
  BusStatusLabel.Caption := s;
end;

procedure TMainForm.UpdateErrorCounters;
{ Display error counters }
var
  tx, rx, ovr: Cardinal;
  s: string;
begin
  canReadErrorCounters(hnd_A, tx, rx, ovr);
  s := Format('Tx Errors = %d, Rx Errors = %d, Overruns = %d', [tx, rx, ovr]);
  ErrorCounterLabel.Caption := s;
end;


procedure TMainForm.OffBusBtnClick(Sender: TObject);
{ Go off bus on the first channel }
begin
  canBusOff(hnd_A);
end;

procedure TMainForm.FormCreate(Sender: TObject);
{ Perform some initialization when the program starts }
begin
  BusStatusLabel.Caption := '';
  ErrorCounterLabel.Caption := '';
end;

procedure TMainForm.OnBus2BtnClick(Sender: TObject);
{ Go on bus on the second channel. }
begin
  channel_B := ChannelCombo2.ItemIndex;
  hnd_B := canOpenChannel(channel_B, canOPEN_ACCEPT_VIRTUAL);
  if hnd_B < 0 then begin
    MessageDlg('Failed to open the selected channel (B)', mtError, [mbOK], 0);
  end;
  canSetNotify(hnd_B, Self.Handle, $FFFFFFFF);
  canBusOn(hnd_B);
end;

procedure TMainForm.OffBus2BtnClick(Sender: TObject);
{ Go off bos on the second channel }
begin
  canBusOff(hnd_B);
end;

procedure TMainForm.ErrBtnClick(Sender: TObject);
{ Send error frame on the second channel }
begin
  canWrite(hnd_B, 0, nil, 0, canMSG_ERROR_FRAME);
end;

procedure TMainForm.Err10_BtnClick(Sender: TObject);
{ Send 10 error frames on the second channel }
var
  i: Integer;
begin
  for i := 1 to 10 do begin
    canWrite(hnd_B, 0, nil, 0, canMSG_ERROR_FRAME);
  end;
end;

procedure TMainForm.TxMsgBtnClick(Sender: TObject);
{ Send one regular message on the second channel }
begin
  canWrite(hnd_B, 100, nil, 0, 0);
end;

procedure TMainForm.ReadMsgBtnClick(Sender: TObject);
{ Read all available messages on the first channel and
throw them away, to empty the queue. }
var
  id: Integer;
  dlc, flags: Cardinal;
  time: longint;
  data: array[0..7] of byte;
begin
  while canRead(hnd_A, id, @data, dlc, flags, time) = canOK do ;
end;

procedure TMainForm.Timer1Timer(Sender: TObject);
{ Periodically (once a second) request that the driver
reports the current bus status. }
begin
  canRequestChipStatus(hnd_A);
end;

end.

