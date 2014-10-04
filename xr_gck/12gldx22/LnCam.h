// Interface for the CLcCam class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LcCam_H_
#define _LcCam_H_

class CLcCam : public ILcCam
{
protected:
	char		m_sN[64];														// Camera Name
	ELcCam		m_eType;														// Camera Type
	
	// For View Matrix
	D3DXVECTOR3	m_vcEye;														// Camera position
	D3DXVECTOR3	m_vcLook;														// Look vector
	D3DXVECTOR3	m_vcUp;															// up vector

	// For Projection Matrix
	FLOAT		m_fScnW;														// Screen Width
	FLOAT		m_fScnH;														// Screen Height
	FLOAT		m_fFv;															// Field of View
	FLOAT		m_fAs;															// Aspect
	FLOAT		m_fNr;															// Near
	FLOAT		m_fFr;															// Far
	
	FLOAT		m_fYaw;
	FLOAT		m_fPitch;

	D3DXMATRIX	m_mtViw;														// View Matrix
	D3DXMATRIX	m_mtViwI;														// View Inverse Matrix
	D3DXMATRIX	m_mtBill;
	D3DXMATRIX	m_mtPrj;														// Projection Matrix
	D3DXMATRIX	m_mtVP;															// m_mtViw * m_mtPrj;
	D3DXMATRIX	m_mtVPI;														// Inverse Matrix of m_mtVwPj
	
	D3DXVECTOR3	m_vcX;															// Camera Axis X
	D3DXVECTOR3	m_vcY;															// Camera Axis Y
	D3DXVECTOR3	m_vcZ;															// Camera Axis Z

	D3DXPLANE	m_plFrsm[6];													// Near, Far, Left, Right, Top, Bottom
	D3DXVECTOR3	m_vcFrsm[8];													// Hexagon Points in World coordinate.

	FLOAT		m_fSpeed;														// Camera Move Speed

	//	Hexagon Points Index
	//
	//     7----------6
	//	  /|         /|
	//	 / |        / |
	//	3--+-------2  |
	//	|  |       |  |
	//	|  4-------+--5
	//	| /        | /
	//	|/         |/
	//	0----------1


	// For Third Person
	D3DXVECTOR3		m_vcBasis;			// Look Pos
	FLOAT			m_fEpslnY;			// Epsilon Y
	FLOAT			m_fGap;				// Distance Between Camera and Basis

	

public:
	CLcCam();
	virtual ~CLcCam();


	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		FrameMove();
	virtual INT		Query(char* sCmd, void* pData);

public:
	void	SetName(char* sN)	;
	char*	GetName()			;
	
	void	SetType(INT	eType)	;
	ELcCam	GetType()			;
	
	const D3DXMATRIX* const GetMatrixViw()const;
	const D3DXMATRIX* const GetMatrixPrj()const;
	const D3DXMATRIX* const GetMatrixViwI()const;
	const D3DXMATRIX* const	GetMatrixBill()const;
	const D3DXMATRIX* const	GetMatrixViwPrj()const;
	const D3DXMATRIX* const	GetMatrixViwPrjI()const;
	
	const D3DXVECTOR3* const GetEye()const;
	const D3DXVECTOR3* const GetLook()const;
	const D3DXVECTOR3* const GetUp()const;

	void	SetEye(D3DXVECTOR3*);
	void	SetLook(D3DXVECTOR3*);
	void	SetUp(D3DXVECTOR3*)	;
	
	FLOAT	GetYaw()			;
	FLOAT	GetPitch()			;
	
	void	SetFov(FLOAT)		;
	void	SetScnW(FLOAT)		;
	void	SetScnH(FLOAT)		;
	void	SetNear(FLOAT)		;
	void	SetFar(FLOAT)		;

	FLOAT	GetFov()			;
	FLOAT	GetScnW()			;
	FLOAT	GetScnH()			;
	FLOAT	GetAspect()			;
	FLOAT	GetNear()			;
	FLOAT	GetFar()			;

	const D3DXVECTOR3* const GetAxisX();
	const D3DXVECTOR3* const GetAxisY();
	const D3DXVECTOR3* const GetAxisZ();

	const D3DXPLANE* const GetFrustumPlanes();
	const D3DXVECTOR3* const GetFrustumPoints();
		
	void	SetParamView(D3DXVECTOR3* vcEye, D3DXVECTOR3* vcLook, D3DXVECTOR3* vcUp);
	void	SetParamProj(FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar);
	
	void	SetTransformViw();
	void	SetTransformPrj();

	// Pick Ray direction
	D3DXVECTOR3	GetDirectionOfMouse(FLOAT fMouseX, FLOAT fMouseY);

	// Individual Camera
	void	SetMatrixViw(D3DXMATRIX*);
	void	SetMatrixPrj(D3DXMATRIX*);

	void	MoveSideward(FLOAT fSpeed);
	void	MoveForward	(FLOAT fSpeed, FLOAT fY=1);
	void	RotateYaw	(FLOAT fDelta, FLOAT fSpeed);
	void	RotatePitch	(FLOAT fDelta, FLOAT fSpeed);
	void	Zoom(FLOAT);


	// For Third Person Camera
	void	SetBasis(D3DXVECTOR3*);
	const	D3DXVECTOR3* const GetBasis() const;

	void	SetEpsilonY(FLOAT)	;
	FLOAT	GetEpsilonY()		;

	void	SetGap(FLOAT)		;
	FLOAT	GetGap()			;

protected:
	void	Update();

};

#endif

