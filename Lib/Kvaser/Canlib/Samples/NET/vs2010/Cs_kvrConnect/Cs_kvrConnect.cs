/*
 * This examples shows how to find a device on your network and stop/start the helper service.
 * You need to run as administrator in order to start/stop service.
 * 
 * Dependences: 
 *      canlib32.dll 
 *      Kvrlib.dll
 *      irisdll.dll 
 *      irisflash.dll 
 *      libxml2.dll 
 *      iconv.dll 
 *      zlib1.dll 
 */

using System;
using System.Collections.Generic;
using System.Text;

using Kvaser.Kvrlib;
using canlibCLSNET;

namespace Cs_kvrConnect {
    class kvrConnect
    {
    
    static String password = "";

    static Int32 isUsedByMe(Kvrlib.DeviceInfo di)
    {
        Int32 i;
        Int32 channel_count;
        UInt64 ean;
        Canlib.canStatus status;
        UInt32 ean_hi = 0;
        UInt32 ean_lo = 0;
        UInt32 serial = 0;

        Canlib.canInitializeLibrary();

        status = Canlib.canGetNumberOfChannels(out channel_count);


        if (!status.Equals(Canlib.canStatus.canOK))
        {
            Console.WriteLine("ERROR: canGetNumberOfChannels failed " + status);
            return -1;
        }
        for (i = 0; (status.Equals(Canlib.canStatus.canOK)) && (i < channel_count); i++)
        {
            object ean0;
            status = Canlib.canGetChannelData(i, Canlib.canCHANNELDATA_CARD_UPC_NO, out ean0);
            if (!status.Equals(Canlib.canStatus.canOK))
            {
                Console.WriteLine("ERROR: canCHANNELDATA_CARD_UPC_NO failed: " + status);
                return -1;
            }
            ean = UInt64.Parse(ean0.ToString());

            ean_hi = (UInt32) ((ean >> 32) & 0xffffffff);
            ean_lo = (UInt32) ((ean) & 0xffffffff);
        
            object serial0;
            status = Canlib.canGetChannelData(i, Canlib.canCHANNELDATA_CARD_SERIAL_NO, out serial0);
        
            if (!status.Equals(Canlib.canStatus.canOK))
            {
                Console.WriteLine("ERROR: canCHANNELDATA_CARD_SERIAL_NO failed: " + status);
                return -1;
            }

            if ((di.ean_hi.Equals(ean_hi)) && (di.ean_lo.Equals(ean_lo)) && (di.ser_no.Equals(serial)))
            {
                return 1;
            }
        }
        return 0;
    }


    static void dumpDeviceInfo (Kvrlib.DeviceInfo di)
    {
        String service_text = "";
        String buf = "";
        Kvrlib.Status status;
        String addr_buf = "";
        Int32 i;
        Kvrlib.ServiceState service_state = 0;
        Kvrlib.StartInfo service_sub_state = 0;

        Console.Write("--------------------------------------------------------------------------\n");
        Console.Write("Device information\n");
        Console.Write("EAN:         {0:X}{1:X}\n", di.ean_hi, di.ean_lo);
        Console.WriteLine("FW version:  " + di.fw_major_ver +"." + di.fw_minor_ver +"." + di.fw_build_ver);
        Console.WriteLine("Serial:      " + di.ser_no);
        Console.WriteLine("Name:        " + di.name);
        Console.WriteLine("Host name:   " + di.host_name);
        Console.Write("Password:    ");
        if (di.accessibility_pwd.Length == 0)
        {
            Console.Write("None\n");
        }
        else
        {
            for (i = 0; (i < di.accessibility_pwd.Length); i++)
            {
                Console.Write("*");
                
            }
            Console.Write("\n");
        }
        Kvrlib.StringFromAddress(out addr_buf, di.device_address);
        Console.WriteLine("IP:          " + addr_buf);
        Kvrlib.StringFromAddress(out addr_buf, di.client_address);
        Console.WriteLine("Client IP:   " + addr_buf);
        Console.WriteLine("Usage:       " + di.usage);
        if ((!di.usage.Equals(Kvrlib.DeviceUsage.FREE)) && (isUsedByMe(di) > 0))
        {
            Console.Write(" - Used by Me!\n");
        }
        else if ((!di.usage.Equals(Kvrlib.DeviceUsage.FREE)) && (!di.usage.Equals(Kvrlib.DeviceUsage.UNKNOWN)))
        {
            Console.Write(" - Used by other!\n");
        }
        else
        {
            Console.Write("\n");
        }
        Console.WriteLine("Alive:       " + ((di.availability & Kvrlib.Availability.FOUND_BY_SCAN) > 0 ? "Yes" : "No"));
        Console.WriteLine("Stored:      " + ((di.availability & Kvrlib.Availability.STORED) > 0 ? "Yes" : "No"));

        // Ask service for status service_text
        status = Kvrlib.DeviceGetServiceStatusText(di, out service_text);
        
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("Service:     FAILED - " + buf);
        }
        else if (service_text.StartsWith("Service: "))
        {
            Console.WriteLine("Service:     " + service_text.Substring(9 , service_text.Length -9));
        }
        else
        {
            Console.Write("%s\n", service_text);
        }

