// Interface for the 3D utilities classes.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _ILCX_Entity_H_
#define _ILCX_Entity_H_

#include <LcType.h>



////////////////////////////////////////////////////////////////////////////////
// Grid and XYZ Axis Line

DCL_INTERFACE ILCX_Entity : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILCX_Entity	);


	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;


	virtual INT		FrameMove()=0;
	virtual void	Render()=0;
};

typedef	ILCX_Entity*	PLCX_ENTITY;


// Create Entity Instance
#define LCXCMD_ENTITY_AXIS	(char*)"Entity Axis"
#define LCXCMD_ENTITY_GRID	(char*)"Entity Grid"

INT LcxEnt_CreateEntity(char* sCmd
				   , PLCX_ENTITY* pOut		// Output instance
				   , LC_HANDLE p1 = NULL	// User Data(Option)
				   , LC_HANDLE p2 = NULL	// No Use
				   , LC_HANDLE p3 = NULL	// No Use
				   , LC_HANDLE p4 = NULL	// No Use
					 );




////////////////////////////////////////////////////////////////////////////////
// Interface for the ILCX_Camera class.

enum ELCX_CAMERA
{
	LCX_CAM_NONE	=0,			//
	LCX_CAM_FIRST	,			// 1st Person
	LCX_CAM_THIRD	,			// 3rd Person
	LCX_CAM_2D		= 0x00100,	// 2d camera
};

struct TLCX_CAMERA
{
	// for Projection Matrix
	FLOAT	fFov;			// Field of View
	FLOAT	fNear;			// Near value
	FLOAT	fFar;			// Far value

	// for View Matrix
	FLOAT	vEye[3];		// Camera Position X,Y,Z
	FLOAT	vLook[3];		// Camera Look At Position X,Y,Z
	FLOAT	vUp[3];			// Camera Up Vector X,Y,Z
};


// Single Camera
DCL_INTERFACE ILCX_Camera : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILCX_Camera	);


	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;


	virtual INT		Update()=0;				// Setup View and Projection Matrix of Camera
	virtual INT		FrameMove()=0;			// Update() + SetTransformViw() and SetTransformPrj()
	virtual INT		SetTransformViw()=0;	// Setup View Matrix of Device and 3d Pipeline.
	virtual INT		SetTransformPrj()=0;	// Setup Projection Matrix of Device and 3d Pipeline.



	// Set Parameter
	virtual void	SetupParamView(const LC_FLOAT3* Eye, const LC_FLOAT3* LookAt, const LC_FLOAT3* Up)=0;	// set the eye, look at, up vector parameter
	virtual void	SetupParamProj(FLOAT Fov, FLOAT ScnW, FLOAT ScnH, FLOAT Near, FLOAT Far)=0;				// set the screen width, height and near, far value parameter

	virtual void	SetupMatrixViw(const LC_MATRIX*)=0;		// set with the view matrix.
	virtual void	SetupMatrixPrj(const LC_MATRIX*)=0;		// set with the projection matrix.

	virtual void	SetEye (const LC_FLOAT3* v)=0;			// set the eye vector parameter.
	virtual void	SetLook(const LC_FLOAT3* v)=0;			// set the look at vector parameter.
	virtual void	SetUp  (const LC_FLOAT3* v)=0;			// set the up vector parameter.

	// Get value
	virtual void	GetParamView(LC_FLOAT3* Eye, LC_FLOAT3* Look, LC_FLOAT3* Up)=0;						// get the releated view matrix parameter.
	virtual void	GetParamProj(FLOAT* Fov, FLOAT* ScnW, FLOAT* ScnH, FLOAT* Near, FLOAT* Far)=0;		// get the releated projection matrix parameter.

	virtual const	LC_MATRIX* const GetMatrixViw()=0;		// get the view matrix.
	virtual const	LC_MATRIX* const GetMatrixPrj()=0;		// get the projection matrix.

	virtual const	LC_FLOAT3* const GetEye()  =0;			// get the eye vector.
	virtual const	LC_FLOAT3* const GetLook() =0;			// get the look at vector.
	virtual const	LC_FLOAT3* const GetUp()   =0;			// get the up vector.

	virtual const	LC_FLOAT3* const GetAxisX()=0;
	virtual const	LC_FLOAT3* const GetAxisY()=0;
	virtual const	LC_FLOAT3* const GetAxisZ()=0;

	virtual const	LC_FLOAT4* const GetFrustumPlanes()=0;
	virtual const	LC_FLOAT3* const GetFrustumPoints()=0;

	// Action
	virtual void	MoveSideward(FLOAT fSpeed)=0;
	virtual void	MoveForward	(FLOAT fSpeed,FLOAT fY=1)=0;
	virtual void	MoveUpward  (FLOAT fSpeed)=0;

	virtual void	RotateYaw	(FLOAT fDelta, FLOAT fSpeed)=0;
	virtual void	RotatePitch	(FLOAT fDelta, FLOAT fSpeed)=0;
};

