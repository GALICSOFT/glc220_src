// Copyright(C) GALICSOFT. All rights reserved.
// GALICSOFT Game Library Collection.
// Type definition for Applications.
//
// platform identity directive
//   windows : _MSC_VER, _PC_WIN32_
//   linux   : _PC_LINUX_
//   iphone  : _IPHONE_
//   android : __ANDROID__
//   bada    : _BADA__
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif

#ifndef _LcType_H_
#define _LcType_H_

// basic type
typedef	int                    INT;
typedef	signed int             INT32;
typedef	unsigned int           UINT32;
typedef	signed char            INT8;
typedef	unsigned char          UINT8;
typedef	signed short int       INT16;
typedef	unsigned short int     UINT16;
typedef	short int              SHORT;
typedef	unsigned short int     USHORT;

#if !defined(__APPLE__) && !defined(__MACOSX__) && !defined(_IPHONE_)
  typedef int                  BOOL;
#endif

typedef	long int               LONG;
typedef	long int               HRESULT;
typedef float                  FLOAT;
typedef double                 DOUBLE;
typedef	unsigned char          BYTE;
typedef	signed char            SCHAR;
typedef	unsigned short         WORD;
typedef unsigned short         MCHAR;							// wide or Multi byte character
typedef	unsigned int           UINT;
typedef	unsigned long int      ULONG;
typedef unsigned int           SIZE_T_;							// size_t

// double word
#if defined(_MSC_VER)
  typedef unsigned long        DWORD;
#else
  typedef unsigned int         DWORD;
#endif

// 64bit int (double int)
#if defined(__GNUC__)
  typedef long long            DINT ;
  typedef unsigned long long   UDINT;
#else
  typedef __int64              DINT ;
  typedef unsigned __int64     UDINT;
#endif

typedef DINT                   INT64;							// 64bit int
typedef UDINT                  UINT64;							// 64bit unsigned int
typedef DINT                   LLONG;							// 64bit long
typedef	UDINT                  QWORD;							// q-word
typedef LLONG                  L_PTR;							// 64bit long pointer
typedef LLONG*                 PL_PTR;
typedef void*                  LPVOID;							// void pointer

typedef BYTE                   COLOR8;							//  8bit color
typedef WORD                   COLOR16;							// 16bit color
typedef UINT                   COLOR32;							// 32bit color
typedef UDINT                  COLOR64;							// 64bit color
typedef int                    LC_FIXED;						// fixed float for mobile solution.
typedef	void*                  LC_HANDLE;						// for mobile device
typedef unsigned int           LC_SOCKET;						// socket descriptor
typedef	unsigned int           LC_STATE;
typedef	int                    LC_DEVICE;						// device handle
typedef unsigned int           LC_VB;							// VBO(Vertex Buffer Object)
typedef unsigned int           LC_IB;							// IBO(Index Buffer Object == VBO)

// for Portability of Microsoft Windows Application
typedef UINT                   WPARAM;
typedef LONG                   LPARAM;
typedef LONG                   LRESULT;

// for OpenGL shader
typedef UINT                   GL_SHADER;
typedef UINT                   GL_PROGRAM;

// Handle for Microsoft windows and X-window system
#if !defined(_MSC_VER)
  struct HWND__ { int unused; };
#endif

#if defined(_MSC_VER)
   typedef struct HWND__      *LC_HWND;

#elif defined(_PC_LINUX_)

  typedef struct tagHNWD
  {
	  LC_HANDLE w, d, n, t;						// window, display, info, temp
  } *LC_HWND;

#else
  typedef void* LC_HWND;

#endif


// define callback
#if defined(_MSC_VER)
  #ifndef LC_CALLBACK
    #define LC_CALLBACK        __stdcall
  #endif

  #ifndef LC_WINAPI
    #define LC_WINAPI          __stdcall
  #endif

#else
  #ifndef LC_CALLBACK
    #define LC_CALLBACK
  #endif

  #ifndef LC_WINAPI
    #define LC_WINAPI
  #endif

#endif

// const and null
#ifndef CONST
  #define CONST   const
#endif

#ifndef NULL
  #define NULL    0
#endif

#ifndef L_NIL
  #define L_NIL	(LC_HANDLE)0
#endif

// GLC basic constant
enum ELC_CONSTANT
{
	LC_SDK_VER               =  0x00022020,						// GLC Library Version: 02.22.20
	LC_SDK_DATE              =  0x20121201,						// Last Build Date 2012-12-01

	// common constant
	LC_BADBEEF               =  0x0BadBeef,
	LC_BADFOOD               =  0x0BadF00d,
	LC_TRUE                  =  1         ,
	LC_FALSE                 =  0         ,

	// file pointer offset
	LC_FILE_BEGIN            =  0x00000000,						// SEEK_SET,
	LC_FILE_CURRENT          =  0x00000001,						// SEEK_CUR,
	LC_FILE_END              =  0x00000002,						// SEEK_END,

	// file return
	LC_FILE_ALREADY_OPEN     =  0x000000F1,
	LC_FILE_CANNT_ACCESS     =  0xFFFFFFF1,
	LC_FILE_NOT_OPENED       =  0xFFFFFFF2,
	LC_FILE_GET_LENGTH_ERR   =  0xFFFFFFF3,

	// input/output value(or type)
	LC_UNKNOWN               =  0x00000000,
	LC_DEFAULT               =  ((UINT)-1),
	LC_INFINITE              =  LC_DEFAULT,

	// dictory and file name length
	LC_MAX_PATH              =  240       ,
	LC_MAX_PATH2             =  320       ,
	LC_MAX_DRIVE             =   16       ,
	LC_MAX_DIR               =  240       ,
	LC_MAX_FNAME             =  240       ,
	LC_MAX_EXT               =   16       ,

