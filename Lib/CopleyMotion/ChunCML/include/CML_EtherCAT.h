/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2002 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file

This header file defines the classes used to represent the
top level of the EtherCAT network interface.  

*/

#ifndef _DEF_INC_ETHERCAT
#define _DEF_INC_ETHERCAT

#include "CML_Settings.h"
#include "CML_Error.h"
#include "CML_Network.h"
#include "CML_Threads.h"
#include "CML_Utils.h"

CML_NAMESPACE_START()

/**
This class holds the error codes that describe EtherCAT
error conditions.
*/
class EtherCatError: public Error
{
public:
   /// Unable to start main EtherCAT 
   static const EtherCatError ThreadStart;

   /// EtherCAT hardware has not been initialized
   static const EtherCatError HardwareNotInit;

   /// Unable to open EtherCAT hardware
   static const EtherCatError OpenHardware;

   /// Error reading from Ethernet socket
   static const EtherCatError ReadHardware;

   /// Error writing to Ethernet socket
   static const EtherCatError WriteHardware;

   /// Remote device did not respond to request (working counter is zero).
   static const EtherCatError NoResponse;

   /// EtherCAT network object not initialized
   static const EtherCatError EcatNotInit;

   /// Specified node was not found on the EtherCAT network
   static const EtherCatError NodeNotFound;

   /// EtherCAT node is currently in boot mode.
   static const EtherCatError NodeBootMode;

   /// Error changing node operational state
   static const EtherCatError NodeStateChange;

   /// EtherCAT message received from network is corrupt.
   static const EtherCatError EcatMsgCorrupt;

   /// Not enough space in frame for new datagram
   static const EtherCatError DatagramWontFit;

   /// EtherCAT node returned an unknown mailbox error code
   static const EtherCatError MboxError;

   /// EtherCAT node reported the syntax of mailbox header is invalid
   static const EtherCatError MboxSyntax;

   /// EtherCAT node does not support the requested mailbox protocol
   static const EtherCatError MboxProtocol;

   /// EtherCAT node returned an invalide mailbox channel code
   static const EtherCatError MboxChannel;

   /// EtherCAT node does not support the requested mailbox service
   static const EtherCatError MboxService;

   /// EtherCAT node reported an invalid mailbox protocol header
   static const EtherCatError MboxHeader;

   /// EtherCAT node reported the length of the mailbox data is too short
   static const EtherCatError MboxTooShort;

   /// EtherCAT node reported insufficient memory for mailbox transfer
   static const EtherCatError MboxMemory;

   /// EtherCAT node reported inconsistent mailbox data length
   static const EtherCatError MboxSize;

   /// EtherCAT node returned incorrectly formatted FoE response
   static const EtherCatError FoEformat;

   /// EtherCAT node returned an FoE error response.
   static const EtherCatError FoEerror;

   /// EtherCAT node has not been initialized
   static const EtherCatError NodeNotInit;

   /// PDO is not currently enabled on network
   static const EtherCatError PdoNotEnabled;

   /// EtherCAT network is not correctly wired
   static const EtherCatError NetworkWiringError;

protected:
   EtherCatError( uint16 id, const char *desc ): Error( id, desc ){}
};

/***************************************************************************/
/**
Configuration object used to customize global settings for the EtherCAT
network.  An object of this type may be passed to the EtherCAT::Open()
method when the network is first opened.

If no settings object is passed to the EtherCAT::Open() method, 
then the behavior is exactly the same as passing a EtherCatSettings object
with the default settings.
*/
/***************************************************************************/
class EtherCatSettings
{
public:
   /// Default constructor for EtherCatSettings object.
   /// This sets all settings to their default values
   EtherCatSettings()
   {
      cycleThreadPriority = 9;
      readThreadPriority = 9;
      cyclePeriod = 1;
   }

   /// Defines the EtherCAT read thread priority.  The read thread is started
   /// when the EtherCAT object is first opened (using EtherCAT::Open()).
   /// This thread is responsible for reading Ethernet messages from the hardware
   /// and processing them as they arrive.
   /// It should be run at a relatively high priority.
   /// Default: 9
   int readThreadPriority;

   /// Defines the EtherCAT cycle thread priority.  The cycle thread is started
   /// when the EtherCAT object is first opened (using EtherCAT::Open()).
   /// This thread is responsible for polling the EtherCAT network at a set 
   /// frequency.  It should be run at a relatively high priority.
   /// Default: 9
   int cycleThreadPriority;

