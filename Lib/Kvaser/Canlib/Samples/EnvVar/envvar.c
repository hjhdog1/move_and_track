/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**  Demo program that demonstrates environment variables in a script.
** ---------------------------------------------------------------------------
*/
#include <windows.h>
#include <stdio.h>
#include "canlib.h"

int   opt_chan;
char  opt_dir[1000];

HWND CreateHideWindow(void);

int main(int argc, char **argv)
{
  int i, h, stat;
  kvEnvHandle h_evInt, h_evUpdateCount;
  kvEnvHandle h_evBigOne, h_evChecksum;
  int evType, evSize, evIntData, evUpdateCountData, tmp;
  HWND hwnd;
  char scriptfile[1000];
  unsigned char data[64];

  opt_chan = 0;
  opt_dir[0] = 0;
  
  for (i=1; i<argc; i++) {
    int tmp;
    char tmpS[1000];
    if (sscanf(argv[i], "-chan=%d%c", &tmp) == 1) {
      opt_chan = tmp;
    } else if (sscanf(argv[i], "-scriptdir=%s", tmpS) == 1) {
      strcpy(opt_dir, tmpS);
    }
  }

  canInitializeLibrary();

  // First, check that the device is really an Eagle.
  stat = canGetChannelData(opt_chan, canCHANNELDATA_CARD_TYPE, &tmp, sizeof(tmp));
  if (stat < 0) {
    fprintf(stderr, "canGetChannelData failed, stat=%d\n", stat);
    exit(1);
  }
  if (tmp != canHWTYPE_EAGLE) {
    fprintf(stderr, "Warning: this is not a Kvaser Eagle. Script operations will probably fail.\n");
  }

  
  // Open a handle to the device
  h = canOpenChannel(opt_chan, 0);
  if (h < 0) {
    fprintf(stderr, "canOpenChannel failed, stat=%d\n", h);
    exit(1);
  }
  

  // Load a compiled script file into slot 0.
  // (If this call returns -32, then you are probaly using a hardware
  // without support for scripts)
  scriptfile[0] = '\0';
  if (strlen(opt_dir) > 0) {
    strcpy(scriptfile, opt_dir);
    strcat(scriptfile, "\\");
  }
  strcat(scriptfile, "test1.txe");

  // Just in case someone has left a started script in this slot. This
  // will not kill a looping script, but it will stop a cooperative
  // script.
  (void)kvScriptStop(h, 0, kvSCRIPT_STOP_NORMAL);

  // Now load the script in slot 0.
  stat = kvScriptLoadFile(h, 0, scriptfile);
  if (stat < 0) {
    fprintf(stderr, "kvScriptLoadFile(%s) failed, stat=%d\n", scriptfile, stat);
    exit(1);
  }

  // Start the script in slot 0.
  stat = kvScriptStart(h, 0);
  if (stat < 0) {
    fprintf(stderr, "kvScriptStart failed, stat=%d\n", stat);
    exit(1);
  }
  
  // For notifications when an envvar is changed, we need an ivisible
  // window.
  hwnd = CreateHideWindow();

  // Setup the notification.
  stat = canSetNotify(h, hwnd, canNOTIFY_ENVVAR);
  if (stat < 0) {
    fprintf(stderr, "canSetNotify failed, stat=%d\n", stat);
  }

  // Open an envvar in the loaded script. h_evInt will be the handle to the
  // envvar.
  h_evInt = kvScriptEnvvarOpen(h, "evInt", &evType, &evSize);
  if (h_evInt < 0) {
    fprintf(stderr, "kvScriptEnvvarOpen(evInt) failed, stat=%d\n", h_evInt);
    exit(1);
  }

  // Some sanity checks.
  if (evType != kvENVVAR_TYPE_INT) {
    fprintf(stderr, "Suspect type, assumed int (1) got %d\n", evType);
  }
  if (evSize != (signed)sizeof(int)) {
    fprintf(stderr, "Suspect size, assumed 4 got %d\n", evSize);
  }

  // Open another envvar.
  h_evUpdateCount = kvScriptEnvvarOpen(h, "evUpdateCount", &evType, &evSize);
  if (h_evUpdateCount < 0) {
    fprintf(stderr, "kvScriptEnvvarOpen(updateCount) failed, stat=%d\n", h_evUpdateCount);
    exit(1);
  }
  if (evType != kvENVVAR_TYPE_INT) {
    fprintf(stderr, "Suspect type, assumed int (1) got %d\n", evType);
  }
  if (evSize != (signed)sizeof(int)) {
    fprintf(stderr, "Suspect size, assumed 4 got %d\n", evSize);
  }


  evIntData = 12345;
  
  for (i=0; i<100; i++) {

    // Since we are using notifications via a window, run the standard
    // Windows message loop first.
    for (;;) {
      MSG hWnd_msg;

      if (PeekMessage(&hWnd_msg, hwnd, 0, 0, TRUE) != 0) {
        TranslateMessage(&hWnd_msg);
        DispatchMessage(&hWnd_msg);
      } else
        break;
    } 

    // Set data in the first envvar.
    stat = kvScriptEnvvarSetData(h_evInt, (unsigned char*)&evIntData, 0, sizeof(int));
    if (stat < 0) {
      fprintf(stderr, "kvScriptEnvvarSetData failed, stat=%d\n", stat);
      exit(1);
    }

    // Read it back.
    stat = kvScriptEnvvarGetData(h_evInt, (unsigned char*)&evIntData, 0, sizeof(int));
    if (stat < 0) {
      fprintf(stderr, "kvScriptEnvvarGetData failed, stat=%d\n", stat);
      exit(1);
    }

    // Read the second envar, which is incrented in the script each
    // time the first envvar is updated.
    stat = kvScriptEnvvarGetData(h_evUpdateCount, (unsigned char*)&evUpdateCountData, 0, sizeof(int));
    if (stat < 0) {
      fprintf(stderr, "kvScriptEnvvarGetData failed (second), stat=%d\n", stat);
      exit(1);
    }

    // Print the status so far...
    printf("evIntData=%d\n", evIntData);
    printf("evUpdateCountData=%d\n", evUpdateCountData);
  }

  //
  // Open another envvar, this time a 64-byte array.
  //
  h_evBigOne = kvScriptEnvvarOpen(h, "evBigOne", &evType, &evSize);
  if (h_evBigOne < 0) {
    fprintf(stderr, "kvScriptEnvvarOpen(evBigOne) failed, stat=%d\n", h_evBigOne);
    exit(1);
  }
  // And yet another one.
  h_evChecksum = kvScriptEnvvarOpen(h, "evChecksum", &evType, &evSize);
  if (h_evChecksum < 0) {
    fprintf(stderr, "kvScriptEnvvarOpen(evChecksum) failed, stat=%d\n", h_evChecksum);
    exit(1);
  }

  // Fill the array with 1,2,3,...
  for (i=0; i<64; i++) {
    data[i] = i;
  }
  stat = kvScriptEnvvarSetData(h_evBigOne, (unsigned char*)data, 0, sizeof(data));
  if (stat < 0) {
    fprintf(stderr, "kvScriptEnvvarSetData failed, stat=%d\n", stat);
    exit(1);
  }

  // When we wrote the envvar, the script calculated its "checksum" and
  // wrote it into the evChecksum environment variable. Now read that checksum back.
  evIntData = 0;
  stat = kvScriptEnvvarGetData(h_evChecksum, (unsigned char*)&evIntData, 0, sizeof(int));
  if (stat < 0) {
    fprintf(stderr, "kvScriptEnvvarGetData failed, stat=%d\n", stat);
    exit(1);
  }
  printf("Checksum=%d (expected %d)\n", evIntData, 63*64/2);
 
  
  // Cleanup.
  stat = kvScriptEnvvarClose(h_evInt);
  if (stat < 0) {
    fprintf(stderr, "kvScriptEnvvarClose(1) failed, stat=%d\n", stat);
    exit(1);
  }
  stat = kvScriptEnvvarClose(h_evUpdateCount);
  if (stat < 0) {
    fprintf(stderr, "kvScriptEnvvarClose(2) failed, stat=%d\n", stat);
    exit(1);
  }
  stat = kvScriptEnvvarClose(h_evBigOne);
  if (stat < 0) {
    fprintf(stderr, "kvScriptEnvvarClose(3) failed, stat=%d\n", stat);
    exit(1);
  }
  stat = kvScriptEnvvarClose(h_evChecksum);
  if (stat < 0) {
    fprintf(stderr, "kvScriptEnvvarClose(4) failed, stat=%d\n", stat);
    exit(1);
  }

  // Stop the script.
  stat = kvScriptStop(h, 0, kvSCRIPT_STOP_NORMAL);
  if (stat < 0) {
    fprintf(stderr, "kvScriptStop failed, stat=%d\n", stat);
    exit(1);
  }

  // Unload the script.
  stat = kvScriptUnload(h, 0);
  if (stat < 0) {
    fprintf(stderr, "kvScriptUnload failed, stat=%d\n", stat);
    exit(1);
  }

  // Close the handle.
  canClose(h);
  return 0;
}

