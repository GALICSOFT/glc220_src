// Implementation of standard device.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#if defined(_WIN32)
  #include <windows.h>

#elif defined(_PC_LINUX_)
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdarg.h>
  #include <time.h>
  #include <sys/ioctl.h>
  #include <linux/soundcard.h>

  //	#include <fcntl.h>
  #define BATTERY_FNAME	"/dev/pollux_batt"
  #define MIXER_DEVICE	"/dev/mixer"

#elif defined(__BADA__)
  #include <FBaseSys.h>

#endif

#include <stdio.h>
#include <stdlib.h>

#include <LcType.h>

#include "LcAux.h"
#include "LcStdev.h"
#include "LcBase.h"


////////////////////////////////////////////////////////////////////////////////
// Recording
// need over 1024byte
// return recording data size
INT LcStv_RecordingSound(char* pBuf)
{
	LOGE("Not Implemented\n");
	return LC_NOT_IMPLEMENTED;
}



////////////////////////////////////////////////////////////////////////////////
// Battery
INT LcStv_BatteryRead()
{
	// Battery Status
	unsigned short uBattery = (unsigned short)LC_BATTERY_HIGH;

#if defined(_PC_LINUX_)

	INT nRead = 0;

	// Device Open
	INT dev_fd = open(BATTERY_FNAME, O_RDONLY);

	// Open Failed.
	if(0>dev_fd)
	{
		LOGE("Battery Information Open Failed\n");
		return 0;
	}


//	// Success
//	LOGI("Battery Information Open Success!\n");


	// Read Device Data.
	nRead = read(dev_fd, &uBattery, sizeof(unsigned short));

	// Device Close
	close(dev_fd);
#endif

	return uBattery;
}



////////////////////////////////////////////////////////////////////////////////
// Implementation of the Mixer.

#if defined(_WIN32)

class CLcStv_Mixer
{
protected:
	UINT		m_nMixer;
	HMIXER		m_hMixer;
	MIXERCAPS	m_mxcaps;

public:
	unsigned long	m_dMin;
	unsigned long	m_dMax;
	unsigned long	m_dCtlID;

public:
	CLcStv_Mixer();
	virtual ~CLcStv_Mixer();

	INT Open();
	INT Close();
	INT GetMasterVolumeControl();
	INT GetMasterVolumeValue(unsigned long* dwVal) const;
	INT SetMasterVolumeValue(unsigned long dwVal) const;
};

#endif




#if defined(_WIN32)
	static CLcStv_Mixer*		g_pDevMixer = NULL;
#elif defined(_PC_LINUX_)
	static LC_DEVICE		g_pDevMixer = -1;
#endif




// Master Volume 장치 사용 권한 얻기
INT LcStv_MixerOpen()
{
#if defined(_WIN32)
	if(g_pDevMixer)
		return LC_OK;


	g_pDevMixer = new CLcStv_Mixer;


	if(FAILED(g_pDevMixer->Open()))
	{
		delete g_pDevMixer;
		g_pDevMixer = NULL;
		return LC_INVALID_DEVICE;
	}

	// Test...
	unsigned long volume = 0;
	if(FAILED(g_pDevMixer->GetMasterVolumeValue(&volume)))
	{
		delete g_pDevMixer;
		g_pDevMixer = NULL;
		return LC_CANNT_ACQUIRE_DEVICE;
	}

#elif defined(_PC_LINUX_)

	if(LC_SUCCEEDED(g_pDevMixer))
		return LC_OK;

	if(0 != access(MIXER_DEVICE, W_OK))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE( "Mixer Access() Failed\n");
		return LC_CANNT_ACQUIRE_DEVICE;
	}


	g_pDevMixer = open(MIXER_DEVICE, O_RDWR | O_NDELAY);
	if(LC_FAILED(g_pDevMixer))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE( "Mixer Open() Failed\n");
		return LC_CANNT_ACQUIRE_DEVICE;
	}
#endif

	return LC_OK;
}


// Master Volume 장치 닫기
INT LcStv_MixerClose()
{
#if defined(_WIN32)

	if(g_pDevMixer)
	{
		delete g_pDevMixer;
		g_pDevMixer = NULL;
	}

#elif defined(_PC_LINUX_)
	if(LC_SUCCEEDED(g_pDevMixer))
	{
		close(g_pDevMixer);
		g_pDevMixer = -1;
	}

#endif


	return LC_OK;
}



// read the Maximum and minimum of Master Volume
INT	LcStv_MixerReadVolumeRange(INT* pMaxMin)
{
#if defined(_WIN32)
	pMaxMin[0] = g_pDevMixer->m_dMax/516;
	pMaxMin[1] = g_pDevMixer->m_dMin;
#else
	pMaxMin[0] = 127;
	pMaxMin[1] = 0;
#endif

	return LC_OK;
}



