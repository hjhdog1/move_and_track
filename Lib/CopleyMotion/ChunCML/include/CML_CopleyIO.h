/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2010 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file 

  Standard CANopen I/O module support.

*/

#ifndef _DEF_INC_COPLEYIO
#define _DEF_INC_COPLEYIO

#include "CML_Settings.h"
#include "CML_Node.h"
#include "CML_PDO.h"
#include "CML_IO.h"
#include "CML_Error.h"

CML_NAMESPACE_START()

/***************************************************************************/
/**
  Object dictionary ID values used on Copley I/O modules.
  */
/***************************************************************************/
enum CIO_OBJID
{
   CIOOBJID_INFO_SERIAL             = 0x3000,  ///<  Serial number
   CIOOBJID_INFO_MODEL              = 0x3001,  ///<  Model number string
   CIOOBJID_INFO_MFGINFO            = 0x3002,  ///<  Amplifier's manufacturing information string
   CIOOBJID_INFO_HWTYPE             = 0x3003,  ///<  Hardware type code
   CIOOBJID_INFO_LOOPRATE           = 0x3004,  ///<  Main loop update rate (Hz)
   CIOOBJID_INFO_FWVERSION          = 0x3010,  ///<  Firmware version number
   CIOOBJID_INFO_BAUD               = 0x3011,  ///<  Serial port baud rate (bps)
   CIOOBJID_INFO_MAXWORDS           = 0x3012,  ///<  Maximum number of words sent with any command
   CIOOBJID_INFO_NAME               = 0x3013,  ///<  I/O module name
   CIOOBJID_INFO_HOSTCFG            = 0x3014,  ///<  Host configuration state (CME use only)
   CIOOBJID_INFO_NODECFG            = 0x3015,  ///<  CAN node ID configuration
   CIOOBJID_INFO_RATECFG            = 0x3016,  ///<  CAN bit rate configuration
   CIOOBJID_INFO_NODEID             = 0x3017,  ///<  CAN node ID
   CIOOBJID_INFO_STATUS             = 0x3018,  ///<  CAN network status word
   CIOOBJID_INFO_RATE               = 0x3019,  ///<  CAN network bit rate
   CIOOBJID_INFO_ANLGINT            = 0x301A,  ///<  Active analog interrupts
   CIOOBJID_INFO_ANLGINTENA         = 0x301B,  ///<  Analog input global interrupt enable
   CIOOBJID_INFO_DIGIINTENA         = 0x301C,  ///<  Digital input global interrupt enable
   CIOOBJID_INFO_PWMPERIODA         = 0x301E,  ///<  PWM bank A period
   CIOOBJID_INFO_PWMPERIODB         = 0x301F,  ///<  PWM bank B period

   CIOOBJID_DIGI_BANKMODE           = 0x3020,  ///<  Digital I/O bank mode
   CIOOBJID_DIGI_PULLUPMSK          = 0x3021,  ///<  Digital I/O pull-up resistor mask
   CIOOBJID_DIGI_TYPEMSK            = 0x3022,  ///<  Digital I/O output type mask
   CIOOBJID_DIGI_FAULTMSK           = 0x3023,  ///<  Digital I/O output fault state mask
   CIOOBJID_DIGI_INVMSK             = 0x3024,  ///<  Digital I/O inversion mask
   CIOOBJID_DIGI_VALUEMSK           = 0x3025,  ///<  Digital I/O data value mask
   CIOOBJID_DIGI_MODEMSK            = 0x3026,  ///<  Digital I/O output fault mode mask
   CIOOBJID_DIGI_RAWMSK             = 0x3027,  ///<  Digital I/O raw data value mask
   CIOOBJID_DIGI_HILOMSK            = 0x3028,  ///<  Digital I/O input low->high interrupt mask
   CIOOBJID_DIGI_LOHIMSK            = 0x3029,  ///<  Digital I/O input high->low interrupt mask
   CIOOBJID_DIGI_DEBOUNCE0          = 0x3030,  ///<  Digital I/O debounce time, bit 0
   CIOOBJID_DIGI_DEBOUNCE1          = 0x3031,  ///<  Digital I/O debounce time, bit 1
   CIOOBJID_DIGI_DEBOUNCE2          = 0x3032,  ///<  Digital I/O debounce time, bit 2
   CIOOBJID_DIGI_DEBOUNCE3          = 0x3033,  ///<  Digital I/O debounce time, bit 3
   CIOOBJID_DIGI_DEBOUNCE4          = 0x3034,  ///<  Digital I/O debounce time, bit 4
   CIOOBJID_DIGI_DEBOUNCE5          = 0x3035,  ///<  Digital I/O debounce time, bit 5
   CIOOBJID_DIGI_DEBOUNCE6          = 0x3036,  ///<  Digital I/O debounce time, bit 6
   CIOOBJID_DIGI_DEBOUNCE7          = 0x3037,  ///<  Digital I/O debounce time, bit 7

