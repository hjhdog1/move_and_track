This is a 32-bit Windows console mode program that tests the accuracy
of time stamping on two CAN bus channels.

Both CAN bus channels must be connected together in order to run this program.


Here's a typical output of the program:
---------------------------------------------------------------------------
[C:\test]kvTimeStampTester.exe -B1000000 -a0 -b3
Starting test of time stamps with Kvaser CANLIB API.
Time stamp resolution:  10 us

First channel:  Kvaser Memorator Professional #0 (Channel 0).
Second channel: Kvaser Memorator Professional #1 (Channel 1).

*** Loop 0 *** (kvTimeStampTester.exe -B1000000 -a0 -b3 )
INFO: The used channels are either MagiSynced or reside on the same
      physical interface. Expect time stamps to be as good as the hardware
      specification claims.
1: Std msgs
INFO:   285 msgs differed    -10 us
INFO: 19333 msgs differed      0 us
INFO:   382 msgs differed     10 us
INFO: maxDiff = 20 us (20 us)
INFO: time=1761 ms, msg count=20000 (20000), 11357 msgs/s
2: Short msgs
INFO:   333 msgs differed    -10 us
INFO: 19334 msgs differed      0 us
INFO:   333 msgs differed     10 us
INFO: maxDiff = 20 us (20 us)
INFO: time=1113 ms, msg count=20000 (40000), 17969 msgs/s
3. Mixed msgs
INFO:   323 msgs differed    -10 us
INFO: 19337 msgs differed      0 us
INFO:   340 msgs differed     10 us
INFO: maxDiff = 20 us (20 us)
INFO: time=1960 ms, msg count=20000 (60000), 10204 msgs/s

Test SUCCESSFULLY completed.
---------------------------------------------------------------------------

The program is written in C and has been tested with Borland and
Microsoft compilers.

