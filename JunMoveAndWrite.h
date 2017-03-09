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

private:
	JunDriveSystem			m_drive;
	JunEMTracker			m_sensor;
	
};



#endif  