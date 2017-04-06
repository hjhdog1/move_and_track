// ------------------------------------------------------------------------------- //
// Code for rotating outer tube by certain angle, wait for data measurement. 
// For Ryu and Ha's experiment
// 2014 Oct. 10
// ------------------------------------------------------------------------------- //

#include "stdafx.h"


// Comment this out to use EtherCAT
#define USE_CAN


#include <windows.h>
#include <iostream>


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
void takePictures();
void ThreetubeRobotTrajectoryExp();
void ThreetubeTest();




int _tmain(int argc, _TCHAR* argv[])
{
	//takePictures();

	clearTubePairExp();

	//ThreetubeTest();
	//ThreetubeRobotTrajectoryExp();

	//JunDriveSystem drive;
	//drive.RigidBodyTranslation(-20);
	//drive.Home();

	//drive.RotateAllTo(90.0);

	//::Sleep(30000);
	//drive.Home();

	::Sleep(3000);
}

void clearTubePairExp()
{
	JunMoveAndWrite move_and_write;

	double vel = 10.0;
	int n_repeats = 1;
	move_and_write.RunUnditeredMotion(vel, n_repeats);

	vel = 120.0;
	int n_stops = 36;
	double dither_magnitude = 40.0;
	int n_dither_steps = 20;
	n_repeats = 1;
	move_and_write.RunDiteredMotion(vel, n_stops, dither_magnitude, n_dither_steps, n_repeats);

}

void takePictures()
{
	JunDriveSystem drive;

	double vel = 120.0;
	double dither_magnitude = 40.0;
	int n_dither_steps = 20;

	drive.SetVelocity(vel);

	
	::std::cout << "Press enter to go 180 deg" << ::std::endl;
	getchar();
	drive.Dither(180.0, dither_magnitude, n_dither_steps);

	::std::cout << "Press enter to go 0 deg" << ::std::endl;
	getchar();
	drive.Dither(0.0, dither_magnitude, n_dither_steps);

	drive.Home();
}

void ThreetubeRobotTrajectoryExp()
{

	JunMoveAndWrite move_and_write;
	
	double ditherMagnitude = 40.0;
	int ditherSteps = 20;

	// training sets
	//move_and_write.RunTrajectory("./trajectories/joints512.txt", "_dithered_training", ditherMagnitude, ditherSteps);
	//move_and_write.RunTrajectoryForAllApproachingDirections("./trajectories/joints512.txt", "_undithered_training_alldirections");

	// validation sets
	//move_and_write.RunTrajectory("./trajectories/joints_rand500.txt", "_dithered_validation", ditherMagnitude, ditherSteps);
	move_and_write.RunTrajectory("./trajectories/joints_rand500.txt", "_undithered_validation", 0.0, 0);
	move_and_write.RunTrajectoryForRandomApproachingDirections("./trajectories/joints_rand500.txt", "_undithered_validation_rand");
	
}

void ThreetubeTest()
{

	JunMoveAndWrite move_and_write;
	
	double ditherMagnitude = 40.0;
	int ditherSteps = 20;

	// training sets
	move_and_write.RunTrajectory("./trajectories/joint_temp.txt", "_temp0", 0.0, 0);
	//move_and_write.RunTrajectory("./trajectories/joints_test_dithered.txt", "_temp1", ditherMagnitude, ditherSteps);
	//move_and_write.RunTrajectoryForAllApproachingDirections("./trajectories/joints_test_dithered.txt", "_temp2");
	//move_and_write.RunTrajectoryForRandomApproachingDirections("./trajectories/joints_test_dithered.txt", "_temp3");
	

	//move_and_write.RunTrajectoryForAllApproachingDirections("./trajectories/joints_test_dithered.txt", "_test_undithered");
	//move_and_write.RunTrajectory("./trajectories/joints_test_undithered.txt", "_test_undithered", 0.0, 0);
	//move_and_write.RunTrajectory("./trajectories/joints_test_dithered.txt", "_test_dithered20", ditherMagnitude, ditherSteps);
	//move_and_write.RunTrajectory("./trajectories/joints_test_dithered.txt", "_test_dithered40", 2*ditherMagnitude, 2*ditherSteps);
	//move_and_write.RunTrajectory("./trajectories/joints_test_dithered.txt", "_test_dithered60", 3*ditherMagnitude, 3*ditherSteps);
}