//Set the volume of Mixer device
INT	LcStv_MixerWriteVolume(INT _channel, INT left, INT right)
{
#if defined(_WIN32)
	unsigned long volume = 0;

	if(0>left)		left = 0;
	if(127<left)	left = 127;
	if(0>right)		right = 0;
	if(127<right)	right = 127;

	volume = max(left, right);
	volume *= 516;
	g_pDevMixer->SetMasterVolumeValue(volume);

#elif defined(_PC_LINUX_)
	INT hr= 0;
	unsigned long volume = 0;
	INT ch = 0;

	_channel |= 0xC0044D00;

	ch = MIXER_WRITE(_channel);

	if(0  >left)	left = 0;
	if(120<left)	left  = 120;

	if(0  >right)	right = 0;
	if(120<right)	right = 120;

	volume = 256 * right + left;

	hr= ioctl(g_pDevMixer, ch, &volume);
	if(LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE( "Write volume() Failed\n");
	}

#endif

	return	LC_OK;
}


INT	LcStv_MixerReadVolume(INT _channel, INT* left, INT* right)
{
	unsigned long volume = 0;

#if defined(_WIN32)
	if(FAILED(g_pDevMixer->GetMasterVolumeValue(&volume)))
		return -1;

	*left = volume;
	*right= volume;

#elif defined(_PC_LINUX_)
	_channel |= 0xC0044D00;

	INT ch = MIXER_READ(_channel);
	if(LC_FAILED(ioctl(g_pDevMixer, ch, &volume)))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE( "Read volume() Failed\n");
	}

	*left =  volume & 0x000000FF;
	*right= (volume>>8) & 0x000000FF;
#endif

	return	volume;
}



//////////////////////////////////////////////////////////////////////////////
// Window Mixer

#if defined(_WIN32)

CLcStv_Mixer::CLcStv_Mixer()
{
	m_nMixer	= 0;
	m_hMixer	= NULL;
	m_dMin		= 0;
	m_dMax		= 0;
	m_dCtlID	= 0;
	memset(&m_mxcaps, 0, sizeof(MIXERCAPS));
}


CLcStv_Mixer::~CLcStv_Mixer()
{
	Close();
}


INT CLcStv_Mixer::Open()
{
	MMRESULT hr = 0;

	if(m_nMixer && m_hMixer && m_dCtlID)
		return 0;


	m_nMixer = ::mixerGetNumDevs();

	if (m_nMixer != 0)
	{
		hr = ::mixerOpen(&m_hMixer, 0, NULL, NULL, MIXER_OBJECTF_MIXER | CALLBACK_WINDOW);
		if (MMSYSERR_NOERROR != hr)
			return -1;

		hr = ::mixerGetDevCaps(reinterpret_cast<UINT>(m_hMixer), &m_mxcaps, sizeof(MIXERCAPS));
		if (MMSYSERR_NOERROR != hr)
			return -1;
	}


	return GetMasterVolumeControl();
}


INT CLcStv_Mixer::Close()
{
	if (!m_hMixer)
		return 0;

	MMRESULT hr = 0;
	hr = ::mixerClose(m_hMixer);

	m_hMixer = NULL;

	if(MMSYSERR_NOERROR == hr)
		return -1;

	return 0;
}

INT CLcStv_Mixer::GetMasterVolumeControl()
{
	MMRESULT hr=0;

	if(!m_hMixer)
		return -1;


	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	hr = ::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer),
						   &mxl,
						   MIXER_OBJECTF_HMIXER |
						   MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (MMSYSERR_NOERROR != hr)
		return -1;

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;

	hr = ::mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_hMixer),
							   &mxlc,
							   MIXER_OBJECTF_HMIXER |
							   MIXER_GETLINECONTROLSF_ONEBYTYPE);
	if (MMSYSERR_NOERROR != hr)
		return -1;

	// store dwControlID
	m_dMin = mxc.Bounds.dwMinimum;
	m_dMax = mxc.Bounds.dwMaximum;
	m_dCtlID = mxc.dwControlID;

	return 0;
}

INT CLcStv_Mixer::GetMasterVolumeValue(unsigned long* dwVal) const
{
	MMRESULT hr=0;

	if(!m_hMixer)
		return -1;


	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_dCtlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;

	hr = ::mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
								 &mxcd,
								 MIXER_OBJECTF_HMIXER |
								 MIXER_GETCONTROLDETAILSF_VALUE);
	if (MMSYSERR_NOERROR != hr)
		return -1;

	*dwVal = mxcdVolume.dwValue;

	return 0;
}

INT CLcStv_Mixer::SetMasterVolumeValue(unsigned long dwVal) const
{
	MMRESULT hr=0;

	if(!m_hMixer)
		return -1;

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { dwVal };
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_dCtlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;

	hr = ::mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
								 &mxcd,
								 MIXER_OBJECTF_HMIXER |
								 MIXER_SETCONTROLDETAILSF_VALUE);
	if (MMSYSERR_NOERROR != hr)
		return -1;

	return 0;
}

#endif

