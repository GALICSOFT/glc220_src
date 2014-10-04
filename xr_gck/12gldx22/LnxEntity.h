// Interface for the 3D Utility class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LcxEntity_H_
#define _LcxEntity_H_

#ifndef interface
#define interface struct
#endif


#ifndef LC_CLASS_DESTROYER
#define LC_CLASS_DESTROYER( CLASS_NAME )	\
virtual ~CLASS_NAME(){}
#endif


#pragma warning(disable : 4786)
#include <vector>
using namespace std;



struct LcCam
{
	FLOAT	fScnW;			// Screen Width
	FLOAT	fScnH;			// Screen Height
	FLOAT	fFov;			// Field of View
	FLOAT	fNear;			// Near value
	FLOAT	fFar;			// Far value

	FLOAT	vEyeX;			// Camera Initial Position X
	FLOAT	vEyeY;			// Camera Initial Position Y
	FLOAT	vEyeZ;			// Camera Initial Position Z

	FLOAT	vLookX;			// Camera Look Position X
	FLOAT	vLookY;			// Camera Look Position Y
	FLOAT	vLookZ;			// Camera Look Position Z

	FLOAT	vUpX;			// Camera Up Vector X
	FLOAT	vUpY;			// Camera Up Vector Y
	FLOAT	vUpZ;			// Camera Up Vector Z
};



////////////////////////////////////////////////////////////////////////////////
// Single Camera

interface ILcCam
{
	LC_CLASS_DESTROYER(	ILcCam	);
	
	enum ELcCam
	{
		LC_CAM_NONE	=0,		//
		LC_CAM_FIRST,		// 1 ÀÎÄª
		LC_CAM_THIRD,		// 3 ÀÎÄª
		LC_CAM_ALL,			// 1~3 ÀÎÄª
	};
	

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		FrameMove()=0;
	virtual INT		Query(char* sCmd, void* pData)=0;


	virtual void	SetName(char* sN)=0;
	virtual char*	GetName()=0;
	
	virtual void	SetType(INT	eType)=0;
	virtual ELcCam	GetType()=0;
	
	virtual const	D3DXMATRIX*  const GetMatrixViw()	const=0;
	virtual const	D3DXMATRIX*  const GetMatrixPrj()	const=0;
	virtual const	D3DXMATRIX*  const GetMatrixViwI()	const=0;
	virtual const	D3DXMATRIX*  const GetMatrixBill()	const=0;
	virtual const	D3DXMATRIX*  const GetMatrixViwPrj()const=0;
	virtual const	D3DXMATRIX*  const GetMatrixViwPrjI()const=0;
	
	virtual const	D3DXVECTOR3* const GetEye()	const=0;
	virtual const	D3DXVECTOR3* const GetLook()const=0;
	virtual const	D3DXVECTOR3* const GetUp()	const=0;

	virtual void	SetEye	(D3DXVECTOR3*)=0;
	virtual void	SetLook	(D3DXVECTOR3*)=0;
	virtual void	SetUp	(D3DXVECTOR3*)=0;
	
	virtual FLOAT	GetYaw	()=0;
	virtual FLOAT	GetPitch()=0;
	
	virtual void	SetFov	(FLOAT)=0;
	virtual void	SetScnW	(FLOAT)=0;
	virtual void	SetScnH	(FLOAT)=0;
	virtual void	SetNear	(FLOAT)=0;
	virtual void	SetFar	(FLOAT)=0;

	virtual FLOAT	GetFov	()=0;
	virtual FLOAT	GetScnW	()=0;
	virtual FLOAT	GetScnH	()=0;
	virtual FLOAT	GetAspect()=0;
	virtual FLOAT	GetNear	()=0;
	virtual FLOAT	GetFar	()=0;

	virtual const	D3DXVECTOR3* const GetAxisX()=0;
	virtual const	D3DXVECTOR3* const GetAxisY()=0;
	virtual const	D3DXVECTOR3* const GetAxisZ()=0;

	virtual const	D3DXPLANE*   const GetFrustumPlanes()=0;
	virtual const	D3DXVECTOR3* const GetFrustumPoints()=0;
		
	virtual void	SetParamView(D3DXVECTOR3* Eye, D3DXVECTOR3* Look, D3DXVECTOR3* Up)=0;
	virtual void	SetParamProj(FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar)=0;
	
	virtual void	SetTransformViw()=0;
	virtual void	SetTransformPrj()=0;

	// Pick Ray direction
	virtual D3DXVECTOR3	GetDirectionOfMouse(FLOAT fMouseX, FLOAT fMouseY)=0;

	// Individual Camera
	virtual void	SetMatrixViw(D3DXMATRIX*)=0;
	virtual void	SetMatrixPrj(D3DXMATRIX*)=0;

