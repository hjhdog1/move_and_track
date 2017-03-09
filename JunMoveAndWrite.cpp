#include "JunMoveAndWrite.h"


JunMoveAndWrite::JunMoveAndWrite()
{
	RunBaseFrameMotion();
}


void JunMoveAndWrite::RunUnditeredMotion(double vel, int num_repeats)
{
	JunStreamWriter writer(&m_drive, &m_sensor);
	m_drive.SetVelocity(vel);

	for(int i = 0; i < num_repeats; i++)
	{
		//// CCW motion
		// initialize streams
		writer.OpenStreams("_CCW", i);
		writer.StartWriting();

		// motion
		m_drive.MoveTo(360.0);

		// close streams
		writer.StopWriting();
		writer.CloseStreams();

		
		//// CW motion
		// initialize streams
		writer.OpenStreams("_CW", i);
		writer.StartWriting();

		// motion
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
		writer_CCW.OpenStreams("_CCW", i);
		writer_CW.OpenStreams("_CW", i);
		
		double step = 360.0/(double)num_measurements;
		for(int j = 0; j < num_measurements; j++)
		{
			double target_angle = step*(double)((j+1)/2);
			if(j%2 == 1)
				target_angle = -target_angle;


			m_drive.MoveTo(target_angle);

			// CCW
			m_drive.MoveTo(target_angle - 180.0);
			m_drive.MoveTo(target_angle + 180.0);
			m_drive.Dither(target_angle, dither_magintude, num_dither_steps);
			// write
			writer_CCW.StartWriting();
			::Sleep(sleeping_time);
			writer_CCW.StopWriting();
			
			// CW
			m_drive.MoveTo(target_angle + 180.0);
			m_drive.MoveTo(target_angle - 180.0);
			m_drive.Dither(target_angle, -dither_magintude, num_dither_steps);
			// write
			writer_CW.StartWriting();
			::Sleep(sleeping_time);
			writer_CW.StopWriting();
		}
		
		// back to home
		m_drive.Home();

		// close streams
		writer_CCW.StopWriting();
		writer_CCW.CloseStreams();
		
		writer_CW.StopWriting();
		writer_CW.CloseStreams();

	}
}

void JunMoveAndWrite::RunBaseFrameMotion()
{
	JunStreamWriter writer(&m_drive, &m_sensor);
	m_drive.SetVelocity(30.0);

	// initialize streams
	writer.OpenStreams("");
	writer.StartWriting();

	// motion
	m_drive.MoveTo(120.0);
	m_drive.MoveTo(-120.0);
	m_drive.Home();

	// close streams
	writer.StopWriting();
	writer.CloseStreams();
}