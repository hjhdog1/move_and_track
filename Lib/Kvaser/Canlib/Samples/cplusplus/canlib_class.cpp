#include <canlib.h>
#include "canlib_class.h"

Canlib::Canlib()
{
  hnd = -1;
  ::canInitializeLibrary();
}


void Canlib::canInitializeLibrary(void)
{
    ::canInitializeLibrary();
}

canStatus Canlib::canOpenChannel(int channel, int flags)
{
    hnd = ::canOpenChannel(channel, flags);
    return ((canStatus)hnd);
}

canStatus Canlib::canSetBusParams(long freq, unsigned int tseg1,
                                  unsigned int tseg2, unsigned int sjw,
                                  unsigned int noSamp, unsigned int syncmode)
{
    return ::canSetBusParams(hnd, freq, tseg1, tseg2, sjw, noSamp, syncmode);
}

canStatus Canlib::canGetBusParams(  long * freq,
                                unsigned int *tseg1,
                                unsigned int *tseg2,
                                unsigned int *sjw,
                                unsigned int *noSamp,
                                unsigned int *syncmode)
{
    return ::canGetBusParams(hnd, freq, tseg1, tseg2, sjw, noSamp, syncmode);
}

canStatus Canlib::canSetBusOutputControl(const unsigned int drivertype)
{
    return ::canSetBusOutputControl(hnd, drivertype);
}

canStatus Canlib::canGetBusOutputControl(unsigned int * drivertype)
{
    return ::canGetBusOutputControl(hnd, drivertype);
}


canStatus Canlib::canAccept(const long envelope, const unsigned int flag)
{
    return ::canAccept(hnd, envelope, flag);
}


canStatus Canlib::canReadStatus(unsigned long * const flags)
{
    return ::canReadStatus(hnd, flags);
}


canStatus Canlib::canReadErrorCounters(unsigned int * txErr,
                                    unsigned int * rxErr,
                                    unsigned int * ovErr)
{
    return ::canReadErrorCounters(hnd, txErr, rxErr, ovErr);
}


canStatus Canlib::canWriteSync(unsigned long timeout)
{
    return ::canWriteSync(hnd, timeout);
}


canStatus Canlib::canReadWait(long * id,
                                   void * msg,
                          unsigned int * dlc,
                          unsigned int * flag,
                          unsigned long * time,
                          unsigned long timeout)
{
    return ::canReadWait(hnd, id, msg, dlc, flag, time, timeout);
}


canStatus Canlib::canReadSpecific(long id, void * msg,
                              unsigned int * dlc, unsigned int * flag,
                              unsigned long * time)
{
    return ::canReadSpecific(hnd, id, msg, dlc, flag, time);
}


canStatus Canlib::canReadSync(unsigned long timeout)
{
    return ::canReadSync(hnd, timeout);
}


canStatus Canlib::canReadSyncSpecific(long id, unsigned long timeout)
{
    return ::canReadSyncSpecific(hnd, id, timeout);
}


canStatus Canlib::canReadSpecificSkip(long id,
                                  void * msg,
                                  unsigned int * dlc,
                                  unsigned int * flag,
                                  unsigned long * time)
{
    return ::canReadSpecificSkip(hnd, id, msg, dlc, flag, time);
}


canStatus Canlib::canSetNotify(HWND aHWnd, unsigned int aNotifyFlags)
{
    return ::canSetNotify(hnd, aHWnd, aNotifyFlags);
}


canStatus Canlib::canTranslateBaud(long * const freq,
                               unsigned int * const tseg1,
                               unsigned int * const tseg2,
                               unsigned int * const sjw,
                               unsigned int * const nosamp,
                               unsigned int * const syncMode)
{
    return ::canTranslateBaud(freq, tseg1, tseg2, sjw, nosamp, syncMode);
}


canStatus Canlib::canGetErrorText(canStatus err, char * buf, unsigned int bufsiz)
{
    return ::canGetErrorText(err, buf, bufsiz);
}


unsigned short Canlib::canGetVersion(void)
{
    return ::canGetVersion();
}


canStatus Canlib::canIoCtl(unsigned int func, void * buf, unsigned int buflen)
{
    return ::canIoCtl(hnd, func, buf, buflen);
}


unsigned long Canlib::canReadTimer(void)
{
    return ::canReadTimer(hnd);
}


canStatus Canlib::canGetNumberOfChannels(int * channelCount)
{
    return ::canGetNumberOfChannels(channelCount);
}


canStatus Canlib::canReadEvent(CanEvent *event)
{
    return ::canReadEvent(hnd, event);
}


canStatus Canlib::canSetBusParamsC200(BYTE btr0, BYTE btr1)
{
    return ::canSetBusParamsC200(hnd, btr0, btr1);
}


canStatus Canlib::canSetDriverMode(int lineMode, int resNet)
{
    return ::canSetDriverMode(hnd, lineMode, resNet);
}


canStatus Canlib::canGetDriverMode(int *lineMode, int *resNet)
{
    return ::canGetDriverMode(hnd, lineMode, resNet);
}


unsigned int Canlib::canGetVersionEx(unsigned int itemCode)
{
    return ::canGetVersionEx(itemCode);
}


canStatus Canlib::canParamGetCount(void)
{
    return ::canParamGetCount();
}



