unit CanChan;
(*
**                        Copyright 1998 by KVASER AB            
**                  P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**            E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*)
interface

uses
  SysUtils, WinTypes, WinProcs, Messages, Classes, Graphics, Controls,
  Forms, Dialogs, CANLIB;

const
   ccAnyChannel = -1;

type
   { Error in the DLL }
   ECanError = class(Exception)
               public
                   ErrCode : integer;
                   ErrText : string[50];
                   constructor Create(text, subtext:string; code:integer);
               end;

   { Error detected in this component }
   ECanChan = class(Exception);

   TCanWriteEvent = procedure(Sender : TObject;
                              id : Longint;
                              msg : PChar;
                              dlc, flag : Integer) of object;

   TCanChanOption = (ccActions, ccOwnBuffers, ccErrorCounters, ccExclusive);
   TCanChanOptions = set of TCanChanOption;

   TCanChanCards = (ccAnyCard, ccPCCAN, ccCANCARD, ccAC2, ccLapCAN, ccPCCAN2);
   TCanChanCircuits = (ccAnyCircuit, ccPCCAN_PHILIPS, ccPCCAN_INTEL527,
                       ccPCCAN_VIRTUAL,
                       ccCANCARD_NEC72005, ccCIRC_SJA1000);
   TCanChanDrivers = (ccPUSHPULL, ccTRISTATE);

   TCanChannel = class(TCustomControl)
   private
      { Private declarations }
      FWinHandle  : HWND;
      FCanHnd     : Integer;
      FRXBufSize  : integer;
      FTXBufSize  : integer;
      FActive     : boolean;
      FBusActive  : boolean;
      FOnCanRx    : TNotifyEvent;
      FOnCanTx    : TNotifyEvent;
      FOnCanErr   : TNotifyEvent;
      FOnWrite    : TCanWriteEvent;
      FOnStateChanged : TNotifyEvent;
      FCard       : TCanChanCards;
      FCircuit    : TCanChanCircuits;
      FChannel    : Integer;
      FOptions    : TCanChanOptions;
      FDriver     : TCanChanDrivers;

      FBaud       : Longint;
      FTseg1      : Integer;
      FTseg2      : Integer;
      FSjw        : Integer;
      FNoSamp     : Integer;
      FSyncMode   : Integer;

      FNotifyFlags  : Integer;

      procedure InitVariables;
      procedure getWMessage(var msg : TMessage);
      procedure setSWDescr(Index : Integer; aValue : Integer );
      function  GetSWDescr(Index : Integer): Integer;
      procedure setBusActive(aValue : Boolean);
      procedure setActive(aValue : Boolean);
      procedure setNotifyFlags(aValue : Integer);
      procedure setWinHandle(aValue : HWND);
      procedure setOnCan(Index : Integer; aProc : TNotifyEvent);
      function  getOnCan(Index : Integer) : TNotifyEvent;
      procedure SetCard(card : TCanChanCards);
      procedure SetCircuit(circuit : TCanChanCircuits);
      procedure SetChannel(channel:integer);
      procedure SetOptions(options : TCanChanOptions);
      procedure SetDriver(driver : TCanChanDrivers);
      procedure SetBusParam(index : integer; value : Integer);
      function  GetBusParam(index : Integer):integer;
      procedure SetBaud(value : Longint);

      procedure AssertActive;
      procedure AssertNotActive;
      procedure AssertBusActive;
      procedure AssertNotBusActive;
      procedure UpdateCANCircuitParameters;

   protected
      { Protected declarations }

   public
     { Public declarations }
     constructor Create(AOwner :TComponent); override;
     destructor  Destroy; override;
     procedure   Check(result : Integer; text : String);
     procedure   Open;
     procedure   Close;
     procedure   ErrorCounters(var txErr, rxErr, ovErr : Cardinal);
     function    Write(id : Longint;
                       msg : PChar;
                       dlc, flag : Cardinal):integer;
     function    WriteSync(timeout : Longint):integer;
     function    Read(var id : Longint;
                      msg : PChar;
                      var dlc, flag : Cardinal;
                      var time : Longint):integer;
     function    ReadWait(var id : Longint;
                          msg : PChar;
                          var dlc, flag : Cardinal;
                          var time : Longint; timeout : Longint):integer;
     function    ReadSpecific(id : Longint;
                              msg : PChar;
                              var dlc, flag : Cardinal;
                              var time : Longint):integer;
     function    ReadSync(timeout : Longint):integer;
     function    ReadSyncSpecific(id, timeout : Longint):integer;
     function    ReadTimer: longint;
     function    ReadStatus: longint;

     {Runtime-only properties }
     property CanHnd    : integer         read FCanHnd;
     property WinHandle : HWND            read FWinHandle write setWinHandle;

  published
    { Published declarations }
    property Card      : TCanChanCards read FCard write SetCard;
    property Circuit   : TCanChanCircuits read FCircuit write SetCircuit;
    property Channel   : Integer read FChannel write SetChannel;
    property Options   : TCanChanOptions read FOptions write SetOptions;
    property Active    : Boolean read FActive write SetActive stored False;
    property RxBufSize : Integer index 1 read GetSWDescr write SetSWDescr;
    property TxBufSize : Integer index 2 read GetSWDescr write SetSWDescr;
    property Baud      : Longint read FBaud write SetBaud;
    property TSeg1     : Integer index 1 read GetBusParam write SetBusParam;
    property TSeg2     : Integer index 2 read GetBusParam write SetBusParam;
    property SJW       : Integer index 3 read GetBusParam write SetBusParam;
    property NoSamp    : Integer index 4 read GetBusParam write SetBusParam;
    property SyncMode  : Integer index 5 read GetBusParam write SetBusParam;
    property Driver    : TCanChanDrivers read FDriver write SetDriver;
    property BusActive : Boolean read FBusActive write SetBusActive stored False;
    property NotifyFlags : Integer read FNotifyFlags write setNotifyFlags;
    property OnCanRx   : TNotifyEvent index 1 read getOnCan write setOnCan;
    property OnCanTx   : TNotifyEvent index 2 read getOnCan write setOnCan;
    property OnCanErr  : TNotifyEvent index 3 read getOnCan write setOnCan;
    property OnStateChanged : TNotifyEvent index 4 read getOnCan write setOnCan;
    property OnWrite   : TCanWriteEvent       read FOnWrite write FOnWrite;
  end;

