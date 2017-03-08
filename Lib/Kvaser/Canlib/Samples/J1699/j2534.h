#ifndef __J2534_H
#define __J2534_H

/* Protocol definitions */
#define J1850VPW							1
#define J1850PWM							2
#define ISO9141								3
#define ISO14230							4
#define CAN									5
#define ISO15765							6
#define SCI_A_ENGINE						7
#define SCI_A_TRANS							8
#define SCI_B_ENGINE						9
#define SCI_B_TRANS							10

/* IOCTL IDs */
#define GET_CONFIG							1
#define SET_CONFIG							2
#define READ_VBATT							3
#define FIVE_BAUD_INIT						4
#define FAST_INIT							5
#define SET_PIN_USE							6
#define CLEAR_TX_BUFFERS					7
#define CLEAR_TX_BUFFER						7
#define CLEAR_RX_BUFFERS					8
#define CLEAR_RX_BUFFER 					8
#define CLEAR_PERIODIC_MSGS					9
#define CLEAR_MSG_FILTERS					10
#define CLEAR_FUNCT_MSG_LOOKUP_TABLE		11
#define ADD_TO_FUNCT_MSG_LOOKUP_TABLE		12
#define DELETE_FROM_FUNCT_MSG_LOOKUP_TABLE	13
#define READ_PROG_VOLTAGE					14

#define READ_CH1_VOLTAGE					0x10000
#define READ_CH2_VOLTAGE					0x10001
#define READ_CH3_VOLTAGE					0x10002
#define READ_CH4_VOLTAGE					0x10003
#define READ_CH5_VOLTAGE					0x10004
#define READ_CH6_VOLTAGE					0x10005

/* Configuration Parameter IDs */
#define DATA_RATE							1
#define LOOPBACK							3
#define NODE_ADDRESS						4
#define NETWORK_LINE						5
#define P1_MIN								6
#define P1_MAX								7
#define P2_MIN								8
#define P2_MAX								9
#define P3_MIN								10
#define P3_MAX								11
#define P4_MIN								12
#define P4_MAX								13
#define W1									14
#define W2									15
#define W3									16
#define W4									17
#define W5									18
#define TIDLE								19
#define TINIL								20
#define TWUP								21
#define PARITY								22
#define BIT_SAMPLE_POINT					23
#define SYNC_JUMP_WIDTH 					24
#define T1_MAX								26
#define T2_MAX								27
#define T4_MAX								28
#define T5_MAX								29
#define ISO15765_BS							30
#define ISO15765_STMIN						31
#define DATA_BITS							100

#define ADC_READINGS_PER_SECOND				0x10000

/* Error IDs */
/*
#define STATUS_NOERROR						0
#define ERR_NOT_SUPPORTED					1
#define ERR_INVALID_CHANNEL_ID				2
#define ERR_INVALID_PROTOCOL_ID				3
#define ERR_NULL_PARAMETER					4
#define ERR_INVALID_IOCTL					5
#define ERR_INVALID_FLAGS					6
#define ERR_FAILED							7
#define ERR_DEVICE_NOT_CONNECTED			8
#define ERR_TIMEOUT							9
#define ERR_INVALID_MSG						10
#define ERR_INVALID_TIME_INTERVAL			11
#define ERR_EXCEEDED_LIMIT					12
#define ERR_INVALID_MSG_ID					13
#define ERR_INVALID_ERROR_ID				14
#define ERR_INVALID_IOCTL_ID				15
#define ERR_BUFFER_EMPTY					16
#define ERR_BUFFER_FULL						17
#define ERR_BUFFER_OVERFLOW					18
#define ERR_PIN_INVALID						19
#define ERR_CHANNEL_IN_USE					20
#define ERR_MSG_PROTOCOL_ID					21
*/
#define STATUS_NOERROR						0x00
#define ERR_NOT_SUPPORTED					0x01
#define ERR_INVALID_CHANNEL_ID				0x02
#define ERR_INVALID_PROTOCOL_ID				0x03
#define ERR_NULL_PARAMETER					0x04
#define ERR_INVALID_IOCTL_VALUE				0x05
#define ERR_INVALID_FLAGS					0x06
#define ERR_FAILED							0x07
#define ERR_DEVICE_NOT_CONNECTED			0x08
#define ERR_TIMEOUT							0x09
#define ERR_INVALID_MSG						0x0A
#define ERR_INVALID_TIME_INTERVAL			0x0B
#define ERR_EXCEEDED_LIMIT					0x0C
#define ERR_INVALID_MSG_ID					0x0D
#define ERR_DEVICE_IN_USE					0x0E
#define ERR_INVALID_IOCTL_ID				0x0F
#define ERR_BUFFER_EMPTY					0x10
#define ERR_BUFFER_FULL						0x11
#define ERR_BUFFER_OVERFLOW					0x12
#define ERR_PIN_INVALID						0x13
#define ERR_CHANNEL_IN_USE					0x14
#define ERR_MSG_PROTOCOL_ID					0x15
#define ERR_INVALID_FILTER_ID				0x16
#define ERR_NO_FLOW_CONTROL					0x17
#define ERR_NOT_UNIQUE						0x18
#define ERR_INVALID_BAUDRATE				0x19
#define ERR_INVALID_DEVICE_ID				0x1A  /*0x20*/


