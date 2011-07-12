package com.kajak3d;

import com.kajak3d.MyGLSurfaceView;

public class RendererJNI 
{
    public native void nativeCreateKajak3DGameApp();    
    public static native void nativePaused();
    public native void nativeResumed();
    public native void nativeResize(int w, int h);
    public native void nativeRender(MyGLSurfaceView.Renderer thisPtr);
}