procedure Register;

implementation

procedure Register;
begin
   RegisterComponents('Kvaser', [TCanChannel]);
end;

constructor ECanError.Create(text, subtext:string; code:integer);
begin
   inherited Create(text);
   ErrText := subtext;
   ErrCode := code;
end;

procedure TCanChannel.InitVariables;
begin
   FActive     := False;
   FBusActive  := False;
   FCanHnd     := canINVALID_HANDLE;
   FRXBufSize  := 200;
   FTXBufSize  := 200;
   FCard       := ccAnyCard;
   FCircuit    := ccAnyCircuit;
   FChannel    := ccAnyChannel;
   FOptions    := [];
   FDriver     := ccPUSHPULL;

   FBaud       := 1000000;
   FTseg1      := 4;
   FTseg2      := 3;
   FSjw        := 1;
   FNoSamp     := 1;
   FSyncMode   := 0;

   FNotifyFlags := 0;
end;

{
| Create the component. Initialize the DLL. Don't open the
| circuit. Stay calm.
}
constructor TCanChannel.Create(AOwner : TComponent);
var
   result : Integer;
begin
   inherited Create(AOwner);
   InitVariables;
   if not (csDesigning in ComponentState) then begin
      FWinHandle := AllocateHWnd(getWMessage);
      result := canLocateHardware;
      Check(result, 'CANLIB initialization failed');
   end;
end;

{
| Destroy the component.
}
destructor TCanChannel.Destroy;
begin
   if not (csDesigning in ComponentState) then begin
     DeallocateHWnd(FWinHandle);
     if FActive then Close;
   end;
   inherited Destroy;
end;

procedure TCanChannel.SetCard(card : TCanChanCards);
begin
   AssertNotActive;
   FCard := card;
   if Assigned(FOnStateChanged) then FOnStateChanged(Self);
end;

procedure TCanChannel.SetCircuit(circuit : TCanChanCircuits);
begin
   AssertNotActive;
   FCircuit := circuit;
   if Assigned(FOnStateChanged) then FOnStateChanged(Self);
end;

procedure TCanChannel.SetChannel(channel:integer);
begin
   AssertNotActive;
   FChannel := channel;
   if Assigned(FOnStateChanged) then FOnStateChanged(Self);
end;

procedure TCanChannel.SetOptions(options : TCanChanOptions);
begin
   AssertNotActive;
   FOptions := options;
end;

procedure TCanChannel.SetDriver(driver : TCanChanDrivers);
begin
   AssertNotBusActive;
   FDriver := driver;
end;

