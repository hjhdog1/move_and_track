#include "JunMoveAndWrite.h"
#include <iostream>
#include "Utilities.h"


JunMoveAndWrite::JunMoveAndWrite()
{
	m_bEmergency = false;
	RunBaseFrameMotion();

	// emergency check thread
	CreateThread(NULL, 0, checking_emergency, this, 0, NULL);
}


void JunMoveAndWrite::RunUnditeredMotion(double vel, int num_repeats)
{
	JunStreamWriter writer(&m_drive, &m_sensor);
	m_drive.SetVelocity(vel);

	int sleeping_time = 2000;
	for(int i = 0; i < num_repeats; i++)
	{
		// initialize streams
		writer.OpenStreams("", i);
		writer.StartWriting();

		// motion
		MoveDriveSystemTo(360.0);
		::Sleep(sleeping_time);
		m_drive.Home();
		::Sleep(sleeping_time);
		MoveDriveSystemTo(360.0);
		::Sleep(sleeping_time);
		m_drive.Home();

		// close streams
		writer.StopWriting();
		writer.CloseStreams();
	}
}

void JunMoveAndWrite::RunDiteredMotion(double vel, int num_measurements, double dither_magintude, int num_dither_steps, int num_repeats)
{
	JunStreamWriter writer_CW(&m_drive, &m_sensor);
	JunStreamWriter writer_CCW(&m_drive, &m_sensor);
	
	m_drive.SetVelocity(vel);


	int recording_time = 2000;
	for(int i = 0; i < num_repeats; i++)
	{
		// initialize streams
		writer_CCW.OpenStreams("_dithered_CCW", i);
		writer_CW.OpenStreams("_dithered_CW", i);
		
		
		double step = 360.0/(double)num_measurements;
		for(int j = 0; j < num_measurements; j++)
		{
			double target_angle = step*(double)((j+1)/2);
			if(j%2 == 1)
				target_angle -= 180.0;
				//target_angle = -target_angle;



			MoveDriveSystemTo(target_angle);

			// CCW
			MoveDriveSystemTo(target_angle - 180.0);
			MoveDriveSystemTo(target_angle);
			DitherDriveSystem(target_angle, dither_magintude, num_dither_steps);
			// write
			::Sleep(2000);	// waiting for drive system to converge
			writer_CCW.StartWriting();
			::Sleep(recording_time);
			writer_CCW.StopWriting();
			
			// CW
			MoveDriveSystemTo(target_angle + 180.0);
			MoveDriveSystemTo(target_angle);
			DitherDriveSystem(target_angle, -dither_magintude, num_dither_steps);
			// write
			::Sleep(2000);	// waiting for drive system to converge
			writer_CW.StartWriting();
			::Sleep(recording_time);
			writer_CW.StopWriting();
		}
		
		// back to home
		m_drive.Home();

		// close streams
		writer_CCW.CloseStreams();
		writer_CW.CloseStreams();

	}
}

void JunMoveAndWrite::RunTrajectory(::std::string path2trajectory, ::std::string outPutFileNameTail, double dither_magintude, int num_dither_steps)
{
	m_drive.SetVelocity(180.0);

	::std::vector< ::std::string> trajectoryStr = ReadLinesFromFile(path2trajectory);

	
	JunStreamWriter writer(&m_drive, &m_sensor);
	writer.ActivateFullConfigurationRecording();
	writer.OpenStreams(outPutFileNameTail);

	::std::vector<::std::string>::iterator it = trajectoryStr.begin();
	::std::vector<double> conf;
	int sleepingTime = 2000;

	int count = 0;
	int nConf = trajectoryStr.size();
	tic();

	for(it; it < trajectoryStr.end(); ++it)
	{
		conf = DoubleVectorFromString(*it);
		DitherDriveSystem(conf, dither_magintude, num_dither_steps);
		// write
		::Sleep(2000);	// waiting for drive system to converge
		writer.StartWriting();
		::Sleep(sleepingTime);
		writer.StopWriting();

		double timeLeft = (double)(nConf-count)/(double)(++count)*toc()/60.0;	// in minutes
		::std::cout << "[" << count << " / " << nConf << "], \t time left: " << timeLeft << " mins" << ::std::endl;
	}
	writer.CloseStreams();

	// back to home
	MoveDriveSystemTo(0.0);
}

