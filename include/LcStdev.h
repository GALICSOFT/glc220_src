//	Copyright(C) GaLic Soft. All rights reserved.
//
// Stdev is Standard (System) Device.
// the input(keypad, Multi-touch Screen), Sensor, haptic motor,
// sound and recording device are called the standard device.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcStdev_H_
#define _LcStdev_H_

#include <LcType.h>


////////////////////////////////////////////////////////////////////////////////
// GLC Input

enum ELC_INPUT
{
	LCIK_READ_WAIT		= 1,
	LCIK_READ_COMPLETE	= 0,

	// Type
	LCIK_NONE			= 0,

	// State
	LCIK_KEYNONE		= 0,
	LCIK_KEYDOWN		= 1,
	LCIK_KEYUP			= 2,
	LCIK_KEYPRESS		= 3,

	// Mouse for pc
	LCIK_LBUTTON		= 0,
	LCIK_RBUTTON		= 1,
	LCIK_MBUTTON		= 2,
};

enum EDirectInput
{
	DDIK_ESCAPE          = 0x01,
	DDIK_1               = 0x02,
	DDIK_2               = 0x03,
	DDIK_3               = 0x04,
	DDIK_4               = 0x05,
	DDIK_5               = 0x06,
	DDIK_6               = 0x07,
	DDIK_7               = 0x08,
	DDIK_8               = 0x09,
	DDIK_9               = 0x0A,
	DDIK_0               = 0x0B,
	DDIK_MINUS           = 0x0C,   /* - on main keyboard */
	DDIK_EQUALS          = 0x0D,
	DDIK_BACK            = 0x0E,   /* backspace */
	DDIK_TAB             = 0x0F,
	DDIK_Q               = 0x10,
	DDIK_W               = 0x11,
	DDIK_E               = 0x12,
	DDIK_R               = 0x13,
	DDIK_T               = 0x14,
	DDIK_Y               = 0x15,
	DDIK_U               = 0x16,
	DDIK_I               = 0x17,
	DDIK_O               = 0x18,
	DDIK_P               = 0x19,
	DDIK_LBRACKET        = 0x1A,
	DDIK_RBRACKET        = 0x1B,
	DDIK_RETURN          = 0x1C,    /* Enter on main keyboard */
	DDIK_LCONTROL        = 0x1D,
	DDIK_A               = 0x1E,
	DDIK_S               = 0x1F,
	DDIK_D               = 0x20,
	DDIK_F               = 0x21,
	DDIK_G               = 0x22,
	DDIK_H               = 0x23,
	DDIK_J               = 0x24,
	DDIK_K               = 0x25,
	DDIK_L               = 0x26,
	DDIK_SEMICOLON       = 0x27,
	DDIK_APOSTROPHE      = 0x28,
	DDIK_GRAVE           = 0x29,    /* accent grave */
	DDIK_LSHIFT          = 0x2A,
	DDIK_BACKSLASH       = 0x2B,
	DDIK_Z               = 0x2C,
	DDIK_X               = 0x2D,
	DDIK_C               = 0x2E,
	DDIK_V               = 0x2F,
	DDIK_B               = 0x30,
	DDIK_N               = 0x31,
	DDIK_M               = 0x32,
	DDIK_COMMA           = 0x33,
	DDIK_PERIOD          = 0x34,    /* . on main keyboard */
	DDIK_SLASH           = 0x35,    /* / on main keyboard */
	DDIK_RSHIFT          = 0x36,
	DDIK_MULTIPLY        = 0x37,    /* * on numeric keypad */
	DDIK_LMENU           = 0x38,    /* left Alt */
	DDIK_SPACE           = 0x39,
	DDIK_CAPITAL         = 0x3A,
	DDIK_F1              = 0x3B,
	DDIK_F2              = 0x3C,
	DDIK_F3              = 0x3D,
	DDIK_F4              = 0x3E,
	DDIK_F5              = 0x3F,
	DDIK_F6              = 0x40,
	DDIK_F7              = 0x41,
	DDIK_F8              = 0x42,
	DDIK_F9              = 0x43,
	DDIK_F10             = 0x44,
	DDIK_NUMLOCK         = 0x45,
	DDIK_SCROLL          = 0x46,    /* Scroll Lock */
	DDIK_NUMPAD7         = 0x47,
	DDIK_NUMPAD8         = 0x48,
	DDIK_NUMPAD9         = 0x49,
	DDIK_SUBTRACT        = 0x4A,    /* - on numeric keypad */
	DDIK_NUMPAD4         = 0x4B,
	DDIK_NUMPAD5         = 0x4C,
	DDIK_NUMPAD6         = 0x4D,
	DDIK_ADD             = 0x4E,    /* + on numeric keypad */
	DDIK_NUMPAD1         = 0x4F,
	DDIK_NUMPAD2         = 0x50,
	DDIK_NUMPAD3         = 0x51,
	DDIK_NUMPAD0         = 0x52,
	DDIK_DECIMAL         = 0x53,    /* . on numeric keypad */
	DDIK_OEM_102         = 0x56,    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	DDIK_F11             = 0x57,
	DDIK_F12             = 0x58,
	DDIK_F13             = 0x64,    /*                     (NEC PC98) */
	DDIK_F14             = 0x65,    /*                     (NEC PC98) */
	DDIK_F15             = 0x66,    /*                     (NEC PC98) */
	DDIK_KANA            = 0x70,    /* (Japanese keyboard)            */
	DDIK_ABNT_C1         = 0x73,    /* /? on Brazilian keyboard */
	DDIK_CONVERT         = 0x79,    /* (Japanese keyboard)            */
	DDIK_NOCONVERT       = 0x7B,    /* (Japanese keyboard)            */
	DDIK_YEN             = 0x7D,    /* (Japanese keyboard)            */
	DDIK_ABNT_C2         = 0x7E,    /* Numpad . on Brazilian keyboard */
	DDIK_NUMPADEQUALS    = 0x8D,    /* = on numeric keypad (NEC PC98) */
	DDIK_PREVTRACK       = 0x90,    /* Previous Track (DDIK_CIRCUMFLEX on Japanese keyboard) */
	DDIK_AT              = 0x91,    /*                     (NEC PC98) */
	DDIK_COLON           = 0x92,    /*                     (NEC PC98) */
	DDIK_UNDERLINE       = 0x93,    /*                     (NEC PC98) */
	DDIK_KANJI           = 0x94,    /* (Japanese keyboard)            */
	DDIK_STOP            = 0x95,    /*                     (NEC PC98) */
	DDIK_AX              = 0x96,    /*                     (Japan AX) */
	DDIK_UNLABELED       = 0x97,    /*                        (J3100) */
	DDIK_NEXTTRACK       = 0x99,    /* Next Track */
	DDIK_NUMPADENTER     = 0x9C,    /* Enter on numeric keypad */
	DDIK_RCONTROL        = 0x9D,
	DDIK_MUTE            = 0xA0,    /* Mute */
	DDIK_CALCULATOR      = 0xA1,    /* Calculator */
	DDIK_PLAYPAUSE       = 0xA2,    /* Play / Pause */
	DDIK_MEDIASTOP       = 0xA4,    /* Media Stop */
	DDIK_VOLUMEDOWN      = 0xAE,    /* Volume - */
	DDIK_VOLUMEUP        = 0xB0,    /* Volume + */
	DDIK_WEBHOME         = 0xB2,    /* Web home */
	DDIK_NUMPADCOMMA     = 0xB3,    /* , on numeric keypad (NEC PC98) */
	DDIK_DIVIDE          = 0xB5,    /* / on numeric keypad */
	DDIK_SYSRQ           = 0xB7,
	DDIK_RMENU           = 0xB8,    /* right Alt */
	DDIK_PAUSE           = 0xC5,    /* Pause */
	DDIK_HOME            = 0xC7,    /* Home on arrow keypad */
	DDIK_UP              = 0xC8,    /* UpArrow on arrow keypad */
	DDIK_PRIOR           = 0xC9,    /* PgUp on arrow keypad */
	DDIK_LEFT            = 0xCB,    /* LeftArrow on arrow keypad */
	DDIK_RIGHT           = 0xCD,    /* RightArrow on arrow keypad */
	DDIK_END             = 0xCF,    /* End on arrow keypad */
	DDIK_DOWN            = 0xD0,    /* DownArrow on arrow keypad */
	DDIK_NEXT            = 0xD1,    /* PgDn on arrow keypad */
	DDIK_INSERT          = 0xD2,    /* Insert on arrow keypad */
	DDIK_DELETE          = 0xD3,    /* Delete on arrow keypad */
	DDIK_LWIN            = 0xDB,    /* Left Windows key */
	DDIK_RWIN            = 0xDC,    /* Right Windows key */
	DDIK_APPS            = 0xDD,    /* AppMenu key */
	DDIK_POWER           = 0xDE,    /* System Power */
	DDIK_SLEEP           = 0xDF,    /* System Sleep */
	DDIK_WAKE            = 0xE3,    /* System Wake */
	DDIK_WEBSEARCH       = 0xE5,    /* Web Search */
	DDIK_WEBFAVORITES    = 0xE6,    /* Web Favorites */
	DDIK_WEBREFRESH      = 0xE7,    /* Web Refresh */
	DDIK_WEBSTOP         = 0xE8,    /* Web Stop */
	DDIK_WEBFORWARD      = 0xE9,    /* Web Forward */
	DDIK_WEBBACK         = 0xEA,    /* Web Back */
	DDIK_MYCOMPUTER      = 0xEB,    /* My Computer */
	DDIK_MAIL            = 0xEC,    /* Mail */
	DDIK_MEDIASELECT     = 0xED,    /* Media Select */
	
