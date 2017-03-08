#
# Calling CANLIB from Python
#
# Requires Python 2.5, or Python 2.3/2.4 with ctypes installed.
# Has been tested with Python 2.5.
# You can get ctypes here: http://starship.python.net/crew/theller/ctypes
#

from ctypes import *

# For sleep()
import time


# -------------------------------------------------------------------------
# dll initialization
# -------------------------------------------------------------------------
# Load canlib32.dll
canlib32 = windll.canlib32

# Load the API functions we use from the dll
canInitializeLibrary = canlib32.canInitializeLibrary
canOpenChannel = canlib32.canOpenChannel
canBusOn = canlib32.canBusOn
canBusOff = canlib32.canBusOff
canClose = canlib32.canClose
canWrite = canlib32.canWrite
canRead = canlib32.canRead
canGetChannelData = canlib32.canGetChannelData

# A few constants from canlib.h
canCHANNELDATA_CARD_FIRMWARE_REV = 9
canCHANNELDATA_DEVDESCR_ASCII = 26


# Define a type for the body of the CAN message. Eight bytes as usual.
MsgDataType = c_uint8 * 8

# Initialize the library...
canInitializeLibrary()

# ... and open channels 0 and 1. These are assumed to be on the same
# terminated CAN bus.
hnd1 = canOpenChannel(c_int(0), c_int(0))
hnd2 = canOpenChannel(c_int(1), c_int(0))

# Go bus on
stat = canBusOn(c_int(hnd1))
if stat < 0: print "canBusOn channel 1 failed: ", stat

stat = canBusOn(c_int(hnd2))
if stat < 0: print "canBusOn channel 2 failed: ", stat

# Setup a message
msg = MsgDataType()
msg[0] = 23
msg[1] = 45

# .. and send it, using identifier 100.
stat = canWrite(c_int(hnd1), c_int(100), pointer(msg), c_int(8), c_int(0))
if stat < 0:
  print "canWrite channel 1 failed: ", stat

# Sleep for a second - this is just a demonstration script anyway :-)
time.sleep(1)

# Here are the variables to receive the message..
rx_msg = MsgDataType()
rx_id = c_int()
rx_dlc = c_int()
rx_flags = c_int()
rx_time = c_int()

# .. so receive it.
stat = canRead(c_int(hnd2), pointer(rx_id), pointer(rx_msg), 
  pointer(rx_dlc), pointer(rx_flags), pointer(rx_time))

if stat < 0:
  print "canRead channel 2 failed: ", stat

# Print it, for the amusement of the crowd
print "Rx id=", rx_id.value
for i in rx_msg: print i,
print "\nRx Len=", rx_dlc.value
print "Rx Flags=", rx_flags.value
print "Rx Time=", rx_time.value

# Obtain the firmware revision for channel (not handle!) 0
fw_rev = c_uint64()
canGetChannelData(c_int(0), canCHANNELDATA_CARD_FIRMWARE_REV, pointer(fw_rev), 8)
print "Firmware revision channel 0 = ", (fw_rev.value >> 48), ".", (fw_rev.value >> 32) & 0xFFFF, ".", (fw_rev.value) & 0xFFFF

# Obtain device name for channel (not handle!) 0
s = create_string_buffer(100)
canGetChannelData(c_int(0), canCHANNELDATA_DEVDESCR_ASCII, pointer(s), 100)
print "Device name: ", s.value


# Some cleanup, which would be done automatically when the DLL unloads.
stat = canBusOff(c_int(hnd1))
stat = canBusOff(c_int(hnd2))

canClose(c_int(hnd1))
canClose(c_int(hnd2))


