Attribute VB_Name = "CANLIB32"
Rem
Rem                  Copyright 1998 by KVASER AB, Mölndal, Sweden
Rem                        WWW: http://www.kvaser.se
Rem
Rem  -------------------------------------------------------------------------
Rem
Rem This file contains definitions for 32-bit CANLIB32.DLL only.
Rem You have to use VB4 or later version.
Rem
Option Explicit

'
' canstat.h
'

' Don't forget to update canGetErrorText in canlib.c!
Public Const canOK = 0
Public Const canERR_PARAM = -1          ' Error in parameter
Public Const canERR_NOMSG = -2          ' No messages available
Public Const canERR_NOTFOUND = -3       ' Specified hw not found
Public Const canERR_NOMEM = -4          ' Out of memory
Public Const canERR_NOCHANNELS = -5     ' No channels avaliable
Public Const canERR_RESERVED_3 = -6
Public Const canERR_TIMEOUT = -7        ' Timeout occurred
Public Const canERR_NOTINITIALIZED = -8 ' Lib not initialized
Public Const canERR_NOHANDLES = -9      ' Can't get handle
Public Const canERR_INVHANDLE = -10     ' Handle is invalid
Public Const canERR_INIFILE = -11       ' Error in the ini-file (16-bit only)
Public Const canERR_DRIVER = -12        ' CAN driver type not supported
Public Const canERR_TXBUFOFL = -13      ' Transmit buffer overflow
Public Const canERR_RESERVED_1 = -14
Public Const canERR_HARDWARE = -15      ' Some hardware error has occurred
Public Const canERR_DYNALOAD = -16      ' Can't find requested DLL
Public Const canERR_DYNALIB = -17       ' DLL seems to be wrong version
Public Const canERR_DYNAINIT = -18      ' Error when initializing DLL
Public Const canERR_RESERVED_4 = -19
Public Const canERR_RESERVED_5 = -20
Public Const canERR_RESERVED_6 = -21
Public Const canERR_RESERVED_2 = -22
Public Const canERR_DRIVERLOAD = -23    ' Can't find/load driver
Public Const canERR_DRIVERFAILED = -24  ' DeviceIOControl failed; use Win32 GetLastError()
Public Const canERR_NOCONFIGMGR = -25   ' Can't find req'd config s/w (e.g. CS/SS)
Public Const canERR_NOCARD = -26        ' The card was removed or not inserted
Public Const canERR_RESERVED_7 = -27
Public Const canERR_REGISTRY = -28      ' Error in the Registry
Public Const canERR_LICENSE = -29       ' The license is not valid.
Public Const canERR_INTERNAL = -30      ' Internal error in the driver.
Public Const canERR_NO_ACCESS = -31             ' Access denied
Public Const canERR_NOT_IMPLEMENTED = -32


' Notification codes
Public Const canEVENT_RX = 32000                ' Receive event
Public Const canEVENT_TX = 32001                ' Transmit event
Public Const canEVENT_ERROR = 32002             ' Error event
Public Const canEVENT_STATUS = 32003                    ' Change-of-status event

' Used in canSetNotify
Public Const canNOTIFY_NONE = 0                                          ' Turn notifications off
Public Const canNOTIFY_RX = &H1                      ' Notify on receive
Public Const canNOTIFY_TX = &H2                      ' Notify on transmit
Public Const canNOTIFY_ERROR = &H4                   ' Notify on error
Public Const canNOTIFY_STATUS = &H8                                      ' Notify on (some) status changes

Public Const canSTAT_ERROR_PASSIVE = &H1            ' The circuit is error passive
Public Const canSTAT_BUS_OFF = &H2                  ' The circuit is Off Bus
Public Const canSTAT_ERROR_WARNING = &H4            ' At least one error counter > 96
Public Const canSTAT_ERROR_ACTIVE = &H8             ' The circuit is error active.
Public Const canSTAT_TX_PENDING = &H10              ' There are messages pending transmission
Public Const canSTAT_RX_PENDING = &H20              ' There are messages in the receive buffer
Public Const canSTAT_RESERVED_1 = &H40
Public Const canSTAT_TXERR = &H80                   ' There has been at least one TX error
Public Const canSTAT_RXERR = &H100                  ' There has been at least one RX error of some sort
Public Const canSTAT_HW_OVERRUN = &H200             ' The has been at least one HW buffer overflow
Public Const canSTAT_SW_OVERRUN = &H400             ' The has been at least one SW buffer overflow
Public Const canSTAT_OVERRUN = &H600

