package com.kvaser.canrestdemo;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.json.JSONObject;
import org.json.JSONTokener;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.CompoundButton.OnCheckedChangeListener;

public class WriteActivity extends MenuActivity {
	protected static final String TAG = "WriteActivity";
	
	public static Context writeContext;
	private static boolean dataIsValid;
	private String idString;
	private boolean idStringIsValid;
	private String dlcString;
	private boolean dlcStringIsValid;
	private String flagString;
	private ArrayList<String> dataList;

	private ArrayList<Boolean> dataIsValidList;
	
	static TextView t_logTop = null;
	static TextView t_logBottom = null;
	CheckBox chk_extended_flag = null;
	RestService restServiceCanWrite;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
		
        setContentView(R.layout.activity_write);
	    writeContext      = this;
	    idString          = "100";
	    idStringIsValid   = true;
	    dlcString         = "0";
	    dlcStringIsValid  = true;
	    flagString        = "0";
	    dataList = new ArrayList<String>();
	    dataList.add(0, "0");
	    dataList.add(1, "0");
	    dataList.add(2, "0");
	    dataList.add(3, "0");
	    dataList.add(4, "0");
	    dataList.add(5, "0");
	    dataList.add(6, "0");
	    dataList.add(7, "0");
	    dataIsValidList = new ArrayList<Boolean>();
	    dataIsValidList.add(0, true);
	    dataIsValidList.add(1, true);
	    dataIsValidList.add(2, true);
	    dataIsValidList.add(3, true);
	    dataIsValidList.add(4, true);
	    dataIsValidList.add(5, true);
	    dataIsValidList.add(6, true);
	    dataIsValidList.add(7, true);
	    
        t_logTop = (TextView) findViewById(R.id.t_logTop);
        t_logTop.setMovementMethod(new ScrollingMovementMethod());
        
        t_logBottom = (TextView) findViewById(R.id.t_logBottom);
        t_logBottom.setMovementMethod(new ScrollingMovementMethod());
        t_logBottom.setText("Empty...");
        
        Button b_canSend = (Button) findViewById(R.id.b_send);
    	b_canSend.setOnClickListener(new View.OnClickListener(){
    		public void onClick(View view){
    			if (dataIsValid) {
    				sendMessage();
    			} else {
    				showShortToast("Not all data fields are valid!");
    			}
    		}
    	});
    	
    	EditText editText_d0 = (EditText) findViewById(R.id.data00);
    	editText_d0.addTextChangedListener(new DataTextWatcher(editText_d0, 0));
    	EditText editText_d1 = (EditText) findViewById(R.id.data01);
    	editText_d1.addTextChangedListener(new DataTextWatcher(editText_d1, 1));
    	EditText editText_d2 = (EditText) findViewById(R.id.data02);
    	editText_d2.addTextChangedListener(new DataTextWatcher(editText_d2, 2));
    	EditText editText_d3 = (EditText) findViewById(R.id.data03);
    	editText_d3.addTextChangedListener(new DataTextWatcher(editText_d3, 3));
    	EditText editText_d4 = (EditText) findViewById(R.id.data04);
    	editText_d4.addTextChangedListener(new DataTextWatcher(editText_d4, 4));
    	EditText editText_d5 = (EditText) findViewById(R.id.data05);
    	editText_d5.addTextChangedListener(new DataTextWatcher(editText_d5, 5));
    	EditText editText_d6 = (EditText) findViewById(R.id.data06);
    	editText_d6.addTextChangedListener(new DataTextWatcher(editText_d6, 6));
    	EditText editText_d7 = (EditText) findViewById(R.id.data07);
    	editText_d7.addTextChangedListener(new DataTextWatcher(editText_d7, 7));
    	
    	EditText editText_canId = (EditText) findViewById(R.id.canId);
    	editText_canId.addTextChangedListener(new canIdTextWatcher(editText_canId));
    	
    	EditText editText_dlc = (EditText) findViewById(R.id.dlc);
    	editText_dlc.addTextChangedListener(new canDlcTextWatcher(editText_dlc));

