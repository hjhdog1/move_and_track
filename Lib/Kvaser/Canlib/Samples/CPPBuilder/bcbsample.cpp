//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("bcbsample.res");
USEFORM("main.cpp", Form1);
USELIB("..\..\LIB\Borland\canlib32.lib");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TForm1), &Form1);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