	virtual void	MoveSideward(FLOAT fSpeed)=0;
	virtual void	MoveForward	(FLOAT fSpeed, FLOAT fY=1)=0;
	virtual void	RotateYaw	(FLOAT fDelta, FLOAT fSpeed)=0;
	virtual void	RotatePitch	(FLOAT fDelta, FLOAT fSpeed)=0;
	virtual	void	Zoom(FLOAT)	= 0;


	// For Third Person Camera
	virtual void	SetBasis(D3DXVECTOR3*) =0;
	virtual const	D3DXVECTOR3* const GetBasis() const=0;

	virtual void	SetEpsilonY(FLOAT)	=0;
	virtual FLOAT	GetEpsilonY()		=0;

	virtual void	SetGap(FLOAT)	=0;
	virtual FLOAT	GetGap()		=0;
};




INT LcCam_Create(char* sCmd
				, ILcCam** pData
				, void* p1=0		// Camera name
				, void* p2=0		// Camera Initial Value
				, void* p3=0		// LPDIRECT3DDEVICE9
				, void* p4=0		// No Use
				, void* p5=0		// No Use
				, void* p6=0		// No Use
				);







////////////////////////////////////////////////////////////////////////////////
// Multi Camera

#define LC_MUTI_CAMERA_MAX	16


interface ILcCamM
{
	LC_CLASS_DESTROYER(	ILcCamM	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		FrameMove()=0;
	virtual INT		Query(char* sCmd, void* pData)=0;


	virtual void	SetName(INT nIdx, char* sN)=0;
	virtual char*	GetName(INT nIdx)=0;
	
	virtual void	SetType(INT nIdx, INT	eType)=0;
	virtual INT		GetType(INT nIdx)=0;
	
	virtual const	D3DXMATRIX*  const GetMatrixViw		(INT nIdx) const=0;
	virtual const	D3DXMATRIX*  const GetMatrixPrj		(INT nIdx) const=0;
	virtual const	D3DXMATRIX*  const GetMatrixViwI	(INT nIdx) const=0;
	virtual const	D3DXMATRIX*  const GetMatrixBill	(INT nIdx) const=0;
	virtual const	D3DXMATRIX*  const GetMatrixViwPrj	(INT nIdx) const=0;
	virtual const	D3DXMATRIX*  const GetMatrixViwPrjI	(INT nIdx) const=0;
	
	virtual const	D3DXVECTOR3* const GetEye (INT nIdx) const=0;
	virtual const	D3DXVECTOR3* const GetLook(INT nIdx) const=0;
	virtual const	D3DXVECTOR3* const GetUp  (INT nIdx) const=0;

	virtual void	SetEye	(INT nIdx, D3DXVECTOR3*)=0;
	virtual void	SetLook	(INT nIdx, D3DXVECTOR3*)=0;
	virtual void	SetUp	(INT nIdx, D3DXVECTOR3*)=0;
	
	virtual FLOAT	GetYaw	(INT nIdx)=0;
	virtual FLOAT	GetPitch(INT nIdx)=0;
	
	virtual void	SetFov	(INT nIdx, FLOAT)=0;
	virtual void	SetScnW	(INT nIdx, FLOAT)=0;
	virtual void	SetScnH	(INT nIdx, FLOAT)=0;
	virtual void	SetNear	(INT nIdx, FLOAT)=0;
	virtual void	SetFar	(INT nIdx, FLOAT)=0;

	virtual FLOAT	GetFov	 (INT nIdx)=0;
	virtual FLOAT	GetScnW	 (INT nIdx)=0;
	virtual FLOAT	GetScnH	 (INT nIdx)=0;
	virtual FLOAT	GetAspect(INT nIdx)=0;
	virtual FLOAT	GetNear	 (INT nIdx)=0;
	virtual FLOAT	GetFar	 (INT nIdx)=0;

	virtual const	D3DXVECTOR3* const GetAxisX(INT nIdx)=0;
	virtual const	D3DXVECTOR3* const GetAxisY(INT nIdx)=0;
	virtual const	D3DXVECTOR3* const GetAxisZ(INT nIdx)=0;

	virtual const	D3DXPLANE*   const GetFrustumPlanes(INT nIdx)=0;
	virtual const	D3DXVECTOR3* const GetFrustumPoints(INT nIdx)=0;
		
	virtual void	SetParamView(INT nIdx, D3DXVECTOR3* Eye, D3DXVECTOR3* Look, D3DXVECTOR3* Up)=0;
	virtual void	SetParamProj(INT nIdx, FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar)=0;
	
	virtual void	SetTransformViw(INT nIdx)=0;
	virtual void	SetTransformPrj(INT nIdx)=0;

	virtual D3DXVECTOR3	GetDirectionOfMouse(INT nIdx, FLOAT fMouseX, FLOAT fMouseY)=0;

	virtual void	SetMatrixViw(INT nIdx, D3DXMATRIX*)=0;
	virtual void	SetMatrixPrj(INT nIdx, D3DXMATRIX*)=0;

	virtual void	MoveSideward(INT nIdx, FLOAT fSpeed)=0;
	virtual void	MoveForward	(INT nIdx, FLOAT fSpeed, FLOAT fY=1)=0;
	virtual void	RotateYaw	(INT nIdx, FLOAT fDelta, FLOAT fSpeed)=0;
	virtual void	RotatePitch	(INT nIdx, FLOAT fDelta, FLOAT fSpeed)=0;
	virtual void	Zoom		(INT nIdx, FLOAT fDelta)=0;


	// For Third Person Camera
	virtual void	SetBasis(INT nIdx, D3DXVECTOR3*)=0;
	virtual const	D3DXVECTOR3* const GetBasis(INT nIdx) const=0;

	virtual void	SetEpsilonY(INT nIdx, FLOAT)=0;
	virtual FLOAT	GetEpsilonY(INT nIdx)		=0;

	virtual void	SetGap(INT nIdx, FLOAT)	=0;
	virtual FLOAT	GetGap(INT nIdx)		=0;

	// Camera Pointer
	virtual	ILcCam*	GetCameraPointer(INT nIdx) =0;
};




INT LcCam_CreateMuti(char* sCmd
				, ILcCamM** pData
				, void* p1=0		// Camera Number. NULLÀÌ¸é LC_MUTI_CAMERA_MAX¸¸Å­ ¸¸µê
				, void* p2=0		// No Use
				, void* p3=0		// No Use
				, void* p4=0		// No Use
				, void* p5=0		// No Use
				, void* p6=0		// No Use
				);






////////////////////////////////////////////////////////////////////////////////
// Entities

interface ILcEnt
{
	LC_CLASS_DESTROYER(	ILcEnt	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		FrameMove()=0;
	virtual void	Render()=0;

	virtual INT		Query(char* sCmd, void* pData)=0;
};


//
INT	LcEnt_Create(char* sCmd				// Command
				, ILcEnt** pData		//
				, void* p1=NULL			// Number or Etc...
				, void* p2=NULL			//
				, void* p3=NULL			//
				, void* p4=NULL			//
				);








////////////////////////////////////////////////////////////////////////////////
// M3D

namespace LcM
{

// Index Buffer Structure

// 16bit Index structure
struct VtxIdx
{
	union { struct { WORD a; WORD b; WORD c; };	WORD m[3];	};