	DDIK_BACKSPACE       = DDIK_BACK    ,    /* backspace */
	DDIK_NUMPADSTAR      = DDIK_MULTIPLY,    /* * on numeric keypad */
	DDIK_LALT            = DDIK_LMENU   ,    /* left Alt */
	DDIK_CAPSLOCK        = DDIK_CAPITAL ,    /* CapsLock */
	DDIK_NUMPADMINUS     = DDIK_SUBTRACT,    /* - on numeric keypad */
	DDIK_NUMPADPLUS      = DDIK_ADD     ,    /* + on numeric keypad */
	DDIK_NUMPADPERIOD    = DDIK_DECIMAL ,    /* . on numeric keypad */
	DDIK_NUMPADSLASH     = DDIK_DIVIDE  ,    /* / on numeric keypad */
	DDIK_RALT            = DDIK_RMENU   ,    /* right Alt */
	DDIK_UPARROW         = DDIK_UP      ,    /* UpArrow on arrow keypad */
	DDIK_PGUP            = DDIK_PRIOR   ,    /* PgUp on arrow keypad */
	DDIK_LEFTARROW       = DDIK_LEFT    ,    /* LeftArrow on arrow keypad */
	DDIK_RIGHTARROW      = DDIK_RIGHT   ,    /* RightArrow on arrow keypad */
	DDIK_DOWNARROW       = DDIK_DOWN    ,    /* DownArrow on arrow keypad */
	DDIK_PGDN            = DDIK_NEXT    ,    /* PgDn on arrow keypad */
};


