/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2002 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file

This header file defines the classes used for the generic top
level network interface.

*/

#ifndef _DEF_INC_NETWORK
#define _DEF_INC_NETWORK

#include "CML_Settings.h"
#include "CML_Can.h"
#include "CML_Error.h"
#include "CML_Reference.h"

CML_NAMESPACE_START()

// Forward references
class Node;
class PDO;

/**
This class holds the error codes that describe various
Netowrk error conditions.
*/
class NetworkError: public Error
{
public:
   /// A node with the specified ID is already present on the network
   static const NetworkError NodeIdUsed;

protected:
   NetworkError( uint16 id, const char *desc ): Error( id, desc ){}
};

/**
Enumeration used to identify a network node state.
*/
enum NodeState
{
   /// Invalid node state
   NODESTATE_INVALID,

   /// Unknown state - the default state on node creation.
   /// The state will be changed when communication with
   /// the node is established.
   NODESTATE_UNKNOWN,

   /// On a node guarding or heartbeat timeout, the state
   /// will change to guard error.
   NODESTATE_GUARDERR,

   /// Stopped state (aka init state for EtherCAT)
   NODESTATE_STOPPED,

   /// Pre-operational state
   NODESTATE_PRE_OP,

   /// Operational state
   NODESTATE_OPERATIONAL,

   /// Safe-operational state
   NODESTATE_SAFE_OP,

   // Boot state
   NODESTATE_BOOT
};

/**
Enumeration used to identify a type of network architecture.
*/
enum NetworkType
{
   /// CANopen network
   NET_TYPE_CANOPEN,

   /// EtherCAT network
   NET_TYPE_ETHERCAT,

   /// Invalid network type
   NET_TYPE_INVALID
};

/**
Private data owned by the network object attached
to every node.
*/
class NetworkNodeInfo
{
public:
   virtual ~NetworkNodeInfo(){}
};

/**
Enumeration used to identify the various types of node
guarding protocols.
*/
enum GuardProtocol 
{
   /// No guarding protocol is in use
   GUARDTYPE_NONE,

   /// The heartbeat protocol is being used
   GUARDTYPE_HEARTBEAT,

   /// Node guarding protocol is being used
   GUARDTYPE_NODEGUARD
};


/**
 * Abstract network class.  This class forms the root of all the
 * different networks that are supported by CML.  Every device managed
 * by CML is associated with exactly one network object.  The network
 * object manages some high level aspects of the communications with 
 * the device.
 */
class Network: public RefObj
{
public:
   Network( void ){}
   virtual ~Network(){}
   virtual NetworkType GetNetworkType( void ) = 0;
   virtual const Error *AttachNode( Node *n ) = 0;
   virtual const Error *DetachNode( Node *n ) = 0;
   virtual const Error *ResetNode( Node *n ) = 0;
   virtual const Error *ResetComm( Node *n ) = 0;
   virtual const Error *PreOpNode( Node *n ) = 0;
   virtual const Error *StartNode( Node *n ) = 0;
   virtual const Error *StopNode( Node *n ) = 0;
   virtual const Error *BootModeNode( Node *n ) = 0;

   virtual const Error *XmitSDO( Node *n, uint8 *data, uint16 len, uint16 *ret, Timeout timeout=2000 ) = 0;
   virtual const Error *XmitPDO( class PDO *pdo, Timeout timeout=2000 ) = 0;
   virtual const Error *PdoEnable( Node *node, uint16 slot, PDO *pdo ) = 0;
   virtual const Error *PdoDisable( Node *node, uint16 slot, PDO *pdo ) = 0;
   virtual const Error *PdoDisable( Node *node, uint16 slot, bool isTxPDO ) = 0;
   virtual const Error *SetNodeGuard( Node *n, GuardProtocol type, Timeout timeout=50, uint8 life=3 ) = 0;

   virtual int32 maxSdoToNode( Node *n );
   virtual int32 maxSdoFromNode( Node *n );

protected:
   static NetworkNodeInfo *GetNodeInfo( Node *n );
   static void SetNodeInfo( Node *n, NetworkNodeInfo *ni );
};

CML_NAMESPACE_END()

#endif

