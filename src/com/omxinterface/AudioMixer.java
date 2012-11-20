package com.omxinterface;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;

import android.nfc.FormatException;
import android.util.Log;

import java.io.InputStream;
import java.io.PushbackInputStream;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.List;

import com.coremedia.iso.IsoFile;
import com.coremedia.iso.boxes.MovieBox;
import com.coremedia.iso.boxes.sampleentry.AudioSampleEntry;
import com.googlecode.mp4parser.authoring.Movie;
import com.googlecode.mp4parser.authoring.Track;
import com.googlecode.mp4parser.authoring.builder.DefaultMp4Builder;
import com.googlecode.mp4parser.authoring.container.mp4.MovieCreator;
import com.googlecode.mp4parser.authoring.tracks.AACTrackImpl;
import com.googlecode.mp4parser.authoring.tracks.H264TrackImpl;

public class AudioMixer {

    public static void Mix(String mp4InFile, String m4aAudioFile, String mp4OutFile) throws IOException, FormatException {
    	
    	RandomAccessFile fs = null;
    	try {
        	fs = new RandomAccessFile(mp4InFile, "r");    		
    	} catch (FileNotFoundException e) {
    		e.printStackTrace(System.err);
    		throw new IOException("File not found " + mp4InFile);
    	}
    	
    	FileChannel fc = fs.getChannel();

    	Movie video = MovieCreator.build(fc);
    	

    	List<Track> tracks = video.getTracks();
    	Log.d("AudioMixer", video.toString());
    	Track mp4AudioTrack = null;
        Track mp4VideoTrack = null; 
        
        for (Track t : tracks) {
            String type = t.getMediaHeaderBox().getType();
            if (type.equals("moov")) {
            	mp4VideoTrack = t;
            }
        }

        if ((mp4VideoTrack == null) || (mp4AudioTrack == null)) {
        	throw new FormatException("Invalid video file format");
        }
        
        InputStream m4aInput = new FileInputStream(m4aAudioFile);
        PushbackInputStream m4aPBI = new PushbackInputStream(m4aInput, 100);
        Track m4aAudioTrack = new AACTrackImpl(m4aPBI);
        
        // this should really be on a frame by frame basis and streaming.
        List<ByteBuffer> mp4AudioSamples = mp4AudioTrack.getSamples();
        
        // this should really be on a frame by frame basis and streaming.
        List<ByteBuffer> m4AAudioSamples = m4aAudioTrack.getSamples();
        
        OMXInterface omxInterface = new OMXInterface();
        
        AudioSampleEntry ase = (AudioSampleEntry)mp4AudioTrack.getSampleDescriptionBox().getSampleEntry();
        
        for (ByteBuffer curBuff : mp4AudioSamples)
        {
        	byte[] srcData = curBuff.array();
        	byte[] destData = null;
        	omxInterface.EncodeDecodeAudio("audio/x-aac", srcData, ase.getChannelCount(), (int)ase.getSampleRate(), destData, false);        	
        }
        
        Movie movie = new Movie();
        movie.addTrack(m4aAudioTrack);
        movie.addTrack(mp4VideoTrack);

        IsoFile out = new DefaultMp4Builder().build(movie);
        FileOutputStream output = new FileOutputStream(mp4OutFile);
        out.getBox(output.getChannel());
        output.close();
        
        fs.close();
        
    }
}
