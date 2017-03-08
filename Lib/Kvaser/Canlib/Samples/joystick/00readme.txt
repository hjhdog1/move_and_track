CANLIB Sample Program 
=====================

Joystick.cpp is a win32 console program that reads the position of a joystick
connected to the game port and sends out CAN messages with the position.

You will need a game port on your computer :-)

Start like this:
  joystick [channel][bitrate][id][msperiod]

where channel is the channel number (default 0), bitrate the CAN bus bit rate
(default 500 kbps), id the CAN identifier (default 500 std) and msperiod
the time between the messages in ms (default 100).

The file mbsig.dbc contains a definition of the transmitted message 
(named "joydata" with id 500)

The program is just intended as an example of how to use CANLIB. It's not
performance optimized in any way. It may, or may not, be useful
for you.

The program is written in C++ and has been tested with Microsoft compilers.

