#include "StdAfx.h"
#include "NodeDisplay.h"
#include "CanlibWaitEvent.h"

using namespace CPlusPlusEvent;

NodeDisplay::NodeDisplay(int NodeId)
{
	InitializeComponent();
	reqShutdown = false;
	myId = NodeId;
	Text = "Receiver Node " + NodeId.ToString();
	textDelegate = gcnew AddTextDelegate(this, &NodeDisplay::AddTextMethod);
	statusDelegate = gcnew AddStatusDelegate(this, &NodeDisplay::DisplayStatusInfo);
	msgDelegate = gcnew AddMsgDelegate(this, &NodeDisplay::DisplayMessageInfo);
	errDelegate = gcnew AddErrFrameDelegate(this, &NodeDisplay::DisplayErrorFrameText);
	myThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &NodeDisplay::ThreadFunction));
	myThread->Start();
} // NodeDisplay constructor


void NodeDisplay::AddTextMethod(String^ newText)
{
	MsgText->AppendText(newText);
}

void NodeDisplay::DisplayErrorFrameText(Int64 time)
{
	MsgText->AppendText("canRead received Error Frame at Time = " + time.ToString() + "\r\n");

}

void NodeDisplay::DisplayMessageInfo(int identifier, array<unsigned char>^ message, 
									 int dlc, int flags, Int64 time)
{
	MsgText->AppendText("canRead received:  ID = " + identifier.ToString("X8") +
						"h   DLC = " + dlc.ToString("X1") + "   Data = ");

	for (int i = 0; i < 8; i++) {
		if (i < dlc) {
			MsgText->AppendText(message[i].ToString("X2") + "h  ");
        }
        else {
			MsgText->AppendText("     ");
        }
	}
	MsgText->AppendText("Flags = " + flags.ToString("X8") + "h   Time = " +
						time.ToString("D") + "\r\n");
}

void NodeDisplay::DisplayStatusInfo(String^ funcName, Canlib::canStatus errorCode)
{
	String^ newText = " ";
	MsgText->AppendText(funcName + " ");
	if (errorCode < Canlib::canStatus::canOK) {
		MsgText->AppendText("returned error code " + ((int)errorCode).ToString() + ": ");
		int status = (int)Canlib::canGetErrorText(errorCode, newText);
		if (status != 0)
		{
			MsgText->AppendText("Error Lookup Failed - " + status.ToString());
		}
		else
		{
			MsgText->AppendText(newText);
		}
		MsgText->AppendText("\r\n");
	}
	else {
		MsgText->AppendText("returned successfully\r\n");
	}
} // DisplayStatusInfo

void NodeDisplay::RequestShutdown()
{
	reqShutdown = true;
} // RequestShutdown

bool NodeDisplay::RequestShutdownStatus()
{
	return (reqShutdown);
} // RequestShutdownStatus


void NodeDisplay::ThreadFunction()
{
	ReceiveNode^ myThreadObject = gcnew ReceiveNode(this);
	myThreadObject->Run();
}

void ReceiveNode::Run()
{
	// The real work performed by the Thread is in this routine
	int channelHandle;
	canlibCLSNET::Canlib::canStatus status;
	Object^ winHandle = gcnew Object();

	// Open the channel (The flags will allow a virtual channel to be used)
	channelHandle = canlibCLSNET::Canlib::canOpenChannel(0, canlibCLSNET::Canlib::canOPEN_ACCEPT_VIRTUAL);
	myForm->Invoke(myForm->statusDelegate, gcnew array<Object^> {"canOpenChannel", channelHandle});

	// Set the bit params (In this case 250 kbits/sec)
	status = canlibCLSNET::Canlib::canSetBusParams(channelHandle, canlibCLSNET::Canlib::canBITRATE_250K, 0, 0, 0, 0, 0);
	myForm->Invoke(myForm->statusDelegate, gcnew array<Object^> {"canSetBusParams", status});

	status = canlibCLSNET::Canlib::canIoCtl(channelHandle, canlibCLSNET::Canlib::canIOCTL_GET_EVENTHANDLE, winHandle);
	myForm->Invoke(myForm->statusDelegate, gcnew array<Object^> {"canIoCtl", status});

	// Make the channel active
	status = canlibCLSNET::Canlib::canBusOn(channelHandle);
	myForm->Invoke(myForm->statusDelegate, gcnew array<Object^> {"canBusOn", status});

	// Put the event handle into a usuable variable form
	CanlibWaitEvent^ kvEvent = gcnew CanlibWaitEvent(winHandle);
	array<System::Threading::WaitHandle^>^ waitHandles = {kvEvent};
	bool NotShutdown = true;

	while (NotShutdown & !(myForm->RequestShutdownStatus())) {
		// Wait for 1 second or the occurence of a Kvaser event
		if (System::Threading::WaitHandle::WaitAny(waitHandles, 1000, false) == 0) {
			int id = 0;
			array<unsigned char>^ data = gcnew array<unsigned char>(8);
			int dlc = 0;
			int flag = 0;
			Int64 time = 0;
			
			// Loop until the receive buffer is empty
			do {
				status = canlibCLSNET::Canlib::canRead(channelHandle, id, data, dlc, flag, time);
				if (status == canlibCLSNET::Canlib::canStatus::canOK) {
					if ((flag & canlibCLSNET::Canlib::canMSG_ERROR_FRAME) == 0) {
						// Message data has been returned
						myForm->Invoke(myForm->msgDelegate, gcnew array<Object^> {id, data, dlc, flag, time});
					}
					else {
						// Error Frame data has been returned.
						myForm->Invoke(myForm->errDelegate, gcnew array<Object^> {time});
					}
				}
			} while (status == canlibCLSNET::Canlib::canStatus::canOK);

			// Check that we exited the loop because the incoming message buffer was empty
			if (status != canlibCLSNET::Canlib::canStatus::canERR_NOMSG) {
				myForm->Invoke(myForm->statusDelegate, gcnew array<Object^> {"canRead", status});
				myForm->Invoke(myForm->textDelegate, gcnew array<Object^> {"Halting Receive Node \r\n"});
				NotShutdown = false;
			}
		} // WaitAny returned because of a CAN event
	}


	// Make the channel inactive
	status = canlibCLSNET::Canlib::canBusOff(channelHandle);
	myForm->Invoke(myForm->statusDelegate, gcnew array<Object^> {"canBusOff", status});

	status = canlibCLSNET::Canlib::canClose(channelHandle);
	myForm->Invoke(myForm->statusDelegate, gcnew array<Object^> {"canClose", status});

	myForm->Invoke(myForm->textDelegate, gcnew array<Object^> {"Thread has completed execution...  Goodbye!"});
}