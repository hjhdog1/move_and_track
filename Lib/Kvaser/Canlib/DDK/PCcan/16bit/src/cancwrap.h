/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _CANCWRAP_H_
#define _CANCWRAP_H_

#ifdef _WINDOWS

#define N_INIPC_INITIALIZE_BOARD 0
#define INIPC_INITIALIZE_BOARD ((int (_far _pascal *)(int))Fun[N_INIPC_INITIALIZE_BOARD].Proc)

#define N_CANPC_RESET_BOARD 1
#define CANPC_RESET_BOARD ((int (_far _pascal *)(void))Fun[N_CANPC_RESET_BOARD].Proc)

#define N_CANPC_RESET_CHIP 2
#define CANPC_RESET_CHIP ((int (_far _pascal *)(void))Fun[N_CANPC_RESET_CHIP].Proc)

#define N_CANPC_INITIALIZE_CHIP 3
#define CANPC_INITIALIZE_CHIP ((int (_far _pascal *)(int presc, int sjw, int tseg1, int tseg2, int sam))Fun[N_CANPC_INITIALIZE_CHIP].Proc)

#define N_CANPC_INITIALIZE_CHIP2 4
#define CANPC_INITIALIZE_CHIP2 ((int (_far _pascal *)(int presc, int sjw, int tseg1, int tseg2, int sam))Fun[N_CANPC_INITIALIZE_CHIP2].Proc)

#define N_CANPC_SET_MODE 5
#define CANPC_SET_MODE ((int (_far _pascal *)(int SleepMode, int SpeedMode))Fun[N_CANPC_SET_MODE].Proc)

#define N_CANPC_SET_MODE2 6
#define CANPC_SET_MODE2 ((int (_far _pascal *)(int SleepMode, int SpeedMode))Fun[N_CANPC_SET_MODE2].Proc)

#define N_CANPC_SET_ACCEPTANCE 7
#define CANPC_SET_ACCEPTANCE  ((int (_far _pascal *)(unsigned int AccCodeStd, unsigned int AccMaskStd, unsigned long AccCodeXtd, unsigned long AccMaskXtd))Fun[N_CANPC_SET_ACCEPTANCE ].Proc)

#define N_CANPC_SET_ACCEPTANCE2 8
#define CANPC_SET_ACCEPTANCE2 ((int (_far _pascal *)(unsigned int AccCodeStd, unsigned int AccMaskStd, unsigned long AccCodeXtd, unsigned long AccMaskXtd))Fun[N_CANPC_SET_ACCEPTANCE2].Proc)

#define N_CANPC_SET_OUTPUT_CONTROL 9
#define CANPC_SET_OUTPUT_CONTROL  ((int (_far _pascal *)(int OutputControl))Fun[N_CANPC_SET_OUTPUT_CONTROL ].Proc)

#define N_CANPC_SET_OUTPUT_CONTROL2 10
#define CANPC_SET_OUTPUT_CONTROL2 ((int (_far _pascal *)(int OutputControl))Fun[N_CANPC_SET_OUTPUT_CONTROL2].Proc)

#define N_CANPC_INITIALIZE_INTERFACE 11
#define CANPC_INITIALIZE_INTERFACE ((int (_far _pascal *)(int ReceiveFifoEnable, int ReceivePollAll, int ReceiveEnableAll, int ReceiveIntEnableAll, int AutoRemoteEnableAll, int TransmitReqFifoEnable, int TransmitPollAll, int TransmitAckEnableAll, int TransmitAckFifoEnableAll, int TransmitRmtFifoEnableAll))Fun[N_CANPC_INITIALIZE_INTERFACE].Proc)

#define N_CANPC_DEFINE_OBJECT 12
#define CANPC_DEFINE_OBJECT  ((int (_far _pascal *)(unsigned long Ident, int_ptr ObjectNumber, int Type, int ReceiveIntEnable, int AutoRemoteEnable, int TransmitAckEnable))Fun[N_CANPC_DEFINE_OBJECT ].Proc)

#define N_CANPC_DEFINE_OBJECT2 13
#define CANPC_DEFINE_OBJECT2 ((int (_far _pascal *)(unsigned long Ident, int_ptr ObjectNumber, int Type, int ReceiveIntEnable, int AutoRemoteEnable, int TransmitAckEnable))Fun[N_CANPC_DEFINE_OBJECT2].Proc)

#define N_CANPC_ENABLE_FIFO 14
#define CANPC_ENABLE_FIFO ((int (_far _pascal *)(void))Fun[N_CANPC_ENABLE_FIFO].Proc)

