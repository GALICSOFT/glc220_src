//	Copyright(C) GaLic Soft. All rights reserved.
//
//	for Initialize system and 3D Rendering Pipe Line.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcBase_H_
#define _LcBase_H_

#include <LcType.h>


////////////////////////////////////////////////////////////////////////////////
// GLC System

//System Initialize and Close
INT LcSys_DestroyObject(PLC_OBJECT*);									// Release the GLC Object. Same as the ILC_Object::Release().

INT  LcSys_Init(  LC_HANDLE apk = NULL									// Initializes the system to execute a program. apk file name for android ex) "com.galic.Totem" in android system. occasionally windows systems use to current directory. other systems set to the null.
				, LC_HANDLE cls = NULL									// class name for android. other systems are null
				, LC_HANDLE env = NULL									// java env handle for android. other systems are null
				, LC_HANDLE etc = NULL									// no use
				);

void LcSys_Close();														// Release the system  and return to main menu.

// for Window( it's same to Microsoft windows system)
typedef LRESULT (LC_CALLBACK* pLc_MsgPrc)(LC_HWND,UINT,WPARAM,LPARAM);	// for Window Message Procedure. only to use in Microsoft windows system.
typedef INT (LC_CALLBACK* pLc_Run)(LC_HANDLE);							// for Setting Main Run function

LC_HWND LcSys_CreateWindow(pLc_MsgPrc									// Message Procedure function pointer
						  , char*     ClassName							// class Name
						  , INT       ScnW=0							// window(screen) width
						  , INT       ScnH=0							// window(screen) height
						  , INT       ScnX=-1							// window(screen) position X for PC. -1 is center
						  , INT       ScnY=-1							// window(screen) position Y for PC. -1 is center
						  , LC_HANDLE Etc=NULL							// Etc value. Form pointer in Bada system
						  );

INT LcSys_DestroyWindow (LC_HWND*);										// destroy window
INT LcSys_SetMainRunFunc (pLc_Run);										// Set Main Processing Run Function
INT LcSys_Run ();														// Main Processing

INT LcSys_ElapseTime (DOUBLE* Elapsed, INT bWrite=0);					// Set/Get the elapsed millisecond time per on frame.
INT LcSys_StoredTime (DOUBLE* Stored, INT bWrite=0);					// Set/Get the Stored Time.
INT LcSys_FramePerSecond (DOUBLE* Frames, INT bWrite=0);				// Set/Get the Frames per one second.




////////////////////////////////////////////////////////////////////////////////
// System Environment

enum ELC_SYSTEM												// System command and values
{
	LCSYS_ORIENT			= 0x00010001,					// Get the orientation. needs int
	LCSYS_ORIENT_PORTRAIT	= 0x00010002,					// NULL, Portrait Orientation Command. just only embedded system
	LCSYS_ORIENT_RLANDSCAPE	= 0x00010003,					// NULL, Right Rotation Landscape. ex) android application default landscape
	LCSYS_ORIENT_LLANDSCAPE	= 0x00010004,					// NULL, Left Rotation Landscape.
	LCSYS_ORIENT_MATRIX		= 0x00010005,					// Get the orient Matrix. needs FLOAT * 16

	LCSYS_WINDOW_SIZE		= 0x00010011,					// int[2], screen width, screen height. same to LcVar_WindowSize
	LCSYS_CONST_WINDOW_SIZE	= 0x00010012,					// int[2], default Sceen Width, and Height. iPhone(320,480), bada,android(480,800), pc(640,480) only to get Method
	LCSYS_ELAPSED_TIME		= 0x00010013,					// double, elapsed millisecond time per on frame. same to LcSys_ElapseTime
	LCSYS_STORED_TIME		= 0x00010014,					// double, Stored Time. same to LcSys_StoredTime
	LCSYS_FRAME_RATE		= 0x00010015,					// double, Frames per one second. same to LcSys_FramePerSecond
};

INT   LcSys_SetAttrib(UINT Cmd,LC_HANDLE pData=NULL);		// Set the System Attibute data
INT   LcSys_GetAttrib(UINT Cmd,LC_HANDLE pData=NULL);		// Get the System Attibute data

WORD* LcSys_QuadIndex();									// return the quad index
void  LcSys_OrientMatrix(void* pOut, INT nModelType);		// Get the system window direction matrix. needs FLOAT*16. ModelType:: LC_OBJ_SPRITE, LC_OBJ_GUI, LC_RSC_MODEL2D




////////////////////////////////////////////////////////////////////////////////
// 3D Rendering Device

// defined in device
enum ELC_DEVICE_CONST
{
	LC_DEV_MAX_QUAD_LIST	= 16384,						// Maximum number of Quad Index

	LC_DEV_SHADER_NUM		= 64,
	LC_DEV_SHADER_BEGIN		=  0,  							// shader begin index
	LC_DEV_SHADER_SPRITE	=  0,  							// shader index for sprite
	LC_DEV_SHADER_SPRITEMX	=  1,  							// shader index for sprite Mx
	LC_DEV_SHADER_TDM0         ,  							// shader index for 2d model
};

enum ELC_DEVICE_ST
{
	LC_CLEAR_COLOR_BUFFER	= 0x00004000,					// GL_COLOR_BUFFER_BIT
	LC_CLEAR_DEPTH_BUFFER	= 0x00000100,					// GL_DEPTH_BUFFER_BIT
	LC_CLEAR_STENCIL_BUFFER	= 0x00000400,					// GL_STENCIL_BUFFER_BIT
};

// Vertex Format for Fixed Pipe Line
enum ELC_DEVICE_FORMAT
{
	LC_FVF_XYZ				= 0x01,							//	1<<0
	LC_FVF_NORMAL			= 0x02,							//	1<<1
	LC_FVF_DIFFUSE			= 0x04,							//	1<<2
	LC_FVF_TEX1				= 0x08,							//	1<<3
	LC_FMT_INDEX16			= 0x1403,						// GL_UNSIGNED_SHORT
	LC_FMT_INDEX32			= 0x140C,						// GL_FIXED
	LC_FMT_UBYTE			= 0x1401,						// GL_UNSIGNED_BYTE
	LC_FMT_FLOAT			= 0x1406,						// GL_FLOAT
	LC_FMT_FIXED			= 0x140C,						// GL_FIXED
};

enum ELC_TRANSFORM
{
	LC_TS_WORLD				= 0x0001,						// world matrix
	LC_TS_VIEW				= 0x0002,						// viewing matrix
	LC_TS_PROJ				= 0x0003,						// projection matrix
	LC_TS_WORLD_IDENTITY	= 0x0011,						// Setup the world matrix to Identity
	LC_TS_VIEW_IDENTITY		= 0x0012,						// Setup the viewing matrix to Identity
	LC_TS_PROJ_IDENTITY		= 0x0013,						// Setup the projection matrix to Identity
	LC_TS_WORLD_EXT			= 0x0021,						// External world matrix
	LC_TS_VIEW_EXT			= 0x0022,						// External viewing matrix
	LC_TS_PROJ_EXT			= 0x0023,						// External projection matrix

	LC_TS_WORLD_PT			= 0x0031,						// world matrix pointer
	LC_TS_VIEW_PT			= 0x0032,						// viewing matrix pointer
	LC_TS_PROJ_PT			= 0x0033,						// projection matrix pointer

};

