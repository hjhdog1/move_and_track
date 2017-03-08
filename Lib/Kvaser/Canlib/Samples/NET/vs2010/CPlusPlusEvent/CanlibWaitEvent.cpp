#include "StdAfx.h"
#include "CanlibWaitEvent.h"

using namespace System;
using namespace CPlusPlusEvent;
using namespace Microsoft::Win32::SafeHandles;

CanlibWaitEvent::CanlibWaitEvent(Object^ we)
{
	unsigned int theHandle = (unsigned int)we;
	IntPtr pointer = IntPtr((Int64)theHandle) ;
	Microsoft::Win32::SafeHandles::SafeWaitHandle^ swHandle = 
		gcnew Microsoft::Win32::SafeHandles::SafeWaitHandle(pointer, true) ;
	SafeWaitHandle = swHandle;
}
