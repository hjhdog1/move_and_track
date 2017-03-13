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


	int sleeping_time = 2000;
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
				target_angle = -target_angle;


			MoveDriveSystemTo(target_angle);

			// CCW
			MoveDriveSystemTo(target_angle - 180.0);
			MoveDriveSystemTo(target_angle);
			DitherDriveSystem(target_angle, dither_magintude, num_dither_steps);
			// write
			writer_CCW.StartWriting();
			::Sleep(sleeping_time);
			writer_CCW.StopWriting();
			
			// CW
			MoveDriveSystemTo(target_angle + 180.0);
			MoveDriveSystemTo(target_angle);
			DitherDriveSystem(target_angle, -dither_magintude, num_dither_steps);
			// write
			writer_CW.StartWriting();
			::Sleep(sleeping_time);
			writer_CW.StopWriting();
		}
		
		// back to home
		m_drive.Home();

		// close streams
		writer_CCW.CloseStreams();
		writer_CW.CloseStreams();

	}
}

void JunMoveAndWrite::RunTrajectory(::std::string path2trajectory, double dither_magintude, int num_dither_steps)
{
	::std::vector< ::std::string> trajectoryStr = ReadLinesFromFile(path2trajectory);

	
	JunStreamWriter writer(&m_drive, &m_sensor);
	writer.ActivateFullConfigurationRecording();
	writer.OpenStreams("_test");

	::std::vector<::std::string>::iterator it = trajectoryStr.begin();
	::std::vector<double> conf;
	for(it; it < trajectoryStr.end(); ++it)
	{
		conf = DoubleVectorFromString(*it);
		DitherDriveSystem(conf, dither_magintude, num_dither_steps);
		writer.StartWriting();
		::Sleep(2000);
		writer.StopWriting();
	}
	writer.CloseStreams();
}


void JunMoveAndWrite::RunBaseFrameMotion()
{
	JunStreamWriter writer(&m_drive, &m_sensor);
	m_drive.SetVelocity(30.0);

	// initialize streams
	writer.OpenStreams("");
	writer.StartWriting();

	// motion
	RotateDriveSystemAllTo(120.0);
	RotateDriveSystemAllTo(-120.0);
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