' Message information flags
Public Const canMSG_MASK = &HFF                   ' Used to mask the non-info bits
Public Const canMSG_RTR = &H1                     ' Message is a remote request
Public Const canMSG_STD = &H2                     ' Message has a standard ID
Public Const canMSG_EXT = &H4                     ' Message has a extended ID
Public Const canMSG_WAKEUP = &H8                  ' Message was received in wakeup mode
Public Const canMSG_NERR = &H10                   ' NERR was active during the message
Public Const canMSG_ERROR_FRAME = &H20            ' Message is an error frame
Public Const canMSG_TXACK = &H40                  ' Message is a TX REQUEST
Public Const canMSG_TXRQ = &H80                   ' Message is a TX ACK

' Message error flags, >= &H0100
Public Const canMSGERR_MASK = &HFF00              ' Used to mask the non-error bits
Public Const canMSGERR_RXERR = &H100              ' Any RX error
Public Const canMSGERR_HW_OVERRUN = &H200         ' HW buffer overrun
Public Const canMSGERR_SW_OVERRUN = &H400         ' SW buffer overrun
Public Const canMSGERR_STUFF = &H800              ' Stuff error
Public Const canMSGERR_FORM = &H1000              ' Form error
Public Const canMSGERR_CRC = &H2000               ' CRC error
Public Const canMSGERR_BIT0 = &H4000              ' Sent dom, read rec
Public Const canMSGERR_BIT1 = &H8000              ' Sent rec, read dom

'
' Convenience values for the message error flags.
'
Public Const canMSGERR_OVERRUN = &H600            ' Any overrun condition.
Public Const canMSGERR_BIT = &HC000               ' Any bit error.
Public Const canMSGERR_BUSERR = &HF800            ' Any RX error


'
' canlib.h
'

Public Const canINVALID_HANDLE = -1

Public Const WM__CANLIB = (&H400 + 16354)      ' &H400 = WM_USER

' Obsolete
Public Const canCIRCUIT_ANY = -1                  ' Any circuit will do
Public Const canCARD_ANY = -1                     ' Any card will do
Public Const canCHANNEL_ANY = -1                  ' Any channel will do

' Flags for canOpenChannel
Public Const canOPEN_EXCLUSIVE = &H8
Public Const canOPEN_REQUIRE_EXTENDED = &H10
Public Const canOPEN_ACCEPT_VIRTUAL = &H20
Public Const canOPEN_OVERRIDE_EXCLUSIVE = &H40
Public Const canOPEN_REQUIRE_INIT_ACCESS = &H80
Public Const canOPEN_NO_INIT_ACCESS = &H100
Public Const canOPEN_ACCEPT_LARGE_DLC = &H200  ' DLC can be greater than 8
  
' Flags for canAccept
Public Const canFILTER_ACCEPT = 1
Public Const canFILTER_REJECT = 2
Public Const canFILTER_SET_CODE_STD = 3
Public Const canFILTER_SET_MASK_STD = 4
Public Const canFILTER_SET_CODE_EXT = 5
Public Const canFILTER_SET_MASK_EXT = 6

Public Const canFILTER_NULL_MASK = 0

'
' CAN driver types - not all are supported on all cards.
'
Public Const canDRIVER_NORMAL = 4
Public Const canDRIVER_SILENT = 1
Public Const canDRIVER_SELFRECEPTION = 8
Public Const canDRIVER_OFF = 0

'
' Common bus speeds. Used in canSetBusParams.
' The values are translated in canlib, canTranslateBaud().
'
Public Const BAUD_1M = -1
Public Const BAUD_500K = -2
Public Const BAUD_250K = -3
Public Const BAUD_125K = -4
Public Const BAUD_100K = -5
Public Const BAUD_62K = -6
Public Const BAUD_50K = -7
Public Const BAUD_83K = -8
Public Const BAUD_83K = -8

Public Const canBITRATE_1M = -1
Public Const canBITRATE_500K = -2
Public Const canBITRATE_250K = -3
Public Const canBITRATE_125K = -4
Public Const canBITRATE_100K = -5
Public Const canBITRATE_62K = -6
Public Const canBITRATE_50K = -7
Public Const canBITRATE_83K = -8
Public Const canBITRATE_10K = -9

