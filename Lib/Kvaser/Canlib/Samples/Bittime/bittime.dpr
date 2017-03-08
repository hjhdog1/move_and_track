program bittime;

uses
  Forms,
  Btrlist in 'Btrlist.pas' {BtrListDlg};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'CAN Bit Timing';
  Application.CreateForm(TBtrListDlg, BtrListDlg);
  Application.Run;
end.