	// default name
	LC_DEFAULT_NAME          =  160       ,						// Max Name Length

	// Return Value
	LC_OK                    =  0x00000000,						// Succeeded default value
	LC_EFAIL                 = -1         ,						// Failed default value

	// error
	LC_INVALID_CALL          =  0xFFFF0001,
	LC_CANNT_ALLOC           =  0xFFFF0002,
	LC_CANNT_FIND_RESOURCE   =  0xFFFF0003,
	LC_NOT_IMPLEMENTED       =  0xFFFF0004,
	LC_CANNT_ACQUIRE_DEVICE  =  0xFFFF0005,
	LC_INVALID_RESOURCE      =  0xFFFF0006,
	LC_INVALID_DEVICE        = -1         ,

	// for resource
	LC_RSC_GPACK             =  0x4B435047,						// G-pack Resource ID = "KCPG" <-GPCK
	LC_RSC_VER               =  0x00020000,						// Resource version
	LC_RSC_HEADER            =  64,								// header offset

	// object, resource data type
	LC_OBJ_UNKNOWN           =  LC_UNKNOWN,						// UnKnown or undefined Object
	LC_OBJ_DEVICE            =  0x00020001,						// Device
	LC_OBJ_SPRITE            =  0x00020002,						// Sprite
	LC_OBJ_SPRITE_C          =  0x00020011,						// Sprite Collector
	LC_OBJ_SPRITE_L          =  0x00020012,						// Sprite Layer
	LC_OBJ_IMAGE             =  0x00020101,						// image
	LC_OBJ_GUI               =  LC_OBJ_SPRITE,					// Gui
	LC_OBJ_MODEL2D           =  0x00020111,						// 2D Model
	LC_OBJ_TSA               =  0x00020112,						// Tsa File
	LC_OBJ_MODEL3D           =  0x00020113,						// 3D Model
	LC_OBJ_MEDIA             =  0x00020200,						// MEDIA
	LC_OBJ_AUDIO             =  0x00020211,						// sound
	LC_OBJ_VIDEO             =  0x00020212,						// video
	LC_OBJ_WAV               =  0x00020221,						// wave
	LC_OBJ_MIDI              =  0x00020222,						// midi
	LC_OBJ_OGG               =  0x00020223,						// ogg
	LC_OBJ_MP3               =  0x00020224,						// mp3
	LC_OBJ_NSD               =  0x00020230,						// Non-standard data
	LC_OBJ_CSV               =  0x00020231,						// csv
	LC_OBJ_XML               =  0x00020232,						// XML

	LC_OBJ_INPUT             =  0x00020300,						// Input
	LC_OBJ_SCRIPT            =  0x00020501,						// Script file

	// system constant
	LC_MAX_EGL_CONFIG        =  16        ,						// Max EGL Configuration

	// Constant for input device
	LC_MAX_KEY               = 256		,
	LC_MAX_TSB               = 16		,
	LC_MAX_SENSOR            = 32		,
	LC_IP_LEN                = 16		,
};


enum LC_VAR
{
	LC_VAR_TYPE_ENGINE       = 0,								// GLC Engine type of mobile or pc
	LC_VAR_VERSION           ,									// GLC Version(_n0) and date(_n1)
	LC_VAR_REF_COUNT         ,									// instance count
	LC_VAR_SCREEN            ,									// screen position(i[0], i[1]), screen width and height(i[2], i[3])
	LC_VAR_WINDOW            ,									// default window hadle(_p0) for default 3d device and application etc...
	LC_VAR_DEVICE            ,									// default 3d device(_p1)
	LC_VAR_SPRITE            ,									// default sprite for gui coordinate(_p0) 2d model coordinate(_p1)
	LC_VAR_SPRITE0           = LC_VAR_SPRITE,					// default sprite 0
	LC_VAR_SPRITE1           ,									// default 2d model coordinate(_p1)
	LC_VAR_APP_MAIN          ,									// Application main pointer
	LC_VAR_APP_INPUT         ,									// Application input pointer
	LC_VAR_APP_NET           ,									// Application network pointer

	LC_VAR_USER_DATA         = 32,								// user data starting index
	LC_VAR_TOTAL             = 64,
};


// succeeded/failed macro
#ifndef LC_SUCCEEDED
  #define LC_SUCCEEDED(hr) ((HRESULT)(hr) >= 0)
  #define LC_FAILED(hr)    ((HRESULT)(hr) <  0)
#endif

// Minimum and maximum macros
#define lc_max(a,b)  (((a) > (b)) ? (a) : (b))
#define lc_min(a,b)  (((a) < (b)) ? (a) : (b))

// inline
#ifndef LC_INLINE
  #if defined(_MSC_VER)
    #define LC_INLINE __inline
  #else
    #define LC_INLINE inline __attribute__ ((always_inline))
  #endif
#endif




////////////////////////////////////////////////////////////////////////////////
// Hierarchy

// Declare the Interface
#ifndef DCL_INTERFACE
  #define DCL_INTERFACE struct
#endif

#ifndef DCL_CLASS_DESTROYER
  #define DCL_CLASS_DESTROYER( CLASS_NAME ) \
  virtual ~CLASS_NAME(){}
#endif


// ILC_Base::Query command
//   set/get owner : simple pointer owner. if the object has owner, it' cannot be released.
//   set/get parent : for world transform or node linking
//   set/get sqc parent : timeline sequence parent for animation.
//     if the model has the timeline sequence parent, it will be skipped on update or frame move and it use the parent animation information in rendering.

