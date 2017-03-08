package com.kvaser.canrestdemo;

import java.util.HashMap;

public class CanlibRest {
	public static final int canMSG_EXT     = 0x0004;
	public static final int canMSG_STD     = 0x0002;
	
	public static final int canIOCTL_FLUSH_RX_BUFFER     = 10;

	protected static final int kvrDeviceUsage_FREE = 1;
	protected static final int kvrDeviceUsage_REMOTE = 2;
	
	public static final int canOK = 0;

	private static final HashMap<Integer, String> errorMap;
	static {
		errorMap = new HashMap<Integer, String>();
		errorMap.put(  canOK, "canOK");
		errorMap.put( -1, "canERR_PARAM");
		errorMap.put( -2, "canERR_NOMSG");
		errorMap.put( -3, "canERR_NOTFOUND");
		errorMap.put( -4, "canERR_NOMEM");
		errorMap.put( -5, "canERR_NOCHANNELS");
		errorMap.put( -6, "canERR_RESERVED_3");
		errorMap.put( -7, "canERR_TIMEOUT");
		errorMap.put( -8, "canERR_NOTINITIALIZED");
		errorMap.put( -9, "canERR_NOHANDLES");
		errorMap.put( -10, "canERR_INVHANDLE");
		errorMap.put( -11, "canERR_INIFILE");
		errorMap.put( -12, "canERR_DRIVER");
		errorMap.put( -13, "canERR_TXBUFOFL");
		errorMap.put( -14, "canERR_RESERVED_1");
		errorMap.put( -15, "canERR_HARDWARE");
		errorMap.put( -16, "canERR_DYNALOAD");
		errorMap.put( -17, "canERR_DYNALIB");
		errorMap.put( -18, "canERR_DYNAINIT");
		errorMap.put( -19, "canERR_NOT_SUPPORTED");
		errorMap.put( -20, "canERR_RESERVED_5");
		errorMap.put( -21, "canERR_RESERVED_6");
		errorMap.put( -22, "canERR_RESERVED_2");
		errorMap.put( -23, "canERR_DRIVERLOAD");
		errorMap.put( -24, "canERR_DRIVERFAILED");
		errorMap.put( -25, "canERR_NOCONFIGMGR");
		errorMap.put( -26, "canERR_NOCARD");
		errorMap.put( -27, "canERR_RESERVED_7");
		errorMap.put( -28, "canERR_REGISTRY");
		errorMap.put( -29, "canERR_LICENSE");
		errorMap.put( -30, "canERR_INTERNAL");
		errorMap.put( -31, "canERR_NO_ACCESS");
		errorMap.put( -32, "canERR_NOT_IMPLEMENTED");
		errorMap.put( -33, "canERR_DEVICE_FILE");
		errorMap.put( -34, "canERR_HOST_FILE");
		errorMap.put( -35, "canERR_DISK");
		errorMap.put( -36, "canERR_CRC");
		errorMap.put( -37, "canERR_CONFIG");
		errorMap.put( -38, "canERR_MEMO_FAIL");
		errorMap.put( -39, "canERR_SCRIPT_FAIL");
		errorMap.put( -40, "canERR_SCRIPT_WRONG_VERSION");
		errorMap.put( -41, "canERR__RESERVED"); 
		errorMap.put( -128, "canERR_INVALID_PASSWORD");
		errorMap.put( -129, "canERR_NO_SUCH_FUNCTION");
		errorMap.put( -130, "canERR_NOT_AUTHORIZED");
		errorMap.put( -131, "canERR_INVALID_SESSION");
	}

	public static String getErrorText(int status) {
		String errorString = errorMap.get(status);
		if (errorString == null) {
			errorString = "Unkown error code " + String.valueOf(status);
		}
		return errorString;
	}

}
