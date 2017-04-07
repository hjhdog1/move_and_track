#include "JunDriveSystem.h"

#include "handleErrors.h"

#include "stdafx.h"
#include "can_kvaser.h"

// Comment this out to use EtherCAT
#define USE_CAN



// If a namespace has been defined in CML_Settings.h, this
// macros starts using it. 
CML_NAMESPACE_USE();

// ---------------------------------------------------------------- //
// CKim - Set the position. Axis are mapped as follows.
// 0 : balance_pair_Translation (Tube 1 and 2)
// 1 : Inner_translation (Tube 3) + pushes tube forward
// 2 : Forceps_Actuation (Additional tool, currently disconnected)
// 3 : balance_pair_Outer_Rotation
// 4 : balance_pair_Innr_Rotation
// 5 : Inner_Rotation (Tube 3)
// 6 : Forceps_Rotation (Additional tool, currently disconnected)
// rotary 10.936 equals to one turn, 1 translation 3.175mm; (3.175 mm / motor command 1)


const double JUN_PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342;

const double moveToGains[3] = {10.0, 10.0, 100.0};

JunDriveSystem::JunDriveSystem()
{
	InitializeAmp();
	InitializeVariables();
}

JunDriveSystem::~JunDriveSystem()
{
	jointHistory.flush();
	jointHistory.close();
}

void JunDriveSystem::SetVelocity(double velocity)
{
	m_velocity_in_inc = ::std::abs(velocity * m_angle_to_inc);
}

void JunDriveSystem::SetAngleUnit(ANGLE_UNIT angle_unit)
{
	m_unit = angle_unit;

	if (angle_unit == DEGREE)
		m_angle_to_inc = m_inc_per_rev / 360.0;
	else if (angle_unit == RADIAN)
		m_angle_to_inc = m_inc_per_rev/ (2*JUN_PI);
	else if (angle_unit == MOTOR_INC)
		m_angle_to_inc = m_inc_per_rev;
}

void JunDriveSystem::MoveTo(double angle)
{
	const Error *err = NULL;

	Point<AMPCT> pos;		
	pos[0] = 0.0;	pos[1] = 0.0;	pos[2] = 0.0;	pos[3] = 0.0;	pos[4] = 0.0;	pos[5] = 0.0;	pos[6] = 0.0;

	pos[4] = angle * m_angle_to_inc;

	// record MoveTo history
	recordJoint(pos, false);

	err = link.MoveTo(pos, m_velocity_in_inc, moveToGains[0]*m_velocity_in_inc, moveToGains[1]*m_velocity_in_inc, moveToGains[2]*m_velocity_in_inc);
	showerr( err, "Moving linkage" );
	err = link.WaitMoveDone();
	showerr( err, "waiting on linkage done" );

	// record MoveTo history
	recordJoint(pos, true);
}

void JunDriveSystem::MoveTo(::std::vector<double> conf, double rigidbodyRot)
{
	const Error *err = NULL;

	Point<AMPCT> pos;		
	pos[0] = 0.0;	pos[1] = 0.0;	pos[2] = 0.0;	pos[3] = 0.0;	pos[4] = 0.0;	pos[5] = 0.0;	pos[6] = 0.0;

	pos[3] = rigidbodyRot * m_angle_to_inc;
	pos[4] = (conf[0] + rigidbodyRot) * m_angle_to_inc;
	pos[5] = (conf[1] + rigidbodyRot) * m_angle_to_inc;
	pos[1] = (conf[2] - m_innerTubeFullExtensionLength) * m_mm_to_inc;		// convert mm to motor count
	
	// record MoveTo history
	recordJoint(pos, false);

	err = link.MoveTo(pos, m_velocity_in_inc, moveToGains[0]*m_velocity_in_inc, moveToGains[1]*m_velocity_in_inc, moveToGains[2]*m_velocity_in_inc);
	showerr( err, "Moving linkage" );
	err = link.WaitMoveDone();
	showerr( err, "waiting on linkage done" );

	// record MoveTo history
	recordJoint(pos, true);
}

void JunDriveSystem::RotateAllTo(double angle)
{
	const Error *err = NULL;

	Point<AMPCT> pos;		
	pos[0] = 0.0;	pos[1] = 0.0;	pos[2] = 0.0;	pos[3] = 0.0;	pos[4] = 0.0;	pos[5] = 0.0;	pos[6] = 0.0;

	pos[3] = angle * m_angle_to_inc;
	pos[4] = angle * m_angle_to_inc;
	pos[5] = angle * m_angle_to_inc;
	
	// record MoveTo history
	recordJoint(pos, false);

	err = link.MoveTo(pos, m_velocity_in_inc, moveToGains[0]*m_velocity_in_inc, moveToGains[1]*m_velocity_in_inc, moveToGains[2]*m_velocity_in_inc);
	showerr( err, "Moving linkage" );
	err = link.WaitMoveDone();
	showerr( err, "waiting on linkage done" );

	// record RotateAllTo history
	recordJoint(pos, true);
}

void JunDriveSystem::Home()
{
	MoveTo(0.0);
}

void JunDriveSystem::Dither(double target_angle, double dither_magnitude, int num_dither_steps)
{
	MoveTo(target_angle);

	if (num_dither_steps <= 0)
		return;

	for(int i = 0; i <= num_dither_steps; i++)
	{
		double cur_angle = target_angle + ::std::pow(-1.0, (double)i) * dither_magnitude * (double)(num_dither_steps-i)/(double)num_dither_steps;
		//double cur_angle = target_angle + ::std::pow(-1.0, (double)i) * dither_magnitude * (::std::exp(-(double)i/(double)num_dither_steps*3.0) - ::std::exp(-3.0));
		MoveTo(cur_angle);
	}
	MoveTo(target_angle);
}

