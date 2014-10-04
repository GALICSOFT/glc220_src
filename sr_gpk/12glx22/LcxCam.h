// Interface for the LCX_Cam class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxCam_H_
#define _LcxCam_H_


// Single Camera
class CLcxCam : public ILCX_Camera
{
protected:
	// For View Matrix
	LCXVECTOR3	m_vcEye;			// Camera position
	LCXVECTOR3	m_vcLook;			// Look vector
	LCXVECTOR3	m_vcUp;				// up vector

	// For Projection Matrix
	FLOAT		m_fFv;				// Field of View
	FLOAT		m_fAs;				// Aspect
	FLOAT		m_fNr;				// Near
	FLOAT		m_fFr;				// Far

	FLOAT		m_fYaw;
	FLOAT		m_fPitch;

	LCXMATRIX	m_mtViw;			// View Matrix
	LCXMATRIX	m_mtViwI;			// View Inverse Matrix
	LCXMATRIX	m_mtBill;
	LCXMATRIX	m_mtPrj;			// Projection Matrix
	LCXMATRIX	m_mtVP;				// m_mtViw * m_mtPrj;
	LCXMATRIX	m_mtVPI;			// Inverse Matrix of m_mtVwPj

	LCXVECTOR3	m_vcAxis[3];		// Camera Axis X, Y, Z

	LCXPLANE	m_plFrsm[6];		// Near, Far, Left, Right, Top, Bottom
	LCXVECTOR3	m_vcFrsm[8];		// Hexagon Points in World coordinate.

public:
	CLcxCam();
	virtual ~CLcxCam();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		GetType()		{	return LCX_CAM_NONE;	}


	// Individual Camera
	virtual void	SetupMatrixViw(const LC_MATRIX* mtViw){	m_mtViw = *mtViw;	}
	virtual void	SetupMatrixPrj(const LC_MATRIX* mtPrj){	m_mtPrj = *mtPrj;	}

	virtual void	SetEye (const LC_FLOAT3* v)	{	m_vcEye  = *v;	}
	virtual void	SetLook(const LC_FLOAT3* v)	{	m_vcLook = *v;	}
	virtual void	SetUp  (const LC_FLOAT3* v)	{	m_vcUp   = *v;	}

	// Get value
	virtual void	GetParamView(LC_FLOAT3* Eye, LC_FLOAT3* Look, LC_FLOAT3* Up);
	virtual void	GetParamProj(FLOAT* Fov, FLOAT* ScnW, FLOAT* ScnH, FLOAT* Near, FLOAT* Far);

	virtual const LC_MATRIX* const GetMatrixViw() {	return &m_mtViw;	}
	virtual const LC_MATRIX* const GetMatrixPrj() {	return &m_mtPrj;	}
	virtual const LC_FLOAT3* const GetEye()  	{	return &m_vcEye;	}
	virtual const LC_FLOAT3* const GetLook() 	{	return &m_vcLook;	}
	virtual const LC_FLOAT3* const GetUp()   	{	return &m_vcUp;		}


	virtual const LC_FLOAT3* const GetAxisX()	{	return &m_vcAxis[0];	}
	virtual const LC_FLOAT3* const GetAxisY()	{	return &m_vcAxis[1];	}
	virtual const LC_FLOAT3* const GetAxisZ()	{	return &m_vcAxis[2];	}

	virtual const LC_FLOAT4* const GetFrustumPlanes(){	return &m_plFrsm[0];	}
	virtual const LC_FLOAT3* const GetFrustumPoints(){	return &m_vcFrsm[0];	}


	// Action
public:
	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	void	Destroy();
};


#endif


