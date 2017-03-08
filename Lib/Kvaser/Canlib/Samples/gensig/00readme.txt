CANLIB Sample Program : gensig.exe
==================================

This is a 32-bit Windows console mode program that can generate
a number of different signals (sine, ramp, etc.) and transmit them
in a stream of CAN messages.

The messages can be interpreted and viewed in your favourite CAN bus
analysis tool by using the CAN database "gensig.dbc".

The program is written in C and has been tested with Borland and
Microsoft compilers.

Type 'gensig -?' for brief instructions on how to use the program.

Example: try running
  gensig -B500 -trig -0
in a command prompt window - this will generate a stream of CAN messages
on channel 0 (at 500 kbit/s) and the messages will contain encoded sine
and cosine signals.
To view these signals, load gensig.dbc in your bus analyser
and select the one or more signals in the "TrigData" message.

