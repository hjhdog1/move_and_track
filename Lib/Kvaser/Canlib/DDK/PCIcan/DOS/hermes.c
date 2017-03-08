/*
**                         Copyright 1999 by KVASER AB            
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
**
** Description:
**   DOS program which scans the PCI bus, searching for a Hermes card;
**   and programs its NVRAM if it's found.
**
*/

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <conio.h>

#include "pciconst.h"
#include "version.h"

#define HERMES_DEVICE_ID_PROTO  0x5920u      // AMCC's default DID
#define HERMES_DEVICE_ID        0x8406u      // Our DID registrated with AMCC
#define HERMES_VENDOR_ID        0x10e8u      // AMCC's VID


typedef unsigned char   UCHAR;
typedef unsigned int    UINT;
typedef unsigned char   BOOL;

int xDetectPCICard(void);
int ShowConfig(void);
int ShowOperationRegs(void);

struct pci_space pci;
struct _chip PciCard;

UCHAR ByteList[128];

void Announce(void)
{
    printf("\nConfiguration program for PCIcan\n");
    printf("(c) 2000 KVASER AB, http://www.kvaser.com\n");
    printf("Version: " VERSION_STRING ", " BUILD_TIME_STRING "\n\n");
}

void Usage(void)
{
    printf("\nUsage:\n");
    printf("  hermes [-c][-o][-write|-read]\n");
    printf("  -c            Print PCI configuration.\n"); 
    printf("  -o            Print Operation Registers.\n");
    printf("  -write        Program the NVRAM.\n");
    printf("  -read         Read & print the NVRAM.\n");
    printf("  -scan         Scan the PCI bus and print a list of devices.\n");
    printf("  -ignore-did   Ignore the DeviceID and use only the VendorID to locate boards.");
    printf("\n");
    exit(1);
}


// Options.
int OptShowConfig = FALSE;
int OptShowOPR = FALSE;
int OptDoProgramming = FALSE;
int OptDoRead = FALSE;
int OptShowUsage = FALSE;
int OptDoScan = FALSE;
int OptIgnoreDevId = FALSE;

void ScanPCI(void);