'
' IOCTL types
'
Public Const canIOCTL_PREFER_EXT = 1
Public Const canIOCTL_PREFER_STD = 2
' 3, 4 reserved.
Public Const canIOCTL_CLEAR_ERROR_COUNTERS = 5
Public Const canIOCTL_SET_TIMER_SCALE = 6
Public Const canIOCTL_SET_TXACK = 7

Type canHWDescr
   circuitType As Long           ' Any one of canCARD_XXX
   cardtype    As Long           ' Any one of PCCAN_xxx etc.
   channel     As Long           ' Channel # or canCHANNEL_ANY
End Type

Type canSWDescr
   rxBufSize   As Long
   txBufSize   As Long
   alloc       As Long              ' Should always be 0 when called from VB
   deAlloc     As Long              ' Should always be 0 when called from VB
End Type


Declare Sub canInitializeLibrary Lib "CANLIB32" ()

Declare Function canClose Lib "CANLIB32" (ByVal Handle As Long) As Long

Declare Function canBusOn Lib "CANLIB32" (ByVal Handle As Long) As Long

Declare Function canBusOff Lib "CANLIB32" (ByVal Handle As Long) As Long

Declare Function canSetBusParams Lib "CANLIB32" (ByVal Handle As Long, ByVal freq As Long, ByVal tseg1 As Long, ByVal tseg2 As Long, ByVal sjw As Long, ByVal noSamp As Long, ByVal syncMode As Long) As Long

Declare Function canGetBusParams Lib "CANLIB32" (ByVal Handle As Long, ByRef freq As Long, ByRef tseg1 As Long, ByRef tseg2 As Long, ByRef sjw As Long, ByRef noSamp As Long, ByRef syncMode As Long) As Long

Declare Function canSetBusOutputControl Lib "CANLIB32" (ByVal Handle As Long, ByVal drivertype As Long) As Long

Declare Function canGetBusOutputControl Lib "CANLIB32" (ByVal Handle As Long, ByRef drivertype As Long) As Long

Declare Function canAccept Lib "CANLIB32" (ByVal Handle As Long, ByVal envelope As Long, ByVal flag As Long) As Long

Declare Function canReadStatus Lib "CANLIB32" (ByVal Handle As Long, ByRef Flags As Long) As Long

Declare Function canReadErrorCounters Lib "CANLIB32" (ByVal Handle As Long, ByRef txErr As Long, ByRef rxErr As Long, ByRef ovErr As Long) As Long

Declare Function canWrite Lib "CANLIB32" (ByVal Handle As Long, ByVal id As Long, ByRef msg As Any, ByVal dlc As Long, ByVal flag As Long) As Long

Declare Function canWriteSync Lib "CANLIB32" (ByVal Handle As Long, ByVal timeout As Long) As Long

Declare Function canRead Lib "CANLIB32" (ByVal Handle As Long, ByRef id As Long, ByRef msg As Any, ByRef dlc As Long, ByRef flag As Long, ByRef time As Long) As Long

Declare Function canReadWait Lib "CANLIB32" (ByVal Handle As Long, ByRef id As Long, ByRef msg As Any, ByRef dlc As Long, ByRef flag As Long, ByRef time As Long, ByRef timeout As Long) As Long

Declare Function canReadSpecific Lib "CANLIB32" (ByVal Handle As Long, ByVal id As Long, ByRef msg As Any, ByRef dlc As Long, ByRef flag As Long, ByRef time As Long) As Long

Declare Function canReadSync Lib "CANLIB32" (ByVal Handle As Long, ByVal timeout As Long) As Long

Declare Function canReadSyncSpecific Lib "CANLIB32" (ByVal Handle As Long, ByVal id As Long, ByVal timeout As Long) As Long

Declare Function canReadSpecificSkip Lib "CANLIB32" (ByVal Handle As Long, ByVal id As Long, ByRef msg As Any, ByRef dlc As Long, ByRef flag As Long, ByRef time As Long) As Long

Declare Function canSetNotify Lib "CANLIB32" (ByVal Handle As Long, ByVal aHWnd As Long, ByVal aNotifyFlags As Long) As Long

