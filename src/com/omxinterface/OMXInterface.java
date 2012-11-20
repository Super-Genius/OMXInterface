package com.omxinterface;

public class OMXInterface {

	    /**
	     * Native JNI - Encode or Decode AAC to sample data
	     * 
	     */
	    public native void EncodeDecodeAudio(String mimeType, byte[] srcData, int nChannels, int sampleRate, byte[] destData, boolean encode);
	    
	    static {
	        System.loadLibrary("OMXInterface");
	    }
	
}