   CIOOBJID_ANLG_IRAW               = 0x3040,  ///<  Analog input raw value
   CIOOBJID_ANLG_ISCALED            = 0x3041,  ///<  Analog input scaled value
   CIOOBJID_ANLG_IFACTOR            = 0x3042,  ///<  Analog input scaling factor
   CIOOBJID_ANLG_IOFFSET            = 0x3043,  ///<  Analog input offset
   CIOOBJID_ANLG_IUPLIMIT           = 0x3044,  ///<  Analog input upper limit for interrupt
   CIOOBJID_ANLG_ILOLIMIT           = 0x3045,  ///<  Analog input lower limit for interrupt
   CIOOBJID_ANLG_IABSDELTA          = 0x3046,  ///<  Analog input absolute delta value for interrrupt
   CIOOBJID_ANLG_IPOSDELTA          = 0x3047,  ///<  Analog input positive delta value for interrrupt
   CIOOBJID_ANLG_INEGDELTA          = 0x3048,  ///<  Analog input negative delta value for interrrupt
   CIOOBJID_ANLG_IFLAGS             = 0x3049,  ///<  Analog input interrrupt flags
   CIOOBJID_ANLG_IMASK              = 0x304A,  ///<  Analog input interrrupt mask

   CIOOBJID_PWM_ORAW                = 0x3050,  ///<  PWM output raw value
   CIOOBJID_PWM_OSCALED             = 0x3051,  ///<  PWM output scaled value
   CIOOBJID_PWM_OFACTOR             = 0x3052,  ///<  PWM output scaling factor
   CIOOBJID_PWM_OOFFSET             = 0x3053   ///<  PWM output offset
};

/***************************************************************************/
/**
  IO Module characteristics data structure.

  This structure is used to hold information about the IO Module such as it's
  model number, serial number, etc.  

  The IO Module characteristics defined in this structure can not be changed.
  They are defined by Copley Controls when the amplifier is designed and/or
  manufactured.

  Use the IOModule::GetIOInfo method to retrieve this information from the module.

*/
/***************************************************************************/
struct CopleyIOInfo
{
#define COPLEY_MAX_STRING    41

   uint32 serial;						///< Serial number
   char model[ COPLEY_MAX_STRING ];     ///< Model number string
   char mfgInfo[ COPLEY_MAX_STRING ];   ///< Amplifier's manufacturing information string
   uint16 hwType;						///< Hardware type code
   uint16 loopRate;						///< Main loop update rate (Hz)
   uint16 fwVersion;					///< Firmware version number
   uint32 baud;							///< Serial port baud rate (bps)
   uint16 maxWords;						///< Maximum number of words sent with any command
   char name[ COPLEY_MAX_STRING ];		///< I/O module name
   char hostCfg[ COPLEY_MAX_STRING ];	///< Host configuration state (CME use only)

   int16 nodeCfg;						///< CAN node ID configuration
   uint16 rateCfg;						///< CAN bit rate configuration
   uint16 nodeID;						///< CAN node ID
   uint16 status;						///< CAN network status word
   uint16 rate;							///< CAN network bit rate

   uint16 anlgInt;						///< Active analog interrupts
   uint16 anlgIntEna;					///< Analog input global interrupt enable
   uint16 digiIntEna;					///< Digital input global interrupt enable

   uint32 pwmPeriodA;					///< PWM bank A period
   uint32 pwmPeriodB;					///< PWM bank B period
};

/***************************************************************************/
/**

  */
/***************************************************************************/
struct CopleyIODigi
{
#define NUMBER_OF_BANKS    11

   uint16 bankMode[ NUMBER_OF_BANKS ];	 ///< Digital I/O bank mode
   uint16 pullupMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O pull-up resistor mask
   uint16 typeMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O output type mask
   uint16 faultMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O output fault state mask
   uint16 invMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O inversion mask
   uint16 valueMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O data value mask
   uint16 modeMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O output fault mode mask
   uint16 rawMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O raw data value mask
   uint16 hiLoMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O input low->high interrupt mask
   uint16 loHiMsk[ NUMBER_OF_BANKS ];	 ///< Digital I/O input high->low interrupt mask
   uint16 debounce0[ NUMBER_OF_BANKS ];	 ///< Digital I/O debounce time, bit 0
   uint16 debounce1[ NUMBER_OF_BANKS ];	 ///< Digital I/O debounce time, bit 1
   uint16 debounce2[ NUMBER_OF_BANKS ];	 ///< Digital I/O debounce time, bit 2
   uint16 debounce3[ NUMBER_OF_BANKS ];	 ///< Digital I/O debounce time, bit 3
   uint16 debounce4[ NUMBER_OF_BANKS ];	 ///< Digital I/O debounce time, bit 4
   uint16 debounce5[ NUMBER_OF_BANKS ];	 ///< Digital I/O debounce time, bit 5
   uint16 debounce6[ NUMBER_OF_BANKS ];	 ///< Digital I/O debounce time, bit 6
   uint16 debounce7[ NUMBER_OF_BANKS ];	 ///< Digital I/O debounce time, bit 7
};