enum ELC_PRIMITIVETYPE
{
	LC_PT_POINT_LIST		=  0x0000,						// GL_POINTS
	LC_PT_LINE_LIST			=  0x0001,						// GL_LINES
	LC_PT_LINE_LOOP			=  0x0002,						// GL_LINE_LOOP
	LC_PT_LINE_STRIP		=  0x0003,						// GL_LINE_STRIP
	LC_PT_TRIANGLE_LIST		=  0x0004,						// GL_TRIANGLES
	LC_PT_TRIANGLE_STRIP	=  0x0005,						// GL_TRIANGLE_STRIP
	LC_PT_TRIANGLE_FAN		=  0x0006,						// GL_TRIANGLE_FAN
};

// Rendering State
enum ELC_RENDERINGSTATE
{															// Status(Value)
	LC_RS_CLEAR_COLOR		= 0x010000,						// glClearColor() Set
	LC_RS_SHADE_MODEL		= 0x010001,						// Flat, Smooth
	LC_RS_ZBIAS				= 0x020001,						// Z-BIAS
	LC_RS_DEPTH_TEST		= 0x0B71,						// GL_DEPTH_TEST. Enable(True), Disable(False)
	LC_RS_CULL_FACE			= 0x0B44,						// GL_CULL_FACE, Enable(True), Disable(False), glCullFace()
	LC_RS_CULL_MODE			,								// glFrontFace(),
	LC_RS_BLEND				= 0x0BE2,						// GL_BLEND: AlphaBlending Enable, Disable
	LC_RS_BLEND_SRC			,								// glBlendFunc(Source, ...): Default GL_SRC_ALPHA
	LC_RS_BLEND_DEST		,								// glBlendFunc(..., Dest): Default GL_ONE_MINUS_SRC_ALPHA
	LC_RS_FOG				= 0x0B60,						// glEnable(GL_FOG), glDisable(GL_FOG)
	LC_RS_FOG_DATA			,								// glFogf...(), GL_FOG_COLOR, GL_FOG_MODE, GL_FOG_START, GL_FOG_END  Set

	LC_RS_LIGHTING			= 0x0B50,						// glEnable(GL_LIGHTING), glDisable(GL_LIGHTING)
	LC_RS_LIGHT0			= 0x4000,						// GL_LIGHT0
	LC_RS_LIGHT1			= 0x4001,						// GL_LIGHT1
	LC_RS_LIGHT2			= 0x4002,						// GL_LIGHT2
	LC_RS_LIGHT3			= 0x4003,						// GL_LIGHT3
	LC_RS_LIGHT4			= 0x4004,						// GL_LIGHT4
	LC_RS_LIGHT5			= 0x4005,						// GL_LIGHT5
	LC_RS_LIGHT6			= 0x4006,						// GL_LIGHT6
	LC_RS_LIGHT7			= 0x4007,						// GL_LIGHT7
	LC_RS_LIGHT0_DATA		,								// glLightfv(GL_LIGHT0, ...)
	LC_RS_LIGHT1_DATA		,								// glLightfv(GL_LIGHT1, ...)
	LC_RS_LIGHT2_DATA		,								// glLightfv(GL_LIGHT2, ...)
	LC_RS_LIGHT3_DATA		,								// glLightfv(GL_LIGHT3, ...)
	LC_RS_LIGHT4_DATA		,								// glLightfv(GL_LIGHT4, ...)
	LC_RS_LIGHT5_DATA		,								// glLightfv(GL_LIGHT5, ...)
	LC_RS_LIGHT6_DATA		,								// glLightfv(GL_LIGHT6, ...)
	LC_RS_LIGHT7_DATA		,								// glLightfv(GL_LIGHT7, ...)
	LC_RS_MATERIAL_DATA		,								//

	LC_RS_LINE_WIDTH		= 0x0B21,						// GL_LINE_WIDTH
	LC_RS_VIEWPORT			= 0x0BA2,						// GL_VIEWPORT for glViewport(). needs LC_RECT pointer
	LC_RS_EAGL_RESIZE		= 0x000F0001,					// EAGL Resize
};														
														
// Rendering Status(State Value)
enum ELC_RENDERINGSTATUS
{
	LC_ST_ENABLE				= 0x0001,					// True
	LC_ST_DISABLE				= 0x0000,					// False
	LC_ST_ZERO					= 0x0000,					// GL_ZERO
	LC_ST_ONE					= 0x0001,					// GL_ONE

	LC_ST_NEVER					= 0x0200,					// GL_NEVER
	LC_ST_LESS					= 0x0201,					// GL_LESS
	LC_ST_EQUAL					= 0x0202,					// GL_EQUAL
	LC_ST_LEQUAL				= 0x0203,					// GL_LEQUAL
	LC_ST_GREATER				= 0x0204,					// GL_GREATER
	LC_ST_NOTEQUAL				= 0x0205,					// GL_NOTEQUAL
	LC_ST_GEQUAL				= 0x0206,					// GL_GEQUAL
	LC_ST_ALWAYS				= 0x0207,					// GL_ALWAYS

	LC_ST_FLAT					= 0x1D00,					// GL_FLAT
	LC_ST_SMOOTH				= 0x1D01,					// GL_SMOOTH
	LC_ST_CW					= 0x0900,					// GL_CW
	LC_ST_CCW					= 0x0901,					// GL_CCW
	LC_ST_FRONT					= 0x0404,					// GL_FRONT
	LC_ST_BACK					= 0x0405,					// GL_BACK
	LC_ST_FRONT_AND_BACK		= 0x0408,					// GL_FRONT_AND_BACK
	LC_ST_CULL_FACE				= 0x0B44,					// GL_CULL_FACE
	LC_ST_POLYGON_OFFSET_FILL	= 0x8037,					// GL_POLYGON_OFFSET_FILL

	LC_ST_SRC_COLOR				= 0x0300,					// GL_SRC_COLOR
	LC_ST_ONE_MINUS_SRC_COLOR	= 0x0301,					// GL_ONE_MINUS_SRC_COLOR
	LC_ST_SRC_ALPHA				= 0x0302,					// GL_SRC_ALPHA
	LC_ST_ONE_MINUS_SRC_ALPHA	= 0x0303,					// GL_ONE_MINUS_SRC_ALPHA
	LC_ST_DST_ALPHA				= 0x0304,					// GL_DST_ALPHA
	LC_ST_ONE_MINUS_DST_ALPHA	= 0x0305,					// GL_ONE_MINUS_DST_ALPHA
	LC_ST_DST_COLOR				= 0x0306,					// GL_DST_COLOR
	LC_ST_ONE_MINUS_DST_COLOR	= 0x0307,					// GL_ONE_MINUS_DST_COLOR
	LC_ST_SRC_ALPHA_SATURATE	= 0x0308,					// GL_SRC_ALPHA_SATURATE

	LC_ST_FOG_LINEAR			= 0x2601,					// GL_LINEAR
	LC_ST_GL_EXP				= 0x0800,					// GL_EXP
	LC_ST_GL_EXP2				= 0x0801,					// GL_EXP2

