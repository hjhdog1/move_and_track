// ------------------------------------------------------------------------------- //
// Code for rotating outer tube by certain angle, wait for data measurement. 
// For Ryu and Ha's experiment
// 2014 Oct. 10
// ------------------------------------------------------------------------------- //

#include "stdafx.h"


// Comment this out to use EtherCAT
#define USE_CAN


#include <cstdio>
#include <cstdlib>;
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <windows.h>


#include "handleErrors.h"

#include <ATC3DG.h>
#include "CML.h"
#include "can_kvaser.h"
#include "LinkTrjPVTFile.h"

#include "JunDriveSystem.h"

using namespace std;

// If a namespace has been defined in CML_Settings.h, this
// macros starts using it. 
CML_NAMESPACE_USE();


/* local data */
int32 canBPS = 1000000;             // CAN network bit rate
int16 canNodeID = 1;                // CANopen node ID
#define AMPCT 7

// CKim - Global variable for handling amp variables
CML::Amp* myAmp;
bool acqFlag,stopFlag, once;
double time0;
std::ofstream fRec, fPos, fTrck[3];

double preciseInc = 10.936 * 0.998888 * 1.000049260526897;
DWORD WINAPI tracking_tracker3_Thread(LPVOID pData)
{
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD record;				
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD* pRecord = &record;	
	COORD coordinate;
	double pos[7];

	while(!stopFlag)
	{
		if(!acqFlag)	
		{
			int i = 2;
			int errCode = GetAsynchronousRecord(i, pRecord, sizeof(record));
			if(errCode!=BIRD_ERROR_SUCCESS) {		errorHandler(errCode);		}

			if(once)	{
				time0 = record.time;	once = false;
			}

			coordinate.Y=6+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
			printf("%8.3f %8.3f %8.3f %8.3f", record.s[0][0], record.s[0][1], record.s[0][2], record.x*25.4);
			fTrck[i]<<record.s[0][0]<<"\t"<<record.s[0][1]<<"\t"<<record.s[0][2]<<"\t"<<record.x*25.4<<"\n";

			coordinate.Y=7+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
			printf("%8.3f %8.3f %8.3f %8.3f", record.s[1][0], record.s[1][1], record.s[1][2], record.y*25.4);
			fTrck[i]<<record.s[1][0]<<"\t"<<record.s[1][1]<<"\t"<<record.s[1][2]<<"\t"<<record.y*25.4<<"\n";

			coordinate.Y=8+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
			printf("%8.3f %8.3f %8.3f %8.3f", record.s[2][0], record.s[2][1], record.s[2][2], record.z*25.4);
			fTrck[i]<<record.s[2][0]<<"\t"<<record.s[2][1]<<"\t"<<record.s[2][2]<<"\t"<<record.z*25.4<<"\n";

			coordinate.Y=9+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
			printf("%8.3f %8.3f %8.3f %8.3f", 0, 0, 0, 1);


		}
		::Sleep(10);
	}

	return 1;
}

DWORD WINAPI tracking_Thread(LPVOID pData)
{
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD record;				
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD* pRecord = &record;	
	COORD coordinate;
	double pos[7];

	while(!stopFlag)
	{
		if(acqFlag)	
		{
			for(int i=0; i<2; i++)
			{
				int errCode = GetAsynchronousRecord(i, pRecord, sizeof(record));
				if(errCode!=BIRD_ERROR_SUCCESS) {		errorHandler(errCode);		}

				if(once)	{
					time0 = record.time;	once = false;
				}

				coordinate.Y=6+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
				printf("%8.3f %8.3f %8.3f %8.3f", record.s[0][0], record.s[0][1], record.s[0][2], record.x*25.4);
				fTrck[i]<<record.s[0][0]<<"\t"<<record.s[0][1]<<"\t"<<record.s[0][2]<<"\t"<<record.x*25.4<<"\n";

				coordinate.Y=7+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
				printf("%8.3f %8.3f %8.3f %8.3f", record.s[1][0], record.s[1][1], record.s[1][2], record.y*25.4);
				fTrck[i]<<record.s[1][0]<<"\t"<<record.s[1][1]<<"\t"<<record.s[1][2]<<"\t"<<record.y*25.4<<"\n";

				coordinate.Y=8+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
				printf("%8.3f %8.3f %8.3f %8.3f", record.s[2][0], record.s[2][1], record.s[2][2], record.z*25.4);
				fTrck[i]<<record.s[2][0]<<"\t"<<record.s[2][1]<<"\t"<<record.s[2][2]<<"\t"<<record.z*25.4<<"\n";

				coordinate.Y=9+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
				printf("%8.3f %8.3f %8.3f %8.3f", 0, 0, 0, 1);
			}

			for(int i=0; i<7; i++)	{	myAmp[i].GetPositionActual(pos[i]);		}

			coordinate.Y=16;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
			printf("%8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f", pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6]);
			fPos<<(record.time - time0)<<"\t"<<pos[3]<<"\t"<<pos[4]<<std::endl;
		}
		//::Sleep(10);
	}

	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	JunDriveSystem drive;
	drive.MoveTo(100.0);
	drive.Home();
}