canStatus Canlib::canParamCommitChanges(void)
{
    return ::canParamCommitChanges();
}



canStatus Canlib::canParamDeleteEntry (int index)
{
    return ::canParamDeleteEntry(index);
}



canStatus Canlib::canParamCreateNewEntry (void)
{
    return ::canParamCreateNewEntry();
}



canStatus Canlib::canParamSwapEntries (int index1, int index2)
{
    return ::canParamSwapEntries(index1, index2);
}


canStatus Canlib::canParamGetName (int index, char *buffer, int maxlen)
{
    return ::canParamGetName(index, buffer, maxlen);
}


canStatus Canlib::canParamGetChannelNumber (int index)
{
    return ::canParamGetChannelNumber(index);
}



canStatus Canlib::canParamGetBusParams (int index,
                                                long* bitrate,
                                                unsigned int *tseg1,
                                                unsigned int *tseg2,
                                                unsigned int *sjw,
                                                unsigned int *noSamp)
{
    return ::canParamGetBusParams(index, bitrate, tseg1, tseg2, sjw, noSamp);
}



canStatus Canlib::canObjBufFreeAll(void)
{
    return ::canObjBufFreeAll(hnd);
}



canStatus Canlib::canObjBufAllocate(int type)
{
    return ::canObjBufAllocate(hnd, type);
}



canStatus Canlib::canObjBufFree(int idx)
{
    return ::canObjBufFree(hnd, idx);
}



canStatus Canlib::canObjBufWrite(int idx, int id, void* msg,
                                        unsigned int dlc, unsigned int flags)
{
    return ::canObjBufWrite(hnd, idx, id, msg, dlc, flags);
}



canStatus Canlib::canObjBufSetFilter(int idx, unsigned int code, unsigned int mask)
{
    return ::canObjBufSetFilter(hnd, idx, code, mask);
}



canStatus Canlib::canObjBufSetFlags(int idx, unsigned int flags)
{
    return  ::canObjBufSetFlags(hnd, idx, flags);
}



canStatus Canlib::canObjBufEnable(int idx)
{
    return  ::canObjBufEnable(hnd, idx);
}



canStatus Canlib::canObjBufDisable(int idx)
{
    return  ::canObjBufDisable(hnd, idx);
}



BOOL Canlib::canProbeVersion(int major, int minor, int oem_id, unsigned int flags)
{
    return ::canProbeVersion(hnd, major, minor, oem_id, flags);
}



canStatus Canlib::canResetBus(void)
{
    return ::canResetBus(hnd);
}



canStatus Canlib::canWriteWait(long id, void * msg, unsigned int dlc,
                               unsigned int flag, unsigned long timeout)
{
    return ::canWriteWait(hnd, id, msg, dlc, flag, timeout);
}

canStatus Canlib::canUnloadLibrary(void)
{
    return ::canUnloadLibrary();
}

canStatus Canlib::canSetAcceptanceFilter(unsigned int code, unsigned int mask,
                                         int is_extended)
{
    return ::canSetAcceptanceFilter(hnd, code, mask, is_extended);
}


canStatus Canlib::canBusOn(void)
{
    return ::canBusOn(hnd);
}

canStatus Canlib::canBusOff(void)
{
    return ::canBusOff(hnd);
}

canStatus Canlib::canRead(long* id, void* msg, unsigned int* dlc,
                          unsigned int* flag, DWORD* time)
{
    return ::canRead(hnd, id, msg, dlc, flag, time);
}


canStatus Canlib::canWrite(long id, void* msg, unsigned int dlc,
                           unsigned int flag)
{
    return ::canWrite(hnd, id, msg, dlc, flag);
}




canStatus Canlib::canWrite(CanMessage cCanMsg)
{
    return ::canWrite(hnd, cCanMsg.id, cCanMsg.msg, cCanMsg.dlc, cCanMsg.flag);
}

canStatus Canlib::canWriteWait(CanMessage cCanMsg, unsigned long timeout)
{
    return ::canWriteWait(hnd, cCanMsg.id, cCanMsg.msg, cCanMsg.dlc, cCanMsg.flag, timeout);
}

canStatus Canlib::canRead(CanMessage *cCanMsg)
{
    return ::canRead(hnd, &(cCanMsg->id), cCanMsg->msg, &(cCanMsg->dlc),
                                        &(cCanMsg->flag), &(cCanMsg->timeStamp));
}

canStatus Canlib::canReadWait(CanMessage *cCanMsg, unsigned long timeout)
{
    return ::canReadWait(hnd, &(cCanMsg->id), cCanMsg->msg, &(cCanMsg->dlc),
                            &(cCanMsg->flag), &(cCanMsg->timeStamp), timeout);
}


canStatus Canlib::canReadSpecific(CanMessage *cCanMsg)
{
    return ::canReadSpecific(hnd, cCanMsg->id, cCanMsg->msg, &(cCanMsg->dlc),
                                    &(cCanMsg->flag), &(cCanMsg->timeStamp));
}

canStatus Canlib::canReadSpecificSkip(CanMessage *cCanMsg)
{
    return ::canReadSpecific(hnd, cCanMsg->id, cCanMsg->msg, &(cCanMsg->dlc),
                                    &(cCanMsg->flag), &(cCanMsg->timeStamp));
}
