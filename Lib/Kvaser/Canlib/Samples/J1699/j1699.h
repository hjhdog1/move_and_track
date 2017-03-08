/*
********************************************************************************
** SAE J1699-3 Test Source Code
**
**  Copyright (C) 2002 Drew Technologies. http://j1699-3.sourceforge.net/
**
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** This computer program is based upon SAE Technical Report J1699,
** which is provided "AS IS"
**
********************************************************************************
*/

/*
** Typedefs and function pointers for J2534 API
*/
typedef long (CALLBACK* PTCONNECT)(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long *);
typedef long (CALLBACK* PTDISCONNECT)(unsigned long);
typedef long (CALLBACK* PTREADMSGS)(unsigned long, void *, unsigned long *, unsigned long);
typedef long (CALLBACK* PTWRITEMSGS)(unsigned long, void *, unsigned long *, unsigned long);
typedef long (CALLBACK* PTSTARTPERIODICMSG)(unsigned long, void *, unsigned long *, unsigned long);
typedef long (CALLBACK* PTSTOPPERIODICMSG)(unsigned long, unsigned long);
typedef long (CALLBACK* PTSTARTMSGFILTER)(unsigned long, unsigned long, void *, void *, void *, unsigned long *);
typedef long (CALLBACK* PTSTOPMSGFILTER)(unsigned long, unsigned long);
typedef long (CALLBACK* PTSETPROGRAMMINGVOLTAGE)(unsigned long, unsigned long);
typedef long (CALLBACK* PTREADVERSION)(unsigned long, char *, char *, char *);
typedef long (CALLBACK* PTGETLASTERROR)(char *);
typedef long (CALLBACK* PTIOCTL)(unsigned long, unsigned long, void *, void *);

/*
** 01/04/06 : Update to J2534 API to 4.04
*/
typedef long (CALLBACK* PTOPEN)(void *, unsigned long *);
typedef long (CALLBACK* PTCLOSE)(unsigned long);

/*
** 06/24/04: Enhancement required to satisfy Prompt 2 for test cases 5.0 & 5.17.
*/
extern unsigned long gUserNumEcusReprgm;

extern unsigned long gRespTimeOutofRange;
extern unsigned long gRespTimeTooSoon;
extern unsigned long gRespTimeTooLate;
extern unsigned long gDetermineProtocol;

extern PTCONNECT PassThruConnect;
extern PTDISCONNECT PassThruDisconnect;
extern PTREADMSGS PassThruReadMsgs;
extern PTWRITEMSGS PassThruWriteMsgs;
extern PTSTARTPERIODICMSG PassThruStartPeriodicMsg;
extern PTSTOPPERIODICMSG PassThruStopPeriodicMsg;
extern PTSTARTMSGFILTER PassThruStartMsgFilter;
extern PTSTOPMSGFILTER PassThruStopMsgFilter;
extern PTSETPROGRAMMINGVOLTAGE PassThruSetProgrammingVoltage;
extern PTREADVERSION PassThruReadVersion;
extern PTGETLASTERROR PassThruGetLastError;
extern PTIOCTL PassThruIoctl;
extern PTOPEN  PassThruOpen;
extern PTCLOSE PassThruClose;

/* maximum number of J2534 devices allowed */
#define MAX_J2534_DEVICES       50

/* Battery voltage limits */
/* Below 11VDC, OBD monitors are not required to run */
#define OBD_BATTERY_MINIMUM     11000

/* Above 18VDC, J1978 scan tools are not required to run */
#define OBD_BATTERY_MAXIMUM     18000

/* Tester present message rate */
/* 4.9 seconds between tester present messages */
#define OBD_TESTER_PRESENT_RATE 2000

/* Time delay to allow for code clearing */
#define CLEAR_CODES_DELAY_MSEC  2000

/* Maximum number of OBD ECUs and protocols */
#define OBD_MAX_ECUS            8
#define OBD_MAX_PROTOCOLS       7

/* OBD response indicator bit */
#define OBD_RESPONSE_BIT        0x40

/* Maximum number of bytes in ISO15765 frame */
#define ISO15765_MAX_BYTES_PER_FRAME 7

/* Function return value definitions */
typedef enum {PASS, FAIL, ABORT} STATUS;