	LC_ST_AMBIENT				= 0x1200,					// GL_AMBIENT
	LC_ST_DIFFUSE				= 0x1201,					// GL_DIFFUSE
	LC_ST_SPECULAR				= 0x1202,					// GL_SPECULAR
	LC_ST_POSITION				= 0x1203,					// GL_POSITION
	LC_ST_SPOT_DIRECTION		= 0x1204,					// GL_SPOT_DIRECTION
	LC_ST_SPOT_EXPONENT			= 0x1205,					// GL_SPOT_EXPONENT
	LC_ST_SPOT_CUTOFF			= 0x1206,					// GL_SPOT_CUTOFF
	LC_ST_CONSTANT_ATTENUATION	= 0x1207,					// GL_CONSTANT_ATTENUATION
	LC_ST_LINEAR_ATTENUATION	= 0x1208,					// GL_LINEAR_ATTENUATION
	LC_ST_QUADRATIC_ATTENUATION = 0x1209,					// GL_QUADRATIC_ATTENUATION
	LC_ST_EMISSION				= 0x1600,					// GL_EMISSION
	LC_ST_SHININESS				= 0x1601,					// GL_SHININESS
};

// texture state
enum ELC_TEXTURE_STATE
{
	LC_TS_TEXTURE_ENV_MODE		= 0x2200,					// GL_TEXTURE_ENV_MODE
	LC_TS_TEXTURE_MAG_FILTER	= 0x2800,					// GL_TEXTURE_MAG_FILTER
	LC_TS_TEXTURE_MIN_FILTER	= 0x2801,					// GL_TEXTURE_MIN_FILTER
	LC_TS_TEXTURE_WRAP_S		= 0x2802,					// GL_TEXTURE_WRAP_S
	LC_TS_TEXTURE_WRAP_T		= 0x2803,					// GL_TEXTURE_WRAP_T
	LC_TS_TEXTURE_ENV_COLOR		= 0x2201,					// GL_TEXTURE_ENV_COLOR
	LC_TS_MODULATE				= 0x2100,					// GL_MODULATE
	LC_TS_DECAL					= 0x2101,					// GL_DECAL
	LC_TS_ADD					= 0x0104,					// GL_ADD
	LC_TS_ADD_SIGNED			= 0x8574,					// GL_ADD_SIGNED
	LC_TS_SUBTRACT				= 0x84E7,					// GL_SUBTRACT
	LC_TS_INTERPOLATE			= 0x8575,					// GL_INTERPOLATE
	LC_TS_NEAREST				= 0x2600,					// GL_NEAREST
	LC_TS_LINEAR				= 0x2601,					// GL_LINEAR
	LC_TS_NEAREST_MIPMAP_NEAREST= 0x2700,					// GL_NEAREST_MIPMAP_NEAREST
	LC_TS_LINEAR_MIPMAP_NEAREST	= 0x2701,					// GL_LINEAR_MIPMAP_NEAREST
	LC_TS_NEAREST_MIPMAP_LINEAR	= 0x2702,					// GL_NEAREST_MIPMAP_LINEAR
	LC_TS_LINEAR_MIPMAP_LINEAR	= 0x2703,					// GL_LINEAR_MIPMAP_LINEAR
	LC_TS_REPEAT				= 0x2901,					// GL_REPEAT
	LC_TS_CLAMP					= 0x812F,					// GL_CLAMP_TO_EDGE
};


struct TLC_FOG
{
	LC_COLORF	FogColor;									// FLOAT r,g,b,a
	FLOAT		FogMode;									// LC_ST_FOG_LINEAR
	FLOAT		FogBegin;									// Fog start
	FLOAT		FogEnd;										// Fog End
};


struct TLC_LIGHT											// GLC Lighting
{
	LC_COLORF	Amb;										// Ambient Color
	LC_COLORF	Dif;										// Diffuse Color
	LC_COLORF	Spc;										// Specular Color
	LC_FLOAT4	Pos;										// Lighting Position
	LC_FLOAT4	Dir;										// Spot Direction
	FLOAT		Spe;										// Spot Exponent
	FLOAT		Cut;										// Spot Cut off
	FLOAT		Att0;										// Constant Attenuation
	FLOAT		Att1;										// Linear Attenuation
	FLOAT		Att2;										// Quadratic Attenuation
};


struct TLC_MATERIAL											// GLC Material
{
	INT			Fce;										// Face
	LC_COLORF	Amb;										// Ambient
	LC_COLORF	Dif;										// Diffuse
	LC_COLORF	Spc;										// Specular
	LC_COLORF	Ems;										// Emissive
	FLOAT		Shp;										// Sharpness
};


// Texture
DCL_INTERFACE	ILC_Texture;
typedef		ILC_Texture*	PLC_TEXTURE;

// Vertex Buffer
DCL_INTERFACE	ILC_VB;
typedef		ILC_VB*		PLC_VB;

// Index Buffer
DCL_INTERFACE	ILC_IB;
typedef		ILC_IB*		PLC_IB;




////////////////////////////////////////////////////////////////////////////////
// 3D rendering

// Device
DCL_INTERFACE ILC_Device : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_Device	);

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;				// inherit from the ILC_Object
	virtual INT		Release()=0;										// inherit from the ILC_Object

	virtual INT		GetObjType() { return LC_OBJ_DEVICE; }				// return the type of instance

	virtual	INT		SetTransform(INT nTransform, const LC_HANDLE v)=0;	// 3D PipeLine World, View, Projection Transform
	virtual	INT		GetTransform(INT nTransform, LC_HANDLE v)=0;

	virtual	INT		BeginScene()=0;										// Prepare Rendering it's going to call the glViewport. and SetContext in iPhone system
	virtual	INT		EndScene()=0;										// Post Rendering

	// Rendering State
	virtual	INT		SetRenderState(INT nState, INT v)=0;				// Redering Machine State
	virtual	INT		SetRenderState(INT nState, LC_HANDLE v)=0;			// Redering Machine State for Pointer
	virtual	INT		SetStatusEnable(INT nStatus)=0;						// Wrapping glEnable()
	virtual	INT		SetStatusDisable(INT nStatus)=0;					// Wrapping glDisable()

	// Texture and Multi-texture...
	virtual INT		SetTextureEnv(INT v1, INT v2)=0;					// glTexEnvi(GL_TEXTURE_ENV, ..., ...)
	virtual INT		SetTextureAddressS (INT nState, INT v)=0;
	virtual INT		SetTextureAddressT (INT nState, INT v)=0;
	virtual INT		SetTextureFilterMin(INT nState, INT v)=0;
	virtual INT		SetTextureFilterMag(INT nState, INT v)=0;
	virtual INT		SetTextureStageOp  (INT nState, INT v)=0;

	virtual	INT		SetFVF(INT nFVF)=0;									// Vertex Format
	virtual INT		SetTexture(INT nState, PLC_TEXTURE pTex)=0;			// Set Texture

	virtual	INT		SetColor(FLOAT r,FLOAT g,FLOAT b,FLOAT a=1.f)=0;	// glColor4f()
	virtual	INT		SetColor(FLOAT* v)=0;								// glColor4v()

	// Rendering Primitive for Vertex Buffer Object
	virtual INT		SetStreamSource(PLC_VB)=0;
	virtual INT		SetIndices(PLC_IB)=0;
	virtual INT		DrawPrimitive()=0;
	virtual INT		DrawIndexedPrimitive(INT nMode, INT nCount)=0;
	virtual INT		DrawPrimitiveUP(INT nMode							// Rendering Primitive for User Memory Pointer
										, INT First
										, INT nVertexCount
										, const void* pVertices
										, INT nStride)=0;

	virtual INT		DrawIndexedPrimitiveUP(INT nMode
										, INT nIndexCount
										, const void *indices
										, const void* pVertices
										, INT dStride)=0;

	virtual INT		Clear(UINT dClearMode)=0;							// glClear()
	virtual INT		Present(LC_HANDLE =0, LC_HANDLE  =0
							, LC_HANDLE =0, LC_HANDLE =0)=0;			// glSwapBuffer() + glFlush()
};

