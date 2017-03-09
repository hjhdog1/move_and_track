#ifndef _JUN_EM_TRACKER_
#define _JUN_EM_TRACKER_


#include <ATC3DG.h>
#include <vector>

class JunEMTracker
{
public:
	JunEMTracker();
	bool					getTransformation(int tracker_id, DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation)	const;
	void					displayTransformation(int tracker_id,  const DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation)	const;
	::std::vector<int>		getConnectedTrackerIds()	const;
	
private:
	void					Initialize();

private:
	int						m_nSensors;			// number of sensors
	
};



#endif  