Declare Function canTranslateBaud Lib "CANLIB32" (ByRef freq As Long, ByRef tseg1 As Long, ByRef tseg2 As Long, ByRef sjw As Long, ByRef noSamp As Long, ByRef syncMode As Long) As Long

Declare Function canGetErrorText Lib "CANLIB32" (ByVal Err As Long, ByRef Buf As Any, ByVal bufsiz As Long) As Long

Declare Function canGetVersion Lib "CANLIB32" () As Long

Declare Function canGetCircuits Lib "CANLIB32" (ByRef context As Long, ByRef name As String, ByRef vendor As String, ByRef version As String, ByRef cardtype As Long, ByRef circtype As Long, ByRef channel As Long) As Long

Declare Function canIoCtl Lib "CANLIB32" (ByVal Handle As Long, ByVal Func As Long, ByRef Buf As Any, ByVal Buflen As Long) As Long

Declare Function canReadTimer Lib "CANLIB32" (ByVal Handle As Long) As Long

Declare Function canOpenChannel Lib "CANLIB32" (ByVal Handle As Long, ByVal Flags As Long) As Long

' canlib32 specific functions
Declare Function canGetNumberOfChannels Lib "CANLIB32" (ByRef channelCount As Long) As Long

Declare Function canGetChannelData Lib "CANLIB32" (ByVal channel As Long, ByVal item As Long, ByRef buffer As Any, ByVal bufsize As Long) As Long

Public Const canCHANNELDATA_CHANNEL_CAP = 1
Public Const canCHANNELDATA_TRANS_CAP = 2
Public Const canCHANNELDATA_CHANNEL_FLAGS = 3             ' available, etc
Public Const canCHANNELDATA_CARD_TYPE = 4                 ' canHWTYPE_xxx
Public Const canCHANNELDATA_CARD_NUMBER = 5               ' Number in machine, 0,1,...
Public Const canCHANNELDATA_CHAN_NO_ON_CARD = 6
Public Const canCHANNELDATA_CARD_SERIAL_NO = 7
Public Const canCHANNELDATA_TRANS_SERIAL_NO = 8
Public Const canCHANNELDATA_CARD_FIRMWARE_REV = 9
Public Const canCHANNELDATA_CARD_HARDWARE_REV = 10
Public Const canCHANNELDATA_CARD_UPC_NO = 11
Public Const canCHANNELDATA_TRANS_UPC_NO = 12
Public Const canCHANNELDATA_CHANNEL_NAME = 13
Public Const canCHANNELDATA_DLL_FILE_VERSION = 14
Public Const canCHANNELDATA_DLL_PRODUCT_VERSION = 15
Public Const canCHANNELDATA_DLL_FILETYPE = 16
Public Const canCHANNELDATA_TRANS_TYPE = 17
Public Const canCHANNELDATA_DEVICE_PHYSICAL_POSITION = 18
Public Const canCHANNELDATA_UI_NUMBER = 19
Public Const canCHANNELDATA_TIMESYNC_ENABLED = 20
Public Const canCHANNELDATA_DRIVER_FILE_VERSION = 21
Public Const canCHANNELDATA_DRIVER_PRODUCT_VERSION = 22
Public Const canCHANNELDATA_MFGNAME_UNICODE = 23
Public Const canCHANNELDATA_MFGNAME_ASCII = 24
Public Const canCHANNELDATA_DEVDESCR_UNICODE = 25
Public Const canCHANNELDATA_DEVDESCR_ASCII = 26

' channelFlags in canChannelData
Public Const canCHANNEL_IS_EXCLUSIVE = &H1
Public Const canCHANNEL_IS_OPEN = &H2

' Hardware types.
Public Const canHWTYPE_NONE = 0               ' Unknown
Public Const canHWTYPE_VIRTUAL = 1            ' Virtual channel.
Public Const canHWTYPE_LAPCAN = 2             ' LAPcan
Public Const canHWTYPE_CANPARI = 3            ' CANpari
Public Const canHWTYPE_PCCAN = 8              ' PCcan-X
Public Const canHWTYPE_PCICAN = 9             ' PCIcan.
Public Const canHWTYPE_USBCAN = 11                ' USBcan Family and relatives
Public Const canHWTYPE_PCICAN_II = 40         ' PCIcan II family
Public Const canHWTYPE_USBCAN_II = 42         ' USBcan II, Memorator et al
Public Const canHWTYPE_SIMULATED = 44         ' SImulated CAN bus for Creator
Public Const canHWTYPE_ACQUISITOR = 46        ' Acquisitor et al
Public Const canHWTYPE_LEAF = 48                  ' Kvaser Leaf Family
Public Const canHWTYPE_USBCAN_LIGHT = 72      ' Kvaser USBcan Light

