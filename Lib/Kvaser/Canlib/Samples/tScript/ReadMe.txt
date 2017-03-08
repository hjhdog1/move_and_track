
                   Copyright 2010 by KVASER AB, SWEDEN      
                        WWW: http://www.kvaser.com

 This software is furnished under a license and may be used and copied
 only in accordance with the terms of such license.

 Description:
 Overview of script examples
 ---------------------------------------------------------------------------


This is a collection of script samples that demonstrates various
aspects of the t script language.

Note that not all script examples are complete! Instead they try to
highlight some specific t script usage, and some script may omit
e.g. connecting to the CAN bus.

Some scripts interact with environment variables, which can be read
and written through calls to canlib.

To compile any of the following examples use

    scc.exe [-dbase=examples.dbc] <script.t>

Examples:
    scc.exe -dbase=examples.dbc can_calc.t
    scc.exe all_can.t
    
Note: If you have installed CANLIB SDK to its default location in Windows, i.e.
in c:\Program Files\. You'll need to copy the script samples to a writable
folder before compiling.


List of example scripts
-----------------------

bus_ctrl.t          - Set up CAN buses for use, and change bitrate from envvar.
default_can.t       - Demonstrates the usage of 'on CanMessage *'
all_can.t           - Demonstrates the usage of 'on CanMessage [*]'
can_calc.t         *- Demonstrate calculations using signal values.
periodic_can.t    **- Send data from an environment variable once per second.
remote_response.t   - Respond to a remote request.
saveCan.t           - Writes incomming CAN messages to file.
script_ctrl.t     **- Use environment variables to load and start script.
logger_ctrl.t     **- Interact with logging from the script.
timer_array.t       - Use array of timers to delay the message response.
send_list.t       **- Send according to a transmit list file, given by an environment variable.
sine_wave.t         - Using a database defined signal, send out sine wave data.
osc_data.t         *- Record and replay "oscilloscope" data.
exc_dump.t          - Demonstrates how to dump exception information.
kwp2000.t/.inc      - Implementation and example use of a subset of kwp2000.
licence.t           - Demonstrates how licences can be created and checked.
customer_data.t   **- Demonstrates how customer data can be read. 
                      You need to write data with Customer Data Utility (through
                      Kvaser Hardware/Tools) or hydra_setparam_userdata.exe

examples.dbc        - Database needed for some examples

histogram/histogram.t ***- Demonstrates the possibility to record data into histogram.

*) Needs a database to compile, add -dbase examples.dbc
**) Needs some special support (e.g. external program using canlib, 
    another script file, configured param.lif)
***) Needs a special database located in the same directory as the program.

