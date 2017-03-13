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

void clearTubePairExp();
void ThreetubeRobotTrajectoryExp();

int _tmain(int argc, _TCHAR* argv[])
{

	ThreetubeRobotTrajectoryExp();

	::Sleep(3000);
}

void clearTubePairExp()
{
	JunMoveAndWrite move_and_write;

	double vel = 120.0;
	int n_repeats = 2;
	move_and_write.RunUnditeredMotion(vel, n_repeats);

	vel = 120.0;
	int n_stops = 4;
	double dither_magnitude = 70.0;
	int n_dither_steps = 20;
	n_repeats = 1;
	move_and_write.RunDiteredMotion(vel, n_stops, dither_magnitude, n_dither_steps, n_repeats);

}

void ThreetubeRobotTrajectoryExp()
{
	JunMoveAndWrite move_and_write;

	//move_and_write.RunTrajectory("./trajectories/joints_training.txt", "_undithered_training", 0.0, 0);
	//move_and_write.RunTrajectory("./trajectories/joints_training.txt", "_dithered_training", 30.0, 20);
	//move_and_write.RunTrajectory("./trajectories/joints_validation.txt", "_undithered_validation", 0.0, 0);
	//move_and_write.RunTrajectory("./trajectories/joints_validation.txt", "_dithered_validation", 30.0, 20);

	
	move_and_write.RunTrajectory("./trajectories/joints_validation.txt", "_dithered_validation", 30.0, 20);
	move_and_write.RunTrajectory("./trajectories/joints_validation.txt", "_undithered_validation", 0.0, 0);
	move_and_write.RunTrajectory("./trajectories/joints_validation.txt", "_dithered_validation", 30.0, 20);
}