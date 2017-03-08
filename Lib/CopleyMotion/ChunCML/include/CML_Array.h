/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2011 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file
This file implements a simple dynamic array template used in CML.
*/


#ifndef _DEF_INC_ARRAY
#define _DEF_INC_ARRAY

#include "CML_Settings.h"

CML_NAMESPACE_START()

template <class C> class Array
{
private:
   int max;
   int len;
   int step;
   C *dat;

   void CheckSize( int l )
   {
      if( l <= max )
         return;

      if( step )
         max += step;
      else
         max *= 2;

      if( max < l ) max = l;
      if( len > max ) len = max;

      C *dd = new C[max];
      for( int i=0; i<len; i++ )
         dd[i] = dat[i];

      delete[] dat;
      dat = dd;
   }

public:

   /**
      Default constructor for a dynamic array.
      @param init Number of objects to initially allocate space for
      @param step When new data is needed, the array size will be increased by this amount
                  If this parameter is negative (default), then the initial value will be used.
                  If this parameter is zero, then the array size will be doubled on each increase.
   */
   Array( int init=32, int step=-1 )
   {
      if( init <= 0 )
      {
         init = 0;
         dat = 0;
      }
      else
         dat = new C[init];

      if( step < 0 )
         step = init ? init : 1;

      this->max  = init;
      this->step = step;
      this->len = 0;
   }

   virtual ~Array()
   {
      delete[] dat;
      max = len = 0;
   }

   int length(void){ return len; }

   /// Return a reference to the item at the specified index.
   C& operator [](int ndx)
   {
      CML_ASSERT( (ndx>=0) && (ndx<len) );
      return dat[ndx];
   }

   /// Append a value to the end of the array
   void add( C val )
   {
      CheckSize(len+1);
      dat[len++] = val;
   }

   // Remove the element at the specified index.  The array will be compacted
   void rem( int ndx )
   {
      len--;
      for( int i=ndx; i<len; i++ )
         dat[i] = dat[i+1];
   }
};

CML_NAMESPACE_END()

#endif

