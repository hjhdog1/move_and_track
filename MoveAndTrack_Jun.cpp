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
void ThreetubeRobotDitheringTest();




int _tmain(int argc, _TCHAR* argv[])
{
	ThreetubeRobotDitheringTest();
	//ThreetubeRobotTrajectoryExp();

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

	double ditherMagnitude = 20.0;
	int ditherSteps = 10;

	move_and_write.RunTrajectory("./trajectories/joints_training_EM.txt", "_undithered_training", 0.0, 0);
	move_and_write.RunTrajectory("./trajectories/joints_training_EM.txt", "_dithered_training", ditherMagnitude, ditherSteps);
	move_and_write.RunTrajectory("./trajectories/joints_validation_EM.txt", "_undithered_validation", 0.0, 0);
	move_and_write.RunTrajectory("./trajectories/joints_validation_EM.txt", "_dithered_validation", ditherMagnitude, ditherSteps);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	::Sleep(300000);

	ditherMagnitude = 30.0;
	ditherSteps = 20;

	move_and_write.RunTrajectory("./trajectories/joints_training_EM.txt", "_dithered_training_2", ditherMagnitude, ditherSteps);
	move_and_write.RunTrajectory("./trajectories/joints_validation_EM.txt", "_dithered_validation_2", ditherMagnitude, ditherSteps);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	::Sleep(300000);

	ditherMagnitude = 50.0;
	ditherSteps = 30;

	move_and_write.RunTrajectory("./trajectories/joints_training_EM.txt", "_dithered_training_3", ditherMagnitude, ditherSteps);
	move_and_write.RunTrajectory("./trajectories/joints_validation_EM.txt", "_dithered_validation_3", ditherMagnitude, ditherSteps);



	//move_and_write.RunTrajectory("./trajectories/joints_training_EM.txt", "_dithered_training", ditherMagnitude, ditherSteps);
	//move_and_write.RunTrajectory("./trajectories/joints_training_EM.txt", "_undithered_training", 0.0, 0);
	//move_and_write.RunTrajectory("./trajectories/joints_training_EM.txt", "_dithered_training2", ditherMagnitude, ditherSteps);
}

void ThreetubeRobotDitheringTest()
{
	JunMoveAndWrite move_and_write;

	double ditherMagnitude = 20.0;
	int ditherSteps = 10;

	move_and_write.RunTrajectory("./trajectories/joints_test_undithered.txt", "_test_undithered", 0.0, 0);
/*
	move_and_write.RunTrajectory("./trajectories/joints_test_dithered.txt", "_test_dithered20", ditherMagnitude, ditherSteps);
	move_and_write.RunTrajectory("./trajectories/joints_test_dithered.txt", "_test_dithered40", 2*ditherMagnitude, 2*ditherSteps);
	move_and_write.RunTrajectory("./trajectories/joints_test_dithered.txt", "_test_dithered60", 3*ditherMagnitude, 3*ditherSteps);*/
}