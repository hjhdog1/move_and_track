This is a 32-bit Windows console mode program that demonstrates how to use
CANLIB at a fairly basic level.

PCcan users:

Note that this program assumes that the CAN busses of the PCcan card
are connected together and are terminated.  You can accomplish this
by setting all the switches on the large switch pack to ON. (Not all
versions of PCCAN support this for high bus speeds; please consult
the manual.)

LAPcan users:

Both CAN busses must be connected together in order to run this program.


Here's a typical output of the program:
---------------------------------------------------------------------------
Starting...
canLocateHardware...OK.
canOpen, channel 0...OK.
Setting the bus speed...OK.
Go bus-on...OK.
canOpen, channel 1...OK.
Setting the bus speed...OK.
Go bus-on...OK.

Sending 100 messages from A to B.
Sending 100 messages in the opposite direction.
Sending 150 messages from A to B.
Sending 150 messages in the opposite direction.

Demonstrating canReadWait()
Send 10 messages from the first controller, and read them from
the second one; wait until they arrive if necessary.

Demonstrating canReadSync().
Send one message from the first controller, pick it up at the
second one after calling canReadSync().

Demonstrating canReadSpecific().
Send a couple of messages and pick them up in the opposite order.

Testing canReadSpecificSkip()
Send a couple of messages and pick up the last one, skipping the
others.

That's all for today!
---------------------------------------------------------------------------

The program is written in C and has been tested with Borland and
Microsoft compilers.

