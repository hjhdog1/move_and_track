#include "stdafx.h"
#include "CPlusPlusForm.h"
#include "NodeDisplay.h"

using namespace CPlusPlusEvent;
using namespace canlibCLSNET;

System::Void CPlusPlusForm::CreateReceiver_Click(System::Object^  sender, System::EventArgs^  e) 
{
	// Limit the number of nodes that can be created
	if (RxNodeCount < MAX_RX_NODES) {
		// Create a new thread
		myRxNodes[RxNodeCount] = gcnew NodeDisplay(RxNodeCount + 1);
		myRxNodes[RxNodeCount]->Show();
		RxNodeCount = RxNodeCount + 1;
	}
	else {
		System::Windows::Forms::MessageBox::Show(
			"You have already created the maximum number of Receive Nodes.  You cannot create more.",
			"Node Creation Error", 
			System::Windows::Forms::MessageBoxButtons::OK, 
			System::Windows::Forms::MessageBoxIcon::Exclamation);
	}
}  // CreateReceiver_Click


System::Void CPlusPlusForm::CreateTransmitterButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	// Limit the number of nodes that can be created
	if (TxNodeCount < MAX_TX_NODES) {
		// Create a new thread
		myTxNode = gcnew Transmitter();
		myTxNode->Show();
		TxNodeCount = TxNodeCount + 1;
	}
	else {
		System::Windows::Forms::MessageBox::Show(
			"You can only create one Transmitter.  You cannot create more.",
			"Transmitter Creation Error", 
			System::Windows::Forms::MessageBoxButtons::OK,
			System::Windows::Forms::MessageBoxIcon::Exclamation);
	}

}  // CreateTransmitterButton_Click


System::Void CPlusPlusForm::ShutdownNodes_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (myTxNode) {
		myTxNode->Close();
	}
	TxNodeCount = 0;
	for (int i = 0; i < RxNodeCount; i++) {
		myRxNodes[i]->RequestShutdown();
	}
	RxNodeCount = 0;

}  // ShutdownNodes_Click
