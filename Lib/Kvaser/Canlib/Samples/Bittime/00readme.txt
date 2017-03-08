This is a program that calculates the available CAN bus parameters given
the clock frequency and the desited bit rate on the bus.

For example, if your system is using a 16 MHz clock for the CAN controller,
and you want a bus speed of 250 kbit/s, enter "250" in the "Bit rate" box
and "16" in the "CAN Clock" box. The table you now will see lists

- the number of quanta before the sampling point ("TSeg1")
- the number of quanta after the sampling poing ("TSeg2")
- the position of the sampling point in percent of the whole bit
- the number of quanta in one bit ("BTQ")
- the Synchronization Jump Width ("SJW")
- the exact bus speed
- the deviation, in percent, between the bus speed you really will get
  and the bus speed you wanted.

Click on a line to see the parameters you should pass to
canSetBusParameters() to get the settings on that line.

Right-click in the table to switch between 0.5% and 1.5% maximum
error in the bus speed.  The default is to show all settings giving
an error which is at most 0.5%.

The basics of the CAN bit timing is described in detail on
http://www.kvaser.com.

The program is written in in Borland Delphi. The algorithm used to
calculate the bit timing should be easy to understand anyway, and you
can reuse it in your own programs whenever you want to let the users
select CAN bus parameters.


