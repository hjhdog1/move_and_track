
#ifndef PCI_H
#define PCI_H

//include "stdtypes.h"
#define UINT8 unsigned char
#define UINT16 unsigned int
#define UINT32 unsigned long

#define TRUE    (0==0)
#define FALSE   (0==1)

typedef unsigned char byte;     //* 8-bit  
typedef unsigned short word;    //* 16-bit 
typedef unsigned long dword;    //* 32-bit 

#define CARRY_FLAG 0x01         //* 80x86 Flags Register Carry Flag bit 
  
/****************************************************************************/
/*   PCI Functions                                                          */
/****************************************************************************/

#define PCI_FUNCTION_ID           0xb1
#define PCI_BIOS_PRESENT          0x01
#define FIND_PCI_DEVICE           0x02
#define FIND_PCI_CLASS_CODE       0x03
#define GENERATE_SPECIAL_CYCLE    0x06
#define READ_CONFIG_BYTE          0x08

#define READ_CONFIG_WORD          0x09
#define READ_CONFIG_DWORD         0x0a
#define WRITE_CONFIG_BYTE         0x0b
#define WRITE_CONFIG_WORD         0x0c
#define WRITE_CONFIG_DWORD        0x0d

#define MAX_BUS_COUNT                   2
#define MAX_DEVICE_COUNT                32
#define MAX_DEVICE_FUNCTIONS    8

/**************************************************************/
/*   Generic Return Code List                                 */
/**************************************************************/
#define SUCCESSFUL               0x00
#define NOT_SUCCESSFUL           0x01

/****************************************************************************/
/*   PCI Configuration Space Registers                                      */
/****************************************************************************/

#define PCI_CS_VENDOR_ID         0x00
#define PCI_CS_DEVICE_ID         0x02
#define PCI_CS_COMMAND           0x04
#define PCI_CS_STATUS            0x06
#define PCI_CS_REVISION_ID       0x08
#define PCI_CS_CLASS_CODE        0x09
#define PCI_CS_CACHE_LINE_SIZE   0x0c
#define PCI_CS_MASTER_LATENCY    0x0d
#define PCI_CS_HEADER_TYPE       0x0e
#define PCI_CS_BIST              0x0f
#define PCI_CS_BASE_ADDRESS_0    0x10
#define PCI_CS_BASE_ADDRESS_1    0x14
#define PCI_CS_BASE_ADDRESS_2    0x18
#define PCI_CS_BASE_ADDRESS_3    0x1c
#define PCI_CS_BASE_ADDRESS_4    0x20
#define PCI_CS_BASE_ADDRESS_5    0x24
#define PCI_CS_SVID           	 0x2C
#define PCI_CS_SID           	 0x2E
#define PCI_CS_EXPANSION_ROM     0x30
#define PCI_CS_INTERRUPT_LINE    0x3c
#define PCI_CS_INTERRUPT_PIN     0x3d
#define PCI_CS_MIN_GNT           0x3e
#define PCI_CS_MAX_LAT           0x3f

/* Note that in nvram, SVID is 6C and SID is 6E */
/****************************************************************************/
/*   AMCC Operation Register Offsets                                        */
/****************************************************************************/

#define AMCC_OP_REG_OMB1         0x00
#define AMCC_OP_REG_OMB2         0x04
#define AMCC_OP_REG_OMB3         0x08 
#define AMCC_OP_REG_OMB4         0x0c
#define AMCC_OP_REG_IMB1         0x10
#define AMCC_OP_REG_IMB2         0x14
#define AMCC_OP_REG_IMB3         0x18 
#define AMCC_OP_REG_IMB4         0x1c
#define AMCC_OP_REG_FIFO         0x20
#define AMCC_OP_REG_MWAR         0x24
#define AMCC_OP_REG_MWTC         0x28
#define AMCC_OP_REG_MRAR         0x2c
#define AMCC_OP_REG_MRTC         0x30
#define AMCC_OP_REG_MBEF         0x34
#define AMCC_OP_REG_INTCSR       0x38
#define AMCC_OP_REG_MCSR         0x3c
#define AMCC_OP_REG_MCSR_NVDATA  (AMCC_OP_REG_MCSR + 2) /* Data in byte 2 */
#define AMCC_OP_REG_MCSR_NVCMD   (AMCC_OP_REG_MCSR + 3) /* Command in byte 3 */