	VtxIdx()						: a(0),b(1),c(2){}
	VtxIdx(WORD A,WORD B,WORD C)	: a(A),b(B),c(C){}
	VtxIdx(WORD* R)					: a(R[0]),b(R[1]),c(R[2]){}
	operator WORD* ()				{	return (WORD *) &a;			}
	operator const WORD* () const	{	return (const WORD *) &a;	}
};

// 32bit Index structure
struct VtxIdx32
{
	union { struct { DWORD a; DWORD b; DWORD c; }; DWORD m[3]; };

	VtxIdx32()						: a(0),b(1),c(2){}
	VtxIdx32(DWORD A,DWORD B,DWORD C):a(A),b(B),c(C){}
	VtxIdx32(DWORD* R)				: a(R[0]),b(R[1]),c(R[2]){}
	operator DWORD* ()				{	return (DWORD *) &a;		}
	operator const DWORD* () const	{	return (const DWORD *) &a;	}
};


// Vertex Buffer Structure

// Diffuse
struct VtxD
{
	FLOAT	p[3];
	DWORD	d;

	VtxD();
	VtxD(FLOAT X,FLOAT Y,FLOAT Z,DWORD D=0xFFFFFFFF);
	enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE) };
};

// Diffuse + Texture 1
struct VtxDUV1
{
	FLOAT	p[3];
	DWORD	d;
	FLOAT	u,v;
	
	VtxDUV1();
	VtxDUV1(FLOAT X,FLOAT Y,FLOAT Z,FLOAT U,FLOAT V,DWORD D=0xFFFFFFFF);
	enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1) };
};

// Normal
struct VtxN
{
	FLOAT	p[3];
	FLOAT	n[3];

	VtxN();
	VtxN(FLOAT X,FLOAT Y,FLOAT Z,FLOAT nX,FLOAT nY,FLOAT nZ);
	enum { FVF = (D3DFVF_XYZ|D3DFVF_NORMAL) };
};

// Texture1
struct VtxUV1
{
	FLOAT	p[3];
	FLOAT	u,v;

