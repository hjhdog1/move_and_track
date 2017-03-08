// CLRcandump.cpp : main project file.

#include "stdafx.h"
#include "stdlib.h"

using namespace System;
using namespace System::IO;
using namespace canlibCLSNET;

//
// Global variables for the command-line options.
// 
static int  quiet = 0;                      // Defines the verbosity.
static bool logToFile = false;
static bool allowVirtual = false;
static long int msgCount = 0;
static long int overRuns = 0;
static __int64 timeOffset = 0;

static void DisplayHelp(void)
{
    Console::Write("\nCANLIB Dump Program\n");
    Console::Write("(Part of the CANLIB SDK from KVASER AB - http://www.kvaser.se)\n");
    Console::Write("\n");
    Console::Write("This is a sample program that just dumps all incoming messages to the screen\n");
    Console::Write("or a file.\n");
    Console::Write("\nUsage: candump [flags] [filename]\n");
    Console::Write("   -X          Listen to CAN channel number X.\n");
    Console::Write("   -B<value>   Set the bitrate. Value is any of 1000,500,250,125.\n");
    Console::Write("               Default bitrate is 125 kbit/s.\n");
    Console::Write("   -h          Print this help text.\n");
    Console::Write("   -q          Be more quiet than usual.\n");
    Console::Write("   -v          Allow use of virtual channel.\n");
    Console::Write("\nIf no filename is specified, standard output is used.\n\n");
    Console::Write("The following keys can be used during logging:\n");
    Console::Write("   ESC         Stop logging.\n");
    Console::Write("   Q           Be more quiet.\n");
    Console::Write("   q           Be less quiet.\n");
    Console::Write("\nExample:\n");
    Console::Write("candump -B250 -0 logfile.log\n");
    Console::Write("   would set CAN channel 0 to 250 kbit/s and log to logfile.log\n");
    Environment::Exit(0);
}

void ReportError(String^ funcName, Canlib::canStatus status)
{
    IO::TextWriter^ errOutput = Console::Error;
    String^ errString;
    Canlib::canGetErrorText(status, errString);
    errOutput->WriteLine("{0}: failed, status = {1} ({2})", funcName, status, errString);
}

int PrepareChannel(int channel, int bitrate)
{
    Canlib::canStatus retStatus;

    // initialize the API library
    Canlib::canInitializeLibrary();

    //
    // First, open a handle to the CAN circuit. Specifying
    // canOPEN_EXCLUSIVE ensures we get a circuit that noone else
    // is using.
    //
    int flags = Canlib::canOPEN_EXCLUSIVE | ((allowVirtual) ? Canlib::canOPEN_ACCEPT_VIRTUAL : 0);
    int hnd = Canlib::canOpenChannel(channel, flags);
    if (hnd < 0) {
        retStatus = (Canlib::canStatus)hnd;
        ReportError("canOpenChannel", retStatus);
        Environment::Exit(1);
    }

    //
    // Using our new shiny handle, we specify the baud rate
    // using one of the convenient canBITRATE_xxx constants.
    //
    // The bit layout is in depth discussed in most CAN
    // controller data sheets, and on the web at
    // http://www.kvaser.se.
    //
    if ((retStatus = Canlib::canSetBusParams(hnd, bitrate, 0, 0, 0, 0, 0)) != 
         Canlib::canStatus::canOK) {
        ReportError("canSetBusParams", retStatus);
        Canlib::canClose(hnd);
        Environment::Exit(1);
    }
    //
    // Then we start the ball rolling.
    // 
    if ((retStatus = Canlib::canBusOn(hnd)) != Canlib::canStatus::canOK) {
        ReportError("canBusOn", retStatus);
        Canlib::canClose(hnd);
        Environment::Exit(1);
    }

    // Return the handle; our caller will need it for
    // further exercising the CAN controller.
    return hnd;
}

void DisplayLeadIn(int handle)
{
    if (quiet == 0) {
        using namespace System::Globalization;
        DateTime dt = DateTime::Now;
        // setup our timer offset
        timeOffset = Canlib::canReadTimer(handle);

        Console::WriteLine("; Logging started at {0}", dt.ToString("U", DateTimeFormatInfo::InvariantInfo));
        Console::WriteLine("; (x = Extended Id, R = Remote Frame, o = Overrun, N = NERR)");
        Console::WriteLine(";  Ident xRoN DLC  Data 0.....................7        Time");
    }
}
void DisplaySummary(int handle)
{
    if (quiet == 0) {
        using namespace System::Globalization;
        DateTime dt = DateTime::Now;
        Console::WriteLine("; Logging ended at {0}", dt.ToString("U", DateTimeFormatInfo::InvariantInfo));
        Console::WriteLine("; {0} messages received.", msgCount);
        Console::WriteLine("; {0} overrun conditions detected.", overRuns);
    }
}

