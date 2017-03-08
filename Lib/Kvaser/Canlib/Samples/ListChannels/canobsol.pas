(*
**                        Copyright 1998 by KVASER AB            
**                  P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**            E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** $Header$
** $Revision$   $NoKeywords: $
*)
(*
** This file contains obsolete elements from CAN.PAS.
*)

const

   {Circuit status flags, retained for compatibility}
   canCIRCSTAT_ERROR_PASSIVE    = $0001;
   canCIRCSTAT_BUS_OFF          = $0002;
   canCIRCSTAT_ERROR_WARNING    = $0004;

{Sticky status flags}
   canSTICKY_ACK_ERROR  =     $80000801; {}
   canSTICKY_FORM_ERROR  =    $80000002; {}
   canSTICKY_STUFF_ERROR  =   $80000004; {}
   canSTICKY_CRC_ERROR  =     $80000008; {}
   canSTICKY_BIT0_ERROR  =    $80000010; {}
   canSTICKY_BIT1_ERROR  =    $80000020; {}
   canSTICKY_HW_OVERRUN  =    $80000040; {}
   canSTICKY_SW_OVERRUN  =    $80000080; {}

   {Circuit types.}
   { Illegal              = 0}
   PCCAN_PHILIPS          = 1;         { 82C200 on PCCAN 1.0 }
   PCCAN_INTEL526         = 2;         { Not supported.}
   PCCAN_INTEL527         = 3;         { 82527 on PCCAN 1.0}
   CANCARD_NEC72005       = 4;         { NEC72005 on CANCard }
   CIRC_VIRTUAL           = 5;         { A virtual chip }
    PCCAN_VIRTUAL         = 5;
   CIRC_SJA1000           = 6;         { SJA1000 on LapCAN }

   { Card types. }
   { Illegal              = 0;}
   canCARD_PCCAN          = 1;         { PCCAN ver 1.x (KVASER)}
   canCARD_CANCARD        = 2;         { CANCard (Softing)}
   canCARD_AC2            = 3;         { CAN-AC2 (Softing)}
   canCARD_LapCAN         = 4;         { LapCAN (KVASER)}
   canCARD_PCCAN2         = 5;         { PCCAN2 (KVASER)}


   canWANT_ACTIONS          = $01; {Used to get callback functions, currently unsupported}
   canWANT_OWN_BUFFERS      = $02; {Used to get own buffers, currently unsupported}
   canWANT_ERROR_COUNTERS   = $04; {Store error counters with every msg}

   { The canFlgXXX are left for compatibility.}
   canFlgACCEPT         = 1;    {Accept this envelope.}
   canFlgREJECT         = 2;    {Reject this envelope.}
   canFlgCODE           = 3;    {Use the envelope parameter as acceptance code.}
   canFlgMASK           = 4;    {Use the envelope parameter as acceptance mask.}

   canDISCARD_MESSAGE   = 3;    {Currently unsupported.}
   canRETAIN_MESSAGE    = 4;    {Currently unsupported.}

   canSLOW_MODE         = $01;  {For Bus parameters - sync on rising edge
                                 only or both rising and falling edge}

   {CAN driver types; these constants are retained for compatibility.}
   canOFF               = 0;    {Driver stage: Disabled.}
   canTRISTATE          = 1;    {Driver stage: Tristated.}
   canPULLUP            = 2;    {Driver stage: Pull-up.}
   canPULLDOWN          = 3;    {Driver stage: Pull-down.}
   canPUSHPULL          = 4;    {Driver stage: Push pull - the usual one!}
   canINVPULLUP         = 5;    {Driver stage: Inverted pull-up.}
   canINVPULLDOWN       = 6;    {Driver stage: Inverted pull-down.}
   canINVPUSHPULL       = 7;    {Driver stage: Inverted push pull.}

   canERR_BUFOFL  =        canERR_RESERVED_3;   {// "Buffer overflow"}
   canERR_REINIT  =        canERR_RESERVED_4;   {// "Reinitialization failed"}
   canERR_MSGLOST  =       canERR_RESERVED_5;   {// "Message lost by CAN-chip"}
   canERR_OVERRUN  =       canERR_RESERVED_6;   {// "Receive buffer overrun"}
   canERR_CARDCOMMFAILED  =canERR_RESERVED_7;   {// "Can't communicate with the card"}

   { These were never implemented }
   canIOCTL_LOCAL_ECHO_ON  = 3;
   canIOCTL_LOCAL_ECHO_OFF = 4;
