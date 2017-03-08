/*
 * This examples shows how to configure a device
 */
#include <stdio.h>
#include "canlib.h"
#include "kvrlib.h"


//------------------------------------------------------------------------------
// List all connected devices
//------------------------------------------------------------------------------
void listDevices (void)
{
  int i;
  canStatus status = canOK;
  char name[100];
  int channel_count = 0;

  canGetNumberOfChannels(&channel_count);

  printf("First argument must be a channel!\n\n");
  printf("Channel\t Name\n");
  printf("--------------------------------------------------------\n");
  for (i = 0; (status == canOK) && (i < channel_count); i++) {
    name[0] = '\0';
    status = canGetChannelData(i, canCHANNELDATA_CHANNEL_NAME, name, sizeof(name));
    printf("%d\t %s\n", i, name);
  } 
}

//------------------------------------------------------------------------------
// Can we configure the device on this channel without password?
//------------------------------------------------------------------------------
int isPasswordFree (unsigned int channelNumber)
{
  kvrStatus       status;
  kvrConfigHandle handle;

  status = kvrConfigOpen(channelNumber, kvrConfigMode_R, "", &handle);
  if (status != kvrOK) {
    printf("Failed to open configuration with empty password on channel %d\n", channelNumber);
    return 0;
  } else {
    kvrConfigClose(handle);
    return 1;
  }
}

//------------------------------------------------------------------------------
// Wait until device appears (or timout occurs)
// Returns 0 if successful
//------------------------------------------------------------------------------
int waitForDevice(unsigned int ean_hi, unsigned int ean_lo, unsigned int serial, 
                  int timeout_in_ms)
{
  unsigned long time_start;
  canStatus stat;

  printf("\nWaiting for device with EAN %08x%08x, and serial %d\n", ean_hi, ean_lo, serial);

  time_start = GetTickCount();
  Sleep(2000);
  
  do {
    int channel_count;
    DWORD tmp_serial[2];
    unsigned long tmp_ean[2];
    int channel;
    
    stat = canGetNumberOfChannels(&channel_count);
    if (stat != canOK) {
      printf("canGetNumberOfChannels() failed.\n");
      return -1;
    }
  
    for (channel=0; channel<channel_count; channel++) {
      stat = canGetChannelData(channel, canCHANNELDATA_CARD_UPC_NO, tmp_ean, sizeof(tmp_ean));
      if (stat != canOK) {
        printf("canGetChannelData(canCHANNELDATA_CARD_UPC_NO) failed.\n");
        return -2;
      }
    
      stat = canGetChannelData(channel, canCHANNELDATA_CARD_SERIAL_NO, tmp_serial, sizeof(tmp_serial));
      if (stat != canOK) {
        printf("canGetChannelData(canCHANNELDATA_CARD_SERIAL_NO) failed.\n");
        return -3;
      }
      
      //printf("%08x%08x %d\n", tmp_ean[1], tmp_ean[0], tmp_serial[0]);
      if (ean_hi == tmp_ean[1] && ean_lo == tmp_ean[0] && serial == tmp_serial[0]) {
        printf("Found!\n\n");
        return 0; // Found!
      }
    }
    
    printf("Try again...\n");
    Sleep(500);
  } while (GetTickCount() < (time_start + timeout_in_ms));
  
  printf("Device did not appear within given timeout\n\n");
  return -4;
}