#define N_CANPC_OPTIMIZE_RCV_SPEED 15
#define CANPC_OPTIMIZE_RCV_SPEED ((int (_far _pascal *)(void))Fun[N_CANPC_OPTIMIZE_RCV_SPEED].Proc)

#define N_CANPC_ENABLE_DYN_OBJ_BUF 16
#define CANPC_ENABLE_DYN_OBJ_BUF ((int (_far _pascal *)(void))Fun[N_CANPC_ENABLE_DYN_OBJ_BUF].Proc)

#define N_CANPC_ENABLE_TIMESTAMPS 17
#define CANPC_ENABLE_TIMESTAMPS ((int (_far _pascal *)(void))Fun[N_CANPC_ENABLE_TIMESTAMPS].Proc)

#define N_CANPC_ENABLE_FIFO_TX_ACK 18
#define CANPC_ENABLE_FIFO_TX_ACK  ((int (_far _pascal *)(void))Fun[N_CANPC_ENABLE_FIFO_TX_ACK ].Proc)

#define N_CANPC_ENABLE_FIFO_TX_ACK2 19
#define CANPC_ENABLE_FIFO_TX_ACK2 ((int (_far _pascal *)(void))Fun[N_CANPC_ENABLE_FIFO_TX_ACK2].Proc)

#define N_CANPC_GET_VERSION 20
#define CANPC_GET_VERSION ((int (_far _pascal *)(int_ptr sw_version, int_ptr fw_version, int_ptr hw_version, int_ptr license, int_ptr can_chip_type))Fun[N_CANPC_GET_VERSION].Proc)

#define N_CANPC_START_CHIP 21
#define CANPC_START_CHIP ((int (_far _pascal *)(void))Fun[N_CANPC_START_CHIP].Proc)

#define N_CANPC_SEND_REMOTE_OBJECT 22
#define CANPC_SEND_REMOTE_OBJECT  ((int (_far _pascal *)(int Ident, int DataLength))Fun[N_CANPC_SEND_REMOTE_OBJECT ].Proc)

#define N_CANPC_SEND_REMOTE_OBJECT2 23
#define CANPC_SEND_REMOTE_OBJECT2 ((int (_far _pascal *)(int Ident, int DataLength))Fun[N_CANPC_SEND_REMOTE_OBJECT2].Proc)

#define N_CANPC_SUPPLY_OBJECT_DATA 24
#define CANPC_SUPPLY_OBJECT_DATA  ((int (_far _pascal *)(int Ident, int DataLength, uns_char_ptr pData))Fun[N_CANPC_SUPPLY_OBJECT_DATA ].Proc)

#define N_CANPC_SUPPLY_OBJECT_DATA2 25
#define CANPC_SUPPLY_OBJECT_DATA2 ((int (_far _pascal *)(int Ident, int DataLength, uns_char_ptr pData))Fun[N_CANPC_SUPPLY_OBJECT_DATA2].Proc)

#define N_CANPC_SUPPLY_RCV_OBJECT_DATA 26
#define CANPC_SUPPLY_RCV_OBJECT_DATA  ((int (_far _pascal *)(int Ident, int DataLength, uns_char_ptr pData))Fun[N_CANPC_SUPPLY_RCV_OBJECT_DATA ].Proc)

#define N_CANPC_SUPPLY_RCV_OBJECT_DATA2 27
#define CANPC_SUPPLY_RCV_OBJECT_DATA2 ((int (_far _pascal *)(int Ident, int DataLength, uns_char_ptr pData))Fun[N_CANPC_SUPPLY_RCV_OBJECT_DATA2].Proc)

#define N_CANPC_SEND_OBJECT 28
#define CANPC_SEND_OBJECT  ((int (_far _pascal *)(int Ident, int DataLength))Fun[N_CANPC_SEND_OBJECT ].Proc)

#define N_CANPC_SEND_OBJECT2 29
#define CANPC_SEND_OBJECT2 ((int (_far _pascal *)(int Ident, int DataLength))Fun[N_CANPC_SEND_OBJECT2].Proc)

#define N_CANPC_WRITE_OBJECT 30
#define CANPC_WRITE_OBJECT  ((int (_far _pascal *)(int Ident, int DataLength, uns_char_ptr pData))Fun[N_CANPC_WRITE_OBJECT ].Proc)

#define N_CANPC_WRITE_OBJECT2 31
#define CANPC_WRITE_OBJECT2 ((int (_far _pascal *)(int Ident, int DataLength, uns_char_ptr pData))Fun[N_CANPC_WRITE_OBJECT2].Proc)