typedef	ILC_Device*	PLC_DEVICE;

#define LCQUERY_GET_SCREEN_WIDTH_HEIGHT (char*)"Get Screen Width and Height"
#define LCQUERY_INVALID                 (char*)"Invalid"
#define LCQUERY_RESTORE                 (char*)"Restore"

// Create Device
INT LcDev_CreateDevice(char* sCmd
					   , PLC_DEVICE* pOut								// Output instance
					   , LC_HANDLE p1									// Window Handle
					   , LC_HANDLE p2 = NULL							// Option
					   , LC_HANDLE p3 = NULL							// No Use
					   , LC_HANDLE p4 = NULL							// No Use
					 );




////////////////////////////////////////////////////////////////////////////////
// Sprite Layer. Mixing and Rendering
// ILC_SpriteS draw is immediately mode. but ILC_Sprite draw mode is delayed draw.
// when you call Draw with different old texture, this instance is going to draw vertex list.
// End() function will be drawing remain vertex list.

DCL_INTERFACE ILC_Sprite : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_Sprite	);

	virtual INT	Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT	Release()=0;

	virtual INT GetObjType() { return LC_OBJ_SPRITE; }					// return the type of instance
	virtual INT GetType() =0;											// return the sprite mode GUI or Model

	virtual INT	Begin(UINT dMode=LC_OBJ_SPRITE)=0;						// Save 3d environment before the execution of Sprite.
	virtual INT	End(UINT dForce=LC_FALSE)=0;							// Draw, Clear and Reload saved 3d environment.

	virtual	INT	SetTransform(INT nTransform, const LC_HANDLE v)=0;		// 3D PipeLine World, View, Projection Transform
	virtual	INT	GetTransform(INT nTransform, LC_HANDLE v)=0;

	virtual	INT	SetRenderState(INT nState, INT v)=0;					// Redering Machine State

	// Draw(), DrawScaling(), DrawEx() Functions redefined to assemble and construct the vertex list in spritemx.
	virtual INT		Draw( PLC_TEXTURE		pTexture					// LC_Texture Pointer
						, const LC_RECTF*	pImgRc						// Image rect
						, const LC_FLOAT2*	vcTrn						// Position
						, const LC_COLORF*	color=NULL					// color
						) =0;

	virtual INT		DrawScaling( PLC_TEXTURE pTexture					// LC_Texture Pointer
						, const LC_RECTF*	pImgRc						// Image rect
						, const LC_FLOAT2*	vcTrn						// Position
						, const LC_FLOAT2*	vcScl						// Scaling
						, const LC_COLORF*	color=NULL					// color
						) =0;


	virtual INT		DrawEx(PLC_TEXTURE		pTexture					// LC_Texture Pointer
						, const LC_RECTF*	pImgRc						// Image rect
						, const LC_FLOAT2*	vcTrn						// Position
						, const LC_FLOAT2*	vcScl						// Scaling
						, const LC_FLOAT2*	vcRot						// Rotation Center
						, FLOAT				fRot						// Angle(Radian)
						, const LC_COLORF*	color=NULL					// color
						) =0;

	// Draw the vertex list
	virtual INT	DrawFlush()=0;											// it is going to render the assembled vertex list. if the current teuxture and new texture is not equal then absolutely draw assembled vertex list and assemble vertex for new texture.
};

typedef	ILC_Sprite*	PLC_SPRITE;

// Create Instance
INT LcDev_CreateSprite(char* sCmd
					   , PLC_SPRITE* pOut								// Output instance
					   , LC_HANDLE p1 = NULL							// No Use
					   , LC_HANDLE p2 = NULL							// No Use
					   , LC_HANDLE p3 = NULL							// No Use
					   , LC_HANDLE p4 = NULL							// No Use
					 );




////////////////////////////////////////////////////////////////////////////////
// Sprite Collector
// Assembling the rectangles and draw.

DCL_INTERFACE ILC_SpriteC : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_SpriteC	);

	virtual INT	Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT	Release()=0;

	virtual INT GetObjType() { return LC_OBJ_SPRITE_C; }				// return the type of instance

	virtual INT		AddRect(const LC_RECTF*	pImgRc						// Image rect
						, const LC_FLOAT2*	vcTrn						// Position
						, const LC_FLOAT2*	vcScl						// Scaling
						, const LC_FLOAT2*	vcRot						// Rotation Center
						, FLOAT				fRot						// Angle(Radian)
						, const LC_COLORF*	color=NULL					// color
						) =0;

	virtual INT		Draw() =0;											// Draw the vertex list
	virtual void	SetTexture(PLC_TEXTURE v) =0;						// Set Texture
};

typedef	ILC_SpriteC*	PLC_SPRITE_C;

// Create Instance
INT LcDev_CreateSpriteC(char* sCmd
					   , PLC_SPRITE_C* pOut								// Output instance
					   , LC_HANDLE p1									// Texture point
					   , LC_HANDLE p2 = NULL							// LC_OBJ_GUI or LC_OBJ_MODEL2D. default is LC_OBJ_GUI.
					   , LC_HANDLE p3 = NULL							// No Use
					   , LC_HANDLE p4 = NULL							// No Use
					 );




////////////////////////////////////////////////////////////////////////////////
// Simple 2D Rendering by rect. It's very slow.

// ILC_SpriteS draw is immediately mode.
DCL_INTERFACE ILC_SpriteS : public ILC_Object							// ILC_SpriteS draw is immediately mode.
{
	DCL_CLASS_DESTROYER(	ILC_SpriteS	);

	virtual INT	Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT	Release()=0;

	virtual INT GetObjType() { return LC_OBJ_SPRITE; }					// return the type of instance
	virtual INT GetType() =0;											// return the sprite mode GUI or Model

	// for 2D Rendering environment
	virtual INT	Begin(UINT dMode=LC_OBJ_SPRITE)=0;						// Save 3d environment before the execution of Sprite.
	virtual INT	End(UINT dForce=LC_FALSE)=0;							// Reload saved 3d environment.

	// 3D PipeLine World, View, Projection Transform
	virtual	INT	SetTransform(INT nTransform, const LC_HANDLE v)=0;		// v needs 4X4 Matrix* or NULL
	virtual	INT	GetTransform(INT nTransform, LC_HANDLE v)=0;			// v needs 4X4 Matrix

	virtual	INT	SetRenderState(INT nState, INT v)=0;					// Redering Machine State

	// draw with image.
	virtual INT		Draw( PLC_TEXTURE		pTexture					// LC_Texture Pointer
						, const LC_RECTF*	pImgRc						// Image rect
						, const LC_FLOAT2*	vcTrn						// Position
						, const LC_COLORF*	color=NULL					// color
						) =0;


	virtual INT		DrawScaling( PLC_TEXTURE pTexture					// LC_Texture Pointer
						, const LC_RECTF*	pImgRc						// Image rect
						, const LC_FLOAT2*	vcTrn						// Position
						, const LC_FLOAT2*	vcScl						// Scaling
						, const LC_COLORF*	color=NULL					// color
						) =0;


	virtual INT		DrawEx(PLC_TEXTURE		pTexture					// LC_Texture Pointer
						, const LC_RECTF*	pImgRc						// Image rect
						, const LC_FLOAT2*	vcTrn						// Position
						, const LC_FLOAT2*	vcScl						// Scaling
						, const LC_FLOAT2*	vcRot						// Rotation Center
						, FLOAT				fRot						// Angle(Radian)
						, const LC_COLORF*	color=NULL					// color
						) =0;
};

