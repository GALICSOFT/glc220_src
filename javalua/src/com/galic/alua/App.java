// Program Start....
//
///////////////////////////////////////////////////////////////////////////////

package com.galic.alua;						// declare the package

import gpack.JniGlue;						// g-pack package
import android.app.Activity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.os.Bundle;

public class App extends Activity
{
	@Override protected void onCreate(Bundle savedInstanceState)
	{
		JniGlue.Create(this);
		super.onCreate(savedInstanceState);
	}

	@Override protected void onResume()
	{
		JniGlue.Resume();
		super.onResume();
	}

	@Override protected void onPause()
	{
		JniGlue.Pause();
		super.onPause();
	}

	@Override protected void onStop()
	{
		JniGlue.Stop();
		super.onStop();
	}

	@Override protected void onDestroy()
	{
		super.onDestroy();
	}

	@Override public boolean onTouchEvent(MotionEvent ev)
	{
		JniGlue.TouchEvent(ev);
		return true;
	}

	@Override public boolean onKeyDown(int key, KeyEvent ev)
	{
		if(JniGlue.KeyDown(key, ev))
			return true;

		return super.onKeyDown(key, ev);
	}

	@Override public boolean onKeyUp(int key, KeyEvent ev)
	{
		if(JniGlue.KeyUp(key, ev))
			return true;

		return super.onKeyUp(key, ev);
	}

	@Override public boolean onKeyLongPress(int key, KeyEvent ev)
	{
		if(JniGlue.KeyLongPress(key, ev))
			return true;

		return super.onKeyUp(key, ev);
	}
}

