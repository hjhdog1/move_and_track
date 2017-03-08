/*
 * This examples shows how to find a device on your network.
 */

#include <stdio.h>
#include "canlib.h"
#include "kvrlib.h"

#ifndef MIN
# define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
# define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif


char password[64] = "Hello World!";


char *getUsage (int usage)
{
  static char *free    = "FREE";
  static char *remote  = "REMOTE";
  static char *usb     = "USB";
  static char *config  = "CONFIG";
  static char *unknown = "UNKNOWN";

  switch (usage) {
    case kvrDeviceUsage_FREE:   return free;
    case kvrDeviceUsage_REMOTE: return remote;
    case kvrDeviceUsage_USB:    return usb;
    case kvrDeviceUsage_CONFIG: return config;
    default:              return unknown;
  }
}


int isUsedByMe (kvrDeviceInfo *di)
{
  int i;
  int channel_count;
  canStatus status;
  char ean[8];
  unsigned int ean_hi = 0;
  unsigned int ean_lo = 0;
  unsigned int serial = 0;
  
  canInitializeLibrary();

  status = canGetNumberOfChannels(&channel_count);
  if (status != canOK) {
    printf("ERROR: canGetNumberOfChannels failed %d\n", status);
    return -1;
  }
     
  for (i = 0; (status == canOK) && (i < channel_count); i++) {
    status = canGetChannelData(i, canCHANNELDATA_CARD_UPC_NO, ean, sizeof(ean));
    if (status != canOK) {
      printf("ERROR: canCHANNELDATA_CARD_UPC_NO failed: %d\n", status);
      return -1;
    }
    ean_hi  = (ean[7] << 24) & 0xFF000000;
    ean_hi += (ean[6] << 16) & 0x00FF0000;
    ean_hi += (ean[5] << 8)  & 0x0000FF00;
    ean_hi += ean[4]         & 0x000000FF;
    ean_lo  = (ean[3] << 24) & 0xFF000000;
    ean_lo += (ean[2] << 16) & 0x00FF0000;
    ean_lo += (ean[1] << 8)  & 0x0000FF00;
    ean_lo += ean[0]         & 0x000000FF;

    status = canGetChannelData(i, canCHANNELDATA_CARD_SERIAL_NO,
                               &serial, sizeof(serial));      
    if (status != canOK) {
      printf("ERROR: canCHANNELDATA_CARD_SERIAL_NO failed: %d\n", status);
      return -1;
    }
    
    if ((di->ean_lo == ean_lo) && (di->ean_hi == ean_hi) &&
        (di->ser_no == serial))
    {
      return 1;
    }
  }

  return 0;
}


//------------------------------------------------------------------------------
// Dump information 
//------------------------------------------------------------------------------
void dumpDeviceInfo (kvrDeviceInfo *di)
{
  char      service_text[256];
  char      buf[256];  
  kvrStatus status;
  char      addr_buf[22];
  int       i;
  int32_t   service_state;
  int32_t   service_sub_state;
  
  printf("--------------------------------------------------------------------------\n");
  printf("Device information\n");
  printf("EAN:         %x%x\n", di->ean_hi, di->ean_lo);
  printf("FW version:  %d.%d.%d\n", 
         di->fw_major_ver, di->fw_minor_ver, di->fw_build_ver);
  printf("Serial:      %ld\n", di->ser_no);
  printf("Name:        %s\n", di->name);
  printf("Host name:   %s\n", di->host_name);
  printf("Password:    ");
  if (!di->accessibility_pwd[0]) {
    printf("None\n");
  } else {
    for(i = 0; i < MIN(32, di->accessibility_pwd[0]); i++) {
      if (i == 16) {
       printf("\n             ");
      }
      printf("%02x ", (uint8_t)di->accessibility_pwd[i + 3]);
    }
    printf(" (%02x%02x)\n", 
           (uint8_t)di->accessibility_pwd[2], (uint8_t)di->accessibility_pwd[1]);
  }
  kvrStringFromAddress(addr_buf, sizeof(addr_buf), &di->device_address);
  printf("IP:          %s\n", addr_buf);
  kvrStringFromAddress(addr_buf, sizeof(addr_buf), &di->client_address);
  printf("Client IP:   %s\n", addr_buf);
  printf("Usage:       %s", getUsage(di->usage));
  if ((di->usage != kvrDeviceUsage_FREE) && (isUsedByMe(di) > 0)) {
    printf(" - Used by Me!\n");
  } else if (di->usage != kvrDeviceUsage_FREE && di->usage != kvrDeviceUsage_UNKNOWN) {
    printf(" - Used by other!\n");
  } else {
    printf("\n");
  }
  printf("Alive:       %s\n", 
         (di->availability & kvrAvailability_FOUND_BY_SCAN ? "Yes" : "No"));
  printf("Stored:      %s\n", (di->availability & kvrAvailability_STORED ? "Yes" : "No"));

  // Ask service for status service_text
  status = kvrDeviceGetServiceStatusText(di, service_text,
                                         sizeof(service_text));
  if (status != kvrOK) {
    kvrGetErrorText(status, buf, sizeof(buf));
    printf("Service:     FAILED - %s\n", buf);      
  } else if (strncmp(service_text, "Service: ", strlen("Service: ")) == 0) {
    printf("Service:     %s\n", &service_text[strlen("Service: ")]);      
  } else {
    printf("%s\n", service_text);      
  }
  
  status = kvrDeviceGetServiceStatus(di, &service_state, &service_sub_state);
  if (status == kvrOK) {
    printf("service_state: %d.%d\n", service_state, service_sub_state);
  } else {
    printf("service_state: unknown\n");
  }
}