' Channel capabilities.
Public Const canCHANNEL_CAP_EXTENDED_CAN = &H1
Public Const canCHANNEL_CAP_BUS_STATISTICS = &H2
Public Const canCHANNEL_CAP_ERROR_COUNTERS = &H4
Public Const canCHANNEL_CAP_CAN_DIAGNOSTICS = &H8
Public Const canCHANNEL_CAP_GENERATE_ERROR = &H10
Public Const canCHANNEL_CAP_GENERATE_OVERLOAD = &H20
Public Const canCHANNEL_CAP_TXREQUEST = &H40
Public Const canCHANNEL_CAP_TXACKNOWLEDGE = &H80
Public Const canCHANNEL_CAP_VIRTUAL = &H10000
Public Const canCHANNEL_CAP_SIMULATED = &H20000

' Driver (transceiver) capabilities
Public Const canDRIVER_CAP_HIGHSPEED = &H1

Public Const canIOCTL_GET_RX_BUFFER_LEVEL = 8
Public Const canIOCTL_GET_TX_BUFFER_LEVEL = 9
Public Const canIOCTL_FLUSH_RX_BUFFER = 10
Public Const canIOCTL_FLUSH_TX_BUFFER = 11
Public Const canIOCTL_GET_TIMER_SCALE = 12
Public Const canIOCTL_SET_TXRQ = 13
Public Const canIOCTL_GET_EVENTHANDLE = 14
Public Const canIOCTL_SET_BYPASS_MODE = 15
Public Const canIOCTL_SET_WAKEUP = 16
Public Const canIOCTL_GET_DRIVERHANDLE = 17
Public Const canIOCTL_MAP_RXQUEUE = 18
Public Const canIOCTL_GET_WAKEUP = 19
Public Const canIOCTL_SET_REPORT_ACCESS_ERRORS = 20
Public Const canIOCTL_GET_REPORT_ACCESS_ERRORS = 21
Public Const canIOCTL_CONNECT_TO_VIRTUAL_BUS = 22
Public Const canIOCTL_DISCONNECT_FROM_VIRTUAL_BUS = 23
Public Const canIOCTL_SET_USER_IOPORT = 24
Public Const canIOCTL_GET_USER_IOPORT = 25
Public Const canIOCTL_SET_BUFFER_WRAPAROUND_MODE = 26
Public Const canIOCTL_SET_RX_QUEUE_SIZE = 27

Type canUserIoPortData
   portNo As Long
   portValue As Long
End Type

Declare Function canWaitForEvent Lib "CANLIB32" (ByVal hnd As Long, ByVal timeout As Long) As Long

Declare Function canSetBusParamsC200 Lib "CANLIB32" (ByVal hnd As Long, ByVal btr0 As Long, ByVal btr1 As Long) As Long

Declare Function canSetDriverMode Lib "CANLIB32" (ByVal hnd As Long, ByVal lineMode As Long, ByVal resNet As Long) As Long

Declare Function canGetDriverMode Lib "CANLIB32" (ByVal hnd As Long, ByRef lineMode As Long, ByRef resNet As Long) As Long

Declare Function canGetVersionEx Lib "CANLIB32" (ByVal itemCode As Long) As Long

Declare Function canParamGetCount Lib "CANLIB32" () As Long

Declare Function canParamCommitChanges Lib "CANLIB32" () As Long

Declare Function canParamDeleteEntry Lib "CANLIB32" (ByVal index As Long) As Long

Declare Function canParamCreateNewEntry Lib "CANLIB32" () As Long

Declare Function canParamSwapEntries Lib "CANLIB32" (ByVal index1 As Long, ByVal index2 As Long) As Long


