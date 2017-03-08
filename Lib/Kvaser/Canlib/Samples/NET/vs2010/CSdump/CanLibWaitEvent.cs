using System;
using System.Threading;
using Microsoft.Win32.SafeHandles;

namespace CSdump
{
  /// <summary>
  /// This object wraps the wait event.
  /// </summary>
  class CanLibWaitEvent : WaitHandle
  {
    /// <summary>
    /// 
    /// </summary>
    /// <param name="we"></param>
    public CanLibWaitEvent(object we)
    {
      SafeWaitHandle swHandle = new SafeWaitHandle(/*pointer*/ (IntPtr)we, true);
      base.SafeWaitHandle = swHandle;
    }

    ~CanLibWaitEvent()
    {
      base.SafeWaitHandle.SetHandleAsInvalid();
    }
  }
}
