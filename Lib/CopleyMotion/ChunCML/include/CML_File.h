/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2010 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file
This file holds various handy functions for parsing files.
*/

#ifndef _DEF_INC_FILE
#define _DEF_INC_FILE

#include "CML_Settings.h"
#include "CML_AmpDef.h"

CML_NAMESPACE_START()

// Local functions
#ifdef CML_FILE_ACCESS_OK
const Error *ReadLine( void *fp, char *buff, int max );
int SplitLine( char *buff, char **seg, int max, char delim=',' );
const Error *StrToInt32( char *str, int32 &i, int base=0 );
const Error *StrToUInt32( char *str, uint32 &i, int base=0 );
const Error *StrToInt16( char *str, int16 &i, int base=0 );
const Error *StrToUInt16( char *str, uint16 &i, int base=0 );
const Error *StrToOutCfg( char *str, OUTPUT_PIN_CONFIG &cfg, uint32 &mask1, uint32 &mask2 );
const Error *StrToFilter( char *str, Filter &flt );
const Error *StrToHostCfg( char *str, char *hostCfg );
COPLEY_HOME_METHOD HomeMethodConvert( uint16 x );
#endif

CML_NAMESPACE_END()

#endif