   /// EtherCAT cycle period.  This parameter defines the
   /// update rate at which the EtherCAT network is polled.
   /// Default: 1 ms.
   Timeout cyclePeriod;
};

/**
 * Low level Ethernet hardware interface.
 */
class EtherCatHardware: public RefObj
{
public:
   virtual ~EtherCatHardware() { KillRef();}
   virtual const Error *Open( void ) = 0;
   virtual const Error *Close( void ) = 0;
   virtual const Error *SendPacket( uchar *msg, uint16 len ) = 0;
   virtual const Error *RecvPacket( uchar *msg, uint16 *len, Timeout timeout=-1 ) = 0;
   virtual const Error *CloseSockets( void ){ return 0;}
};

/***************************************************************************/
/**
The EtherCAT class is the top level interface into the EtherCAT network.
There should be at least one object of this class in every EtherCAT based
application.
*/
/***************************************************************************/
class EtherCAT: public Network
{
public:
   EtherCAT( void );
   ~EtherCAT( void );

   const Error *Open( EtherCatHardware &hw );
   const Error *Open( EtherCatHardware &xmit, EtherCatHardware &recv );
   const Error *Open( EtherCatHardware &hw, EtherCatSettings &settings );
   const Error *Open( EtherCatHardware &xmit, EtherCatHardware &recv, EtherCatSettings &settings );
   const Error *Close( void );

   /// Return the network type.
   /// @return Always returns the value NET_TYPE_ETHERCAT
   NetworkType GetNetworkType( void ){ return NET_TYPE_ETHERCAT; }

   const Error *AttachNode( Node *n );
   const Error *DetachNode( Node *n );
   const Error *ResetNode( Node *n );
   const Error *ResetComm( Node *n );
   const Error *PreOpNode( Node *n );
   const Error *StartNode( Node *n );
   const Error *StopNode( Node *n );
   const Error *BootModeNode( Node *n );

   const Error *GetIdFromEEPROM( Node *n, struct NodeIdentity &id );

   const Error *XmitSDO( Node *n, uint8 *data, uint16 len, uint16 *ret, Timeout timeout=2000 );
   const Error *XmitPDO( class PDO *pdo, Timeout timeout=2000 );
   const Error *PdoDisable( Node *node, uint16 slot, PDO *pdo );
   const Error *PdoDisable( Node *node, uint16 slot, bool isTxPDO );
   const Error *PdoEnable( Node *node, uint16 slot, PDO *pdo );

   const Error *SetNodeGuard( Node *n, GuardProtocol type, Timeout timeout=200, uint8 life=0 );

   const Error *SetSync0Period( Node *n, uint32 ns );

   /// Return the number of nodes discovered on the network
   /// @return The number of nodes on the EtherCAT network
   uint16 getNodeCount( void ){ return nodeCt; }

   const Error *GetNodeAddress( Node *n, uint16 &addr );

   // File over EtherCAT support
   const Error *FoE_DnldStart( Node *n, const char *filename, uint32 password, Timeout to=2000 );
   const Error *FoE_DnldData( Node *n, int32 len, uint8 *buff, Timeout to=2000, bool end=true );
   int32 FoE_LastErrInfo( Node *n, char *msg, int maxMsg );
   
   int32 maxSdoToNode( Node *n );
   int32 maxSdoFromNode( Node *n );

   const Error *WaitCycleUpdate( Timeout to );

   Timeout GetCyclicPeriod( void ){ return settings.cyclePeriod; }

protected:
   const Error *InitDistClk( void );
   const Error *NodeRead( Node *n, int16 addr, int16 len, void *buff );
   const Error *NodeWrite( Node *n, int16 addr, int16 len, void *buff );
   const Error *NodeWrite( Node *n, int16 addr, int16 len, int32 value );
   const Error *SetNodeAlState( Node *n, uint16 state );
   const Error *WaitEEPROM( Node *n, Timeout to, uint16 &stat );
   const Error *NodeReadEEPROM( Node *n, int32 addr, int32 *ret );
   const Error *SendFrame( class EcatFrame *f, Timeout timeout=0, int maxRetry=2 );
   const Error *FoE_Dnld( Node *n, uint16 len, Timeout to );
   const Error *MailboxTransfer( Node *n, uint16 len, uint16 *ret, Timeout timeout=2000 );
   const Error *AddToFrame( class EcatFrame *frame, class EcatDgram *dg );

private:
   class EtherCatNodeInfo *GetEcatInfo( Node *n );

