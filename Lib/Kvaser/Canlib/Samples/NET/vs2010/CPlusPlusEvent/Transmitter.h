#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace canlibCLSNET;

namespace CPlusPlusEvent {

	/// <summary>
	/// Summary for Transmitter
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Transmitter : public System::Windows::Forms::Form
	{
	public:
		Transmitter(void);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Transmitter()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  StandardRB;

	private: System::Windows::Forms::MaskedTextBox^  CanIdTB;
	private: System::Windows::Forms::RadioButton^  ExtendedRB;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::MaskedTextBox^  DlcTB;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::MaskedTextBox^  DataTB1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::MaskedTextBox^  DataTB2;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::MaskedTextBox^  DataTB3;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::MaskedTextBox^  DataTB4;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::MaskedTextBox^  DataTB5;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::MaskedTextBox^  DataTB6;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::MaskedTextBox^  DataTB7;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::MaskedTextBox^  DataTB8;
	private: System::Windows::Forms::Button^  SendButton;

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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->ExtendedRB = (gcnew System::Windows::Forms::RadioButton());
			this->StandardRB = (gcnew System::Windows::Forms::RadioButton());
			this->CanIdTB = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->DlcTB = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->DataTB1 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->DataTB2 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->DataTB3 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->DataTB4 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->DataTB5 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->DataTB6 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->DataTB7 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->DataTB8 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->SendButton = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->ExtendedRB);
			this->groupBox1->Controls->Add(this->StandardRB);
			this->groupBox1->Controls->Add(this->CanIdTB);
			this->groupBox1->Location = System::Drawing::Point(29, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(227, 75);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"CAN Identifier";
			// 
			// ExtendedRB
			// 
			this->ExtendedRB->AutoSize = true;
			this->ExtendedRB->Location = System::Drawing::Point(126, 46);
			this->ExtendedRB->Name = L"ExtendedRB";
			this->ExtendedRB->Size = System::Drawing::Size(70, 17);
			this->ExtendedRB->TabIndex = 2;
			this->ExtendedRB->TabStop = true;
			this->ExtendedRB->Text = L"Extended";
			this->ExtendedRB->UseVisualStyleBackColor = true;
			// 
			// StandardRB
			// 
			this->StandardRB->AutoSize = true;
			this->StandardRB->Location = System::Drawing::Point(31, 46);
			this->StandardRB->Name = L"StandardRB";
			this->StandardRB->Size = System::Drawing::Size(68, 17);
			this->StandardRB->TabIndex = 1;
			this->StandardRB->TabStop = true;
			this->StandardRB->Text = L"Standard";
			this->StandardRB->UseVisualStyleBackColor = true;
			// 
			// CanIdTB
			// 
			this->CanIdTB->Location = System::Drawing::Point(31, 20);
			this->CanIdTB->Mask = L"999999990";
			this->CanIdTB->Name = L"CanIdTB";
			this->CanIdTB->PromptChar = ' ';
			this->CanIdTB->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->CanIdTB->Size = System::Drawing::Size(165, 20);
			this->CanIdTB->TabIndex = 0;
			this->CanIdTB->Text = L"0";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(103, 102);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(28, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"DLC";
			// 
			// DlcTB
			// 
			this->DlcTB->Location = System::Drawing::Point(137, 98);
			this->DlcTB->Mask = L"0";
			this->DlcTB->Name = L"DlcTB";
			this->DlcTB->PromptChar = ' ';
			this->DlcTB->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DlcTB->Size = System::Drawing::Size(44, 20);
			this->DlcTB->TabIndex = 2;
			this->DlcTB->Text = L"0";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(35, 130);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(37, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Byte 1";
			// 
			// DataTB1
			// 
			this->DataTB1->Location = System::Drawing::Point(78, 127);
			this->DataTB1->Mask = L"990";
			this->DataTB1->Name = L"DataTB1";
			this->DataTB1->PromptChar = ' ';
			this->DataTB1->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DataTB1->Size = System::Drawing::Size(39, 20);
			this->DataTB1->TabIndex = 4;
			this->DataTB1->Text = L"0";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(35, 157);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(37, 13);
			this->label3->TabIndex = 5;
			this->label3->Text = L"Byte 2";
			// 
			// DataTB2
			// 
			this->DataTB2->Location = System::Drawing::Point(78, 154);
			this->DataTB2->Mask = L"990";
			this->DataTB2->Name = L"DataTB2";
			this->DataTB2->PromptChar = ' ';
			this->DataTB2->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DataTB2->Size = System::Drawing::Size(39, 20);
			this->DataTB2->TabIndex = 6;
			this->DataTB2->Text = L"0";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(35, 184);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(37, 13);
			this->label4->TabIndex = 7;
			this->label4->Text = L"Byte 3";
			// 
			// DataTB3
			// 
			this->DataTB3->Location = System::Drawing::Point(78, 181);
			this->DataTB3->Mask = L"990";
			this->DataTB3->Name = L"DataTB3";
			this->DataTB3->PromptChar = ' ';
			this->DataTB3->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DataTB3->Size = System::Drawing::Size(39, 20);
			this->DataTB3->TabIndex = 8;
			this->DataTB3->Text = L"0";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(35, 211);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(37, 13);
			this->label5->TabIndex = 9;
			this->label5->Text = L"Byte 4";
			// 
			// DataTB4
			// 
			this->DataTB4->Location = System::Drawing::Point(78, 208);
			this->DataTB4->Mask = L"990";
			this->DataTB4->Name = L"DataTB4";
			this->DataTB4->PromptChar = ' ';
			this->DataTB4->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DataTB4->Size = System::Drawing::Size(39, 20);
			this->DataTB4->TabIndex = 10;
			this->DataTB4->Text = L"0";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(168, 130);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(37, 13);
			this->label6->TabIndex = 11;
			this->label6->Text = L"Byte 5";
			// 
			// DataTB5
			// 
			this->DataTB5->Location = System::Drawing::Point(211, 127);
			this->DataTB5->Mask = L"990";
			this->DataTB5->Name = L"DataTB5";
			this->DataTB5->PromptChar = ' ';
			this->DataTB5->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DataTB5->Size = System::Drawing::Size(39, 20);
			this->DataTB5->TabIndex = 12;
			this->DataTB5->Text = L"0";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(168, 157);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(37, 13);
			this->label7->TabIndex = 13;
			this->label7->Text = L"Byte 6";
			// 
			// DataTB6
			// 
			this->DataTB6->Location = System::Drawing::Point(211, 154);
			this->DataTB6->Mask = L"990";
			this->DataTB6->Name = L"DataTB6";
			this->DataTB6->PromptChar = ' ';
			this->DataTB6->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DataTB6->Size = System::Drawing::Size(39, 20);
			this->DataTB6->TabIndex = 14;
			this->DataTB6->Text = L"0";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(168, 184);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(37, 13);
			this->label8->TabIndex = 15;
			this->label8->Text = L"Byte 7";
			// 
			// DataTB7
			// 
			this->DataTB7->Location = System::Drawing::Point(211, 181);
			this->DataTB7->Mask = L"990";
			this->DataTB7->Name = L"DataTB7";
			this->DataTB7->PromptChar = ' ';
			this->DataTB7->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DataTB7->Size = System::Drawing::Size(39, 20);
			this->DataTB7->TabIndex = 16;
			this->DataTB7->Text = L"0";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(168, 211);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(37, 13);
			this->label9->TabIndex = 17;
			this->label9->Text = L"Byte 8";
			// 
			// DataTB8
			// 
			this->DataTB8->Location = System::Drawing::Point(211, 208);
			this->DataTB8->Mask = L"990";
			this->DataTB8->Name = L"DataTB8";
			this->DataTB8->PromptChar = ' ';
			this->DataTB8->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->DataTB8->Size = System::Drawing::Size(39, 20);
			this->DataTB8->TabIndex = 18;
			this->DataTB8->Text = L"0";
			// 
			// SendButton
			// 
			this->SendButton->Location = System::Drawing::Point(105, 240);
			this->SendButton->Name = L"SendButton";
			this->SendButton->Size = System::Drawing::Size(75, 23);
			this->SendButton->TabIndex = 19;
			this->SendButton->Text = L"SEND";
			this->SendButton->UseVisualStyleBackColor = true;
			this->SendButton->Click += gcnew System::EventHandler(this, &Transmitter::SendButton_Click);
			// 
			// Transmitter
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 274);
			this->ControlBox = false;
			this->Controls->Add(this->SendButton);
			this->Controls->Add(this->DataTB8);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->DataTB7);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->DataTB6);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->DataTB5);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->DataTB4);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->DataTB3);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->DataTB2);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->DataTB1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->DlcTB);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->groupBox1);
			this->Name = L"Transmitter";
			this->Text = L"Transmitter";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Transmitter::Transmitter_FormClosing);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private:
			int channelHandle;

			void DisplayError(canlibCLSNET::Canlib::canStatus errorCode, String^ msgTitle);
    
	private: System::Void Transmitter_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
			 
	private: System::Void SendButton_Click(System::Object^  sender, System::EventArgs^  e);

};
}
