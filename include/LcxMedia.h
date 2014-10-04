//	Copyright(C) GaLic Soft. All rights reserved.
//	GaLic Game Library Collection
//
// Interface for the Media class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxMedia_H_
#define _LcxMedia_H_

#include <LcType.h>


////////////////////////////////////////////////////////////////////////////////
// Interface for the ILCX_Input class.

DCL_INTERFACE ILCX_Input : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILCX_Input	);


	virtual INT			Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT			Release()=0;


	virtual INT			Update()=0;												// Input Update
	virtual void		Reset()=0;												// reset(clear) the input data
	virtual void		SetActive(INT bActive=LC_TRUE)=0;						// return input values only active state.
	virtual INT			GetActive()=0;


	virtual const BYTE* const GetKeyMap()const=0;								// Key+Button Map
	virtual const BYTE* const GetMtsMap()const=0;								// Multi-touch Screen Map

	virtual LC_STATE	KeyDown(INT nKey)=0;
	virtual LC_STATE	KeyUp(INT nKey)=0;
	virtual LC_STATE	KeyPress(INT nKey)=0;
	virtual LC_STATE	KeyState(INT nKey)=0;

	virtual LC_STATE	MtsDown(INT nMultiTouch=0)=0;							// Multi-touch Screen down
	virtual LC_STATE	MtsUp(INT nMultiTouch=0)=0;								// Multi-touch Screen up
	virtual LC_STATE	MtsPress(INT nMultiTouch=0)=0;							// Multi-touch Screen press
	virtual LC_STATE	MtsState(INT nMultiTouch=0)=0;							// Multi-touch Screen state

	virtual const FLOAT* const GetMtsPos(int nMultiTouch=0)const=0;				// Multi-touch Screen Position Need FLOAT x 3
	virtual const FLOAT* const GetMtsGrd(int nMultiTouch=0)const=0;				// Gradient of Multi-touch Screen Need FLOAT x 3
	virtual LC_STATE	IsMtsMove()=0;

	virtual void		SetCurKey(BYTE* pKey, INT iLen = LC_MAX_KEY)=0;
	virtual void		SetCurTsb(BYTE* pTsb, INT iLen = LC_MAX_TSB)=0;
	virtual void		SetTsbPos(FLOAT* vcPos, int nMultiTouch=0)=0;

	virtual LC_STATE	IsInRect(INT left, INT top, INT right, INT bottom
								, INT nMultiTouch =0)=0;						// return  TRUE or FALSE.

	virtual LC_STATE	IsInRect2(INT left, INT top, INT right, INT bottom)=0;	// return  TRUE or FALSE
};

typedef ILCX_Input*	PLCX_INPUT;

// For Keypad, Multi-touch Screen controller
// Create Input device. So, you have not to call LcStv_InputOpen..()
INT LcxEnt_CreateInput(char* sCmd
				   , PLCX_INPUT* pOut					// Output instance
				   , LC_HANDLE p1						// Window Handle
				   , LC_HANDLE p2 = NULL				// No Use
				   , LC_HANDLE p3 = NULL				// No Use
				   , LC_HANDLE p4 = NULL				// No Use
					 );




////////////////////////////////////////////////////////////////////////////////
// Interface for the ILCX_Sound class.

enum ELCX_AV
{
	LCX_AV_PLAY		= 0x03,								// Play
	LCX_AV_STOP		= 0x00,								// Stop
	LCX_AV_RESET	= 0x01,								// Reset
	LCX_AV_PAUSE	= 0x02,								// Loop
	LCX_AV_INFINITE	= LC_INFINITE,
	LCX_AV_PCM		= 0x00001001,						// Format
};


struct TLCX_SOUND_INF									// Sound Information
{								
	UINT	Format;										// Audio Format. It's not use. This sdk only supports PCM
	UINT	SndB;										// Audio Bit Rate: => 8 or 16. This sdk only supports 16bit sound
	UINT	SndC;										// Channels: Mono =>1, stereo =>2
	UINT	SndHz;										// Sampling Rate: ex) 8000 kHz => 8, 22050 kHz => 22050
	BYTE*	Buff;										// Audio Buffer
	UINT	Size;										// Buffer Size
};



DCL_INTERFACE ILCX_Sound : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILCX_Sound	);


	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;


	virtual INT		Play()=0;
	virtual INT		Stop()=0;
	virtual INT		Reset()=0;
	virtual INT		Pause()=0;
	virtual void	SetVolume(FLOAT Vol)=0;
	virtual FLOAT	GetVolume()=0;
	virtual INT		SetRepeat(INT repeat=LCX_AV_INFINITE)=0;
	virtual INT		GetRepeat()=0;
	virtual INT		GetStatus()=0;

	virtual UINT	GetType()= 0;	// return the ELCX_AV type
};

typedef ILCX_Sound*	PLCX_SOUND;

// for iPhone, Linux, and Windows
INT LcxSmd_ALCDeviceOpen();								// Open the OpenAL sound device.
INT	LcxSmd_ALCDeviceClose();							// close the OpenAL sound device.

