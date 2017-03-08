package com.kvaser.canrestdemo;

import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.MenuItem;


public class MenuActivity extends Activity {
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}
	
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
      switch (item.getItemId()) {
      	case R.id.menu_settings:
      		startActivity(new Intent(this, PrefsActivity.class));
      		break;
      	case R.id.menu_raw_rest:
      		startActivity(new Intent(this, RestActivity.class));
      		break;
      	case R.id.menu_read_log:
      		startActivity(new Intent(this, ReadLogActivity.class));
      		break;
      	case R.id.menu_write:
      		startActivity(new Intent(this, WriteActivity.class));
      		break;
      	case R.id.menu_prop:
      		startActivity(new Intent(this, PropActivity.class));
      		break;
      	default:
      		return super.onOptionsItemSelected(item);
      }
      return true;
    }
}
