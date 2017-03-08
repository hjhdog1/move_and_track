#pragma once
#include "NodeDisplay.h"
#include "Transmitter.h"

namespace CPlusPlusEvent {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for CPlusPlusForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CPlusPlusForm : public System::Windows::Forms::Form
	{
	public:
		CPlusPlusForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			RxNodeCount = 0;
			TxNodeCount = 0;
			myRxNodes = gcnew array<NodeDisplay^>(MAX_RX_NODES + 1);

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CPlusPlusForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  CreateReceiver;
	private: System::Windows::Forms::Button^  CreateTransmitterButton;
	private: System::Windows::Forms::Button^  ShutdownNodes;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->CreateReceiver = (gcnew System::Windows::Forms::Button());
			this->CreateTransmitterButton = (gcnew System::Windows::Forms::Button());
			this->ShutdownNodes = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// CreateReceiver
			// 
			this->CreateReceiver->Location = System::Drawing::Point(69, 21);
			this->CreateReceiver->Name = L"CreateReceiver";
			this->CreateReceiver->Size = System::Drawing::Size(147, 52);
			this->CreateReceiver->TabIndex = 0;
			this->CreateReceiver->Text = L"Create Receiving Node";
			this->CreateReceiver->UseVisualStyleBackColor = true;
			this->CreateReceiver->Click += gcnew System::EventHandler(this, &CPlusPlusForm::CreateReceiver_Click);
			// 
			// CreateTransmitterButton
			// 
			this->CreateTransmitterButton->Location = System::Drawing::Point(69, 106);
			this->CreateTransmitterButton->Name = L"CreateTransmitterButton";
			this->CreateTransmitterButton->Size = System::Drawing::Size(147, 52);
			this->CreateTransmitterButton->TabIndex = 1;
			this->CreateTransmitterButton->Text = L"Create Transmitter";
			this->CreateTransmitterButton->UseVisualStyleBackColor = true;
			this->CreateTransmitterButton->Click += gcnew System::EventHandler(this, &CPlusPlusForm::CreateTransmitterButton_Click);
			// 
			// ShutdownNodes
			// 
			this->ShutdownNodes->Location = System::Drawing::Point(69, 191);
			this->ShutdownNodes->Name = L"ShutdownNodes";
			this->ShutdownNodes->Size = System::Drawing::Size(147, 52);
			this->ShutdownNodes->TabIndex = 2;
			this->ShutdownNodes->Text = L"Shutdown Nodes";
			this->ShutdownNodes->UseVisualStyleBackColor = true;
			this->ShutdownNodes->Click += gcnew System::EventHandler(this, &CPlusPlusForm::ShutdownNodes_Click);
			// 
			// CPlusPlusForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 264);
			this->Controls->Add(this->ShutdownNodes);
			this->Controls->Add(this->CreateTransmitterButton);
			this->Controls->Add(this->CreateReceiver);
			this->Name = L"CPlusPlusForm";
			this->Text = L"C++ Sample";
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
        literal int MAX_RX_NODES = 3;
		literal int MAX_TX_NODES = 1;
		int RxNodeCount;
		int TxNodeCount;
		array<NodeDisplay^>^ myRxNodes;
		Transmitter^ myTxNode;

	private: System::Void CreateReceiver_Click(System::Object^  sender, System::EventArgs^  e);

	private: System::Void CreateTransmitterButton_Click(System::Object^  sender, System::EventArgs^  e);

	private: System::Void ShutdownNodes_Click(System::Object^  sender, System::EventArgs^  e);

};
}