   uint32 xmitRef, recvRef;
   EtherCatSettings settings;
   Semaphore *stopSem;
   Mutex mtx;
   Mutex cyclicMutex;
   EventMap cyclicUpdate;

   bool readThreadRunning;
   bool cycThreadRunning;
   uint8 *mac;
   uint16 nodeCt;
   int32 *nodeAlias;
   uint32 *nodes;
   uint32 sentFrames[ CML_MAX_ECAT_FRAMES ];
   uint8 nextFrameIndex;
   uint32 nextFrameID;
   int refClkNode;

   class EcatCycleThread: public Thread
   {
   public:
      class EtherCAT *ecat;
      void run( void );
   } cycleThread;

   class EcatReadThread: public Thread
   {
   public:
      class EtherCAT *ecat;
      void run( void );
   } readThread;

   void CycleThreadFunc( void );
   void ReadThreadFunc( void );

   const Error *PollChange( Node *n, int16 addr, int16 len, void *buff );
   const Error *ConfigSyncMgr( Node *node, bool txPDO );
   void SaveFrameRef( EcatFrame *frame );
   EcatFrame *FindFrame( uint8 index, uint32 id );
   EcatFrame *FindFrame( uint16 len, uint8 *buff );
   void ReleaseFrameRef( EcatFrame *frame );
   const Error *FindSyncMgrCfg( Node *n, bool boot );
   const Error *CfgSyncMgr( Node *n, uint16 smReg, uint16 base, uint16 len, uint16 ctrl );
   void LoadPdoDat( uint32 ref, uint8 *buff, int max );

   friend class PDO_List;
   friend class RPDO_List;
   friend class EcatCycleThread;
   friend class EcatReadThread;
   friend class Linkage;
};

/* Generic EtherCAT datagram class.

   At the lowest levels, an EtherCAT packet is made up of one or more reads and/or writes
   to memory locations on the slave nodes.  Each of these memory accesses is called a
   datagram.

   There are several different types of datagrams; reads/writes to a node based on it's 
   location in the network, reads/writes to a node based on it's address, broadcast 
   accesses to all nodes on the network, etc.

   This class makes up the base for all datagrams.
*/
class EcatDgram
{
   // Local data area for short memory accesses to prevent excessive memory allocation
   uint8 local[4];

   // The command byte identifies the type of memory access (broadcast read, logical write, etc)
   uint8 cmd;

   // The index is a value set by the master and not changed by the slave nodes
   uint8 index;

   // The ADP field typically holds the node address of the slave on the network.
   int16 adp;

   // The ADO field typically holds the memory offset within the slave's address space
   int16 ado;

   // This gives the length of the data transfer in bytes
   int16 len;

   // Points to a memory location of at least len bytes where data will be stored
   uint8 *dat;

   // Points to the starting location of this datagram within an EtherCAT frame
   uint8 *framePtr;

   // Used to build a linked list of datagrams within an EtherCAT frame
   EcatDgram *next;

public:
   EcatDgram( void );
   EcatDgram( uint8 cmd, int16 adp, int16 ado, int16 len, void *ptr=0 );
   EcatDgram( uint8 cmd, int16 adp, int16 ado, int16 len, int32 val );
   virtual ~EcatDgram();
   void Init( uint8 cmd, int16 adp, int16 ado, int16 len, int32 val );
   void Init( uint8 cmd, int16 adp, int16 ado, int16 len, void *ptr=0 );
   void Reset( void );
   void setNext( EcatDgram *n );
   EcatDgram *getNext( void );
   virtual const Error *Load( void *buff, int16 &off );
   bool checkNdx( void );
   void setData( void *ptr );
   void setData( int32 val );
   void setNdx( uint8 ndx );
   uint8 getNdx(void);
   int16 getDgramLen( void );
   int16 getADP( void );
   int16 getADO( void );
   int16 getWKT( void );
   int16 getData16s( void );
   uint16 getData16u( void );
   int32 getData32s( void );
   uint32 getData32u( void );
   int16 getData( void *ptr, int16 max );
   void *getDataPtr( void );
   int16 getDataLen( void );
   virtual const Error *NewData( void );
};

