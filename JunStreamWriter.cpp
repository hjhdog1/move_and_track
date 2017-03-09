#include "JunStreamWriter.h"
#include <string>


bool					JunStreamWriter::m_bWriting;
JunDriveSystem*			JunStreamWriter::m_drive;
JunEMTracker*			JunStreamWriter::m_sensor;
::std::vector<int>		JunStreamWriter::m_sensorIds;
::std::ofstream			JunStreamWriter::m_sensorStrm[4];
::std::ofstream			JunStreamWriter::m_driveStrm;
HANDLE					JunStreamWriter::hThread;

void JunStreamWriter::Initialize(JunDriveSystem* drive, JunEMTracker* sensor)
{
	m_bWriting = false;


	m_drive = drive;
	m_sensor = sensor;
	m_sensorIds = m_sensor->getConnectedTrackerIds();

	hThread = CreateThread(NULL, 0, writing_Thread, 0, 0, NULL);

}

void JunStreamWriter::OpenStreams(::std::string fileNameTail)
{
	::std::string fileName = "./data/JAng";
	fileName.append(fileNameTail);
	fileName.append(".txt");
	m_driveStrm.open(fileName);
	for(unsigned int i = 0; i < m_sensorIds.size(); i++)
	{
		int sensorId = m_sensorIds[i];
		
		fileName = "./data/Trck";
		fileName.append(std::to_string(sensorId+1));
		fileName.append(fileNameTail);
		fileName.append(".txt");

		m_sensorStrm[sensorId].open(fileName);
	}
}

void JunStreamWriter::OpenStreams(::std::string fileNameTail, int fileNumber)
{
	fileNameTail.append("_");
	fileNameTail.append(std::to_string(fileNumber));

	OpenStreams(fileNameTail);
}

void JunStreamWriter::CloseStreams()
{
	StopWriting();

	m_driveStrm.close();
	for(unsigned int i = 0; i < m_sensorIds.size(); i++)
		m_sensorStrm[m_sensorIds[i]].close();
}

void JunStreamWriter::StartWriting()
{
	m_bWriting = true;
}

void JunStreamWriter::StopWriting()
{
	m_bWriting = false;
	::Sleep(100);
	for(unsigned int i = 0; i < m_sensorIds.size(); i++)
		m_sensorStrm[m_sensorIds[i]].flush();

	m_driveStrm.flush();
}

void JunStreamWriter::TerminateWriter()
{
	StopWriting();
	WaitForSingleObject(hThread,5000);
}


DWORD WINAPI JunStreamWriter::writing_Thread(LPVOID pData)
{
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD transformation;				
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD* pRecord = &transformation;	
	
	bool b_first_iter = true;
	double time0;

	while(true)
	{
		if(m_bWriting)	
		{
			for(unsigned int i = 0; i < m_sensorIds.size(); i++)
			{
				int sensorId = m_sensorIds[i];
				if(m_sensor->getTransformation(sensorId, transformation))
				{
					if(b_first_iter)
					{
						time0 = transformation.time;
						b_first_iter = false;
					}

					write_transformation(&m_sensorStrm[sensorId], transformation);
					m_sensor->displayTransformation(sensorId, transformation);
				}
			}

			double angle = m_drive->GetCurrentAngle();
			m_driveStrm << (transformation.time - time0) << "\t" << m_drive->GetCurrentAngle(3) << "\t" << m_drive->GetCurrentAngle(4) <<std::endl;
		}
	}

	return 1;
}

void JunStreamWriter::write_transformation(::std::ofstream* strm, const DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation)
{
	if(m_driveStrm == NULL)
		return;

	*strm <<transformation.s[0][0]<<"\t"<<transformation.s[0][1]<<"\t"<<transformation.s[0][2]<<"\t"<<transformation.x*25.4<<"\n";
	*strm <<transformation.s[1][0]<<"\t"<<transformation.s[1][1]<<"\t"<<transformation.s[1][2]<<"\t"<<transformation.y*25.4<<"\n";
	*strm <<transformation.s[2][0]<<"\t"<<transformation.s[2][1]<<"\t"<<transformation.s[2][2]<<"\t"<<transformation.z*25.4<<"\n";
}