procedure TCanChannel.SetBaud(value : Longint);
var tseg1, tseg2, sjw, nosamp, syncmode, stat : Cardinal;
begin
   AssertNotBusActive;
   if value < 0 then begin
      stat := canTranslateBaud(value, tseg1, tseg2, sjw, nosamp, syncmode);
      Check(stat, 'Illegal Bus Parameter Value');
      FTseg1 := tseg1;
      FTseg2 := tseg2;
      FSjw := sjw;
      FNoSamp := nosamp;
      FSyncMode := syncmode;
   end;
   FBaud := value;
   if Assigned(FOnStateChanged) then FOnStateChanged(Self);
end;

procedure TCanChannel.SetBusParam(index : integer; value : Integer);
begin
   AssertNotBusActive;
   case index of
    1: FTseg1      := value;
    2: FTseg2      := value;
    3: FSjw        := value;
    4: FNoSamp     := value;
    5: FSyncMode   := value;
   end;
   if Assigned(FOnStateChanged) then FOnStateChanged(Self);
end;

function  TCanChannel.GetBusParam(index : Integer):integer;
begin
   case index of
    1: Result := FTseg1;
    2: Result := FTseg2;
    3: Result := FSjw;
    4: Result := FNoSamp;
    5: Result := FSyncMode;
   end;
end;

{
| Check the error code; raise appropriate exception.
}
procedure TCanChannel.Check(result : integer; text : string);
var s : array[0..50] of char;
begin
   case result of
      canOK:             exit;
   else
      if Assigned(canGetErrorText) then begin
         canGetErrorText(result, s, sizeof(s));
      end else begin
         StrPCopy(s, Format('Error number %d - no text available.',[result]));
      end;
      raise ECanError.Create(text, StrPas(s), result);
   end;
end;

{
| Raise an exception if the channel is inactive.
}
procedure TCanChannel.AssertActive;
begin
   if not FActive then raise ECanChan.Create('Channel is not active');
end;

{
| Raise an exception if the channel is active.
}
procedure TCanChannel.AssertNotActive;
begin
   if FActive then raise ECanChan.Create('Channel is active');
end;

{
| Raise an exception if the channel is off-bus.
}
procedure TCanChannel.AssertBusActive;
begin
   if not FBusActive then raise ECanChan.Create('Channel is off-bus');
end;

{
| Raise an exception if the channel is on-bus.
}
procedure TCanChannel.AssertNotBusActive;
begin
   if FBusActive then raise ECanChan.Create('Channel is on-bus');
end;

{
| Handle notification messages.
}
procedure TCanChannel.GetWMessage(var msg: TMessage);
var Msg1 : TWMCan;
    stat : longint;
begin
   if msg.Msg = WM__CanLib then begin
      Msg1 := TWMCan(msg);
      if FActive then begin
         case Msg1.minorMsg of
            canEVENT_RX: begin
               if Assigned(FOnCanRx) then FOnCanRx(Self);
            end;

            canEVENT_TX: begin
               if Assigned(FOnCanTx) then FOnCanTx(Self);
            end;

            canEVENT_ERROR: begin
               if Assigned(FOnCanErr) then FOnCanErr(Self);
            end;

            canEVENT_STATUS: begin
               canReadStatus(FCanHnd, stat);
               if (stat and canSTAT_BUS_OFF)<>0 then FBusActive := False;
               if Assigned(FOnStateChanged) then FOnStateChanged(Self);
            end;
         else 
            {Do nothing};
         end;
      end;
   end;
end;

procedure TCanChannel.SetActive(aValue : boolean);
begin
   if (csDesigning in ComponentState) then Exit;
   if aValue then Open else Close;
   if Assigned(FOnStateChanged) then FOnStateChanged(Self);
end;

procedure TCanChannel.UpdateCANCircuitParameters;
var stat : Longint;
    driver : Integer;
begin
   if csDesigning in ComponentState then Exit;
   stat := canSetBusParams(FCanHnd, FBaud, FTseg1, FTseg2,
                           FSjw, FNoSamp, FSyncMode);

   if stat < 0 then begin
      canClose(FCanHnd);
      FCanHnd := canINVALID_HANDLE;
      Check(stat, 'Illegal bus parameters');
   end;

   case FDriver of
      ccPUSHPULL: driver := canPUSHPULL;
      ccTRISTATE: driver := canTRISTATE;
   end;

   stat := canSetBusOutputControl(FCanHnd, driver);
   if stat < 0 then begin
      canClose(FCanHnd);
      FCanHnd := canINVALID_HANDLE;
      Check(stat, 'Illegal Bus Driver Type');
   end;
end;

