// CPlusPlusEvent.cpp : main project file.

#include "stdafx.h"
#include "CPlusPlusForm.h"

using namespace CPlusPlusEvent;
using namespace canlibCLSNET;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Initialize library when the program is started.
	canlibCLSNET::Canlib::canInitializeLibrary();

	// Create the main window and run it
	Application::Run(gcnew CPlusPlusForm());
	return 0;
}
