/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2002 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** 
\file

This header file defines a set of classes used to handle 
reference counting within the CML library.

*/

#ifndef _DEF_INC_REFERENCE
#define _DEF_INC_REFERENCE

#include "CML_Settings.h"
#include "CML_Threads.h"

CML_NAMESPACE_START()

/**
This class is used to track object references in the CML library.
Most CML objects use the RefObj class as a base class.  This class assignes
an integer ID to the object which can be used as a safer alternative to keeping
a pointer to the object.
*/
class RefObj
{
   int32 refID;
   const char *name;

protected:
   void KillRef( void );

public:
   RefObj( const char *name=0 );
   ~RefObj();

   void SetRefName( const char *name );
   uint32 GrabRef( void );
   static void ReleaseRef( uint32 id );
   static RefObj *LockRef( uint32 id );
   void UnlockRef( void );
   uint32 RefID( void ){ return refID+1; }

   static void LogRefs( void );
};

/***************************************************************************/
/**
This is a utility class that locks a reference in it's constructor, and unlocks 
it in it's destructor.  It can be used to ensure that a reference is properly 
unlocked when a function returns.
*/
/***************************************************************************/
template<class RefClass> class RefObjLocker
{
   uint32 ref;
   RefClass *ptr;

public:
   /// Lock the passed reference
   /// @param r The reference to lock
   RefObjLocker( uint32 r )
   {
      ref = r;
      if( r )
	 ptr = (RefClass*)RefObj::LockRef(r);
      else
	 ptr = 0;
   }

   /// Unlock the reference
   ~RefObjLocker()
   {
      if( ptr )
	 ptr->UnlockRef();
   }

   /// Return a pointer to the referenced object
   /// @return a pointer to the locked reference object
   RefClass *operator->( void )
   {
      return ptr;
   }

   /// Return a C++ reference to the referenced object
   /// @return a C++ reference to the locked reference object
   RefClass &operator*( void )
   {
      return *ptr;
   }

   operator RefClass *( void )
   {
      return ptr;
   }
};

CML_NAMESPACE_END()

#endif
