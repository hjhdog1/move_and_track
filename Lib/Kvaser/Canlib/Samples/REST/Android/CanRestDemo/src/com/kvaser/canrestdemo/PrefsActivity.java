package com.kvaser.canrestdemo;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.widget.Toast;

public class PrefsActivity extends PreferenceActivity  implements OnSharedPreferenceChangeListener {
	protected static final String TAG = "PreferenceActivity";
	
	public static final String KEY_URL                = "Url";
	public static final String KEY_BITRATE            = "BitRate";
	public static final String KEY_DRIVER_TYPE        = "driverType";
	public static final String KEY_SESSION_ID         = "SessionId";
	public static final String KEY_METER_ALPHA_ENABLE = "MeterAlphaEnable";
	public static final String KEY_ACCESS_PASSWORD    = "AccessPassword";
	public static final String KEY_CAN_HANDLE         = "CanHandle";
	public static final String KEY_READ_BURST_SIZE    = "ReadBurstSize";
	public static final String KEY_SESSION_TIMEOUT    = "SessionTimeout";
	public static final String KEY_READ_DELAY         = "ReadDelay";
	
	public static final String KEY_WRITE_DLC          = "WriteDataDlc";
	public static final String KEY_WRITE_ID           = "WriteDataId";
	public static final String KEY_WRITE_FLAG         = "WriteFlag";
	public static final String KEY_WRITE_DATA         = "WriteDataList";
	
	public static final String KEY_METER_ALPHA_ID     = "AlphaId";
	public static final String KEY_METER_ALPHA_BYTE   = "AlphaByte";
	public static final String KEY_METER_BETA_ID      = "BetaId";
	public static final String KEY_METER_BETA_BYTE    = "BetaByte";
	public static final String KEY_METER_GAMMA_ID     = "GammaId";
	public static final String KEY_METER_GAMMA_BYTE   = "GammaByte";
	
	public static Context prefsContext;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
   
		addPreferencesFromResource(R.xml.prefs);
   
		SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
	    sharedPreferences.registerOnSharedPreferenceChangeListener(this);
	    
	    prefsContext = this; 	
	    