/* Miscellaneous definitions */
#define SHORT_TO_GROUND						0xFFFFFFFE
#define VOLTAGE_OFF							0xFFFFFFFF
#define NO_PARITY							0
#define ODD_PARITY							1
#define EVEN_PARITY							2

/* RxStatus definitions */
#define TX_MSG_TYPE							0x00000001
#define ISO15765_FIRST_FRAME				0x00000002
#define START_OF_MESSAGE					0x00000002
#define RX_BREAK							0x00000004
#define TX_DONE								0x00000008
#define ISO15765_PADDING_ERROR				0x00000010
#define ISO15765_ADDR_TYPE					0x00000080
#define CAN_29BIT_ID						0x00000100

/* PassThruConnect / TxFlags definitions */
#define TX_NORMAL_TRANSMIT					0x00000000
#ifndef ISO15765_ADDR_TYPE
#define ISO15765_ADDR_TYPE					0x00000080
#endif
#define ISO15765_FRAME_PAD					0x00000040
#define ISO15765_EXT_ADDR					0x00000080
#ifndef CAN_29BIT_ID
#define CAN_29BIT_ID						0x00000100
#endif
#define CAN_EXTENDED_ID						0x00000100
#define TX_BLOCKING							0x00010000
#define SCI_TX_VOLTAGE						0x00800000
#define SCI_MODE                            0x00400000
#define WAIT_P3_MIN_ONLY                    0x00000200

#define CONNECT_ETHERNET_ONLY				0x80000000
#define ISO9141_NO_CHECKSUM					0x40000000

#define ISO9141_K_LINE_ONLY                 0x00001000

/* Filter definitions */
#define PASS_FILTER							0x00000001
#define BLOCK_FILTER						0x00000002
#define FLOW_CONTROL_FILTER					0x00000003

/* Message Structure */
typedef struct
{
	unsigned long ProtocolID;
	unsigned long RxStatus;
	unsigned long TxFlags;
	unsigned long Timestamp;
	unsigned long DataSize;
	unsigned long ExtraDataIndex;
	unsigned char Data[4128];
} PASSTHRU_MSG;

/* IOCTL Structures */
typedef struct
{
	unsigned long Parameter;
	unsigned long Value;
} SCONFIG;

typedef struct
{
	unsigned long NumOfParams;
	SCONFIG *ConfigPtr;
} SCONFIG_LIST;

typedef struct
{
	unsigned long NumOfBytes;
	unsigned char *BytePtr;
} SBYTE_ARRAY;

typedef struct
{
	unsigned long NumOfPins;
	unsigned char *PinUsePtr;
} SPIN_CONTROL;

typedef struct
{
	unsigned long PinNumber;
	unsigned long PinUse;
	unsigned long Parameter;
} SPIN_USE;

#endif /* __J2534_H */

