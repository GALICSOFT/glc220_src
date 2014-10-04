// Java Native Interface between g-pack
//
///////////////////////////////////////////////////////////////////////////////

package gpack;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.opengl.GLSurfaceView;
import android.util.Log;

import android.os.Vibrator;
import android.content.Context;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import android.media.MediaPlayer;
import android.media.AudioManager;
//import android.content.*;
//import android.content.pm.*;
//import android.content.Context;
//import android.content.pm.ApplicationInfo;
import android.content.res.*;
import java.io.*;
import java.util.ArrayList;

//import android.view.WindowManager;
//import android.view.Window;


public class JniGlue
{
	////////////////////////////////////////////////////////////////////////////
	// constant
	public static final String	GPACK_CLASS		= "gpack/JniGlue";
	public static final String	TAG				= "G-pack";

	public static final int		LCIK_KEYDOWN	= 1;
	public static final int		LCIK_KEYUP		= 2;
	public static final int		LCIK_KEYPRESS	= 3;


	////////////////////////////////////////////////////////////////////////////
	// Member value

	public static int			m_bCreate	= 0;
	public static GLSurfaceView	m_View		= null;
	public static String		m_szPck		= null;			// Package Name
	public static Activity		m_act		= null;			// Activity Instance

	// audio
	public static AudioManager	m_AudioMng	= null;
	public static int			m_AudioVol	= 0;

	// Media Player for ogg, mp3
	public static ArrayList<MediaPlayer> m_vMpl	= new ArrayList<MediaPlayer>();

	// asset manager
	public static AssetManager	m_asst		= null;

	// haptic motor
	public static Vibrator		m_haptic	= null;

	// for g-sensor
	public static SensorManager	m_sensor	= null;
	public static SensorLstn[]	m_snhw		= new SensorLstn[11];

	// for long press the back key
	public static long			m_exitLast	= System.currentTimeMillis();
	public static long			m_exitCount	= 0;
	public static long			m_exitDelta	= 1800L;

	public static int			m_fd = -1;

	////////////////////////////////////////////////////////////////////////////
	// Renderer class
	public static class Renderer implements GLSurfaceView.Renderer
	{
		public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
			Log.w(TAG, "JniGlue::onSurfaceCreated");
			//gl.glDisable(GL10.GL_DITHER);

			if(0 == m_bCreate)
			{
				JniGlue.SetupSys(m_szPck, GPACK_CLASS);
				JniGlue.CreateFrame();
				m_bCreate = 1;
			}
			else
			{
				JniGlue.Restore();
			}
		}

		public void onSurfaceChanged(GL10 gl, int w, int h)
		{
			gl.glViewport(0, 0, w, h);
		}

