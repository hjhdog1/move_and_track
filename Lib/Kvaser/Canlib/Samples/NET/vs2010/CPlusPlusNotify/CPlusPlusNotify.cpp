// CPlusPlusNotify.cpp : main project file.

#include "stdafx.h"
#include "MainForm.h"

using namespace CPlusPlusNotify;
using namespace canlibCLSNET;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

   // Initialize the library once for the application.
   canlibCLSNET::Canlib::canInitializeLibrary();

	// Create the main window and run it
	Application::Run(gcnew MainForm());
	return 0;
}