	VtxUV1();
	VtxUV1(FLOAT X,FLOAT Y,FLOAT Z, FLOAT U,FLOAT V);
	enum { FVF = (D3DFVF_XYZ|D3DFVF_TEX1) };
};

// Normal + Texture1
struct VtxNUV1
{
	FLOAT	p[3];
	FLOAT	n[3];
	FLOAT	u,v;

	VtxNUV1();
	VtxNUV1(FLOAT X,FLOAT Y,FLOAT Z,FLOAT nX,FLOAT nY,FLOAT nZ, FLOAT U,FLOAT V);
	enum { FVF = (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) };
};

// Normal + Diffuse + Texture1
struct VtxNDUV1
{
	FLOAT	p[3];
	FLOAT	n[3];
	DWORD	d;
	FLOAT	u,v;

	VtxNDUV1();
	VtxNDUV1(FLOAT X,FLOAT Y,FLOAT Z
		,	 FLOAT nX,FLOAT nY,FLOAT nZ
		,	 FLOAT U,FLOAT V,  DWORD D=0xFFFFFFFF);
	enum { FVF = (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1) };
};

// Normal + Diffuse + Texture2
struct VtxNDUV2
{
	FLOAT	p[3];
	FLOAT	n[3];
	DWORD	d;
	FLOAT	u0,v0;
	FLOAT	u1,v1;

	VtxNDUV2();
	VtxNDUV2(FLOAT X,FLOAT Y,FLOAT Z
		,	 FLOAT nX,FLOAT nY,FLOAT nZ
		,	 FLOAT U0,FLOAT V0, FLOAT U1,FLOAT V1
		,	 DWORD D=0xFFFFFFFF);
	enum { FVF = (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2) };
};


struct VtxNDUV3
{
	FLOAT	p[3];
	FLOAT	n[3];
	DWORD	d;
	FLOAT	u0,v0;
	FLOAT	u1,v1;
	FLOAT	u2,v2;

	VtxNDUV3();
	VtxNDUV3(FLOAT X,FLOAT Y,FLOAT Z
		,	 FLOAT nX,FLOAT nY,FLOAT nZ
		,	 FLOAT U0,FLOAT V0
		,	 FLOAT U1,FLOAT V1
		,	 FLOAT U2,FLOAT V2
		,	 DWORD D=0xFFFFFFFF);
	enum { FVF = (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3) };
};


struct VtxRHWD
{
	FLOAT p[4];
	DWORD d;

	VtxRHWD();
	VtxRHWD(FLOAT X,FLOAT Y,FLOAT Z,DWORD D=0xFFFFFFFF);
	enum {FVF =(D3DFVF_XYZRHW | D3DFVF_DIFFUSE), };
};


struct VtxRHWDUV1															// RHW + Diffuse + Texture 1
{
	FLOAT p[4];
	DWORD d;
	FLOAT u,v;

	VtxRHWDUV1();
	VtxRHWDUV1(FLOAT X,FLOAT Y,FLOAT Z,FLOAT U,FLOAT V, DWORD D=0xFFFFFFFF);

	enum { FVF = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1) };
};

};// namespace LcM



////////////////////////////////////////////////////////////////////////////////



// Index
interface ILcIdx
{
	LC_CLASS_DESTROYER(	ILcIdx	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)= 0;
	virtual void	Destroy()=0;

	virtual void*	GetIndex()=0;
	virtual DWORD	GetFMT()=0;
};


// Interface
interface ILcVtx
{
	LC_CLASS_DESTROYER(	ILcVtx	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual void	Render()=0;

	virtual void*	GetVertex()=0;
	virtual DWORD	GetFVF()=0;
	virtual UINT	GetStrid()=0;
	virtual INT		GetVtxNum()=0;

	virtual INT		Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)=0;
	virtual INT		Unlock()=0;

	virtual	void	SetTexture(INT nStage, void* pTx)=0;
	virtual	void	DrawPrimitive(DWORD PrimitiveType, UINT StartVertex, UINT PrimitiveCount)=0;

	virtual	void	SetPtType(DWORD dPtType)=0;
	virtual	void	SetPtNum(UINT iPtNum)=0;


	// Vertex SetUp( if Lock ppbdata is Null.
	virtual	void	SetPos		(INT nIdx, FLOAT X, FLOAT Y, FLOAT Z, FLOAT W=1.F)=0;
	virtual	void	SetNormal	(INT nIdx, FLOAT X, FLOAT Y, FLOAT Z)=0;

	/* rgba is 0~ 1.f */
	virtual	void	SetDiffuse	(INT nIdx, FLOAT R, FLOAT G, FLOAT B, FLOAT A)=0;
	virtual	void	SetUV		(INT nIdx, INT nUV, FLOAT U, FLOAT V)=0;
};