typedef ILCX_Camera*	PLCX_CAMERA;

// Create Camera Instance
#define LCXCMD_CAMERA_2D	(char*)"2D Camera"
#define LCXCMD_CAMERA_3D	NULL

INT LcxEnt_CreateCamera(char* sCmd
					   , PLCX_CAMERA* pOut			// Output instance
					   , LC_HANDLE p1 = NULL		// User Data(Option)
					   , LC_HANDLE p2 = NULL		// No Use
					   , LC_HANDLE p3 = NULL		// No Use
					   , LC_HANDLE p4 = NULL		// No Use
						);


////5////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Game Model Interface

enum ELCX_MDL
{
	LCXM_TEXNAME			= 64,				// Texture Name Length 64(0x40)
	LCXM_ANINAME			= 40,				// Animation or Sequence Name
	LCXM_FRAME_INTERVAL		= 40,				// Default Animation Time Interval(40ms==>25Frame)

	// state
	LCXM_ST_ACTIVE			= 0x10000000,		// Query active state
	LCXM_ST_UPDATE			= 0x10000001,		// Enable update state
	LCXM_ST_RENDER			= 0x10000002,		// Enable rendering state

	// Resource Type: GetRscType()
	LCXM_SRC_TDA			= 0x20000201,		// 2D Sequence List File
	LCXM_SRC_TDM			= 0x20000202,		// 2D Model File
	LCXM_SRC_TDS			= 0x20000203,		// Simple 2D Model Layer

	LCXM_SRC_ASE			= 0x20000301,		// 3D Max ASE File
	LCXM_SRC_ACM			= 0x20000303,		// 3D LC ACM File
	LCXM_SRC_ID				= 0x20000401,		// Source file id(name). it has not file extension

	// Attributes...
	LCXM_ADVANCE_TIME		= 0x20001001,		// Advanced Time(DOUBLE)
	LCXM_CURRENT_TIME		= 0x20001002,		// Current Time (DOUBLE)
	LCXM_CURRENT_FRAME		= 0x20001003,		// Current Frame(FLOAT)
	LCXM_REPEAT_COUNT		= 0x20001004,		// repeat count (INT). NULL is infinite
	LCXM_ANIMATION			= 0x20001005,		// Set/Get Current Animation. if the current animation finished then will be changed this animation
	LCXM_ANIMATION_ABS		= 0x20001006,		// Set/Get Change the Current Animation Immediately. you use the LC_ARG4 {name, (LC_HANDLE)index}

	LCXM_LAYER_MATRIX		= 0x20001012,		// Updated Layer Matrix
	LCXM_LAYER_MATRIX_PT	= 0x20001013,		// Updated Layer Matrix Address
	LCXM_GEOMETRY_MATRIX	= 0x20001015,		// Updated Geometry Matrix
	LCXM_GEOMETRY_MATRIX_PT	= 0x20001016,		// Updated Geometry Matrix Address

	LCXM_LOOP_SECTION		= 0x20001021,		// Animation Loop Section
	LCXM_FRAME_INDEX		= 0x20001022,
	LCXM_FRAME_INDEX_ABSOLUTE=0x20001023,
	LCXM_MAX_ANI_BLENDING	= 0x20001024,		// Skinning Animation Matrix Blending Number
	LCXM_FRAME_SPEED		= 0x20001025,		// Animation Frame Speed (FLOAT)
	LCXM_REPEATE_COUNT		= 0x20001026,		// Repeate Count(INT)
	LCXM_END_FREEZE_FRAME	= 0x20001027,		// End Freeze Frame

	LCXM_BOUNDING_RECT		= 0x20001041,		// Bounding rect. only 2d model. it needs LCXRECT
	LCXM_BOUNDING_BOX		= 0x20001042,		// Bounding box
	LCXM_BOUNDING_SPHERE	= 0x20001043,		// Bounding sphere
	LCXM_DIFFUSE_COLOR		= 0x20001046,		// Diffuse Color. It needs FLOAT4 RGBA color.