///////////////////////
int main(int argc, char *argv[])
{
    int i;

    Announce();
    
    for (i=1; i<argc; i++) {
        if (strcmp(argv[i], "-c") == 0) OptShowConfig = TRUE;
        else if (strcmp(argv[i], "-o") == 0) OptShowOPR = TRUE;
        else if (strcmp(argv[i], "-write") == 0) OptDoProgramming = TRUE;
        else if (strcmp(argv[i], "-read") == 0) OptDoRead = TRUE;
        else if (strcmp(argv[i], "-scan") == 0) OptDoScan = TRUE;
        else if (strcmp(argv[i], "-ignore-did") == 0) OptIgnoreDevId = TRUE;
        else OptShowUsage = TRUE;
    }

    if (OptDoProgramming && OptDoRead) Usage();
    if (OptShowUsage) Usage();
    
    // oldfunc = getvect(0x1c);

    if (OptDoScan) ScanPCI();
    
    if( xDetectPCICard() == PCI_NOT_SUCCESSFUL )
    {
        printf( "No card found.\n" );
        return 0;
    }

    //
    // One wait state is required by SJA1000
    //
    WriteOPR( PTCR_5920, 0x81818181L );
    //WriteOPR( INTCSR_5920, 0x00002C0CL );   // Enable ADDON interrupt.

    if (OptShowConfig) ShowConfig();
    if (OptShowOPR) ShowOperationRegs();

    if (OptDoProgramming) {
        unsigned char csum, tmp;
        int ok;
        
        memset(ByteList, 0, sizeof(ByteList));

        // Vendor ID
        ByteList[0x40] = (HERMES_VENDOR_ID & 0xFF);
        ByteList[0x41] = ((HERMES_VENDOR_ID >> 8) & 0xFF);

        // Device ID
        ByteList[0x42] = (HERMES_DEVICE_ID & 0xFF);
        ByteList[0x43] = ((HERMES_DEVICE_ID >> 8) & 0xFF);

        // S5920 special modes (0x01 = Target Latency Enable)
        ByteList[0x45] = 0x01;

        // Revision ID
        ByteList[0x48] = 0x00;

        // Class Code (0xFF0000 = no base class code defined for device)
        ByteList[0x49] = 0x00;
        ByteList[0x4a] = 0x00;
        ByteList[0x4b] = 0xff;

        // Required data
        // 0x81 = I/O space, locate anywhere, 0x80 bytes
        // Other values are simply required as-is - see the 5920 docs.
        ByteList[0x50] = 0x81;
        ByteList[0x51] = 0xff;
        ByteList[0x52] = 0xe8;
        ByteList[0x53] = 0x10;

        // region 1
        //        ByteList[0x54] = 0x01;
        //        ByteList[0x55] = 0xfc;
        //        ByteList[0x56] = 0xff;
        //        ByteList[0x57] = 0xff;

        //
        // Region 1 for the SJA1000's
        //
        // 0xFFFFFC01 = I/O space, len = 0x400 (prototype cards)
        // 0xFFFFFF81 = I/O space, len = 0x80  (what it should be eventually)
        // 0xFFFFFE01 = I/O space, len = 0x200 (128 bytes per circut)
#define REGION1_LEN 0x80L
        printf("Note: Length of region 1 now set to %d bytes/sja1000.\n", REGION1_LEN/4);
        ByteList[0x54] = (unsigned char)(~((REGION1_LEN-1) & 0xFF) | 0x01);
        ByteList[0x55] = (unsigned char)(~((REGION1_LEN-1)>>8) & 0xFF);
        ByteList[0x56] = (unsigned char)(~((REGION1_LEN-1)>>16) & 0xFF);
        ByteList[0x57] = (unsigned char)(~((REGION1_LEN-1)>>24) & 0xFF);

        /*
        Region 2:
        ByteList[0x58] = 0x01;
        ByteList[0x59] = 0xfe;
        ByteList[0x5a] = 0xff;
        ByteList[0x5b] = 0xff;
        */
        
        //
        // Region 2 for the registers in the Xilinx
        // 
#define REGION2_LEN 0x08L
        printf("Note: Length of region 2 now set to %d bytes.\n", REGION2_LEN);
        ByteList[0x58] = (unsigned char)(~((REGION2_LEN-1) & 0xFF) | 0x01);
        ByteList[0x59] = (unsigned char)(~((REGION2_LEN-1)>>8) & 0xFF);
        ByteList[0x5a] = (unsigned char)(~((REGION2_LEN-1)>>16) & 0xFF);
        ByteList[0x5b] = (unsigned char)(~((REGION2_LEN-1)>>24) & 0xFF);


        
        // Region 3 : 5c - 5f
        // Region 4 : 60 - 63

        // Unused: 64 - 6b
        // qqq S/N: 64-67

        // SVID
        ByteList[0x6c] = 0;
        ByteList[0x6d] = 0;

        // SID
        ByteList[0x6e] = 0;
        ByteList[0x6f] = 0;

        // Unused: 74-7b
        // qqq EAN: 74-78

        // Interrupt line
        ByteList[0x7c] = 0xff;

        // Interrupt pin, 0x01 = INTA#.
        ByteList[0x7d] = 0x01;

        // Unused: 7f
        // Use it for a checksum, so we can have a diag program read the NVRAM
        // contents and detect (some) problems this way.
        csum = 0;
        for (i = 0; i < 0x80; i++) {
            csum += ByteList[i];
        }
        ByteList[0x7f] = ~csum;
        

        for (i = 0; i < 0x80; i++) {
            SetnvRAM(i, ByteList[i]);
        }
        printf("\nNVRAM programmed... verifying..\n");

        csum = 0;
        ok = TRUE;

        for (i = 0; i < 0x80; i++) {
            tmp = GetnvRAM(i);
            if (tmp != ByteList[i]) {
                printf("ERROR: byte 0x%02x read 0x%02x expected 0x%02x.\n", i, tmp, ByteList[i]);
                ok = FALSE;
            }
            csum += tmp;
        }
        if (!ok) {
            printf("\nERROR: NVRAM verification FAILED.\n");
        } else {
            printf("\nNVRAM verification OK.\n");
        }
                
    } else if (OptDoRead) {
        int j, k;
        unsigned char csum;
        
        memset(ByteList, 0, sizeof(ByteList));
        
        for(i = 0; i < 0x80; i++) {
            ByteList[i] = GetnvRAM(i);
        }
        
        for (j=0; j<sizeof(ByteList)/16; j++) {
            printf("%04x  ", j*16);
            for (k=0; k<16; k++) {
                printf("%02x ", ByteList[j*16 + k]);
            }
            printf("\n");
        }

        csum = 0;
        for (i=0; i<0x80; i++) {
            csum += ByteList[i];
        }
        if (~csum) {
            printf("WARNING: NVRAM has wrong checksum!\n");
        } else {
            printf("NVRAM checksum OK.\n");
        }
    }
    return 0;
}