/* NAK Response ID and Codes */
#define NAK                     0x7F
#define NAK_GENERAL_REJECT      0x10
#define NAK_NOT_SUPPORTED       0x11
#define NAK_INVALID_FORMAT      0x12
#define NAK_REPEAT_REQUEST      0x21
#define NAK_SEQUENCE_ERROR      0x22
#define NAK_RESPONSE_PENDING    0x78

/* Tester node address */
#define TESTER_NODE_ADDRESS     0xF1

/* User prompt type definitions */
#define ENTER_PROMPT            1
#define YES_NO_PROMPT           2
#define YES_NO_ALL_PROMPT       3
#define NO_WAIT_PROMPT          4

/* SID 9 InfoTypes */
#define INF_TYPE_VIN_COUNT      0x01
#define INF_TYPE_VIN            0x02
#define INF_TYPE_CALID_COUNT    0x03
#define INF_TYPE_CALID          0x04
#define INF_TYPE_CVN_COUNT      0x05
#define INF_TYPE_CVN            0x06
#define INF_TYPE_IPT_COUNT      0x07
#define INF_TYPE_IPT            0x08
#define INF_TYPE_ECUNAME        0x0A
#define INF_TYPE_IPD            0x0B

#define INF_TYPE_IPT_NODI       20      // number of IPT data items

#define	IPT_OBDCOND_INDEX   0   //INF8 & INFB
#define	IPT_IGNCNTR_INDEX   1   //INF8 & INFB
#define	IPT_CATCOMP1_INDEX  2   //INF8
#define	IPT_HCCATCOMP_INDEX 2   //INFB
#define	IPT_CATCOND1_INDEX  3   //INF8
#define	IPT_HCCATCOND_INDEX 3   //INFB
#define	IPT_CATCOMP2_INDEX  4   //INF8
#define	IPT_NCATCOMP_INDEX  4   //INFB
#define	IPT_CATCOND2_INDEX  5   //INF8
#define	IPT_NCATCOND_INDEX  5   //INFB
#define	IPT_O2COMP1_INDEX   6   //INF8
#define	IPT_NADSCOMP_INDEX  6   //INFB
#define	IPT_O2COND1_INDEX   7   //INF8
#define	IPT_NADSCOND_INDEX  7   //INFB
#define	IPT_O2COMP2_INDEX   8   //INF8
#define	IPT_PMCOMP_INDEX    8   //INFB
#define	IPT_O2COND2_INDEX   9   //INF8
#define	IPT_PMCOND_INDEX    9   //INFB
#define	IPT_INF8_EGRCOMP_INDEX   10  //INF8
#define	IPT_EGSCOMP_INDEX   10  //INFB
#define	IPT_INF8_EGRCOND_INDEX   11  //INF8
#define	IPT_EGSCOND_INDEX   11  //INFB
#define	IPT_AIRCOMP_INDEX   12  //INF8
#define	IPT_INFB_EGRCOMP_INDEX   12  //INFB
#define	IPT_AIRCOND_INDEX   13  //INF8
#define	IPT_INFB_EGRCOND_INDEX   13  //INFB
#define	IPT_EVAPCOMP_INDEX  14  //INF8
#define	IPT_BPCOMP_INDEX    14  //INFB
#define	IPT_EVAPCOND_INDEX  15  //INF8
#define	IPT_BPCOND_INDEX    15  //INFB
#define	IPT_SO2SCOMP1_INDEX 16  //INF8
#define	IPT_SO2SCOND1_INDEX 17  //INF8
#define	IPT_SO2SCOMP2_INDEX 18  //INF8
#define	IPT_SO2SCOND2_INDEX 19  //INF8


/* SidRequest Flags */
#define SID_REQ_NORMAL                      0x00000000
#define SID_REQ_RETURN_AFTER_ALL_RESPONSES  0x00000001
#define SID_REQ_NO_PERIODIC_DISABLE         0x00000002
#define SID_REQ_ALLOW_NO_RESPONSE           0x00000004

/* LogMsg Flags */
#define LOG_NORMAL_MSG          0
#define LOG_REQ_MSG             1

