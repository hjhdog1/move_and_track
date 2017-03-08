Friend Class CanlibWaitEvent
    Inherits System.Threading.WaitHandle

    Public Sub New(ByVal we As Object)
        Dim theHandle As UInt32 = DirectCast(we, UInt32)
        Dim pointer As New IntPtr(CLng(theHandle))
        Dim swHandle As New Microsoft.Win32.SafeHandles.SafeWaitHandle(pointer, True)
        MyBase.SafeWaitHandle = swHandle
    End Sub

End Class