		public void onDrawFrame(GL10 gl)
		{
			//gl.glClearColor( 0, 0.6F, 0.8F, 1.0F);
			//gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);

			int hr = 0;

			if(1==m_bCreate)
				hr = JniGlue.Proc();

			if(0>hr)
			{
				Log.w(TAG, "JniGlue::Exit");
				JniGlue.Destroy();
				System.exit(0);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////
	// AndSens class
	public static class SensorLstn implements SensorEventListener
	{
		public void onSensorChanged(SensorEvent ev)
		{
			int   type = ev.sensor.getType();

			float x = ev.values[0];
			float y = ev.values[1];
			float z = ev.values[2];
			//Log.i(TAG, "SensorLstn: " + type + ": " + x + ", " + y + ", " + z );

			JniGlue.SetupSensor(type, x, y, z);
		}
		public void onAccuracyChanged(Sensor s, int a){}
	}

	//
	////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////
	// common interface

	// create sound pool, audio manager, sound list array
	public static void Create(Activity act)
	{
		if(null != act && m_act == act)
			return;

		m_act	= act;
		m_szPck = m_act.getApplicationInfo().packageName;

		Log.w(TAG, "JniGlue::Create:: " + m_szPck);

		if(null == m_szPck)
			return;

		// Hide the window title.
		//m_act.requestWindowFeature(Window.FEATURE_NO_TITLE);
		//m_act.getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

		// Create our surface view and set it as the content of our Activity
		m_View = new GLSurfaceView(m_act);

		m_View.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
		m_View.setRenderer(new Renderer());
		m_act.setContentView(m_View);

		// Create Haptic motor manager
		m_haptic = (Vibrator)m_act.getSystemService(Context.VIBRATOR_SERVICE);

		// Create audio Manager
		m_AudioMng = (AudioManager)m_act.getSystemService(Context.AUDIO_SERVICE);
		m_AudioVol = m_AudioMng.getStreamVolume(AudioManager.STREAM_MUSIC);

		// Create asset manager
		m_asst = m_act.getAssets();


		// Create SensorLstn
		m_sensor = (SensorManager)m_act.getSystemService(Context.SENSOR_SERVICE);
		if(null != m_sensor)
		{
			for(int i=0; i<m_snhw.length; ++i)
				m_snhw[i] = new SensorLstn();
		}

		//if(-1 == m_fd)
		//	m_fd = JniCB_MpCreate("smd/theme.mp3");
		}

	// destroy sound list and g-pack instance
	public static void Destroy()
	{
		int i=0;
		JniGlue.DestroyFrame();

		//m_AudioMng.setStreamVolume(AudioManager.STREAM_MUSIC, m_AudioVol, AudioManager.FLAG_PLAY_SOUND);

		if(null != m_sensor)
		{
			for(i=0; i<m_snhw.length; ++i)
			{
				m_sensor.unregisterListener(m_snhw[i]);
				m_snhw[i] = null;
			}
			m_sensor = null;
		}

		// release the media player
		int size = m_vMpl.size();
		for(i=0; i<size; ++i)
		{
			MediaPlayer mp = m_vMpl.get(i);

			if(null == mp)
				continue;

			mp.release();
			mp = null;
		}
		m_vMpl.clear();

		// release the asset manager
		if(null != m_asst)
		{
			m_asst.close();
			m_asst = null;
		}

		m_bCreate = 0;
	}

	// Restore g-pack instance
	public static int Restore()
	{
		return RestoreFrame();
	}

	public static void Resume()
	{
		Log.w(TAG, "JniGlue::Resume");

		//if(0<=m_fd)
		//	JniCB_MpPlay(m_fd);

		if(null != m_sensor)
		{
			int gsn_type [] =
			{
				Sensor.TYPE_ACCELEROMETER		, //1
				Sensor.TYPE_MAGNETIC_FIELD		, //2
				Sensor.TYPE_ORIENTATION			, //3
				Sensor.TYPE_GYROSCOPE			, //4
				Sensor.TYPE_LIGHT				, //5
				Sensor.TYPE_PRESSURE			, //6
				Sensor.TYPE_TEMPERATURE			, //7
				Sensor.TYPE_PROXIMITY			, //8
				Sensor.TYPE_GRAVITY				, //9
				Sensor.TYPE_LINEAR_ACCELERATION	, //10
				Sensor.TYPE_ROTATION_VECTOR		, //11
			};

			for(int i=0; i<m_snhw.length; ++i)
			{
				m_sensor.registerListener(m_snhw[i]
										, m_sensor.getDefaultSensor(gsn_type[i])
										, SensorManager.SENSOR_DELAY_GAME);
			}
		}

		m_View.onResume();
	}

	public static void Pause()
	{
		Log.w(TAG, "JniGlue::Pause");

		//if(0<=m_fd)
		//	JniCB_MpPause(m_fd);

		if(null != m_sensor)
		{
			for(int i=0; i<m_snhw.length; ++i)
				m_sensor.unregisterListener(m_snhw[i]);
		}

		m_View.onPause();
	}

	// Stop g-pack instance
	public static int Stop()
	{
		return JniGlue.InvalidFrame();
	}

	// Run Procedure of g-pack instance
	public static int Proc()
	{
		return JniGlue.ProcFrame();
	}

	// touch event
	public static void TouchEvent(MotionEvent ev)
	{
		int action	= ev.getAction();
		int act		= action & MotionEvent.ACTION_MASK;

		if		(MotionEvent.ACTION_DOWN == act)
		{
			int id = 0x0;
			int x  = (int)ev.getX(0);
			int y  = (int)ev.getY(0);

			//String st = String.format("Touch::Down[0/1]:: %d %d", x, y);	Log.w(TAG, st);

			JniGlue.SetupTouch(id, LCIK_KEYDOWN, x, y);
		}

		else if(MotionEvent.ACTION_POINTER_DOWN == act)
		{
			int id = ev.getActionIndex();
			int x  = (int)ev.getX(id);
			int y  = (int)ev.getY(id);

			//int nCnt = ev.getPointerCount();
			//String st = String.format("Touch::PDown[%d/%d]:: %d %d", id, nCnt, x, y);	Log.w(TAG, st);

			JniGlue.SetupTouch( id, LCIK_KEYDOWN, x, y);
		}

		else if(MotionEvent.ACTION_MOVE == act)
		{
			int nCnt = ev.getPointerCount();

			for (int p = 0; p < nCnt; p++)
			{
				int id = ev.getPointerId(p);
				int ix = ev.findPointerIndex(id);
				if(-1== ix)
					continue;

				int x = (int)ev.getX(p);
				int y = (int)ev.getY(p);

				//String st = String.format("Touch::Move[%d %d %d]:: %d %d", p, id, ix, x, y);	Log.w(TAG, st);

				JniGlue.SetupTouch( id, LCIK_KEYPRESS, x, y);
			}
		}

		else if(MotionEvent.ACTION_POINTER_UP == act)
		{
			int id = ev.getActionIndex();
			int x = (int)ev.getX(id);
			int y = (int)ev.getY(id);

			//int nCnt = ev.getPointerCount();
			//String st = String.format("Touch::PUp[%d/%d]:: %d %d", id, nCnt, x, y);	Log.w(TAG, st);

			JniGlue.SetupTouch(id, LCIK_KEYUP, x, y);
		}

		else if(MotionEvent.ACTION_UP == act)
		{
			int id = 0x0;
			int x  = (int)ev.getX(0);
			int y  = (int)ev.getY(0);

			//String st = String.format("Touch::Up[0]:: %d %d", x, y);	Log.w(TAG, st);

			JniGlue.SetupTouch(id, LCIK_KEYUP, x, y);
		}
	}

	// Key Down
	public static boolean KeyDown(int key, KeyEvent ev)
	{
		//Log.i(TAG, "KeyDown: " + key);

		JniGlue.SetupKeypad(key, 1);

		if(key == KeyEvent.KEYCODE_BACK)
		{
			long curTime = System.currentTimeMillis();

			//Log.i(TAG, "Exit Count: " + m_exitCount);

			++m_exitCount;
			if(2>=m_exitCount)
				m_exitLast = curTime;


			if( curTime >(m_exitLast + m_exitDelta))
			{
				m_exitCount = 0;
				JniGlue.Destroy();
				System.exit(0);
				return true;
			}

			return true;
		}

		if(key == KeyEvent.KEYCODE_VOLUME_UP)
		{
			JniGlue.MasterVolumeUp();
			return true;
		}
		if(key == KeyEvent.KEYCODE_VOLUME_DOWN)
		{
			JniGlue.MasterVolumeDown();
			return true;
		}

		//if     (key == KeyEvent.KEYCODE_BACK  )	Log.w(TAG, "KeyDown BACK ----");
		//else if(key == KeyEvent.KEYCODE_HOME  )	Log.w(TAG, "KeyDown HOME   --");
		//else if(key == KeyEvent.KEYCODE_MENU  )	Log.w(TAG, "KeyDown MENU   --");
		//else if(key == KeyEvent.KEYCODE_SEARCH)	Log.w(TAG, "KeyDown SEARCH --");

		return false;
	}

	// Key up
	public static boolean KeyUp(int key, KeyEvent ev)
	{
		//Log.i(TAG, "KeyUp: " + key);

		JniGlue.SetupKeypad(key, 0);

		if(key == KeyEvent.KEYCODE_BACK)
		{
			m_exitCount = 0;
			return true;
		}

		return false;
	}

	public static boolean KeyLongPress(int key, KeyEvent ev)
	{
		//Log.i(TAG, "KeyLongPress: " + key);

		if(key == KeyEvent.KEYCODE_BACK)
			return true;

		return false;
	}

	////////////////////////////////////////////////////////////////////////////
	// public method

	public static void MasterVolumeUp()
	{
		int v	= m_AudioMng.getStreamVolume(AudioManager.STREAM_MUSIC);
		int m	= m_AudioMng.getStreamMaxVolume(AudioManager.STREAM_MUSIC);

		//Log.i(TAG, "Volume: " + v);

		v++;
		if(v <= m)
			m_AudioMng.setStreamVolume(AudioManager.STREAM_MUSIC
										, v
										, AudioManager.FLAG_PLAY_SOUND);

	}

	public static void MasterVolumeDown()
	{
		int v = m_AudioMng.getStreamVolume(AudioManager.STREAM_MUSIC);

		//Log.i(TAG, "Volume: " + v);

		v--;
		if( 0<= v)
			m_AudioMng.setStreamVolume(AudioManager.STREAM_MUSIC
										, v
										, AudioManager.FLAG_PLAY_SOUND);
	}


	////////////////////////////////////////////////////////////////////////////
	// callback from g-pack: haptic
	// haptic play

	public static int JniCB_HapticEnable()
	{
		//String st = String.format("Call Haptic Enable");	Log.w(TAG, st);

		if(null == m_haptic)
			return -1;

		return 0;
	}

	// haptic play
	public static void JniCB_HapticPlay(int millis)
	{
		//String st = String.format("Call Haptic Play::%d", mS);	Log.w(TAG, st);

		if(null == m_haptic)
			return;

		m_haptic.vibrate(millis);
	}

	// haptic play pattern
	public static void JniCB_HapticPlayPattern(long [] pattern, int rpt)
	{
		//String st = String.format("Call Haptic Pattern::%d", rpt);	Log.w(TAG, st);
		if(null == m_haptic)
			return;

		//long[] pattern = {500, 200, 1000, 2000};
		//m_haptic.vibrate(pattern, -1);
		m_haptic.vibrate(pattern, rpt);
	}

	// haptic stop
	public static void JniCB_HapticStop()
	{
		//String st = String.format("Call Haptic Stop");	Log.w(TAG, st);

		if(null == m_haptic)
			return;

		m_haptic.cancel();
	}


	////////////////////////////////////////////////////////////////////////////
	// callback from g-pack: media player

	// create the media player
	public static int JniCB_MpCreate(String srcFile)
	{
		AssetFileDescriptor afd = null;
		MediaPlayer         mp  = null;
		FileDescriptor      fd  = null;
		long                iOff= 0;
		long                iLen= 0;

		int idx		= 0;
		int size	= 0;

		try
		{
			afd = m_asst.openFd(srcFile);
			if(null == afd)
			{
				Log.e(TAG, "Mplayer::File not Exist::" + srcFile);
				return -1;
			}

			fd   = afd.getFileDescriptor();
			iOff = afd.getStartOffset();
			iLen = afd.getLength();

			mp = new MediaPlayer();
			mp.setDataSource(fd, iOff, iLen);
			mp.prepare();

			afd.close();
			afd = null;

			// find the null media player or index
			idx  = 0;
			size = m_vMpl.size();
			for(idx=0; idx<size; ++idx)
			{
				MediaPlayer tmp = m_vMpl.get(idx);
				if(null == tmp)
					break;
			}

			// add to the mp list
			if(idx == size)
				m_vMpl.add(mp);
			else
				m_vMpl.set(idx, mp);
		}
		catch(IOException e)
		{
			Log.e(TAG, "Mplayer::Create::File I/O Err::" + srcFile);
			return -2;
		}

		Log.w(TAG, "Mplayer::Create::Success:: " + idx  + " :: "+ srcFile);
		return idx;
	}

	// play
	public static int JniCB_MpPlay(int fd)
	{
		Log.w(TAG, "Mplayer::Play:: " + fd);

		MediaPlayer mp = null;
		int size = m_vMpl.size();

		if(0>fd || fd>= size)
			return -1;

		mp = m_vMpl.get(fd);
		if(null == mp)
			return -1;

		mp.start();
		return 0;
	}

	// stop
	public static int JniCB_MpStop(int fd)
	{
		//Log.w(TAG, "Mplayer::Stop:: " + fd);

		MediaPlayer mp = null;
		int size = m_vMpl.size();

		if(0>fd || fd>= size)
			return -1;

		mp = m_vMpl.get(fd);
		if(null == mp)
			return -1;

		if(mp.isPlaying())
		{
			mp.stop();

			try
			{
				mp.prepare();
				mp.seekTo(0);
			}
			catch(IOException e)
			{
				Log.e(TAG, "Mplayer::Stop::prepare Err");
			}
		}

		return 0;
	}

	// pause
	public static int JniCB_MpPause(int fd)
	{
		//Log.w(TAG, "Mplayer::Pause:: " + fd);

		MediaPlayer mp = null;
		int size = m_vMpl.size();

		if(0>fd || fd>= size)
			return -1;

		mp = m_vMpl.get(fd);
		if(null == mp)
			return -1;

		if(mp.isPlaying())
			mp.pause();

		return 0;
	}

	// reset
	public static int JniCB_MpReset(int fd)
	{
		//Log.w(TAG, "Mplayer::Reset:: " + fd);

		MediaPlayer mp = null;
		int size = m_vMpl.size();

		if(0>fd || fd>= size)
			return -1;

		mp = m_vMpl.get(fd);
		if(null == mp)
			return -1;

		mp = m_vMpl.get(fd);
		if(null == mp)
			return -1;


		if(mp.isPlaying())
		{
			mp.stop();

			try
			{
				mp.prepare();
				mp.seekTo(0);
			}
			catch(IOException e)
			{
				Log.e(TAG, "Mplayer::Reset::prepare Err");
			}
		}

		/*
		mp.reset();

		try
		{
			mp.prepare();
		}
		catch(IOException e)
		{
			Log.e(TAG, "Mplayer::reset Err");
			return -2;
		}
		*/
		return 0;
	}

	// destroy
	public static int JniCB_MpDestroy(int fd)
	{
		//Log.w(TAG, "Mplayer::Destroy:: " + fd);

		MediaPlayer mp = null;
		int size = m_vMpl.size();

		if(0>fd || fd>= size)
			return -1;

		mp = m_vMpl.get(fd);
		if(null == mp)
			return 0;

		mp.release();
		mp = null;
		m_vMpl.set(fd, null);

		return 0;
	}

	// query
	public static int JniCB_MpSetData(int fd, int opt, int val)
	{
		//Log.w(TAG, "Mplayer::SetData:: " + fd + " :: " + opt + " :: " +val);

		MediaPlayer mp = null;
		int size = m_vMpl.size();

		if(0>fd || fd>= size)
			return -1;

		mp = m_vMpl.get(fd);
		if(null == mp)
			return -1;


		if     (0x01 == opt)
		{
			boolean v = (0 == val)? false: true;
			mp.setLooping(v);
		}
		else if(0x02 == opt)
		{
			float vol = val/10000.0F;
			//Log.w(TAG, "Mplayer::SetVolume:: " + vol);

			mp.setVolume(vol, vol);
		}
		else
		{
			return -2;
		}

		return 0;
	}

	public static int JniCB_MpGetData(int fd, int opt)
	{
		//Log.w(TAG, "Mplayer::GetData:: " + fd + " :: " + opt);

		MediaPlayer mp = null;
		int size = m_vMpl.size();

		if(0>fd || fd>= size)
			return -1;

		mp = m_vMpl.get(fd);
		if(null == mp)
			return -1;


		if     (0x01 == opt)
		{
			boolean v = mp.isLooping();
			int val = (v)? 1: 0;
			return val;
		}
		else if(0x03 == opt)
		{
			boolean v = mp.isPlaying();
			int val = (v)? 1: 0;
			return val;
		}

		return -2;
	}

	////////////////////////////////////////////////////////////////////////////
	// Native Method
	public static native int CreateFrame();
	public static native int DestroyFrame();
	public static native int ProcFrame();
	public static native int InvalidFrame();
	public static native int RestoreFrame();

	public static native int SetupSys(String pck, String cls);					// calls the Lc_Init()
	public static native int SetupTouch(int n, int t, int x, int y);			// calls the LcStv_InputWriteMts()
	public static native int SetupKeypad(int key, int t);						// calls the LcStv_InputWriteKey()
	public static native int SetupSensor(int n, float x, float y, float z);		// calls the LcStv_SensorWrite()

	static	{	System.loadLibrary("g_pack_app");	}
}