	// Command Execution: SetAttrib()
	LCXM_EXEC_TEXTURE_LOAD	= 0x20100001,		// Texture Loading Command.
	LCXM_TEXTURE_LIST_NUMBER= 0x20100002,		// Texture List Number
	LCXM_TEXTURE_LIST_NAME	= 0x20100003,		// Texture Name List
	LCXM_TEXTURE_LIST_POINTER=0x20100004,		// Texture Pointer List
	LCXM_SPRITE_LAYER		= 0x20100011,		// Set/Get Sprite Layer

	LCXM_CREAE_COLONE		= 0x20100012,		// Create instance(clone) from original instance
};




////////////////////////////////////////////////////////////////////////////////
// Three-dimensional Game Model Interface
DCL_INTERFACE ILCX_Mdl : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILCX_Mdl	);


	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual	INT		FrameMove()=0;
	virtual void	Render()=0;
	virtual INT		GetRscType()=0;
};

typedef	ILCX_Mdl*	PLCX_MDL;

// Create 3D Model
INT LcxEnt_CreateMdl(char* sCmd
				 , PLCX_MDL* pOut				// Output instance
				 , LC_HANDLE sName				// Model File Name
				 , LC_HANDLE pOrg	= NULL		// Original LCX_Mdl Pointer for Clone Creating
				 , LC_HANDLE p3		= NULL		// Not Use
				 , LC_HANDLE p4		= NULL		// Not Use
				 );




#if !defined(_LC_ORG_SRC_)
////////////////////////////////////////////////////////////////////////////////
// 2d Texture Animation Sequence List Interface

struct TLC_TDAREC
{
	char		sName[LCXM_ANINAME];						// Sequence Name
	LCXVECTOR2	pos[4];										// position of vertices. primitive type is triangle fan
	LCXVECTOR2	tex[4];										// texture coordinate uv of vertices. primitive type is triangle fan

	TLC_TDAREC();
	LCXVECTOR2	GetWH();									// get the width and height
	LCXVECTOR2	GetXY();									// get the position
};


DCL_INTERFACE ILCX_Tda : public ILC_Object
{
	DCL_CLASS_DESTROYER( ILCX_Tda );

	virtual INT	Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT	Release()=0;
	virtual INT	GetRscType(){	return LCXM_SRC_TDA; }

	virtual INT	GetRecIdx(char* recName)=0;					// get the record index. if NULL == recName then return the count of Records

	virtual TLC_TDAREC* GetRec(char* recName)=0;			// get the record pointer with name
	virtual TLC_TDAREC* GetRec(INT recIdx)=0;				// get the record pointer with index

	virtual PLC_TEXTURE GetTexture()=0;						// get the texture

	// managing for Tda List
	static ILCX_Tda* Find(INT nIdx);						// get the instance with idx in Tda List
	static INT       FindIdx(char* sFile);					// get the idx in Tda List
	static INT       ListRelease();							// release all instance in tda list.
};

typedef	ILCX_Tda*	PLCX_TDA;

// Create TDA Instance from file
INT LcxEnt_CreateTda(char* sCmd
					 , PLCX_TDA* pOut						// Output instance
					 , LC_HANDLE sFile						// File Name
					 , LC_HANDLE p2		= NULL				// No use
					 , LC_HANDLE p3		= NULL				// No use
					 , LC_HANDLE p4		= NULL				// No use
					 );


// Create TDA Instance from memory with name, buffer pointer, and size
//		PLC_ARGS option
//		p0 : sName	Model File Name
//		p1 : pMem	starting memory pointer
//		p2 : pSize	size(unsigned int) pointer
INT LcxEnt_CreateTdaFromMemory(char* sCmd, PLCX_TDA* pOut, PLC_ARGS	pArg);




struct TLCX_ANI_TIME2D											// 2D sprite animation time squence
{
	INT			nAniC	;										// Current Animation
	INT			nAniS	;										// Current Sequence for current Animation
	DOUBLE		dTimeB	;										// Begin Time
	DOUBLE		dTimeC	;										// Current Time

	TLCX_ANI_TIME2D():nAniC(-1),nAniS(-1),dTimeB(0),dTimeC(0){}
};

////////////////////////////////////////////////////////////////////////////////
// Two-dimensional Game Model Interface

DCL_INTERFACE ILCX_Tdm : public ILC_Object
{
	DCL_CLASS_DESTROYER( ILCX_Tdm );

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual INT		GetRscType(){	return LCXM_SRC_TDM; }

	virtual	INT		FrameMove()=0;
	virtual void	Render()=0;

	// Rendering Position
	virtual INT		SetPosition(const LC_FLOAT2*)=0;			// position: float2
	virtual INT		SetRotation(const LC_FLOAT3*)=0;			// rotation: float3 rotation center(x,y) + rotation angle(z: radian)
	virtual INT		SetScaling (const LC_FLOAT2*)=0;			// scaling : float2

	// managing for original Tdm List
	static ILCX_Tdm* Find(INT nIdx);							// get the instance with idx in original Tdm List
	static INT       FindIdx(char* sFile);						// get the idx in original Tdm List
	static INT       ListRelease();								// release all instance in original Tdm list.
};

