#ifndef CANLIB_CLASS_H
#define CANLIB_CLASS_H

#include "canlib.h"

class CanMessage {
private:
protected:
public:
    long            id;
    char            msg[8];
    unsigned int    dlc;
    unsigned int    flag;
    DWORD           timeStamp;
};

class Canlib {
  private:
    int hnd;

  public:
    Canlib();
    
    void canInitializeLibrary(void);
        
    canStatus canOpenChannel(int channel, int flags);
    canStatus canSetBusParams(long freq, unsigned int tseg1, unsigned int tseg2,
                  unsigned int sjw, unsigned int noSamp, unsigned int syncmode);
    canStatus canGetBusParams(  long *freq,
                                unsigned int *tseg1,
                                unsigned int *tseg2,
                                unsigned int *sjw,
                                unsigned int *noSamp,
                                unsigned int *syncmode);

    canStatus canSetBusOutputControl(const unsigned int drivertype);
    canStatus canGetBusOutputControl(unsigned int * drivertype);
    canStatus canAccept(const long envelope, const unsigned int flag);
    canStatus canReadStatus(unsigned long * const flags);
    canStatus canReadErrorCounters(unsigned int * txErr,
                                    unsigned int * rxErr,
                                    unsigned int * ovErr);
    canStatus canWriteSync(unsigned long timeout);
    canStatus canReadWait(long * id,
                          void * msg,
                          unsigned int * dlc,
                          unsigned int * flag,
                          unsigned long * time,
                          unsigned long timeout);
    canStatus canReadSpecific(long id, void * msg,
                              unsigned int * dlc, unsigned int * flag,
                              unsigned long * time);
    canStatus canReadSync(unsigned long timeout);
    canStatus canReadSyncSpecific(long id, unsigned long timeout);
    canStatus canReadSpecificSkip(long id,
                                  void * msg,
                                  unsigned int * dlc,
                                  unsigned int * flag,
                                  unsigned long * time);
    canStatus canSetNotify(HWND aHWnd, unsigned int aNotifyFlags);
    canStatus canTranslateBaud(long * const freq,
                               unsigned int * const tseg1,
                               unsigned int * const tseg2,
                               unsigned int * const sjw,
                               unsigned int * const nosamp,
                               unsigned int * const syncMode);
    canStatus canGetErrorText(canStatus err, char * buf, unsigned int bufsiz);
    unsigned short canGetVersion(void);
    canStatus canIoCtl(unsigned int func, void * buf, unsigned int buflen);
    unsigned long canReadTimer(void);
    canStatus canGetNumberOfChannels(int * channelCount);
    canStatus canReadEvent(CanEvent *event);
    canStatus canSetBusParamsC200(BYTE btr0, BYTE btr1);
    canStatus canSetDriverMode(int lineMode, int resNet);
    canStatus canGetDriverMode(int *lineMode, int *resNet);

    unsigned int canGetVersionEx(unsigned int itemCode);

    canStatus canParamGetCount (void);
    canStatus canParamCommitChanges (void);
    canStatus canParamDeleteEntry (int index);
    canStatus canParamCreateNewEntry (void);
    canStatus canParamSwapEntries (int index1, int index2);
    canStatus canParamGetName (int index, char *buffer, int maxlen);
    canStatus canParamGetChannelNumber (int index);
    canStatus canParamGetBusParams (int index,
                                  long* bitrate,
                                  unsigned int *tseg1,
                                  unsigned int *tseg2,
                                  unsigned int *sjw,
                                  unsigned int *noSamp);
    canStatus canObjBufFreeAll(void);
    canStatus canObjBufAllocate(int type);
    canStatus canObjBufFree(int idx);
    canStatus canObjBufWrite(int idx, int id, void* msg, unsigned int dlc,
                                unsigned int flags);
    canStatus canObjBufSetFilter(int idx, unsigned int code, unsigned int mask);
    canStatus canObjBufSetFlags(int idx, unsigned int flags);
    canStatus canObjBufEnable(int idx);
    canStatus canObjBufDisable(int idx);
    BOOL canProbeVersion(int major, int minor, int oem_id, unsigned int flags);
    canStatus canResetBus(void);
    canStatus canWriteWait(long id, void * msg,
                            unsigned int dlc, unsigned int flag,
                            unsigned long timeout);

    canStatus canBusOn(void);
    canStatus canBusOff(void);
    canStatus canUnloadLibrary(void);
    canStatus canSetAcceptanceFilter(unsigned int code,
                                  unsigned int mask, int is_extended);

    
    canStatus canRead(long* id, void* msg, unsigned int* dlc,
                      unsigned int* flag, DWORD* time);
    canStatus canRead(CanMessage *cCanMsg);
    canStatus canReadWait(CanMessage *cCanMsg, unsigned long timeout);
    canStatus canReadSpecific(CanMessage *cCanMsg);
    canStatus canReadSpecificSkip(CanMessage *cCanMsg);

    canStatus canWrite(long id, void* msg, unsigned int dlc, unsigned int flag);

    canStatus canWrite(CanMessage cCanMsg);
    canStatus canWriteWait(CanMessage cCanMsg, unsigned long timeout);
};

#endif
