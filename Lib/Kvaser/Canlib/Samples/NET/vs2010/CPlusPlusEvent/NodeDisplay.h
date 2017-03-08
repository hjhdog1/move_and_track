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
	/// Summary for NodeDisplay
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class NodeDisplay : public System::Windows::Forms::Form
	{
	public:
		NodeDisplay(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		NodeDisplay(int NodeId);

		delegate void AddErrFrameDelegate(Int64 time);
		delegate void AddMsgDelegate(int identifier, array<unsigned char>^ message, int dlc, 
									 int flags, Int64 time);
		delegate void AddStatusDelegate(String^ funcName, Canlib::canStatus errorCode);
		delegate void AddTextDelegate(String^ newText);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~NodeDisplay()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::RichTextBox^  MsgText;
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
			this->MsgText = (gcnew System::Windows::Forms::RichTextBox());
			this->SuspendLayout();
			// 
			// MsgText
			// 
			this->MsgText->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->MsgText->Location = System::Drawing::Point(12, 12);
			this->MsgText->Name = L"MsgText";
			this->MsgText->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::ForcedVertical;
			this->MsgText->Size = System::Drawing::Size(561, 240);
			this->MsgText->TabIndex = 0;
			this->MsgText->Text = L"";
			// 
			// NodeDisplay
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(585, 264);
			this->ControlBox = false;
			this->Controls->Add(this->MsgText);
			this->Name = L"NodeDisplay";
			this->Text = L"Receiving Node";
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
		int myId;
		System::Threading::Thread^ myThread;
		bool reqShutdown;

		void ThreadFunction();

	public:
		AddErrFrameDelegate^ errDelegate;
		AddMsgDelegate^ msgDelegate;
		AddStatusDelegate^ statusDelegate;
		AddTextDelegate^ textDelegate;
    
	public:
		void AddTextMethod(String^ newText);
		void DisplayErrorFrameText(Int64 time);
		void DisplayMessageInfo(int identifier, array<unsigned char>^ message, 
								int dlc, int flags, Int64 time);
		void DisplayStatusInfo(String^ funcName, Canlib::canStatus errorCode);
		void RequestShutdown();
		bool RequestShutdownStatus();

	};

	public ref class ReceiveNode
	{
		// Methods
		public:
			ReceiveNode(NodeDisplay^ assignedForm)
			{
				myForm = assignedForm;
			}

			void Run();

		// Fields
		private:
			NodeDisplay^ myForm;
	};

}
