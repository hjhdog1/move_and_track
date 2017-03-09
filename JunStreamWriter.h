#ifndef _JUN_STREAM_WRITER_
#define _JUN_STREAM_WRITER_

// This is a singleton class

#include <fstream>
#include <windows.h>

#include "JunDriveSystem.h"
#include "JunEMTracker.h"

class JunStreamWriter
{
	
public:
	JunStreamWriter(JunDriveSystem* drive, JunEMTracker* sensor);
	~JunStreamWriter();
	void					OpenStreams(::std::string fileNameTail);
	void					OpenStreams(::std::string fileNameTail, int fileNumber);
	void					CloseStreams();
	void					StartWriting();
	void					StopWriting();

private:
	void					Initialize(JunDriveSystem* drive, JunEMTracker* sensor);
	static void				write_transformation(::std::ofstream* strm, const DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation);
	static DWORD WINAPI		writing_Thread(LPVOID pData);
	void					TerminateWriter();
	
private:
	bool					m_bWriting;
	JunDriveSystem*			m_drive;
	JunEMTracker*			m_sensor;
	::std::vector<int>		m_sensorIds;
	::std::ofstream			m_sensorStrm[4];
	::std::ofstream			m_driveStrm;

	HANDLE					hThread;

};



#endif  