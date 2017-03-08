/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2002 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file

This header file defines the classes used for the top level of 
the CANopen network.  

*/

#ifndef _DEF_INC_CANOPEN
#define _DEF_INC_CANOPEN

#include "CML_Settings.h"
#include "CML_Can.h"
#include "CML_Error.h"
#include "CML_EventMap.h"
#include "CML_Network.h"
#include "CML_Threads.h"
#include "CML_Utils.h"

CML_NAMESPACE_START()

/**
This class holds the error codes that describe CANopen 
error conditions.
*/
class CanOpenError: public CanError
{
public:
   /// Indicates that the specified port name is invalid
   static const CanOpenError ThreadStart;

   /// One of the parameters passed to the CANopen function is invalid
   static const CanOpenError BadParam;

   /// The SDO object is busy
   static const CanOpenError SDO_Busy;

   /// The SDO up/download failed with a timeout
   static const CanOpenError SDO_Timeout;

   /// Some unknown error occurred
   static const CanOpenError SDO_Unknown;

   /// The mux (index/sub-index) received in a SDO message
   /// is inconsistent with the object being accessed.
   static const CanOpenError SDO_BadMuxRcvd;

   /// An improperly formatted SDO message was received.
   static const CanOpenError SDO_BadMsgRcvd;

   /// An illegal node ID was specified
   static const CanOpenError BadNodeID;

   /// The object being used has not been initialized.
   /// This error indicates a coding error, i.e. trying
   /// to use a Receiver object without initializing it.
   static const CanOpenError NotInitialized;

   /// An attempt was made to initialize an object that
   /// has already been initialized, and doesn't allow
   /// multiple initialization.
   static const CanOpenError Initialized;

   /// The requested feature is not supported by this node
   static const CanOpenError NotSupported;

   /// Monitor already running - An attempt is made to start
   /// the heartbeat or node guarding and it's already running.
   static const CanOpenError MonitorRunning;

   /// The node returned an illegal field count for the object
   /// being requested in it's object dictionary.
   static const CanOpenError IllegalFieldCt;

   /// An attempt was made to disable a receiver that wasn't enabled
   static const CanOpenError RcvrNotFound;

   /// An attempt was made to enable a receiver that was already enabled
   static const CanOpenError RcvrPresent;

   /// The CANopen port is closed
   static const CanOpenError Closed;

protected:
   CanOpenError( uint16 id, const char *desc ): CanError( id, desc ){}
};

/***************************************************************************/
/**
Configuration object used to customize global settings for the CANopen
network.  An object of this type may be passed to the CanOpen::Open()
method when the network is first opened.

If no CanOpenSettings object is passed to the CanOpen::Open() method, 
then the behavior is exactly the same as passing a CanOpenSettings object
with the default settings.
*/
/***************************************************************************/
class CanOpenSettings
{
public:
   CanOpenSettings();

   /// Defines the read thread priority.  The read thread is started
   /// when the CanOpen object is first opened (using CanOpen::Open()).
   /// This thread is responsible for reading messages from the
   /// CANopen network and calling the message handlers associated with
   /// them.  It should be run at a relatively high priority.
   /// Default: 9
   int readThreadPriority;
};

/***************************************************************************/
/**
The CanOpenNodeInfo structure holds some data required by the CANopen network
interface which is present in every node it manages.  The contents of this
structure should be considered the private property of the CANopen class.
*/
/***************************************************************************/
struct CanOpenNodeInfo: public NetworkNodeInfo
{
   // Pointer to the containing node.
   Node *me;

   // Pointers used to create a linked list of nodes
   CanOpenNodeInfo *next, *prev;

   /// Time (milliseconds) when next guarding event is due
   uint32 eventTime;

   /// This value keeps track of the toggle bit used with 
   /// node guarding.  It's set to -1 if the toggle bit isn't
   /// used.
   int16 guardToggle;

   /// This variable gives the guard time in milliseconds.
   /// It's used both in heartbeat and node guarding modes.
   int32 guardTimeout;

   /// Max number of guard timeouts before an error is generated
   uint8 lifeTime;

   /// Counter used to track guard timeouts
   uint8 lifeCounter;