/* PCI config structure */
struct pci_space {
    UINT16 ven_id;
    UINT16 dev_id;
    UINT16 command;
    UINT16 status;
    UINT8 rev;
    UINT8 classCode[3];
    UINT8 cache_line;
    UINT8 latency;
    UINT8 header_type;
    UINT8 bist;
    UINT32 base[6];
	UINT32 res1;
    UINT16 sv_id;
    UINT16 s_id;
    UINT32 exp_rom;
    UINT32 res2;
    UINT32 res3;
    UINT8 int_line;
    UINT8 int_pin;
    UINT8 min_gnt;
	UINT8 max_lat;
};


/* region information - applies to region 0 as well as passthru */
struct _reg {
	UINT32 base;	/* base address of I/O or memory 
					   exactly as read from the chip*/
	UINT32 size;	/* size in bytes */
};

/* define for base register (bottom bits of BADR/base) */
#define BASE_IO 1			/* bit 1 set means IO space */
#define NUM_BASE_REGIONS 6

struct _chip {
	/* values based on hardware registers... */
	struct _reg reg[NUM_BASE_REGIONS];	// support all defined by PCI
	UINT32 class_code;			/* upper 3 bytes of long register */
	UINT16 vendor_id;
	UINT16 device_id;
	UINT16 subsys_vendor_id;
	UINT16 subsys_id;
	int type;					/* AMCC_5933, AMCC_5920 */
	UINT8 revision_id;
	/* PCI BIOS stuff... */
	UINT16 index;
	UINT8 bus;
	UINT8 device;				/* device and function */
};

enum { AMCC_NOT_INITIALIZED, AMCC_5920, AMCC_5933 };


#define OMB_5920	0x0C
#define IMB_5920	0x1C
#define MBEF_5920	0x34
#define INTCSR_5920 0x38
#define RCR_5920	0x3C
#define PTCR_5920	0x60


#define	PCI_SUCCESSFUL 0
#define	PCI_NOT_SUCCESSFUL 1
#define PCI_FUNC_NOT_SUPPORTED 0x81
#define	PCI_BAD_VENDOR_ID 0x83
#define	PCI_DEVICE_NOT_FOUND 0x86
#define	PCI_BAD_REGISTER_NUMBER 0x87


#define HIGH_BYTE(ax) (ax >> 8)
#define LOW_BYTE(ax) (ax & 0xFF)

/*** PROTOTYPES ****/

int xDetectPCICard();

int ShowOperationRegs();

int ShowConfig();

UINT32 GetOPR( UINT8 Offset );

int GetRegionInfo( struct _chip *chip );

UINT8 pciReadCFG(UINT8 bus, UINT8 device, struct pci_space * pPci );

UINT8 GetnvRAM( UINT16 Offset );

void WriteOPR( UINT8 Offset, UINT32 wData );

void SetnvRAM( UINT16 Offset, UINT8 bData );

UINT8  pciConfigRead8( UINT8, UINT8, UINT8, UINT8 *);
UINT8 pciConfigRead16( UINT8, UINT8, UINT8, UINT16 * );
UINT8 pciConfigRead32( UINT8, UINT8, UINT8, UINT32 * );

UINT8 pciConfigWrite8( UINT8, UINT8, UINT8, UINT8 );
UINT8 pciConfigWrite16( UINT8, UINT8, UINT8, UINT16 );
UINT8 pciConfigWrite32( UINT8, UINT8, UINT8, UINT32 );

UINT8 pciBiosPresent(UINT8 *hardware_mechanism,
                            UINT16 *interface_level_version,
                            UINT8 *last_pci_bus_number);



#endif
// PCI_H
