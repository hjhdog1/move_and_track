#include "JunEMTracker.h"
#include <windows.h>
#include "handleErrors.h"

JunEMTracker::JunEMTracker()
{
	Initialize();
}

 bool JunEMTracker::getTransformation(int tracker_id, DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& trasformation)
{
	
	int errCode = GetAsynchronousRecord(tracker_id, &trasformation, sizeof(trasformation));
	if(errCode!=BIRD_ERROR_SUCCESS)
	{
		errorHandler(errCode);
		return false;
	}
	return true;
}

void JunEMTracker::displayTransformation(int tracker_id,  const DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation)
{
	COORD coordinate;
	int i = tracker_id;

	coordinate.Y=6+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
	printf("%8.3f %8.3f %8.3f %8.3f", transformation.s[0][0], transformation.s[0][1], transformation.s[0][2], transformation.x*25.4);
	
	coordinate.Y=7+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
	printf("%8.3f %8.3f %8.3f %8.3f", transformation.s[1][0], transformation.s[1][1], transformation.s[1][2], transformation.y*25.4);
	
	coordinate.Y=8+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
	printf("%8.3f %8.3f %8.3f %8.3f", transformation.s[2][0], transformation.s[2][1], transformation.s[2][2], transformation.z*25.4);
	
	coordinate.Y=9+5*i;		coordinate.X=4;		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
	printf("%8.3f %8.3f %8.3f %8.3f", 0, 0, 0, 1);
}


 void JunEMTracker::Initialize()
{
	
	// ---------------------------------------------------------------- //
	// CKim - Set up the Ascension EM tracker.
	// Related include and library files are 'ATC3DG.h' 
	// ---------------------------------------------------------------- //
	printf( "Initializing tracker wait please...\n" );
   	SYSTEM_CONFIGURATION	trackerConfig;		SENSOR_CONFIGURATION*	sensorConfig;		
	TRANSMITTER_CONFIGURATION	xmtrConfig;		int trackerError;		int sensorNum = 0;		short xmtrId = 0;
	DATA_FORMAT_TYPE type = DOUBLE_POSITION_MATRIX_TIME_Q;	
	
	// CKim - Initialize the tracker and get system configuration
	trackerError = InitializeBIRDSystem();
	if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
	
	trackerError = GetBIRDSystemConfiguration(&trackerConfig);
	if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}

	// CKim - Configure the sensors. Get the number of connected sensors. Set the data format for sensor reading
	sensorConfig = new SENSOR_CONFIGURATION[trackerConfig.numberSensors];
	for(int i=0; i<trackerConfig.numberSensors; i++)
	{
		trackerError = GetSensorConfiguration(i, &(sensorConfig[i]));
		if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
		if(sensorConfig[i].attached)
		{
			trackerError = SetSensorParameter(i,DATA_FORMAT,&type,sizeof(type));  
			if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}
			sensorNum++;						
		}
	}

	printf("Number of sensors connected : %d\n",sensorNum);
	printf("Press any key to stat tracking");
	std::getchar();

	// CKim - Configure the transmitter and turn it on. xmtrId must be type short!!
	trackerError = GetTransmitterConfiguration(0,&xmtrConfig);
	if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}

	trackerError = SetSystemParameter(SELECT_TRANSMITTER, &xmtrId, sizeof(xmtrId));
	if(trackerError!=BIRD_ERROR_SUCCESS)	{		errorHandler(trackerError);			}

	// JHa - Set number of sensors
	m_nSensors = sensorNum;
}
