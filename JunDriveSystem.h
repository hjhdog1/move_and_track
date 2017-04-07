#ifndef _JUN_DRIVE_SYSTEM_
#define _JUN_DRIVE_SYSTEM_

#include "LinkTrjPVTFile.h"
#include <vector>
#include <fstream>



class JunDriveSystem
{
public:
	enum ANGLE_UNIT {DEGREE, RADIAN, MOTOR_INC};

public:
	JunDriveSystem();
	~JunDriveSystem();
	void					SetVelocity(double velocity);
	void					SetAngleUnit(ANGLE_UNIT angle_unit);
	void					MoveTo(double angle);
	void					MoveTo(::std::vector<double> conf, double rigidbodyRot = 0.0);	// move to (rotation1, rotation2, translation3)
	void					RotateAllTo(double angle);	// rotate outer two tubes
	void					Home();
	void					Dither(double target_angle, double dither_magnitude, int num_dither_steps);
	void					Dither(::std::vector<double> target_conf, double dither_magnitude, int num_dither_steps);
	double					GetCurrentAngle(int amp_id = 4);
	::std::vector<double>	JunDriveSystem::GetCurrentConfiguration();

	// test function
	void					RigidBodyTranslation(double trans);

	double					getFullTranslation() const {return m_innerTubeFullExtensionLength;}

private:
	void					InitializeAmp();
	void					InitializeVariables();
	void					recordJoint(const Point<7>& pos, bool isAfterMove);
	

private:
	// regarding angles
	ANGLE_UNIT				m_unit;
	double					m_angle_to_inc, m_mm_to_inc;
	double					m_velocity_in_inc;

	double					m_inc_per_rev;

	// regarding 3-tube robot
	double					m_innerTubeFullExtensionLength;	// in mm

	// regarding amps
	static const int		AMPCT = 7;		// number of amps
	Linkage					link;
	Amp						amp[AMPCT];

	// for crash dump
	::std::ofstream			jointHistory;
	
};



#endif  