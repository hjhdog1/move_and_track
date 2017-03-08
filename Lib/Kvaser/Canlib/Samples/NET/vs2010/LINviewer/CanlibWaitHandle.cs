using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Microsoft.Win32.SafeHandles;

namespace LinViewer
{
   class CanlibWaitHandle : WaitHandle
   {
      public CanlibWaitHandle(object we)
      {
          uint theHandle = (uint) we;
          IntPtr pointer = new IntPtr((long) theHandle);
          SafeWaitHandle swHandle = new SafeWaitHandle(pointer, true);
          base.SafeWaitHandle = swHandle;
      } // constructor
   } // CanlibWaitHandle class
} // LinViewer namespace
