(readme.txt for 32-bit can, canchan, and canchanex units for Delphi 4 and 5)

CANLIB.PAS is a Pascal translation of canlib.h and canstat.h. It also
contains some extra code to load the canlib32.dll driver dll dynamically.

CanChanEx.PAS is a Delphi component (TCanChanEx) which encapsulates a
handle to a CAN circuit. It requires CANLIB.PAS.

CanChan.pas is an older version of canchanex.pas. They are not compatible
and canchan.pas is present for backwards compatibility only.


INSTALLATION

Refer to the Delphi documentation for an in-detail description on how
to install components.


USAGE

Refer to the sample programs and the source code - but at a minimum
you want do do this:

first drop the TCanChanEx component onto a form, then

- set the Channel property to the number of the channel you want to
  use (0,1,2...) - the channel number is dependent on the hardware you
  are using

- set the Active property to True (or call Open),

- set the Bitrate property to the desired baud rate (you can use the
  predefined constants canBITRATE_1M, canBITRATE_500K, canBITRATE_250K, and
  canBITRATE_125K if you don't want to fiddle around with the rest of bus
  timing parameters).

- set the property BusActive to True

... and then use the functions Write(...) and Read(...) to send and
receive messages.  The OnCanRx event handler will be called when a
message arrives.  There might be more than one message in the queue
when OnCanRx is called.



TCanChannelEx Reference
=======================

The component is a rather thin wrapper around the CANLIB32 API so most of
the functionality of this component is described in the canlib.chm help file.

Properties

   -- Run-time only --

    CanHnd: integer;
        The handle to the currently open CAN channel. Can be used with
        the API functions in CANLIB32.
        Read only.

    WinHandle: HWND;
        The handle to the hidden window which the component uses for
        notification purposes.
        Read only.

    LoadFailed: Boolean;
        True if the component failed in loading the canlib32.dll.
        Read only.
    
    Now: Cardinal;
        The current time.
        Read only.

    TransmitErrors: Cardinal;
        The current value of the transmit error counter.
        Read only.

    ReceiveErrors: Cardinal;
        The current value of the receive error counter.
        Read only.

    Overruns: Cardinal;
        Number of overruns detected.
        Read only.

    ChannelCount: Cardinal;
        The number of currently installed CAN channels.
        Read only.

    ChannelNames[idx: Integer]: string; 
        The names of the curently installed CAN channels.
        Read only.

    ChannelName: String;
        The name of the currently opened channel.
        Read only.

    ChannelStatus: Cardinal;
        The status of the channel.
        See the canReadStatus API.


  -- Published --

    Channel: Integer;
        The CAN channel number. Channel numbering starts with 0.
        Use the ChannelNames property to find out what channels are
        installed in your computer.


    Options: TCanChanOptions;
        Channel options. Any combination of
            ccNotExclusive
            ccNoVirtual
        See the canOpenChannel API for a description of the options
        (note that the menaing of the options here are the reverse
        to what the API wants.)

    Active: Boolean; 
        Setting this property to TRUE is the same as calling the Open method.
        Setting it to FALSE is the same as calling the Close method.

    BitRate: Longint;
        The desired bit rate, in bits/s. This property can be used
        EITHER together with any one of the predefined constants
        canBITRATE_xxx, OR in conjunction with the properties TSeg1, TSeg2, SJW,
        and Samples.
        For more information, see the canSetBusParams API.

    TSeg1: Integer;
        The number of quanta before the sampling point, NOT including
        the sync segment.
        For more information, see the canSetBusParams API.

    TSeg2: Integer;
        The number of quanta after the sampling point.
        For more information, see the canSetBusParams API.

    SJW: Integer;
        The number of quantas used for resynchnonization.
        For more information, see the canSetBusParams API.

    Samples: Integer; 
        The number of samples taken in each bit.
        For more information, see the canSetBusParams API.

    Driver: TCanChanDrivers; 
        The CAN Bus driver type. Any one of the canDRIVER_xxx types.
        See the canSetBusOutputControl API.

    BusActive: Boolean;
        Setting this property to TRUE is the same as calling the BusOn method.
        Setting it to FALSE is the same as calling the BusOff method.

    OnCanRx: TNotifyEvent;
        This event is called when the receive queue goes from empty to
        not empty. In the event handler, you have to call the Read method
        repeatedly until it returns canERR_NOMSG.

    OnCanTx: TNotifyEvent;
        Called each time a CAN messaghe is transmitted.

    OnStateChanged: TNotifyEvent;
        Called when the circuit changes status (error active/error passive/bus off)

    OnWrite: TCanWriteEvent;
        Called each time you call canWrite. Acts as a kind of "local echo".


Methods

    constructor Create(AOwner: TComponent);
    destructor Destroy;

    procedure Check(result: Integer; text: string);
        A utility function which raises an exception of type ECanChan if result
        is not equal to canOK. The exception contains the string
        text.

    procedure Open;
        Opens the selected channel (use the Channel property).
        Corresponds to the canOpenChannel API.

    procedure Close;
        Closes the currently open channel.
        Corresponds to the canClose API.

    procedure BusOn;
        Takes the currently open channel on-bus. Corresponds
        to the canBusOn API.

    procedure BusOff;
        Takes the currently open channel off-bus. Corresponds
        to the canBusOff API.

    function SetHardwareFilters(id: Longint; flag: Cardinal): integer;
        Corresponds to the canAccept API.

    function Write(id: Longint; var msg; dlc, flags: Cardinal): integer;
        Corresponds to the canWrite API.

    function WriteSync(timeout: Cardinal): integer;
        Corresponds to the canWriteSync API.

    function Read(var id: Longint; var msg; var dlc, flags: Cardinal; 
                  var time: Cardinal): integer;
        Corresponds to the canRead API.

    function ReadWait(var id: Longint; var msg; var dlc, flags: Cardinal; 
                      var time: Cardinal; timeout: Cardinal): integer;
        Corresponds to the canReadWait API.

    function ReadSpecific(id: Longint; var msg; var dlc, flags: Cardinal; 
                          var time: Cardinal): integer;
        Corresponds to the canReadSpecific API.

    function ReadSync(timeout: Cardinal): integer;
        Corresponds to the canReadSync API.

    function ReadSyncSpecific(id: Longint; timeout: Cardinal): integer;
        Corresponds to the canReadSyncSpecific API.

    function ReadSpecificSkip(id: Longint; var msg; var dlc, flags: Cardinal; 
                              var time: Cardinal): Integer;
        Corresponds to the canReadSpecificSkip API.

    function WaitForEvent(timeout: Cardinal): Integer;
        Corresponds to the canWaitForEvent API.