typedef	ILC_SpriteS*	PLC_SPRITE_S;

// Create Simple Sprite
INT LcDev_CreateSpriteS(char* sCmd
					, PLC_SPRITE_S* pOut								// Output instance
					, LC_HANDLE p1 = NULL								// int pointer. Sprite type. default is LC_OBJ_GUI. game model needs LC_OBJ_MODEL2D
					, LC_HANDLE p2 = NULL								// No Use
					, LC_HANDLE p3 = NULL								// No Use
					, LC_HANDLE p4 = NULL								// No Use
					 );




////////////////////////////////////////////////////////////////////////////////
// Simple 2D Layer

// ILC_SpriteL draw has simple one "2d Camera" and draw environment.
// when you use the 2d model with camera. this instance will be very helpful.

DCL_INTERFACE ILC_SpriteL : public ILC_Object							// ILC_SpriteL draw is immediately mode.
{
	DCL_CLASS_DESTROYER(	ILC_SpriteL	);

	virtual INT	Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT	Release()=0;

	virtual INT GetObjType() { return LC_OBJ_SPRITE_L; }				// return the type of instance
	virtual INT GetType() =0;											// return the sprite mode GUI or Model

	// for 2D Rendering environment
	virtual INT	Begin(UINT dMode=LC_OBJ_SPRITE)=0;						// Save 3d environment before the execution of Sprite.
	virtual INT	End(UINT dForce=LC_FALSE)=0;							// Reload saved 3d environment.
	virtual INT	Draw(PLC_TEXTURE pTex, FLOAT* vtxPos					// simple vertex draw
					, FLOAT* vtxTex, FLOAT* vColor= NULL)=0;			//

	virtual	INT	SetRenderState(INT nState, INT v)=0;					// Redering Machine State

	// 3D PipeLine View, Projection Transform. world matrix is not applied. you use the SetPosition, SetScaling or SetRotation
	virtual	INT	SetTransform(INT nTransform, const LC_HANDLE v)=0;		// v needs 4X4 Matrix* or NULL. it can be applied only view and projection
	virtual	INT	GetTransform(INT nTransform, LC_HANDLE v)=0;			// v needs 4X4 Matrix. it will be returned world, view, projection matrix

	virtual INT	SetRotation (const LC_FLOAT3* v)=0;						// setup the layer Rotation Centre(x,y) and angle(radian:z)
	virtual INT	SetScaling  (const LC_FLOAT2* v)=0;						// setup the layer scaling
	virtual INT	SetPosition (const LC_FLOAT2* v)=0;						// setup the layer position

	virtual const LC_FLOAT3* GetRotation ()=0;							// get Rotation Centre(x,y) and angle(radian:z)
	virtual const LC_FLOAT2* GetScaling  ()=0;							// get scaling of layer
	virtual const LC_FLOAT2* GetPosition ()=0;							// get position of layer
};

typedef	ILC_SpriteL*	PLC_SPRITE_L;

// Create Sprite Layer
INT LcDev_CreateSpriteL(char* sCmd
					, PLC_SPRITE_L* pOut								// Output instance
					, LC_HANDLE p1 = NULL								// int pointer. Sprite type. default is LC_OBJ_GUI. game model needs LC_OBJ_MODEL2D
					, LC_HANDLE p2 = NULL								// No Use
					, LC_HANDLE p3 = NULL								// No Use
					, LC_HANDLE p4 = NULL								// No Use
					 );




////////////////////////////////////////////////////////////////////////////////
// Image

enum ELC_IMAGE_RSC
{
	LC_IMG_BMP				= 0x2	,
	LC_IMG_TGA				= 0x3	,
	LC_IMG_PNG				= 0x4	,
	LC_IMG_JPG				= 0x5	,						// Only Window Application
	LC_IMG_GPT				= 0x6	,						// for target application
	LC_IMG_MAX_WIDTH		= 1024	,						// Image max width
	LC_IMG_MAX_HEIGHT		= 1024	,						// Image max Height
};

enum ELC_PIXEL_FORMAT
{
	LC_FMT_ALPHA			= 0x1906,						// GL_ALPHA
	LC_FMT_RGB				= 0x1907,						// GL_RGB
	LC_FMT_RGBA				= 0x1908,						// GL_RGBA
	LC_FMT_LUMINANCE		= 0x1909,						// GL_LUMINANCE
	LC_FMT_LUMINANCE_ALPHA	= 0x190A,						// GL_LUMINANCE_ALPHA
	LC_FMT_WORD_565			= 0x8363,						// GL_UNSIGNED_SHORT_5_6_5
	LC_FMT_WORD_5551		= 0x8034,						// GL_UNSIGNED_SHORT_5_5_5_1
	LC_FMT_WORD_4444		= 0x8033,						// GL_UNSIGNED_SHORT_4_4_4_4
};

struct TLC_IMAGE
{
	UINT	ImgF;											// Image Format
	UINT	Type;											// Image Type
	UINT	ImgW;											// Image Width
	UINT	ImgH;											// Image Height
	UINT	ImgD;											// Image Depth
	UINT	ImgC;											// Image Color Key
	BYTE*	pPxl;											// pixel
};

DCL_INTERFACE ILC_Image : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_Image	);

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual UINT	GetRscType()=0;							// Resource File Format BMP, TGA, PNG, GPT
	virtual UINT	GetFormat()=0;							// Pixel Format. only GPT. ==> GL_RGB, RGBA, LUMINANCE, LUMINANCE_ALPHA
	virtual UINT	GetType()=0;							// Pixel Type. only GPT. ==> UNSIGNED_SHORT565, UNSIGNED_SHORT4444, UNSIGNED_SHORT5551

	virtual UINT	GetImgW()=0;							// Image Width
	virtual UINT	GetImgH()=0;							// Image Height
	virtual UINT	GetImgD()=0;							// Image Depth(Channel). 1,2,3, and 4 (byte)
	virtual UINT	GetImgC()=0;							// Image Color Key. only GPT.
	virtual BYTE*	GetPixel()=0;							// Pixel data

	virtual INT		SaveToFile(char* sFile
							,INT ImgType, ULONG* time_s)=0;	// Pixel data Save to TGA or GPT with time
};

typedef	ILC_Image*	PLC_IMAGE;

// Create Image
#define LCCMD_IMAGE_RESOURCE	(char*)"Resource"
#define LCCMD_IMAGE_MEMORY		(char*)"Memory"
#define LCCMD_IMAGE_DEFAULT	NULL

INT LcDev_CreateImage(char* sCmd							// Null is File, "Memory" is Memory buffer.
					   , PLC_IMAGE* pOut					// Output instance
					   , LC_HANDLE p1						// sCmd => NULL ==>File Name, "Resource" ==>Resource ID, "Memory" ==> Image Width
					   , LC_HANDLE p2						// "Memory" ==> Image Height
					   , LC_HANDLE p3 = NULL				// Color Key
					   , LC_HANDLE p4 = NULL				// Convert pixel bit 32 ==> 16 option
					 );

