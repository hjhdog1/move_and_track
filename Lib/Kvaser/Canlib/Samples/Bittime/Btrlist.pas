unit Btrlist;
(*
**                        Copyright 1998 by KVASER AB
**                  P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**            E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*)
(*
** This program lists all possible bus parameter settings given a clock
** frequency and the desired bus speed, and displays them in a table.
*)

interface

uses WinTypes, WinProcs, Classes, Graphics, Forms, Controls, Buttons,
  StdCtrls, ExtCtrls, Menus, ComCtrls;

type
  TBittimingData = record
    prescaler: byte;
    tseg1: byte;
    tseg2: byte;
    sjw: byte;
    sp: double;
    speed: Cardinal;
  end;

type
  TBtrListDlg = class(TForm)
    PopupMenu1: TPopupMenu;
    Tolerance051: TMenuItem;
    Tolerance151: TMenuItem;
    ListView1: TListView;
    Panel2: TPanel;
    Label1: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    Edit1: TEdit;
    Edit2: TEdit;
    Panel1: TPanel;
    PanelPopup: TPopupMenu;
    CopytoClipboard1: TMenuItem;
    Image1: TImage;
    Button1: TButton;
    N1: TMenuItem;
    Tolerance101: TMenuItem;
    Tolerance251: TMenuItem;
    ShowExactValuesOnly1: TMenuItem;
    procedure Edit1Change(Sender: TObject);
    procedure Edit2Change(Sender: TObject);
    procedure Tolerance1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure ListView1Compare(Sender: TObject; Item1, Item2: TListItem;
      Data: Integer; var Compare: Integer);
    procedure ListView1ColumnClick(Sender: TObject; Column: TListColumn);
    procedure ListView1SelectItem(Sender: TObject; Item: TListItem;
      Selected: Boolean);
    procedure ListView1DblClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormKeyPress(Sender: TObject; var Key: Char);
    procedure CopytoClipboard1Click(Sender: TObject);
    procedure Image1Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure FormShow(Sender: TObject);
  private
    FBitRate: Longint;
    FClockFreq: Longint;
    FSelectedBittiming: TBittimingData;
    FTolerance: Double;
    FSortDirection: Integer;
    FSortColumn: Integer;
    FMaxPrescaler: Integer;
    procedure ListAllBittimings(s: TListItems; clock, bitrate: longint; tolerance: Double);
    procedure SetBitRate(x: Longint);
    procedure SetCLockFreq(x: Longint);
    procedure DoCalc;
  public
    { Public declarations }
    property Bitrate: longint read FBitRate write SetBitRate;
    property ClockFreq: longint read FClockFreq write SetClockFreq;
    property SelectedBittiming: TBittimingData read FSelectedBittiming;
  end;

var
  BtrListDlg: TBtrListDlg;

implementation

{$R *.DFM}
uses
  SysUtils, Clipbrd, ShellAPI, Dialogs;

procedure TBtrListDlg.ListAllBittimings(s: TListItems; clock, bitrate: longint; tolerance: Double);
var
  tmp, tmp2, err: double;
  presc, btq, t1, t2, sjw: integer;
  itm: TListItem;
begin
  s.BeginUpdate;
  s.Clear;
  if bitrate = 0 then Exit;
  tmp := clock / bitrate;
  for presc := 1 to FMaxPrescaler do begin
    tmp2 := tmp / presc;
    btq := Round(tmp2);
    if btq in [4..32] then begin
      if abs(tmp2 / btq - 1) > tolerance then Continue;
      err := - (tmp2 / btq - 1);
      for t1 := 3 to 17 do begin
        t2 := btq - t1;
        if (t1 < t2) or (t2 > 8) or (t2 < 2) then Continue;
        for sjw := 1 to 4 do begin
          itm := s.Add;
          itm.Caption := Format('%2d', [t1]);
          itm.SubItems.Add(Format('%2d', [t2]));
          itm.SubItems.Add(Format('%2.1f', [ t1 / btq * 100]));
          itm.SubItems.Add(Format('%2d', [btq]));
          itm.SubItems.Add(Format('%2d', [sjw]));
          itm.SubItems.Add(Format('%3.3f', [bitrate*(1-err)/1000]));
          itm.SubItems.Add(Format('%2.2f', [-err*100]));
        end;
      end;
    end;
  end;
  s.EndUpdate;
end;


procedure TBtrListDlg.SetBitRate(x: Longint);
begin
  FBitRate := x;
  Edit1.Text := FloatToStrF(FBitRate / 1000, ffFixed, 7, 2);
  DoCalc;
end;

procedure TBtrListDlg.SetClockFreq(x: Longint);
begin
  FClockFreq := x;
  Edit2.Text := FloatToStrF(x / 1000000, ffFixed, 7, 4);
  DoCalc;
end;

procedure TBtrListDlg.Edit1Change(Sender: TObject);
var
  val: longint;
begin
  Panel1.Caption := '';
  val := bitRate;
  try
    val := Round(StrToFloat(Edit1.Text) * 1000);
    if val = 0 then val := bitRate;
  except
    on E: EConvertError do begin
      val := bitRate;
      Panel1.Caption := 'Error: "' + Edit1.Text + '" is not a valid bit rate, using '+FloatToStr(bitRate/1000)+' kbit/s';
    end;
  end;
  FBitRate := val;
  DoCalc;
end;

procedure TBtrListDlg.DoCalc;
begin
  ListAllBittimings(ListView1.Items, Round(ClockFreq/2), Bitrate, FTolerance);
end;

