package com.kvaser.canrestdemo;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends MenuActivity {
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);	
        setContentView(R.layout.activity_main);
	}

	public void startSettings(View view) {
    	startActivity(new Intent(this, PrefsActivity.class));
    }
    
    public void startRawRest(View view) {
    	startActivity(new Intent(this, RestActivity.class));
    }
    
    public void startReadLog(View view) {
    	startActivity(new Intent(this, ReadLogActivity.class));
    }
    
    public void startWrite(View view) {
    	startActivity(new Intent(this, WriteActivity.class));
    }
    
}