    	chk_extended_flag = (CheckBox) findViewById(R.id.chk_extended);
        chk_extended_flag.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if (chk_extended_flag.isChecked()) {
					flagString   = String.valueOf(CanlibRest.canMSG_EXT);
		        } else {
		        	flagString   = "0";
		        }        		
				SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(writeContext);            
                preferences.edit().putString(PrefsActivity.KEY_WRITE_FLAG, flagString).commit();

				initDataToBeSent();
			}

		});
        
		initDataToBeSent();
    }

    public static void showShortToast(String text) {
        Toast.makeText(writeContext, text, Toast.LENGTH_LONG).show();
    }

    public void validateData(EditText editText, int index) {
    	String stringValue = editText.getText().toString();
    	if (stringValue.matches("")) {
    		dataIsValidList.set(index, false);
    		editText.setError("Don't leave it empty");
    	} else if (Integer.valueOf(stringValue) > 255) {
    		dataIsValidList.set(index, false);
    		editText.setError("Max 255");
    	} else {
        	dataIsValidList.set(index, true);
    		editText.setError(null);
    		dataList.set(index, stringValue);
    		String dataString = TextUtils.join(",", dataList);
    		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);            
            preferences.edit().putString(PrefsActivity.KEY_WRITE_DATA, dataString).commit();
    		initDataToBeSent();
    	}
    }
    
    public void validateId(EditText editText) {
    	String stringValue = editText.getText().toString();
    	
    	if (stringValue.matches("")) {
        	idStringIsValid = false;
    		editText.setError("Don't leave it empty");
    		return;
    	}
    	
    	try {
			if (Integer.valueOf(stringValue) > 0xFFFFFFF) {
    			idStringIsValid = false;
    			editText.setError("Value to large");
    		} else {
            	idStringIsValid = true;
        		editText.setError(null);
        		idString = stringValue;
        		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);            
                preferences.edit().putString(PrefsActivity.KEY_WRITE_ID, idString).commit();
        		initDataToBeSent();
        	}
		}
		catch (Exception e) {
			idStringIsValid = false;
			editText.setError("Value to large");
		}	
    }

    public void validateDlc(EditText editText) {
    	String stringValue = editText.getText().toString();
    	if (stringValue.matches("")) {
        	dlcStringIsValid = false;
    		editText.setError("Don't leave it empty");
    	} else if (Integer.valueOf(stringValue) > 15) {
        	dlcStringIsValid = false;
    		editText.setError("Max 15");
    	} else {
        	dlcStringIsValid = true;
    		editText.setError(null);
    		dlcString = stringValue;
    		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);            
            preferences.edit().putString(PrefsActivity.KEY_WRITE_DLC, dlcString).commit();
    		initDataToBeSent();
    	}
    }

    private void initDataToBeSent() {
    	
    	dataIsValid = idStringIsValid && dlcStringIsValid;
    	for (int i = 0; i < dataIsValidList.size(); i++) {
    		dataIsValid = dataIsValid && dataIsValidList.get(0);
    	}
    	
    	if (dataIsValid) {
    		SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
    		String url = preferences.getString(PrefsActivity.KEY_URL, "http://192.168.1.10:8080");
    		String sessionId = preferences.getString(PrefsActivity.KEY_SESSION_ID, "");
    		int canHandle = Integer.valueOf(preferences.getString(PrefsActivity.KEY_CAN_HANDLE, "0"));
            String url_session = url + "/" + sessionId;

            restServiceCanWrite = new RestService(mHandlerGet, this, url_session + "/canWrite", RestService.GET);
            restServiceCanWrite.addParam("ident", Integer.toString(RestActivity.IDENT_WRITE));
            restServiceCanWrite.addParam("hnd", Integer.toString(canHandle));
            restServiceCanWrite.addParam("id", idString);
            restServiceCanWrite.addParam("dlc", dlcString);
            restServiceCanWrite.addParam("flag", flagString);
            
            String msgString = dataList.get(0).toString();
            msgString += "," + dataList.get(1).toString();
            msgString += "," + dataList.get(2).toString();
            msgString += "," + dataList.get(3).toString();
            msgString += "," + dataList.get(4).toString();
            msgString += "," + dataList.get(5).toString();
            msgString += "," + dataList.get(6).toString();
            msgString += "," + dataList.get(7).toString();
            restServiceCanWrite.addParam("msg", msgString);
    		
    	} else {
    		showShortToast("Not all data fields are valid!");
    	}
    }


	private static final Handler mHandlerGet = new Handler(){
    	@Override
    	public void handleMessage(Message msg){
    			Bundle bundle = (Bundle) msg.obj;
    			int responseCode = bundle.getInt("responseCode");
    	        
    			if (responseCode == 200) {
    				parseResult(bundle.getString("result"));
    			}
    			t_logTop.append(bundle.getString("result"));
    		}
    	
    	private void parseResult(String json_text) {
	        try {
	        	JSONTokener jsonTokener = new JSONTokener(json_text);
	        	JSONObject json = (JSONObject) jsonTokener.nextValue();
	        	int status = json.getInt("stat");
	        	t_logBottom.append("\n" + CanlibRest.getErrorText(status));
	        }
	        catch (Exception e) {
	            Log.e(TAG, "Get: Failed to parse JSON.", e);
	        }
    	}


    };

	private class DataTextWatcher implements TextWatcher {
    	protected EditText editText;
    	protected int index;

    	public DataTextWatcher(EditText editText, int index) {
    		this.editText = editText;
    		this.index    = index;
    	}
		@Override
		public void afterTextChanged(Editable arg0) {
		}

		@Override
		public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
		}

		@Override
		public void onTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
			validateData(editText, index);
		}    	
    }
    
    private class canIdTextWatcher extends DataTextWatcher {

		public canIdTextWatcher(EditText editText) {
			super(editText, -1);
		}

		@Override
		public void onTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
			validateId(editText);
		}
    }

    private class canDlcTextWatcher extends DataTextWatcher {


		public canDlcTextWatcher(EditText editText) {
			super(editText, -1);
		}

		@Override
		public void onTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
			validateDlc(editText);
		}
    }
    
    
    private void sendMessage() {
    	try {
    		t_logTop.setText("Calling CAN Write\n" + restServiceCanWrite.getString() + "\n------\n");
			restServiceCanWrite.execute();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
    
	@Override
	public void onResume() {
		super.onResume();
        Log.d(TAG, "onResume()");
        
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        
        idString = preferences.getString(PrefsActivity.KEY_WRITE_ID, idString);
		dlcString = preferences.getString(PrefsActivity.KEY_WRITE_DLC, dlcString);
		flagString = preferences.getString(PrefsActivity.KEY_WRITE_FLAG, flagString);
		
		String dataString = preferences.getString(PrefsActivity.KEY_WRITE_DATA, TextUtils.join(",", dataList));
		List<String> dList = Arrays.asList(TextUtils.split(dataString, ","));
		dataList = new ArrayList<String>(dList);
		
		EditText editText_dlc = (EditText) findViewById(R.id.dlc);
		editText_dlc.setText(dlcString);
		EditText editText_id = (EditText) findViewById(R.id.canId);
		editText_id.setText(idString);
		chk_extended_flag.setChecked(flagString.matches(String.valueOf(CanlibRest.canMSG_EXT)));
		
    	EditText editText_d0 = (EditText) findViewById(R.id.data00);
    	editText_d0.setText(dataList.get(0).toString());
    	EditText editText_d1 = (EditText) findViewById(R.id.data01);
    	editText_d1.setText(dataList.get(1).toString());
    	EditText editText_d2 = (EditText) findViewById(R.id.data02);
    	editText_d2.setText(dataList.get(2).toString());
    	EditText editText_d3 = (EditText) findViewById(R.id.data03);
    	editText_d3.setText(dataList.get(3).toString());
    	EditText editText_d4 = (EditText) findViewById(R.id.data04);
    	editText_d4.setText(dataList.get(4).toString());
    	EditText editText_d5 = (EditText) findViewById(R.id.data05);
    	editText_d5.setText(dataList.get(5).toString());
    	EditText editText_d6 = (EditText) findViewById(R.id.data06);
    	editText_d6.setText(dataList.get(6).toString());
    	EditText editText_d7 = (EditText) findViewById(R.id.data07);
    	editText_d7.setText(dataList.get(7).toString());
    	editText_d7.requestFocus();
    	
    	initDataToBeSent();
	}

}
