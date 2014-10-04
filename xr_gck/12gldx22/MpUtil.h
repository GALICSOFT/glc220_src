// Interface for the Media Player Utilities.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MpUtil_H_
#define _MpUtil_H_

//typedef struct IDirectMusicLoader8			*LPDIRECTMUSICLOADER		;
//typedef struct IDirectMusicAudioPath			*LPDIRECTMUSICAUDIOPATH		;
//typedef struct IDirectMusicPerformance8		*LPDIRECTMUSICPERFORMANCE	;
//typedef struct IDirectMusicSegment8			*LPDIRECTMUSICSEGMENT		;


#define MEDIA_PLAY(p)				{	if(p){	(p)->Play() ;		}	}
#define MEDIA_STOP(p)				{	if(p){	(p)->Stop() ;		}	}
#define MEDIA_REST(p)				{	if(p){	(p)->Reset();		}	}
#define MEDIA_PAUSE(p)				{	if(p){	(p)->Pause();		}	}
#define MEDIA_REPEAT_INFINITE(p)	{	if(p){	(p)->SetRepeat();	}	}


//#define DSBVOLUME_MIN					-10000
//#define DSBVOLUME_MAX					0


#define MEDIA_MAX_VOLUME				10000L
#define MEDIA_MUSIC_MAX_VOLUME			9600L

#define MEDIA_STATE_STOP				0X00000000L
#define MEDIA_STATE_PLAY				0X00000001L
#define MEDIA_STATE_PAUSE				0X00000002L
#define MEDIA_STATE_RESET				0X00000004L


//typedef LPDIRECTMUSICLOADER			LPDML;
//typedef LPDIRECTMUSICAUDIOPATH		LPDMA;
//typedef LPDIRECTMUSICPERFORMANCE		LPDMP;
//typedef LPDIRECTMUSICSEGMENT			LPDMS;
//
//typedef LPDIRECTSOUNDBUFFER			LPDSB;
//typedef LPDIRECTSOUND3DBUFFER			LPDS3D;




enum ELnMedia
{
//	LNMDAFMT_UNKNOWN	= 0xFFFFFFFF,	// 0000 0000 0000	: Loader	Player
//		
//	LNMDAFMT_WAV_		= 0x0008,	// 0000 0000 1000	: Wave
//	LNMDAFMT_WAV_M		= 0x0009,	// 0000 0000 1001	: Wave		Mono and Stero
//	LNMDAFMT_WAV_S		= 0x000C,	// 0000 0000 1100	: 3d		Sound
//
//	LNMDAFMT_MSC_		= 0x0080,	// 0000 1000 0000	: Music
//	LNMDAFMT_MW_M		= 0x0081,	// 0000 1000 0001	: Music		Wave
//	LNMDAFMT_MW_S		= 0x0084,	// 0000 1000 0100	: Music		Wave 3D
//
//	LNMDAFMT_MSC_M		= 0x0090,	// 0000 1001 0000	: Music		Mono and Stero
//	LNMDAFMT_MSC_S		= 0x00C0,	// 0000 1100 0000	: Music		3D
//	
//	LNMDAFMT_SHW		= 0x0800,	// 1000 0000 0000	: DShow
//	LNMDAFMT_SHW_M		= 0x0900,	// 1001 0000 0000	: DShow		Mp3
//	LNMDAFMT_SHW_A		= 0x0A00,	// 1010 0000 0000	: DShow		AVI
//
//	LPMDAFMT_WAV_		= 0xF008,	// 0000 0000 1000	: PDA Wave
//	LPMDAFMT_WAV_M		= 0xF009,	// 0000 0000 1001	: PDA Wave		Mono and Stero
//	LPMDAFMT_WAV_S		= 0xF00C,	// 0000 0000 1100	: PDA 3d		Sound

	LC_SMD_UNKNOWN	= 0x0000,	// 0000 0000 0000	: Loader	Player
		
	LC_SMD_WV_D		= 0x0008,	// 0000 0000 1000	: Wave		Default
	LC_SMD_WV_M		= 0x0009,	// 0000 0000 1001	: Wave		Mono and Stero
	LC_SMD_WV_S		= 0x000C,	// 0000 0000 1100	: 3d		Sound

	LC_SMD_MW_D		= 0x0080,	// 0000 1000 0000	: Music		Default
	LC_SMD_MW_M		= 0x0081,	// 0000 1000 0001	: Music		Wave
	LC_SMD_MW_S		= 0x0084,	// 0000 1000 0100	: Music		Wave 3D

	LC_SMD_MC_M		= 0x0090,	// 0000 1001 0000	: Music		Mono and Stero
	LC_SMD_MC_S		= 0x00C0,	// 0000 1100 0000	: Music		3D
	
	LC_SMD_SH_D		= 0x0800,	// 1000 0000 0000	: DShow		Default
	LC_SMD_SH_M		= 0x0900,	// 1001 0000 0000	: DShow		Mp3
	LC_SMD_SH_A		= 0x0A00,	// 1010 0000 0000	: DShow		AVI
};


#endif