//Material
interface ILcMtl
{
	LC_CLASS_DESTROYER(	ILcMtl	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;

	virtual void	SetMaterial()=0;

	virtual void	SetAmbient(FLOAT* pVal/*D3DXCOLOR*/ )=0;
	virtual void	SetDiffuse(FLOAT* pVal/*D3DXCOLOR*/ )=0;
    virtual void	SetSpecular(FLOAT* pVal/*D3DXCOLOR*/ )=0;
    virtual void	SetEmissive(FLOAT* pVal/*D3DXCOLOR*/ )=0;
    virtual void	SetPower(FLOAT pVal)=0;				// Sharpness if specular highlight

	virtual FLOAT*	GetDiffuse()=0;
    virtual FLOAT*	GetAmbient()=0;
    virtual FLOAT*	GetSpecular()=0;
    virtual FLOAT*	GetEmissive()=0;
    virtual FLOAT	GetPower()=0;
};


//Lighting
interface ILcLgt
{
	LC_CLASS_DESTROYER(	ILcLgt	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;

	virtual void	SetLight(INT nIdx)=0;										// pDev->SetLight( nIdx,....)
	virtual void	SetLightEnable(BOOL nEnable)=0;								// pDev->LightEnable( nIdx,....)

	virtual void	SetLgtType(DWORD dType)=0;
	
	virtual void	SetLgtAmbient(FLOAT* pVal/*D3DXCOLOR*/ )=0;
	virtual void	SetLgtDiffuse(FLOAT* pVal/*D3DXCOLOR*/ )=0;
    virtual void	SetLgtSpecular(FLOAT* pVal/*D3DXCOLOR*/ )=0;

    virtual void	SetLgtPosition(FLOAT* pVal/*D3DXVECTOR3*/ )=0;
	virtual void	SetLgtDirection(FLOAT* pVal/*D3DXVECTOR3*/ )=0;

	virtual void	SetLgtRange(FLOAT pVal)=0;									// Cutoff range
    virtual void	SetLgtFalloff(FLOAT pVal)=0;								// Falloff

	virtual void	SetLgtAttenuation0(FLOAT pVal)=0;							// Constant attenuation
	virtual void	SetLgtAttenuation1(FLOAT pVal)=0;							// Linear attenuation
	virtual void	SetLgtAttenuation2(FLOAT pVal)=0;							// Quadratic attenuation
	virtual void	SetLgtTheta(FLOAT pVal)=0;									// Inner angle of spotlight cone
	virtual void	SetLgtPhi(FLOAT pVal)=0;									// Outer angle of spotlight cone


	virtual DWORD	GetLgtType()=0;
	
	virtual FLOAT*	GetLgtAmbient()=0;
	virtual FLOAT*	GetLgtDiffuse()=0;
    virtual FLOAT*	GetLgtSpecular()=0;

    virtual FLOAT*	GetLgtPosition()=0;
	virtual FLOAT*	GetLgtDirection()=0;

	virtual FLOAT	GetLgtRange()=0;											// Cutoff range
    virtual FLOAT	GetLgtFalloff()=0;											// Falloff

	virtual FLOAT	GetLgtAttenuation0()=0;										// Constant attenuation
	virtual FLOAT	GetLgtAttenuation1()=0;										// Linear attenuation
	virtual FLOAT	GetLgtAttenuation2()=0;										// Quadratic attenuation
	virtual FLOAT	GetLgtTheta()=0;											// Inner angle of spotlight cone
	virtual FLOAT	GetLgtPhi()=0;												// Outer angle of spotlight cone
};


// Mesh
interface ILcMsh
{
	LC_CLASS_DESTROYER(	ILcMsh	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual	void	Destroy()=0;

	virtual	INT		FrameMove()=0;
	virtual	void	Render()=0;

	virtual INT		Query(char* sCmd, void* pData)=0;

	virtual INT		Lock(UINT nGeo, UINT OffsetToLock, UINT SizeToLock, void** ppbData)=0;
	virtual INT		Unlock(UINT nGeo)=0;

	// Vertex SetUp( if Lock ppbdata is Null.
	virtual	void	SetPos		(INT nGeo, INT nVtx, FLOAT X, FLOAT Y, FLOAT Z, FLOAT W=1.F)=0;
	virtual	void	SetNormal	(INT nGeo, INT nVtx, FLOAT X, FLOAT Y, FLOAT Z)=0;

	// rgba is 0~ 1.f
	virtual	void	SetDiffuse	(INT nGeo, INT nVtx, FLOAT R, FLOAT G, FLOAT B, FLOAT A)=0;
	virtual	void	SetUV		(INT nGeo, INT nVtx, INT nUV, FLOAT U, FLOAT V)=0;

	virtual	void	GetPos		(INT nGeo, INT nVtx, FLOAT* X, FLOAT* Y, FLOAT* Z, FLOAT* W)=0;
	virtual	void	GetNormal	(INT nGeo, INT nVtx, FLOAT* X, FLOAT* Y, FLOAT* Z)=0;
	virtual	void	GetDiffuse	(INT nGeo, INT nVtx, FLOAT* R, FLOAT* G, FLOAT* B, FLOAT* A)=0;
	virtual	void	GetUV		(INT nGeo, INT nVtx, INT  nUV, FLOAT* U, FLOAT* V)=0;
};





// Index buffer
INT	LcM3d_CreateIdx(char* sCmd			// Command
				, ILcIdx** pData		//
				, void* p1				// Need D3D Device
				, void* p2				// Total Indices
				, void* p3=0			// Index memory copy source
				, void* p4=0			// No Use
				);



// Vertex Buffer
// Command Ex) "Diffuse", "Diffuse Texture1", "Normal Texture1", "Normal Diffuse Texture1"
// Primitive Type: "TRIANGLELIST", "TRIANGLESTRIP"
INT	LcM3d_CreateVtx(char* sCmd			// Command
				, ILcVtx** pData		//
				, void* p1				// Need D3D Device
				, void* p2				// Primitive Type
				, void* p3				// Primitive Count
				, void* p4				// Total Vertices
				, void* p5=0			// Vertex Memory Copy Source
				, void* p6=0			// Source Total Size
				, void* p7=0			// No Use
				, void* p8=0			// No Use
				, void* p9=0			// No Use
				);


// Command Ex) "PC"
INT	LcM3d_CreateMtl(char* sCmd			// Command
					, ILcMtl** pData	//
					, void* p1			// d3dDevice
					, void* p2			// Ambient
					, void* p3			// Diffuse
					, void* p4			// Specular
					, void* p5			// Emissive
					, void* p6			// Power
					);

// Command Ex) "PC"
INT	LcM3d_CreateLgt(char* sCmd			// Command
					, ILcLgt** pData	//
					, void* p1			// d3dDevice
					, void* p2			// Type
					, void* p3			// Ambient
					, void* p4			// Diffuse
					, void* p5			// Specular
					, void* p6			// Position
					, void* p7			// Direction
					, void* p8			// Etc array. {Range, Falloff, Attenuation0, Attenuation1, Attenuation2, Theta, Phi, }
					);


// Simple X-File Loader
INT LcM3d_CreateMsh(char* sCmd
				 , ILcMsh** pData
				 , void* p1				// LPDIRECT3DDEVICE9
				 , void* p2				// X-file Name
				 , void* p3=0			// No Use
				 , void* p4=0			// No Use
				 );





////////////////////////////////////////////////////////////////////////////////
// Lc 3D Model

struct TbndAB;

interface ILcMdl
{
	LC_CLASS_DESTROYER(	ILcMdl	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		FrameMove()=0;
	virtual void	Render()=0;

	virtual INT		Query(char* sCmd, void* pVal)=0;
	virtual INT		GetType()=0;
	virtual const char*	const GetName()const=0;
	virtual ILcMdl* GetOriginal()=0;

	virtual INT		SetAttrib(char* sCmd, void* pVal)=0;
	virtual INT		GetAttrib(char* sCmd, void* pVal)=0;

	virtual INT		SetPosition(FLOAT* float3)=0;
	virtual INT		SetRotation(FLOAT* float4, INT nOpt=LCM_ROT_XYZ)=0;		// XYZ: X,Y,Z Angle. Acm is AXIS which is Axis(x,y,z) and radian w.
	virtual INT		SetScaling(FLOAT* float3)=0;

	virtual INT		GetPosition(FLOAT* float3)=0;
	virtual INT		GetRotation(FLOAT* float4)=0;
	virtual INT		GetScaling(FLOAT* float3)=0;
	virtual	FLOAT	GetSortValue()	=0;
	virtual	const TbndAB* const	GetBndInf() const=0;

	enum ELcmConst
	{
		// Model Type
		LCM_NONE		= 0x00000000,
		LCM_BILL		= 0x00000001,
		LCM_SOLID		= 0x00000002,
		LCM_ACM			= 0x00000004,
		LCM_ASE			= 0x00000008,
		LCM_XFILE		= 0x00010000,

		// for Info...
		LCM_TEX_NAME	= 128,
		LCM_HEADER_SIZE = 256,
		LCM_FILE_NAME	= 128,

		// Rotation Type
		LCM_ROT_XYZ		= 0x00000000,
		LCM_ROT_AXIS	= 0x00000001,


		// Rendering Level
		LCM_RND_NONE	= 0x00000000,	// No Update and No Rendering
		LCM_RND_UPDATE	= 0x00000001,	// Only for Update
		LCM_RND_RENDER	= 0x00000002,	// Update and Rendering
		LCM_RND_ALL		= 0x000000FF,	// No Limit
	};


	struct _Tattach
	{
		INT		nI	;					// Geometry Index
		char*	sN	;					// Name
		FLOAT*	tm	;					// Transformation Matrix Pointer
		INT		nM	;					// Material
	};

	struct _Tframe
	{
		char	sN[64];					// Phase Name
		INT		nB	;					// Begin Frame
		INT		nE	;					// End Frame
		_Tframe();
		_Tframe(char* s, INT B, INT E);
	};

	struct _Tmtl
	{
		void*	dMtl	;				// D3DMATERIAL9 Pointer
		char*	sDif	;				// Diffuse Texture File
		char*	sNor	;				// Normal Texture File
		char*	sSpc	;				// Specular Texture File
		char*	sEms	;				// Emissive Texture File
	};


	struct XTrack						// X-File Track
	{
		FLOAT	fTrackPosition;
		FLOAT	fTrackSpeed;
	};
};


INT LcMdl_Create(char* sCmd
				 , ILcMdl** pData
				 , void* p1			// Original Source: Original Source°¡ NULLÀÌ¸é »õ·Î ¸¸µê
				 , void* p2 = 0		// Model File Name
				 , void* p3 = 0		// No Use
				 , void* p4 = 0		// No Use
				 );







typedef vector<ILcMdl* >		lsLcMdl;
typedef lsLcMdl::iterator		itLcMdl;


// Packages : ¸ðµ¨µé ¸®½ºÆ® ÀÚ·á±¸Á¶.
interface ILcmPack
{
	LC_CLASS_DESTROYER(	ILcmPack	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual	void	Destroy()=0;
	virtual	INT		FrameMove()=0;
	virtual	void	Render()=0;

	virtual INT		Query(char* sCmd, void* pVal)=0;

	virtual void	SetLcl(void* pLcl)=0;
	virtual void	AddObj(ILcMdl* pSrc
							, D3DXVECTOR3 vcP		// Position
							, void* p1		// Camera
							, void* p2		// Field
							, void* p3		// Field Local
							, void* p4		// Parent Package
							)=0;

	// For STL
	virtual	void	Push_Back(ILcMdl* pMdB)=0;
	virtual	INT		Size()=0;
	virtual	ILcMdl*	GetObj(int n)=0;
	virtual	INT		Erase(int n)=0;	
	virtual	INT		Erase(ILcMdl*)=0;	
};


typedef vector<ILcmPack* >		lsLcmPack;
typedef lsLcmPack::iterator		itLcmPack;


INT LcMdl_CreatePack(char* sCmd
				 , ILcmPack** pData
				 , void* p1			// LPDIRECT3DDEVICE9
				 , void* p2 = 0		// Local Field
				 , void* p3 = 0		// No Use
				 , void* p4 = 0		// No Use
				 );





// Cuboid Axis Aligned Bounding Box
//
//  Edges
//                 (Max)
//   y   5-----------7
//   |  /|          /|
//   | / |   z     / |
//   |/  |  /     /  |
//   1-----------3   |
//   |   |/      |   |
//   |   4-------|---6
//   |  /        |  /
//   | /         | /
//   |/          |/
//   0-----------2----- x
// (Min)

struct TbndAB
{
	struct _Tvtx
	{
		D3DXVECTOR3	p;
		DWORD		d;
		_Tvtx() : p(0,0,0), d(0xFFFFFFFF){}
	};
	