/* Macro for handling the 'engineering test' capability */
#define ERROR_RETURN   {if(ErrorReturn()!=PASS)return(FAIL);}

#define ER_BYPASS_USER_PROMPT   0x00000001
#define ER_CONTINUE             0x00000002
#define ER_DONT_CLEAR_ERROR_COUNT 0x00000004

STATUS        ErrorReturn (void);
unsigned long ErrorCount (void);
unsigned long ErrorFlags (unsigned long newflags);
unsigned char TestContinue (char *);
unsigned char WarnContinue (char *);

/* Protocol list structure */
typedef struct
{
	unsigned long Protocol;
	unsigned long ChannelID;
	unsigned long InitFlags;
	unsigned long TesterPresentID;
	unsigned long FilterID;
	unsigned long FlowFilterID[OBD_MAX_ECUS];
	unsigned long HeaderSize;
	unsigned long BaudRate;
	char Name[20];
} PROTOCOL_LIST;

/* OBD message response data structures */
typedef struct
{
	unsigned char PID;
	unsigned char Data[21];
} SID1;

typedef struct
{
	unsigned char PID;
	unsigned char FrameNumber;
	unsigned char Data[4];
} SID2;

typedef struct
{
	unsigned char OBDMID;
	unsigned char SDTID;
	unsigned char UASID;
	unsigned char TVHI;
	unsigned char TVLO;
	unsigned char MINTLHI;
	unsigned char MINTLLO;
	unsigned char MAXTLHI;
	unsigned char MAXTLLO;
} SID6;

typedef struct
{
	unsigned char INF;
	unsigned char NumItems;
	unsigned char Data[4];
} SID9;


typedef struct
{
	unsigned char  INF;         // INF Type
	unsigned char  NODI;        // number data items
	unsigned short IPT[INF_TYPE_IPT_NODI];
	unsigned short Flags;       // application flags. set 1 when valid
} SID9IPT;

typedef struct
{
	unsigned char FirstID;
	unsigned char IDBits[4];
} ID_SUPPORT;

typedef struct
{
	unsigned char FirstID;
	unsigned char FrameNumber;
	unsigned char IDBits[4];
} FF_SUPPORT;

typedef struct
{
	unsigned char   Header[4];
	unsigned char   Sid1PidSupportSize;
	ID_SUPPORT      Sid1PidSupport[8];
	unsigned short  Sid1PidSize;
	unsigned char   Sid1Pid[32];
	
    unsigned char   Sid2PidSupportSize;
	FF_SUPPORT      Sid2PidSupport[8];
	unsigned short  Sid2PidSize;
	unsigned char   Sid2Pid[2048];
	
    BOOL            Sid3Supported;
    unsigned short  Sid3Size;
	unsigned char   Sid3[2048];
	
    unsigned short  Sid4Size;
	unsigned char   Sid4[8];
	
    unsigned char   Sid5TidSupportSize;
	ID_SUPPORT      Sid5TidSupport[8];
	unsigned short  Sid5TidSize;
	unsigned char   Sid5Tid[2048];
	
    unsigned char   Sid6MidSupportSize;
	ID_SUPPORT      Sid6MidSupport[8];
	unsigned short  Sid6MidSize;
	unsigned char   Sid6Mid[2048];
	
    BOOL            Sid7Supported;
    unsigned short  Sid7Size;
	unsigned char   Sid7[2048];
	
    unsigned char   Sid8TidSupportSize;
	ID_SUPPORT      Sid8TidSupport[8];
	unsigned short  Sid8TidSize;
	unsigned char   Sid8Tid[2048];
	
    unsigned char   Sid9InfSupportSize;
	ID_SUPPORT      Sid9InfSupport[8];
	unsigned short  Sid9InfSize;
	unsigned char   Sid9Inf[2048];
	
    BOOL            SidASupported;
    unsigned short  SidASize;
	unsigned char   SidA[2048];
} OBD_DATA;

/* Service ID (Mode) request structure */
typedef struct
{
	unsigned char SID;
	unsigned char NumIds;
	unsigned char Ids[8];
} SID_REQ;

typedef struct
{
	unsigned short Size;
	unsigned char  DTC[2048];
} DTC_LIST;