        status = Kvrlib.DeviceGetServiceStatus(di, out service_state, out service_sub_state);
        if (status.Equals(Kvrlib.Status.OK))
        {
            Console.WriteLine("service_state: " + service_state +"." + service_sub_state);
        }
        else
        {
            Console.WriteLine("service_state: unknown");
        }
    }

    static Kvrlib.Status discoverDevices(Kvrlib.DiscoveryHnd discoveryHandle)
    {
        Kvrlib.Status status;
        Kvrlib.DeviceInfo[] device_info = new Kvrlib.DeviceInfo[64];
        Int32 devices;
        Int32 delay_ms = 500;
        Int32 timeout_ms = 300;
        String buf = "";


        status = Kvrlib.DiscoveryStart(discoveryHandle, delay_ms, timeout_ms);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("kvrDiscoveryStart() FAILED - " + buf);
            return status;
        }

        devices = 0;
        while (status.Equals(Kvrlib.Status.OK))
        {
            status = Kvrlib.DiscoveryGetResults(discoveryHandle, out device_info[devices]);
            if (status.Equals(Kvrlib.Status.OK))
            {

                dumpDeviceInfo(device_info[devices]);
                if (!Kvrlib.DiscoverySetPassword(device_info[devices], password).Equals(Kvrlib.Status.OK))
                {
                    Console.WriteLine("Unable to set password: {0:s} ({1:d})", password, password.Length);
                }
               
                if (!Kvrlib.DiscoverySetEncryptionKey(device_info[devices], "testkey").Equals(Kvrlib.Status.OK))
                {
                    Console.WriteLine("Unable to set Ecryption key");
                }

                if (!Kvrlib.DiscoverySetEncryptionKey(device_info[devices], "").Equals(Kvrlib.Status.OK))
                {
                    Console.WriteLine("Unable to set Ecryption key");
                }

                devices++;
            }
            else
            {
                if (status.Equals(Kvrlib.Status.BLANK))
                {
                    Console.WriteLine("kvrDiscoveryGetResults() failed " + status);
                }
            }
        }