// This routine is called each time a Windows message arrives to our
// invisible notification window. Note that it is not called
// magically in the background. It is called from the Windows message
// loop (PeekMessage et al) above.
//
// We could use kvSetNotifyCallback instead of canSetNotify; then the
// callback will be done in the context of another thread created by
// canlib. Each method has its merits and its pitfalls...
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM__CANLIB) {
    //
    // A canlib event has occured
    //
    WORD canEVENT = LOWORD(lParam);

    printf("Tallyho!\n");
    
    switch (canEVENT)
    {
      case canEVENT_TX:
        printf("CAN message sent\n");
        break;
      case canEVENT_RX:
        printf("CAN message received\n");
        break;
      case canEVENT_ERROR:
        printf("CAN message received\n");
        break;
      case canEVENT_STATUS:
        printf("CAN message received\n");
        break;
      case canEVENT_ENVVAR:
        printf("Envvar updated\n");
        break;
      default:
        printf("Unknown event %d\n", canEVENT);
        break;
    }
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// This routine creates an invisible window.
HWND CreateHideWindow(void)
{
  WNDCLASS window;
  HWND windowHandle;

  memset(&window, 0, sizeof(WNDCLASS));
  window.lpszClassName = "Accept Window";
  window.lpfnWndProc = (WNDPROC) WindowProc;	


  if (!RegisterClass(&window)) {
    fprintf(stderr, "Registerclass failed, stat=%d\n", GetLastError());
    return NULL;
  }

  windowHandle = CreateWindow(window.lpszClassName,
                              window.lpszClassName,
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              (HWND) NULL,
                              (HMENU) NULL,
                              (HINSTANCE) NULL,
                              (LPVOID) NULL);

  if (windowHandle == NULL) {
    fprintf(stderr, "CreateWindow failed, stat=%d\n", GetLastError());
    return NULL;
  }
  return windowHandle;
}
