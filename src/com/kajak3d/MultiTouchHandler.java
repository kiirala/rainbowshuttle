package com.kajak3d;

import com.kajak3d.TouchListenerJNI;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

/**
 * Multitoucher listener for Android 2.0 and higher.
 * @author teemu
 *
 */

public class MultiTouchHandler implements OnTouchListener {

	public boolean onTouch(View v, MotionEvent event) 
	{
		//dumpEvent(event);
		
		int action = event.getAction();
		int actionCode = action & MotionEvent.ACTION_MASK;		
		int pointerId = action >> MotionEvent.ACTION_POINTER_ID_SHIFT;	
		int kajakAction = getKajakEventCode(actionCode);
				
		if (actionCode == MotionEvent.ACTION_DOWN || actionCode == MotionEvent.ACTION_UP)
		{
			TouchListenerJNI.nativeHandleTouchEvent(kajakAction, pointerId, (int)event.getX(pointerId), (int)event.getY(pointerId));
			return true;
		}
		else if (actionCode == MotionEvent.ACTION_MOVE) 
		{
			for (int i = 0; i < event.getPointerCount(); ++i) 
			{
				TouchListenerJNI.nativeHandleTouchEvent(getKajakEventCode(MotionEvent.ACTION_MOVE),
										event.getPointerId(i), (int) event.getX(i), (int) event.getY(i));
			}
		}
		return true;
	}
	
	private int getKajakEventCode(int actionCode)
	{
		switch (actionCode) {
		case MotionEvent.ACTION_POINTER_DOWN:
		case MotionEvent.ACTION_DOWN: {
			return 1;
		}
		case MotionEvent.ACTION_POINTER_UP:
		case MotionEvent.ACTION_UP: {
			return 2;
		}
		case MotionEvent.ACTION_MOVE: {
			return 3;
		}
		default:
			break;
		}		
		return 0;		
	}
	
    private void dumpEvent(MotionEvent evt)
    {
    	String names[] = { "DOWN", "UP", "MOVE", "CANCEL", "OUTSIDE", "POINTER_DOWN", "POINTER_UP", "7", "8", "9", "10" };
    	int actionCode = evt.getAction() & MotionEvent.ACTION_MASK;
    	StringBuilder sb = new StringBuilder();
    	sb.append("event ACTION_").append(names[actionCode]);
    	
    	if (actionCode == MotionEvent.ACTION_POINTER_DOWN || actionCode == MotionEvent.ACTION_POINTER_UP)
    	{
    		int pointerId = evt.getAction() >> MotionEvent.ACTION_POINTER_ID_SHIFT;
    		sb.append(" (pid ").append(pointerId).append(")");
    	}
    	
    	sb.append(" [");
    	for (int i = 0 ; i < evt.getPointerCount() ; ++i)
    	{
    		sb.append("#").append(i);
    		sb.append("(pid ").append(evt.getPointerId(i));
    		sb.append(") ").append(" X:").append((int)evt.getX(i)).append(" Y:").append((int)evt.getY(i));    		
    	}
    	Log.d("dumpEvent", sb.toString());
    }
}