// Broadcast read.  This type of datagram reads data from the same location on every node in the network.
// Returned data is ORed, so any bit set in any node will be set in the accumulated response.
// The ADP address is sent out from the master as zero and incremented by every slave that responds to 
// the read.
struct BRD: public EcatDgram
{
   BRD( int16 addr, int16 len ): EcatDgram( 7, 0, addr, len ){}
};

// Broadcast write.  This type of datagram writes data to the same location on every node in the network.
struct BWR: public EcatDgram
{
   BWR( int16 addr, int16 len, void *data ): EcatDgram( 8, 0, addr, len, data ){}
   BWR( int16 addr, int16 len, int32 data ): EcatDgram( 8, 0, addr, len, data ){}
};

// Read by position in network (aka Auto Increment Physical Read)
// The read is performed on the node who's position matches the passed address.  Pass 0 for the node
// closest to the master, 1 for the next node, etc.
// In the datagram sent out, the ADP field will hold -N for a passed address of N.  
// Each node increments the ADP value as it passes, and the node that receives a zero responds to the read.
struct APRD: public EcatDgram
{
   APRD( int16 pos, int16 addr, int16 len ): EcatDgram( 1, -pos, addr, len ){}
};

// Write by position in network (Auto Increment Physical Write)
// Like the APRD datagram, but a write version.
struct APWR: public EcatDgram
{
   APWR( int16 pos, int16 addr, int16 len, void *data ): EcatDgram( 2, -pos, addr, len, data ){}
   APWR( int16 pos, int16 addr, int16 len, int32 data ): EcatDgram( 2, -pos, addr, len, data ){}
};

// Read by position in network and write to the same address of all following nodes.
// The read is performed on the node who's position matches the passed address.  Pass 0 for the node
// closest to the master, 1 for the next node, etc.
// All nodes after that position will have the data read from the earlier node written to the same address
struct ARMW: public EcatDgram
{
   ARMW( int16 pos, int16 addr, int16 len ): EcatDgram( 13, -pos, addr, len ){}
};

// Read by assigned node ID (Configured Address Physical Read)
// The master assigns each node a unique 16-bit address at startup.  This datagram reads
// from memory locations within the slave based on these assigned addresses
struct FPRD: public EcatDgram
{
   FPRD( int16 node, int16 addr, int16 len ): EcatDgram( 4, node, addr, len ){}
};

// Write by assigned node ID (Configured Address Physical Write)
struct FPWR: public EcatDgram
{
   FPWR( int16 node, int16 addr, int16 len, void *dat ): EcatDgram( 5, node, addr, len, dat ){}
   FPWR( int16 node, int16 addr, int16 len, int32  dat ): EcatDgram( 5, node, addr, len, dat ){}
};

// Maximum size of an EtherCAT frame (based on Ethernet standard)
#define MAX_ECAT_DATA       1500
#define MAX_ECAT_FRAME      (MAX_ECAT_DATA+18)


// EtherCAT frame class.  This is used internally to represent a single frame of data
// sent over the EtherCAT network.  The frame consists of a header structure followed by a series
// of datagrams.
class EcatFrame: public RefObj
{
   // Source and destination MAC addresses.  These are part of the 
   // EtherCAT frame header
   uint8 srcMAC[6], dstMAC[6];

   // This datagram is used internally to track a frame on the network
   // This is a write to address 0 of the 65535th node on the network.
   // In the unlikely event that there actually are that many nodes, address 0
   // is always read only, so this shouldn't cause harm.
   APWR refDgram;

   // This points to the last datagram added to the frame
   EcatDgram *last;

   // Buffer used to hold the raw bytes of the frame
   uint8 buff[MAX_ECAT_FRAME];

   // Total size of the frame in bytes
   int16 size;


   Semaphore sem;
   static uint8 dgIndex;


public:

   EcatFrame( void );
   void Reset( void );
   void SetFrameID( uint8 index, uint32 id );
   uint32 GetFrameID( void );
   uint8 GetFrameIndex( void );
   static uint8 FindFrameID( uint16 len, uint8 *buff, uint32 &id );
   virtual ~EcatFrame();
   const Error *WaitResponse( Timeout timeout );
   int16 getSize( void );
   uint8 *getBuff( void );
   const Error *Add( EcatDgram *dg );
   virtual const Error *Process( uint8 *resp, int16 len );
   bool IsEmpty( void );
};


CML_NAMESPACE_END()

#endif