#define LCQUERY_SET_OWNER		(char*)"Set Owner"
#define LCQUERY_SET_PARENT		(char*)"Set Parent"
#define LCQUERY_SET_TS_PARENT	(char*)"Set TS Parent"

#define LCQUERY_GET_OWNER		(char*)"Get Owner"
#define LCQUERY_GET_PARENT		(char*)"Get Parent"
#define LCQUERY_GET_TS_PARENT	(char*)"Get TS Parent"




////////////////////////////////////////////////////////////////////////////////
// Declaration the GLC base object which is the base of all instance.

DCL_INTERFACE ILC_Base
{
	// Read, Write, or execution of Object: success: >=0 failed: <0
	virtual INT Query(char* sCmd, LC_HANDLE pData)=0;

	// Release Object. It returns remained Reference Count.
	// If all objects are released perfectly, then
	// the Release function returns zero. Failed <0
	//
	// the Release function is same to call LcSys_DestroyObject()
	// definded in LC_Base.h
	virtual INT	Release()=0;
};

typedef ILC_Base*	PLC_BASE;




////////////////////////////////////////////////////////////////////////////////
// Declaration the GLC game object which is the scene graph of all objects

DCL_INTERFACE ILC_Object : public ILC_Base
{
protected:
	union{ struct { INT id0, id1, id2, id3; };	INT m_id[4]; };	// object id(128bit)

	// basic node												// all node related world transform.
	ILC_Object*	m_Prn;											// parent node.
	ILC_Object*	m_Chd;											// child node
	ILC_Object*	m_Bfr;											// previous node
	ILC_Object*	m_Nxt;											// next node

	// Linking intance											// linkin extra data
	LC_HANDLE	m_Own;											// Instance Onwer: if there the owner exist, this instance cannot be released.
	LC_HANDLE	m_Usr[4];										// extra instance created by user.

public:
	ILC_Object();
	virtual ~ILC_Object();

	// for id
	INT			GetId(INT n=0);									// return the id
	const INT* const GetIds() const;							// return the ids

	void		SetId(INT n, INT v);							// set the id
	void		SetIds(INT =LC_DEFAULT, INT =LC_DEFAULT, INT =LC_DEFAULT, INT =LC_DEFAULT);
	void		GetIds(INT& a,INT& b,INT& c,INT& d);

	virtual INT GetObjType() { return LC_OBJ_UNKNOWN; }			// get the Object type

	// Node Iteration
	INT			HasParent()		;								// Has parent?
	INT			HasNotParent()	;								// Has no parent?
	INT			HasChild()		;								// Has child?
	INT			HasNotChild()	;								// Has no child?
	INT			IsSiblingF()	;								// Am I first child?

	INT			IsSiblingL()	;								// Am I last child?
	INT			IsRoot()		;								// Am I root?
	INT			IsChild()		;								// Am I Child?

	ILC_Object* GetParent()		;								// parent
	ILC_Object* GetChild()		;								// child
	ILC_Object* GetBefore()		;								// previous
	ILC_Object* GetNext()		;								// next

	ILC_Object* GetSiblingF()	;								// find first node from sibling list
	ILC_Object* GetSiblingL()	;								// find last node from sibling list
	ILC_Object* FindRoot()		;								// find root node

	void		AttachTo(ILC_Object* Parent);					// set the parent node. (I'm a child node)
	void		Attach(ILC_Object* Child);						// attach the child node. (I'm parent node)
	void		Detach();										// detach
	INT			CountNodes();									// count node


	// extra data
	LC_HANDLE	GetOwner()		;								// retrieve the owner
	LC_HANDLE	GetUsrData(INT n=0);							// sequence parent(Tsq)

	void		SetOwner(LC_HANDLE v=NULL);						// Setting Owner. if Owner Exist. Texture instance cannot be destroyed.
	void		SetUsrData(INT n=0, LC_HANDLE v=NULL);			// setup the user data

	virtual INT	SetAttrib(UINT dCmd, LC_HANDLE pData){ return LC_EFAIL;	}	// Extended the Query. set the attribute value
	virtual INT	GetAttrib(UINT dCmd, LC_HANDLE pData){ return LC_EFAIL;	}	// Extended the Query. get the attribute value
};

typedef ILC_Object*	PLC_OBJECT;




////////////////////////////////////////////////////////////////////////////////
// for event and message listener

DCL_INTERFACE ILC_MsgPrc : public ILC_Base
{
	virtual INT Query(char*, LC_HANDLE)	{	return LC_EFAIL;	}
	virtual INT	Release()				{	return LC_EFAIL;	}

	// for event or mesage procedure function
	virtual INT	MsgPrc(LC_HANDLE =0,LC_HANDLE =0,LC_HANDLE =0,LC_HANDLE =0) =0;
};

typedef ILC_MsgPrc*	PLC_MSGPRC;




////////////////////////////////////////////////////////////////////////////////
// 2D, 3D basic structure

struct LC_INT2_  { union{ struct{ INT x, y;       }; INT m[2]; }; };
struct LC_INT3_  { union{ struct{ INT x, y, z;    }; INT m[3]; }; };
struct LC_INT4_  { union{ struct{ INT x, y, z, w; }; INT m[4]; }; };

struct LC_FLOAT2_{ union{ struct{ FLOAT x, y;     }; INT m[2]; }; };
struct LC_FLOAT3_{ union{ struct{ FLOAT x, y, z;  }; INT m[3]; }; };
struct LC_FLOAT4_{ union{
		struct	{ FLOAT x, y, z, w; };
		struct	{ FLOAT a, b, c, d; };
		FLOAT m[4];
	};
};

struct LC_POINT2
{
	INT x, y;