/* Local function prototypes */
STATUS TestWithNoDtc(void);
STATUS TestWithPendingDtc(void);
STATUS TestWithConfirmedDtc(void);
STATUS TestWithFaultRepaired(void);
STATUS TestWithNoFaultsAfter3DriveCycles(void);
STATUS TestToVerifyInUseCounters(void);
STATUS TestToVerifyPerformanceCounters(void);
STATUS FindJ2534Interface(void);
STATUS DetermineProtocol(void);
STATUS CheckMILLight(void);
STATUS SidRequest(SID_REQ *, unsigned long);
STATUS SidResetResponseData(PASSTHRU_MSG *);
STATUS SidSaveResponseData(PASSTHRU_MSG *, SID_REQ *);
STATUS ConnectProtocol(void);
STATUS DisconnectProtocol(void);
void StopTest(STATUS ExitCode);
void InitProtocolList(void);
int IsDTCPending(void);
int IsDTCStored(void);
void Log2534Error(char *, unsigned long, char *);
void LogPrint(const char *, ...);
void ScreenPrint(const char * fmt, ...);
void LogMsg(PASSTHRU_MSG *, unsigned long);
void LogMsgCopy (void);
void ClearLogMsgCopy (void);
char LogUserPrompt(char *, unsigned long);
STATUS ClearCodes(void);
STATUS VerifyMILData(void);
STATUS VerifyMonitorTestSupportAndResults(void);
STATUS VerifyReverseOrderSupport(void);
STATUS VerifyGroupDiagnosticSupport(void);
STATUS VerifyGroupMonitorTestSupport(void);
STATUS VerifyGroupFreezeFrameSupport(void);
STATUS VerifyGroupFreezeFrameResponse (void);
STATUS VerifyGroupControlSupport(void);
STATUS VerifyGroupVehicleInformationSupport(void);
STATUS VerifyDiagnosticSupportAndData(void);
STATUS VerifyDiagnosticBurstSupport(void);
STATUS VerifyFreezeFrameSupportAndData(void);
STATUS VerifyDTCStoredData(void);
STATUS VerifyDTCPendingData(void);
STATUS VerifyO2TestResults(void);
STATUS VerifyControlSupportAndData(void);
STATUS VerifyVehicleInformationSupportAndData(void);
STATUS VerifyLinkActive(void);					 //05/01/04
STATUS VerifyReverseGroupDiagnosticSupport(void);//05/01/04
STATUS VerifyIM_Ready (void);
STATUS VerifyINF8Data ( unsigned long  EcuIndex );
STATUS VerifyINFBData ( unsigned long  EcuIndex );
STATUS VerifyPermanentCodeSupport(void);
STATUS TestToVerifyPermanentCodes(void);
STATUS VerifyReservedServices(void);

STATUS VerifyEcuID (unsigned char EcuId[]);
STATUS LogSid9Ipt (void);

STATUS AppendLogFile (void);
STATUS VerifyVINFormat (void);

STATUS RequestSID1SupportData (void);
unsigned int IsSid1PidSupported (unsigned int EcuIndex, unsigned int PidIndex);

STATUS RequestSID9SupportData (void);
STATUS GetSid9IptData (unsigned int EcuIndex, SID9IPT * pSid9Ipt);
unsigned int IsSid9InfSupported (unsigned int EcuIndex, unsigned int InfIndex);

unsigned int GetEcuId (unsigned int EcuIndex);

STATUS StartPeriodicMsg (void);
STATUS StopPeriodicMsg (BOOL bLogError);

STATUS DetermineVariablePidSize (void);
void SaveDTCList (int nSID);

BOOL   ReadSid9IptFromLogFile ( const char * szTestSectionStart,
								const char * szTestSectionEnd,
								SID9IPT Sid9Ipt[]);

unsigned int IsSid2PidSupported (unsigned int EcuIndex, unsigned int PidIndex);
unsigned int IsSid8TidSupported (unsigned int EcuIndex, unsigned int TidIndex);

unsigned long LookupEcuIndex (PASSTHRU_MSG *RxMsg);

void LogVersionInformation (void);

void clear_keyboard_buffer (void);

STATUS VerifyCALIDFormat (unsigned long  EcuIndex, unsigned long  Inf3NumItems);