//------------------------------------------------------------------------------
// Can we configure the device on this channel (i.e. no-one else is using it)?
//------------------------------------------------------------------------------
int isAvailibleForConfig (unsigned int canlib_channel, const char *password)
{
  int             can_hnd;
  canStatus       stat;
  DWORD           bus_type;
  DWORD           serial[2];
  DWORD           tmp_serial[2];
  unsigned long   ean[2];
  unsigned long   tmp_ean[2];
  int             chan_no;
  int             tmp_chan;
  kvrConfigHandle cfg_hnd;

  can_hnd = canOpenChannel(canlib_channel, canOPEN_EXCLUSIVE);
  if (can_hnd < 0) {
    printf("Channel %d can not be opened exclusively.\n", canlib_channel);
    return 0;
  } 
  
  
  stat = canIoCtl(can_hnd, canIOCTL_GET_BUS_TYPE, &bus_type, sizeof(bus_type));
  if (stat) {
    printf("ERROR: failed to get bustype %d\n", stat);
    canClose(can_hnd);
    return 0;
  }

  if (bus_type != kvBUSTYPE_GROUP_LOCAL) {
    printf("Channel is not local (bus type:%d).\n", bus_type);
    canClose(can_hnd);
    return 0;
  }
  canClose(can_hnd);
  
  
  // 
  // check all channels on a given device
  // 

  stat = canGetChannelData(canlib_channel, canCHANNELDATA_CARD_UPC_NO, ean, sizeof(ean));
  stat = canGetChannelData(canlib_channel, canCHANNELDATA_CARD_SERIAL_NO, serial, sizeof(serial));
  stat = canGetChannelData(canlib_channel, canCHANNELDATA_CHAN_NO_ON_CARD, &chan_no, sizeof(chan_no));  

  tmp_chan = canlib_channel - chan_no;
  while (1) {

    can_hnd = canOpenChannel(tmp_chan, canOPEN_EXCLUSIVE);
    if (can_hnd < 0) {
      printf("Channel %d (same device as channel %d) can not be opened exclusively.\n", tmp_chan, canlib_channel);
      return 0;
    } 
    canClose(can_hnd);  
    
    stat = canGetChannelData(++tmp_chan, canCHANNELDATA_CARD_UPC_NO, tmp_ean, sizeof(tmp_ean));
    stat = canGetChannelData(tmp_chan, canCHANNELDATA_CARD_SERIAL_NO, tmp_serial, sizeof(tmp_serial));
    if (tmp_ean[0] != ean[0] || tmp_ean[1] != ean[1] || 
        tmp_serial[0] != serial[0] || tmp_serial[1] != serial[1]) {
      break;
    }
  }  
  
  canClose(can_hnd);

  if (isPasswordFree(canlib_channel)) {
    printf("No password is needed for configuring channel %d\n", canlib_channel);

    // This test will remove the device from Kvaser Hardware
    stat = kvrConfigOpen(canlib_channel, kvrConfigMode_RW, "", &cfg_hnd);
    if (stat != kvrOK) {
      printf("Failed to open configuration with empty password on channel %d\n", canlib_channel);
      return 0;
    } else {
      kvrConfigClose(cfg_hnd);
      // Wait for the device to reappear in Kvaser Hardware      
      return 0 == waitForDevice(ean[1], ean[0], serial[0], 10000); // 10s timeout;
    }
  } else {
    printf("Password is needed for configuring channel %d\n", canlib_channel);

    // This test will remove the device from Kvaser Hardware
    stat = kvrConfigOpen(canlib_channel, kvrConfigMode_RW, password, &cfg_hnd);
    if (stat != kvrOK) {
      printf("Failed to open configuration with supplied password '%s' on channel %d\n", password, canlib_channel);
      return 0;
    } else {
      kvrConfigClose(cfg_hnd);
      // Wait for the device to reappear in Kvaser Hardware
      return 0 == waitForDevice(ean[1], ean[0], serial[0], 10000); // 10s timeout;
    }
  }
  
  return 1;  
}  

//------------------------------------------------------------------------------
// Scan for available networks and print some information about them
//------------------------------------------------------------------------------
kvrStatus doScanNetworks (kvrConfigHandle handle)
{
  kvrStatus status;
  kvrStatus stat;

  int32_t active   = 0;          // is a passive scan
  int32_t bss_type = kvrBss_ANY; // infrastructure and adhoc
  int32_t domain   = kvrRegulatoryDomain_WORLD;

  char ssid[40];
  char securityString[200];
  int32_t rssi;
  int32_t channel;
  kvrAddress mac;
  uint32_t capability;
  uint32_t type_wpa;
  kvrCipherInfoElement wpa_info;
  kvrCipherInfoElement rsn_info;
  
  status = kvrWlanStartScan(handle, active, bss_type, domain);
  if (status != kvrOK) {
    printf("Could not start scan (%d)\n", status);
    return status; 
  }

  do {
    status = kvrWlanGetScanResults(handle, &rssi, &channel, &mac, &bss_type,
                                   ssid, &capability, &type_wpa,
                                   &wpa_info, &rsn_info );    
    if (status == kvrOK) {
      printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
      printf("SSID: %s\n", ssid);
      printf("RSSI: %d dBm\n", rssi);
      printf("WLAN Channel: %d\n", channel);
      printf("MAC address: %02X %02X %02X %02X %02X %02X\n", 
             mac.address[0], mac.address[1], mac.address[2], 
             mac.address[3], mac.address[4], mac.address[5] );
      printf("BSS type: %d\n", bss_type);
      printf("Capabilities: 0x%04x\n", capability);
      
      // Convert to string
      stat = kvrWlanGetSecurityText(securityString, sizeof(securityString),
                                    capability, type_wpa, &wpa_info, &rsn_info);
      printf("Security");
      if (stat == kvrERR_PARAMETER) {
        printf("(truncated)");
      }
      printf(": %s\n", securityString);
    }
  } while ((status == kvrOK) || (status == kvrERR_NO_ANSWER));
  
  // kvrERR_BLANK => no more networks => OK
  return (status == kvrERR_BLANK ? kvrOK : status);
}


