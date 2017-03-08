unit T1main;

interface

uses
  SysUtils, WinTypes, WinProcs, Messages, Classes, Graphics, Controls,
  Forms, Dialogs, Canlib, CanChanEx, StdCtrls;

type
  TForm1 = class(TForm)
    OpenBtn: TButton;
    Label1: TLabel;
    BusOnBtn: TButton;
    Label2: TLabel;
    SendBtn: TButton;
    procedure FormCreate(Sender: TObject);
    procedure OpenBtnClick(Sender: TObject);
    procedure BusOnBtnClick(Sender: TObject);
    procedure SendBtnClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    can : TCanChannelEx;
    seq : integer;
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.FormCreate(Sender: TObject);
begin
   can := TCanChannelEx.Create(Self);
   seq := 0;
end;

procedure TForm1.OpenBtnClick(Sender: TObject);
begin
   with can do begin
      if not Active then begin
         Bitrate := canBITRATE_500K;
         Channel := 0;
         Open;
      end else begin
         Close;
      end;
      if Active then Label1.Caption := 'Active'
      else Label1.Caption := 'Inactive';
   end;
end;

procedure TForm1.BusOnBtnClick(Sender: TObject);
begin
   with can do begin
      if Active then begin
         BusActive := not BusActive;
         if BusActive then Label2.Caption := 'On Bus'
         else Label2.Caption := 'Off Bus';
      end;
   end;
end;


procedure TForm1.SendBtnClick(Sender: TObject);
var msg : array[0..7] of char;
begin
   { Send a message with (11-bit) id 432, length 8 bytes,
     containing what happens to be present in the msg array
     (probably garbage) }
   can.Write(432, msg, 8, 0);
   inc(Seq);
end;

end.