typedef	ILCX_Tdm*	PLCX_TDM;

// Create Instance
INT LcxEnt_CreateTdm(char* sCmd
				 , PLCX_TDM* pOut								// Output instance
				 , LC_HANDLE sName								// Model File Name
				 , LC_HANDLE pLyr	= NULL						// Layer pointer. Layer pointer can be setup to SetAttrib(LCXM_SPRITE_LAYER)
				 , LC_HANDLE pOpt	= NULL						// Other Option
				 , LC_HANDLE p4		= NULL						// No Use or Layer
				 );


// Create TDM Instance from memory with name, buffer pointer, and size
// PLC_ARGS option
//	p0 : sName	Model File Name
//	p1 : pMem	starting memory pointer
//	p2 : pSize	size(unsigned int) pointer
//	p3 : pLyr	Layer pointer
//	p4 : pOpt	Texture Load Option. It needs TLC_TEXTURE pointer

INT LcxEnt_CreateTdmFromMemory(char* sCmd, PLCX_TDM* pOut, PLC_ARGS	pArg);




////////////////////////////////////////////////////////////////////////////////
// 2d model sequnce

DCL_INTERFACE ILCX_Tds : public ILC_Object
{
	DCL_CLASS_DESTROYER( ILCX_Tds );

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual INT		GetRscType(){	return LCXM_SRC_TDS; }

	virtual	INT		FrameMove()=0;
	virtual void	Render()=0;

	// Rendering Position
	virtual INT		SetPosition(const LC_FLOAT2*)=0;			// position: float2
	virtual INT		SetRotation(const LC_FLOAT3*)=0;			// rotation: float3 rotation center(x,y) + rotation angle(z: radian)
	virtual INT		SetScaling (const LC_FLOAT2*)=0;			// scaling : float2

	// managing for original Tds List
	static ILCX_Tds* Find(INT nIdx);							// get the instance with idx in original Tds List
	static INT       FindIdx(char* sFile);						// get the idx in original Tds List
	static INT       ListRelease();								// release all instance in original Tds list.
};

typedef	ILCX_Tds*	PLCX_TDS;

// Create Instance
INT LcxEnt_CreateTds(char* sCmd
				 , PLCX_TDS* pOut								// Output instance
				 , LC_HANDLE sName								// Model File Name
				 , LC_HANDLE pLyr	= NULL						// Layer pointer. Layer pointer can be setup to SetAttrib(LCXM_SPRITE_LAYER)
				 , LC_HANDLE pOpt	= NULL						// Other Option
				 , LC_HANDLE p4		= NULL						// No Use or Layer
				 );


// Create TDS Instance from memory with name, buffer pointer, and size
// PLC_ARGS option
//	p0 : sName	Model File Name
//	p1 : pMem	starting memory pointer
//	p2 : pSize	size(unsigned int) pointer
//	p3 : pLyr	Layer pointer
//	p4 : pOpt	Texture Load Option. It needs TLC_TEXTURE pointer

INT LcxEnt_CreateTdsFromMemory(char* sCmd, PLCX_TDS* pOut, PLC_ARGS	pArg);


// release 2d model base resource
INT LcxEnt_ReleaseTdmBase();




////////////////////////////////////////////////////////////////////////////////
// Graphic user interface.

// Gui Type: Dialog, Button, List, Combo box, Static, Edit Box, Check Box, Radio button, slide bar
// Dialog: Only Popup window
// Data structure: Tree Structure


enum ELCX_GUI
{
	// GUI type
	LCXGUI_TYPE			= 0x30000440,		// GuiDefault Type
	LCXGUI_DIALOG		= 0x30000441,		// Dialog
	LCXGUI_SYSBUTTON	= 0x30000442,		// System Button
	LCXGUI_BUTTON		= 0x30000443,		// Button
	LCXGUI_EDIT			= 0x30000444,		// Edit
	LCXGUI_STATIC		= 0x30000445,		// Static String

	LCXGUI_MAX_NAME		= LC_DEFAULT_NAME,	// Max Name Length