	LC_POINT2() : x(0), y(0){}
	LC_POINT2(INT X,INT Y) : x(X), y(Y){}

	operator INT*()             {	return &x;             }
	operator const INT*() const {	return (const INT*)&x; }
	LC_POINT2& operator+=(const LC_POINT2& v){x+=v.x;y+=v.y;return *this; }
	LC_POINT2& operator-=(const LC_POINT2& v){x-=v.x;y-=v.y;return *this; }
};

struct LC_POINT3
{
	INT x, y, z;

	LC_POINT3() : x(0), y(0), z(0){}
	LC_POINT3(INT X,INT Y,INT Z) : x(X), y(Y), z(Z){}

	operator INT*()             {	return &x;             }
	operator const INT*() const {	return (const INT*)&x; }
	LC_POINT3& operator+=(const LC_POINT3& v){x+=v.x;y+=v.y;z+=v.z;return *this;	}
	LC_POINT3& operator-=(const LC_POINT3& v){x-=v.x;y-=v.y;z-=v.z;return *this;	}
};

struct LC_RECT
{
	union
	{ 
		struct{  INT   x,   y,    w,   h;     };		//w ==> width, h==>height
		struct{  INT   l,   t,    r,   b;     };
		struct{  INT left, top, right, bottom; };
		FLOAT m[4];
	};

	LC_RECT() : x(0), y(0), w(0), h(0){}
	LC_RECT(INT X,INT Y,INT W,INT H) : x(X), y(Y), w(W), h(H){}

	operator INT*()             {	return &x;             }
	operator const INT*() const {	return (const INT*)&x; }
};

struct LC_SHORT2 {
	union{ struct{	SHORT x, y; }; SHORT m[2]; };

	LC_SHORT2() : x(0), y(0){}
	LC_SHORT2(SHORT X,SHORT Y) : x(X), y(Y){}

	operator SHORT*()             {	return &x;              }
	operator const SHORT*() const {	return (const SHORT*)&x;}
};

struct LC_WORD2
{
	union{ struct{	WORD x, y;    };	WORD m[2];	};

	LC_WORD2() : x(0), y(0){}
	LC_WORD2(WORD X,WORD Y) : x(X), y(Y){}

	operator WORD*()             {	return &x;              }
	operator const WORD*() const {	return (const WORD*)&x; }
};


#if defined(_MSC_VER)
  #pragma pack(push, 2)
#endif

struct LC_SHORT3
{
	union{ struct{ SHORT x, y, z;}; SHORT m[3]; };

	LC_SHORT3() : x(0), y(0), z(0){}
	LC_SHORT3(SHORT X,SHORT Y,SHORT Z) : x(X), y(Y), z(Z){}

	operator SHORT*()             {	return &x;               }
	operator const SHORT*() const {	return (const SHORT*)&x; }
}
#if defined(_MSC_VER)
  #pragma pack(pop)
#else
  __attribute__((packed))
#endif
;

#if defined(_MSC_VER)
  #pragma pack(push, 2)
#endif

struct LC_WORD3
{
	union{ struct{ WORD x, y, z;}; WORD m[3]; };

	LC_WORD3() : x(0), y(0), z(0){}
	LC_WORD3(WORD X,WORD Y,WORD Z) : x(X), y(Y), z(Z){}

	operator WORD*()             {	return &x;              }
	operator const WORD*() const {	return (const WORD*)&x; }
}
#if defined(_MSC_VER)
  #pragma pack(pop)
#else
  __attribute__((packed))
#endif
;


struct LC_INT2
{
	union{ struct { INT x, y;	};	INT	m[2]; };

	LC_INT2() : x(0), y(0){}
	LC_INT2(INT X,INT Y) : x(X), y(Y){}

	operator INT*()             {	return &x;             }
	operator const INT*() const {	return (const INT*)&x; }
	LC_INT2& operator+=(const LC_INT2& v){x+=v.x;y+=v.y;return *this; }
	LC_INT2& operator-=(const LC_INT2& v){x-=v.x;y-=v.y;return *this; }
};


struct LC_INT3
{
	union{ struct { INT x, y, z; };	INT	m[3]; };

	LC_INT3() : x(0), y(0), z(0){}
	LC_INT3(INT X,INT Y,INT Z) : x(X), y(Y), z(Z){}

	operator INT*()             {	return &x;             }
	operator const INT*() const {	return (const INT*)&x; }
	LC_INT3& operator+=(const LC_INT3& v){x+=v.x;y+=v.y;z+=v.z;return *this; }
	LC_INT3& operator-=(const LC_INT3& v){x-=v.x;y-=v.y;z-=v.z;return *this; }
};


struct LC_INT4
{
	union{ struct { INT x, y, z, w;	}; INT m[4]; };

	LC_INT4() : x(0), y(0), z(0), w(0){}
	LC_INT4(INT X,INT Y,INT Z,INT W) : x(X), y(Y), z(Z), w(W){}

	operator INT*()             {	return &x;             }
	operator const INT*() const {	return (const INT*)&x; }
	LC_INT4& operator+=(const LC_INT4& v){x+=v.x;y+=v.y;z+=v.z;w+=v.w;return *this; }
	LC_INT4& operator-=(const LC_INT4& v){x-=v.x;y-=v.y;z-=v.z;w-=v.w;return *this; }
};


struct LC_FLOAT2
{
	union{ struct { FLOAT x, y;	};	FLOAT m[2];	};

	LC_FLOAT2() : x(0), y(0){}
	LC_FLOAT2(FLOAT X,FLOAT Y) : x(X), y(Y){}

