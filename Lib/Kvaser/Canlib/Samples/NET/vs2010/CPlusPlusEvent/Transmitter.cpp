#include "StdAfx.h"
#include "Transmitter.h"

using namespace CPlusPlusEvent;
using namespace canlibCLSNET;

Transmitter::Transmitter(void)
{
	InitializeComponent();
	//
	//TODO: Add the constructor code here
	//

	// restrict control entry
	StandardRB->Checked = true;

	canlibCLSNET::Canlib::canStatus status;

	// Open the channel (The flags will allow a virtual channel to be used)
	channelHandle = canlibCLSNET::Canlib::canOpenChannel(1, canlibCLSNET::Canlib::canOPEN_ACCEPT_VIRTUAL);
	DisplayError(((Canlib::canStatus)channelHandle), "canOpenChannel");

	// Set the bit params (In this case 250 kbits/sec)
	status = canlibCLSNET::Canlib::canSetBusParams(channelHandle, canlibCLSNET::Canlib::canBITRATE_250K, 0, 0, 0, 0, 0);
	DisplayError(status, "canSetBusParams");

	// Make the channel active
	status = canlibCLSNET::Canlib::canBusOn(channelHandle);
	DisplayError(status, "canBusOn");
}


void Transmitter::DisplayError(canlibCLSNET::Canlib::canStatus errorCode, String^ msgTitle)
{
	String^ msgDescr = " ";
	if (errorCode < canlibCLSNET::Canlib::canStatus::canOK)
	{
		if (canlibCLSNET::Canlib::canGetErrorText(errorCode, msgDescr) != 
			canlibCLSNET::Canlib::canStatus::canOK)
		{
			msgDescr = "Unknown error";
		}
		MessageBox::Show(msgTitle + " returned error code " + ((int)errorCode).ToString() + " - " + msgDescr, 
			             msgTitle, 
						 MessageBoxButtons::OK, 
						 MessageBoxIcon::Exclamation);
	}
}

System::Void Transmitter::Transmitter_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
{
	Canlib::canStatus status;
	
	// Make the channel inactive
	status = canlibCLSNET::Canlib::canBusOff(channelHandle);
	DisplayError(status, "canBusOff");

	status = canlibCLSNET::Canlib::canClose(channelHandle);
	DisplayError(status, "canClose");
}

System::Void Transmitter::SendButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	Int32 id = 0;
	array<unsigned char>^ data = gcnew array<unsigned char>(8);
	int dlc = 0;
	Int32 flag = 0;

	id = Convert::ToInt32(CanIdTB->Text);
	dlc = Convert::ToInt32(DlcTB->Text);
	data[0] = (unsigned char) Convert::ToInt16(DataTB1->Text);
	data[1] = (unsigned char) Convert::ToInt16(DataTB2->Text);
	data[2] = (unsigned char) Convert::ToInt16(DataTB3->Text);
	data[3] = (unsigned char) Convert::ToInt16(DataTB4->Text);
	data[4] = (unsigned char) Convert::ToInt16(DataTB5->Text);
	data[5] = (unsigned char) Convert::ToInt16(DataTB6->Text);
	data[6] = (unsigned char) Convert::ToInt16(DataTB7->Text);
	data[7] = (unsigned char) Convert::ToInt16(DataTB8->Text);
	if (StandardRB->Checked) {
		flag = canlibCLSNET::Canlib::canMSG_STD;
	}
	else {
		flag = canlibCLSNET::Canlib::canMSG_EXT;
	}
	Canlib::canStatus status = Canlib::canWrite(channelHandle, id, data, dlc, flag);

}