	// GUI State
	LCXGUI_DISABLE		= 0x30000000,		// Action and Event Disable
	LCXGUI_ENABLE		= 0x30000001,		// Actin and Event Enable
	LCXGUI_HIDE			= 0x30000010,		// Hide
	LCXGUI_SHOW			= 0x30000011,		// Show
	LCXGUI_ACTIVE		= 0x30000012,		// Show and Active
	LCXGUI_DRAG			= 0x30000014,		// Drag

	// Event Message or Status Value
	LCXGUI_EVNT			= 0x10000000,		// Read/Write Event Information
	LCXGUI_EVNT_DOWN	= 0x10000001,		// Mouse(touch) down
	LCXGUI_EVNT_UP		= 0x10000002,		// Mouse(touch) up
	LCXGUI_EVNT_PRESS	= 0x10000004,		// Mouse(touch) pressed
	LCXGUI_EVNT_MOVE	= 0x10000010,		// Move(drag)
	LCXGUI_EVNT_OVER	= 0x10000020,		// Mouse(touch) over

	LCXGUI_EVNT_CLOSE	= 0x70000000,		// Close Event

	// Member Value
	LCXGUI_INFO			= 0x30001001,		// Gui Info
	LCXGUI_ID			= 0x30001002,		// id. it needs unsigned int
	LCXGUI_NAME			= 0x30001003,		// Name
	LCXGUI_POSITION		= 0x30001004,		// Read/Write Gui Position
	LCXGUI_EXTERN		= 0x30001005,		// setup the vertex with extern value
	LCXGUI_TEXTURE		= 0x30001007,		// setup the texture
	LCXGUI_FONT			= 0x30001008,		// setup the Font. it needs TLC_FONT
	LCXGUI_TEXT			= 0x30001009,		// get the Font string. it must be need font. if there is not exist the font, then the return value is null.
	LCXGUI_CHILD_GUI	= 0x3000100A,		// get the child GUI in dialog. it needs int ID value and pointer. ex) PLCX_GUI pChild = (PLCX_GUI)0x001; pDialog->GetAttrib(LCXGUI_CHILD_GUI, &pChild);// 0x001 is exam id.
};



DCL_INTERFACE	ILCX_Gui;
typedef		ILCX_Gui*	PLCX_GUI;


struct TLCX_GUI_INF				// Gui Info
{
	UINT     nId   ;			// id
	char*    sName ;			// Name
	char*    sText ;			// Text
	UINT     nType ;			// type

	LC_RECTF* rEvnt ;			// Event(or Full Region) Rect(left, top, width, height)
	LC_RECTF* rTitle;			// Menu(or Title) Rect. just only dialog
	LC_RECTF* rBrcs ;			// Button, System button or Static Region.
	LC_RECTF* rRscs ;			// Residue or Remain Rect.  for All Windows

	PLCX_GUI pPrnt ;			// Parent Gui Window

	TLCX_GUI_INF();
};


struct TLCX_GUI_EVENT
{
	PLCX_GUI _o;				// event object
	UINT     _t;				// type
	UINT     _m;				// message

	union						// event value
	{
		//4 * 16 = 64byte
		FLOAT _sf[16];			// FLOAT 16
		BYTE  _sb[64];			// byte 64
		DINT  _sn;				// int 2
	};

	TLCX_GUI_EVENT(PLCX_GUI Gui=0, INT Type=0, INT Msg=0);
	TLCX_GUI_EVENT(const TLCX_GUI_EVENT& rhs);
	TLCX_GUI_EVENT& operator=(const TLCX_GUI_EVENT& rhs);

	void SetUp(PLCX_GUI Gui, INT Type, INT Msg);
	void SetObject(PLCX_GUI Gui);
	void SetType(UINT Type);
	void SetMessage(UINT Message);
	void SetVal(FLOAT* v, int numberOfFloat);
	void SetVal(BYTE* v, int numberOfByte);
	void SetVal(DINT v);
	void ClearValue();
	void ClearAll();
};



// Extern Gui Texture
// rEvent:
//     Dialog:  all event region
//     Button, static:  position
//
// rRects: need 32[LC_RECTF]
//     Dialog:  0~ 3: title
//              4~11: system button: 4:position 5: normal 6: over 7: pressed
//             12~31: body
//     Button: Normal, Over, Pressed.
//     Static: one Rect
//

struct TLCX_GUI_EXTERN		// Extern Gui Texture
{
	TLC_IMAGE* txInf;		// Texture Info
	LC_RECTF*  rEvnt;		// Event: position + width and height => (x, y, width, height).
	LC_RECTF*  rRcts;		// Rects

	TLCX_GUI_EXTERN();
};