	// casting
	operator FLOAT*()				{	return(FLOAT*)&x;		}
	operator const FLOAT*() const	{	return(const FLOAT*)&x;	}
	FLOAT& operator[](int n)		{	return *((&x)+ n);		}

	// assignment operators
	LC_FLOAT2& operator +=(const LC_FLOAT2& v){ x += v.x;   y += v.y; return *this;	}
	LC_FLOAT2& operator -=(const LC_FLOAT2& v){ x -= v.x;   y -= v.y; return *this;	}
	LC_FLOAT2& operator *=(FLOAT v)			  { x *= v  ;   y *=   v; return *this;	}
	LC_FLOAT2& operator /=(FLOAT v)			  { v=1.0F/v; x*=v; y*=v; return *this;	}

	// unary operators
	LC_FLOAT2 operator +() const	{	return *this;				}
	LC_FLOAT2 operator -() const	{	return LC_FLOAT2(-x, -y);	}

	// binary operators
	LC_FLOAT2 operator+(const LC_FLOAT2& v)const{	return LC_FLOAT2(x + v.x, y + v.y);	}
	LC_FLOAT2 operator-(const LC_FLOAT2& v)const{	return LC_FLOAT2(x - v.x, y - v.y);	}
	LC_FLOAT2 operator*(FLOAT v) const			{	return LC_FLOAT2(x * v  , y * v  );	}
	LC_FLOAT2 operator/(FLOAT v) const	{ v=1.0F/v;	return LC_FLOAT2(x * v  , y * v  );	}
};


struct LC_FLOAT3
{
	union{ struct { FLOAT x, y, z;	};	FLOAT m[3];	};

	LC_FLOAT3() : x(0), y(0), z(0){}
	LC_FLOAT3(FLOAT X,FLOAT Y,FLOAT Z) : x(X), y(Y), z(Z){}

	// casting
	operator FLOAT*()				{	return(FLOAT*)&x;			}
	operator const FLOAT*() const	{	return(const FLOAT*)&x;		}
	FLOAT& operator[](int n)		{	return *((&x)+ n);			}

	// assignment operators
	LC_FLOAT3& operator +=(const LC_FLOAT3& v){ x +=v.x; y +=v.y; z +=v.z; return *this;	}
	LC_FLOAT3& operator -=(const LC_FLOAT3& v){ x -=v.x; y -=v.y; z -=v.z; return *this;	}
	LC_FLOAT3& operator *=(FLOAT v)			  { x *= v ; y *= v ; z *=v  ; return *this;	}
	LC_FLOAT3& operator /=(FLOAT v)			  { v =1.0F/v; x*=v; y*=v; z*=v; return *this;	}

	// unary operators
	LC_FLOAT3 operator +() const	{	return *this;					}
	LC_FLOAT3 operator -() const	{	return LC_FLOAT3(-x, -y, -z);	}

	// binary operators
	LC_FLOAT3 operator +(const LC_FLOAT3& v) const{	return LC_FLOAT3(x+v.x, y+v.y, z+v.z); }
	LC_FLOAT3 operator -(const LC_FLOAT3& v) const{	return LC_FLOAT3(x-v.x, y-v.y, z-v.z); }
	LC_FLOAT3 operator *(FLOAT v) const	          {	return LC_FLOAT3(x*v  , y*v  , z*v  ); }
	LC_FLOAT3 operator /(FLOAT v) const	{ v=1.0F/v; return LC_FLOAT3(x*v  , y*v  , z*v  ); }
};


struct LC_FLOAT4
{
	union
	{
		struct { FLOAT x, y, z, w; };
		struct { FLOAT a, b, c, d; };
		FLOAT m[4];
	};

	LC_FLOAT4() : x(0), y(0), z(0), w(0){}
	LC_FLOAT4(FLOAT X,FLOAT Y,FLOAT Z,FLOAT W) : x(X), y(Y), z(Z), w(W){}

	// casting
	operator FLOAT*()				{	return(FLOAT*)&x;       }
	operator const FLOAT*() const	{	return(const FLOAT*)&x; }
	FLOAT& operator[](int n)		{	return *((&x)+ n);      }

	// assignment operators
	LC_FLOAT4& operator +=(const LC_FLOAT4& v){ x +=v.x; y +=v.y; z +=v.z; w +=v.w; return *this; }
	LC_FLOAT4& operator -=(const LC_FLOAT4& v){ x -=v.x; y -=v.y; z -=v.z; w -=v.w; return *this; }
	LC_FLOAT4& operator *=(FLOAT v)			  { x *= v ;  y *=v ; z *=v  ; w *= v;  return *this; }
	LC_FLOAT4& operator /=(FLOAT v)			  { v =1.0F/v; x*=v; y*=v; z*=v; w*=v;  return *this; }

	// unary operators
	LC_FLOAT4 operator +() const	{	return *this;					}
	LC_FLOAT4 operator -() const	{	return LC_FLOAT4(-x,-y,-z,-w);	}

	// binary operators
	LC_FLOAT4 operator +(const LC_FLOAT4& v) const{	return LC_FLOAT4(x+v.x, y+v.y, z+v.z, w+v.w); }
	LC_FLOAT4 operator -(const LC_FLOAT4& v) const{	return LC_FLOAT4(x-v.x, y-v.y, z-v.z, w-v.w); }
	LC_FLOAT4 operator *(FLOAT v) const			  {	return LC_FLOAT4(x*v  , y*v  , z*v  , w*v  ); }
	LC_FLOAT4 operator /(FLOAT v) const	{ v=1.0F/v;	return LC_FLOAT4(x*v  , y*v  , z*v  , w*v  ); }
};


struct LC_COLORF
{
	union{ struct{ FLOAT r, g, b, a; };	FLOAT m[4];	};

