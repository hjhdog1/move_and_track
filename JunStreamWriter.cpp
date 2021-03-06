#include "JunStreamWriter.h"

#include <string>
#include <direct.h>


JunStreamWriter::JunStreamWriter(JunDriveSystem* drive, JunEMTracker* sensor)
{
	Initialize(drive, sensor);
}

JunStreamWriter::~JunStreamWriter()
{
	TerminateWriter();
}

void JunStreamWriter::Initialize(JunDriveSystem* drive, JunEMTracker* sensor)
{
	m_bWriting = false;
	m_bStop = false;
	m_recordFullConfiguration = false;


	m_drive = drive;
	m_sensor = sensor;
	m_sensorIds = m_sensor->getConnectedTrackerIds();

	hThread = CreateThread(NULL, 0, writing_Thread, this, 0, NULL);

}

void JunStreamWriter::OpenStreams(::std::string fileNameTail)
{
	mkdir("./data");

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


void JunStreamWriter::ActivateFullConfigurationRecording()
{
	m_recordFullConfiguration = true;
}

void JunStreamWriter::DeactivateFullConfigurationRecording()
{
	m_recordFullConfiguration = false;
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

DWORD WINAPI JunStreamWriter::writing_Thread(LPVOID pData)
{
	JunStreamWriter* writer =  (JunStreamWriter*)pData;

	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD transformation;				
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD* pRecord = &transformation;	
	
	bool b_first_iter = true;
	double time0;

	while(!writer->m_bStop)
	{
		if(writer->m_bWriting)	
		{
			for(unsigned int i = 0; i < writer->m_sensorIds.size(); i++)
			{
				int sensorId = writer->m_sensorIds[i];
				if(writer->m_sensor->getTransformation(sensorId, transformation))
				{
					if(b_first_iter)
					{
						time0 = transformation.time;
						b_first_iter = false;
					}

					write_transformation(&writer->m_sensorStrm[sensorId], transformation);
					writer->m_sensor->displayTransformation(sensorId, transformation);
				}
			}

			if(writer->m_recordFullConfiguration)
			{
				::std::vector<double> conf = writer->m_drive->GetCurrentConfiguration();
				writer->m_driveStrm << (transformation.time - time0) << "\t" << conf[0] << "\t" << conf[1] << "\t" << conf[2] <<std::endl;
			}
			else
				writer->m_driveStrm << (transformation.time - time0) << "\t" << writer->m_drive->GetCurrentAngle(3) << "\t" << writer->m_drive->GetCurrentAngle(4) <<std::endl;
		}
	}

	return 1;
}

void JunStreamWriter::write_transformation(::std::ofstream* strm, const DOUBLE_POSITION_MATRIX_TIME_Q_RECORD& transformation)
{
	*strm <<transformation.s[0][0]<<"\t"<<transformation.s[0][1]<<"\t"<<transformation.s[0][2]<<"\t"<<transformation.x*25.4<<"\n";
	*strm <<transformation.s[1][0]<<"\t"<<transformation.s[1][1]<<"\t"<<transformation.s[1][2]<<"\t"<<transformation.y*25.4<<"\n";
	*strm <<transformation.s[2][0]<<"\t"<<transformation.s[2][1]<<"\t"<<transformation.s[2][2]<<"\t"<<transformation.z*25.4<<"\n";
}

void JunStreamWriter::TerminateWriter()
{
	StopWriting();
	m_bStop = true;
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
}
