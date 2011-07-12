package com.kajak3d;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.util.Log;

public class AccelerometerEventListener implements SensorEventListener {

	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub

	}

	public void onSensorChanged(SensorEvent event) 
	{
		/*Log.d("onSensorChanged", "XYZ-value " + Float.toString(event.values[0]) + " " +
				Float.toString(event.values[1]) + " " +
				Float.toString(event.values[2]) );
		*/
		nativeSensorEvent(0, event.values[0], event.values[1], event.values[2]);
	}
	
	private static native void nativeSensorEvent(int sensorId, float x, float y, float z);
}