	    Preference sessionTimeoutPreference = this.findPreference(KEY_SESSION_TIMEOUT);
	    sessionTimeoutPreference.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {

	        @Override
	        public boolean onPreferenceChange(Preference preference, Object newValue) {
	        	String valueString = (String) newValue;
	        	if (valueString.matches("")) {
	        		return true;
	        	} else {
		        	int seconds = Integer.valueOf(valueString);
					if (seconds > 86399) {
						 showShortToast("Max session timeout is 86399 s.");
						return false;
					} else {
						return true;
					}
	        	}
	        }
	    });
	    
	    Preference readDelayPreference = this.findPreference(KEY_READ_DELAY);
	    readDelayPreference.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {

	        @Override
	        public boolean onPreferenceChange(Preference preference, Object newValue) {
	        	String valueString = (String) newValue;
	        	if (valueString.matches("")) {
	        		return true;
	        	} else {
		        	int readBurstSize = Integer.valueOf(valueString);
					if (readBurstSize < 50) {
						 showShortToast("Minimum delay is 50 ms!");
						return false;
					} else {
						return true;
					}
	        	}
	        }
	    });

	}
	
    public static void showShortToast(String text) {
        Toast.makeText(prefsContext, text, Toast.LENGTH_LONG).show();
    }

    @Override
    protected void onResume() {
        super.onResume();

        fillPreferencesSummary();
        getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
    }

    private void fillPreferencesSummary() {
    	fillUrl();
    	fillSessionId();
    	fillAccessPassword();
    	fillSessionTimeout();
    	fillCanHandle();
    	fillBitrate();
    	fillDriverType();
    	fillReadBurstSize();
    	fillReadDelay();
    	fillMeterId(KEY_METER_ALPHA_ID);
    	fillMeterId(KEY_METER_BETA_ID);
    	fillMeterId(KEY_METER_GAMMA_ID);
    	fillMeterByte(KEY_METER_ALPHA_BYTE);
    	fillMeterByte(KEY_METER_BETA_BYTE);
    	fillMeterByte(KEY_METER_GAMMA_BYTE);
	}

 	private void fillSessionId() {
    	Preference preference = findPreference(KEY_SESSION_ID);
		EditTextPreference editTextPreference = (EditTextPreference) preference;
		String sessionId = editTextPreference.getText();
		if (sessionId == null || sessionId.matches("")) {
			editTextPreference.setSummary("No active session");
		} else {
			editTextPreference.setSummary(sessionId);
		}
	}

    private void fillAccessPassword() {
		Preference preference = findPreference(KEY_ACCESS_PASSWORD);
		EditTextPreference editTextPreference = (EditTextPreference) preference;
		String accessPassword = editTextPreference.getText();
		if (accessPassword == null || accessPassword.matches("")) {
			editTextPreference.setSummary("No password set");
		} else {
			editTextPreference.setSummary("Access password is set");	
		}
	}
    
    private void fillSessionTimeout() {
		Preference preference = findPreference(KEY_SESSION_TIMEOUT);
		EditTextPreference editTextPreference = (EditTextPreference) preference;
		String sessionTimeout = editTextPreference.getText();
		if (sessionTimeout == null || sessionTimeout.matches("")) {
			editTextPreference.setSummary("Using default timeout");
		} else {
			int seconds = Integer.valueOf(sessionTimeout);
			int minutes = (seconds / 60) % 60;
			int hours = (seconds /(60 * 60)) % 24;
			seconds = seconds % 60;
			String txt = String.format("%02d:%02d:%02d (h:m:s)", hours, minutes, seconds);
			editTextPreference.setSummary("Timeout is " + txt);	
		}
	}
    
	private void fillUrl() {
		Preference preference = findPreference(KEY_URL);
		EditTextPreference editTextPreference = (EditTextPreference) preference;
		String url = editTextPreference.getText();
		if (url == null) {
			editTextPreference.setSummary("Using default url");			
		} else {
			editTextPreference.setSummary("Using " + url);
		}
	}

	private void fillCanHandle() {
		Preference preference = findPreference(KEY_CAN_HANDLE);
		EditTextPreference editTextPreference = (EditTextPreference) preference;
		String canHandle = editTextPreference.getText();
		if (canHandle == null || canHandle.matches("")) {
			editTextPreference.setSummary("No current CAN handle");			
		} else {
			editTextPreference.setSummary("Using CAN handle " + canHandle);
		}
	}

	
	private void fillBitrate() {
		Preference preference = findPreference(KEY_BITRATE);
		ListPreference  listPreference = (ListPreference) preference;
		String bitrateString = listPreference.getEntry().toString();
		listPreference.setSummary(bitrateString);
	}
	
	private void fillDriverType() {
		Preference preference = findPreference(KEY_DRIVER_TYPE);
		ListPreference  listPreference = (ListPreference) preference;
		String driverTypeString = listPreference.getEntry().toString();
		listPreference.setSummary(driverTypeString);
	}

	private void fillReadBurstSize() {
		Preference preference = findPreference(KEY_READ_BURST_SIZE);
		EditTextPreference editTextPreference = (EditTextPreference) preference;
		String readBurstSize = editTextPreference.getText();
		if (readBurstSize == null || readBurstSize.matches("")) {
			editTextPreference.setSummary("Using default number of messages");
		} else {
			editTextPreference.setSummary("Reading " + readBurstSize + " messages at a time");
		}
	}
	
	private void fillReadDelay() {
		Preference preference = findPreference(KEY_READ_DELAY);
		EditTextPreference editTextPreference = (EditTextPreference) preference;
		String readDelay = editTextPreference.getText();
		if (readDelay == null || readDelay.matches("")) {
			editTextPreference.setSummary("Using default delay");
		} else {
			editTextPreference.setSummary("Delay is " + readDelay + " ms");
		}
	}
	
    private void fillMeterId(String keyMeterId) {
		Preference preference = findPreference(keyMeterId);
		EditTextPreference editTextPreference = (EditTextPreference) preference;
		String canId = editTextPreference.getText();
		if (canId == null || canId.matches("")) {
			editTextPreference.setSummary("Using default Id");
		} else {
			editTextPreference.setSummary("Using CAN id " + canId);
		}
	}

	private void fillMeterByte(String keyMeterByte) {
		Preference preference = findPreference(keyMeterByte);
		ListPreference  listPreference = (ListPreference) preference;
		String canByte = listPreference.getEntry().toString();
		if (canByte == null || canByte.matches("")) {
			listPreference.setSummary("Using default byte nuber");
		} else {
			listPreference.setSummary("Using " + canByte);
		}
		
	}

    
	@Override
    protected void onPause() {
        super.onPause();

        getPreferenceScreen().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);    
    }

    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
    	Preference pref = findPreference(key);

    	if (key.equals(KEY_URL)) {
        	fillUrl();
        }
        
    	if (key.equals(KEY_SESSION_ID)) {    	
    		fillSessionId();
    	}
    	
    	if (key.equals(KEY_ACCESS_PASSWORD)) {    	
    		fillAccessPassword();
    	}
    	
    	if (key.equals(KEY_SESSION_TIMEOUT)) {    	
    		fillSessionTimeout();
    	}
    	
    	if (key.equals(KEY_CAN_HANDLE)) {    	
    		fillCanHandle();
    	}
        
        if (key.equals(KEY_BITRATE)) {
        	fillBitrate();
        }
        
        if (key.equals(KEY_DRIVER_TYPE)) {
        	fillDriverType();
        }
        
        if (key.equals(KEY_READ_BURST_SIZE)) {
        	fillReadBurstSize();
        }
        
        if (key.equals(KEY_READ_DELAY)) {
        	fillReadDelay();
        }

        if (key.equals(KEY_METER_ALPHA_ID) ||
        	key.equals(KEY_METER_BETA_ID) ||
        	key.equals(KEY_METER_GAMMA_ID)) {
        	fillMeterId(key);
        }
    
        
        if (key.equals(KEY_METER_ALPHA_BYTE) ||
            key.equals(KEY_METER_BETA_BYTE) ||
            key.equals(KEY_METER_GAMMA_BYTE)) {
        	fillMeterByte(key);
        }
        
    }
}