procedure TBtrListDlg.Edit2Change(Sender: TObject);
var
  val: longint;
begin
  Panel1.Caption := '';
  val := ClockFreq;
  try
    val := Round(StrToFloat(Edit2.Text) * 1000000);
  except
    on E: EConvertError do begin
      val := ClockFreq;
      Panel1.Caption := 'Error: "' + Edit2.Text + '" is not a valid clock frequency, using '+FloatToStr(ClockFreq/1000000)+' MHz';
    end;
  end;
  FClockFreq := val;
  DoCalc;
end;

procedure TBtrListDlg.Tolerance1Click(Sender: TObject);
var i: Integer;
begin
  for i := 0 to PopupMenu1.Items.Count-1 do begin
    PopupMenu1.Items[i].Checked := False;
  end;
  FTolerance := (Sender as TMenuItem).Tag / 1000.0;
  (Sender as TMenuItem).Checked := True;
  DoCalc;
end;

procedure TBtrListDlg.FormCreate(Sender: TObject);
var i: Integer;
begin
  FClockFreq := 16000000;
  BitRate := 250000;
  ClockFreq := FClockFreq;
  FTolerance := 0;
  FSortColumn := 0;
  FSortDirection := 0;
  FMaxPrescaler := 64;
  for i:=1 to ParamCount do begin
     if Copy(ParamStr(i), 1, 2) = '-p' then begin
       try
         FMaxPrescaler := StrToInt(Copy(ParamStr(i), 3, 255));
       except
       end;
     end;
  end;
end;

procedure TBtrListDlg.ListView1Compare(Sender: TObject; Item1,
  Item2: TListItem; Data: Integer; var Compare: Integer);
begin
  if FSortColumn = 0 then begin
    Compare := CompareStr(Item1.Caption, Item2.Caption);
  end else begin
    Compare := CompareStr(Item1.SubItems[FSortColumn - 1], Item2.SubItems[FSortColumn - 1]);
  end;
  if FSortDirection <> 0 then Compare := -Compare;

end;

procedure TBtrListDlg.ListView1ColumnClick(Sender: TObject;
  Column: TListColumn);
begin
  if FSortColumn = Column.Index then FSortDirection := FSortDirection xor 1;
  FSortColumn := Column.Index;
  ListView1.CustomSort(nil, FSortDirection);
end;

procedure TBtrListDlg.ListView1SelectItem(Sender: TObject; Item: TListItem;
  Selected: Boolean);
begin
  if Selected then with FSelectedBittiming do begin
    tseg1 := StrToInt(Item.Caption);
    tseg2 := StrToInt(Item.SubItems[0]);
    sjw := StrToInt(Item.SubItems[3]);
    prescaler := Round(FClockFreq/2) div (FBitRate * (tseg1 + tseg2));
    sp := StrToFloat(Item.SubItems[1]);
    speed := Round(StrToFloat(Item.SubItems[4])*1000);
    Panel1.Caption := Format('canSetBusParams(hnd, %d, %d, %d, %d, %d, %d);',
      [speed, tseg1-1, tseg2, sjw, 1, 0]);
  end;
end;

procedure TBtrListDlg.ListView1DblClick(Sender: TObject);
begin
   ModalResult := mrOK;
end;

procedure TBtrListDlg.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  if ListView1.Selected = nil then ModalResult := mrCancel;
end;

procedure TBtrListDlg.FormKeyPress(Sender: TObject; var Key: Char);
begin
  if Key = chr(27) then Close;
end;

procedure TBtrListDlg.CopytoClipboard1Click(Sender: TObject);
begin
  Clipboard.AsText := Panel1.Caption;
end;

procedure TBtrListDlg.Image1Click(Sender: TObject);
begin
ShellExecute (0, 'open',
 'http://www.kvaser.com',
 nil, nil, SW_NORMAL);

end;

procedure TBtrListDlg.Button1Click(Sender: TObject);
begin
   MessageDlg('Bit rate = the desired CAN bus bit rate, in kbit/s'#10#13 +
              'CAN Clock = the frequency of the clock signal fed to the CAN controller'#10#13 +
              '  NOTE: it is assumed that this clock is divided by 2 inside the CAN controller'#10#13 +
              '  So if you are using a 16 MHz oscillator to feed, for example, a SJA1000, enter'#10#13 +
              '  "16" in this box.'#10#13 +
              ''#10#13 + 
              'TSeg1 = the number of bit quanta before the sampling point'#10#13 +
              'including the start bit.'#10#13 +
              'TSeg2 = the number of bit quanta after the sampling point.'#10#13 +
              'SP% = the position of the sampling point, in percent of the whole bit.'#10#13 +
              'BTQ = the number of bit quanta in a bit.'#10#13 +
              'SJW = the maximal number of bit quanta that the CAN controller can'#10#13 +
              'use to resynchronize the clock.'#10#13 +
              'Speed = the resulting bus speed, in bits per second.'#10#13 +
              'Err% = the deviation between the desired bus speed and the resulting bus speed.'#10#13 +
              ''#10#13 +
              'Right-click in the list to change the maximal error for the listed items.'#10#13 +
              ''#10#13 +
              'Below the table, you can see the corresponding call to canSetBusParams().'#10#13 +
              'Right-click to copy the text to the clipboard.'#10#13 +
              ''#10#13,

      mtInformation, [mbOK], 0);
end;

procedure TBtrListDlg.FormShow(Sender: TObject);
begin
   ShowExactValuesOnly1.OnClick(ShowExactValuesOnly1);
end;

end.

