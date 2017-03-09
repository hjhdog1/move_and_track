// ------------------------------------------------------------------------------- //
// Code for rotating outer tube by certain angle, wait for data measurement. 
// For Ryu and Ha's experiment
// 2014 Oct. 10
// ------------------------------------------------------------------------------- //

#include "stdafx.h"


// Comment this out to use EtherCAT
#define USE_CAN


#include <windows.h>


#include "handleErrors.h"

#include "JunDriveSystem.h"
#include "JunEMTracker.h"
#include "JunStreamWriter.h"
#include "JunMoveAndWrite.h"

using namespace std;

// If a namespace has been defined in CML_Settings.h, this
// macros starts using it. 
CML_NAMESPACE_USE();


int _tmain(int argc, _TCHAR* argv[])
{
	JunMoveAndWrite move_and_write;

	double vel = 10.0;
	int n_repeats = 4;
	move_and_write.RunUnditeredMotion(vel, n_repeats);

	vel = 120.0;
	int n_stops = 4;
	double dither_magnitude = 50.0;
	int n_dither_steps = 20;
	n_repeats = 1;
	move_and_write.RunDiteredMotion(vel, n_stops, dither_magnitude, n_dither_steps, n_repeats);



	::Sleep(3000);
}