//------------------------------------------------------------------------------
// Configure a device
//------------------------------------------------------------------------------
kvrStatus doConfigure (kvrConfigHandle handle)
{
  kvrStatus status;
  char new_xml_config[4096];
  char old_xml_config[4096];
  char xml_error[2048];
  
  // Save the old configuration
  status = kvrConfigGet(handle, old_xml_config, sizeof(old_xml_config));
  if (status != kvrOK) {
    printf("Could not read configuration from device (%d)\n", status);
    kvrConfigClose(handle);
    return status; 
  }
  printf("Old configuration: %s\n", old_xml_config);
 
  // Adjust settings in XML file based on data from doScanNetworks()
  // ...
  memcpy(new_xml_config, old_xml_config, sizeof(old_xml_config));

  // Check that the new configuration is valid
  memset(xml_error, 0, sizeof(xml_error));
  status = kvrConfigVerifyXml(new_xml_config, xml_error, sizeof(xml_error));
  if (status != kvrOK) {
    printf("The XML configuration is not valid (%d):\n%s\n",
    status, xml_error);
    kvrConfigClose(handle);
    return status;
  }
   
  // Download new configuration
  status = kvrConfigSet(handle, old_xml_config); 
  if (status != kvrOK) {
    printf("Could not write configuration to device (%d)\n", status);
    return status; 
  }

  return status;
}


//------------------------------------------------------------------------------
// Try configuration
//------------------------------------------------------------------------------
kvrStatus doTryConfiguration (kvrConfigHandle handle, int seconds)
{
  kvrStatus status;

  kvrAddress address;  
  kvrAddress mac; 
  kvrAddress netmask;  
  kvrAddress gateway;
  int32_t dhcp;
  
  int32_t state;
  int32_t tx_rate;
  int32_t rx_rate;
  int32_t channel;
  int32_t rssi_mean;
  int32_t tx_power;
    
  kvrRssiHistory rssi[14] = {0};
  kvrRttHistory rtt[14] = {0};  
  int rtt_len = sizeof(rtt) / sizeof(kvrRttHistory);
  int rssi_len = sizeof(rssi) / sizeof(kvrRssiHistory);
  int rtt_actual;
  int rssi_actual;

  // connection test. 1 = activate ping
  status = kvrNetworkConnectionTest(handle, 1);
  if (status != kvrOK) {
    printf("Could not start ping(%d)\n", status);
    return status; 
  }
  
  do {
    Sleep(1000);
    // Ask for RSSI and RTT so that we get updated 
    // values when calling kvrNetworkGetConnectionStatus()
    status = kvrNetworkGetRssiRtt(handle, rssi, rssi_len, &rssi_actual,
                                  rtt, rtt_len, &rtt_actual);
    if (status != kvrOK) {
      printf("Could not get RSSI / RTT (%d)\n", status);
      break;
    }

    status = kvrNetworkGetConnectionStatus(handle, &state, &tx_rate, &rx_rate,
                                           &channel, &rssi_mean, &tx_power); 
    if (status != kvrOK) {
      printf("Could not get status (%d)\n", status);
      break;
    }

    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("Connection state: %d\n", state);
    printf("Transmit rate: %d kbit/s\n", tx_rate);
    printf("Receive rate: %d kbit/s\n", rx_rate);
    printf("Channel: %d\n", channel);
    printf("Receive Signal Strength Indicator: %d dBm\n", rssi_mean);
    printf("Transmit power level: %d dB\n", tx_power);
  } while (--seconds > 0);

  // connection test. 0 = deactivate ping
  status = kvrNetworkConnectionTest(handle, 0);
  if (status != kvrOK) {
    printf("Could not stop ping(%d)\n", status);
    return status; 
  }
  
  status = kvrNetworkGetAddressInfo(handle, &address, &mac, &netmask, &gateway, &dhcp);
  if (status != kvrOK) {
    printf("Could not get IP info(%d)\n", status);
    return status; 
  }
  
  // Assume IP v.4, i.e. address/netmask/gateway.type is kvrAddressType_IPV4
  printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
  printf("DHCP: %s\n", dhcp ? "ON" : "OFF");
  printf("MAC address: %02X %02X %02X %02X %02X %02X\n", 
         mac.address[0], mac.address[1], mac.address[2], 
         mac.address[3], mac.address[4], mac.address[5] );
  printf("IP Address: %d.%d.%d.%d\n", address.address[0], address.address[1],
    address.address[2], address.address[3]);    
  printf("Netmask: %d.%d.%d.%d\n", netmask.address[0], netmask.address[1],
    netmask.address[2], netmask.address[3]);
  printf("Gateway: %d.%d.%d.%d\n", gateway.address[0], gateway.address[1],
    gateway.address[2], gateway.address[3]);
    
  
  return status;
}