/***************************************************************************/
/**

  */
/***************************************************************************/
struct CopleyIOAnlg
{
#define NUMBER_OF_INPUTS    13

   uint16 iRaw[ NUMBER_OF_INPUTS ];		 ///< Analog input raw value
   uint32 iScaled[ NUMBER_OF_INPUTS ];	 ///< Analog input scaled value
   uint32 iFactor[ NUMBER_OF_INPUTS ];	 ///< Analog input scaling factor
   uint32 iOffset[ NUMBER_OF_INPUTS ];	 ///< Analog input offset
   uint32 iUpLimit[ NUMBER_OF_INPUTS ];	 ///< Analog input upper limit for interrupt
   uint32 iLoLimit[ NUMBER_OF_INPUTS ];	 ///< Analog input lower limit for interrupt
   uint32 iAbsDelta[ NUMBER_OF_INPUTS ]; ///< Analog input absolute delta value for interrrupt
   uint32 iPosDelta[ NUMBER_OF_INPUTS ]; ///< Analog input positive delta value for interrrupt
   uint32 iNegDelta[ NUMBER_OF_INPUTS ]; ///< Analog input negative delta value for interrrupt
   uint16 iFlags[ NUMBER_OF_INPUTS ];	 ///< Analog input interrrupt flags
   uint16 iMask[ NUMBER_OF_INPUTS ];	 ///< Analog input interrrupt mask
};

/***************************************************************************/
/**

  */
/***************************************************************************/
struct CopleyIOPWM
{
#define NUMBER_OF_OUTPUTS    13

   uint16 oRaw[ NUMBER_OF_OUTPUTS ];	 ///< PWM output raw value
   uint32 oScaled[ NUMBER_OF_OUTPUTS ];	 ///< PWM output scaled value
   uint32 oFactor[ NUMBER_OF_OUTPUTS ];	 ///< PWM output scaling factor
   int32 oOffset[ NUMBER_OF_OUTPUTS ];	 ///< PWM output offset
};

/***************************************************************************/
/**
  IO Module configuration structure.  This structure contains all user 
  configurable parameters used by an IO module which may be stored in non-volatile
  memory.
  */
/***************************************************************************/
struct CopleyIOCfg
{
	/// Global IO Module parameters
	CopleyIOInfo info;

	/// Digital IO parameters
	CopleyIODigi digi;

	/// Analog input parameters
	CopleyIOAnlg anlg;

	/// PWM output parameters
	CopleyIOPWM pwm;
};

/***************************************************************************/
class CopleyIO: public IOModule
{
public:
   CopleyIO( void );
   CopleyIO( Network &net, int16 nodeID );
   CopleyIO( Network &net, int16 nodeID, IOModuleSettings &settings );
   virtual ~CopleyIO();
   virtual const Error *Init( Network &net, int16 nodeID );
   virtual const Error *Init( Network &net, int16 nodeID, IOModuleSettings &settings );

   /************************************************************************/
   // Just added for .cci transfer
   /************************************************************************/
   const Error *GetIOInfo( CopleyIOInfo &info );
   const Error *GetIODigi( CopleyIODigi &digi );
   const Error *GetIOAnlg( CopleyIOAnlg &anlg );
   const Error *GetIOPWM( CopleyIOPWM &pwm, CopleyIOInfo &info );
   const Error *GetIOCfg( CopleyIOCfg &cfg );
   const Error *SetIOInfo( CopleyIOInfo &info );
   const Error *SetIODigi( CopleyIODigi &digi );
   const Error *SetIOAnlg( CopleyIOAnlg &anlg );
   const Error *SetIOPWM( CopleyIOPWM &pwm, CopleyIOInfo &info );
   const Error *SetIOConfig( CopleyIOCfg &cfg );
   const Error *SaveIOConfig( void );
   const Error *SaveIOConfig( CopleyIOCfg &cfg );
   const Error *LoadFromFile( const char *name, int &line );
   const Error *SerialCmd( uint8 opcode, uint8 &ct, uint8 max, uint16 *data );

   uint8 lastSerialError;
};

/***************************************************************************/
/**
This class represents error conditions that can occur when loading IO module
data from a data file.
*/
/***************************************************************************/
class IOFileError: public Error
{
   public:
      static const IOFileError bankInvalid;  ///< I/O bank invalid

   protected:
      /// Standard protected constructor
      IOFileError( uint16 id, const char *desc ): Error( id, desc ){}
};

CML_NAMESPACE_END()

#endif

