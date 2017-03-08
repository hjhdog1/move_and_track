/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2002 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file

This header file defines the classes that define a generic
node on the network.

*/

#ifndef _DEF_INC_CO_NODE
#define _DEF_INC_CO_NODE

#include "CML_Settings.h"
#include "CML_CanOpen.h"
#include "CML_EtherCAT.h"
#include "CML_EventMap.h"
#include "CML_Network.h"
#include "CML_Reference.h"
#include "CML_SDO.h"
#include "CML_PDO.h"

CML_NAMESPACE_START()

/**
This class represents node errors. 
There is one static member for each defined node error.
*/
class NodeError: public Error
{
public:
   /// A node guarding or heartbeat timeout occurred
   static const NodeError GuardTimeout;

   /// The network this node is connected to has been deleted
   static const NodeError NetworkUnavailable;

protected:
   /// Standard protected constructor
   NodeError( uint16 id, const char *desc ): Error( id, desc ){}
};

/***************************************************************************/
/**
CANopen identity object.  Each node is required to include an identity 
object on it's object dictionary at location 0x1018.  The only required
parameter is the vendorID.  All others are included at the manufacturer's 
discretion.
*/
/***************************************************************************/
struct NodeIdentity
{
   /// A unique vendor ID assigned by CiA (Can in Automation)
   uint32 vendorID;
   /// Manufacturer's product code
   uint32 productCode;
   /// Revision number which identifies CANopen functionality
   uint32 revision;
   /// Product serial number
   uint32 serial;
};

/***************************************************************************/
/**
Node class.  Objects of this class represent individual nodes on 
the CANopen or EtherCAT network.
*/
/***************************************************************************/
class Node: public RefObj
{
   /// Private copy constructor (not supported)
   Node( const Node& );

   /// Private assignment operator (not supported)
   Node& operator=( const Node& );

public:
   /// This SDO may be used to get/set values in the node's
   /// object dictionary.
   SDO sdo;

   Node();
   Node( Network &net, int16 nodeID );
   virtual ~Node();

   virtual const Error *StopGuarding( void );
   virtual const Error *StartHeartbeat( uint16 period, uint16 timeout );
   virtual const Error *StartNodeGuard( uint16 guardTime, byte lifeFactor );

   virtual const Error *Init( Network &co, int16 nodeID );
   virtual const Error *UnInit( void );
   virtual NetworkType GetNetworkType( void );
   virtual uint32 GetNetworkRef( void );

   virtual const Error *PdoSet( uint16 n, PDO &pdo, bool enable=true );

   virtual const Error *PdoEnable( uint16 n, PDO &pdo );
   virtual const Error *PdoDisable( uint16 n, PDO &pdo );
   virtual const Error *RpdoDisable( uint16 n );
   virtual const Error *TpdoDisable( uint16 n );

   virtual const Error *StartNode( void );
   virtual const Error *StopNode( void );
   virtual const Error *PreOpNode( void );
   virtual const Error *ResetNode( void );
   virtual const Error *ResetComm( void );

   /// Returns the present state of this node.
   /// Note that this requires node guarding or heartbeats to
   /// be enabled.
   /// @return The present node state.
   virtual NodeState GetState( void ){ return state; }

   /// Read the device type from the object dictionary
   /// @param devType Where the device type is returned
   /// @return An error object
   virtual const Error *GetDeviceType( uint32 &devType ){
      return sdo.Upld32( 0x1000, 0, devType );
   }

   /// Read the error register from the object dictionary
   /// @param err Reference to where the error should be returned.
   /// @return An error object
   virtual const Error *GetErrorRegister( byte &err ){
      return sdo.Upld8( 0x1001, 0, err );
   }

   /// Read the manufacturer status register from the object dictionary.
   /// @param stat Reference to the int32 where the status will be returned
   /// @return An error object
   virtual const Error *GetMfgStatus( uint32 &stat ){
      return sdo.Upld32( 0x1002, 0, stat );
   }