// for android system.
INT LcxSmd_AndDeviceOpen();								// Open the OpenSL Engine.
INT	LcxSmd_AndDeviceClose();							// close the OpenSL Engine.

// OpenAL Interface
INT LcxSmd_CreateALSndFromFile(char* sCmd				// Create Sound Instance
								, PLCX_SOUND* pOut		// Output instance
								, LC_HANDLE p1			// Sound File Name
								, LC_HANDLE p2 = NULL	// User Data(Option)
								, LC_HANDLE p3 = NULL	// No Use
								, LC_HANDLE p4 = NULL	// No Use
								);

INT LcxSmd_CreateALSndFromMemory(char* sCmd				// Create Sound Instance
								, PLCX_SOUND* pOut		// Output instance
								, LC_HANDLE p1			// struct TLCX_SOUND_INF Address
								, LC_HANDLE p2			// Memory Size Address(unsigned int*)
								, LC_HANDLE p3			// Resource Type. LC_OBJ_XXX, unsigned int
								, LC_HANDLE p4 = NULL	// No Use
								);

INT LcxSmd_CreateALSndFromBuffer(char* sCmd				// Create Sound Instance
								, PLCX_SOUND* pOut		// Output instance
								, LC_HANDLE p1			// struct TLCX_SOUND_INF Address
								, LC_HANDLE p2 = NULL	// No Use
								, LC_HANDLE p3 = NULL	// No Use
								, LC_HANDLE p4 = NULL	// No Use
								);

// OpenSL/MediaPlayer for Android Interface
INT LcxSmd_CreateSLSndFromFile(char* sCmd				// Create Sound Instance
								, PLCX_SOUND* pOut		// Output instance
								, LC_HANDLE p1			// Sound File Name
								, LC_HANDLE p2 = NULL	// User Data(Option)
								, LC_HANDLE p3 = NULL	// No Use
								, LC_HANDLE p4 = NULL	// No Use
								);

INT LcxSmd_CreateSLSndFromMemory(char* sCmd				// Create Sound Instance
								, PLCX_SOUND* pOut		// Output instance
								, LC_HANDLE p1			// Memory Address
								, LC_HANDLE p2			// Memory Size Address(unsigned int*)
								, LC_HANDLE p3			// Resource Type. LC_OBJ_XXX, unsigned int
								, LC_HANDLE p4 = NULL	// No Use
								);

INT LcxSmd_CreateSLSndFromBuffer(char* sCmd				// Create Sound Instance
								, PLCX_SOUND* pOut		// Output instance
								, LC_HANDLE p1			// struct TLCX_SOUND_INF Address
								, LC_HANDLE p2 = NULL	// No Use
								, LC_HANDLE p3 = NULL	// No Use
								, LC_HANDLE p4 = NULL	// No Use
								);


#if defined(__ANDROID__)
#define LcxSmd_DeviceOpen            LcxSmd_AndDeviceOpen
#define LcxSmd_DeviceClose           LcxSmd_AndDeviceClose
#define LcxSmd_CreateSoundFromFile   LcxSmd_CreateSLSndFromFile
#define LcxSmd_CreateSoundFromMemory LcxSmd_CreateSLSndFromMemory
#define LcxSmd_CreateSoundFromBuffer LcxSmd_CreateSLSndFromBuffer

#else
#define LcxSmd_DeviceOpen            LcxSmd_ALCDeviceOpen
#define LcxSmd_DeviceClose           LcxSmd_ALCDeviceClose
#define LcxSmd_CreateSoundFromFile   LcxSmd_CreateALSndFromFile
#define LcxSmd_CreateSoundFromMemory LcxSmd_CreateALSndFromMemory
#define LcxSmd_CreateSoundFromBuffer LcxSmd_CreateALSndFromBuffer

#endif




////////////////////////////////////////////////////////////////////////////////
// Interface for the ILCX_Show class.

enum ELCX_SHOW
{
	// STATE
	LCX_AV_SHOW	= 0x03,
	LCX_AV_HIDE	= 0x00,
};


DCL_INTERFACE ILCX_Show : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILCX_Show	);


	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual void	Play()=0;
	virtual void	Stop()=0;
	virtual void	Reset()=0;
	virtual void	Pause()=0;
	virtual void	SetVolume(FLOAT fVol)=0;
	virtual FLOAT	GetVolume()=0;
	virtual void	SetRepeat(INT dRepeat= LCX_AV_INFINITE)=0;
	virtual INT		GetRepeat()=0;
	virtual INT		GetStatus()=0;
};

typedef ILCX_Show*	PLCX_SHOW;

INT LcxEnt_CreateShow(char*     sCmd			// Create Show Instance
				   , PLCX_SHOW* pOut			// Output instance
				   , LC_HANDLE  p1				// Show File Name
				   , LC_HANDLE  p2 = NULL		// User Data(Option)
				   , LC_HANDLE  p3 = NULL		// No Use
				   , LC_HANDLE  p4 = NULL		// No Use
					 );


#endif