{
| Open the channel. Set bus parameters. Stay off-bus.
}
procedure TCanChannel.Open;
var
   result : Integer;
   hnd : integer;
   hw : canHWDescr;
   sw : canSWDescr;
   flags : integer;
begin
   if FActive then Exit;

   case FCard of
      ccAnyCard:        hw.CardType := canCARD_ANY;
      ccPCCAN:          hw.CardType := canCARD_PCCAN;
      ccCANCARD:        hw.CardType := canCARD_CANCARD;
      ccAC2:            hw.CardType := canCARD_AC2;
      ccLapCAN:         hw.CardType := canCARD_LAPCAN;
      ccPCCAN2:         hw.CardType := canCARD_PCCAN2;
   end;

   case FCircuit of
      ccAnyCircuit:        hw.CircuitType := canCIRCUIT_ANY;
      ccPCCAN_PHILIPS:     hw.CircuitType := PCCAN_PHILIPS;
      ccPCCAN_INTEL527:    hw.CircuitType := PCCAN_INTEL527;
      ccCANCARD_NEC72005:  hw.CircuitType := CANCARD_NEC72005;
      ccPCCAN_VIRTUAL:     hw.CircuitType := PCCAN_VIRTUAL;
      ccCIRC_SJA1000:      hw.CircuitType := CIRC_SJA1000;
   end;

   hw.Channel := FChannel;

   flags := 0;
   if ccActions in FOptions then flags := flags or canWANT_ACTIONS;
   if ccOwnBuffers in FOptions then flags := flags or canWANT_OWN_BUFFERS;
   if ccErrorCounters in FOptions then flags := flags or canWANT_ERROR_COUNTERS;
   if ccExclusive  in FOptions then flags := flags or canOPEN_EXCLUSIVE;

   { TCanChanDrivers = (ccPUSHPULL, ccTRISTATE); }

   with sw do begin
      rxBufSize := FRxBufSize;
      txBufSize := FTxBufSize;
      alloc := nil;
      dealloc := nil;
   end;

   hnd := canOpen(hw, @sw, flags);
   if hnd < 0 then begin
      Check(hnd, 'canOpen failed');
   end;

   FCanHnd := hnd;
   UpdateCANCircuitParameters;

   result := canSetNotify(hnd, FWinHandle, FNotifyFlags);
   if result < 0 then begin
      canClose(hnd);
      {Throw exception }
      Check(result, 'SetNotify failed in Open');
   end;

   FCanHnd := hnd;
   FActive := True;
   FBusActive := False;
   if Assigned(FOnStateChanged) then FOnStateChanged(Self);
end;

{
| Close the channel.
}
procedure TCanChannel.Close;
var
   result : Integer;
begin
   if FActive then begin
      result := canClose(FCanHnd);
      if not (csDestroying in Componentstate) then begin
         FCanHnd := canINVALID_HANDLE;
         Check(result,'canClose failed');
         FActive := False;
         FBusActive := False;
         if Assigned(FOnStateChanged) then FOnStateChanged(Self);
      end;
   end;
end;

{
| Get the error counters.
}
procedure TCanChannel.ErrorCounters(var txErr, rxErr, ovErr : Cardinal);
var
   result : Integer;
begin
   if FActive then begin
       result := canReadErrorCounters(FCanHnd, txErr, RxErr, ovErr);
       Check(result,'ReadErrCounters failed');
   end;
end;

function  TCanChannel.Write(id : Longint;
                            msg : PChar;
                            dlc, flag : Cardinal):integer;
begin
   Result := canWrite(FCanHnd, id, msg, dlc, flag);
   { "Local echo" - call OnWrite }
   if (result = canOk) and assigned(FOnWrite) then
      FOnWrite(self, id, msg, dlc, flag);
end;

function  TCanChannel.WriteSync(timeout : Longint) : integer;
begin
   Result := canWriteSync(FCanHnd, timeout);
end;

function  TCanChannel.Read(var id : Longint;
                           msg : PChar;
                           var dlc, flag : Cardinal;
                           var time : Longint):integer;
begin
   Result := canRead(FCanHnd, id, msg, dlc, flag,time);
end;

function  TCanChannel.ReadWait(var id : Longint;
                               msg : PChar;
                               var dlc, flag : Cardinal;
                               var time : Longint; timeout : Longint):integer;
begin
   Result := canReadWait(FCanHnd, id, msg, dlc, flag, time, timeout);
end;

function  TCanChannel.ReadSpecific(id : Longint;
                                   msg : PChar;
                                   var dlc, flag : Cardinal;
                                   var time : Longint):integer;
begin
   Result := canReadSpecific(FCanHnd, id, msg, dlc, flag, time);