' Must be treated like canGetErrorText -  i.e. define a local VB function
' that converts the string parameter. Left as an exercise to the reader.
' Declare Function canParamGetName Lib "CANLIB32" (ByVal index As Long, ByVal buffer As String, ByVal maxlen As Long) As Long
' Declare Function canParamSetName Lib "CANLIB32" (ByVal index As Long, ByVal buffer As String) As Long
' Declare Function canParamFindByName Lib "CANLIB32" (ByVal name As String) As Long



Declare Function canParamGetChannelNumber Lib "CANLIB32" (ByVal index As Long) As Long

Declare Function canParamGetBusParams Lib "CANLIB32" (ByVal index As Long, ByRef bitrate As Long, ByRef tseg1 As Long, ByRef tseg2 As Long, ByRef sjw As Long, ByRef noSamp As Long) As Long

Declare Function canParamSetChannelNumber Lib "CANLIB32" (ByVal index As Long, ByVal channel As Long) As Long

Declare Function canParamSetBusParams Lib "CANLIB32" (ByVal index As Long, ByVal bitrate As Long, ByVal tseg1 As Long, ByVal tseg2 As Long, ByVal sjw As Long, ByVal noSamp As Long) As Long

' Frees all object buffers associated with the specified handle.
Declare Function canObjBufFreeAll Lib "CANLIB32" (ByVal Handle As Long) As Long

' Allocates an object buffer of the specified type.
Declare Function canObjBufAllocate Lib "CANLIB32" (ByVal Handle As Long, ByVal tp As Long) As Long
Public Const canOBJBUF_TYPE_AUTO_RESPONSE = &H1
Public Const canOBJBUF_TYPE_PERIODIC_TX = &H2

' Deallocates the object buffer with the specified index.
Declare Function canObjBufFree Lib "CANLIB32" (ByVal Handle As Long, ByVal idx As Long) As Long

' Writes CAN data to the object buffer with the specified index.
Declare Function canObjBufWrite Lib "CANLIB32" (ByVal Handle As Long, ByVal idx As Long, ByVal id As Long, ByRef msg As Any, ByVal dlc As Long, ByVal Flags As Long) As Long

' For an AUTO_RESPONSE buffer, set the code and mask that together define
' the identifier(s) that trigger(s) the automatic response.
Declare Function canObjBufSetFilter Lib "CANLIB32" (ByVal Handle As Long, ByVal idx As Long, ByVal code As Long, ByVal mask As Long) As Long

' Sets buffer-speficic flags.
Declare Function canObjBufSetFlags Lib "CANLIB32" (ByVal Handle As Long, ByVal idx As Long, ByVal Flags As Long) As Long
' The buffer responds to RTRs only, not regular messages.
' AUTO_RESPONSE buffers only
Public Const canOBJBUF_AUTO_RESPONSE_RTR_ONLY = &H1

' Sets transmission period for auto tx buffers.
Declare Function canObjBufSetPeriod Lib "CANLIB32" (ByVal hnd As Long, ByVal idx As Long, ByVal period As Long) As Long

' Enable object buffer with index idx.
Declare Function canObjBufEnable Lib "CANLIB32" (ByVal Handle As Long, ByVal idx As Long) As Long

' Disable object buffer with index idx.
Declare Function canObjBufDisable Lib "CANLIB32" (ByVal Handle As Long, ByVal idx As Long) As Long

' For certain diagnostics.
Declare Function canObjBufSendBurst Lib "CANLIB32" (ByVal hnd As Long, ByVal idx As Long, ByVal burstlen As Long) As Long

' Check for specific version(s) of CANLIB.
Public Const canVERSION_DONT_ACCEPT_LATER = &H1
Public Const canVERSION_DONT_ACCEPT_BETAS = &H2
Declare Function canProbeVersion Lib "CANLIB32" (ByVal hnd As Long, ByVal major As Long, ByVal minor As Long, ByVal oem_id As Long, ByVal Flags As Long) As Boolean

' Try to "reset" the CAN bus.
Declare Function canResetBus Lib "CANLIB32" (ByVal Handle As Long) As Long

' Convenience function that combines canWrite and canWriteSync.
Declare Function canWriteWait Lib "CANLIB32" (ByVal Handle As Long, ByVal id As Long, ByRef msg As Any, ByVal dlc As Long, ByVal flag As Long, ByVal timeout As Long) As Long

' Tell canlib32.dll to unload its DLLs.
Declare Function canUnloadLibrary Lib "CANLIB32" () As Long

