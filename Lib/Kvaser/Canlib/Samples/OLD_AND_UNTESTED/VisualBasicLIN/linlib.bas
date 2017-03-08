Attribute VB_Name = "LINLIB"


 Public Const linINVALID_HANDLE = -1

' Return codes from the LINlib functions.
    
Public Enum LinStatus
     linOK = 0
     linERR_NOMSG = -1                     ' No messages available
     linERR_NOTRUNNING = -3                '
     linERR_RUNNING = -4                   '
     linERR_MASTERONLY = -5                '
     linERR_SLAVEONLY = -6                 '
     linERR_PARAM = -7                     '  Error in parameter
     linERR_NOTFOUND = -8                  '  Specified hw not found
     linERR_NOMEM = -9                     '  Out of memory
     linERR_NOCHANNELS = -10               '  No channels avaliable
     linERR_TIMEOUT = -11                  '  Timeout occurred
     linERR_NOTINITIALIZED = -12           '  Lib not initialized
     linERR_NOHANDLES = -13                '  Can't get handle
     linERR_INVHANDLE = -14                '  Handle is invalid
     linERR_CANERROR = -15                 '
     linERR_ERRRESP = -16                  '  There was an error response from the LIN interface
     linERR_WRONGRESP = -17                '  The LIN interface response wasn't the expected one
     linERR_DRIVER = -18                   '  CAN driver type not supported
     linERR_DRIVERFAILED = -19             '  DeviceIOControl failed; use Win32 GetLastError()
     linERR_NOCARD = -20                   '  The card was removed or not inserted
     linERR_LICENSE = -21                  '  The license is not valid.
     linERR_INTERNAL = -22                 '  Internal error in the driver.
     linERR_NO_ACCESS = -23                '  Access denied
     linERR_VERSION = -24                  '  Function not supported in this version.
End Enum

 
' sizeof(LinMessageInfo) should be 68 for correct alignment
' The precision of the timing data given in us (microseconds) can be less than
' one microsecond; for low bitrates the lowest bits might always be zero.
' The min and max values listed inside [] of the message timing values can be
' calculated from the LIN specification by using the shortest (0 bytes) or
' longest (8 bytes) messages at the lowest or highest allowed bitrate.
' The LIN interface will accept messages that are a bit out-of-bounds as well.

Type LinMessageInfo

'  Timestamp in ms of the falling edge of the synch
'  break of the message. Uses the canlib CAN timer.
   timestamp As Long
'
'  Length of the synch break in microseconds.
'  [650 .. 13000],  [400 .. 8000] for a wakeup signal.
   synchBreakLength As Long

'  The total frame length in microseconds; from the synch break to the
'  end of the crc. [2200 .. 173600]
   frameLength As Long
   
'  The bitrate of the message in bits per seconds.
'  Range [1000 .. 20000] (plus some margin)
   bitrate As Long

'  The checksum as read from the LIN bus. Might not
'  match the data in case of LIN_CSUM_ERROR.
   checkSum As Byte
   
'  The id with parity of the message as read from the
'  LIN bus. Might be invalid in case of LIN_PARITY_ERROR.
   idPar As Byte

'  Dummy for alignment
   z As Integer

'  Time in microseconds of the falling edges in the
'  synch byte relative the falling edge of the start bit.
   synchEdgeTime(4) As Long