int ShowOperationRegs(void)
{
    printf( "Opr INTCSR = %8.8X\n", GetOPR( INTCSR_5920 )   );
    printf( "Opr PTCR = %8.8lX\n",  GetOPR( PTCR_5920 )     );
    printf( "Opr MBEF = %8.8lX\n",  GetOPR( MBEF_5920 )     );
    printf( "Opr OMB = %8.8lX\n",   GetOPR( OMB_5920 )      );
    printf( "Opr IMB = %8.8lX\n",   GetOPR( IMB_5920 )      );
    printf( "Opr RCR = %8.8lX\n",   GetOPR( RCR_5920 )      );
    return 0;
}

int ShowConfig(void)
{
    printf( "DEV = %04X\n", pci.dev_id );
    printf( "VEN = %04X\n", pci.ven_id );

    printf( "SVID = %04X\n", pci.sv_id );
    printf( "SID = %04X\n", pci.s_id );
    printf( "Cmd = %04X\n", pci.command );

    printf( "Status = %04X\n", pci.status );
    printf( "BIST = %d\n", pci.bist );

    printf( " Class 1 = %X\n", pci.classCode[0] );
    printf( " Class 2 = %X\n", pci.classCode[1] );
    printf( " Class 3 = %X\n", pci.classCode[2] );

    printf( "Base 0 = %8.8lX\n", (UINT32)pci.base[0] );
    printf( "Base 1 = %8.8lX\n", (UINT32)pci.base[1] );
    printf( "Base 2 = %8.8lX\n", (UINT32)pci.base[2] );
    printf( "Base 3 = %8.8lX\n", (UINT32)pci.base[3] );
    printf( "Base 4 = %8.8lX\n", (UINT32)pci.base[4] );
    printf( "Base 5 = %8.8lX\n", (UINT32)pci.base[5] );

    printf( " Int line = %X\n", pci.int_line );
    printf( " Int pin = %X\n", pci.int_pin );

    printf( " Expansion Rom = %8.8lX\n", pci.exp_rom );
    return 0;
}


//
// Write a byte to the NVRAM
//
void SetnvRAM( UINT16 Offset, UINT8 bData )
{
    UINT32 dwRes;
    UINT8 *ptr8;
    unsigned long loop;

    ptr8 = (UINT8*)&dwRes;

    loop = 0;
    while ( GetOPR( RCR_5920 ) & 0x80000000L ) {
        if (++loop > 100000L) {
            printf( "NVRAM Not Ready, offset=%04x\n", Offset );
            return;
        }
    }

    dwRes = 0;
    ptr8[2] = LOW_BYTE( Offset );
    ptr8[3] = 0x80;

    WriteOPR( RCR_5920, dwRes );    // Load low_addr

    dwRes = 0;
    ptr8[2] = HIGH_BYTE( Offset );
    ptr8[3] = 0xA0;

    WriteOPR( RCR_5920, dwRes );    // Load high_addr

    dwRes = 0;
    ptr8[2] = bData;
    ptr8[3] = 0xC0;

    WriteOPR( RCR_5920, dwRes );    // Load data byte

    loop = 0;
    while( (dwRes=GetOPR( RCR_5920 )) & 0x80000000L ) {
        if (++loop > 100000L) {
            printf( "NVRAM Not Ready, offset=%04x\n", Offset );
            return;
        }
    }
}


