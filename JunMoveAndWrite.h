#ifndef _JUN_MOVE_AND_WRITE_
#define _JUN_MOVE_AND_WRITE_


#include "JunStreamWriter.h"



class JunMoveAndWrite
{
public:
	JunMoveAndWrite();
	void					RunUnditeredMotion(double vel, int num_repeats);
	void					RunDiteredMotion(double vel, int num_measurements, double dither_magintude, int num_dither_steps, int num_repeats);

	void					RunTrajectory(::std::string path2trajectory, ::std::string outPutFileNameTail, double dither_magintude, int num_dither_steps);	// for 3 tube robot
	void					RunTrajectoryForAllApproachingDirections(::std::string path2trajectory, ::std::string outPutFileNameTail);	// for 3 tube robot
	void					RunTrajectoryForRandomApproachingDirections(::std::string path2trajectory, ::std::string outPutFileNameTail);	// for 3 tube robot

private:
	void					RunBaseFrameMotion();
	void					MoveDriveSystemTo(double angle);
	void					RotateDriveSystemAllTo(double angle);
	void					DitherDriveSystem(double target_angle, double dither_magintude, int num_dither_steps);
	void					DitherDriveSystem(::std::vector<double> target_conf, double dither_magintude, int num_dither_steps);
	void					CheckEmergenceStop();
	static DWORD WINAPI		checking_emergency(LPVOID pData);
	

private:
	JunDriveSystem			m_drive;
	JunEMTracker			m_sensor;
	bool					m_bEmergency;
	
};



#endif  