#define N_CANPC_READ_RCV_DATA 32
#define CANPC_READ_RCV_DATA  ((int (_far _pascal *)(int Ident, uns_char_ptr pRCV_Data, uns_long_ptr Time))Fun[N_CANPC_READ_RCV_DATA ].Proc)

#define N_CANPC_READ_RCV_DATA2 33
#define CANPC_READ_RCV_DATA2 ((int (_far _pascal *)(int Ident, uns_char_ptr pRCV_Data, uns_long_ptr Time))Fun[N_CANPC_READ_RCV_DATA2].Proc)

#define N_CANPC_READ_XMT_DATA 34
#define CANPC_READ_XMT_DATA  ((int (_far _pascal *)(int Ident, int_ptr pDataLength, uns_char_ptr pXMT_Data))Fun[N_CANPC_READ_XMT_DATA ].Proc)

#define N_CANPC_READ_XMT_DATA2 35
#define CANPC_READ_XMT_DATA2 ((int (_far _pascal *)(int Ident, int_ptr pDataLength, uns_char_ptr pXMT_Data))Fun[N_CANPC_READ_XMT_DATA2].Proc)

#define N_CANPC_READ_AC 36
#define CANPC_READ_AC ((int (_far _pascal *)(param_ptr param))Fun[N_CANPC_READ_AC].Proc)

#define N_CANPC_SEND_DATA 37
#define CANPC_SEND_DATA  ((int (_far _pascal *)(unsigned long Ident, int Xtd, int DataLength, uns_char_ptr pData))Fun[N_CANPC_SEND_DATA ].Proc)

#define N_CANPC_SEND_DATA2 38 
#define CANPC_SEND_DATA2 ((int (_far _pascal *)(unsigned long Ident, int Xtd, int DataLength, uns_char_ptr pData))Fun[N_CANPC_SEND_DATA2].Proc)

#define N_CANPC_SEND_REMOTE 39
#define CANPC_SEND_REMOTE  ((int (_far _pascal *)(unsigned long Ident, int Xtd, int DataLength))Fun[N_CANPC_SEND_REMOTE ].Proc)

#define N_CANPC_SEND_REMOTE2 40
#define CANPC_SEND_REMOTE2 ((int (_far _pascal *)(unsigned long Ident, int Xtd, int DataLength))Fun[N_CANPC_SEND_REMOTE2].Proc)

#define N_CANPC_SET_TRIGGER 41
#define CANPC_SET_TRIGGER  ((int (_far _pascal *)(int level))Fun[N_CANPC_SET_TRIGGER ].Proc)

#define N_CANPC_SET_TRIGGER2 42
#define CANPC_SET_TRIGGER2 ((int (_far _pascal *)(int level))Fun[N_CANPC_SET_TRIGGER2].Proc)

#define N_CANPC_REINITIALIZE 43
#define CANPC_REINITIALIZE ((int (_far _pascal *)(void))Fun[N_CANPC_REINITIALIZE].Proc)

#define N_CANPC_RESET_RCV_FIFO 44
#define CANPC_RESET_RCV_FIFO ((int (_far _pascal *)(void))Fun[N_CANPC_RESET_RCV_FIFO].Proc)

#define N_CANPC_GET_TIME 45
#define CANPC_GET_TIME ((int (_far _pascal *)(uns_long_ptr time))Fun[N_CANPC_GET_TIME].Proc)

#else /*ifdef _WINDOWS*/

#define INIPC_INITIALIZE_BOARD INIPC_initialize_board
#define CANPC_RESET_BOARD CANPC_reset_board
#define CANPC_RESET_CHIP CANPC_reset_chip
#define CANPC_INITIALIZE_CHIP CANPC_initialize_chip
#define CANPC_INITIALIZE_CHIP2 CANPC_initialize_chip2
#define CANPC_SET_MODE  CANPC_set_mode
#define CANPC_SET_MODE2 CANPC_set_mode2
#define CANPC_SET_ACCEPTANCE  CANPC_set_acceptance 
#define CANPC_SET_OUTPUT_CONTROL CANPC_set_output_control  
#define CANPC_INITIALIZE_INTERFACE CANPC_initialize_interface
#define CANPC_ENABLE_FIFO CANPC_enable_fifo
#define CANPC_ENABLE_FIFO_TX_ACK CANPC_enable_fifo_transmit_ack
#define CANPC_START_CHIP CANPC_start_chip
#define CANPC_READ_AC CANPC_read_ac
#define CANPC_SEND_DATA CANPC_send_data
#define CANPC_SEND_REMOTE CANPC_send_remote
#define CANPC_REINITIALIZE CANPC_reinitialize
#endif /* ifdef _WINDOWS*/


#endif /* ifdef _CANCWRAP_H_*/