// Create Image From File
INT LcDev_CreateImageFromFile(char* sCmd					// absolutely, you must set the value to Null.
					   , PLC_IMAGE* pOut					// Output instance
					   , LC_HANDLE p1						// File Name
					   , LC_HANDLE p2 = NULL				// absolutely, you must set the value to Null.
					   , LC_HANDLE p3 = NULL				// Color Key
					   , LC_HANDLE p4 = NULL				// Convert pixel bit 32 ==> 16 option
					 );

// Create Image From Memory
INT LcDev_CreateImageFromMemory(char* sCmd					// Null
					   , PLC_IMAGE* pOut					// Output instance
					   , LC_HANDLE p1						// memory
					   , LC_HANDLE p2						// memory buffer size
					   , LC_HANDLE p3 = NULL				// Color Key
					   , LC_HANDLE p4 = NULL				// Convert pixel bit 32 ==> 16 option
					 );

// Convert pixel bit 32 ==> 16
// if dInFormat is zero, then the format value will be reset such as
//  LC_FMT_WORD_565, LC_FMT_WORD_5551, LC_FMT_WORD_44444 at function inside.
//
//  Color Key: 0x00000000 ==> disable the colorkey
//  Channel == 3
//    Color Key enable  ==> LC_FMT_WORD_565(GL_UNSIGNED_SHORT_5_6_5)
//    Color Key disable ==> LC_FMT_WORD_5551(GL_UNSIGNED_SHORT_5_5_5_1)
//  Channel == 4
//    regardless of Color Key ==> LC_FMT_WORD_4444(GL_UNSIGNED_SHORT_4_4_4_4)

INT	LcDev_Convert32to16Color(WORD*       pDst
						   , UINT*       pOutFormat			// ELC_PIXEL_FORMAT: LC_FMT_RGB or LC_FMT_RGBA.
						   , UINT*       pOutType			// ELC_PIXEL_FORMAT: LC_FMT_WORD_565, LC_FMT_WORD_5551,or LC_FMT_WORD_4444
						   , const BYTE* InSrc				// Source Pixel buffer
						   , INT         InImgW				// Image Width
						   , INT         InImgH				// Image Height
						   , INT         InImgD				// Image Depth(byte)
						   , UINT        InColorKey			// Color Key
						   , UINT        dInFormat=0		// LC_FMT_WORD_565, LC_FMT_WORD_5551, LC_FMT_WORD_44444
						   );




////////////////////////////////////////////////////////////////////////////////
// Texture

struct TLC_TEXTURE											// For Load Texture Option
{
	char*		Name;										// Image File Path or Name
	UINT		ColKey;										// Image Color Key
	UINT		MipLvl;										// Mipmap Level
	UINT		Use16;										// Convert 16(LC_FMT_INDEX16)bit?
	LC_HANDLE	Mem;										// Memory Pointer
	UINT		Size;										// Memory Size
	UINT		Pool;										// Save Original Memory? LC_TRUE, LC_FALSE
};

DCL_INTERFACE ILC_Texture : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_Texture	);

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual const char* const GetName()const=0;				// Resource Name
	virtual UINT	GetId()=0;								// OpenGL ID
	virtual UINT	GetFormat()=0;							// Pixel Format rgb, rgba, luminance...
	virtual UINT	GetType()=0;							// Pixel Type	u_short 565, 5551, 4444

	virtual UINT	GetImgW()=0;							// Image Width
	virtual UINT	GetImgH()=0;							// Image Height
	virtual UINT	GetImgD()=0;							// Image Depth
	virtual UINT	GetImgC()=0;							// Image ColorKey
	virtual UINT	GetTexW()=0;							// Texture Width
	virtual UINT	GetTexH()=0;							// Texture Height

	virtual	INT		BindTexture(INT modulate=0				// Binding Texture: If moduate =0  then Release Binding
					, INT filter_min=LC_TS_LINEAR			// Minification Filter. GL_LINEAR: 0x2601
					, INT filter_mag=LC_TS_LINEAR			// Magnification Filter
					, INT smp_addrS=LC_TS_REPEAT			// Texture Address mode for the S coordinate. GL_REPEAT: 0x2901
					, INT smp_addrT=LC_TS_REPEAT			// Texture Address mode for the T coordinate
					)=0;

	virtual	INT		CopyTexImage2D()=0;						// glCopyImage2D: Copy to Texture from Frame Buffer
};

// Rendering Copy Texture
INT LcDev_CreateTextureTarget(  char* sCmd					// Null
						, PLC_TEXTURE* pOut					// Output instance
						, LC_HANDLE p1 = NULL				// Image Width.
						, LC_HANDLE p2 = NULL				// Image Height.
						 );

// Create Texture from merged file:
// if the p4 is the pointer of unsigned int has LC_FMT_INDEX16, then this function operates with same as the LcDev_CreateTexture.
INT LcDev_CreateTextureFromFile(char* sCmd					// Null
						, PLC_TEXTURE* pOut					// Output instance
						, LC_HANDLE p1						// "File Name"
						, LC_HANDLE p2 = NULL				// Color Key(unsigned int 32bit)
						, LC_HANDLE p3 = NULL				// Mipmap Level (NULL == 0 Level, unsigned 32bit)
						, LC_HANDLE p4 = NULL				// Convert 16bit?(unsigned 32bit and LC_FMT_INDEX16 is 16bit conversion)
						 );

// Create Texture from specified TLC_IMAGE struct value
INT LcDev_CreateTextureFromBuffer(char* sCmd				// Null
						, PLC_TEXTURE* pOut					// Output instance
						, LC_HANDLE p1						// struct TLC_IMAGE Address
						, LC_HANDLE p2 = NULL				// Mipmap Level (NULL == 0 Level, unsigned 32bit)
						 );

// Create Texture from Pixel in Stored Memory
// Same to LcDev_CreateTextureFromFile(). create texture in in memory
INT LcDev_CreateTextureFromMemory(char* sCmd				// Null
						, PLC_TEXTURE* pOut					// Output instance
						, LC_HANDLE p1						// Memory Address
						, LC_HANDLE p2						// Memory Size(UINT)
						, LC_HANDLE p3						// "Name"
						, LC_HANDLE p4 = NULL				// Color Key
						, LC_HANDLE p5 = NULL				// Mipmap Level (NULL == 0 Level, unsigned 32bit)
						, LC_HANDLE p6 = NULL				// Convert pixel bit 32 ==> 16 option
						 );

INT LcDev_CreateRenderCopyTexture(char* sCmd				// Null
						, PLC_TEXTURE* pOut					// Output instance.
						, LC_HANDLE p1						// Texture Width Address. ex) INT Width = 512, => &Width
						, LC_HANDLE p2						// Texture Height Address. ex) INT Height = 512, => &Height
						 );




////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer and Index Buffer Data

DCL_INTERFACE ILC_VB : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_VB	);

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual INT		Fill(LC_HANDLE Buf,INT BufSize,INT bReCreate=LC_FALSE)=0;	// Fill Buffer
	virtual INT		Bind(INT bBind=LC_TRUE)=0;									// Bind Buffer
};