#if defined(USE_DIRECT_INPUT)

	#define LCIK_ESCAPE			DDIK_ESCAPE
	#define LCIK_1				DDIK_1
	#define LCIK_2				DDIK_2
	#define LCIK_3				DDIK_3
	#define LCIK_4				DDIK_4
	#define LCIK_5				DDIK_5
	#define LCIK_6				DDIK_6
	#define LCIK_7				DDIK_7
	#define LCIK_8				DDIK_8
	#define LCIK_9				DDIK_9
	#define LCIK_0				DDIK_0
	#define LCIK_MINUS			DDIK_MINUS
	#define LCIK_EQUALS			DDIK_EQUALS
	#define LCIK_BACK			DDIK_BACK
	#define LCIK_TAB			DDIK_TAB
	#define LCIK_Q				DDIK_Q
	#define LCIK_W				DDIK_W
	#define LCIK_E				DDIK_E
	#define LCIK_R				DDIK_R
	#define LCIK_T				DDIK_T
	#define LCIK_Y				DDIK_Y
	#define LCIK_U				DDIK_U
	#define LCIK_I				DDIK_I
	#define LCIK_O				DDIK_O
	#define LCIK_P				DDIK_P
	#define LCIK_LBRACKET		DDIK_LBRACKET
	#define LCIK_RBRACKET		DDIK_RBRACKET
	#define LCIK_RETURN			DDIK_RETURN
	#define LCIK_LCONTROL		DDIK_LCONTROL
	#define LCIK_A				DDIK_A
	#define LCIK_S				DDIK_S
	#define LCIK_D				DDIK_D
	#define LCIK_F				DDIK_F
	#define LCIK_G				DDIK_G
	#define LCIK_H				DDIK_H
	#define LCIK_J				DDIK_J
	#define LCIK_K				DDIK_K
	#define LCIK_L				DDIK_L
	#define LCIK_SEMICOLON		DDIK_SEMICOLON
	#define LCIK_APOSTROPHE		DDIK_APOSTROPHE
	#define LCIK_GRAVE			DDIK_GRAVE
	#define LCIK_LSHIFT			DDIK_LSHIFT
	#define LCIK_BACKSLASH		DDIK_BACKSLASH
	#define LCIK_Z				DDIK_Z
	#define LCIK_X				DDIK_X
	#define LCIK_C				DDIK_C
	#define LCIK_V				DDIK_V
	#define LCIK_B				DDIK_B
	#define LCIK_N				DDIK_N
	#define LCIK_M				DDIK_M
	#define LCIK_COMMA			DDIK_COMMA
	#define LCIK_PERIOD			DDIK_PERIOD
	#define LCIK_SLASH			DDIK_SLASH
	#define LCIK_RSHIFT			DDIK_RSHIFT
	#define LCIK_MULTIPLY		DDIK_MULTIPLY
	#define LCIK_LMENU			DDIK_LMENU
	#define LCIK_SPACE			DDIK_SPACE
	#define LCIK_CAPITAL		DDIK_CAPITAL
	#define LCIK_F1				DDIK_F1
	#define LCIK_F2				DDIK_F2
	#define LCIK_F3				DDIK_F3
	#define LCIK_F4				DDIK_F4
	#define LCIK_F5				DDIK_F5
	#define LCIK_F6				DDIK_F6
	#define LCIK_F7				DDIK_F7
	#define LCIK_F8				DDIK_F8
	#define LCIK_F9				DDIK_F9
	#define LCIK_F10			DDIK_F10
	#define LCIK_NUMLOCK		DDIK_NUMLOCK
	#define LCIK_SCROLL			DDIK_SCROLL
	#define LCIK_NUMPAD7		DDIK_NUMPAD7
	#define LCIK_NUMPAD8		DDIK_NUMPAD8
	#define LCIK_NUMPAD9		DDIK_NUMPAD9
	#define LCIK_SUBTRACT		DDIK_SUBTRACT
	#define LCIK_NUMPAD4		DDIK_NUMPAD4
	#define LCIK_NUMPAD5		DDIK_NUMPAD5
	#define LCIK_NUMPAD6		DDIK_NUMPAD6
	#define LCIK_ADD			DDIK_ADD
	#define LCIK_NUMPAD1		DDIK_NUMPAD1
	#define LCIK_NUMPAD2		DDIK_NUMPAD2
	#define LCIK_NUMPAD3		DDIK_NUMPAD3
	#define LCIK_NUMPAD0		DDIK_NUMPAD0
	#define LCIK_DECIMAL		DDIK_DECIMAL
	#define LCIK_OEM_102		DDIK_OEM_102
	#define LCIK_F11			DDIK_F11
	#define LCIK_F12			DDIK_F12
	#define LCIK_F13			DDIK_F13
	#define LCIK_F14			DDIK_F14
	#define LCIK_F15			DDIK_F15
	#define LCIK_KANA			DDIK_KANA
	#define LCIK_ABNT_C1		DDIK_ABNT_C1
	#define LCIK_CONVERT		DDIK_CONVERT
	#define LCIK_NOCONVERT		DDIK_NOCONVERT
	#define LCIK_YEN			DDIK_YEN
	#define LCIK_ABNT_C2		DDIK_ABNT_C2
	#define LCIK_NUMPADEQUALS	DDIK_NUMPADEQUALS
	#define LCIK_PREVTRACK		DDIK_PREVTRACK
	#define LCIK_AT				DDIK_AT
	#define LCIK_COLON			DDIK_COLON
	#define LCIK_UNDERLINE		DDIK_UNDERLINE
	#define LCIK_KANJI			DDIK_KANJI
	#define LCIK_STOP			DDIK_STOP
	#define LCIK_AX				DDIK_AX
	#define LCIK_UNLABELED		DDIK_UNLABELED
	#define LCIK_NEXTTRACK		DDIK_NEXTTRACK
	#define LCIK_NUMPADENTER	DDIK_NUMPADENTER
	#define LCIK_RCONTROL		DDIK_RCONTROL
	#define LCIK_MUTE			DDIK_MUTE
	#define LCIK_CALCULATOR		DDIK_CALCULATOR
	#define LCIK_PLAYPAUSE		DDIK_PLAYPAUSE
	#define LCIK_MEDIASTOP		DDIK_MEDIASTOP
	#define LCIK_VOLUMEDOWN		DDIK_VOLUMEDOWN
	#define LCIK_VOLUMEUP		DDIK_VOLUMEUP
	#define LCIK_WEBHOME		DDIK_WEBHOME
	#define LCIK_NUMPADCOMMA	DDIK_NUMPADCOMMA
	#define LCIK_DIVIDE			DDIK_DIVIDE
	#define LCIK_SYSRQ			DDIK_SYSRQ
	#define LCIK_RMENU			DDIK_RMENU
	#define LCIK_PAUSE			DDIK_PAUSE
	#define LCIK_HOME			DDIK_HOME
	#define LCIK_UP				DDIK_UP
	#define LCIK_PRIOR			DDIK_PRIOR
	#define LCIK_LEFT			DDIK_LEFT
	#define LCIK_RIGHT			DDIK_RIGHT
	#define LCIK_END			DDIK_END
	#define LCIK_DOWN			DDIK_DOWN
	#define LCIK_NEXT			DDIK_NEXT
	#define LCIK_INSERT			DDIK_INSERT
	#define LCIK_DELETE			DDIK_DELETE
	#define LCIK_LWIN			DDIK_LWIN
	#define LCIK_RWIN			DDIK_RWIN
	#define LCIK_APPS			DDIK_APPS
	#define LCIK_POWER			DDIK_POWER
	#define LCIK_SLEEP			DDIK_SLEEP
	#define LCIK_WAKE			DDIK_WAKE
	#define LCIK_WEBSEARCH		DDIK_WEBSEARCH
	#define LCIK_WEBFAVORITES	DDIK_WEBFAVORITES
	#define LCIK_WEBREFRESH		DDIK_WEBREFRESH
	#define LCIK_WEBSTOP		DDIK_WEBSTOP
	#define LCIK_WEBFORWARD		DDIK_WEBFORWARD
	#define LCIK_WEBBACK		DDIK_WEBBACK
	#define LCIK_MYCOMPUTER		DDIK_MYCOMPUTER
	#define LCIK_MAIL			DDIK_MAIL
	#define LCIK_MEDIASELECT	DDIK_MEDIASELECT

	#define LCIK_BACKSPACE		DDIK_BACKSPACE
	#define LCIK_NUMPADSTAR		DDIK_NUMPADSTAR
	#define LCIK_LALT			DDIK_LALT
	#define LCIK_CAPSLOCK		DDIK_CAPSLOCK
	#define LCIK_NUMPADMINUS	DDIK_NUMPADMINUS
	#define LCIK_NUMPADPLUS		DDIK_NUMPADPLUS
	#define LCIK_NUMPADPERIOD	DDIK_NUMPADPERIOD
	#define LCIK_NUMPADSLASH	DDIK_NUMPADSLASH
	#define LCIK_RALT			DDIK_RALT
	#define LCIK_UPARROW		DDIK_UPARROW
	#define LCIK_PGUP			DDIK_PGUP
	#define LCIK_LEFTARROW		DDIK_LEFTARROW
	#define LCIK_RIGHTARROW		DDIK_RIGHTARROW
	#define LCIK_DOWNARROW		DDIK_DOWNARROW
	#define LCIK_PGDN			DDIK_PGDN
	#define LCIK_CIRCUMFLEX		DDIK_CIRCUMFLEX