	LC_COLORF() : r(0), g(0), b(0), a(0){}
	LC_COLORF(FLOAT R,FLOAT G,FLOAT B,FLOAT A) : r(R), g(G), b(B), a(A){}


	// casting
	operator COLOR32 () const
	{
		COLOR32 R = (COLOR32) lc_min( (r * 255.F + 0.5F), 255.F );
		COLOR32 G = (COLOR32) lc_min( (g * 255.F + 0.5F), 255.F );
		COLOR32 B = (COLOR32) lc_min( (b * 255.F + 0.5F), 255.F );
		COLOR32 A = (COLOR32) lc_min( (a * 255.F + 0.5F), 255.F );

		return (A << 24) | (R << 16) | (G << 8) | B;
	}

	operator FLOAT *()			{	return (FLOAT*) &r;			}
	operator const FLOAT *() const{	return (const FLOAT*) &r;	}
	FLOAT& operator[](int n)	{	return *((&r)+ n);			}

	// assignment operators
	LC_COLORF& operator +=(const LC_COLORF& v){	r +=v.r; g +=v.g; b +=v.b; a +=v.a;	return *this; }
	LC_COLORF& operator -=(const LC_COLORF& v){	r -=v.r; g -=v.g; b -=v.b; a -=v.a; return *this; }
	LC_COLORF& operator *=(FLOAT v)			{	r *=v;   g *=v;   b *=v;   a *= v;	return *this; }
	LC_COLORF& operator /=(FLOAT v)			{	v=1.0F/v; r*=v;  g*=v; b*=v; a*=v;  return *this; }

	// unary operators
	LC_COLORF operator +() const {	return *this;								}
	LC_COLORF operator -() const {	return LC_COLORF(1.F-r, 1.F-g, 1.F-b, a);	}	// Negative

	// binary operators. Add, Subtract, Modulate, Devide
	LC_COLORF operator +(const LC_COLORF& v) const{	return LC_COLORF(r+v.r, g+v.g, b+v.b, a+v.a); }
	LC_COLORF operator -(const LC_COLORF& v) const{ return LC_COLORF(r-v.r, g-v.g, b-v.b, a-v.a); }
	LC_COLORF operator *( FLOAT v)const	{           return LC_COLORF(r*v  , g*v  , b*v  , a*v  ); }
	LC_COLORF operator /( FLOAT v)const	{v=1.0F/v;  return LC_COLORF(r*v  , g*v  , b*v  , a*v  ); }
};


struct LC_COLORB
{
	union{ struct{ BYTE r, g, b, a; };	BYTE m[4];	};

	LC_COLORB() : r(0), g(0), b(0), a(0){}
	LC_COLORB(BYTE R,BYTE G,BYTE B,BYTE A) : r(R), g(G), b(B), a(A){}

	LC_COLORB(const LC_COLORF& v)
	{
		r = (BYTE) lc_min( (v.r * 255.F), 255.F );
		g = (BYTE) lc_min( (v.g * 255.F), 255.F );
		b = (BYTE) lc_min( (v.b * 255.F), 255.F );
		a = (BYTE) lc_min( (v.a * 255.F), 255.F );
	}

	LC_COLORB(COLOR32 v){r=BYTE(v>>16);g=BYTE(v>>8);b=BYTE(v>>0);a=BYTE(v>>24);} // v must be a argb
	LC_COLORB(const LC_COLORB& v){r =v.r;	g =v.g;	b =v.b;	a =v.a;}

	operator BYTE*()             {	return &r;              }
	operator const BYTE*() const {	return (const BYTE*)&r; }
};


struct LC_RECTF
{
	union
	{
		struct{  FLOAT   x,   y,    w,    h;    };
		struct{  FLOAT   l,   t,    r,    b;    };
		struct{  FLOAT left, top, right, bottom;};
		FLOAT	m[4];
	};

	LC_RECTF() : x(0), y(0), w(0), h(0){}
	LC_RECTF(FLOAT X,FLOAT Y,FLOAT W,FLOAT H) : x(X), y(Y), w(W), h(H){}

	operator FLOAT*()             {	return &x;               }
	operator const FLOAT*() const {	return (const FLOAT*)&x; }
};


// 2d transform matrix 3x2
struct LC_MATRIX3X2
{
	union { struct {
			FLOAT _11, _12;
			FLOAT _21, _22;
			FLOAT _31, _32; };
            FLOAT m[3][2]; };

	LC_MATRIX3X2()	: _11(1), _12(0)
					, _21(0), _22(1)
					, _31(0), _32(0){}

	LC_MATRIX3X2( FLOAT t11, FLOAT t12
				, FLOAT t21, FLOAT t22
				, FLOAT t31, FLOAT t32)
					: _11(t11), _12(t12)
					, _21(t21), _22(t22)
					, _31(t31), _32(t32){}

	operator FLOAT*()             {	return &_11;               }
	operator const FLOAT*() const {	return (const FLOAT*)&_11; }
};


// 2d transform matrix 3x3
struct LC_MATRIX3X3
{
	union { struct {
			FLOAT _11, _12, _13;
			FLOAT _21, _22, _23;
			FLOAT _31, _32, _33; };
            FLOAT m[3][3]; };

	LC_MATRIX3X3()	: _11(1), _12(0), _13(0)
					, _21(0), _22(1), _23(0)
					, _31(0), _32(0), _33(1){}

	LC_MATRIX3X3(FLOAT t11, FLOAT t12, FLOAT t13
				, FLOAT t21, FLOAT t22, FLOAT t23
				, FLOAT t31, FLOAT t32, FLOAT t33)
					: _11(t11), _12(t12), _13(t13)
					, _21(t21), _22(t22), _23(t23)
					, _31(t31), _32(t32), _33(t33){}

