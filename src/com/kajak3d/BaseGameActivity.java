package com.kajak3d;

import java.util.List;
import java.util.Queue;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.ConfigurationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.PixelFormat;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.widget.Toast;

/**
* android java kajak3d base game activity
*
* Last update: 
* 21th february 2011
*
* see comments in code for enabling OGL ES 2.0 support.
*
*/
public class BaseGameActivity extends Activity 
{
	private AccelerometerEventListener	accelerometerListener	= null;
    private MyGLSurfaceView.Renderer mRenderer = null;
    private MyGLSurfaceView				view					= null;
    private Display						defaultDisplay			= null;
    private PixelFormat					pixelFormat				= null;
    private SensorManager				sensorMgr				= null;
    private PowerManager.WakeLock		mWakeLock				= null;
    
	static private boolean 				activityInitialized		= false;
	private String						apkFilePath				= null;
	private int							screenWidth;
	private int							screenHeight;
	private int							bpp;
	
	private GameSoundManager			mSoundMgr				= null;
	private int							mSampleId				= -1;
	
	private native void nativeCreateKajak3DConfig(Activity thisPtr, String apkPath, String storagePath, int width, int height, int bpp);
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);        
		Log.d("BaseGameActivity", "Running Android OS Version " + Integer.toString(android.os.Build.VERSION.SDK_INT));
		
		PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		mWakeLock = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, "BaseGameWakeLock");
		
		setVolumeControlStream(AudioManager.STREAM_MUSIC);
		
		if (!activityInitialized)
		{
			try 
			{
				getApkPath();
			} 
			catch (NameNotFoundException e)
			{
				Log.d("BaseGameActivity", "package name not found, aborting...");
				this.finish();
			}
			
			getDisplayInfo();

			// uncomment this to enable accelerometer
			setupSensorService();
			
			nativeCreateKajak3DConfig(this, apkFilePath, this.getFilesDir().toString(), screenWidth, screenHeight, pixelFormat.bytesPerPixel);

			view = new GameView(this);    					 // comment this out if you want opengl 2.0 es, and
			//view = new OpenGL20SurfaceView(getApplication());  // uncomment this if you want opengl es 2.0
			mRenderer = new OpenGL11Renderer();
			view.setRenderer(mRenderer);
			//view.setRenderMode(MyGLSurfaceView.RENDERMODE_WHEN_DIRTY);			
			view.setFocusable(true);
			view.setFocusableInTouchMode(true);
			
			if (supportMultitouch())
			{
				view.setOnTouchListener(new MultiTouchHandler());
				Log.d("BaseGameActivity", "Multitouch is supported.");
			}
			else
			{
				view.setOnTouchListener(new SingleTouchHandler());
				Log.d("BaseGameActivity", "Multitouch is NOT supported.");				
			}
			
			mSoundMgr = new GameSoundManager(this);
			mSoundMgr.setAudio();

			setContentView(view);
			activityInitialized = true;
		}
    }
    
    @Override
    public void onPause()
    {	
    	super.onPause();
    	Log.d("BaseGameActivity", "onPause");
    	mSoundMgr.pauseMusicStreams();

    	mWakeLock.release();
    }

    @Override
    public void onResume()
    {
    	super.onResume();
    	Log.d("BaseGameActivity", "onResume");
    	mSoundMgr.resumeMusicStreams();
    	
    	mWakeLock.acquire();
    }

    @Override
    public void onDestroy()
    {
        Log.d("BaseGameActivity", "onDestroy");
        mSoundMgr.release();
    }
    
    public void killApplication()
    {
    	Log.d("BaseGameActivity", "killApplication");
		
		if (accelerometerListener != null)
			sensorMgr.unregisterListener(accelerometerListener);
    	System.exit(1);
    	this.finish();
    }
    
    private boolean supportsOpenGLES20() 
    {
    	ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
    	ConfigurationInfo info = am.getDeviceConfigurationInfo();
    	return info.reqGlEsVersion >= 0x20000;
    }
    
    private boolean setupSensorService()
    {
    	sensorMgr = (SensorManager)this.getSystemService(Context.SENSOR_SERVICE);
    	List<Sensor> listOfSensors = sensorMgr.getSensorList(Sensor.TYPE_ACCELEROMETER);
    	
    	if (listOfSensors.isEmpty()) return false;
    	
    	accelerometerListener = new AccelerometerEventListener();
    	sensorMgr.registerListener(accelerometerListener, sensorMgr.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_GAME);
    	
    	return true;
    }

	private boolean supportMultitouch()
	{
		final int requiredVersionForMultitouch = 5;
		return (android.os.Build.VERSION.SDK_INT >= requiredVersionForMultitouch);
	}
		 
    private void getApkPath() throws NameNotFoundException
    {
    	String packageName = this.getClass().getPackage().getName();
    	Log.d("BaseGameActivity", "package name: " + packageName);
    	
        ApplicationInfo appInfo = null;
        PackageManager packageManager = this.getPackageManager();
        appInfo = packageManager.getApplicationInfo(packageName, 0);        	
        
    	apkFilePath = appInfo.sourceDir;
    }
    
    private void getDisplayInfo()
    {
		defaultDisplay = ((WindowManager) this.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();

		screenWidth = defaultDisplay.getWidth();
		screenHeight = defaultDisplay.getHeight();

		pixelFormat = new PixelFormat();
		PixelFormat.getPixelFormatInfo(defaultDisplay.getPixelFormat(), pixelFormat);   	    	
    }
    
    static 
    {
        System.loadLibrary("Kajak3DGameWrapper"); 
    }

}
