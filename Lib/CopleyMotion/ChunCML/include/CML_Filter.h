/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2002 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file

This file defines the Filter object.

The Filter object represents a two pole filter structure used
in various locations within the amplifier.

*/

#ifndef _DEF_INC_FILTER
#define _DEF_INC_FILTER

#include "CML_Settings.h"
#include "CML_SDO.h"
#include "CML_Utils.h"

CML_NAMESPACE_START()

/***************************************************************************/
/**
Generic filter structure.  This structure holds the coefficients used by the
amplifier in various configurable filters.
*/
/***************************************************************************/

class Filter
{

   /// FPGA based amplifiers use floating point coefficients
   float fcoef[5];

   /// This parameter will be true if filter coefficients are stored
   /// as floating point values.
   bool asFloat;

      /// DSP based amplifiers store filter coefficients as 
   /// 16-bit integers with a scaling factor
   int16 icoef[6];

   /// These words hold information about the filter.  They are 
   /// presently reserved for use by the CME program.
   uint16 info[4];

public:
   Filter( void );

   const Error *LoadFromCCX( int32 coef[], int ct );
   const Error *Upld( SDO &sdo, int16 index, bool asFloat=false );
   const Error *Dnld( SDO &sdo, int16 index, bool asFloat=false );
   void getIntCoef( int16 &a1, int16 &a2, int16 &b0, int16 &b1, int16 &b2, int16 &k );
   void getFloatCoef( float &a1, float &a2, float &b0, float &b1, float &b2 );
   void setIntCoef( int16 a1, int16 a2, int16 b0, int16 b1, int16 b2, int16 k );
   void setFloatCoef( float a1, float a2, float b0, float b1, float b2 );
};

CML_NAMESPACE_END()

#endif

