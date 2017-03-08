/*
 * This example shows how to get RSSI and RTT values for a connection.
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

  canInitializeLibrary();
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
// Get RSSI and RTT values
//------------------------------------------------------------------------------
void getRssiRtt (kvrConfigHandle handle, int *rssi_mean, int *rtt_mean)
{
  kvrStatus status;
  kvrRssiHistory rssi[14] = {0};
  kvrRttHistory rtt[14] = {0};  
  int rtt_len = sizeof(rtt) / sizeof(kvrRttHistory);
  int rssi_len = sizeof(rssi) / sizeof(kvrRssiHistory);
  int rtt_actual;
  int rssi_actual;

  *rssi_mean = 0;
  *rtt_mean = 0;
  
  status = kvrNetworkGetRssiRtt(handle, rssi, rssi_len, &rssi_actual,
                                rtt, rtt_len, &rtt_actual);
  if (status == kvrOK) {
    int i;
    printf("RSSI (%d):", rssi_actual);
    for(i = 0; i < rssi_actual; i++) {
      *rssi_mean += rssi[i];
      printf(" %d", rssi[i]);
    }
    printf("\nRTT (%d):", rtt_actual);
    for(i = 0; i < rtt_actual; i++) {
      *rtt_mean += rtt[i];
      printf(" %d", rtt[i]);
    }
    
    printf("\n");
    
    if (rssi_actual) {
      *rssi_mean = *rssi_mean / rssi_actual;
    }
    
    if (rtt_actual) {
      *rtt_mean = *rtt_mean / rtt_actual;
    }
  }
}


//------------------------------------------------------------------------------
// kvrNetworkConnectionTest.exe <channel>
//------------------------------------------------------------------------------
int main (int argc, char *argv[])
{
  kvrConfigHandle config_handle;
  kvrStatus status;
  int canlib_channel = 0;
  int j;
  int rssi_mean, rtt_mean;
  kvrInitializeLibrary();

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

  printf("canlib channel = %d\n", canlib_channel);
  
  status = kvrConfigOpen(canlib_channel, kvrConfigMode_R, "", &config_handle);
  if (status != kvrOK) {
    printf("Could not start config (%d)\n", status);
    return status; 
  }

  printf("config_handle = %d\n\n", config_handle);
  
  printf("kvrNetworkConnectionTest( on )\n\n");
  status = kvrNetworkConnectionTest(config_handle, 1); // Start sending pings.
  if (status != kvrOK) {
    printf("Could not start connection test (%d)\n", status);
    return status; 
  }

  Sleep(2000); // Wait for the device to connect.

  for (j = 0; j < 10; j++) {
    Sleep(2000); // Wait for some pings to be sent.
    getRssiRtt(config_handle, &rssi_mean, &rtt_mean);
    printf("rssi_mean = %d\n", rssi_mean);    
    printf("rtt_mean = %d\n\n", rtt_mean);   
  }
  
  status = kvrNetworkConnectionTest(config_handle, 0); // Stop sending pings.
  printf("Done!\n");
  kvrConfigClose(config_handle);

  return 0;
}