#else

// - on main keyboard
	#define WNVK_MINUS		0xBD
	#define WNVK_EQUALS		0xBB
	#define WNVK_BACKSLASH	0xDC

	#define WNVK_LBRACKET	0xDB
	#define WNVK_RBRACKET	0xDD

	#define WNVK_SEMICOLON	0xBA
	#define WNVK_APOSTROPHE	0xDE

	#define WNVK_COMMA		0xBC

	// . on main keyboard
	#define WNVK_PERIOD		0xBE
	#define WNVK_SLASH		0xBF

	#define WNVK_0			0x30
	#define WNVK_1			0x31
	#define WNVK_2			0x32
	#define WNVK_3			0x33
	#define WNVK_4			0x34
	#define WNVK_5			0x35
	#define WNVK_6			0x36
	#define WNVK_7			0x37
	#define WNVK_8			0x38
	#define WNVK_9			0x39
	#define WNVK_A			0x41
	#define WNVK_B			0x42
	#define WNVK_C			0x43
	#define WNVK_D			0x44
	#define WNVK_E			0x45
	#define WNVK_F			0x46
	#define WNVK_G			0x47
	#define WNVK_H			0x48
	#define WNVK_I			0x49
	#define WNVK_J			0x4A
	#define WNVK_K			0x4B
	#define WNVK_L			0x4C
	#define WNVK_M			0x4D
	#define WNVK_N			0x4E
	#define WNVK_O			0x4F
	#define WNVK_P			0x50
	#define WNVK_Q			0x51
	#define WNVK_R			0x52
	#define WNVK_S			0x53
	#define WNVK_T			0x54
	#define WNVK_U			0x55
	#define WNVK_V			0x56
	#define WNVK_W			0x57
	#define WNVK_X			0x58
	#define WNVK_Y			0x59
	#define WNVK_Z			0x5A

	#define WNVK_LBUTTON	0x01
	#define WNVK_RBUTTON	0x02
	#define WNVK_CANCEL		0x03
	#define WNVK_MBUTTON	0x04
	#define WNVK_BACK		0x08
	#define WNVK_TAB		0x09
	#define WNVK_CLEAR		0x0C
	#define WNVK_RETURN		0x0D
	#define WNVK_SHIFT		0x10
	#define WNVK_CONTROL	0x11
	#define WNVK_MENU		0x12
	#define WNVK_PAUSE		0x13
	#define WNVK_CAPITAL	0x14
	#define WNVK_ESCAPE		0x1B
	#define WNVK_SPACE		0x20
	#define WNVK_PRIOR		0x21
	#define WNVK_NEXT		0x22
	#define WNVK_END		0x23
	#define WNVK_HOME		0x24
	#define WNVK_LEFT		0x25
	#define WNVK_UP			0x26
	#define WNVK_RIGHT		0x27
	#define WNVK_DOWN		0x28
	#define WNVK_SELECT		0x29
	#define WNVK_EXECUTE	0x2B
	#define WNVK_SNAPSHOT	0x2C
	#define WNVK_INSERT		0x2D
	#define WNVK_DELETE		0x2E
	#define WNVK_HELP		0x2F
	#define WNVK_LWIN		0x5B
	#define WNVK_RWIN		0x5C
	#define WNVK_APPS		0x5D
	#define WNVK_NUMPAD0	0x60
	#define WNVK_NUMPAD1	0x61
	#define WNVK_NUMPAD2	0x62
	#define WNVK_NUMPAD3	0x63
	#define WNVK_NUMPAD4	0x64
	#define WNVK_NUMPAD5	0x65
	#define WNVK_NUMPAD6	0x66
	#define WNVK_NUMPAD7	0x67
	#define WNVK_NUMPAD8	0x68
	#define WNVK_NUMPAD9	0x69
	#define WNVK_MULTIPLY	0x6A
	#define WNVK_ADD		0x6B
	#define WNVK_SEPARATOR	0x6C
	#define WNVK_SUBTRACT	0x6D
	#define WNVK_DECIMAL	0x6E
	#define WNVK_DIVIDE		0x6F
	#define WNVK_F1			0x70
	#define WNVK_F2			0x71
	#define WNVK_F3			0x72
	#define WNVK_F4			0x73
	#define WNVK_F5			0x74
	#define WNVK_F6			0x75
	#define WNVK_F7			0x76
	#define WNVK_F8			0x77
	#define WNVK_F9			0x78
	#define WNVK_F10		0x79
	#define WNVK_F11		0x7A
	#define WNVK_F12		0x7B
	#define WNVK_F13		0x7C
	#define WNVK_F14		0x7D
	#define WNVK_F15		0x7E
	#define WNVK_F16		0x7F
	#define WNVK_F17		0x80
	#define WNVK_F18		0x81
	#define WNVK_F19		0x82
	#define WNVK_F20		0x83
	#define WNVK_F21		0x84
	#define WNVK_F22		0x85
	#define WNVK_F23		0x86
	#define WNVK_F24		0x87
	#define WNVK_NUMLOCK	0x90
	#define WNVK_SCROLL		0x91
	#define WNVK_LSHIFT		0xA0
	#define WNVK_RSHIFT		0xA1
	#define WNVK_LCONTROL	0xA2
	#define WNVK_RCONTROL	0xA3
	#define WNVK_LMENU		0xA4
	#define WNVK_RMENU		0xA5
	#define WNVK_PACKET		0xE7
	#define WNVK_ATTN		0xF6
	#define WNVK_CRSEL		0xF7
	#define WNVK_EXSEL		0xF8
	#define WNVK_EREOF		0xF9
	#define WNVK_PLAY		0xFA
	#define WNVK_ZOOM		0xFB
	#define WNVK_NONAME		0xFC
	#define WNVK_PA1		0xFD
	#define WNVK_OEM_CLEAR	0xFE

	#define WNVK_KANA		0x15
	#define WNVK_HANGEUL	0x15
	#define WNVK_HANGUL		0x15
	#define WNVK_JUNJA		0x17
	#define WNVK_FINAL		0x18
	#define WNVK_HANJA		0x19
	#define WNVK_KANJI		0x19

	#define WNVK_CONVERT	0x1C
	#define WNVK_NONCONVERT	0x1D
	#define WNVK_ACCEPT		0x1E
	#define WNVK_MODECHANGE	0x1F

	#define WNVK_SELECT		0x29
	#define WNVK_PRINT		0x2A
	#define WNVK_EXECUTE	0x2B
	#define WNVK_SNAPSHOT	0x2C
	#define WNVK_INSERT		0x2D
	#define WNVK_DELETE		0x2E
	#define WNVK_HELP		0x2F
	#define WNVK_PROCESSKEY	0xE5


	#define LCIK_LBUTTON		WNVK_LBUTTON
	#define LCIK_RBUTTON		WNVK_RBUTTON
	#define LCIK_CANCEL			WNVK_CANCEL
	#define LCIK_MBUTTON		WNVK_MBUTTON

	#define LCIK_BACK			WNVK_BACK
	#define LCIK_TAB			WNVK_TAB

	#define LCIK_CLEAR			WNVK_CLEAR
	#define LCIK_RETURN			WNVK_RETURN

	#define LCIK_SHIFT			WNVK_SHIFT
	#define LCIK_CONTROL		WNVK_CONTROL
	#define LCIK_MENU			WNVK_MENU
	#define LCIK_PAUSE			WNVK_PAUSE
	#define LCIK_CAPITAL		WNVK_CAPITAL

	#define LCIK_KANA			WNVK_KANA
	#define LCIK_HANGEUL		WNVK_HANGEUL
	#define LCIK_HANGUL			WNVK_HANGUL
	#define LCIK_JUNJA			WNVK_JUNJA
	#define LCIK_FINAL			WNVK_FINAL
	#define LCIK_HANJA			WNVK_HANJA
	#define LCIK_KANJI			WNVK_KANJI

	#define LCIK_ESCAPE			WNVK_ESCAPE

	#define LCIK_CONVERT		WNVK_CONVERT
	#define LCIK_NONCONVERT		WNVK_NONCONVERT
	#define LCIK_ACCEPT			WNVK_ACCEPT
	#define LCIK_MODECHANGE		WNVK_MODECHANGE

	#define LCIK_SPACE			WNVK_SPACE
	#define LCIK_PRIOR			WNVK_PRIOR
	#define LCIK_NEXT			WNVK_NEXT

	#define LCIK_PAGEUP			WNVK_PRIOR
	#define LCIK_PAGEDOWN		WNVK_NEXT
	#define LCIK_VOL_UP			WNVK_PRIOR
	#define LCIK_VOL_DOWN		WNVK_NEXT

	#define LCIK_END			WNVK_END
	#define LCIK_HOME			WNVK_HOME
	#define LCIK_LEFT			WNVK_LEFT
	#define LCIK_UP				WNVK_UP
	#define LCIK_RIGHT			WNVK_RIGHT
	#define LCIK_DOWN			WNVK_DOWN
	#define LCIK_SELECT			WNVK_SELECT
	#define LCIK_PRINT			WNVK_PRINT
	#define LCIK_EXECUTE		WNVK_EXECUTE
	#define LCIK_SNAPSHOT		WNVK_SNAPSHOT
	#define LCIK_INSERT			WNVK_INSERT
	#define LCIK_DELETE			WNVK_DELETE
	#define LCIK_HELP			WNVK_HELP

	#define LCIK_LWIN			WNVK_LWIN
	#define LCIK_RWIN			WNVK_RWIN
	#define LCIK_APPS			WNVK_APPS

	#define LCIK_NUMPAD0		WNVK_NUMPAD0
	#define LCIK_NUMPAD1		WNVK_NUMPAD1
	#define LCIK_NUMPAD2		WNVK_NUMPAD2
	#define LCIK_NUMPAD3		WNVK_NUMPAD3
	#define LCIK_NUMPAD4		WNVK_NUMPAD4
	#define LCIK_NUMPAD5		WNVK_NUMPAD5
	#define LCIK_NUMPAD6		WNVK_NUMPAD6
	#define LCIK_NUMPAD7		WNVK_NUMPAD7
	#define LCIK_NUMPAD8		WNVK_NUMPAD8
	#define LCIK_NUMPAD9		WNVK_NUMPAD9
	#define LCIK_MULTIPLY		WNVK_MULTIPLY
	#define LCIK_ADD			WNVK_ADD
	#define LCIK_SEPARATOR		WNVK_SEPARATOR
	#define LCIK_SUBTRACT		WNVK_SUBTRACT
	#define LCIK_DECIMAL		WNVK_DECIMAL
	#define LCIK_DIVIDE			WNVK_DIVIDE
	#define LCIK_F1				WNVK_F1
	#define LCIK_F2				WNVK_F2
	#define LCIK_F3				WNVK_F3
	#define LCIK_F4				WNVK_F4
	#define LCIK_F5				WNVK_F5
	#define LCIK_F6				WNVK_F6
	#define LCIK_F7				WNVK_F7
	#define LCIK_F8				WNVK_F8
	#define LCIK_F9				WNVK_F9
	#define LCIK_F10			WNVK_F10
	#define LCIK_F11			WNVK_F11
	#define LCIK_F12			WNVK_F12
	#define LCIK_F13			WNVK_F13
	#define LCIK_F14			WNVK_F14
	#define LCIK_F15			WNVK_F15
	#define LCIK_F16			WNVK_F16
	#define LCIK_F17			WNVK_F17
	#define LCIK_F18			WNVK_F18
	#define LCIK_F19			WNVK_F19
	#define LCIK_F20			WNVK_F20
	#define LCIK_F21			WNVK_F21
	#define LCIK_F22			WNVK_F22
	#define LCIK_F23			WNVK_F23
	#define LCIK_F24			WNVK_F24

	#define LCIK_NUMLOCK		WNVK_NUMLOCK
	#define LCIK_SCROLL			WNVK_SCROLL

	#define LCIK_LSHIFT			WNVK_LSHIFT
	#define LCIK_RSHIFT			WNVK_RSHIFT
	#define LCIK_LCONTROL		WNVK_LCONTROL
	#define LCIK_RCONTROL		WNVK_RCONTROL
	#define LCIK_LMENU			WNVK_LMENU
	#define LCIK_RMENU			WNVK_RMENU
	#define LCIK_LALT			WNVK_LMENU
	#define LCIK_RALT			WNVK_RMENU

	#define LCIK_PROCESSKEY		WNVK_PROCESSKEY

	#define LCIK_ATTN			WNVK_ATTN
	#define LCIK_CRSEL			WNVK_CRSEL
	#define LCIK_EXSEL			WNVK_EXSEL
	#define LCIK_EREOF			WNVK_EREOF
	#define LCIK_PLAY			WNVK_PLAY
	#define LCIK_ZOOM			WNVK_ZOOM
	#define LCIK_NONAME			WNVK_NONAME
	#define LCIK_PA1			WNVK_PA1
	#define LCIK_OEM_CLEAR		WNVK_OEM_CLEAR

	#define LCIK_MINUS			WNVK_MINUS
	#define LCIK_EQUALS			WNVK_EQUALS
	#define LCIK_BACKSLASH		WNVK_BACKSLASH
	#define LCIK_LBRACKET		WNVK_LBRACKET
	#define LCIK_RBRACKET		WNVK_RBRACKET
	#define LCIK_SEMICOLON		WNVK_SEMICOLON
	#define LCIK_APOSTROPHE		WNVK_APOSTROPHE
	#define LCIK_COMMA			WNVK_COMMA
	#define LCIK_PERIOD			WNVK_PERIOD
	#define LCIK_SLASH			WNVK_SLASH
	#define LCIK_SYSRQ			WNVK_SNAPSHOT
	#define LCIK_GRAVE			WNVK_SELECT

	#define LCIK_0				WNVK_0
	#define LCIK_1				WNVK_1
	#define LCIK_2				WNVK_2
	#define LCIK_3				WNVK_3
	#define LCIK_4				WNVK_4
	#define LCIK_5				WNVK_5
	#define LCIK_6				WNVK_6
	#define LCIK_7				WNVK_7
	#define LCIK_8				WNVK_8
	#define LCIK_9				WNVK_9

	#define LCIK_A				WNVK_A
	#define LCIK_B				WNVK_B
	#define LCIK_C				WNVK_C
	#define LCIK_D				WNVK_D
	#define LCIK_E				WNVK_E
	#define LCIK_F				WNVK_F
	#define LCIK_G				WNVK_G
	#define LCIK_H				WNVK_H
	#define LCIK_I				WNVK_I
	#define LCIK_J				WNVK_J
	#define LCIK_K				WNVK_K
	#define LCIK_L				WNVK_L
	#define LCIK_M				WNVK_M
	#define LCIK_N				WNVK_N
	#define LCIK_O				WNVK_O
	#define LCIK_P				WNVK_P
	#define LCIK_Q				WNVK_Q
	#define LCIK_R				WNVK_R
	#define LCIK_S				WNVK_S
	#define LCIK_T				WNVK_T
	#define LCIK_U				WNVK_U
	#define LCIK_V				WNVK_V
	#define LCIK_W				WNVK_W
	#define LCIK_X				WNVK_X
	#define LCIK_Y				WNVK_Y
	#define LCIK_Z				WNVK_Z

