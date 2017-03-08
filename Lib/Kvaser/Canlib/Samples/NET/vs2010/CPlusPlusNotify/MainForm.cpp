#include "stdafx.h"
#include "MainForm.h"

using namespace CPlusPlusNotify;
using namespace canlibCLSNET;

System::Void MainForm::DisplayStatusInfo(String^ funcName, canlibCLSNET::Canlib::canStatus errorCode)
{
    String^ newText = " ";
    OutputTB->AppendText(String::Concat(funcName, " "));
    if (errorCode < Canlib::canStatus::canOK)
    {
        OutputTB->AppendText(String::Concat("returned error code ", errorCode.ToString(), ": "));
        Canlib::canStatus status = Canlib::canGetErrorText(errorCode, newText);
        if (status != Canlib::canStatus::canOK)
        {
            OutputTB->AppendText(String::Concat("Error Lookup Failed - ", status.ToString()));
        }
        else
        {
            OutputTB->AppendText(newText);
        }
        OutputTB->AppendText("\r\n");
    }
    else
    {
        OutputTB->AppendText("returned successfully\r\n");
    }
}

System::Void MainForm::DisplayErrorFrameText(Int64 time)
{
   OutputTB->AppendText(String::Concat("canRead received Error Frame at Time = ", time.ToString(), "\r\n"));
}

System::Void MainForm::DisplayEventInfo(String^ eventName)
{
   OutputTB->AppendText(String::Concat("Event message received:  ", eventName, "\r\n"));
}

System::Void MainForm::DisplayMessageInfo(Int32 identifier, array<Byte>^ message, Int32 dlc, Int32 flags, Int64 time)
{
   OutputTB->AppendText("canRead received:  ID = ");
   OutputTB->AppendText(identifier.ToString("X8"));
   OutputTB->AppendText("h   DLC = ");
   OutputTB->AppendText(dlc.ToString("X1"));
   OutputTB->AppendText("   Data = ");
   for (Int32 i = 0; i < 7; i++) {
      if (i < dlc)
      {
       OutputTB->AppendText(String::Concat(message[i].ToString("X2"), "h  "));
      }
      else
      {
       OutputTB->AppendText("     ");
      }
   }
    
   OutputTB->AppendText("Flags = ");
   OutputTB->AppendText(flags.ToString("X8"));
   OutputTB->AppendText("h   Time = ");
   OutputTB->AppendText(time.ToString("D"));
   OutputTB->AppendText("\r\n");
}

System::Void MainForm::InitChannel_Click(System::Object^  sender, System::EventArgs^  e)
{
   Int32 desiredChannel = Convert::ToInt32(ChannelNumTB->Text);

   ChannelHandle = Canlib::canOpenChannel(desiredChannel, Canlib::canOPEN_ACCEPT_VIRTUAL);
   DisplayStatusInfo("canOpenChannel", (Canlib::canStatus)ChannelHandle);

   Canlib::canStatus status = Canlib::canSetBusParams(ChannelHandle, Canlib::canBITRATE_250K, 0, 0, 0, 0, 0);
   DisplayStatusInfo("canSetBusParams", status);

   status = Canlib::canSetNotify(ChannelHandle, this->Handle, 1);
   DisplayStatusInfo("canSetNotify", status);
}

System::Void MainForm::BusOn_Click(System::Object^  sender, System::EventArgs^  e) 
{
   Canlib::canStatus status = Canlib::canBusOn(ChannelHandle);
   DisplayStatusInfo("canBusOn", status);
}

System::Void MainForm::BusOff_Click(System::Object^  sender, System::EventArgs^  e) 
{
   Canlib::canStatus status = Canlib::canBusOff(ChannelHandle);
   DisplayStatusInfo("canBusOff", status);
}

System::Void MainForm::CloseChannel_Click(System::Object^  sender, System::EventArgs^  e) 
{
   Canlib::canStatus status = Canlib::canClose(ChannelHandle);
   DisplayStatusInfo("canClose", status);
}

System::Void MainForm::WndProc(Message% m)
{
   array<Byte, 1> ^message = gcnew array<Byte>(8);
  
   if (m.Msg == Canlib::WM__CANLIB)
   {
      Canlib::canStatus status;
      switch ((Int32)m.LParam) {
         case Canlib::canEVENT_RX:
            DisplayEventInfo("canEVENT_RX");
            break;
         case Canlib::canEVENT_TX:
            DisplayEventInfo("canEVENT_TX");
            break;
         case Canlib::canEVENT_ERROR:
            DisplayEventInfo("canEVENT_ERROR");
            break;
         case Canlib::canEVENT_STATUS:
            DisplayEventInfo("canEVENT_STATUS");
            break;
         default:
            DisplayEventInfo("*** UNKNOWN EVENT ***");
            break;
      }
      do
      {
         Int32 dlc;
         Int32 flags;
         Int32 identifier;
         Int64 timestamp;
         status = Canlib::canRead(ChannelHandle, identifier, message, dlc, flags, timestamp);
         if (status == Canlib::canStatus::canOK)
         {
            if (flags & Canlib::canMSG_ERROR_FRAME)
            {
               DisplayErrorFrameText(timestamp);
            }
            else
            {
               DisplayMessageInfo(identifier, message, dlc, flags, timestamp);
            }
         }
      }
      while ((status == Canlib::canStatus::canOK));
      if (status != Canlib::canStatus::canERR_NOMSG)
      {
         DisplayStatusInfo("canRead", status);
      }
   }

   Form::WndProc(m);
}