'
'  Start time in microseconds of each data byte. In case of
'  8-byte messages, the crc time isn't included (but can be deduced
'  from frameLength).
   byteTime(8) As Long

End Type


' Bits in the flags result from linReadMessage()/linReadMessageWait():
  
  Public Const LIN_TX = 1               '//< The message was something we transmitted on the bus
  Public Const LIN_RX = 2               '//< The message was something we received from the bus
  Public Const LIN_WAKEUP_FRAME = 4     '//< A wake up frame was received. Id/msg/dlc are undefined
  Public Const LIN_NODATA = 8           '//< No data, only a header
  Public Const LIN_CSUM_ERROR = 16      '//< Checksum error
  Public Const LIN_PARITY_ERROR = 32    '//< ID parity error
  Public Const LIN_SYNCH_ERROR = 64     '//< A synch error
  Public Const LIN_BIT_ERROR = 128      '//< Bit error when transmitting

Declare Sub linInitializeLibrary Lib "LINLIB" ()


' Retrieves the transceiver information of a (LAPcan II) channel.
' For a Kvaser LIN Leaf it retrieves the transceiver type
' and device information.
' Returns linOK on success.
'
' The CAN channel is opened, but no CAN messages are transmitted on it.
' A LIN interface need not be powered for this call to succeed. It will
' only work on a LAPcan channel.
'
' Note: the information may not always be accurate.  Especially after
' changing transceiver on a running LAPcan card, one should go on bus
' and off bus again to be sure the transceiver information is updated.
'
' In case of a LIN interface, *ttype will be set to canTRANSCEIVER_TYPE_LIN
' by the call (see canstat.h).
Declare Function linGetTransceiverData Lib "LINLIB" (ByVal Channel As Long, _
                                                     ByRef eanNo As Any, _
                                                     ByRef serNo As Any, _
                                                     ByRef ttype As Long) As LinStatus


Public Const canTRANSCEIVER_TYPE_LIN = 19  ' retieved from canstat.h

' Open a channel to a LIN interface.
' For DRV Lin:
' The cable must be powered and
' connected to a LAPcan channel.
' For Kvaser LIN Leaf:
' The Leaf must be powered from the LIN side.
'
' The channel number is the same that
' is used by canOpenChannel() to access the LAPcan channel.
'
'LinHandle LINLIBAPI linOpenChannel(int channel, int flags);
Declare Function linOpenChannel Lib "LINLIB" (ByVal Channel As Long, _
                                              ByVal Flags As MasterOrSlave) As Long




' Close the LIN channel.
Declare Function linClose Lib "LINLIB" (ByVal Handle As Long) As LinStatus


' Get firmware version of the LIN Interface.
' The version numbers aren't valid until linBusOn() has been called.
Declare Function linGetFirmwareVersion Lib "LINLIB" (ByVal Handle As Long, _
                                                     ByVal bootVerMajor As String, _
                                                     ByVal bootVerMinor As String, _
                                                     ByVal bootVerBuild As String, _
                                                     ByVal appVerMajor As String, _
                                                     ByVal appVerMinor As String, _
                                                     ByVal appVerBuild As String) As LinStatus


' Set the bit rate for a master, the initial bitrate for a slave.
' The LIN Interface should not be on bus.
Declare Function linSetBitrate Lib "LINLIB" (ByVal Handle As Long, _
                                             ByVal bps As Long) As LinStatus

' Activate the LIN interface.
' It will be reset, so any setups done earlier will be lost.
Declare Function linBusOn Lib "LINLIB" (ByVal Handle As Long) As LinStatus


'Deactivate the LIN interface. It will be reset clearing any settings
'such as LIN data buffers active.
Declare Function linBusOff Lib "LINLIB" (ByVal Handle As Long) As LinStatus


' Return the current timer value (used for timestamps)
Declare Function linReadTimer Lib "LINLIB" (ByVal Handle As Long) As LinStatus


' Write a LIN message. Only available in master mode.
Declare Function linWriteMessage Lib "LINLIB" (ByVal Handle As Long, _
                                               ByVal id As Long, _
                                               ByRef msg As Any, _
                                               ByVal dlc As Long) As LinStatus


' Write a LIN header (which a slave is expected to fill in with
' data).  Only available in master mode.
Declare Function linRequestMessage Lib "LINLIB" (ByVal Handle As Long, _
                                                 ByVal id As Long) As LinStatus


' If a message is received (including echo of what we transmitted
' ourselves with linWriteMessage()), linOK it is returned.
Declare Function linReadMessage Lib "LINLIB" (ByVal Handle As Long, _
                                              ByRef id As Long, _
                                              ByRef msg As Any, _
                                              ByRef dlc As Long, _
                                              ByRef Flags As Long, _
                                              ByRef msginfo As LinMessageInfo) As LinStatus


' If a message is received (including echo of what we transmitted
' ourselves with linWriteMessage()), linOK is returned. Times out after
' timeout ms.  Returns linERR_NOMSG if nothing is received.
Declare Function linReadMessageWait Lib "LINLIB" (ByVal Handle As Long, _
                                                  ByRef id As Long, _
                                                  ByRef msg As Any, _
                                                  ByRef dlc As Long, _
                                                  ByRef Flags As Long, _
                                                  ByRef msginfo As LinMessageInfo, _
                                                  ByVal timeout As Long) As LinStatus

' Update the message buffers for a slave. It will be used the next
' time the slave is polled for this id.
' The LIN Interface must be on bus.
Declare Function linUpdateMessage Lib "LINLIB" (ByVal Handle As Long, _
                                                ByVal id As Long, _
                                                ByRef msg As Any, _
                                                ByVal dlc As Long) As LinStatus

' Using this function, it is possible to use the LIN interface to create
' corrupted LIN messages.
' It can make a message to be too long and produce illegal checksums.
' The functon should be called for each LIN identifier that should be
' effected. To return to normal mode, either restart the LIN interface (by
' going off bus and on again) or call the function with delay and cFlags
' set to zero.
' The delay parameter will result in a delay of this many bittimes
' after the header and before the first data byte.
' Setting bits in cFlags one can control other things:
'  LIN_MSG_DISTURB_CSUM:   The checksum of transmitted messages will be inverted.
'  LIN_MSG_DISTURB_PARITY: The two parity bits will be inverted.
'
' This function will work in both slave and master mode.
'
' As only the master transmits the header, LIN_MSG_DISTURB_PARITY has
' no effect in slave mode.
'
' The LIN Interface must be on bus for this command to work.
' It is supported in firmware version 2.4.1 and later.

Public Const LIN_MSG_DISTURB_CSUM = 1
Public Const LIN_MSG_DISTURB_PARITY = 2    '// Used only in master mode

Declare Function linSetupIllegalMessage Lib "LINLIB" (ByVal Handle As Long, _
                                                      ByVal id As Long, _
                                                      ByVal cFlags As Long, _
                                                      ByVal delay As Long) As LinStatus
                                                      

' With this function, it is possible to change settings on a LIN Interface that is
' on-bus. When going on-bus, the bitrate and the flag values listed here are set
' to the default value (either as hard-coded in the firmware, or as stored in the
' non-voilatile memory of the LIN Interface).
' With this function, it is possible to select checksum according to Lin 2.0 or
' to turn variable message length off (in this case, the message length will
' depend on the message ID).
' In master mode, it is also possible to change the bitrate without going off bus
' first.
' The LIN Interface must be on bus for this command to work.
' It is supported in firmware version 2.5.1 and later.
Declare Function linSetupLIN Lib "LINLIB" (ByVal Handle As Long, _
                                           ByVal lFlags As Long, _
                                           ByVal bps As Long) As LinStatus

Public Const LIN_ENHANCED_CHECKSUM = 1 '// Default off
Public Const LIN_VARIABLE_DLC = 2      '// Default on


' Write a wakeup frame.
' If count is zero, one single wakeup frame is transmitted.
' If count > 1, several wakeup frames are transmitted
' spaced with 'interval' bittimes.  The LIN Interface will interrupt the sequence
' when a LIN message or another command is received. The stream of
' wakeups will be recived as incoming messages with the LIN_RX flag bit
' set.
Declare Function linWriteWakeup Lib "LINLIB" (ByVal Handle As Long, _
                                              ByVal count As Long, _
                                              ByVal interval As Long) As LinStatus



' Clear the message buffer for a slave. It will not answer next time it is polled.
Declare Function linClearMessage Lib "LINLIB" (ByVal Handle As Long, _
                                               ByVal id As Long) As LinStatus


' Call this function to make sure all messages transmitted to the
' LIN Interface has been received by it.
'
' When messages are transmitted to the LIN Interface, they are queued by
' Windows before appearing on the CAN bus.
'
' The function returns linOK if all writes are done, linERR_TIMEOUT in
' case of timeout or possibly some other error code.
'
' Note that if the LIN Interface is in master mode and a LIN message has been
' transmitted with linWriteMessage(), this function will return when
' the LIN Interface has received the message. If another LIN message is being
' received or transmitted, the message will not be transmitted on the
' LIN bus at once. And even if the LIN Interface is idle, the header of the new
' message will just have been started when linWriteSync() returns.
'
' After calling linUpdateMessage() and linClearMessage() for a slave,
' this function is enough to know that the LIN Interface is updated.
'
' After linWriteMessage(), it is advisable to wait until the message is
' echoed by linReadMessage() before transmitting a new message, or in
' case of a schedule table being used, transmit the next message when
' the previous one is known to be complete.
'
' When, in master mode, a message should be transmitted after a poll
' (reception) is done, it might be necessary to call linWriteMessage()
' before the result is received via linReadMessage() as the LIN Interface waits
' up to the maximum frame length before knowing a received message is
' complete. A new message to transmit will force completion if the
' currently received one.
Declare Function linWriteSync Lib "LINLIB" (ByVal Handle As Long, _
                                            ByVal timeout As Long) As LinStatus