DCL_INTERFACE ILCX_Gui : public ILC_Object
{
	DCL_CLASS_DESTROYER( ILCX_Gui );


	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual	INT		FrameMove()=0;
	virtual void	Render()=0;

	virtual INT		GetRscType()=0;								// Resource type: LCXGUI_DIALOG, ...

	// Rendering Position
	virtual INT		SetPosition(const LC_FLOAT2*)=0;			// position float2
	virtual INT		SetRotation(const LC_FLOAT3*)=0;			// rotation: float3 rotation center(x,y) + radian(z)
	virtual INT		SetScaling (const LC_FLOAT2*)=0;			// scaling: float2

	// Node process
	virtual INT		 Attach (PLCX_GUI pChild)=0;				// Attatch child Gui
	virtual INT		 Detatch(PLCX_GUI pChild)=0;				// Detatch child Gui
	virtual PLCX_GUI GetParent()=0;								// return Parent Gui
	virtual PLCX_GUI GetRoot()  =0;								// return Root Gui

	// values
	virtual void	SetState(INT v)=0;							// Write the gui state
	virtual INT		SetText(const char* s,LC_COLORF* c=NULL)=0; // Write the string in gui and text. col=NULL is LC_COLORF(0,0,0,1)
};




// Create graphic user interface.
#define LCXCMD_GUI_DIALOG	(char*)"Dialog"
#define LCXCMD_GUI_SYSBTN	(char*)"System Button"
#define LCXCMD_GUI_BUTTON	(char*)"Button"
#define LCXCMD_GUI_STATIC	(char*)"Static"
#define LCXCMD_GUI_LIST	(char*)"List"

INT LcxEnt_CreateGui(char*       sCmd					// Command(sCmd).
					, PLCX_GUI*  pOut					// Output data
					, LC_HANDLE    p1					// Gui Info TLCX_GUI* Pointer
					, LC_HANDLE    p2 = NULL			// Gui extern data
					, LC_HANDLE    p3 = NULL			// No Use
					, LC_HANDLE    p4 = NULL			// No Use
					);

// Create gui list from file.
INT LcxEnt_CreateGuiListFromFile(char*	 sCmd			// No Use: just null
							, PLCX_GUI** pLst			// Output data List. Return value is the count of List
							, LC_HANDLE  sTex			// output texture file. it needs 260 byte
							, LC_HANDLE  File			// file name
							, LC_HANDLE    p3 = NULL	// No Use
							, LC_HANDLE    p4 = NULL	// No Use
							);


INT		 LcxGui_Process(TLCX_GUI_EVENT* EventInOut);	// Update Gui and return the gui count which event occurred.
void	 LcxGui_Render();								// Rendering All Gui



// Dialog functions.
void     LcxGui_AddToDialogList(PLCX_GUI pDlg);			// Add to Dialog List
void	 LcxGui_AddToRootDialogList(PLCX_GUI pDlg);		// Add to root dialog list
void	 LcxGui_RemoveDialog(PLCX_GUI pGui);			// Remove one dialog
INT		 LcxGui_DestroyAllDialog();						// Remove all dialog list

void	 LcxGui_SetActiveDialog(PLCX_GUI pDlg);			// Set active dialog. NULL is all dialogs are non-active
PLCX_GUI LcxGui_GetActiveDialog();						// get active dialog.




////////////////////////////////////////////////////////////////////////////////
// Effect interface.

enum ELCX_EFFECT
{
	// Effect  type
	LCXEFT_TYPE			= 0x40000100,		// Effect Default Type

	LCXEFT_2D_PARTICLE	= 0x40000001,		// 2D Particle
	LCXEFT_3D_PARTICLE	= 0x40000002,		// 3D Particle
	LCXEFT_TEXTURE_ANI	= 0x40000004,		// Texture Animation

	LCXEFT_REN_TIANGLE	= 0x40000110,		// triangle type
	LCXEFT_REN_QUAD		= 0x40000120,		// quad type
	LCXEFT_REN_POINT	= 0x40000140,		// Point Sprite type

	// Member Value
	LCXEFT_INFO			= 0x40001001,		// Effect Info
};


struct TLCX_PRT2d;

struct TLCX_PRT2dINF				// Particle Info
{
	// particle
	INT				nPrtNum;		// Particle Number
	INT				bPrtRen;		// Rendering type: Point sprite=0, Triangle =1 , Quad=2
	INT				nPrtSize;		// Particle Pointer size
	TLCX_PRT2d*		pPrtList;		// Particle List

	// rendering option
	FLOAT			nPntL;			// Point sprite size
	PLC_TEXTURE		pTex;			// Texture Pointer

