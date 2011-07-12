package com.kajak3d;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.util.Log;

public class OpenGL11Renderer extends RendererJNI implements MyGLSurfaceView.Renderer 
{
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	Log.d("OpenGL11Renderer", "onSurfaceCreated");
    	nativeCreateKajak3DGameApp();
    	nativeResumed();
	}
	
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		nativeResize(width, height);
	}
	
	public void onDrawFrame(GL10 gl) 
	{
		nativeRender(this);
	}

	public void onSurfaceLost()
	{
	}
}
