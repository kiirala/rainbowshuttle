package com.kajak3d;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceHolder;

public class GameView extends MyGLSurfaceView 
{
    private static final int KAJAK3D_BACK_KEY = 4;
    private static final int KAJAK3D_MENU_KEY = 5;
    
	public GameView(Context context) {
		super(context);
	}

	public GameView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) 
	{
    	if (keyCode == KeyEvent.KEYCODE_BACK)
    	{
    		Log.d("GameView", "back button down");
    		queueEvent(new Runnable() {				
				public void run() 
				{
		    		KeyEventJNI.nativeHandleKeyEvent(KAJAK3D_BACK_KEY, 1);
				}
			});
			event.startTracking();
			return true;			
    	}
    	else if (keyCode == KeyEvent.KEYCODE_MENU)
    	{
    		Log.d("KeyHandler", "home button down");
    		queueEvent(new Runnable() {				
				public void run() 
				{    		
					KeyEventJNI.nativeHandleKeyEvent(KAJAK3D_MENU_KEY, 1);
				}
    		});
			event.startTracking();
			return true;			
    	}		
    	
		return false;
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) 
	{
    	if (keyCode == KeyEvent.KEYCODE_BACK)
    	{
    		Log.d("GameView", "back button up");
    		queueEvent(new Runnable() {				
				public void run() 
				{
		    		KeyEventJNI.nativeHandleKeyEvent(KAJAK3D_BACK_KEY, 2);					
				}
			});
    		return true;    		
    	}
    	else if (keyCode == KeyEvent.KEYCODE_MENU)
    	{
    		Log.d("KeyHandler", "home button up");
    		queueEvent(new Runnable() {				
				public void run() 
				{    		
					KeyEventJNI.nativeHandleKeyEvent(KAJAK3D_MENU_KEY, 2);
				}
    		});
    		return true;    		
    	}
    	
		return false;
	}

	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event)
	{
		if (event.getAction() == event.ACTION_UP)
		{
			Log.d("GameView", "onKeyLongPress button up");
			return onKeyUp(keyCode, event);
		}
		
		return super.onKeyLongPress(keyCode, event);
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) 
	{
		super.surfaceDestroyed(holder);
		queueEvent(new Runnable() 
		{				
			public void run() 
			{    				
				RendererJNI.nativePaused();
			}
		});
	}
}
