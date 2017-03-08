package com.kvaser.canrestdemo;

import org.json.JSONArray;
import org.json.JSONException;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Handler;
import android.widget.SeekBar;
import android.widget.TextView;

public class PropSlider {
	private final TextView textView;
	private SeekBar seekBar;
	
	private int canId   = 258;
	private int canByte = 1;
	private int value   = 1;
	
	private SharedPreferences preferences;
	RestService restServiceCanWrite;
	private Handler mHandlerGet;
	private Context propActivity;
	private String keyMeterId;
	private String keyMeterByte;
	
	public PropSlider(String name, PropActivity propActivity, SharedPreferences preferences, Handler mHandlerGet, TextView newTextView, SeekBar newSeekBar) {
		this.textView = newTextView;
		this.seekBar  = newSeekBar;
		this.preferences = preferences;
		this.mHandlerGet = mHandlerGet;
		this.propActivity = propActivity;
		
		keyMeterId = name + "Id";
		keyMeterByte = name + "Byte";
		newSeekBar.setMax(254);
		
	    updateRestCanWrite();
	    
		newSeekBar.setOnSeekBarChangeListener(new myOnSeekBarChangeListener() { 	
    		@Override
    		public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
    			int newValue = (progress - 127);
    			setNewValue(newValue);
    			sendCanMessage(canId, canByte, value);
    		}
        });
		reset();
	}

	private void sendCanMessage(int canId, int canByte, int value) {
		if (restServiceCanWrite == null) {
			return;
		}
		// The protocol specifies that we should send CAN messages with DLC equal to
		// the data byte we are interested in. E.g. if we want to send 25, and are 
		// sending data in byte 3, we should use DLC = 4 and send the data "0, 0, 0, 25"
		String data = "0,0,0,0,0,0,0,0".substring(0, 2*canByte) + Integer.toString(value);
		restServiceCanWrite.setParam("msg", data);
		restServiceCanWrite.setParam("id", Integer.toString(canId));
		restServiceCanWrite.execute();
	}
	
	public void reset() {
		int newValue = 0;
		seekBar.setProgress((byte)newValue + 127);
		setNewValue(newValue);
	}

	private void setNewValue(int newValue) {
		String valueString = String.valueOf(((byte)newValue));
		textView.setText(valueString);
		value = newValue;
		PropActivity.updateMrProp();
	}

	public void setCanId(int newCanId) {
		canId = newCanId;
	}

	public void setCanByte(int newCanByte) {
		canByte = newCanByte;
	}

	public void setNewValue(int canId, JSONArray msgArray) throws JSONException {
		if (this.canId == canId) {
    		int value = msgArray.getInt(this.canByte);
			setNewValue(value);
			seekBar.setProgress((byte)value + 127);
			}
	}

	public int getValue() {
		return value;
	}

    private abstract class myOnSeekBarChangeListener implements SeekBar.OnSeekBarChangeListener {

		@Override
		public abstract void onProgressChanged(SeekBar seekBar, int progress,
				boolean fromUser);

		@Override
		public void onStartTrackingTouch(SeekBar seekBar) {
		}

		@Override
		public void onStopTrackingTouch(SeekBar seekBar) {
		}
    	
    }

	public void updateRestCanWrite() {
		String url = preferences.getString(PrefsActivity.KEY_URL, "http://192.168.1.10:8080");
		String sessionId = preferences.getString(PrefsActivity.KEY_SESSION_ID, "");
		int canHandle = Integer.valueOf(preferences.getString(PrefsActivity.KEY_CAN_HANDLE, "0"));
        String url_session = url + "/" + sessionId;
		
        String canIdString = preferences.getString(keyMeterId, "258");
        canId = Integer.valueOf(canIdString);
        String alphaByteString = preferences.getString(keyMeterByte, "1");
        canByte = Integer.valueOf(alphaByteString);
        
        restServiceCanWrite = new RestService(mHandlerGet, propActivity, url_session + "/canWrite", RestService.GET);
        restServiceCanWrite.addParam("ident", Integer.toString(RestActivity.IDENT_WRITE));
        restServiceCanWrite.addParam("hnd", Integer.toString(canHandle));
        restServiceCanWrite.addParam("id", Integer.toString(canId));
        restServiceCanWrite.addParam("dlc", Integer.toString(canByte + 1));
        restServiceCanWrite.addParam("flag", Integer.toString(CanlibRest.canMSG_STD));
		
	}
}
