package com.kvaser.canrestdemo;

import org.json.JSONObject;
import org.json.JSONTokener;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class RestActivity extends MenuActivity{
	protected static final String TAG = RestActivity.class.getSimpleName();
	public static final int CAN_BUS_ON = 1;
	
	public static final int IDENT_DEVICE_STATUS         = 1;
	public static final int IDENT_INIT                   = 2;
	public static final int IDENT_OPEN_CHANNEL           = 3;
	public static final int IDENT_SET_BUS_OUTPUT_CONTROL = 4;
	public static final int IDENT_SET_BUS_PARAMS         = 5;
	public static final int IDENT_BUS_ON                 = 6;
	public static final int IDENT_BUS_OFF                = 7;
	public static final int IDENT_READ                   = 8;
	public static final int IDENT_FLUSH_RX               = 9;
	public static final int IDENT_CLOSE_CHANNEL          = 10;
	public static final int IDENT_UNLOAD                 = 11;
	public static final int IDENT_WRITE                  = 12;
	
	private static final int CLEAR_ID = 1;
	
	private static RestActivity restActivityContext;

	static TextView t_logTop = null;
	static TextView t_logBottom = null;
	RestService restServiceDeviceStatus,
				restServiceCanInit,
				restServiceCanOpenChannel,
				restServiceCanRead, 
				restServiceCanBusOn, 
				restServiceCanBusOff, 
				restServiceCanSetBusOutputControl, 
				restServiceCanSetBusParams,
				restServiceCanClose,
				restServiceCanUnloadLibrary,
				restServiceCanFlushRx;
	
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rest);
        
        t_logBottom = (TextView) findViewById(R.id.t_logBottom);
        t_logBottom.setMovementMethod(new ScrollingMovementMethod());
        t_logBottom.setText("Empty...");
        
        registerForContextMenu(t_logBottom);
        
        restActivityContext = this;
    }

	
	@Override
	public void onResume() {
		super.onResume();
        Log.d(TAG, "onResume()");
        initiateButtons();
	}
	
	
    @Override
    public void onCreateContextMenu(ContextMenu menu, View v,
            ContextMenuInfo menuInfo) {
        super.onCreateContextMenu(menu, v, menuInfo);
        menu.add(0, CLEAR_ID, 0, "Clear");
    }
	
    
    @Override
    public boolean onContextItemSelected(MenuItem item) {
    	switch(item.getItemId()) {
            case CLEAR_ID:
            	t_logBottom.setText("Empty...");
                return true;
        }
        return super.onContextItemSelected(item);
    }

    
	private void initiateButtons() {
		
		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
		String url = preferences.getString(PrefsActivity.KEY_URL, "http://192.168.1.10:8080");
		String sessionId = preferences.getString(PrefsActivity.KEY_SESSION_ID, "");
		int canHandle = Integer.valueOf(preferences.getString(PrefsActivity.KEY_CAN_HANDLE, "0"));
        String accessPassword = preferences.getString(PrefsActivity.KEY_ACCESS_PASSWORD, "");
        String sessionTimeout = preferences.getString(PrefsActivity.KEY_SESSION_TIMEOUT, "");
        String readBurstSize = preferences.getString(PrefsActivity.KEY_READ_BURST_SIZE, "50");
        String bitRateConstant = preferences.getString(PrefsActivity.KEY_BITRATE, "-4");
        String driverType = preferences.getString(PrefsActivity.KEY_DRIVER_TYPE, "4");
        
        restServiceDeviceStatus = new RestService(mHandlerGet, this, url + "/deviceStatus", RestService.GET);
        restServiceDeviceStatus.addParam("ident", Integer.toString(IDENT_DEVICE_STATUS));
        
        restServiceCanInit = new RestService(mHandlerGet, this, url + "/canInitializeLibrary", RestService.GET);
        restServiceCanInit.addParam("ident", Integer.toString(IDENT_INIT));
        if (!accessPassword.matches("")) {
        	restServiceCanInit.addParam("password", accessPassword);
        }
        if (!sessionTimeout.matches("")) {
        	restServiceCanInit.addParam("timeout", sessionTimeout);
        }

        String url_session = url + "/" + sessionId;
        
		restServiceCanOpenChannel = new RestService(mHandlerGet, this, url_session + "/canOpenChannel", RestService.GET);
        restServiceCanOpenChannel.addParam("ident", Integer.toString(IDENT_OPEN_CHANNEL));
        restServiceCanOpenChannel.addParam("channel", "0");
        restServiceCanOpenChannel.addParam("flags", "0");
        
        restServiceCanBusOn = new RestService(mHandlerGet, this, url_session + "/canBusOn", RestService.GET);
        restServiceCanBusOn.addParam("ident", Integer.toString(IDENT_BUS_ON));
        restServiceCanBusOn.addParam("hnd", Integer.toString(canHandle));
                
        restServiceCanBusOff = new RestService(mHandlerGet, this, url_session + "/canBusOff", RestService.GET);
        restServiceCanBusOff.addParam("ident", Integer.toString(IDENT_BUS_OFF));
        restServiceCanBusOff.addParam("hnd", Integer.toString(canHandle));
        
        restServiceCanSetBusOutputControl = new RestService(mHandlerGet, this, url_session + "/canSetBusOutputControl", RestService.GET);
        restServiceCanSetBusOutputControl.addParam("ident", Integer.toString(IDENT_SET_BUS_OUTPUT_CONTROL));
        restServiceCanSetBusOutputControl.addParam("hnd", Integer.toString(canHandle));
        restServiceCanSetBusOutputControl.addParam("drivertype", driverType);

        restServiceCanSetBusParams = new RestService(mHandlerGet, this, url_session + "/canSetBusParams", RestService.GET);
        restServiceCanSetBusParams.addParam("ident", Integer.toString(IDENT_SET_BUS_PARAMS));
        restServiceCanSetBusParams.addParam("hnd", Integer.toString(canHandle));
        restServiceCanSetBusParams.addParam("freq", bitRateConstant);

        restServiceCanRead = new RestService(mHandlerGet, this, url_session + "/canRead", RestService.GET);
        restServiceCanRead.addParam("ident", Integer.toString(IDENT_READ));
        restServiceCanRead.addParam("hnd", Integer.toString(canHandle));
        if (!readBurstSize.matches("")) {
        	restServiceCanRead.addParam("max", readBurstSize);
        }
        
        restServiceCanClose = new RestService(mHandlerGet, this, url_session + "/canClose", RestService.GET);
        restServiceCanClose.addParam("ident", Integer.toString(IDENT_CLOSE_CHANNEL));
        restServiceCanClose.addParam("hnd", Integer.toString(canHandle));
        
        restServiceCanUnloadLibrary = new RestService(mHandlerGet, this, url_session + "/canUnloadLibrary", RestService.GET);
        restServiceCanUnloadLibrary.addParam("ident", Integer.toString(IDENT_UNLOAD));
        
        restServiceCanFlushRx = new RestService(mHandlerGet, this, url_session + "/canIoCtl", RestService.GET);
        restServiceCanFlushRx.addParam("ident", Integer.toString(IDENT_FLUSH_RX));
        restServiceCanFlushRx.addParam("hnd", Integer.toString(canHandle));
        restServiceCanFlushRx.addParam("func", Integer.toString(CanlibRest.canIOCTL_FLUSH_RX_BUFFER));
        
        t_logTop = (TextView) findViewById(R.id.t_logTop);
        t_logTop.setMovementMethod(new ScrollingMovementMethod());
       
        Button b_isDeviceStatus = (Button) findViewById(R.id.b_is_device_status);
    	b_isDeviceStatus.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling deviceStatus\n" + restServiceDeviceStatus.getString() + "\n------\n");
					restServiceDeviceStatus.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});

        
        Button b_canInit = (Button) findViewById(R.id.b_can_init);
    	b_canInit.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Init\n" + restServiceCanInit.getString() + "\n------\n");
					restServiceCanInit.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});

    	Button b_canOpenChannel = (Button) findViewById(R.id.b_can_open_channel);
    	b_canOpenChannel.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Open Channel\n" + restServiceCanOpenChannel.getString() + "\n------\n");
					restServiceCanOpenChannel.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});
    	
    	Button b_canRead = (Button) findViewById(R.id.b_can_read);
    	b_canRead.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling CAN Read\n" + restServiceCanRead.getString() + "\n------\n");
					restServiceCanRead.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});
    	
        Button b_canBusOn = (Button) findViewById(R.id.b_can_bus_on);
    	b_canBusOn.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Bus on\n" + restServiceCanBusOn.getString() + "\n------\n");
					restServiceCanBusOn.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});
    	
        Button b_canBusOff = (Button) findViewById(R.id.b_can_bus_off);
    	b_canBusOff.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Bus off\n" + restServiceCanBusOff.getString() + "\n------\n");
					restServiceCanBusOff.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});
    	
    	
    	Button b_canBusOutputControl = (Button) findViewById(R.id.b_can_bus_output_control);
    	b_canBusOutputControl.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Set Bus Output Control\n" + restServiceCanSetBusOutputControl.getString() + "\n------\n");
					restServiceCanSetBusOutputControl.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});
    	
    	Button b_canSetBusParams = (Button) findViewById(R.id.b_can_set_bus_params);
    	b_canSetBusParams.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Set Bus Params\n" + restServiceCanSetBusParams.getString() + "\n------\n");
					restServiceCanSetBusParams.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});

   	Button b_canFlushRx = (Button) findViewById(R.id.b_can_flush_rx);
    	b_canFlushRx.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Can Flush RX\n" + restServiceCanFlushRx.getString() + "\n------\n");
					restServiceCanFlushRx.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});
    	
    	Button b_canClose = (Button) findViewById(R.id.b_can_close);
    	b_canClose.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Can Close\n" + restServiceCanClose.getString() + "\n------\n");
					restServiceCanClose.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});

    	Button b_canUnloadLibrary = (Button) findViewById(R.id.b_can_unload_library);
    	b_canUnloadLibrary.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
    				t_logTop.setText("Calling Can Unload Library\n" + restServiceCanUnloadLibrary.getString() + "\n------\n");
					restServiceCanUnloadLibrary.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});
    	    	
 	}

	//Overridden handler to process incoming response. Response string is attached as msg.obj.
    private static final Handler mHandlerGet = new Handler(){
    	@Override
    	public void handleMessage(Message msg){
    			Bundle bundle = (Bundle) msg.obj;
    			int responseCode = bundle.getInt("responseCode");
    			if (responseCode == 200) {
    				parseResult(bundle.getString("result"));
    			} else {
    				Log.d(TAG, "mHandlerGet() responseCode:" + bundle.getInt("responseCode"));
        			Log.d(TAG, "mHandlerGet() result:" + bundle.getString("result"));
    			}
    			t_logTop.append(bundle.getString("result") + "\n");
    		}
    	private void parseResult(String json_text) {
	        try {
	        	JSONTokener jsonTokener = new JSONTokener(json_text);
	        	JSONObject json = (JSONObject) jsonTokener.nextValue();
	        	int canStatus;
	        	int ident = json.getInt("ident");
	        	switch(ident) {
	        	case IDENT_DEVICE_STATUS:
	        		int usage = json.getInt("usage");
	        		if (usage == CanlibRest.kvrDeviceUsage_FREE) {
	        			t_logBottom.append("\nkvrDeviceUsage_FREE");
	        		} else if (usage == CanlibRest.kvrDeviceUsage_REMOTE) {
	        			t_logBottom.append("\nkvrDeviceUsage_REMOTE");
	        		}
	        		break;
	        	case IDENT_INIT:
	        		canStatus = json.getInt("stat");
	        		t_logBottom.append("\n" + CanlibRest.getErrorText(canStatus));
	        		if (canStatus == CanlibRest.canOK) {
	        			restActivityContext.setSession(json.getString("session"));
	        		}
	    			break;
	        	case IDENT_OPEN_CHANNEL:
	        		int hnd = json.getInt("hnd");
	        		canStatus = json.getInt("stat");
	        		t_logBottom.append("\n" + CanlibRest.getErrorText(canStatus));
	        		if (canStatus == CanlibRest.canOK) {
	        			restActivityContext.setHandle(hnd);
	        		}
	        		break;
	        		        	
	        	default:
	        		canStatus = json.getInt("stat");
	        		t_logBottom.append("\n" + CanlibRest.getErrorText(canStatus));
	        	}
	        }
	        catch (Exception e) {
	            Log.e(TAG, "Get: Failed to parse JSON:" + json_text + "\n", e);
	        }
    	}
    };


	public void setSession(String sessionId) {
		Log.d(TAG, "parseSession:" + sessionId);
		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(restActivityContext);
        
        preferences.edit().putString(PrefsActivity.KEY_SESSION_ID, sessionId).commit();
        initiateButtons();
	}

	public void setHandle(int hnd) {
		Log.d(TAG, "parseHnd:" + String.valueOf(hnd));
		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(restActivityContext);

        preferences.edit().putString(PrefsActivity.KEY_CAN_HANDLE, String.valueOf(hnd)).commit();
        initiateButtons();
	}
}