// Create Vertex Buffer Instance
INT LcDev_CreateVB(char* sCmd
			   , PLC_VB* pOut								// Output instance
			   , LC_HANDLE p1								// Input FVF
			   , LC_HANDLE p2 = NULL						// Input Buffer pointer
			   , LC_HANDLE p3 = NULL						// Input Buffer Size(Byte)
			   , LC_HANDLE p4 = NULL						// No Use
				 );




DCL_INTERFACE ILC_IB : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_IB	);

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual INT		Fill(LC_HANDLE Buf,INT BufSize,INT ReCreate=LC_FALSE)=0;	// Fill Buffer
	virtual INT		Bind(INT bBind=LC_TRUE)=0;									// Bind Buffer
	virtual UINT	GetFmt()=0;													// Get Format
};

// Create Vertex Buffer Instance
INT LcDev_CreateIB(char* sCmd
			   , PLC_IB* pOut								// Output instance
			   , LC_HANDLE p1								// Input FVF
			   , LC_HANDLE p2 = NULL						// Input Buffer pointer
			   , LC_HANDLE p3 = NULL						// Input Buffer Size(Byte)
			   , LC_HANDLE p4 = NULL						// No Use
				 );




////////////////////////////////////////////////////////////////////////////////
// Application Frame Work

DCL_INTERFACE ILC_AppFrame : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_AppFrame	);

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual INT		Create(PLC_ARGS =NULL)=0;									// call when the instance create time.
	virtual INT		Cleanup()=0;												// cleanup the initial values and condition

	virtual INT		Run (LC_HANDLE =NULL)=0;									// include while loop
	virtual INT		Proc(LC_HANDLE =NULL)=0;									// include one execution

	virtual INT		Init()		=0;												// application init
	virtual INT		Destroy()	=0;												// destroy the rendering data
	virtual INT		FrameMove()	=0;												// update the rendering data
	virtual INT		Render()	=0;												// Rendering
};

typedef	ILC_AppFrame*	PLC_APPFRAME;

// user defininig functions.
extern PLC_APPFRAME LcSys_GetMainApp();											// Get    the main application instance. It must be implemented in code.
extern INT          LcSys_CreateAppFrame(char* sCmd								// Create the main application instance. It must be implemented in program start.
										, PLC_APPFRAME* pOut
										, PLC_ARGS =NULL);

extern INT          LcSys_DestroyAppFrame();									// Release the main application instance. It must be implemented in program end.
extern INT          LcSys_ProcAppFrame(LC_HANDLE);								// Run the main application instance.
extern INT          LcSys_ExecAppFrame(char* sCmd, LC_HANDLE pData);			// Execute by Command




////////////////////////////////////////////////////////////////////////////////
// 3D Application Auxiliary

// GLC 3D Index, Vertex Format
namespace LC3D
{
	// 16-bit Triangle Index Stream
	#if defined(_MSC_VER)
	  #pragma pack(push, 2)
	#endif

	struct VtxIdxW
	{
		union{	struct{ WORD a; WORD b; WORD c;}; WORD m[3]; };

		VtxIdxW() : a(0), b(1), c(2){}
		VtxIdxW(WORD A,WORD B,WORD C) : a(A), b(B), c(C){}
		VtxIdxW(const WORD* v)	{ a = v[0]; b = v[1]; c = v[2];	}

		operator WORD*()			{	return (WORD*)&a;		}
		operator const WORD*() const{	return (const WORD*)&a;	}

		enum { FVF = (LC_FMT_INDEX16), };
	}
	#if defined(_MSC_VER)
	  #pragma pack(pop)
	#else
	  __attribute__((packed))
	#endif
	;

	// 32-bit FIXED Triangle Index Stream
	struct VtxIdxF
	{
		union{	struct{ LC_FIXED a; LC_FIXED b; LC_FIXED c;}; LC_FIXED m[3]; };

		VtxIdxF() : a(0), b(1), c(2){}
		VtxIdxF(INT A,INT B,INT C) : a(A<<16), b(B<<16), c(C<<16){};
		VtxIdxF(const LC_FIXED* v)	{ a = v[0]; b = v[1]; c = v[2];	}

		operator LC_FIXED*()			{	return (LC_FIXED*)&a;		}
		operator const LC_FIXED*() const{	return (const LC_FIXED*)&a;	}

		enum { FVF = (LC_FMT_INDEX32), };
	};


	////////////////////////////////////////////////////////////////////////////
	// Float type

	struct Vxf
	{
		LC_FLOAT3	p;		// Position Vector
		enum { FVF = (LC_FVF_XYZ), };
	};

	struct VxfD
	{
		LC_FLOAT3	p;		// Position Vector
		LC_COLORB	d;		// Diffuse
		enum { FVF = (LC_FVF_XYZ|LC_FVF_DIFFUSE), };
	};

	struct VxfDUV1
	{
		LC_FLOAT3	p;		// Position Vector
		LC_COLORB	d;		// Diffuse
		LC_FLOAT2	t;		// Texture Coord
		enum { FVF = (LC_FVF_XYZ|LC_FVF_DIFFUSE|LC_FVF_TEX1), };
	};

	struct VxfN
	{
		LC_FLOAT3	p;		// Position Vector
		LC_FLOAT3	n;		// Normal Vector
		enum { FVF = (LC_FVF_XYZ|LC_FVF_NORMAL), };
	};

	struct VxfUV1
	{
		LC_FLOAT3	p;		// Position Vector
		LC_FLOAT2	t;		// Texture Coord
		enum { FVF = (LC_FVF_XYZ|LC_FVF_TEX1), };
	};

	struct VxfNUV1
	{
		LC_FLOAT3	p;		// Position Vector
		LC_FLOAT3	n;		// Normal Vector
		LC_FLOAT2	t;		// Texture Coord
		enum { FVF = (LC_FVF_XYZ|LC_FVF_NORMAL|LC_FVF_TEX1), };
	};

	struct VxfNDUV1
	{
		LC_FLOAT3	p;		// Position Vector
		LC_FLOAT3	n;		// Normal Vector
		LC_COLORB	d;		// Diffuse
		LC_FLOAT2	t;		// Texture Coord
		enum { FVF = (LC_FVF_XYZ|LC_FVF_NORMAL|LC_FVF_DIFFUSE|LC_FVF_TEX1), };
	};

	////////////////////////////////////////////////////////////////////////////
	// Fixed Type

	struct Vxi
	{
		LC_INT3	p;		// Position Vector
		enum { FVF = (LC_FVF_XYZ), };
	};

	struct VxiD
	{
		LC_INT3	p;		// Position Vector
		LC_COLORB	d;		// Diffuse
		enum { FVF = (LC_FVF_XYZ|LC_FVF_DIFFUSE), };
	};

	struct VxiDUV1
	{
		LC_INT3	p;		// Position Vector
		LC_COLORB	d;		// Diffuse
		LC_INT2	t;		// Texture Coord
		enum { FVF = (LC_FVF_XYZ|LC_FVF_DIFFUSE|LC_FVF_TEX1), };
	};

	struct VxiN
	{
		LC_INT3	p;		// Position Vector
		LC_INT3	n;		// Normal Vector
		enum { FVF = (LC_FVF_XYZ|LC_FVF_NORMAL), };
	};

	struct VxiUV1
	{
		LC_INT3	p;		// Position Vector
		LC_INT2	t;		// Texture Coord
		enum { FVF = (LC_FVF_XYZ|LC_FVF_TEX1), };
	};