   virtual const Error *GetErrorHistory( uint16 &ct, uint32 *array );

   /// Clear the error history (object 0x1003) array for this node.
   /// @return An error object.
   virtual const Error *ClearErrorHistory( void ){
      return sdo.Dnld32( 0x1003, 0, (uint32)0 );
   }

   /// Read the manufacturer's device name string from the object dictionary
   /// @param len Holds the size of the buffer on entry, and the
   ///            length of the downloaded data on return.
   /// @param str An array of characters used to upload the string.
   /// @return An error object
   virtual const Error *GetMfgDeviceName( int32 &len, char *str ){
      return sdo.UpldString( 0x1008, 0, len, str );
   }

   /// Read the manufacturer's Hardware version string from the object dictionary
   /// @param len Holds the size of the buffer on entry, and the
   ///            length of the downloaded data on return.
   /// @param str An array of characters used to upload the string.
   /// @return An error object
   virtual const Error *GetMfgHardwareVer( int32 &len, char *str ){
      return sdo.UpldString( 0x1009, 0, len, str );
   }

   /// Read the manufacturer's software version string from the object dictionary
   /// @param len Holds the size of the buffer on entry, and the
   ///            length of the downloaded data on return.
   /// @param str An array of characters used to upload the string.
   /// @return An error object
   virtual const Error *GetMfgSoftwareVer( int32 &len, char *str ){
      return sdo.UpldString( 0x100A, 0, len, str );
   }

   virtual const Error *GetIdentity( NodeIdentity &id );

   /// Set the COB-ID of the synch message.  If bit 30 of the
   /// ID is set, then this node will be the synch producer.
   /// @param id COB-ID to set
   /// @return An error object
   virtual const Error *SetSynchId( uint32 id ){
      return sdo.Dnld32( 0x1005, 0, id );
   }

   /// Return the COB-ID of the synch message.  Note that if
   /// this node is producing the synch message, bit 30 will be set.
   /// @param id Where the COB-ID is returned
   /// @return An error object.
   virtual const Error *GetSynchId( uint32 &id ){
      return sdo.Upld32( 0x1005, 0, id );
   }

   /// Set the SYNC message interval in microseconds.
   /// @param per The period in microseconds.
   /// @return An error object.
   virtual const Error *SetSynchPeriod( uint32 per ){
      return sdo.Dnld32( 0x1006, 0, per );
   }

   /// Get the SYNC message interval in microseconds.
   /// @param per Period will be returned here
   /// @return An error object.
   virtual const Error *GetSynchPeriod( uint32 &per ){
      return sdo.Upld32( 0x1006, 0, per );
   }

   virtual const Error *SynchStart( void );
   virtual const Error *SynchStop( void );

   /// Return the node ID associated with this node
   /// @return The node ID
   virtual int16 GetNodeID( void ){
      return nodeID;
   }

   /// Return true if this node object has been initialized
   bool IsInitialized( void ){ return netRef != 0; }

   virtual void SetState( NodeState newState );

   virtual int32 maxSdoToNode( void );
   virtual int32 maxSdoFromNode( void );

protected:
   /// Overload this function to handle emergency objects sent
   /// by this node.
   virtual void HandleEmergency( CanFrame &frame ){}

   /// Overload this function to handle changes to the nodes
   /// state.  Note that the state member variable will have
   /// been changed to the new state before this function is
   /// called.
   virtual void HandleStateChange( NodeState from, NodeState to ){}

private:

   /// The NodeID for this node.
   int16 nodeID;

   /// The network this node is attached to
   uint32 netRef;

   /// Holds the current state of this node.
   NodeState state;

   /// General purpose mutex for this node.
   Mutex mtx;

   /// Data owned by the network object
   class NetworkNodeInfo *nodeInfo;

   friend class Network;
   friend class CanOpen;
};

CML_NAMESPACE_END()

#endif

