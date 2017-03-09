#ifndef _JUN_STREAM_WRITER_
#define _JUN_STREAM_WRITER_

// This is a singleton class

#include <fstream>
#include <windows.h>

#include "JunDriveSystem.h"
#include "JunEMTracker.h"

class JunStreamWriter
{
private:
	JunStreamWriter(){};

public:
	static void					Initialize(JunDriveSystem* drive, JunEMTracker* sensor);
	static void					OpenStreams(::std::string fileNameTail);
	static void					OpenStreams(::std::string fileNameTail, int fileNumber);
	static void					CloseStreams();
	static void					StartWriting();
	static void					StopWriting();
	static void					TerminateWriter();

private:
	static DWORD WINAPI			writing_Thread(LPVOID pData);
	static void					write_transformation(::std::ofstream* strm, const DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation);

private:
	static bool					m_bWriting;
	static JunDriveSystem*		m_drive;
	static JunEMTracker*		m_sensor;
	static ::std::vector<int>	m_sensorIds;
	static ::std::ofstream		m_sensorStrm[4];
	static ::std::ofstream		m_driveStrm;

	static HANDLE				hThread;

};



#endif  