	_Tvtx		vcE[8];		// Edge

	D3DXVECTOR3	vcM;		// Max
	D3DXVECTOR3	vcN;		// Min
	D3DXVECTOR3	vcC;		// Center

	FLOAT		fX;			// Width
	FLOAT		fY;			// Height
	FLOAT		fZ;			// Depth
	FLOAT		fR;			// Radius
	void*		pOwn;		// Owner of Bound Box

	TbndAB();
	TbndAB(const D3DXVECTOR3& Min, const D3DXVECTOR3& Max, void* _pOwn=NULL);
	
	void			Set(const D3DXVECTOR3& Min, const D3DXVECTOR3& Max, void* _pOwn=NULL);
	void			RenderBox(void* pDev);
	void			SetOwner(void* _pOwn);
	void*			GetOwner();
	void			SetColor(DWORD dColor);

	D3DXVECTOR3*	GetMin() const;
	D3DXVECTOR3*	GetMax() const;
	D3DXVECTOR3*	GetCenter() const;
	FLOAT			GetWidth();
	FLOAT			GetHeight();
	FLOAT			GetDepth();
	FLOAT			GetRadius();
	DWORD			GetColor();
};


struct TbndAABB																	// Picking AABB
{
	D3DXVECTOR3	vPck;															// Get Position
	TbndAB*		pBnd;															// Bound Box Pointer
	FLOAT		fStlSrtR;														// Distance From Camera

