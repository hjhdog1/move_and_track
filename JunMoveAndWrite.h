#ifndef _JUN_MOVE_AND_WRITE_
#define _JUN_MOVE_AND_WRITE_


#include "JunStreamWriter.h"



class JunMoveAndWrite
{
public:
	JunMoveAndWrite();
	void					RunUnditeredMotion(double vel, int num_repeats);
	void					RunDiteredMotion(double vel, int num_measurements, double dither_magintude, int num_dither_steps, int num_repeats);

private:
	void					RunBaseFrameMotion();
	void					MoveDriveSystemTo(double angle);
	void					MoveDriveSystemAllTo(double angle);
	void					DitherDriveSystem(double target_angle, double dither_magintude, int num_dither_steps);
	void					CheckEmergenceStop();
	static DWORD WINAPI		checking_emergency(LPVOID pData);
	

private:
	JunDriveSystem			m_drive;
	JunEMTracker			m_sensor;
	bool					m_bEmergency;
	
};



#endif  