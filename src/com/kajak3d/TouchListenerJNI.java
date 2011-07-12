package com.kajak3d;

public class TouchListenerJNI 
{
    public static native void nativeHandleTouchEvent(int motionEvent, int pid, int x, int y);
}

