program Test2;

uses
  Forms,
  T2main in 'T2MAIN.PAS' {Form1},
  CanChanEx in 'canchanex.pas';

{$R *.RES}

begin
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