Declare Function canSetAcceptanceFilter Lib "CANLIB32" (ByVal hnd As Long, ByVal code As Long, ByVal mask As Long, ByVal is_extended As Long) As Long

Declare Function kvReadDeviceCustomerData Lib "CANLIB32" (ByVal hnd As Long, ByVal userNumber As Long, ByVal itemNumber As Long, ByRef data As Any, ByVal bufsize As Long) As Long

'----------------------------------------------------------------------
' Obsolete definitions follow.
'----------------------------------------------------------------------

' Old-style Circuit status flags
Public Const canCIRCSTAT_ERROR_PASSIVE = &H1       ' Error passive
Public Const canCIRCSTAT_BUS_OFF = &H2             ' Bus off
Public Const canCIRCSTAT_ERROR_WARNING = &H4       ' Error counter > 96


' Circuit types.
Public Const PCCAN_PHILIPS = 1                    ' 82C200 on PCCAN 1.0
Public Const PCCAN_INTEL526 = 2                   ' Not supported.
Public Const PCCAN_INTEL527 = 3                   ' 82527 on PCCAN 1.0
Public Const CANCARD_NEC72005 = 4                 ' NEC72005 on CANCard

' Card types.
Public Const canCARD_PCCAN = 1                    ' PCCAN ver 1.x (KVASER)
Public Const canCARD_CANCARD = 2                  ' CANCard (Softing)
Public Const canCARD_AC2 = 3                      ' CAN-AC2 (Softing)


Public Const canWANT_ACTIONS = &H1
Public Const canWANT_OWN_BUFFERS = &H2
Public Const canWANT_ERROR_COUNTERS = &H4

' The canFlgXXX are left for compatibility.
Public Const canFlgACCEPT = 1
Public Const canFlgREJECT = 2
Public Const canFlgCODE = 3
Public Const canFlgMASK = 4

' Flags for action routines
Public Const canDISCARD_MESSAGE = 3
Public Const canRETAIN_MESSAGE = 4

' For busParams - sync on rising edge only or both rising and falling edge
Public Const canSLOW_MODE = &H1 ' Sync on rising and falling edge

'
' CAN driver types; these constants are retained for compatibility.
'
Public Const canOFF = 0
Public Const canTRISTATE = 1
Public Const canPULLUP = 2
Public Const canPULLDOWN = 3
Public Const canPUSHPULL = 4                   ' This is the usual setting.
Public Const canINVPULLUP = 5
Public Const canINVPULLDOWN = 6
Public Const canINVPUSHPULL = 7

Declare Function canInstallAction Lib "CANLIB32" (ByVal Handle As Long, ByVal id As Long, ByRef Func As Long) As Long

Declare Function canUninstallAction Lib "CANLIB32" (ByVal Handle As Long, ByRef id As Long) As Long

Declare Function canInstallOwnBuffer Lib "CANLIB32" (ByVal Handle As Long, ByRef id As Long, ByRef length As Long, ByRef Buf As String) As Long

Declare Function canUninstallOwnBuffer Lib "CANLIB32" (ByVal Handle As Long, ByRef id As Long) As Long

Public Const canIOCTL_LOCAL_ECHO_ON = 3
Public Const canIOCTL_LOCAL_ECHO_OFF = 4

' Not obsolete.. but new program should use canInitializeLibrary
Declare Function canLocateHardware Lib "CANLIB32" () As Long

' Not obsolete.. but new programs should use canOpenChannel instead
Declare Function canOpen Lib "CANLIB32" (ByRef hwdescr As canHWDescr, ByRef swdescr As canSWDescr, ByVal Flags As Long) As Long

' -------------------------------------------------------------------------------
' VB Special fixes below
' -------------------------------------------------------------------------------
Declare Function GetErrorText Lib "CANLIB32" Alias "canGetErrorText" (ByVal Err As Long, ByVal Buf As String, ByVal bufsiz As Long) As Long

'
' Use this one instead of canGetErrorText to get the error text for
' a status code
'
Public Function GetErrorTextVB(ByVal Err As Long) As String
   Dim Buf As String * 200
   Dim stat As Integer
   stat = GetErrorText(Err, Buf, 200)
   Buf = Left(Buf, InStr(1, Buf, Chr(0), vbBinaryCompare) - 1)
   GetErrorTextVB = Buf
End Function
