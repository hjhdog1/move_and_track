#ifndef _JUN_DRIVE_SYSTEM_
#define _JUN_DRIVE_SYSTEM_

#include "LinkTrjPVTFile.h"



class JunDriveSystem
{
public:
	enum ANGLE_UNIT {DEGREE, RADIAN, MOTOR_INC};

public:
	JunDriveSystem();
	void					SetVelocity(double velocity);
	void					SetAngleUnit(ANGLE_UNIT angle_unit);
	void					MoveTo(double angle);
	void					MoveAllTo(double angle);
	void					Home();
	void					Dither(double target_angle, double dither_magnitude, int num_dither_steps);
	double					GetCurrentAngle(int amp_id = 4);

private:
	void					InitializeAmp();
	void					InitializeVariables();
	

private:
	// regarding angles
	ANGLE_UNIT				m_unit;
	double					m_to_inc;
	double					m_velocity_in_inc;

	double					m_inc_per_rev;

	// regarding amps
	static const int		AMPCT = 7;		// number of amps
	Linkage					link;
	Amp						amp[AMPCT];
	
};



#endif  