package com.kajak3d;

import com.kajak3d.TouchListenerJNI;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

/**
 * TouchEvent Listener for Android 1.6 devices which don't support multitouch.
 * @author teemu
 *
 */
public class SingleTouchHandler implements OnTouchListener {

	public boolean onTouch(View v, MotionEvent event) 
	{
    	int actionCode = event.getAction();
    	int action = 0;
    	
		switch (actionCode) {
		case MotionEvent.ACTION_DOWN: {
			action = 1;
		}
			break;
		case MotionEvent.ACTION_UP: {
			action = 2;
		}
			break;
		case MotionEvent.ACTION_MOVE: {
			action = 3;
		}
			break;

		default:
			break;
		}
		
		//dumpEvent(event);
		TouchListenerJNI.nativeHandleTouchEvent(action, 0, (int)event.getX(), (int)event.getY());
		return true;
	}
	
	private void dumpEvent(final MotionEvent event)
	{
		Log.d("SingleTouchHandler", "eventCode " + Integer.toString(event.getAction()));		
	}
	

}