	// boundary and Initial value
	LC_RECTF		IntlRect;		// boundary
	LC_FLOAT2		IntlPos;		// Position
	LC_FLOAT2		IntlVel;		// Velocity
	LC_FLOAT2		IntlAcc;		// Acceleration

	FLOAT			IntlRot;		// rotation Angle
	FLOAT			IntlRov;		// rotation Angle Velocity

	LC_FLOAT2		IntlScl;		// scale
	LC_FLOAT2		IntlScv;		// Scale Velocity

	LC_COLORF		IntlCol;		// Color
	LC_COLORF		IntlCov;		// Color Velocity

	FLOAT			Life;			// Life
	FLOAT			Lifv;			// Life speed. Fade Speed

	FLOAT			IntlMgW;		// Width
	FLOAT			IntlMgH;		// Height

	TLCX_PRT2dINF();
};

struct TLCX_PRT2d					// 2D Particle
{
	INT				idx;			// Particle Index
	LC_FLOAT2		pos;			// Position
	LC_COLORF		col;			// Color
	LC_FLOAT2		mag;			// Width(x) and height(y)
	FLOAT			Life;			// Life time

	TLCX_PRT2d();

	virtual INT Init(TLCX_PRT2dINF* pInf);
	virtual INT Reset(TLCX_PRT2dINF* pInf);
	virtual INT Update(TLCX_PRT2dINF* pInf);
};


DCL_INTERFACE ILCX_Effect : public ILC_Object
{
	DCL_CLASS_DESTROYER( ILCX_Effect );


	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;


	virtual	INT		FrameMove()=0;
	virtual void	Render()=0;

	virtual INT		GetRscType()=0;								// Resource type: LCXEFT_2D_PARTICLE, ...
};

typedef ILCX_Effect*	PLCX_EFFECT;

// Create Effect Command
#define LCXCMD_EFT_PARTICLE2D	"Particle 2D"
#define LCXCMD_EFT_PARTICLE3D	"Particle 3D"
#define LCXCMD_EFT_TEXTURE_ANI	"Texture Animation"

INT LcxEnt_CreateEffect( char*        sCmd		// Command.
					   , PLCX_EFFECT* pOut		// Output data
					   , LC_HANDLE    p1		// TLCX_PARTICLE_INF
					   , LC_HANDLE    p2 = NULL	// Effect Info TLCX_Effect* Pointer
					   , LC_HANDLE    p3 = NULL	// No Use
					   , LC_HANDLE    p4 = NULL	// No Use
						);



////////////////////////////////////////////////////////////////////////////////
// Motion Sequence

enum ELCX_MOSQC
{
	// Motion inv
	LCXMO_SQC2D			= 0x50000001,		// 2D Motion
	LCXMO_SQC3D			= 0x50000002,		// 3D Motion

	// Motion type
	LCXMO_CART_INTP		= 0x50000010,		// Cartesian interpolation coordinate path
	LCXMO_BEZIER_INTP	= 0x50000020,		// Bezier interpolation coordinate path
	LCXMO_HERMITE_INTP	= 0x50000040,		// Hermite interpolation coordinate path

	// Member Value
	LCXMO_ADVANCE_TIME	= 0x50001001,		// Advance Time
	LCXMO_CURRENT_TIME	= 0x50001002,		// Current Time
	LCXMO_RESULT_SQC	= 0x50001004,		// Sequence

	// Motion state(get) and Execution(set)
	LCXMO_DEFAULT		= LC_DEFAULT,		// default state. do not exeqution
	LCXMO_BEGIN			= 0x50010000,		// start..
	LCXMO_PLAY			= 0x50010001,		// playing
	LCXMO_FINISH		= 0x50010002,		// end state

	// etc...
	LCXMO_NAME			= 40		,		// Motion Name length
	LCXMO_EVENT			= 4			,		// Event list number

};


// The motion sequence is action of translation, scaling,
// rotation and color variation for object with time line.
// initial conditions for motion
struct TLCX_MOSQIC2D
{
	// basic sequence
	DOUBLE		time;				// time interval
	LC_FLOAT2	pos;				// position
	LC_FLOAT2	scl;				// scaling
	LC_FLOAT2	roc;				// rotation center offset =>  rot_center =  pos + ros
	FLOAT		ror;				// rotation angle(radian).
	LC_COLORF	clr;				// color

	INT		nIdx;					// Index
	char	name[LCXMO_NAME];		// motion name or command
	INT		keys[LCXMO_EVENT];		// action key list or applied events

