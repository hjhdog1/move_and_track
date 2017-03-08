package com.kvaser.canrestdemo;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONTokener;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;

public class ReadLogActivity extends MenuActivity {
	protected static final String TAG = "ReadLogActivity";
	
	RestService restServiceCanRead;
	static TextView t_log = null;
			
	private int handlerReadDelay = 1000;

	private CheckBox chk_continuous_read = null;
	private CheckBox chk_short_log = null;

	static TextView t_heading; 

	static long msgsPerSeconds;
	static long last_received_stamp;
	
	static Boolean short_log = false;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
		
        setContentView(R.layout.activity_readlog);
        
        t_heading = (TextView) findViewById(R.id.t_log_title);
        last_received_stamp = 0;
 
        t_log = (TextView) findViewById(R.id.t_log);
        t_log.setMovementMethod(new ScrollingMovementMethod());
        
        chk_continuous_read = (CheckBox) findViewById(R.id.chk_continuous_read);
        chk_continuous_read.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if (chk_continuous_read.isChecked()) {
		        	mHandlerGet.postDelayed(runnable, handlerReadDelay);
		        } else {
		        	mHandlerGet.removeCallbacks(runnable);
		        }
			}

		});
        
        chk_short_log  = (CheckBox) findViewById(R.id.chk_short_log);
        chk_short_log.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if (chk_short_log.isChecked()) {
		        	short_log = true;
		        } else {
		        	short_log = false;
		        }
			}

		});
        
        
		Button b_canRead = (Button) findViewById(R.id.b_can_read);
    	b_canRead.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			try {
					restServiceCanRead.execute();
				} catch (Exception e) {
					e.printStackTrace();
				}
    		}
    	});
 
		Button b_logClear = (Button) findViewById(R.id.b_log_clear);
    	b_logClear.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			t_log.setText(R.string.log_header);
    		}
    	});
	}

    
	@Override
	public void onResume() {
		super.onResume();
        Log.d(TAG, "onResume()");
        initiateButtons();
                
        if (chk_continuous_read.isChecked()) {
        	mHandlerGet.postDelayed(runnable, handlerReadDelay);
        }
	}

	@Override
	public void onPause() {
		super.onPause();
        Log.d(TAG, "onPause()");
        mHandlerGet.removeCallbacks(runnable);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
        Log.d(TAG, "onDestroy()");
	}

	private Runnable runnable = new Runnable() {
		@Override
		public void run() {
			restServiceCanRead.execute();
			// Watch out, this could potentially create a huge queue
			// if the handlerReadDelay is too small.
			mHandlerGet.postDelayed(this, handlerReadDelay);
		}
	};

	static void updateMsgsPerSeconds(long num_msg, long time_ms) {
		if (num_msg != 0) {
			msgsPerSeconds = (num_msg  * 1000)/ time_ms;
			
			t_heading.setText("Received " + num_msg + " at " + msgsPerSeconds + " msg/s.");
		}
	}
	
	private void initiateButtons() {
		
		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
		String url = preferences.getString(PrefsActivity.KEY_URL, "http://192.168.1.10:8080");
		String sessionId = preferences.getString(PrefsActivity.KEY_SESSION_ID, "");
		int canHandle = Integer.valueOf(preferences.getString(PrefsActivity.KEY_CAN_HANDLE, "0"));
        String readBurstSize = preferences.getString(PrefsActivity.KEY_READ_BURST_SIZE, "50");
        String url_session = url + "/" + sessionId;
        String readDelay = preferences.getString(PrefsActivity.KEY_READ_DELAY, "500");
        if (readDelay.matches("")) {
        	readDelay = "1000";
        }
        
        restServiceCanRead = new RestService(mHandlerGet, this, url_session + "/canRead", RestService.GET); //Create new rest service for get
        restServiceCanRead.addParam("ident", Integer.toString(RestActivity.IDENT_READ));
        restServiceCanRead.addParam("hnd", Integer.toString(canHandle));
        if (!readBurstSize.matches("")) {
        	restServiceCanRead.addParam("max", readBurstSize);
        }
        
        
        handlerReadDelay = Integer.valueOf(readDelay);      
	}
    
	private static final Handler mHandlerGet = new Handler(){



		@Override
    	public void handleMessage(Message msg){
    			Bundle bundle = (Bundle) msg.obj;
    			int responseCode = bundle.getInt("responseCode");    	        
    			if (responseCode == 200) {
    				parseResult(bundle.getString("result"));
    			}
    		}
    	
    	private void parseResult(String json_text) {
	        try {
	        	JSONTokener jsonTokener = new JSONTokener(json_text);
	        	JSONObject json = (JSONObject) jsonTokener.nextValue();
	        	int ident = json.getInt("ident");
	        	long now_stamp;
				switch(ident) {
	        	case RestActivity.IDENT_READ:
	        		JSONArray  jsonArrayMsgs = json.getJSONArray("msgs");
	        		int msg_length = jsonArrayMsgs.length();
	        		if (short_log && msg_length != 0) {
	        			t_log.setText("");
	        		}
		        	for (int i = 0; i < msg_length; i++) {
		        		JSONObject row = jsonArrayMsgs.getJSONObject(i);
		        		String txt = String.format("%-12s %04d  %04x  %d %25s",
		        				row.getString("time"),
		        				row.getInt("id"),
		        				row.getInt("flag"),
		        				row.getInt("dlc"),
		        				row.getJSONArray("msg"));
		        		t_log.append("\n" + txt);
		        	}
	        		now_stamp = System.currentTimeMillis();
	        		if (last_received_stamp != 0) {
	        			updateMsgsPerSeconds(msg_length, now_stamp - last_received_stamp);
	        		}
	        		last_received_stamp = now_stamp;
	    			break;

	        	default:
	        		Log.w(TAG, "Unknown ident:" + ident);
	        	}
	        }
	        catch (Exception e) {
	            Log.e(TAG, "Get: Failed to parse JSON.", e);
	        }
    	}

		
    };
    
}