//------------------------------------------------------------------------------
// Broadcast for all devices of a specific EAN and add them to the device list
// Note that the device list could have some devices in it already.
//------------------------------------------------------------------------------
kvrStatus setupBroadcast (kvrDiscoveryHandle handle)
{
  char        buf[256];
  kvrStatus   status;  
  kvrAddress  addr_list[10];
  uint32_t    no_addrs        = 0;
  uint32_t    i;

  status = kvrDiscoveryGetDefaultAddresses(addr_list, 
                                           sizeof(addr_list)/sizeof(kvrAddress),
                                           &no_addrs, 
                                           kvrAddressTypeFlag_ALL);  

  if (status != kvrOK) {
    kvrGetErrorText(status, buf, sizeof(buf));
    printf("kvrDiscoveryGetDefaultAddresses() FAILED - %s\n", buf);
    return status;
  }

  if (no_addrs < sizeof(addr_list)/sizeof(kvrAddress)) {
    status = kvrAddressFromString(kvrAddressType_IPV4, &addr_list[no_addrs], "10.0.3.66");
    if (status != kvrOK) {
      printf("ERROR: kvrAddressFromString(%d, 10.0.3.1) failed\n",no_addrs);
    } else {
      no_addrs++;
    }
  } else {
    printf("NOTE: We don't have room for all devices in addr_list[%d].\n",
           sizeof(addr_list)/sizeof(kvrAddress));
  }

  for (i=0; i < no_addrs; i++) {
    status = kvrStringFromAddress(buf, sizeof(buf), &addr_list[i]);
    printf("Looking for device using: %s\n", buf);
  }

  status = kvrDiscoverySetAddresses(handle, addr_list, no_addrs);
  if (status != kvrOK) {
    kvrGetErrorText(status, buf, sizeof(buf));
    printf("kvrDiscoverySetAddresses() FAILED - %s\n", buf);
    return status;
  }

  return status;
}


//------------------------------------------------------------------------------
// Discover devices and add them to device list
//------------------------------------------------------------------------------
kvrStatus discoverDevices (kvrDiscoveryHandle handle)
{
  kvrStatus status;
  kvrDeviceInfo device_info[64];
  int devices;
  uint32_t delay_ms = 500;
  uint32_t timeout_ms = 300;
  char        buf[256];
  
  status = kvrDiscoveryStart(handle, delay_ms, timeout_ms);
  if (status != kvrOK) {
    kvrGetErrorText(status, buf, sizeof(buf));
    printf("kvrDiscoveryStart() FAILED - %s\n", buf);
    return status;
  }
  
  devices = 0;
  while (status == kvrOK) {    
    status = kvrDiscoveryGetResults(handle, &device_info[devices]);
    if (status == kvrOK) {
      dumpDeviceInfo(&device_info[devices]);
      // Add some data and request store
      if (kvrDiscoverySetPassword(&device_info[devices], password) != kvrOK) {
        printf("Unable to set password: %s (%d)\n", password, strlen(password));
      }
      // Here we can decide to connect to the device
      //device_info[devices].request_connection = 1;
      devices++;
    } else {
      if (status != kvrERR_BLANK) {
        printf("kvrDiscoveryGetResults() failed %d\n", status );
      }
    }
  }

  status = kvrDiscoveryStoreDevices(device_info, devices);
  if (status != kvrOK) {
    kvrGetErrorText(status, buf, sizeof(buf));
    printf("Device store failed: %s\n", buf);
  }

  return kvrOK;
}




//------------------------------------------------------------------------------
// Setup a number of WLAN devices for future CANLIB use
//------------------------------------------------------------------------------
int main (int argc, char *argv[])
{
  kvrStatus status;
  kvrDiscoveryHandle handle;
  char        buf[256];

  if (argc > 1) {
    strcpy(password, argv[1]);
  }

  kvrInitializeLibrary();

  status = kvrDiscoveryOpen(&handle);
  if (status != kvrOK) {
    kvrGetErrorText(status, buf, sizeof(buf));
    printf("kvrDiscoveryOpen() FAILED - %s\n", buf);
    return status;
  }
 
  status = setupBroadcast(handle);
  if (status != kvrOK) {
    kvrGetErrorText(status, buf, sizeof(buf));
    printf("setupBroadcast() FAILED - %s\n", buf);
    return status;
  }
  status = discoverDevices(handle);
  if (status != kvrOK) {
    kvrGetErrorText(status, buf, sizeof(buf));
    printf("discoverDevices() FAILED - %s\n", buf);
    return status;
  }
  
  kvrDiscoveryClose(handle);
  kvrUnloadLibrary();

  return 0;
}