	struct VxiNUV1
	{
		LC_INT3	p;		// Position Vector
		LC_INT3	n;		// Normal Vector
		LC_INT2	t;		// Texture Coord
		enum { FVF = (LC_FVF_XYZ|LC_FVF_NORMAL|LC_FVF_TEX1), };
	};

	struct VxiNDUV1
	{
		LC_INT3	p;		// Position Vector
		LC_INT3	n;		// Normal Vector
		LC_COLORB	d;		// Diffuse
		LC_INT2	t;		// Texture Coord
		enum { FVF = (LC_FVF_XYZ|LC_FVF_NORMAL|LC_FVF_DIFFUSE|LC_FVF_TEX1), };
	};


	////////////////////////////////////////////////////////////////////////////
	// 2D Vertex

	struct Vx2UV
	{
		LC_FLOAT2	p;
		LC_FLOAT2	t;

		Vx2UV():p(0,0),t(0,0){}
		Vx2UV(FLOAT X,FLOAT Y,FLOAT U,FLOAT V):p(X,Y),t(U,V){}
	};

	struct Vx2DUV
	{
		LC_FLOAT2 p;
		LC_COLORB d;
		LC_FLOAT2 t;

		Vx2DUV() : p(0,0),d(0xFFFFFFFF),t(0,0){}
		Vx2DUV(FLOAT X,FLOAT Y,FLOAT U,FLOAT V,UINT D=0xFFFFFFFF):p(X,Y),d(D),t(U,V){}
	};
};




////////////////////////////////////////////////////////////////////////////////
//	LC_Font Interface for printing string to 3d device.

enum ELC_FONT
{
	LC_FONT_ALIGN_LT =0x0	,								// alignment left top
	LC_FONT_ALIGN_RT		,								// alignment right top
	LC_FONT_ALIGN_CT		,								// alignment center top
	LC_FONT_ALIGN_LM		,								// alignment left middle
	LC_FONT_ALIGN_RM		,								// alignment right middle
	LC_FONT_ALIGN_CM		,								// alignment center middle
	LC_FONT_ALIGN_LB		,								// alignment left bottom
	LC_FONT_ALIGN_RB		,								// alignment right bottom
	LC_FONT_ALIGN_CB		,								// alignment center bottom

	// for {Set|Get}Attrib
	LC_FONT_ALIGN	 =0x00001001,							// alignment
	LC_FONT_COLOR			,								// color. it needs LC_COLORF address
	LC_FONT_POSITION		,								// position. it needs LC_FLOAT2 address
	LC_FONT_SCALING			,								// scaling. it needs LC_FLOAT2 address
	LC_FONT_SCALEUP			,								// font resolution scaling up. Mininum is 1 and Maximum is 4
	LC_FONT_CHAR_COUNT		,								// Character Number(count)
	LC_FONT_RECT			,								// Character Rectangle. needs LC_RECTF. if you use this data to position you needs scale up value by LC_FONT_SCALEUP
	LC_FONT_RECT_SCALEUP	,								// Character Rectangle with scale up value.
};

struct TLC_FONT
{
	INT		iHeight	;										// Height
	char*	sName	;										// Font File Name or LCCMD_FONT_DEFAULT
	BYTE**	pBuf	;										// Buffer address
	INT		iLen	;										// Buffer Length

	TLC_FONT() : iHeight(0), sName(NULL), pBuf(NULL), iLen(0){}
	TLC_FONT(char*	_s,	INT _h=12)
		: iHeight(_h), sName(_s), pBuf(NULL), iLen(0){}

	TLC_FONT(char* _s, unsigned char** _b=NULL, INT _l=0, INT _h=12)
		: iHeight(_h), sName(_s), pBuf(_b), iLen(_l){}
};

DCL_INTERFACE ILC_Font : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILC_Font	);


	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual INT		SetString  (const char* sMsg)=0;							// Setting the string
	virtual	INT		SetColor   (const LC_COLORF*)=0;							// String color
	virtual INT		SetPosition(const LC_FLOAT2*)=0;							// String Position
	virtual INT		SetScaling (const LC_FLOAT2*)=0;							// scaling: float2

	virtual INT		DrawTxt    (PLC_SPRITE pSprite= NULL						// Rendering with sprite pointer
								, INT  Begin= 0									// Begin String Index	start is 0
								, INT  End  =-1									// End string Index -1 means to draw all string from begin
								, const LC_FLOAT2* vcTrn = NULL					// Position
								, const LC_COLORF* color = NULL					// Color
								)=0;

	virtual const char * const GetString  () const =0;							// return saved string in font instance
	virtual const FLOAT* const GetColor   () const =0;							// Get color
	virtual const FLOAT* const GetPosition() const =0;							// Get position
	virtual PLC_TEXTURE        GetTexture () const =0;							// GetTexture

	virtual INT	GetRect(LC_RECTF* pRc, INT Idx= LC_DEFAULT) =0;					// Get Rect with index. return rc value is left, top, right, bottom. if index = LC_DEFAULT then return the all region(left, top, right, bottom)
	virtual INT SetupVtxLst(LC_FLOAT2* OutPos, LC_FLOAT2* OutTex)=0;			// Setup the Vertex List. OutPos and OutTex need 4 array of LCXVECTOR2.
};

typedef	ILC_Font*	PLC_FONT;

INT LcDev_TFFontOpen();										// FreeTypeFont Library Open ==> See http://www.freetype.org/
INT	LcDev_TFFontClose();									// FreeTypeFont Liberay Close

#define LCCMD_FONT_FILE	(char*)NULL
#define LCCMD_FONT_MEMORY	(char*)NULL
#define LCCMD_FONT_DEFAULT	(char*)"Default Font"

// Create Font from File
INT LcDev_CreateFontFromFile(char* sCmd						// LCCMD_FONT_FILE
						   , PLC_FONT* pOut					// Output instance
						   , LC_HANDLE p1					// PLC_SPRITE
						   , LC_HANDLE p2					// TLC_Font*
						   , LC_HANDLE p3 = NULL			// No Use
						   , LC_HANDLE p4 = NULL			// No Use
							 );

// create font from memory buffer
INT LcDev_CreateFontFromMemory(char* sCmd					// LCCMD_FONT_MEMORY or LCCMD_FONT_DEFAULT
						   , PLC_FONT* pOut					// Output instance
						   , LC_HANDLE p1					// PLC_SPRITE
						   , LC_HANDLE p2					// TLC_Font*
						   , LC_HANDLE p3 = NULL			// No Use
						   , LC_HANDLE p4 = NULL			// No Use
							 );




#if defined(__ANDROID__) || defined(ANDROID)
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

// the below functions were redefined if jni.h and they were implemented in glc system.
// jint JNI_OnLoad(JavaVM* vm, void* reserved);
// void JNI_OnUnload(JavaVM* vm, void* reserved);

// additional functions for android system.
JavaVM* JNI_OnGetJavaVM();
int     JNI_OnFindStreamName(char* name);										// return stream id, failed: <0
int     JNI_OnFindStreamRscId(int rsId);										// find the stream id with resource id.
int     JNI_OnAddStream(char* name, int rsId, int streamid, void* pAux);		// failed: <0, succeeded return index 0<=

#ifdef __cplusplus
}
#endif

#endif // __ANDROID__

#endif //_LcBase_H_