/* OBD type definitions */
//const char *OBD_TYPE[14] = {
extern const char *OBD_TYPE[14];

/* Global variables */
extern char gLogFileName[80];
extern char gUserModelYear[80];
extern int  gModelYear;
extern char gUserMake[80];
extern char gUserModel[80];
extern unsigned long gUserNumEcus;
extern unsigned char gOBDKeywords[2];
extern unsigned char gOBDFailureBypassed;
extern unsigned char gOBDTestFailed;
extern unsigned char gOBDDetermined;
extern unsigned long gOBDRequestDelay;
extern unsigned long gOBDMaxResponseTimeMsecs;
extern unsigned long gOBDMinResponseTimeMsecs; /* 6/7/04 - Added to verify min response time. */
extern unsigned long gOBDListIndex;
extern unsigned long gOBDFoundIndex;
extern PROTOCOL_LIST gOBDList[OBD_MAX_PROTOCOLS];
extern unsigned long gOBDNumEcus;
extern unsigned long gOBDNumEcusResp;          /* 06/17/04 - Added to pass number of responding cntrls to call routine */
extern unsigned char gOBDEngineRunning;
extern unsigned char gOBDEngineWarm;
extern unsigned char gOBDDTCPending;
extern unsigned char gOBDDTCStored;
extern unsigned char gOBDDTCHistorical;
extern unsigned char gOBDDTCPermanent;
extern unsigned char gOBDIMDriveCycle;
extern unsigned char gOBDDieselFlag;
extern unsigned char gOBDHybridFlag;
extern unsigned long gOBDProtocolOrder;
extern unsigned long gOBDMonitorCount;
extern unsigned long gOBDAggregateResponseTimeMsecs;
extern unsigned long gOBDAggregateResponses;
extern unsigned long gLastLogTime;
extern unsigned char gIgnoreNoResponse;
extern unsigned char gIgnoreUnsupported;
extern unsigned char gSuspendScreenOutput;
extern unsigned char gSuspendLogOutput;
extern OBD_DATA gOBDResponse[OBD_MAX_ECUS];
extern OBD_DATA gOBDCompareResponse[OBD_MAX_ECUS];
extern char gVIN[18];
extern FILE *ghLogFile;
extern PASSTHRU_MSG gTesterPresentMsg;
extern BOOL gService0ASupported;
extern BOOL gVerifyLink;

#define LOG_BUFFER_COPY_SIZE     16384
extern char gszLogBufferCopy[LOG_BUFFER_COPY_SIZE];
extern unsigned long gLogBufferCopyIndex;
extern long gSid1VariablePidSize;
char *gBanner;

/* char *gBanner;*/ /* By Honda */
extern unsigned long gOBDNumEcusCan;               /* by Honda */
extern unsigned char gOBDResponseTA[OBD_MAX_ECUS]; /* by Honda */

extern FILE *ghTempLogFile;
extern char gszTempLogFilename[MAX_PATH];

extern DTC_LIST DTCList[OBD_MAX_ECUS];

extern unsigned long DeviceID;

extern SID1 Sid1Pid1[OBD_MAX_ECUS];     // capture the response from SID01 PID01 response.

typedef enum {eTestNone=0, eTestNoDTC=5, eTestPendingDTC, eTestConfirmedDTC, eTestFaultRepaired,
			  eTestNoFault3DriveCycle, eTestInUseCounters, eTestPerformanceCounters} TEST_PHASE;

typedef enum {eMILBulbCheck=1, eEstCommIgnOnEngOff, eClearDTCsEngOff, eVerMILStatusEngOff,
			  eVerSID6EngOff, eVerSID1EngOff, eVerSID8EngOff, eEstCommEngRun, eClearDTCsEngRun,
			  eVerSID1EngRun, eVerSID2EngRun, eVerSID3EngRun, eVerSID5EngRun, eVerSID6EngRun,
			  eVerSID7EngRun, eVerSID8EngRun, eVerSID9EngRun, eVerSID1Rev, eVerSID1IdleMsgTim,
			  eVerSID1BurstMsgTim, } TEST_5_SUBSECTION;

extern TEST_PHASE  TestPhase;
extern unsigned char TestSubsection;    // test subsection to be used in conjunction with TestPhase