void JunDriveSystem::Dither(::std::vector<double> target_conf, double dither_magnitude, int num_dither_steps)
{
	MoveTo(target_conf);
	
	if (num_dither_steps <= 0)
		return;

	// dither
	for(int j = 0; j < 2; j++)
	{
		for(int i = 0; i <= num_dither_steps; i++)
		{
			::std::vector<double> cur_conf = target_conf;
			cur_conf[j] = target_conf[j] + ::std::pow(-1.0, (double)i) * dither_magnitude * (double)(num_dither_steps-i)/(double)num_dither_steps;
			MoveTo(cur_conf);
		}
		MoveTo(target_conf);
	}
}

double JunDriveSystem::GetCurrentAngle(int amp_id)
{
	double angle;
	amp[amp_id].GetPositionActual(angle);

	return angle/m_angle_to_inc;
}

::std::vector<double> JunDriveSystem::GetCurrentConfiguration()
{
	::std::vector<double> conf(3);
	amp[4].GetPositionActual(conf[0]);
	conf[0] /= m_angle_to_inc;
	amp[5].GetPositionActual(conf[1]);
	conf[1] /= m_angle_to_inc;
	amp[1].GetPositionActual(conf[2]);
	conf[2]  = conf[2]/m_mm_to_inc + m_innerTubeFullExtensionLength;

	return conf;
}

void JunDriveSystem::RigidBodyTranslation(double trans)
{
	const Error *err = NULL;

	Point<AMPCT> pos;		
	pos[0] = 0.0;	pos[1] = 0.0;	pos[2] = 0.0;	pos[3] = 0.0;	pos[4] = 0.0;	pos[5] = 0.0;	pos[6] = 0.0;

	pos[0] = trans * m_mm_to_inc;
	pos[1] = trans * m_mm_to_inc;
	
	// record MoveTo history
	recordJoint(pos, false);

	err = link.MoveTo(pos, m_velocity_in_inc, moveToGains[0]*m_velocity_in_inc, moveToGains[1]*m_velocity_in_inc, moveToGains[2]*m_velocity_in_inc);
	showerr( err, "Moving linkage" );
	err = link.WaitMoveDone();
	showerr( err, "waiting on linkage done" );

	// record RotateAllTo history
	recordJoint(pos, true);
}

void JunDriveSystem::InitializeAmp()
{
	int32 canBPS = 1000000;             // CAN network bit rate
	int16 canNodeID = 1;                // CANopen node ID


	// CKim - Generate log file for debugging
	cml.SetDebugLevel( LOG_EVERYTHING );

	// Create an 'CanInterface' object used to access the low level CAN network.
	#if defined( USE_CAN )
	   static KvaserCAN hw("CAN");
	   hw.SetBaud( canBPS );
	#elif defined( WIN32 )
	   WinUdpEcatHardware hw( "eth0" );
	#else
	   LinuxEcatHardware hw( "eth0" );
	#endif

	   
	// Open the 'CANopen' network object
	#if defined( USE_CAN )
	   static CanOpen net;
	#else
	 EtherCAT net;
	#endif
	const Error *err = net.Open( hw );
	//showerr( err, "Opening network" );

	// Initialize the amplifier using default settings
	AmpSettings set;
	set.guardTime = 0;
	printf( "Doing init\n" );
	
	for(int i=0; i<AMPCT; i++ )
	{
		err = amp[i].Init( net, 1+i, set );
		showerr( err, "Initting amp" );

		// CKim - Get the motor info stored in the memory of the amp. How do you set it?
		MtrInfo mtrInfo;
		err = amp[i].GetMtrInfo( mtrInfo );
		showerr( err, "Getting motor info\n" );

		err = amp[i].SetCountsPerUnit( mtrInfo.ctsPerRev );
		showerr( err, "Setting cpr\n" );

		// Zeros the amps actual positions
		err= amp[i].SetPositionActual(0);
		showerr( err, "Setting position_Zero\n" );
	}

	// CKim - 'Linkage' object allows coordinated motion between the motors. 
	// Create a linkage object holding these amps. 
	err = link.Init( AMPCT, amp );
	showerr( err, "Linkage init" );
}

void JunDriveSystem::InitializeVariables()
{
	m_innerTubeFullExtensionLength = 55.0*0.5*JUN_PI;
	
	m_inc_per_rev = -10.936 * 0.998888 * 1.000049260526897;

	m_mm_to_inc = 1.0 / 3.175;
	SetAngleUnit(DEGREE);
	SetVelocity(60.0);			// set velocity to be 60deg/sec

	jointHistory.open("JointsHistory.txt");
}

void JunDriveSystem::recordJoint(const Point<7>& pos, bool isAfterMove)
{
	if (isAfterMove)
		jointHistory << "After move:\t";
	else
		jointHistory << "Before move:\t";

	jointHistory << pos[3]/m_angle_to_inc << "\t";
	jointHistory << pos[4]/m_angle_to_inc << "\t";
	jointHistory << pos[5]/m_angle_to_inc << "\t";
	jointHistory << pos[1]/m_mm_to_inc + m_innerTubeFullExtensionLength << ::std::endl;

	jointHistory.flush();
}