end;

function  TCanChannel.ReadSync(timeout : Longint):integer;
begin
   Result := canReadSync(FCanHnd, timeout);
end;

function  TCanChannel.ReadSyncSpecific(id, timeout : Longint):integer;
begin
   Result := canReadSyncSpecific(FCanHnd, id, timeout);
end;

{
function  TCanChannel.getBusOpen(Index : Integer) : Integer;
begin
   result := canERR_PARAM;
   case Index of
      1: result := FOpenFlags;
      2: if FSwDescrPtr <> Nil then result := FSwDescrPtr^.rxBufSize;
      3: if FSwDescrPtr <> Nil then result := FSwDescrPtr^.txBufSize;
      else
         result := canERR_PARAM;
   end;
end;
}

procedure TCanChannel.setSWDescr(Index : Integer; aValue : Integer );
begin
   AssertNotActive;
   case Index of
      1: FRXBufSize := aValue;
      2: FTXBufSize := aValue;
   end;
end;

function TCanChannel.GetSWDescr(Index : Integer): Integer;
begin
   case Index of
      1: Result := FRXBufSize;
      2: Result := FTXBufSize;
   end;
end;


procedure TCanChannel.setNotifyFlags(aValue : Integer);
var
   result : Integer;
begin
    FNotifyFlags := aValue;
    if (not (csDesigning in ComponentState)) and FBusActive then begin
      AssertActive;
      result := canSetNotify(FCanHnd, FWinHandle, FNotifyFlags);
      Check(result, 'SetNotifyFlags failed');
   end;
end;

procedure TCanChannel.setWinHandle(aValue : HWND);
var
   result : Integer;
begin
   FWinHandle := aValue;
   if (not (csDesigning in ComponentState)) and FBusActive then begin
      AssertActive;
      result := canSetNotify(FCanHnd, FWinHandle, FNotifyFlags);
      Check(result, 'SetWinHandle failed');
   end;
end;

procedure TCanChannel.SetBusActive(aValue : Boolean);
var
   result : Integer;
begin
   if (csDesigning in ComponentState) or
      (FBusActive = aValue) or
      (not Active) then Exit;

   if aValue then begin
      UpdateCANCircuitParameters;
      if (FNotifyFlags <> 0) then begin
         result := canSetNotify(FCanHnd, FWinHandle, FNotifyFlags);
         Check(result, 'setNotify failed');
      end;
      result := canBusOn(FCanHnd);
      Check(result, 'Could not change state to Bus On');
   end else begin
      result := canBusOff(FCanHnd);
      Check(result, 'Could not change state to Bus Off');
   end;
   FBusActive := aValue;
   if Assigned(FOnStateChanged) then FOnStateChanged(Self);
end;

procedure TCanChannel.setOnCan(Index : Integer; aProc : TNotifyEvent);
begin
   case Index of
      1: FOnCanRx   := aProc;
      2: FOnCanTx   := aProc;
      3: FOnCanErr  := aProc;
      4: FOnStateChanged := aProc;
   end;
   if not Assigned(aProc) then begin
      case Index of
         1: setNotifyFlags(FNotifyFlags and (not canNOTIFY_RX));
         2: setNotifyFlags(FNotifyFlags and (not canNOTIFY_TX));
         3: setNotifyFlags(FNotifyFlags and (not canNOTIFY_ERROR));
         4: setNotifyFlags(FNotifyFlags and (not canNOTIFY_STATUS));
      end;
   end else begin
      case Index of
         1: setNotifyFlags(FNotifyFlags or canNOTIFY_RX);
         2: setNotifyFlags(FNotifyFlags or canNOTIFY_TX);
         3: setNotifyFlags(FNotifyFlags or canNOTIFY_ERROR);
         4: setNotifyFlags(FNotifyFlags or canNOTIFY_STATUS);
      end;
   end;
end;

function TCanChannel.getOnCan(Index : Integer) : TNotifyEvent;
begin
   case Index of
      1:    result := FOnCanRx;
      2:    result := FOnCanTx;
      3:    result := FOnCanErr;
      4:    Result := FOnStateChanged;
   else
            result := nil;
   end;
end;

function  TCanChannel.ReadTimer:longint;
begin
{$IFDEF WIN32}
   Result := canReadTimer(FCanHnd);
{$ELSE}
   Result := canReadTimer;
{$ENDIF}
end;

function TCanChannel.ReadStatus: Longint;
var stat : canStatus;
begin
   stat := canReadStatus(FCanHnd, Result);
   Check(stat, 'Read CAN status failed');
end;

end.
