#pragma once

#include "CML.h"
#include "CML_TrjScurve.h"

using namespace CML;

class LinkTrjPVTFile: public LinkTrajectory
{
	int m_axNum;
	int m_ptNum;
	int m_cnt;
	double** m_P;
	double** m_V;
	double* m_T;
   
   /// Private copy constructor (not supported)
   LinkTrjPVTFile( const LinkTrjPVTFile& );

   /// Private assignment operator (not supported)
   LinkTrjPVTFile& operator=( const LinkTrjPVTFile& );

public:
	LinkTrjPVTFile(void);
	~LinkTrjPVTFile(void);
	
	// CKim - Set the number of linked axis and number of point in trajectory
	void	InitAxes(int axNum, int ptNum);
	
	// CKim - Set one pvt point
	void	AddPVT(int i, uunit p[], uunit v[], uunit t);

	virtual int GetDim( void )	{ return m_axNum; }

	virtual	const	Error *StartNew(void)	{	return 0;	}//&CML::Error::OK;	};
	virtual	void	Finish(void)			{	};
	
	// CKim - this is the function called by CAN service to send trajectory
	// return 0 for the time to end.
	virtual const Error *NextSegment( uunit pos[], uunit vel[], uint8 &time );
};



/***************************************************************************/
class LinkTrjScurve: public LinkTrajectory
{
   TrjScurve trj;
   Point<CML_MAX_AMPS_PER_LINK> start;
   double scale[CML_MAX_AMPS_PER_LINK];
   
   /// Private copy constructor (not supported)
   LinkTrjScurve( const LinkTrjScurve& );

   /// Private assignment operator (not supported)
   LinkTrjScurve& operator=( const LinkTrjScurve& );

public:
   LinkTrjScurve();

   const Error *Calculate( PointN &start, PointN &end, uunit vel, uunit acc, uunit dec, uunit jrk );
   int GetDim( void ){ return start.getDim(); }

   const Error *StartNew( void );
   void Finish( void );
   const Error *NextSegment( uunit pos[], uunit vel[], uint8 &time );

};