   /// This variable identifies the type of guard protocol
   /// being used by this node.  The options are none, heartbeat
   /// monitor, and node guarding.
   GuardProtocol guardType;

   /// State I'm waiting for
   NodeState desired;

   /// Pointer to semaphore used when waiting for state change
   Semaphore *semPtr;

   /// Pointer to semaphore used when waiting for an SDO response
   Semaphore *sdoSemPtr;

   /// Pointer to SDO data buffer
   uint8 *sdoBuff;

   CanOpenNodeInfo( Node *node );
   ~CanOpenNodeInfo( void );
   void Unlink( void );
};

/***************************************************************************/
/**
The CanOpen class is the top level interface into the CANopen network.
There should be at least one object of this class in every CANopen based
application.  Normally, only one object will be necessary, however if more
then one independent CANopen network is in use, then more then one object
will be necessary.

On startup, a low level CAN interface object should be created.  This 
object should be passed to the CANopen object's Open() method.
*/
/***************************************************************************/
class CanOpen: public Network, public Thread
{
public:
   CanOpen( void );
   virtual ~CanOpen( void );
   const Error *Open( CanInterface &can );
   const Error *Open( CanInterface &can, CanOpenSettings &settings );
   void Close( void );

   /// Return the network type.
   /// @return Always returns the value NET_TYPE_CANOPEN
   NetworkType GetNetworkType( void ){ return NET_TYPE_CANOPEN; }

   const Error *AttachNode( Node *n );
   const Error *DetachNode( Node *n );

   const Error *SetNodeGuard( Node *n, GuardProtocol type, Timeout timeout=50, uint8 life=3 );

   const Error *ResetNode( Node *n );
   const Error *ResetComm( Node *n );
   const Error *PreOpNode( Node *n );
   const Error *StartNode( Node *n );
   const Error *StopNode( Node *n );
   const Error *BootModeNode( Node *n );

   const Error *Xmit( CanFrame &frame, Timeout timeout=2000 );

   const Error *XmitSDO( Node *n, uint8 *data, uint16 len, uint16 *ret, Timeout timeout=2000 );
   const Error *XmitPDO( class PDO *pdo, Timeout timeout=2000 );

   const Error *PdoEnable( Node *node, uint16 slot, PDO *pdo );
   const Error *PdoDisable( Node *node, uint16 slot, PDO *pdo );
   const Error *PdoDisable( Node *node, uint16 slot, bool isTxPDO );

   /// Return the node ID of the synch producer for this network.
   /// @return The synch producer node ID, or 0 if no synch producer
   /// has been registered.
   int16 GetSynchProducer( void ){
      return synchProducer;
   }

   /// Set the node ID of the synch producer for this network.
   /// @param nodeID The new synch producer node ID, or 0 for none.
   void SetSynchProducer( int16 nodeID ){
      synchProducer = nodeID;
   }

   /// Return the number of error frames received over then CAN network
   /// since the last time the counter was cleared
   /// @return The number of error frames received since the last call
   /// to CanOpen::ClearErrorFrameCounter();
   int32 GetErrorFrameCounter( void ){
      return errorFrameCt;
   }

   /// Clear the error frame counter
   void ClearErrorFrameCounter( void ){
      errorFrameCt = 0;
   }

   const Error *EnableReceiver( uint32 canMsgID, class Receiver *rcvr );
   const Error *DisableReceiver( uint32 canMsgID );

private:
   const Error *NMT_Msg( int code, int nodeID );
   void HandleNmtFrame( CanFrame &frame, Node *n );
   const Error *WaitNodeState( Node *n, NodeState state, Timeout timeout );
   CanOpenNodeInfo *GetCoInfo( Node *n );

   class coHashEntry **searchHash( uint32 id );
   uint32 LookupReceiver( uint32 canMsgID );
   void ClearHash( void );

   /// This hash is used to keep track of the Receiver
   /// objects that are enabled.
   class coHashEntry *hash[ CML_HASH_SIZE ];
   Mutex hashMtx;

   /// This array keeps track of all node objects associated 
   /// with this CANopen network
   uint32 nodes[128];

   /// Mutex used to control access to local data
   Mutex mtx;

   /// Reference to the can interface object passed during construction
   uint32 canRef;