void JunMoveAndWrite::RunTrajectoryForAllApproachingDirections(::std::string path2trajectory, ::std::string outPutFileNameTail)
{
	m_drive.SetVelocity(180.0);

	::std::vector< ::std::string> trajectoryStr = ReadLinesFromFile(path2trajectory);

	
	JunStreamWriter writer(&m_drive, &m_sensor);
	writer.ActivateFullConfigurationRecording();
	writer.OpenStreams(outPutFileNameTail);

	// constants for different approaching directions
	double tempAngle_tube2[4] = {-180.0, -180.0, 180.0, 180.0};
	double tempAngle_tube3[4] = {-360.0, 180.0, -180.0, 360.0};

	::std::vector<::std::string>::iterator it = trajectoryStr.begin();
	::std::vector<double> conf, tempConf;
	int sleepingTime = 2000;

	int count = 0;
	int nConf = trajectoryStr.size();
	tic();

	for(it; it < trajectoryStr.end(); ++it)
	{
		conf = DoubleVectorFromString(*it);
		DitherDriveSystem(conf, 0.0, 0);
		
		// (tube 2, tube 3) = (CCW, CCW), (CCW, CW), (CW, CCW), (CW, CW)
		for(int i = 0 ; i < 4 ; i++)
		{
			tempConf = conf;
			tempConf[0] += tempAngle_tube2[i];
			tempConf[1] += tempAngle_tube3[i];
			DitherDriveSystem(tempConf, 0.0, 0);
			DitherDriveSystem(conf, 0.0, 0);

			// write
			::Sleep(2000);	// waiting for drive system to converge
			writer.StartWriting();
			::Sleep(sleepingTime);
			writer.StopWriting();
		}

		double timeLeft = (double)(nConf-count)/(double)(++count)*toc()/60.0;	// in minutes
		::std::cout << "[" << count << " / " << nConf << "], \t time left: " << timeLeft << " mins" << ::std::endl;
	}
	writer.CloseStreams();

	// back to home
	MoveDriveSystemTo(0.0);
}

void JunMoveAndWrite::RunTrajectoryForRandomApproachingDirections(::std::string path2trajectory, ::std::string outPutFileNameTail)
{
	m_drive.SetVelocity(180.0);

	::std::vector< ::std::string> trajectoryStr = ReadLinesFromFile(path2trajectory);

	
	JunStreamWriter writer(&m_drive, &m_sensor);
	writer.ActivateFullConfigurationRecording();
	writer.OpenStreams(outPutFileNameTail);


	::std::vector<::std::string>::iterator it = trajectoryStr.begin();
	::std::vector<double> conf, tempConf;
	int sleepingTime = 2000;

	int count = 0;
	int nConf = trajectoryStr.size();
	tic();

	for(it; it < trajectoryStr.end(); ++it)
	{
		conf = DoubleVectorFromString(*it);
		DitherDriveSystem(conf, 0.0, 0);
		
		::std::vector<double> randValues = randVector(3);
		double rand_a12 = 180.0 * (randValues[0] - 0.5);	// between -50 to 50
		double rand_a13 = 180.0 * (randValues[1] - 0.5);	// between -50 to 50
		double rand_t13 = 40.0 * (randValues[2] - 0.5);	// between -15 to 15

		tempConf = conf;
		tempConf[0] += rand_a12;
		tempConf[1] += rand_a13;
		tempConf[2] += rand_t13;
		tempConf[2] = max(min(tempConf[2], 85.0), 5.0);

		DitherDriveSystem(tempConf, 0.0, 0);
		DitherDriveSystem(conf, 0.0, 0);

		// write
		::Sleep(2000);	// waiting for drive system to converge
		writer.StartWriting();
		::Sleep(sleepingTime);
		writer.StopWriting();

		double timeLeft = (double)(nConf-count)/(double)(++count)*toc()/60.0;	// in minutes
		::std::cout << "[" << count << " / " << nConf << "], \t time left: " << timeLeft << " mins" << ::std::endl;
	}
	writer.CloseStreams();

	// back to home
	MoveDriveSystemTo(0.0);
}

void JunMoveAndWrite::RunBaseFrameMotion()
{
	JunStreamWriter writer(&m_drive, &m_sensor);
	m_drive.SetVelocity(30.0);

	// initialize streams
	writer.OpenStreams("_base");
	writer.StartWriting();

	// motion
	RotateDriveSystemAllTo(160.0);
	RotateDriveSystemAllTo(-160.0);
	m_drive.Home();

	// close streams
	writer.StopWriting();
	writer.CloseStreams();
}

void JunMoveAndWrite::MoveDriveSystemTo(double angle)
{
	CheckEmergenceStop();
	m_drive.MoveTo(angle);
	CheckEmergenceStop();
}

void JunMoveAndWrite::RotateDriveSystemAllTo(double angle)
{
	CheckEmergenceStop();
	m_drive.RotateAllTo(angle);
	CheckEmergenceStop();
}

void JunMoveAndWrite::DitherDriveSystem(double target_angle, double dither_magintude, int num_dither_steps)
{
	CheckEmergenceStop();
	m_drive.Dither(target_angle, dither_magintude, num_dither_steps);
	CheckEmergenceStop();
}

void JunMoveAndWrite::DitherDriveSystem(::std::vector<double> target_conf, double dither_magintude, int num_dither_steps)
{
	CheckEmergenceStop();
	m_drive.Dither(target_conf, dither_magintude, num_dither_steps);
	CheckEmergenceStop();
}

void JunMoveAndWrite::CheckEmergenceStop()
{
	if(!m_bEmergency)
		return;

	::std::cout << "############### EMERGENCY!!! ###############" <<::std::endl;
	m_drive.SetVelocity(100.0);
	m_drive.Home();
	::Sleep(1000);
	exit(0);
}

DWORD WINAPI JunMoveAndWrite::checking_emergency(LPVOID pData)
{
	JunMoveAndWrite* move_and_write = (JunMoveAndWrite*)pData;
	while(true)
	{
		char key = getchar();
		if(key == 's' || key == 'S')
			move_and_write->m_bEmergency = true;
	}

}