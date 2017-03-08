package com.kvaser.canrestdemo;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONTokener;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

public class PropActivity extends MenuActivity {
	protected static final String TAG = "PropActivity";
	
	RestService restServiceCanRead;
//	RestService restServiceCanWrite;
	

	// We add to the runnable message queue with this delay as the interval, so
	// this could potentially create a huge queue if the handlerReadDelay is too small.
	private int handlerReadDelay = 1000;
		
	private static PropSlider propSliderAlpha;
	private static PropSlider propSliderBeta;
	private static PropSlider propSliderGamma;
	private static ImageView mrProp;
	
	TextView t_labelSeekBar1 = null;
	SeekBar seekBar1 = null;
	
	TextView t_labelSeekBar2 = null;
	SeekBar seekBar2 = null;
	
	TextView t_labelSeekBar3 = null;
	SeekBar seekBar3 = null;

	
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
                
        setContentView(R.layout.activity_prop);
        
        mrProp = (ImageView) findViewById(R.id.mrProp);
        mrProp.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				propSliderAlpha.reset();
				propSliderBeta.reset();
				propSliderGamma.reset();
			}
		});

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        
        
        
        t_labelSeekBar1 = (TextView) findViewById(R.id.labelSeekBar1);
        seekBar1 = (SeekBar) findViewById(R.id.seekBar1);
        propSliderAlpha = new PropSlider("Alpha", this, preferences, mHandlerGet, t_labelSeekBar1, seekBar1);

        t_labelSeekBar2 = (TextView) findViewById(R.id.labelSeekBar2);
        seekBar2 = (SeekBar) findViewById(R.id.seekBar2);
        propSliderBeta = new PropSlider("Beta", this, preferences, mHandlerGet, t_labelSeekBar2, seekBar2);

        t_labelSeekBar3 = (TextView) findViewById(R.id.labelSeekBar3);
        seekBar3 = (SeekBar) findViewById(R.id.seekBar3);
        propSliderGamma = new PropSlider("Gamma", this, preferences, mHandlerGet, t_labelSeekBar3, seekBar3);

        initiateRestCall();
    }
    

	@Override
	public void onResume() {
		super.onResume();
        Log.d(TAG, "onResume()");
        initiateRestCall();
        mHandlerGet.postDelayed(runnable, handlerReadDelay);
	}

	@Override
	public void onPause() {
		super.onPause();
        Log.d(TAG, "onPause()");
        mHandlerGet.removeCallbacks(runnable);
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
	        	int status;
	        	switch(ident) {
	        	case RestActivity.IDENT_READ:
	        		status = json.getInt("stat");
	        		if (status == 0) {
		        	JSONArray  jsonArrayMsgs = json.getJSONArray("msgs");
		        	for (int i = 0; i < jsonArrayMsgs.length(); i++) {
		        		JSONObject row = jsonArrayMsgs.getJSONObject(i);
		        		int canId = row.getInt("id");
						propSliderAlpha.setNewValue(canId, row.getJSONArray("msg"));
		        		propSliderBeta.setNewValue(canId, row.getJSONArray("msg"));
		        		propSliderGamma.setNewValue(canId, row.getJSONArray("msg"));
		        		}
	        		}
	    			break;
	        	case RestActivity.IDENT_WRITE:
	        		status = json.getInt("stat");
	        		if (status != 0) {
	        			Log.w(TAG, "Write respons: " + json_text);
	        		}
	        		break;
	        	default:
	        		Log.d(TAG, "Unknown ident:" + ident);
	        	}
	        }
	        catch (Exception e) {
	            Log.e(TAG, "Get: Failed to parse JSON:\n" + json_text, e);
	        }
    	}


	};

	static void updateMrProp() {
		if (propSliderAlpha != null &&
			propSliderBeta != null &&
			propSliderGamma != null) {
			if (propSliderAlpha.getValue() == 0 &&
					propSliderBeta.getValue() == 0	&&
					propSliderGamma.getValue() == 0) {
						mrProp.setImageResource(R.drawable.propeller_bw);
					} else {
						mrProp.setImageResource(R.drawable.propeller);
					}			
		} else {
			mrProp.setImageResource(R.drawable.propeller_bw);
		}
	}

	private void initiateRestCall() {
		
		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
		String url = preferences.getString(PrefsActivity.KEY_URL, "http://192.168.1.10:8080");
		String sessionId = preferences.getString(PrefsActivity.KEY_SESSION_ID, "");
		int canHandle = Integer.valueOf(preferences.getString(PrefsActivity.KEY_CAN_HANDLE, "0"));
        String readBurstSize = preferences.getString(PrefsActivity.KEY_READ_BURST_SIZE, "50");
        String url_session = url + "/" + sessionId;
        String readDelay = preferences.getString(PrefsActivity.KEY_READ_DELAY, "500");
                
        restServiceCanRead = new RestService(mHandlerGet, this, url_session + "/canRead", RestService.GET);
        restServiceCanRead.addParam("ident", Integer.toString(RestActivity.IDENT_READ));
        restServiceCanRead.addParam("hnd", Integer.toString(canHandle));
        if (!readBurstSize.matches("")) {
        	restServiceCanRead.addParam("max", readBurstSize);
        }
        
        handlerReadDelay = Integer.valueOf(readDelay);
                
        // TODO This could be improved by creating an array of sliders
        propSliderAlpha.updateRestCanWrite();
        propSliderBeta.updateRestCanWrite();
        propSliderGamma.updateRestCanWrite();
        
	}
	
}