        status = Kvrlib.DiscoveryStoreDevices(device_info);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("Device store failed: " + buf);
            return status;
        }
        return status;
    }

    static Kvrlib.Status setupBroadcast(Kvrlib.DiscoveryHnd discoveryHandle)
    {
        String buf = "";
        Kvrlib.Status status;
        Kvrlib.Address[] addr_list = new Kvrlib.Address[20];
        Int32 no_addrs = 0;
        Int32 i;

        status = Kvrlib.DiscoveryGetDefaultAddresses(out addr_list, Kvrlib.AddressTypeFlag.ALL);

        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("kvrDiscoveryGetDefaultAddresses() FAILED - " + buf);
            return status;
        }

        if (no_addrs < addr_list.Length)
        {
            String tmp_addr = "192.168.3.66";
            status = Kvrlib.AddressFromString(Kvrlib.AddressType.IPV4, out addr_list[no_addrs], tmp_addr);
            if (!status.Equals(Kvrlib.Status.OK))
            {
                Console.WriteLine("ERROR: kvrAddressFromString(" + no_addrs + ", " + tmp_addr + ") failed");
                return status;
            }
            else
            {
                no_addrs++;
            }

        }
        else
        {
            Console.WriteLine("NOTE: We don't have room for all devices in addr_list[" + addr_list.Length + "]");
        }

        for (i = 0; i < no_addrs; i++ )
        {
            status = Kvrlib.StringFromAddress(out buf, addr_list[i]);
            Console.WriteLine("Looking for device using: " + buf);
        }
        
        //Create address from string.
        Kvrlib.Address addressFromString;
        String addr_string = "127.0.0.1";
        status = Kvrlib.AddressFromString(Kvrlib.AddressType.IPV4, out addressFromString, addr_string);

        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("AddressFromString() FAILED - " + buf);
            return status;
        }
        Kvrlib.StringFromAddress(out addr_string, addressFromString);
        Console.WriteLine("Created address from string: " + addr_string);
        
        status = Kvrlib.DiscoverySetAddresses(discoveryHandle, addr_list);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("DiscoverySetAddresses() FAILED - " + buf);
            return status;
        }
        return status;
    }

    static Kvrlib.Status testService()
    {
        Kvrlib.Status status = new Kvrlib.Status();
        String buf = "";
        Int32 serviceStatus;

        status = Kvrlib.ServiceQuery(out serviceStatus);
        Console.WriteLine("ServiceQuery() service status - " + serviceStatus);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("ServiceQuery() FAILED - " + buf);
            return status;
        }
        System.Threading.Thread.Sleep(2000);
        status = Kvrlib.ServiceStop(out serviceStatus);
        Console.WriteLine("ServiceStop() service status - " + serviceStatus);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("ServiceStop() FAILED - " + buf);
            return status;
        }
        System.Threading.Thread.Sleep(2000);
        status = Kvrlib.ServiceStart(out serviceStatus);
        Console.WriteLine("ServiceStart() service status - " + serviceStatus);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("ServiceStart() FAILED - " + buf);
            return status;
        }
        System.Threading.Thread.Sleep(2000);
        status = Kvrlib.ServiceQuery(out serviceStatus);
        Console.WriteLine("ServiceQuery() service status - " + serviceStatus);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("ServiceQuery() FAILED - " + buf);
            return status;
        }
        return status;
    }

    
    static int Main(string[] args)
    {

        Kvrlib.Status status;
        Kvrlib.DiscoveryHnd discoveryHandle;
        String buf = "";

        if (args.Length > 0)
        {
            password = args[0];
        }
        
        Kvrlib.InitializeLibrary();

        status = Kvrlib.DiscoveryOpen(out discoveryHandle);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("DiscoveryOpen() FAILED - " + buf);
            return -1;
        }


        status = setupBroadcast(discoveryHandle);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("setupBroadcast() FAILED - " + buf);
            return -2;
        }

        status = discoverDevices(discoveryHandle);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("discoverDevices() FAILED - " + buf);
            return -3;
        }

        status = Kvrlib.DiscoveryClearDevicesAtExit(true);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("DiscoveryClearDevicesAtExit() FAILED - " + buf);
            return -4;
        }
        status = Kvrlib.DiscoveryClearDevicesAtExit(false);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("DiscoveryClearDevicesAtExit() FAILED - " + buf);
            return -5;
        }

        status = Kvrlib.DiscoveryClose(discoveryHandle);
        if (!status.Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("DiscoveryClose() FAILED - " + buf);
            return -6;
        }

        if (!testService().Equals(Kvrlib.Status.OK))
        {
            Kvrlib.GetErrorText(status, out buf);
            Console.WriteLine("testService() FAILED - " + buf);
            return -7;
        }

        return 0;
    }
  }
}