#endif




// Keypad Mapping
typedef unsigned char*	PKEYMAP;


// Multi-touch Screen
struct TLC_INPUT_TS
{
	INT	x,y;		// Multi-touch Screen x, y
	INT	t;			// Multi-touch state
};


INT  LcStv_InputOpen();															// Input device open.
INT  LcStv_InputClose();														// Input device release.

INT  LcStv_InputReadKey(PKEYMAP pkey);											// gain Keypad data(needs over 32 unsigned char: minimum 32byte)
INT  LcStv_InputWriteKey(INT key, INT v);										// write the keypad data. key is LCIK_ key..

INT  LcStv_InputReadMts(INT idx, TLC_INPUT_TS* ts);								// gain Multi-touch Screen coordinate and state. ex) TLC_INPUT_TS t; LcStv_InputReadMts([0,16], &t);
INT  LcStv_InputWriteMts(INT idx, TLC_INPUT_TS* ts=NULL);						// write the ts data. NULL sets to default. ex) TLC_INPUT_TS t...; LcStv_InputWriteMts([0,16], &t);
INT  LcStv_InputCountMts();														// read the active touch pointers




////////////////////////////////////////////////////////////////////////////////
// GLC Sensor

enum LC_SENSOR_TYPE
{
	LC_GSN_NONE					= 0 ,
	LC_GSN_ACCELEROMETER		= 1 ,
	LC_GSN_MAGNETIC_FIELD		= 2 ,
	LC_GSN_ORIENTATION			= 3 ,
	LC_GSN_GYROSCOPE			= 4 ,
	LC_GSN_LIGHT				= 5 ,
	LC_GSN_PRESSURE				= 6 ,
	LC_GSN_TEMPERATURE			= 7 ,
	LC_GSN_PROXIMITY			= 8 ,
	LC_GSN_GRAVITY				= 9 ,
	LC_GSN_LINEAR_ACCELERATION	= 10,
	LC_GSN_ROTATION_VECTOR		= 11,
};