//
// Read a byte from NVRAM
//
UINT8 GetnvRAM( UINT16 Offset )
{

    UINT32 dwRes;
    UINT8 *ptr8;
    unsigned long loop;

    ptr8 = (UINT8*)&dwRes;

    loop = 0;
    while( GetOPR( RCR_5920 ) & 0x80000000L ) {
        if (++loop > 100000L) {
            printf( "NVRAM Not Ready, offset=%04x\n", Offset );
            return 0;
        }
    }

    dwRes = 0;
    ptr8[2] = LOW_BYTE( Offset );
    ptr8[3] = 0x80;

    WriteOPR( RCR_5920, dwRes );    // Load low_addr

    dwRes = 0;
    ptr8[2] = HIGH_BYTE( Offset );
    ptr8[3] = 0xA0;

    WriteOPR( RCR_5920, dwRes );    // Load high_addr

    dwRes = 0;
    ptr8[3] = 0xE0;
    WriteOPR( RCR_5920, dwRes );    // Initiate Read

    loop = 0;
    while( (dwRes=GetOPR( RCR_5920 )) & 0x80000000L ) {
        if (++loop > 100000L) {
            printf( "NVRAM Not Ready, offset=%04x\n", Offset );
            return 0;
        }
    }

    return ptr8[2]; 
}

/////////////////////////////////////////////////////////////
void WriteOPR( UINT8 Offset, UINT32 wData )
{
    UINT16 *ptr;

    ptr = (UINT16*)&wData;

    outpw( Offset + (UINT16)(PciCard.reg[0].base & 0xFFFFFFFCL), ptr[0]);
    outpw( Offset + 2 + (UINT16)(PciCard.reg[0].base & 0xFFFFFFFCL), ptr[1]);
}


/////////////////////////////////////////////////////////////
UINT32 GetOPR( UINT8 Offset )
{
    UINT16 *ptr;
    UINT32 dwRes;

    ptr = (UINT16*)&dwRes;
    ptr[0] = inpw(Offset + (UINT16)(PciCard.reg[0].base & 0xFFFFFFFCL));
    ptr[1] = inpw(Offset+2 + (UINT16)(PciCard.reg[0].base & 0xFFFFFFFCL));

    return dwRes;
}

//////////////////////////////////////////////////////////////////////////
UINT8 pciReadCFG(UINT8 bus, UINT8 device, struct pci_space * pPci )
{
    UINT16 i;
    UINT8 status;
    UINT8 * ptr;

    ptr = (UINT8 *)pPci;

    for( i = 0; i < sizeof( struct pci_space ); i++ ) 
    {
        status = pciConfigRead8 (bus, device, i+0, &ptr[i] );
        if ( status != PCI_SUCCESSFUL )
            return( status );
    }

    return( status );
}

//////////////////////////////////////////////////////////////////////////
UINT8 pciConfigRead8 ( UINT8 bus, UINT8 device, UINT8 offset, UINT8 *byte )
{
    int ret_status;
    UINT16 ax, flags;

    // must do AX last because compiler uses it for other stuff
    _BX = (bus << 8) | (device);
    _DI = (UINT16)offset;
    _AH = PCI_FUNCTION_ID;
    _AL = READ_CONFIG_BYTE;
    geninterrupt( 0x1A );

    // save regs before compiler overwrites...
    ax = _AX;
    flags = _FLAGS;
    *byte = _CL;

    if ((flags & CARRY_FLAG) == 0) {
        ret_status = HIGH_BYTE(ax);
    } else {
        ret_status = PCI_NOT_SUCCESSFUL;
    }

    return( ret_status );
}

//////////////////////////////////////////////////////////////////////////
UINT8 pciConfigRead16 ( UINT8 bus, UINT8 device, UINT8 offset, UINT16 *word )
{
    int ret_status;
    UINT16 ax, flags;

    // must do AX last because compiler uses it for other stuff
    _BX = (bus << 8) | (device);
    _DI = (UINT16)offset;
    _AH = PCI_FUNCTION_ID;
    _AL = READ_CONFIG_WORD;
    geninterrupt( 0x1A );

    // save regs before compiler overwrites...
    ax = _AX;
    flags = _FLAGS;
    *word = _CX;

    if( (flags & CARRY_FLAG) == 0 )
        ret_status = HIGH_BYTE(ax);
    else 
        ret_status = PCI_NOT_SUCCESSFUL;

    return( ret_status );
}

