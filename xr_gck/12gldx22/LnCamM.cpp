// Implementation of the CLcCamM class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <windows.h>
#include <stdio.h>
#include <d3dx9.h>

#include "LnxEntity.h"
#include "LnCam.h"
#include "LnCamA.h"

#include "LnCamM.h"



CLcCamM::CLcCamM()
{
	for(int i=0; i<LC_MUTI_CAMERA_MAX; ++i)
		m_pCam[i]	= NULL;
}


CLcCamM::~CLcCamM()
{
	Destroy();
}



void CLcCamM::Destroy()
{
	for(int i=0; i<LC_MUTI_CAMERA_MAX; ++i)
	{
		delete m_pCam[i];
		m_pCam[i]	= NULL;
	}
}


INT CLcCamM::Create(void* p1, void*, void*, void*)
{
	INT		i =0;
	char	sName[64];

	for(i=0; i<LC_MUTI_CAMERA_MAX; ++i)
	{
		sprintf(sName, "LcCamera: %d", i);

		m_pCam[i] = new CLcCamA;
	
		if(FAILED(m_pCam[i]->Create(sName, NULL, p1)))
			return -1;
	}

	return 0;
}


INT CLcCamM::FrameMove()
{
	int i=0;

	for(i=0; i<LC_MUTI_CAMERA_MAX; ++i)
		m_pCam[i]->FrameMove();

	return 0;
}



INT  CLcCamM::Query(char*, void*)
{
	return -1;
}



void CLcCamM::SetName(INT nIdx, char* sN)
{
}


char* CLcCamM::GetName(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;

	return m_pCam[nIdx]->GetName();
}



void CLcCamM::SetType(INT nIdx, INT eType)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetType(eType);
}


INT CLcCamM::GetType(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return -1;
	
	return m_pCam[nIdx]->GetType();
}



const D3DXMATRIX*  const CLcCamM::GetMatrixViw(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetMatrixViw();
}


const D3DXMATRIX*  const CLcCamM::GetMatrixPrj(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetMatrixPrj();
}


const D3DXMATRIX*  const CLcCamM::GetMatrixViwI(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetMatrixViwI();
}


const D3DXMATRIX*  const CLcCamM::GetMatrixBill(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetMatrixBill();
}


const D3DXMATRIX*  const CLcCamM::GetMatrixViwPrj(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetMatrixViwPrj();
}


const D3DXMATRIX*  const CLcCamM::GetMatrixViwPrjI(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetMatrixViwPrjI();
}



const D3DXVECTOR3* const CLcCamM::GetEye(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetEye();
}


const D3DXVECTOR3* const CLcCamM::GetLook(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetLook();
}


const D3DXVECTOR3* const CLcCamM::GetUp(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetUp();
}



void CLcCamM::SetEye(INT nIdx, D3DXVECTOR3* pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetEye(pVal);
}


void CLcCamM::SetLook(INT nIdx, D3DXVECTOR3* pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetLook(pVal);
}


void CLcCamM::SetUp(INT nIdx, D3DXVECTOR3* pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetUp(pVal);
}



FLOAT CLcCamM::GetYaw(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetYaw();
}


FLOAT CLcCamM::GetPitch(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetPitch();
}



void CLcCamM::SetFov(INT nIdx, FLOAT pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetFov(pVal);
}


void CLcCamM::SetScnW(INT nIdx, FLOAT pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetScnW(pVal);
}


void CLcCamM::SetScnH(INT nIdx, FLOAT pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetScnH(pVal);
}


void CLcCamM::SetNear(INT nIdx, FLOAT pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetNear(pVal);
}


void CLcCamM::SetFar(INT nIdx, FLOAT pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetFar(pVal);
}



FLOAT CLcCamM::GetFov(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetFov();
}


FLOAT CLcCamM::GetScnW(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetScnW();
}


FLOAT CLcCamM::GetScnH(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetScnH();
}


FLOAT CLcCamM::GetAspect(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetAspect();
}


