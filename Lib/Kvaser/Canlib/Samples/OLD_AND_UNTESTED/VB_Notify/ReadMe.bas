Attribute VB_Name = "ReadMe"
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' How to use the CAN Event Notification from CANLIB within Visual Basic
' ---------------------------------------------------------------------
'
' *** This demo may only be copied together with this information file! ***
'
' This example demonstrates how you can use the event notification
' functions from the Kvaser CANLIB within Visual Basic. This example
' is not functional complete, so it does not compile! It only shows
' a little trick to get the notifications.
'
' This code fragment was made by:
'      Hasse Muller of
'      Ordina Technical Automation B.V.
'      Deventer, The Netherlands
'
' It was made during development of a CAN based (automotive) production line for:
'      Power-Packer Europa B.V.
'      Oldenzaal, The Netherlands
'
' This fragment is provided 'as-is' and no rights can be obtained from it. Usage
' is on your own risk! Please also understand the risk of the tricks shown in this
' example. By not fully understanding how this example works, your system can
' become very unstable.
'
' If you have any questions you may contact Ordina Technical Automation B.V. in
' the Netherlands. Telephone: (+31)570-504500, Fax: (+31)570-504591. The author
' of this example is Ing. J.C. Muller. e-mail: hasse.muller@ordina.nl
'
'
'
' This example consists of the following files:
' -CANLIB32.bas  ' Include file from Kvaser's CANLIB
' -Readme.bas    ' This file!
' -Notify.bas    ' Notification Module
' -CNotify.cls   ' Notification Class
' -INotify.cls   ' Notification Interface
' -frmNotify     ' Notification Form
'
'
' How does it work?
' -----------------
' The canSetNotify functions of CANLIB send Messages to a window. We install
' our own windows messagehandler in between and filter out the CANLIB messages.
'
' We have a Notification Class that handles all the events for one specific CAN
' channel. Yes, you can create multiple Notification objects, for each channel one.
' In this notification class, we only want events for the correct channel! This is
' done by making a collection of Object pointers and CAN Handles in the notify.bas.
' When a new message arives, we recreate our object from our collection (by it's
' handle) and call the correct notification function....
'
'
' How to use?
' -----------
' 1) Add all these files to your project.
' 2) Include this readme.bas file to your project.
' 3) Mention 'Ordina Technical Automation' in a visible part
'    of your project (e.g. splash screen or about box)
' 4) Create an instance of CNotify:
'    dim withevents CanNotify as CNotify
'    Set CanNotify = new CNotify
' 5) now start the notification by calling:
'    CanNotify.SetUpEventNotification mHandle
'    mHandle is the CAN Handle of your CAN Channel!
' 6) From now on you get notifictions events like:
'    -DataReceived()
'    -MessageSent()
'    -BusError(Active As Boolean)
' 7) You can stop notification by calling:
'    CanNotify.CancelEventNotification
'
' Be sure to understand the tricks used!
'
' last update: 15-04-2002