///////////////////////////////////////////////////////////////////
// Look for our PCI device
int xDetectPCICard(void)
{
    // PCI parameters...
    UINT8 hardware_mechanism, ldevice, lbus;
    UINT16 interface_level_version;
    UINT8 last_pci_bus_number, status;
    UINT16 vendor_id;

    // caller should already have checked for PCI BIOS
    if ( !pciBiosPresent( &hardware_mechanism,
                          &interface_level_version,
                          &last_pci_bus_number)) {
        printf("No PCI BIOS found!\n");
        return PCI_NOT_SUCCESSFUL;
    }

    for( lbus = 0; lbus <= last_pci_bus_number; lbus++ )
    {
        for( ldevice = 0; ldevice != 0x20; ldevice ++ )
        {
            // top 5 bits is device number
            // bottom 3 bits are function

            status = pciConfigRead16( lbus, ldevice<<3, PCI_CS_DEVICE_ID, &vendor_id );

            if ( status != PCI_SUCCESSFUL )
                continue;                               // not found

            if ( vendor_id == 0xFFFF ) continue;    // not valid

            if( pciReadCFG( lbus, ldevice<<3, &pci ) != PCI_SUCCESSFUL) 
            {
                printf("Error reading PCI buffer.\n");
                exit(NOT_SUCCESSFUL);
            }

            if ((pci.ven_id == HERMES_VENDOR_ID)
            && ( pci.dev_id == HERMES_DEVICE_ID || pci.dev_id == HERMES_DEVICE_ID_PROTO || OptIgnoreDevId)) {
                printf( "Found a Hermes..\n" );
                if (pci.dev_id == HERMES_DEVICE_ID_PROTO) {
                    printf("WARNING: apparently a prototype!\n");
                }

                PciCard.bus = lbus;
                PciCard.device = ldevice << 3;
                PciCard.type = AMCC_5920;
                PciCard.device_id = pci.dev_id;
                PciCard.vendor_id = pci.ven_id;
                PciCard.subsys_vendor_id = pci.sv_id;
                PciCard.subsys_id = pci.s_id;
                PciCard.revision_id = pci.rev;
                //PciCard.class_code

                GetRegionInfo( &PciCard );

                return PCI_SUCCESSFUL;
            } else {
                // qqq we could print a more verbose list of cards here
            }
        }
    }

    return PCI_NOT_SUCCESSFUL;
}

void ScanPCI(void)
{
    // PCI parameters...
    UINT8 hardware_mechanism, ldevice, lbus;
    UINT16 interface_level_version;
    UINT8 last_pci_bus_number, status;
    UINT16 vendor_id;

    // caller should already have checked for PCI BIOS
    if ( !pciBiosPresent( &hardware_mechanism,
                          &interface_level_version,
                          &last_pci_bus_number)) {
        printf("No PCI BIOS found!\n");
        return;
    }

    printf("Bus      Number   Vendor Id      Device Id\n");
    printf("==========================================\n");

    for( lbus = 0; lbus <= last_pci_bus_number; lbus++ )
    {
        for( ldevice = 0; ldevice != 0x20; ldevice ++ )
        {
            // top 5 bits is device number
            // bottom 3 bits are function

            status = pciConfigRead16( lbus, ldevice<<3, PCI_CS_DEVICE_ID, &vendor_id );

            if ( status != PCI_SUCCESSFUL )
                continue;

            if ( vendor_id == 0xFFFF ) continue;

            if( pciReadCFG( lbus, ldevice<<3, &pci ) != PCI_SUCCESSFUL) 
            {
                printf("Error reading PCI buffer.\n");
                return;
            }

            printf("%02d       %02d       %04x            %04x  %s\n",
                   lbus, ldevice, pci.ven_id, pci.dev_id,
                  (pci.ven_id == HERMES_VENDOR_ID && (pci.dev_id == HERMES_DEVICE_ID || pci.dev_id == HERMES_DEVICE_ID_PROTO))?
                  ((pci.dev_id == HERMES_DEVICE_ID)? "(Our hero!)" : "(Our prototype!)") :
                   "");
        }
    }
}