	TbndAABB();
	TbndAABB(const D3DXVECTOR3& vcP, TbndAB* _pBnd, FLOAT _fD);
};

typedef vector<TbndAABB >		lsPkBndA;
typedef lsPkBndA::iterator		itPkBndA;


INT	LcxUtil_PickAABB(FLOAT* fDist												// Distance
					, D3DXVECTOR3* pvcOut										// In, Out: Picking Position
					, D3DXVECTOR3* pvcCamPos									// In: Camera Position
					, D3DXVECTOR3* pvcRayDir									// In: Ray Direction
					, TbndAB* pIn												// In: Bound Box for Test.
					);





////////////////////////////////////////////////////////////////////////////////
//

interface ILcSmd
{
	LC_CLASS_DESTROYER(	ILcSmd	);

	enum EMediaType
	{
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


	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;

	virtual INT		Query(char* sCmd, void* pData)=0;

	virtual DWORD	GetType()=0;
	virtual void	Play()=0;
	virtual void	Stop()=0;
	virtual void	Reset()=0;
	virtual void	Pause()=0;
	virtual void	SetVolume(LONG dVol)=0;
	virtual LONG	GetVolume()=0;
	virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*INFINITE*/)=0;
	virtual DWORD	GetStatus()=0;


	virtual void*	GetTexture()=0;
	virtual INT		GetVideoW()=0;
	virtual INT		GetVideoH()=0;
};


INT		LcSmd_DeviceInit(HWND hWnd);
void	LcSmd_DeviceClose();


INT		LcSmd_Create(char* sCmd
					, ILcSmd** pData
					, void* p1			// Wave File Name
					, void* p2=0		// Sound Format(EMediaType)
					, void* p3=0		// Flag for 3D sound
					, void* p4=0		// Event ID
					, void* p5=0		// LPDIRECT3DDEVICE9 pointer for Direct Show
					, void* p6=0		// No Use
					, void* p7=0		// No Use
					, void* p8=0		// No Use
					);


void*	LcSmd_GetDirectSound();			// Get IDirectSound8 pointer
void*	LcSmd_GetDirectMusicLoader();	// Get Dierect Music Loader
HWND	LcSmd_GetHwnd()		;			// Get Window Handle about to use Sound and Music.





class CLcxDShow : public ILcSmd
{
public:
	class ILcxDShowClip
	{
	public:
		ILcxDShowClip(){};
		virtual ~ILcxDShowClip(){};

//		virtual INT		Create(void* pDev, INT nEventID, char* sFile)=0;
		virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
		virtual void	Destroy()=0;
		virtual INT		ProcessEvent()=0;