   /// This variable is used to determine if a synch message producer
   /// has been assigned to this network.
   int16 synchProducer;

   /// This variable is used to keep a count of CAN error frames for
   /// diagnostic purposes.
   int32 errorFrameCt;

   /// Local thread used to manage node guarding & heartbeat messages 
   /// for nodes on this network.
   class NodeGuardThread: public CanOpenNodeInfo, public Thread
   {
      CanOpen *co;
      Semaphore sem;
      Mutex mtx;
   public:
      NodeGuardThread( void );
      void SetCo( CanOpen *coPtr ){ co = coPtr; }
      const Error *SetNodeGuard( CanOpenNodeInfo *ni, GuardProtocol type, int32 timeout, uint8 life );
      void HandleNMT( CanFrame &frame, CanOpenNodeInfo *ni );
      void AddNode( CanOpenNodeInfo *ni, uint32 time );
      void run( void );
   } guard;

   void run( void );
};

/***************************************************************************/
/**
CANopen receiver object.  This class allows the programmer to create routines
that are run whenever a CAN frame with a specific ID is received.  

To run code when a message is received, create a new class that extends
Receiver.  The Receiver::Init() function should be called with the CAN message ID
of the frames to be received.  Whenever this new class is Enabled(), the
member function NewFrame() will be called once for every frame received with
a matching ID.
*/
/***************************************************************************/
class Receiver: public RefObj
{
public:
   Receiver();
//   Receiver( CanOpen &canOpen, uint32 id );
   virtual ~Receiver();

//   const Error *Init( CanOpen &canOpen, uint32 id );

   virtual int NewFrame( CanFrame &frame );

   /// Return a reference to the CanOpen object 
   /// associated with this receiver
   /// @return The CanOpen object 
//   CanOpen &GetCanOpen(void){
//      return *co;
//   }

   /// Return true if this receiver has been initialized
   /// @return True if initialized
//   bool IsInitialized( void )
//   {
//      return co != 0;
//   }

protected:
//   CanOpen *co;

private:
//   uint32 id;
//   Receiver *next;

   /// Private copy constructor (not supported)
   Receiver( const Receiver& );

   /// Private assignment operator (not supported)
   Receiver& operator=( const Receiver& );
};


/***************************************************************************/
/**
CANopen Layer Setting Services object.

The Layer Setting Services (LSS) protocol is part of the CANopen network 
standard.  The intent of LSS is to allow low level network settings, such
as the network bit rate and device node ID numbers to be configured over 
the network.

The CANopen protocol requires each device on the network to have a unique
node ID number in the range 1 to 127.  In general, it's not possible to 
communicate with a device using CANopen if it doesn't have a unique node
ID in this range.

The LSS protocol allows some limited communication with any device on the 
network even if it doesn't have a node ID set.  This allows node ID numbers
to be assigned to devices over the network.

This object implements the LSS protocol and allows devices on the network
to be queried and configured.  

For more detailed information on the LSS protocol please see the CANopen 
standard document DSP305.
*/
/***************************************************************************/
class LSS: public Receiver
{
   Semaphore sem;
   Timeout to;
   int max, tot;
   uint8 recvCS;
   uint32 *serial;
   uint32 recvData;
   uint32 coRef;
public:
   LSS( CanOpen &co );
   ~LSS();
   int FindAmplifiers( int max, uint32 serial[] );

   /// Set the timeout value used by the LSS protocol.
   /// @param to The new timeout (milliseconds)
   void setTimeout( Timeout to ){ this->to = to; }

   /// Get the current timeout value used by the LSS protocol
   /// @return The current timeout in milliseconds.
   Timeout getTimeout( void ){ return to; }

   const Error *GetAmpNodeID( uint32 serial, byte &nodeID );
   const Error *SetAmpNodeID( uint32 serial, byte nodeID );
protected:
   const Error *SelectAmp( uint32 serial );
   uint32 FindAmpSerial( uint32 low, uint32 high );
   int NewFrame( CanFrame &frame );
   const Error *Xmit( byte cs, uint32 data=0 );
private:
   /// Private copy constructor (not supported)
   LSS( const LSS& );

   /// Private assignment operator (not supported)
   LSS& operator=( const LSS& );
};

CML_NAMESPACE_END()

#endif