/////////////////////////////////////////////////////////////////////////
int GetRegionInfo( struct _chip *chip )
{
    int region;
    UINT8 status;
    UINT32 offset, data32, save_data32;

    for ( region = 0; region < NUM_BASE_REGIONS; region++ )
    {
        chip->reg[region].size = 0L;
        chip->reg[region].base = 0L;

        // Save current region base...
        offset = PCI_CS_BASE_ADDRESS_0 + (4 * region);

        // Read the base address assigned by bios
        status = pciConfigRead32( chip->bus, chip->device, offset, &save_data32);
        if ( status != PCI_SUCCESSFUL )
            return( status );

        // Write 0xFFFFFFFF, then read to get size...
        status = pciConfigWrite32( chip->bus, chip->device, offset, 0xFFFFFFFFL );
        if ( status != PCI_SUCCESSFUL )
            return( status );

        // Read ~size
        status = pciConfigRead32( chip->bus, chip->device, offset, &data32 );
        if ( status != PCI_SUCCESSFUL )
            return( status );

        // Restore
        status = pciConfigWrite32( chip->bus, chip->device, offset, save_data32 );
        if ( status != PCI_SUCCESSFUL )
            return( status );

        if ( !data32 ) continue;                            // disabled

        chip->reg[region].base = save_data32;

        if ( data32 & 1L )
        {
            data32 &= 0xFFFFFFFCL;      // drop out don't care bits for I/O
            //          chip->reg[region].IOSpace = 1;
        }
        else
        {
            data32 &= 0xFFFFFFF0L;      // drop out don't care bits for memory
            //          chip->reg[region].IOSpace = 0;
        }

        data32 = ~data32;
        data32++;
        chip->reg[region].size = data32;

        printf( "Current configuration for Region %d: Size = %8.8lX, Base = %8.8lX\n",
                region, chip->reg[region].size, chip->reg[region].base );

    }

    return PCI_SUCCESSFUL;
}

/****************************************************************************/
UINT8 pciConfigRead32 ( UINT8 bus, UINT8 device, UINT8 offset, UINT32 *ulong )
{
    int ret_status;
    UINT16 ax, flags;

    // must do AX last because compiler uses it for other stuff
    _BX = (bus << 8) | (device);
    _DI = (UINT16)offset;
    _AH = PCI_FUNCTION_ID;
    _AL = READ_CONFIG_DWORD;
    geninterrupt( 0x1A );

    // save regs before compiler overwrites...
    ax = _AX;
    flags = _FLAGS;
    *ulong = _ECX;

    if( (flags & CARRY_FLAG) == 0 )
        ret_status = HIGH_BYTE(ax);
    else ret_status = PCI_NOT_SUCCESSFUL;

    return( ret_status );
}

/****************************************************************************/
UINT8 pciConfigWrite32 ( UINT8 bus, UINT8 device, UINT8 offset, UINT32 ulong )
{
    int ret_status;
    UINT16 ax, flags;

    // must do AX last because compiler uses it for other stuff
    _BX = (bus << 8) | (device);
    _ECX = ulong;
    _DI = (UINT16)offset;
    _AH = PCI_FUNCTION_ID;
    _AL = WRITE_CONFIG_DWORD;
    geninterrupt( 0x1A );

    // save regs before compiler overwrites...
    ax = _AX;
    flags = _FLAGS;

    if( (flags & CARRY_FLAG) == 0 )
        ret_status = HIGH_BYTE(ax);
    else ret_status = PCI_NOT_SUCCESSFUL;

    return( ret_status );
}


/////////////////////////////////////////////////////////////////////
UINT8 pciBiosPresent(UINT8 *hardware_mechanism,
                     UINT16 *interface_level_version,
                     UINT8 *last_pci_bus_number)
{
    int ret_status;          /* Function Return Status. */
    byte bios_present_status;/* Indicates if PCI bios present */
    union REGS regs;

    regs.h.ah = PCI_FUNCTION_ID;
    regs.h.al = PCI_BIOS_PRESENT;
    int86( 0x1A, &regs, &regs );
    enable();


    /* Save registers before overwritten by compiler usage of registers */
    bios_present_status = regs.h.ah;


    /* First check if CARRY FLAG Set, if so, BIOS not present */
    if (!regs.x.cflag) 
    {
        /* Next, must check that AH (BIOS Present Status) == 0 */
        if (bios_present_status == 0) 
        {

            /* Check bytes in pci_signature for PCI Signature */
            if (regs.x.dx == 0x4350 ) 
            {

                /* Indicate to caller that PCI bios present */
                ret_status = TRUE;

                /* Extract calling parameters from saved registers */
                if (hardware_mechanism != NULL) 
                {
                    *hardware_mechanism = regs.h.al;
                    *interface_level_version = regs.x.bx;
                    *last_pci_bus_number = regs.h.cl;
                }
            }
        }

        else                /* if (bios_present_status != 0) */
            ret_status = FALSE;
    }

    else                        /* error since carry flag is set */
        ret_status = FALSE;

    return (ret_status);
}
