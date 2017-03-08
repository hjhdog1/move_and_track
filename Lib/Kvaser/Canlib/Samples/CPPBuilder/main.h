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
#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include <canlib.h>
#include <ComCtrls.hpp>

struct TWMCanLib
{
    Cardinal Msg;
    DWORD hnd;
    WORD event;
    WORD _pad;
    Longint Result;
};


class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TComboBox *Channel1Combo;
    TGroupBox *GroupBox2;
    TComboBox *Channel2Combo;
    TRadioGroup *RadioGroup1;
    TRadioGroup *RadioGroup2;
	TStatusBar *StatusBar;
	TPanel *Panel1;
	TButton *Button1;
	TButton *Button2;
	TButton *Button5;
	TPanel *Panel2;
	TButton *onBus2;
	TButton *OffBus2;
	TButton *Send2;
	TListView *ListView1;
	TListView *ListView2;
	TButton *Button7;
	TButton *Button8;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall RadioGroup1Click(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall Channel1ComboChange(TObject *Sender);
    void __fastcall RadioGroup2Click(TObject *Sender);
    void __fastcall onBus2Click(TObject *Sender);
    void __fastcall OffBus2Click(TObject *Sender);
    void __fastcall Channel2ComboChange(TObject *Sender);
    void __fastcall Button5Click(TObject *Sender);
    void __fastcall Send2Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
private:	// User declarations
    DWORD Handle1, Handle2;
protected:
    void __fastcall WMCanLib(TWMCanLib& Message);
    BEGIN_MESSAGE_MAP
       MESSAGE_HANDLER(WM__CANLIB, TWMCanLib, WMCanLib);
    END_MESSAGE_MAP(TForm)
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
