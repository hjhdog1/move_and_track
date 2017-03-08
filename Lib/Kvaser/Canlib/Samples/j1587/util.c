/* Help functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "canlib.h"
#include "j1587lib.h"


//--------------------------------------------------------------------
void PrintHwType (int t, int chan)
{
    switch(t)
    {
        case canHWTYPE_VIRTUAL:
            printf("Using Virtual channel %d\n", chan);
            break;
        case canHWTYPE_LAPCAN:
            printf("Using LAPcan channel %d\n", chan);
            break;
        case canHWTYPE_PCCAN:
            printf("Using PCcan channel %d\n", chan);
            break;
        case canHWTYPE_PCICAN:
            printf("Using PCIcan channel %d\n", chan);
            break;
        case canHWTYPE_USBCAN:
            printf("Using USBcan channel %d\n", chan);
            break;
        case canHWTYPE_PCICAN_II:
            printf("Using PCIcanI I channel %d\n", chan);
            break;
        case canHWTYPE_USBCAN_II:
            printf("Using USBcan II channel %d\n", chan);
            break;
        case canHWTYPE_LEAF:
            printf("Using Kvaser Leaf channel %d\n", chan);
            break;
        case canHWTYPE_MEMORATOR_PRO:
            printf("Using Kvaser Memorator Professional channel %d\n", chan);
            break;
        case canHWTYPE_USBCAN_PRO:
            printf("Using Kvaser USBcan Professional channel %d\n", chan);
            break;
        case canHWTYPE_USBCAN_LIGHT:
            printf("Using Kvaser USBcan Light channel %d\n", chan);
            break;
        default:
            printf("Channel %d is unknown or undefined\n", chan);
    }
}


//--------------------------------------------------------------------
void CheckAndPrintError (J1587Status stat, char * str) 
{
  //if (stat != j1587OK) {
    if (stat < j1587OK) {    
        char buffer[100];
        GetErrorText(stat, buffer, sizeof(buffer));
        printf("%s %s (stat = %d)\n", str, buffer, stat);
        exit(stat);
    }
}


//--------------------------------------------------------------------
void GetErrorText (J1587Status err, char* buf, int bufsiz)
{
    if (bufsiz < 52) return;


    switch (err) {
        case j1587ERR_NOMSG:
            sprintf(buf, "No messages available");
            break;
        case j1587ERR_NOTRUNNING:
            sprintf(buf, "j1587ERR_NOTRUNNING");
            break;
        case j1587ERR_RUNNING:
            sprintf(buf, "j1587ERR_RUNNING");
            break;
        case j1587ERR_NORMALONLY:
            sprintf(buf, "j1587ERR_NORMALONLY");
            break;
        case j1587ERR_NODEONLY:
            sprintf(buf, "j1587ERR_NODEONLY");
            break;
        case j1587ERR_PARAM:
            sprintf(buf, "Error in parameter");
            break;
        case j1587ERR_NOTFOUND:
            sprintf(buf, "Specified hw not found");
            break;
        case j1587ERR_NOMEM:
            sprintf(buf, "Out of memory");
            break;
        case j1587ERR_NOCHANNELS:
            sprintf(buf, "No channels avaliable");
            break;
        case j1587ERR_TIMEOUT:
            sprintf(buf, "Timeout ocurred");
            break;
        case j1587ERR_NOTINITIALIZED:
            sprintf(buf, "Lib not initialized");
            break;
        case j1587ERR_NOHANDLES:
            sprintf(buf, "Can't get handle");
            break;
        case j1587ERR_INVHANDLE:
            sprintf(buf, "Handle is invalid");
            break;
        case j1587ERR_CANERROR:
            sprintf(buf, "j1587ERR_CANERROR");
            break;
        case j1587ERR_ERRRESP:
            sprintf(buf, "There was an error response from the cable");
            break;
        case j1587ERR_WRONGRESP:
            sprintf(buf, "The cable response wasn't the expected one");
            break;
        case j1587ERR_DRIVER:
            sprintf(buf, "CAN driver type not supported");
            break;
        case j1587ERR_DRIVERFAILED:
            sprintf(buf, "DeviceIOControl failed; use Win32 GetLastError()");
            break;
        case j1587ERR_NOCARD:
            sprintf(buf, "The card was removed or not inserted");
            break;
        case j1587ERR_LICENSE:
            sprintf(buf, "The license is not valid.");
            break;
        case j1587ERR_INTERNAL:
            sprintf(buf, "Internal error in the driver.");
            break;
        case j1587ERR_NO_ACCESS:
            sprintf(buf, "Access denied");
            break;
        case j1587ERR_VERSION:
            sprintf(buf, "Function not supported in this version.");
            break;
        default:
            sprintf(buf, "Unknown error. ");
            break;
    }
    return;
}