	operator FLOAT*()             {	return &_11;               }
	operator const FLOAT*() const {	return (const FLOAT*)&_11; }
};


// 3d transform matrix 4x3
struct LC_MATRIX4X3
{
	union { struct {
			FLOAT _11, _12, _13;
			FLOAT _21, _22, _23;
			FLOAT _31, _32, _33;
			FLOAT _41, _42, _43; };
            FLOAT m[4][3]; };

	LC_MATRIX4X3()
				: _11(1), _12(0), _13(0)
				, _21(0), _22(1), _23(0)
				, _31(0), _32(0), _33(1)
				, _41(0), _42(0), _43(0){}

	LC_MATRIX4X3(
			   FLOAT t11, FLOAT t12, FLOAT t13
			,  FLOAT t21, FLOAT t22, FLOAT t23
			,  FLOAT t31, FLOAT t32, FLOAT t33
			,  FLOAT t41, FLOAT t42, FLOAT t43)
				: _11(t11), _12(t12), _13(t13)
				, _21(t21), _22(t22), _23(t23)
				, _31(t31), _32(t32), _33(t33)
				, _41(t41), _42(t42), _43(t43){}

	operator FLOAT*()             {	return &_11;               }
	operator const FLOAT*() const {	return (const FLOAT*)&_11; }
};


// 3d transform matrix 4x4
struct LC_MATRIX
{
	union { struct {
			FLOAT _11, _12, _13, _14;
			FLOAT _21, _22, _23, _24;
			FLOAT _31, _32, _33, _34;
			FLOAT _41, _42, _43, _44; };
            FLOAT m[4][4]; };

	LC_MATRIX() : _11(1), _12(0), _13(0), _14(0)
				, _21(0), _22(1), _23(0), _24(0)
				, _31(0), _32(0), _33(1), _34(0)
				, _41(0), _42(0), _43(0), _44(1){}

	LC_MATRIX(FLOAT t11, FLOAT t12, FLOAT t13, FLOAT t14
			, FLOAT t21, FLOAT t22, FLOAT t23, FLOAT t24
			, FLOAT t31, FLOAT t32, FLOAT t33, FLOAT t34
			, FLOAT t41, FLOAT t42, FLOAT t43, FLOAT t44)
				: _11(t11), _12(t12), _13(t13), _14(t14)
				, _21(t21), _22(t22), _23(t23), _24(t24)
				, _31(t31), _32(t32), _33(t33), _34(t34)
				, _41(t41), _42(t42), _43(t43), _44(t44){}

	operator FLOAT*()             {	return &_11;               }
	operator const FLOAT*() const {	return (const FLOAT*)&_11; }
};




////////////////////////////////////////////////////////////////////////////////
// glc argument structure

struct TLC_ARG
{
    union {LC_HANDLE p; DOUBLE d; DINT n; UDINT u; };

    operator INT         (){ return (INT)n;          }
    operator UINT        (){ return (UINT)n;         }
    operator SHORT       (){ return (SHORT)n;        }
    operator LONG        (){ return (LONG)n;         }
    operator ULONG       (){ return (ULONG)n;        }
    operator WORD        (){ return (WORD)n;         }
    operator QWORD       (){ return (QWORD)n;        }

    operator DOUBLE      (){ return d;               }
    operator FLOAT       (){ return (float)d;        }

    operator LC_HANDLE   (){ return p;               }
    operator char*       (){ return (char*)p;        }
    operator const char* (){ return (const char*)p;  }
    operator BYTE*       (){ return (BYTE*)p;        }
    operator const BYTE* (){ return (const BYTE*)p;  }

    INT       operator =(const INT       &v){ n=v; return (INT   )n; }
    UINT      operator =(const UINT      &v){ n=v; return (UINT  )n; }
    SHORT     operator =(const SHORT     &v){ n=v; return (SHORT )n; }
    USHORT    operator =(const USHORT    &v){ n=v; return (USHORT)n; }
    LONG      operator =(const LONG      &v){ n=v; return (LONG  )n; }
    ULONG     operator =(const ULONG     &v){ n=v; return (ULONG )n; }
    QWORD     operator =(const QWORD     &v){ n=v; return (QWORD )n; }

    DOUBLE    operator =(const DOUBLE    &v){ d=v; return (DOUBLE)d; }
    FLOAT     operator =(const FLOAT     &v){ d=v; return (FLOAT )d; }

    LC_HANDLE operator =(const LC_HANDLE &v){ p=(LC_HANDLE)v; return (LC_HANDLE)p;}
    char*     operator =(const char*     &v){ p=(LC_HANDLE)v; return (char* )p;   }
    BYTE*     operator =(const BYTE*     &v){ p=(LC_HANDLE)v; return (BYTE* )p;   }
};


struct TLC_ARGS
{
	union
	{
		struct { TLC_ARG _0,_1,_2,_3,_4,_5,_6,_7; };
		struct { TLC_ARG p0,p1,p2,p3,p4,p5,p6,p7; };
		TLC_ARG m[8];
	};
};

typedef TLC_ARGS*		PLC_ARGS;


#define _MAKE_ARG1(v,v0                     )\
	TLC_ARGS v={0}; v._0=(v0);

#define _MAKE_ARG2(v,v0,v1                  )\
	TLC_ARGS v={0}; v._0=(v0);v._1=(v1);

#define _MAKE_ARG3(v,v0,v1,v2               )\
	TLC_ARGS v={0}; v._0=(v0);v._1=(v1);v._2=(v2);