FLOAT CLcCamM::GetNear(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetNear();
}


FLOAT CLcCamM::GetFar(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetFar();
}



const D3DXVECTOR3* const CLcCamM::GetAxisX(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetAxisX();
}


const D3DXVECTOR3* const CLcCamM::GetAxisY(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetAxisY();
}


const D3DXVECTOR3* const CLcCamM::GetAxisZ(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetAxisZ();
}



const D3DXPLANE*   const CLcCamM::GetFrustumPlanes(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetFrustumPlanes();
}


const D3DXVECTOR3* const CLcCamM::GetFrustumPoints(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetFrustumPoints();
}



void CLcCamM::SetParamView(INT nIdx, D3DXVECTOR3* Eye, D3DXVECTOR3* Look, D3DXVECTOR3* Up)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetParamView(Eye, Look, Up);
}


void CLcCamM::SetParamProj(INT nIdx, FLOAT fFov, FLOAT fScnW, FLOAT fScnH, FLOAT fNear, FLOAT fFar)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetParamProj(fFov, fScnW, fScnH, fNear, fFar);
}



void CLcCamM::SetTransformViw(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetTransformViw();
}


void CLcCamM::SetTransformPrj(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetTransformPrj();
}



D3DXVECTOR3 CLcCamM::GetDirectionOfMouse(INT nIdx, FLOAT fMouseX, FLOAT fMouseY)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	
	return m_pCam[nIdx]->GetDirectionOfMouse(fMouseX, fMouseY);
}



void CLcCamM::SetMatrixViw(INT nIdx, D3DXMATRIX* pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetMatrixViw(pVal);
}


void CLcCamM::SetMatrixPrj(INT nIdx, D3DXMATRIX* pVal)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetMatrixPrj(pVal);
}



void CLcCamM::MoveSideward(INT nIdx, FLOAT fSpeed)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->MoveSideward(fSpeed);
}


void CLcCamM::MoveForward(INT nIdx, FLOAT fSpeed, FLOAT fY)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->MoveForward(fSpeed, fY);
}


void CLcCamM::RotateYaw(INT nIdx, FLOAT fDelta, FLOAT fSpeed)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->RotateYaw(fDelta, fSpeed);
}


void CLcCamM::RotatePitch(INT nIdx, FLOAT fDelta, FLOAT fSpeed)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->RotatePitch(fDelta, fSpeed);
}


void CLcCamM::Zoom(INT nIdx, FLOAT fDelta)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->Zoom(fDelta);
}



// For Third Person Camera
void CLcCamM::SetBasis(INT nIdx, D3DXVECTOR3* vcP)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetBasis(vcP);
}

const D3DXVECTOR3* const CLcCamM::GetBasis(INT nIdx) const
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx]->GetBasis();
}

void CLcCamM::SetEpsilonY(INT nIdx, FLOAT fY)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetEpsilonY(fY);
}

FLOAT CLcCamM::GetEpsilonY(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetEpsilonY();
}

void CLcCamM::SetGap(INT nIdx, FLOAT fG)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return;
	
	m_pCam[nIdx]->SetGap(fG);
}

FLOAT CLcCamM::GetGap(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return FLT_MAX;
	
	return m_pCam[nIdx]->GetGap();
}



ILcCam* CLcCamM::GetCameraPointer(INT nIdx)
{
	if(nIdx>=LC_MUTI_CAMERA_MAX)
		return NULL;
	
	return m_pCam[nIdx];
}



INT LcCam_CreateMuti(char* sCmd
				, ILcCamM** pData
				, void* p1			// Camera Number. NULL이면 LC_MUTI_CAMERA_MAX만큼 만듦
				, void* p2			// No Use
				, void* p3			// No Use
				, void* p4			// No Use
				, void* p5			// No Use
				, void* p6			// No Use
				)
{
	(*pData) = NULL;

	CLcCamM* pObj = new CLcCamM;

	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}