	public:
		virtual INT		Run()=0;
		virtual INT		Stop()=0;
		virtual INT		Reset()=0;

		virtual void*	GetTexture()=0;
		virtual INT		GetVideoW()=0;
		virtual INT		GetVideoH()=0;
		virtual	INT		GetEventID()=0;

		virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*INFINITE*/)=0;


		virtual void	SetScnPosX(INT nVal)=0;
		virtual void	SetScnPosY(INT nVal)=0;
		virtual void	SetScnWidth(INT nVal)=0;
		virtual	void	SetScnHeight(INT nVal)=0;
		virtual	void	SetDiffuse(DWORD nVal)=0;
		virtual INT		Draw()=0;
	};

protected:
	ILcxDShowClip*		m_pVid;

public:
	CLcxDShow();
	virtual ~CLcxDShow();

	virtual INT		Create(void* p1=0, void* p2=0, void* p3=0, void* p4=0);
	virtual void	Destroy();
	virtual INT		Query(char* sCmd, void* pData);

	virtual DWORD	GetType();
	virtual void	Play();
	virtual void	Stop();
	virtual void	Reset();
	virtual void	Pause();
	virtual void	SetVolume(LONG dVol);
	virtual LONG	GetVolume();
	virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*INFINITE*/);
	virtual DWORD	GetStatus();

	virtual void*	GetTexture();
	virtual INT		GetVideoW();
	virtual INT		GetVideoH();

protected:
	INT				ProcessEvent();
	INT				GetEventID();
};


INT		LcSmd_CreateDShowFromFile(ILcSmd** ppDShow
								  , void* pDev			// LPDIRECT3DDEVICE9 pointer
								  , INT nEvntId
								  ,	char* sFile);




#endif


