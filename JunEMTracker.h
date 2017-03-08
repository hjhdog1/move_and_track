#ifndef _JUN_EM_TRACKER__
#define _JUN_EM_TRACKER_


#include <ATC3DG.h>


class JunEMTracker
{
public:
	JunEMTracker();
	bool					getTransformation(int tracker_id, DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation);
	void					displayTransformation(int tracker_id,  const DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation);

private:
	void					Initialize();

private:
	int						m_nSensors;			// number of sensors
};



#endif  