#include "StdAfx.h"
#include "LinkTrjPVTFile.h"


LinkTrjPVTFile::LinkTrjPVTFile(void)
{
}


LinkTrjPVTFile::~LinkTrjPVTFile(void)
{
	for(int i=0; i<m_axNum; i++)
	{
		delete[] m_P[i];		delete[] m_V[i];
	}
	delete[] m_P;		delete[] m_V;		delete[] m_T;
}


void LinkTrjPVTFile::InitAxes(int axNum, int ptNum)
{
	m_axNum = axNum;		m_ptNum = ptNum;			
	m_P = new double* [m_axNum];
	m_V = new double* [m_axNum];
	m_T = new double[m_ptNum];


	for(int i=0; i<m_axNum; i++)
	{
		m_P[i] = new double[m_ptNum];	m_V[i] = new double[m_ptNum];
		for(int j=0; j<m_ptNum; j++)
		{
			m_P[i][j] = m_V[i][j] = m_T[j] = 0.0;
		}
	}
	m_cnt = 0;
};

	
// CKim - Set one pvt point
void LinkTrjPVTFile::AddPVT(int idx, uunit p[], uunit v[], uunit t)
{
	for(int i=0; i<m_axNum; i++)
	{
		m_P[i][idx] = p[i];		m_V[i][idx] = v[i];
	}
	m_T[idx] = t;
}


const Error* LinkTrjPVTFile::NextSegment( uunit pos[], uunit vel[], uint8 &time )
{
	// CKim - this is the function called by CAN service to send trajectory
	// pos/vel is the array of position and velocity, time is duration of the segment
	// return 0 for the time to end.
	if(m_cnt >= m_ptNum)	{	time = 0;	}
	else
	{
		for(int i=0; i<m_axNum; i++)
		{
			pos[i] = m_P[i][m_cnt];		vel[i] = m_V[i][m_cnt];
	
			if(m_cnt == 0)	{	time = m_T[m_cnt];					}
			else			{	time = m_T[m_cnt] -m_T[m_cnt-1];	}
		}
		m_cnt++;
	}
	return 0;//&CML::Error::OK;
}