//
//int _tmain(int argc, _TCHAR* argv[])
//{
//
//	// ---------------------------------------------------------------- //
//	// CKim - Initializing motor controller.  The robot uses 
//	// 'Copley Motion Libraries'  from Copley Controls. 
//	// The libraries define one global object of type 'CopleyMotionLibraries' named cml.
//	// ---------------------------------------------------------------- //
//
//	// CKim - Generate log file for debugging
//	cml.SetDebugLevel( LOG_EVERYTHING );
//
//   // Create an 'CanInterface' object used to access the low level CAN network.
//	#if defined( USE_CAN )
//	   KvaserCAN hw("CAN");
//	   hw.SetBaud( canBPS );
//	#elif defined( WIN32 )
//	   WinUdpEcatHardware hw( "eth0" );
//	#else
//	   LinuxEcatHardware hw( "eth0" );
//	#endif
//
//	// Open the 'CANopen' network object
//	#if defined( USE_CAN )
//	   CanOpen net;
//	#else
//	 EtherCAT net;
//	#endif
//	const Error *err = net.Open( hw );
//	showerr( err, "Opening network" );
//   
//	// Initialize the amplifier using default settings
//	Amp amp[AMPCT];
//	AmpSettings set;
//	set.guardTime = 0;
//	printf( "Doing init\n" );
//	
//	for(int i=0; i<AMPCT; i++ )
//	{
//		err = amp[i].Init( net, 1+i, set );
//		showerr( err, "Initting amp" );
//
//		// CKim - Get the motor info stored in the memory of the amp. How do you set it?
//		MtrInfo mtrInfo;
//		err = amp[i].GetMtrInfo( mtrInfo );
//		showerr( err, "Getting motor info\n" );
//
//		err = amp[i].SetCountsPerUnit( mtrInfo.ctsPerRev );
//		showerr( err, "Setting cpr\n" );
//
//		// Zeros the amps actual positions
//		err= amp[i].SetPositionActual(0);
//		showerr( err, "Setting position_Zero\n" );
//	}
// 
//	// CKim - Set global pointer 
//	myAmp = amp;	acqFlag = false;	stopFlag = false;	once = true;
//	fPos.open("./JAng.txt");
//	fTrck[0].open("./Trck1.txt");
//	fTrck[1].open("./Trck2.txt");
//	fTrck[2].open("./Trck3.txt");
//
//	// CKim - 'Linkage' object allows coordinated motion between the motors. 
//	// Create a linkage object holding these amps. 
//	Linkage link;
//	err = link.Init( AMPCT, amp );
//	showerr( err, "Linkage init" );
//   
//	// Setup the velocity, acceleration, deceleration & jerk limits
//	// for multi-axis moves using the linkage object
//	err = link.SetMoveLimits( 20, 20, 20, 20 );
//	showerr( err, "setting move limits" );
//
//	
//	// ---------------------------------------------------------------- //
//	// CKim - Set up the Ascension EM tracker.
//	// Related include and library files are 'ATC3DG.h' 
//	// ---------------------------------------------------------------- //
//	printf( "Initializing tracker wait please...\n" );
//   	SYSTEM_CONFIGURATION	trackerConfig;		SENSOR_CONFIGURATION*	sensorConfig;		
//	TRANSMITTER_CONFIGURATION	xmtrConfig;		int trackerError;		int sensorNum = 0;		short xmtrId = 0;
//	DATA_FORMAT_TYPE type = DOUBLE_POSITION_MATRIX_TIME_Q;	
//	
//	// CKim - Initialize the tracker and get system configuration
//	trackerError = InitializeBIRDSystem();
//	if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
//	
//	trackerError = GetBIRDSystemConfiguration(&trackerConfig);
//	if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
//
//	// CKim - Configure the sensors. Get the number of connected sensors. Set the data format for sensor reading
//	sensorConfig = new SENSOR_CONFIGURATION[trackerConfig.numberSensors];
//	for(int i=0; i<trackerConfig.numberSensors; i++)
//	{
//		trackerError = GetSensorConfiguration(i, &(sensorConfig[i]));
//		if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
//		if(sensorConfig[i].attached)
//		{
//			trackerError = SetSensorParameter(i,DATA_FORMAT,&type,sizeof(type));  
//			if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
//			sensorNum++;						
//		}
//	}
//
//	printf("Number of sensors connected : %d\n",sensorNum);
//	printf("Press any key to stat tracking");
//	std::getchar();
//
//	// CKim - Configure the transmitter and turn it on. xmtrId must be type short!!
//	trackerError = GetTransmitterConfiguration(0,&xmtrConfig);
//	if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
//
//	trackerError = SetSystemParameter(SELECT_TRANSMITTER, &xmtrId, sizeof(xmtrId));
//	if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
//
//	// JHa- Start thread for reading EM tracker 3 for computing robot base frame & rotate base 1 and 2 together. 
//	if(sensorNum == 3){
//		HANDLE hThread_tracker3 = CreateThread(NULL, 0, tracking_tracker3_Thread, 0, 0, NULL);
//		
//		// declare local pos vaiable.
//		Point<AMPCT> pos;		
//		pos[0] = 0.0;	pos[1] = 0.0;	pos[2] = 0.0;	pos[3] = 0.0;	pos[4] = 0.0;	pos[5] = 0.0;	pos[6] = 0.0;
//
//		// rotates bases with local pos & vel variables.
//		double inc = 10.936;		double vel = 10.936/12.0;
//
//		pos[3] = pos[4] = inc/3;	// minus = CCW, plus = CW
//		err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//		showerr( err, "Moving linkage" );
//		err = link.WaitMoveDone( 1000 * 1000 );
//		showerr( err, "waiting on linkage done" );
//
//		pos[3] = pos[4] = -inc/3;	// minus = CCW, plus = CW
//		err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//		showerr( err, "Moving linkage" );
//		err = link.WaitMoveDone( 1000 * 1000 );
//		showerr( err, "waiting on linkage done" );
//
//		pos[3] = pos[4] = 0;	// minus = CCW, plus = CW
//		err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//		showerr( err, "Moving linkage" );
//		err = link.WaitMoveDone( 1000 * 1000 );
//		showerr( err, "waiting on linkage done" );
//
//		// terminating thread for tracker 3
//		DWORD dwExit;
//		GetExitCodeThread(hThread_tracker3, &dwExit);
//        TerminateThread(hThread_tracker3, dwExit);	//ExitThread(dwExit)
//        CloseHandle(hThread_tracker3);
//	}
//	fTrck[2].flush();
//	fTrck[2].close();
//
//	
//	// CKim - Start thread for reading data
//	acqFlag = true;
//	HANDLE hThread = CreateThread(NULL, 0, tracking_Thread, 0, 0, NULL);
//	
//	// ---------------------------------------------------------------- //
//	// CKim - Start moving - for Jun's experiment
//	// ---------------------------------------------------------------- //
//	// CKim - Set the position. Axis are mapped as follows.
//	// 0 : balance_pair_Translation (Tube 1 and 2)
//	// 1 : Inner_translation (Tube 3) + pushes tube forward
//	// 2 : Forceps_Actuation (Additional tool, currently disconnected)
//	// 3 : balance_pair_Outer_Rotation
//	// 4 : balance_pair_Innr_Rotation
//	// 5 : Inner_Rotation (Tube 3)
//	// 6 : Forceps_Rotation (Additional tool, currently disconnected)
//	// rotary 10.936 equals to one turn, 1 translation 3.175mm; (3.175 mm / motor command 1)
//	
//	// CKim - Move robot to each positions. 'MoveTo' function will cause the linkage object to create a 
//	// multi-axis S-curve move to the new position. This trajectory will be passed down to the amplifiers using
//	// the PVT trajectory mode
//
//	Point<AMPCT> pos;		
//	pos[0] = 0.0;	pos[1] = 0.0;	pos[2] = 0.0;	pos[3] = 0.0;	pos[4] = 0.0;	pos[5] = 0.0;	pos[6] = 0.0;
//	
//
//
//	//// ---------------------------------------------------------------- //
//	//// JHa - base1 or 2 rotations - base 1 was rotated in optimal tube pair experiment. 
//	//double inc = 10.936;		double vel = 10.936/5.0;		acqFlag = true;
//	//int sleepTime = 2000;		int nPreRotation = 10;
//
//	////printf("Press any key to start.");
//	////std::getchar();
//
//	//for(int i = 0 ; i < 100; i++)
//	//{
//	//	vel = 10.936/5.0;
//
//	//	pos[4] = -(double)nPreRotation*inc;	// minus = CCW, plus = CW
//	//	err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//	//	showerr( err, "Moving linkage" );
//	//	err = link.WaitMoveDone( 1000 * 1000 );
//	//	showerr( err, "waiting on linkage done" );
//	//	
//	//	Sleep(sleepTime);
//	//
//	//	pos[4] = 0.0;	// minus = CCW, plus = CW
//	//	err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//	//	showerr( err, "Moving linkage" );
//	//	err = link.WaitMoveDone( 1000 * 1000 );
//	//	showerr( err, "waiting on linkage done" );
//	//
//	//	Sleep(sleepTime);
//
//	//	//for(int j = 0; j < 10; j++)
//	//	//{
//	//	//	pos[4] = pow(-1.0,(double)j) * 0.1*(double)(9-j)/(double)9*inc;	// minus = CCW, plus = CW
//	//	//	err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//	//	//	showerr( err, "Moving linkage" );
//	//	//	err = link.WaitMoveDone( 1000 * 100 );
//	//	//	showerr( err, "waiting on linkage done" );
//	//	//}
//
//	//	
//	//	vel = 10.936/10.0;
//
//	//	pos[4] = -2.0*inc;	// minus = CCW, plus = CW
//	//	err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//	//	showerr( err, "Moving linkage" );
//	//	err = link.WaitMoveDone( 1000 * 1000 );
//	//	showerr( err, "waiting on linkage done" );
//	//	
//	//	Sleep(sleepTime);
//	//
//	//	pos[4] += 2.0*inc;	// minus = CCW, plus = CW
//	//	err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//	//	showerr( err, "Moving linkage" );
//	//	err = link.WaitMoveDone( 1000 * 1000 );
//	//	showerr( err, "waiting on linkage done" );
//
//	//	//Sleep(sleepTime);
//
//	//	//pos[4] = -1.0*inc;	// minus = CCW, plus = CW
//	//	//err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//	//	//showerr( err, "Moving linkage" );
//	//	//err = link.WaitMoveDone( 1000 * 1000 );
//	//	//showerr( err, "waiting on linkage done" );
//	//	//
//	//	//Sleep(sleepTime);
//	//
//	//	//pos[4] += 1.0*inc;	// minus = CCW, plus = CW
//	//	//err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//	//	//showerr( err, "Moving linkage" );
//	//	//err = link.WaitMoveDone( 1000 * 100 );
//	//	//showerr( err, "waiting on linkage done" );
//	//	
//	//	acqFlag = false;
//	//	fPos.flush();
//	//	fTrck[0].flush();
//	//	fTrck[1].flush();
//
//	//	fPos.close();
//	//	fTrck[0].close();
//	//	fTrck[1].close();
//
//	//	printf("Press any key to do it agian.");
//	//	std::getchar();
//	//			
//	//	std::string fTrckName[2];
//	//	std::string fPosName = "./JAng_";
//	//	fTrckName[0] = "./Trck1_";
//	//	fTrckName[1] = "./Trck2_";
//	//	fPosName.append(std::to_string(i+1));
//	//	fTrckName[0].append(std::to_string(i+1));
//	//	fTrckName[1].append(std::to_string(i+1));
//	//	fPosName.append(".txt");
//	//	fTrckName[0].append(".txt");
//	//	fTrckName[1].append(".txt");
//
//	//	fPos.open(fPosName);
//	//	fTrck[0].open(fTrckName[0]);
//	//	fTrck[1].open(fTrckName[1]);
//
//	//	acqFlag = true;
//	//	
//	//}
//	//// ---------------------------------------------------------------- //
//
//
//	// ---------------------------------------------------------------- //
//	// JHa - base1 or 2 rotations - base 1 was rotated in optimal tube pair experiment. 
//	// EM tracker is assumed to be fixed.
//	double inc = 10.936;		double vel = 10.936/10.0;		acqFlag = true;
//	int sleepTime = 2000;		int nPreRotation = 10;
//
//	//printf("Press any key to start.");
//	//std::getchar();
//
//	for(int i = 0 ; i < 100; i++)
//	{
//		vel = 10.936/10.0;
//		for(int j = 0 ; j < nPreRotation; j++)
//		{
//			pos[4] = -inc;	// minus = CCW, plus = CW
//			err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//			showerr( err, "Moving linkage" );
//			err = link.WaitMoveDone( 1000 * 1000 );
//			showerr( err, "waiting on linkage done" );
//		
//			Sleep(sleepTime);
//	
//			pos[4] = 0.0;	// minus = CCW, plus = CW
//			err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//			showerr( err, "Moving linkage" );
//			err = link.WaitMoveDone( 1000 * 1000 );
//			showerr( err, "waiting on linkage done" );
//	
//			Sleep(sleepTime);
//		}
//		
//
//		vel = 10.936/72.0;
//		for(int j = 0 ; j < 2; j++)
//		{
//			pos[4] = -0.5*inc;	// minus = CCW, plus = CW
//			err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//			showerr( err, "Moving linkage" );
//			err = link.WaitMoveDone( 1000 * 1000 );
//			showerr( err, "waiting on linkage done" );
//		
//			Sleep(sleepTime);
//	
//			pos[4] = -inc;	// minus = CCW, plus = CW
//			err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//			showerr( err, "Moving linkage" );
//			err = link.WaitMoveDone( 1000 * 1000 );
//			showerr( err, "waiting on linkage done" );
//
//			Sleep(sleepTime);
//
//			pos[4] = -0.5*inc;	// minus = CCW, plus = CW
//			err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//			showerr( err, "Moving linkage" );
//			err = link.WaitMoveDone( 1000 * 1000 );
//			showerr( err, "waiting on linkage done" );
//		
//			Sleep(sleepTime);
//	
//			pos[4] = 0.0;	// minus = CCW, plus = CW
//			err = link.MoveTo(pos,vel,10.0*vel,10.0*vel,100.0*vel);
//			showerr( err, "Moving linkage" );
//			err = link.WaitMoveDone( 1000 * 1000 );
//			showerr( err, "waiting on linkage done" );
//
//			Sleep(sleepTime);
//		}
//		
//		acqFlag = false;
//		fPos.flush();
//		fTrck[0].flush();
//		fTrck[1].flush();
//
//		fPos.close();
//		fTrck[0].close();
//		fTrck[1].close();
//
//		printf("Press any key to do it agian.");
//		std::getchar();
//				
//		std::string fTrckName[2];
//		std::string fPosName = "./JAng_";
//		fTrckName[0] = "./Trck1_";
//		fTrckName[1] = "./Trck2_";
//		fPosName.append(std::to_string(i+1));
//		fTrckName[0].append(std::to_string(i+1));
//		fTrckName[1].append(std::to_string(i+1));
//		fPosName.append(".txt");
//		fTrckName[0].append(".txt");
//		fTrckName[1].append(".txt");
//
//		fPos.open(fPosName);
//		fTrck[0].open(fTrckName[0]);
//		fTrck[1].open(fTrckName[1]);
//
//		acqFlag = true;
//		
//	}
//	// ---------------------------------------------------------------- //
//
//	stopFlag = true;
//	WaitForSingleObject(hThread,5000);
//
//
//	fPos.close();
//	fTrck[0].close();
//	fTrck[1].close();
//}


