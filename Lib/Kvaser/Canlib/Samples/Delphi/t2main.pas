unit T2main;
{
| Note: this application manually creates the CanChannel components.
| This is for testing purposes only and is not recommended practice.
}
interface

uses
  SysUtils, WinTypes, WinProcs, Messages, Classes, Graphics, Controls,
  Forms, Dialogs, CanChanEx, StdCtrls;

type
  TForm1 = class(TForm)
    GroupBox1: TGroupBox;
    Label3: TLabel;
    Label4: TLabel;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    GroupBox2: TGroupBox;
    Label1: TLabel;
    Label2: TLabel;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    ListBox1: TListBox;
    Button7: TButton;
    Label5: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure Button7Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
    CanChannel1: TCanChannelEx;
    CanChannel2: TCanChannelEx;
    procedure CanChannel1CanRx(Sender: TObject);
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}
uses
  CANLIB;

procedure TForm1.Button1Click(Sender: TObject);
begin
  with CanChannel1 do begin
    if not Active then begin
      Channel := 0;
      Bitrate := canBITRATE_1M;
      Open;
      CanChannel1.OnCanRx := CanChannel1CanRx;
    end else begin
      Close;
    end;
    if Active then Label1.Caption := 'Active' else Label1.Caption := 'Inactive';
  end;
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
  with CanChannel1 do begin
    if not Active then Exit;
    BusActive := not BusActive;
    if BusActive then Label2.Caption := 'On bus' else Label2.Caption := 'Off bus';
  end;
end;

procedure TForm1.Button3Click(Sender: TObject);
var msg: array[0..7] of char;
begin
  with CanChannel1 do begin
    Check(Write(438, msg, sizeof(msg), 0), 'Write failed');
  end;
end;

procedure TForm1.Button4Click(Sender: TObject);
begin
  with CanChannel2 do begin
    Channel := 1;
    Bitrate := canBITRATE_1M;
    Active := not Active;
    if Active then Label3.Caption := 'Active' else Label3.Caption := 'Inactive';
  end;
end;

procedure TForm1.Button5Click(Sender: TObject);
begin
  with CanChannel2 do begin
    if not Active then Exit;
    BusActive := not BusActive;
    if BusActive then Label4.Caption := 'On bus' else Label4.Caption := 'Off bus';
  end;
end;

procedure TForm1.Button6Click(Sender: TObject);
var msg: array[0..7] of char;
begin
  with CanChannel2 do begin
    Check(Write(765, msg, sizeof(msg), 0), 'Write failed');
    Check(Write(766, msg, sizeof(msg), 0), 'Write failed');
    Check(Write(767, msg, sizeof(msg), 0), 'Write failed');
    Check(Write(768, msg, sizeof(msg), 0), 'Write failed');
  end;

end;

procedure TForm1.CanChannel1CanRx(Sender: TObject);
var
  dlc, flag, time: cardinal;
  msg: array[0..7] of char;
  id: longint;
begin
  with CanChannel1 do begin
    while Read(id, msg, dlc, flag, time) >= 0 do begin
      ListBox1.Items.Add(Format('Id=%d Len=%d', [id, dlc]));
    end;
  end;
end;

procedure TForm1.Button7Click(Sender: TObject);
var dlc, flag, time: cardinal;
  msg: array[0..7] of char;
  id: longint;
begin
  with CanChannel1 do begin
    while Read(id, msg, dlc, flag, time) >= 0 do begin
      ListBox1.Items.Add(Format('Id=%d Len=%d', [id, dlc]));
    end;
  end;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  CanChannel1 := TCanChannelEx.Create(Self);
  CanChannel2 := TCanChannelEx.Create(Self);
end;

end.

