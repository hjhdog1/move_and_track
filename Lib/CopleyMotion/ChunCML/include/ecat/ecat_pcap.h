#ifndef _DEF_INC_ECAT_PCAP
#define _DEF_INC_ECAT_PCAP

#include "CML_EtherCAT.h"

CML_NAMESPACE_START()

class PcapEcatHardware: public EtherCatHardware
{
   void *hndl;
   const char *ifname;
   void *devList;
   const Error *FindDevNames( void );
public:
   PcapEcatHardware( const char *name=0 );
   virtual ~PcapEcatHardware( void );
   const Error *Open( void );
   const Error *Close( void );
   const Error *SendPacket( uchar *msg, uint16 len );
   const Error *RecvPacket( uchar *msg, uint16 *len, Timeout timeout=-1 );
   const char *GetAdapterName( int index );
   const char *GetAdapterDesc( int index );
};

CML_NAMESPACE_END()

#endif