void EmptyReceiveBuffer(int handle)
{
    Canlib::canStatus status;
    int id, dlc, flags;
    array <unsigned char, 1> ^msg = gcnew array<unsigned char>(8);
    __int64 time;
    __int64 adjTime, timeFrac, timeInt;
    int i;

    //
    // Read a message.
    //
    while ((status = Canlib::canRead(handle, id, msg, dlc, flags, time)) == Canlib::canStatus::canOK) {
        if (quiet <= 1) {
            if ((flags & Canlib::canMSG_ERROR_FRAME) == 0) {
                // A message real message so print identifier and flags.
                Console::Write("{0:X8} {1}{2}{3}{4}  {5:G2}        ",
                               id,
                               (flags & Canlib::canMSG_EXT)        ? "x" : " ",
                               (flags & Canlib::canMSG_RTR)        ? "R" : " ",
                               (flags & Canlib::canMSGERR_OVERRUN) ? "o" : " ",
                               (flags & Canlib::canMSG_NERR)       ? "N" : " ", // TJA 1053/1054 transceivers only
                               dlc);
                // Print the data bytes, but not for Remote Frames.
                // Print at most 8 bytes - the DLC might be larger
                // but there are never more than 8 bytes in a
                // message.
                if (dlc > 8) dlc = 8;
                if ((flags & Canlib::canMSG_RTR) == 0) {
                    for (i = 0; i < dlc; i++) Console::Write("{0:X2} ", msg[i]);
                    for (; i < 8; i++) Console::Write("   ");
                } 
                else {
                    Console::Write("                        ");
                }
                msgCount++;
            }
            else {
                // An error frame.
                // CANLIB will set the id to something controller
                // dependent.
                Console::Write("  Error Frames                                  ");
            }
            

            // Print the time stamp, formatted as seconds.
            adjTime = time - timeOffset;
            if (adjTime < 0) {
                // negative values may occur due to time between BusOn call and ReadTimer call.
                timeFrac = (-adjTime) % 1000;
                timeInt  = (-adjTime) / 1000;
                Console::WriteLine("  -{0,5}.{1:D3}", timeInt, timeFrac);
            } 
            else {
                timeFrac = adjTime % 1000;
                timeInt  = adjTime / 1000;
                Console::WriteLine("  {0,6}.{1:D3}", timeInt, timeFrac);
            }
            
            // Keep some statistics.
            if (flags & Canlib::canMSGERR_OVERRUN) overRuns++;
        }
        //
        // When there are no more messages in the queue,
        // stat will be equal to canERR_NOMSG.
        //
    }

    if (status != Canlib::canStatus::canERR_NOMSG) {
        ReportError("canRead", status);
    }
}

void RecordMessages(int handle)
{
    bool finished = false;
    IO::TextWriter^ errOutput = Console::Error;

    while (!finished) {
        while (!Console::KeyAvailable) {
            EmptyReceiveBuffer(handle);
            Threading::Thread::Sleep(50);
        }

        ConsoleKeyInfo cki;
        cki = Console::ReadKey(true);
        switch (cki.Key) {
            case ConsoleKey::Escape:
                finished = true;
                break;
            case ConsoleKey::Q:
                if ((int)(cki.Modifiers & ConsoleModifiers::Shift)) {
                    if (quiet < 5) quiet++;
                }
                else {
                    if (quiet > 0) quiet--;
                }
                errOutput->WriteLine("New quiet level = {0}", quiet);                
                break;
        }
    }
}

int main(array<System::String ^> ^args)
{
    int  bitrate = Canlib::canBITRATE_125K; // Selected bit rate.
    int  channel = 0;                       // Channel #
    String^ logFileName = "";               // Name of log file
    IO::StreamWriter^ filePtr = nullptr;
    IO::TextWriter^ stdOutput = Console::Out;
    IO::TextWriter^ errOutput = Console::Error;

    // Determine the command line arguements.
    for (int i = 0; i < args->Length; i++) {
        if (args[i][0] != '-') {
            logFileName = String::Copy(args[i]);
            try
            {
                filePtr = File::CreateText(logFileName);
            }
            catch (IO::IOException^ e) 
            {
                errOutput->WriteLine(e->Message);
                DisplayHelp();
            }
            logToFile = true;
        }
        else if (args[i][1] == 'h') {
            // needs help
            DisplayHelp();
        }
        else if (args[i][1] == 'q') {
            // shhhhhhh!
            quiet++;
        }
        else if (args[i][1] == 'v') {
            allowVirtual = true;
        }
        else if (args[i][1] == 'B') {
            // baudrate chosen
            int tmp = Convert::ToInt32(args[i]->Substring(2));
            switch (tmp) {
                case 1000: bitrate = Canlib::canBITRATE_1M;   break;
                case 500 : bitrate = Canlib::canBITRATE_500K; break;
                case 250 : bitrate = Canlib::canBITRATE_250K; break;
                case 125 : bitrate = Canlib::canBITRATE_125K; break;
                default  : DisplayHelp();                     break;
            }
        }
        else {
            // channel number to use
            channel = Convert::ToInt32(args[i]->Substring(1));
        }
    }
    
    int handle = PrepareChannel(channel, bitrate);
    
    if ((Canlib::canStatus)handle >= Canlib::canStatus::canOK) {
        // everything initialized so lets start tracking data
        if (logToFile) Console::SetOut(filePtr);

        DisplayLeadIn(handle);

        RecordMessages(handle);

        DisplaySummary(handle);

        // close channel
        Canlib::canBusOff(handle);
        Canlib::canClose(handle);

        // close the log file
        if (logToFile)  filePtr->Close();
    }


    return 0;
}
