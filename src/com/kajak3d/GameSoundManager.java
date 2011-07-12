package com.kajak3d;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.util.Log;

import android.os.Vibrator;

public class GameSoundManager extends AudioJNI
{
	private static final int MAX_STREAMS = 5;
	
	private Context mContext = null;
	private SoundPool mPool = null;
	private AudioManager mAudioManager = null;
	
	private HashMap<Integer, Integer> mCurrentlyPlaying = null;
	private HashMap<String, Integer> mLoadedMusicsFileToIdMap = null;
	private HashMap<Integer, MediaPlayer> mLoadedMusicIdToObjectMap = null;
	private int mMpIdCount = 0;

	private Vibrator mVibrator = null;

	GameSoundManager(Context appContext)
	{
		mContext = appContext;		
		mPool = new SoundPool(MAX_STREAMS, AudioManager.STREAM_MUSIC, 0);
		mAudioManager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
		
		mLoadedMusicsFileToIdMap = new HashMap<String, Integer>();
		mLoadedMusicIdToObjectMap = new HashMap<Integer, MediaPlayer>();
		mCurrentlyPlaying = new HashMap<Integer, Integer>();

		mVibrator = (Vibrator) mContext.getSystemService(Context.VIBRATOR_SERVICE);
	}
	
	public void setAudio()
	{
		setAudio(this);
	}
	
	public int loadSound(final String filename)
	{
		Log.d("GameSoundManager", "loadSound() called, filename: " + filename);
		try
		{
			AssetFileDescriptor afd = mContext.getAssets().openFd(filename);
			return mPool.load(afd, 1);
		}
		catch (IOException e)
		{
			Log.d("GameSoundManager", e.toString());
		}
		
		return -1;
	}
	
	public int loadMusic(final String filename)
	{
		Log.d("GameSoundManager", "loadMusic() called, filename: " + filename);		
		if (mLoadedMusicsFileToIdMap.containsKey(filename))
		{
			int id = mLoadedMusicsFileToIdMap.get(filename);
			Log.d("GameSoundManager", "found in map, returning id: " + Integer.toString(id));		
			return id;
		}
				
		try
		{
			AssetFileDescriptor afd = mContext.getAssets().openFd(filename);
			MediaPlayer mp = new MediaPlayer();
			mp.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
			mp.prepare();
			++mMpIdCount;
			mLoadedMusicsFileToIdMap.put(filename, mMpIdCount);
			mLoadedMusicIdToObjectMap.put(mMpIdCount, mp);
			Log.d("GameSoundManager", "loadMusic, new entry: " + filename + " musicId: " + Integer.toString(mMpIdCount));			
		}
		catch (IOException e)
		{
			Log.d("GameSoundManager", e.toString());
			return -1;
		}
		catch (IllegalStateException e)
		{
			Log.d("GameSoundManager", e.toString());
		}
		catch (IllegalArgumentException e)
		{
			Log.d("GameSoundManager", e.toString());			
		}
		
		return mMpIdCount;
	}
	
	public void unloadSound(int soundId)
	{
		mPool.unload(soundId);
		Log.d("GameSoundManager", "unloadSound() released id: " + Integer.toString(soundId));		
	}
	
	public void unloadMusic(int musicId)
	{
		if (mLoadedMusicIdToObjectMap.containsKey(musicId) == true)
		{
			MediaPlayer mp = mLoadedMusicIdToObjectMap.get(musicId);
			mp.release();
			Log.d("GameSoundManager", "unloadMusic() released id: " + Integer.toString(musicId));
		}
	}
	
	public int playSound(int soundId, int loopCount)
	{
		final int retVal = mPool.play(soundId, 1, 1, 1, loopCount, 1);
		Log.d("GameSoundManager", "playSound() retVal " + Integer.toString(retVal));
		return retVal;
	}
	
	public int playMusic(int musicId, int looping)
	{
		MediaPlayer mp = mLoadedMusicIdToObjectMap.get(musicId);
		Log.d("GameSoundManager", "attempting to playMusic() musicId " + Integer.toString(musicId));
		
		if (mp != null && !mCurrentlyPlaying.containsKey(musicId))
		{
			mp.setLooping(1 == looping);			
			mp.start();
			mCurrentlyPlaying.put(musicId, musicId);
			return 1;				
		}
		
		return -1;
	}
	
	public void stopSound(int soundId)
	{
		mPool.stop(soundId);
		Log.d("GameSoundManager", "stopSound() Id " + Integer.toString(soundId));		
	}
	
	public void stopMusic(int musicId) throws IllegalStateException, IOException
	{
		if(mCurrentlyPlaying.containsKey(musicId))
		{
			MediaPlayer mp = mLoadedMusicIdToObjectMap.get(musicId);
			if (mp != null)
			{
				mp.stop();
				mp.prepare();
				mCurrentlyPlaying.remove(musicId);
			}
		}
	}
	
	public void pauseSound(int soundId)
	{
		// not possible right now...
	}
	
	public void pauseMusic(int musicId)
	{
		if (mCurrentlyPlaying.containsKey(musicId))
		{
			MediaPlayer mp = mLoadedMusicIdToObjectMap.get(musicId);
			if (mp != null)
			{
				mp.pause();
				mCurrentlyPlaying.remove(musicId);
			}
		}
	}
	
	public void release()
	{
		mPool.release();
		mPool = null;
	}
	
	public void resumeMusicStreams()
	{
		Object[] listOfMps = mCurrentlyPlaying.values().toArray();
		for (int i=0 ; i < listOfMps.length ; ++i )
		{
			MediaPlayer mp = mLoadedMusicIdToObjectMap.get( (Integer) listOfMps[i]);
			mp.start();
		}
	}

	public void pauseMusicStreams()
	{
		Object[] listOfMps = mCurrentlyPlaying.values().toArray();
		for (int i=0 ; i < listOfMps.length ; ++i )
		{
			MediaPlayer mp = mLoadedMusicIdToObjectMap.get( (Integer) listOfMps[i]);			
			mp.pause();
		}
	}

	public void vibrate(int milliseconds)
	{
		mVibrator.vibrate(milliseconds);
	}
}