#define _MAKE_ARG4(v,v0,v1,v2,v3            )\
	TLC_ARGS v={0}; v._0=(v0);v._1=(v1);v._2=(v2);v._3=(v3);

#define _MAKE_ARG5(v,v0,v1,v2,v3,v4         )\
	TLC_ARGS v={0}; v._0=(v0);v._1=(v1);v._2=(v2);v._3=(v3);v._4=(v4);

#define _MAKE_ARG6(v,v0,v1,v2,v3,v4,v5      )\
	TLC_ARGS v={0}; v._0=(v0);v._1=(v1);v._2=(v2);v._3=(v3);v._4=(v4);v._5=(v5);

#define _MAKE_ARG7(v,v0,v1,v2,v3,v4,v5,v6   )\
	TLC_ARGS v={0}; v._0=(v0);v._1=(v1);v._2=(v2);v._3=(v3);v._4=(v4);v._5=(v5);v._6=(v6);

#define _MAKE_ARG8(v,v0,v1,v2,v3,v4,v5,v6,v7)\
	TLC_ARGS v={0}; v._0=(v0);v._1=(v1);v._2=(v2);v._3=(v3);v._4=(v4);v._5=(v5);v._6=(v6);v._7=(v7);


#define MAKE_ARG1(v,v0                     )\
	v._0=(v0);v._1=L_NIL;v._2=L_NIL;v._3=L_NIL;v._4=L_NIL;v._5=L_NIL;v._6=L_NIL;v._7=L_NIL;

#define MAKE_ARG2(v,v0,v1                  )\
	v._0=(v0);v._1=(v1) ;v._2=L_NIL;v._3=L_NIL;v._4=L_NIL;v._5=L_NIL;v._6=L_NIL;v._7=L_NIL;

#define MAKE_ARG3(v,v0,v1,v2               )\
	v._0=(v0);v._1=(v1) ;v._2=(v2) ;v._3=L_NIL;v._4=L_NIL;v._5=L_NIL;v._6=L_NIL;v._7=L_NIL;

#define MAKE_ARG4(v,v0,v1,v2,v3            )\
	v._0=(v0);v._1=(v1) ;v._2=(v2) ;v._3=(v3) ;v._4=L_NIL;v._5=L_NIL;v._6=L_NIL;v._7=L_NIL;

#define MAKE_ARG5(v,v0,v1,v2,v3,v4         )\
	v._0=(v0);v._1=(v1) ;v._2=(v2) ;v._3=(v3) ;v._4=(v4) ;v._5=L_NIL;v._6=L_NIL;v._7=L_NIL;

#define MAKE_ARG6(v,v0,v1,v2,v3,v4,v5      )\
	v._0=(v0);v._1=(v1) ;v._2=(v2) ;v._3=(v3) ;v._4=(v4) ;v._5=(v5) ;v._6=L_NIL;v._7=L_NIL;

#define MAKE_ARG7(v,v0,v1,v2,v3,v4,v5,v6   )\
	v._0=(v0);v._1=(v1) ;v._2=(v2) ;v._3=(v3) ;v._4=(v4) ;v._5=(v5) ;v._6=(v6) ;v._6=L_NIL;

#define MAKE_ARG8(v,v0,v1,v2,v3,v4,v5,v6,v7)\
	v._0=(v0);v._1=(v1) ;v._2=(v2) ;v._3=(v3) ;v._4=(v4) ;v._5=(v5) ;v._6=(v6) ;v._7=(v7) ;



////////////////////////////////////////////////////////////////////////////////
// to save the GLOBAL VARIABLEs chunk for system and application

union TLC_VAR
{
	union{ INT    i[ 4]; struct{ INT    _i0, _i1, _i2, _i3; }; struct{ INT _ix,_iy,_iw,_ih; }; };
	union{ LPVOID p[ 2]; struct{ LPVOID _p0, _p1; }; };

	char          b[16];
	union{ SHORT  s[ 8]; struct{ SHORT  _s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7;}; };
	union{ FLOAT  f[ 4]; struct{ FLOAT  _f0, _f1, _f2, _f3; }; struct{ FLOAT x,y,z,w; }; };
	union{ DOUBLE d[ 2]; struct{ DOUBLE _d0, _d1; }; };
	union{ DINT   n[ 2]; struct{ DINT   _n0, _n1; }; };
	union{ UDINT  u[ 2]; struct{ UDINT  _u0, _u1; }; };
};

const TLC_VAR*  LcVar_Read (LC_VAR  _lcVar);							// read the data from global, if _lcVar == LC_VAR_TOTAL then get the all chunk
const LC_HANDLE LcVar_ReadP(LC_VAR  _lcVar);							// get the pointer from global.
void LcVar_Write (LC_VAR _lcVar, TLC_VAR* pChunk=0);					// write the data to global. if(LC_VAR_WINDOW <= _lcVar && _lcVar <= LC_VAR_APP_NET) then directly access
void LcVar_WriteP(LC_VAR _lcVar, LC_HANDLE p0, LC_HANDLE p1=0);			// write the data to global for pointer.

INT  LcVar_Version(UINT* dVer, UINT* dDate);							// SDK Version: Version(Major(Hi-word), Minor(Low-word), Build-date
INT  LcVar_WindowSize(INT* Width, INT* Height, INT bWrite=0);			// screen width, screen height, bWrite = 0 ==> get the screen width and height. else ==> set the screen size
INT  LcVar_RefCountObject();											// Get the Remained GLC Object Reference Count.
INT  LcVar_IncRefCountObject();											// Increase the Reference Count. when the object is created, this function must be called.
INT  LcVar_DecRefCountObject();											// Decrease the Reference Count. when the object is destroy, this function must be called.


#endif	//_LcType_H_

