/*
**                         Copyright 1999 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
**
** Description:
**
**
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "canlib.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    int i, chanCount;
	char name[64];

    canInitializeLibrary();
    if (canGetNumberOfChannels(&chanCount) < 0) return;
    
    for (i=0; i<chanCount; i++) {
       canGetChannelData(i, canCHANNELDATA_CHANNEL_NAME, name, sizeof(name));
       Channel1Combo->Items->Add((AnsiString)name);
       Channel2Combo->Items->Add((AnsiString)name);
    }
    Handle1 = Handle2 = canINVALID_HANDLE;
	Channel1Combo->ItemIndex = 0;
    Channel2Combo->ItemIndex = 1;
    Channel1ComboChange(Channel1Combo);
    Channel2ComboChange(Channel2Combo);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::RadioGroup1Click(TObject *Sender)
{
   TRadioGroup *g = dynamic_cast<TRadioGroup*>(Sender);
   int stat = canERR_PARAM;
   switch (g->ItemIndex) {
     case 0: stat = canSetBusParamsC200(Handle1, 0x07, 0x23);
        break;
     case 1: stat = canSetBusParamsC200(Handle1, 0x03, 0x23);
        break;
     case 2: stat = canSetBusParamsC200(Handle1, 0x01, 0x23);
        break;
   }
   if (stat < 0) {
      MessageDlg("Can't set bit rate.", mtError, TMsgDlgButtons()<<mbOK, 0);
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
   canBusOff(Handle1);
   RadioGroup1Click(RadioGroup1);
   canSetNotify(Handle1, Handle, canNOTIFY_RX);
   canBusOn(Handle1);
   StatusBar->Panels->Items[0]->Text = "Channel1: On Bus";
   Button2->Enabled = true;
   Button1->Enabled = false;
   Button5->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
   canBusOff(Handle1);
   StatusBar->Panels->Items[0]->Text = "Channel1: Off Bus";
   Button2->Enabled = false;
   Button1->Enabled = true;
   Button5->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Channel1ComboChange(TObject *Sender)
{
   canClose(Handle1);
   Handle1 = canOpenChannel(Channel1Combo->ItemIndex, canOPEN_EXCLUSIVE|canOPEN_ACCEPT_VIRTUAL);
   if (Handle1 < 0) {
      ShowMessage("Can't open channel.");
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RadioGroup2Click(TObject *Sender)
{
   TRadioGroup *g = dynamic_cast<TRadioGroup*>(Sender);
   int stat = canERR_PARAM;
   switch (g->ItemIndex) {
     case 0: stat = canSetBusParamsC200(Handle2, 0x07, 0x23);
        break;
     case 1: stat = canSetBusParamsC200(Handle2, 0x03, 0x23);
        break;
     case 2: stat = canSetBusParamsC200(Handle2, 0x01, 0x23);
        break;
   }
   if (stat < 0) {
      MessageDlg("Can't set bit rate.", mtError, TMsgDlgButtons()<<mbOK, 0);
   }
    
}
//---------------------------------------------------------------------------
void __fastcall TForm1::onBus2Click(TObject *Sender)
{
   canBusOff(Handle2);
   RadioGroup2Click(RadioGroup2);
   canSetNotify(Handle2, Handle, canNOTIFY_RX);
   canBusOn(Handle2);
   StatusBar->Panels->Items[1]->Text = "Channel2: On Bus";
   onBus2->Enabled = false;
   OffBus2->Enabled = true;
   Send2->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OffBus2Click(TObject *Sender)
{
   canBusOff(Handle2);
   StatusBar->Panels->Items[1]->Text = "Channel2: Off Bus";
   onBus2->Enabled = true;
   OffBus2->Enabled = false;
   Send2->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Channel2ComboChange(TObject *Sender)
{
   canClose(Handle2);
   Handle2 = canOpenChannel(Channel2Combo->ItemIndex, canOPEN_EXCLUSIVE|canOPEN_ACCEPT_VIRTUAL);
   if (Handle2 < 0) {
      ShowMessage("Can't open channel.");
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::WMCanLib(TWMCanLib& Message)
{
    int stat;
    long id;
    unsigned int dlc, flags;
    unsigned char data[8];
    unsigned long time;
    char s[64];

    do {
       stat = canRead(Message.hnd, &id, data, &dlc, &flags, &time);
       if(Message.hnd == Handle1)
       {
         if (stat == canOK) {
			//wsprintf(s, "Id: %d Dlc: %d ",  id, dlc);
			//ListBox1->Items->Add(s);
			TListItem *li =  ListView2->Items->Add();
			li->Caption = id;
			li->SubItems->Add(dlc);
			li->SubItems->Add(time);
         }
       }
       else
	   {
         if (stat == canOK){
			TListItem *li =  ListView1->Items->Add();
			li->Caption = id;
			li->SubItems->Add(dlc);
			li->SubItems->Add(time);
		 }
	   }
    } while (stat == canOK);
}


void __fastcall TForm1::Button5Click(TObject *Sender)
{
   canWrite(Handle1, 123, "BBBBBBBB", 8, 0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Send2Click(TObject *Sender)
{
    canWrite(Handle2, 321, "AAAAAAAA", 8, 0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button7Click(TObject *Sender)
{
ListView2->Items->Clear();	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button8Click(TObject *Sender)
{
ListView1->Items->Clear();	
}
//---------------------------------------------------------------------------