	// advanced opsition
	LC_FLOAT2	psa;				// acceleration => psa  = a(t)
	LC_FLOAT2	psv;				// velocity		=> psv += a * dt  => pos += v * dt
	LC_FLOAT2	scv;				// scaling speed  => scl += scv * dt
	FLOAT		rov;				// rotation speed.  ==>	ror += rov * dt
	LC_COLORF	clv;				// color speed  ==> color += clv * dt

	TLCX_MOSQIC2D();
	TLCX_MOSQIC2D(const TLCX_MOSQIC2D& v);
	TLCX_MOSQIC2D(char* sName, int TimeOut=0, int* KeyList=NULL);
	TLCX_MOSQIC2D& operator =(const TLCX_MOSQIC2D& v);

	void Reset();
};


struct TLCX_MOSQ2D					// result Motion sequence
{
	DOUBLE		time;				// current time
	LC_FLOAT2	pos;				// position
	LC_FLOAT2	scl;				// scaling
	LC_FLOAT2	roc;				// rotation center
	FLOAT		ror;				// rotation angle(radian).
	LC_COLORF	clr;				// color

	TLCX_MOSQ2D();
};


DCL_INTERFACE ILCX_MoSqc : public ILC_Object
{
	DCL_CLASS_DESTROYER( ILCX_MoSqc );

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual	INT		FrameMove()=0;

	virtual INT		GetRscType()=0;								// Resource type: LCXMO_2D_PARTICLE, ...
};

typedef ILCX_MoSqc*	PLCX_MOSQC;

// Create Motion Sequence
#define LCXCMD_MOSQC_2D	(char*)"MoSqc 2D"
#define LCXCMD_MOSQC_3D	(char*)"MoSqc 3D"

INT LcxEnt_CreateMoSqc( char*     sCmd			// Command.
					, PLCX_MOSQC* pOut			// Output data
					, LC_HANDLE    p1			// Motion Sequence List. => TLCX_MOSQC pointer
					, LC_HANDLE    p2			// List Counter. Int type pointer
					, LC_HANDLE    p3 = NULL	// No Use
					, LC_HANDLE    p4 = NULL	// No Use
					);



#endif// _LC_ORG_SRC_





////////////////////////////////////////////////////////////////////////////////
// Two-dimensional Game Field Map
enum ELCX_TDF
{
	// Type
	LCXTDF_TOPVIEW			= 0x00000011,		// rectangle tile of top view method field.
	LCXTDF_ISOMETRY			= 0x00000012,		// Isometry tile.
};



DCL_INTERFACE ILCX_Tdf : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILCX_Tdf	);

	virtual INT		Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT		Release()=0;

	virtual INT		FrameMove()=0;
	virtual void	Render()=0;
};

typedef ILCX_Tdf*	PLCX_TDF;

// make 2d vertex list from the drawing image
void LcxEnt_VtxFromDrawingImage(
				 LC3D::Vx2UV*		OutVtx		// Output Vertex List(Position + Texture Coord)
			   , const FLOAT		ImgW		// Input Image width
			   , const FLOAT		ImgH		// Input Image height
			   , const LCXRECT*		pImgRc		// Input Image Rect
			   , const LCXVECTOR2*	OffsetPos	// StartPosition
			   , INT		bCCW = LC_TRUE		// Texture coordinate orientation
			  );

void LcxEnt_VtxFromDrawingImage(
				 LCXVECTOR2*		OutPos		// Output Position
			   , LCXVECTOR2*		OutTex		// Output Texture Coordinate
			   , const FLOAT		ImgW		// Input Image width
			   , const FLOAT		ImgH		// Input Image height
			   , const LCXRECT*		pImgRc		// Input Image Rect
			   , const LCXVECTOR2*	OffsetPos	// StartPosition
			   , INT		bCCW = LC_TRUE		// Texture coordinate orientation
			   );

void LcxEnt_VtxFromDrawingImage(
				  LCXVECTOR2*		OutPos		// Output position List. need 4 LCXVECTOR2
				, LCXVECTOR2*		OutTex		// Output Texture List. need 4 LCXVECTOR2
				, const FLOAT		ImgW		// Image Width
				, const FLOAT		ImgH		// Image Height
				, const LCXRECT*	pImgRc		// Image rect
				, const LCXVECTOR2* OffsetPos	// Position
				, const LCXVECTOR2*	pvcScl		// Scaling
				, const LCXVECTOR2*	pvcRot		// Rotation Center
				, FLOAT				fRot		// Angle(Radian)
				, INT		bCCW = LC_TRUE		// Texture coordinate orientation
				);


#endif

