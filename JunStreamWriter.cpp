#include "JunStreamWriter.h"



void JunStreamWriter::Initialize(JunDriveSystem* drive, JunEMTracker* sensor)
{
	m_bWriting = false;
	for(int i = 0; i < 4; i++)
		m_sensorStrm[i] = NULL;
	m_driveStrm = NULL;

	m_drive = drive;
	m_sensor = sensor;

	hThread = CreateThread(NULL, 0, writing_Thread, 0, 0, NULL);

}

void JunStreamWriter::SetStream_drive(::std::ofstream* strm)
{
	m_driveStrm = strm;
}

void JunStreamWriter::SetStream_sensor(int tracker_id, ::std::ofstream* strm)
{
	m_sensorStrm[tracker_id] = strm;
}

void JunStreamWriter::StartWriting()
{
	m_bWriting = true;
}

void JunStreamWriter::StopWriting()
{
	m_bWriting = false;
	for(int i = 0; i < 4; i++)
		if(m_sensorStrm[i] != NULL)
			m_sensorStrm[i]->flush();

	if(m_driveStrm != NULL)
		m_driveStrm->flush();

}

void JunStreamWriter::CloseWriter()
{
	StopWriting();
	::Sleep(1000);
	WaitForSingleObject(hThread,5000);
}


DWORD WINAPI JunStreamWriter::writing_Thread(LPVOID pData)
{
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD transformation;				
	DOUBLE_POSITION_MATRIX_TIME_Q_RECORD* pRecord = &transformation;	
	COORD coordinate;
	double pos[7];

	bool b_first_iter = true;
	double time0;

	while(true)
	{
		if(m_bWriting)	
		{
			for(int i=0; i<3; i++)
			{
				if(m_sensor->getTransformation(i, transformation))
				{
					if(b_first_iter)
					{
						time0 = transformation.time;
						b_first_iter = false;
					}

					write_transformation(m_sensorStrm[i], transformation);
					m_sensor->displayTransformation(i, transformation);
				}
			}

			double angle = m_drive->GetCurrentAngle();
			*m_driveStrm << (transformation.time - time0) << "\t" << m_drive->GetCurrentAngle(3) << "\t" << m_drive->GetCurrentAngle(4) <<std::endl;
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