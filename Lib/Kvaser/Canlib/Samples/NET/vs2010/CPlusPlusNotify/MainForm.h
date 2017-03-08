#pragma once


namespace CPlusPlusNotify {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
   using namespace canlibCLSNET;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
   private: System::Windows::Forms::Button^  InitChannel;
   protected: 
   private: System::Windows::Forms::Button^  BusOn;
   private: System::Windows::Forms::Button^  BusOff;
   private: System::Windows::Forms::Button^  CloseChannel;
   private: System::Windows::Forms::Label^  label1;
   private: System::Windows::Forms::MaskedTextBox^  ChannelNumTB;
   private: System::Windows::Forms::RichTextBox^  OutputTB;


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
         this->InitChannel = (gcnew System::Windows::Forms::Button());
         this->BusOn = (gcnew System::Windows::Forms::Button());
         this->BusOff = (gcnew System::Windows::Forms::Button());
         this->CloseChannel = (gcnew System::Windows::Forms::Button());
         this->label1 = (gcnew System::Windows::Forms::Label());
         this->ChannelNumTB = (gcnew System::Windows::Forms::MaskedTextBox());
         this->OutputTB = (gcnew System::Windows::Forms::RichTextBox());
         this->SuspendLayout();
         // 
         // InitChannel
         // 
         this->InitChannel->Location = System::Drawing::Point(13, 13);
         this->InitChannel->Name = L"InitChannel";
         this->InitChannel->Size = System::Drawing::Size(75, 39);
         this->InitChannel->TabIndex = 0;
         this->InitChannel->Text = L"Initialize Channel";
         this->InitChannel->UseVisualStyleBackColor = true;
         this->InitChannel->Click += gcnew System::EventHandler(this, &MainForm::InitChannel_Click);
         // 
         // BusOn
         // 
         this->BusOn->Location = System::Drawing::Point(13, 72);
         this->BusOn->Name = L"BusOn";
         this->BusOn->Size = System::Drawing::Size(75, 23);
         this->BusOn->TabIndex = 1;
         this->BusOn->Text = L"Go Bus On";
         this->BusOn->UseVisualStyleBackColor = true;
         this->BusOn->Click += gcnew System::EventHandler(this, &MainForm::BusOn_Click);
         // 
         // BusOff
         // 
         this->BusOff->Location = System::Drawing::Point(13, 115);
         this->BusOff->Name = L"BusOff";
         this->BusOff->Size = System::Drawing::Size(75, 23);
         this->BusOff->TabIndex = 2;
         this->BusOff->Text = L"Go Bus Off";
         this->BusOff->UseVisualStyleBackColor = true;
         this->BusOff->Click += gcnew System::EventHandler(this, &MainForm::BusOff_Click);
         // 
         // CloseChannel
         // 
         this->CloseChannel->Location = System::Drawing::Point(13, 158);
         this->CloseChannel->Name = L"CloseChannel";
         this->CloseChannel->Size = System::Drawing::Size(75, 39);
         this->CloseChannel->TabIndex = 3;
         this->CloseChannel->Text = L"Close Channel";
         this->CloseChannel->UseVisualStyleBackColor = true;
         this->CloseChannel->Click += gcnew System::EventHandler(this, &MainForm::CloseChannel_Click);
         // 
         // label1
         // 
         this->label1->AutoSize = true;
         this->label1->Location = System::Drawing::Point(111, 13);
         this->label1->Name = L"label1";
         this->label1->Size = System::Drawing::Size(85, 13);
         this->label1->TabIndex = 4;
         this->label1->Text = L"Desired Channel";
         // 
         // ChannelNumTB
         // 
         this->ChannelNumTB->Location = System::Drawing::Point(114, 32);
         this->ChannelNumTB->Mask = L"90";
         this->ChannelNumTB->Name = L"ChannelNumTB";
         this->ChannelNumTB->PromptChar = ' ';
         this->ChannelNumTB->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
         this->ChannelNumTB->Size = System::Drawing::Size(44, 20);
         this->ChannelNumTB->TabIndex = 5;
         this->ChannelNumTB->Text = L"0";
         // 
         // OutputTB
         // 
         this->OutputTB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
            | System::Windows::Forms::AnchorStyles::Left) 
            | System::Windows::Forms::AnchorStyles::Right));
         this->OutputTB->Location = System::Drawing::Point(114, 72);
         this->OutputTB->Name = L"OutputTB";
         this->OutputTB->Size = System::Drawing::Size(670, 276);
         this->OutputTB->TabIndex = 6;
         this->OutputTB->Text = L"";
         // 
         // Form1
         // 
         this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
         this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
         this->ClientSize = System::Drawing::Size(796, 360);
         this->Controls->Add(this->OutputTB);
         this->Controls->Add(this->ChannelNumTB);
         this->Controls->Add(this->label1);
         this->Controls->Add(this->CloseChannel);
         this->Controls->Add(this->BusOff);
         this->Controls->Add(this->BusOn);
         this->Controls->Add(this->InitChannel);
         this->Name = L"Form1";
         this->Text = L"C++ Notify";
         this->ResumeLayout(false);
         this->PerformLayout();

      }
#pragma endregion
   private: System::Void InitChannel_Click(System::Object^  sender, System::EventArgs^  e);

   private: System::Void BusOn_Click(System::Object^  sender, System::EventArgs^  e);
            
   private: System::Void BusOff_Click(System::Object^  sender, System::EventArgs^  e);
            
   private: System::Void CloseChannel_Click(System::Object^  sender, System::EventArgs^  e);

   protected: virtual System::Void WndProc(Message% m) override;
   private: System::Void DisplayStatusInfo(String^ funcName, canlibCLSNET::Canlib::canStatus errorCode);
   private: System::Void DisplayErrorFrameText(Int64 time);
   private: System::Void DisplayEventInfo(String^ eventName);
   private: System::Void DisplayMessageInfo(Int32 identifier, array<Byte>^ message, Int32 dlc, Int32 flags, Int64 time);
    
   private:
      System::Int32 ChannelHandle;
};
}

