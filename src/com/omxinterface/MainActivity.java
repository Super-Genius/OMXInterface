package com.omxinterface;

import java.io.IOException;
import java.io.InputStream;

import com.coremedia.iso.PropertyBoxParserImpl;
import com.telly.omxinterface.R;

import android.nfc.FormatException;
import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.view.Menu;

public class MainActivity extends Activity {

	String extStorage = Environment.getExternalStorageDirectory().getPath(); 
	 
    private String MP4_FILE = extStorage + "/movie.mp4";
    private String M4A_FILE = extStorage + "/music.m4a";
    private String MP4_OUT_FILE = extStorage + "/movie-mix.mp4";
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        InputStream is;
        is = getResources().openRawResource(R.raw.isoparser);
        
        PropertyBoxParserImpl.SetParserProperties(is);
        try {
        	AudioMixer.Mix(MP4_FILE,  M4A_FILE, MP4_OUT_FILE);
		} catch (IOException e) {
			e.printStackTrace();
		} catch (FormatException e) {
			e.printStackTrace();
		}
        
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
}