// sensor data
struct TLC_SENSOR
{
	FLOAT x, y, z;			// value
	INT   t;				// type
};


INT  LcStv_SensorOpen();							// Init Device
INT  LcStv_SensorClose();							// Release Device
INT  LcStv_SensorWrite(INT n, TLC_SENSOR* v);		// Write data
INT  LcStv_SensorRead(INT n, TLC_SENSOR* v);			// Read data




////////////////////////////////////////////////////////////////////////////////
// GLC Haptic motor

INT  LcStv_HapticOpen();										// Init Vibration Motor Device
INT  LcStv_HapticClose();										// Release Device
INT  LcStv_HapticEnable();										// Motor Is Enable=LC_OK, Disable = LC_EFAIL
INT  LcStv_HapticPlay(INT millisec);							// One-Action Motor Play
INT  LcStv_HapticPattern(INT p[], INT n, INT r=LC_INFINITE);	// Motor Play pattern	n is {delay-play} count, r is repeat -1 infinite else once
INT  LcStv_HapticStop();										// Motor Stop




////////////////////////////////////////////////////////////////////////////////
// Mixer

enum ELC_MIXER_CHANNEL
{
	LC_MIXER_VOLUME		= 0x0,	// Speaker
	LC_MIXER_HEADPHONE	= 0x0,	// Speaker
	LC_MIXER_BASS		= 0x1,	// Bass
	LC_MIXER_TREBLE		= 0x2,	// Treble
	LC_MIXER_PCM		= 0x4,	// PCM
	LC_MIXER_LINE		= 0x6,	// Line
	LC_MIXER_MIC		= 0x7,	// Microphone
};


INT LcStv_MixerOpen();											// Mixer Device Init
INT LcStv_MixerClose();											// Mixer Device Release
INT	LcStv_MixerReadVolumeRange(INT* pMaxMin);					// need 8byte INT. Get Mixer Device Volume Range. Max, Min value range is [0 ,127]
INT	LcStv_MixerReadVolume (INT channel, INT* left, INT* right);	// Get the Mixer Device Volume
INT	LcStv_MixerWriteVolume(INT channel, INT  left, INT  right);	// Set the Mixer Device Volume


enum ELC_BATTERY
{
	// Battery state
	LC_BATTERY_HIGH = 1,
	LC_BATTERY_MID,
	LC_BATTERY_LOW,
	LC_BATTERY_EMPTY,
};

INT LcStv_BatteryRead();				// return the Battery Remain

INT LcStv_RecordingSound(char* pBuf);	// Recording: need minimum 1024 byte. return Recorded size


#endif //_LC_Stdev_H_