//------------------------------------------------------------------------------
// Scan for available networks, configure a local (USB) device, and use the new
// configuration to establish a connection to WLAN.
//------------------------------------------------------------------------------
int main (int argc, char *argv[])
{
  char *password = ""; //"Secret";
 
  kvrConfigHandle handle;
  kvrStatus status;
  int canlib_channel;
    
  DWORD serial[2];
  unsigned long ean[2];
  canStatus stat;  
  
  // Initialize kvrlib
  kvrInitializeLibrary();
  canInitializeLibrary();
  
  switch (argc) {
  case 1:
    listDevices();
    return 0;
  case 2:
    canlib_channel = argv[1][0] - '0';
    break;
  default:
    listDevices();
    return -1;
  }
  
  
  stat = canGetChannelData(canlib_channel, canCHANNELDATA_CARD_UPC_NO, ean, sizeof(ean));
  stat = canGetChannelData(canlib_channel, canCHANNELDATA_CARD_SERIAL_NO, serial, sizeof(serial));
  
  //----------------------------------------------------------------------------
  // Check configuration status 

  // Can we configure the device?
  if (isAvailibleForConfig (canlib_channel, password)) {
    printf("Channel %d is availible for configuration\n", canlib_channel);
  } else {
    printf("Channel %d can not be opened for configuration.\n", canlib_channel);
    canUnloadLibrary();
    return -1;
  }
  canUnloadLibrary();

  //----------------------------------------------------------------------------
  // Start configuration - read only
  status = kvrConfigOpen(canlib_channel, kvrConfigMode_R, password, &handle);
  if (status != kvrOK) {
    printf("Could not start config (%d)\n", status);
    return status; 
  }

  // List available networks. This information could be 
  // helpful when creating the XML configuration.
  status = doScanNetworks(handle);	
  if (status != kvrOK) {
    printf("Scan networks failed (%d)\n", status);
    kvrConfigClose(handle);
    return status;
  }

  kvrConfigClose(handle);

  //----------------------------------------------------------------------------
  // Start configuration - read/write

  status = kvrConfigOpen(canlib_channel, kvrConfigMode_RW, password, &handle);
  if (status != kvrOK) {
    printf("Could not start config (%d)\n", status);
    return status; 
  }

  // Configure the device by writing the new XML configuration
  status = doConfigure(handle);
  if (status != kvrOK) {
    printf("Could not write new configuration (%d)\n", status);
    kvrConfigClose(handle);
    return status;
  }
  
  // Done!
  kvrConfigClose(handle);

  // Wait for reboot
  if (waitForDevice(ean[1], ean[0], serial[0], 10000) != 0) { //10s timeout
    printf("waitForDevice() failed.\n");
    return -1;
  }
  
  //----------------------------------------------------------------------------
  // Start configuration - read only
  status = kvrConfigOpen(canlib_channel, kvrConfigMode_R, password, &handle);
  if (status != kvrOK) {
    printf("Could not start config (%d)\n", status);
    return status; 
  }
  
  // Test the new configuration for 5 s
  status = doTryConfiguration(handle, 5);
  if (status != kvrOK) {
    printf("doTryConfiguration failed (%d)\n", status);
    return status;
  }
  
  kvrConfigClose(handle);
  
  printf("\nDone!\n");
  
  kvrUnloadLibrary();
  return 0;
}
