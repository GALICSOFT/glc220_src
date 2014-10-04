// Implementation of the CLcCamA class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <windows.h>
#include <stdio.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"
#include "LnCam.h"
#include "LnCamA.h"



CLcCamA::CLcCamA()
{
	strcpy(m_sN, "LC_CAMERA");
	m_eType		= LC_CAM_ALL;
}

CLcCamA::~CLcCamA()
{

}


INT CLcCamA::Create(void* p1, void* p2, void* p3, void* p4)
{
	if(p1)
	{
		char* s	=(char*)p1;
		strcpy(m_sN, s);
	}

	if(p2)
	{
		LcCam*	t=(LcCam*)p2;

		m_fScnW	=t->fScnW;
		m_fScnH	=t->fScnH;
		m_fAs	= m_fScnW/ m_fScnH;
		m_fFv	=t->fFov;
		m_fNr	=t->fNear;
		m_fFr	=t->fFar;

		m_vcEye.x	= t->vEyeX;
		m_vcEye.y	= t->vEyeY;
		m_vcEye.z	= t->vEyeZ;

		m_vcLook.x	= t->vLookX;
		m_vcLook.y	= t->vLookY;
		m_vcLook.z	= t->vLookZ;

		m_vcUp.x	= t->vUpX;
		m_vcUp.y	= t->vUpY;
		m_vcUp.z	= t->vUpZ;
	}

	
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();
	LPDIRECT3DSURFACE9	pSfc =NULL;
	D3DSURFACE_DESC		desc;
	pDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSfc );
	pSfc->GetDesc( &desc );
	pSfc->Release();

	m_fScnW	= FLOAT(desc.Width);
	m_fScnH	= FLOAT(desc.Height);
	m_fAs	= m_fScnW/ m_fScnH;


	Update();

	return 0;
}


INT CLcCamA::FrameMove()
{
	Update();

	SetTransformViw();
	SetTransformPrj();

	return 0;
}

void CLcCamA::MoveSideward(FLOAT fSpeed)
{
	D3DXVECTOR3 tmp(m_mtViw._11, 0, m_mtViw._31);
	D3DXVec3Normalize(&tmp,&tmp);

	m_vcBasis+= tmp * fSpeed;
	m_vcEye  += tmp * fSpeed;
	m_vcLook += tmp * fSpeed;
}


void CLcCamA::MoveForward(FLOAT fSpeed, FLOAT fY)
{
	D3DXVECTOR3 tmp(m_mtViw._13, m_mtViw._23*fY, m_mtViw._33);
	D3DXVec3Normalize(&tmp,&tmp);

	m_vcBasis+= tmp * fSpeed;
	m_vcEye  += tmp * fSpeed;
	m_vcLook += tmp * fSpeed;
}



void CLcCamA::RotateYaw(FLOAT fDelta,FLOAT fSpeed)
{
	m_fYaw		= D3DXToRadian(fDelta*fSpeed);

	D3DXMATRIX	rot;
	D3DXVECTOR3	vcZ = m_vcLook - m_vcEye;

	if(LC_CAM_FIRST >= m_eType)
	{
		D3DXMatrixRotationY(&rot, m_fYaw);
		D3DXVec3TransformCoord(&vcZ, &vcZ, &rot);
		D3DXVec3TransformCoord(&m_vcUp, &m_vcUp, &rot);
		
		m_vcLook	= vcZ + m_vcEye;
		D3DXMatrixLookAtLH(&m_mtViw, &m_vcEye, &m_vcLook, &m_vcUp);
	}
	
	else if(LC_CAM_THIRD <= m_eType)
	{
		D3DXVECTOR3 vcZ;
		D3DXVECTOR3 vcY;
		D3DXVECTOR3 vcX;

		D3DXMATRIX	rtY;
		D3DXMATRIX	rtX;

		// 월드 좌표 y 축에 대한 회전
		D3DXMatrixRotationY(&rtY, m_fYaw);

		vcZ = m_vcEye - m_vcLook;
		vcY = D3DXVECTOR3(m_mtViw._12, m_mtViw._22, m_mtViw._32);

		D3DXVec3TransformCoord(&vcZ, &vcZ, &rtY);
		D3DXVec3TransformCoord(&vcY, &vcY, &rtY);

		m_vcEye	= m_vcLook + vcZ;
		m_vcUp	= vcY;
		D3DXMatrixLookAtLH(&m_mtViw, &m_vcEye, &m_vcLook, &m_vcUp);
	}
}


void CLcCamA::RotatePitch(FLOAT fDelta,FLOAT fSpeed)
{
	m_fPitch	= D3DXToRadian(fDelta*fSpeed);

	D3DXMATRIX	rot;
	D3DXVECTOR3 vcZ = m_vcLook - m_vcEye;
	D3DXVECTOR3 vcX = D3DXVECTOR3(m_mtViw._11, m_mtViw._21, m_mtViw._31);

	if(LC_CAM_FIRST >= m_eType)
	{
		D3DXMatrixRotationAxis(&rot, & vcX, m_fPitch);
		D3DXVec3TransformCoord(&vcZ, &vcZ, &rot);
		D3DXVec3TransformCoord(&m_vcUp, &m_vcUp, &rot);

		m_vcLook	= vcZ + m_vcEye;
	}
	
	else if(LC_CAM_THIRD <= m_eType)
	{
		D3DXVECTOR3 vcZ;
		D3DXVECTOR3 vcY;
		D3DXVECTOR3 vcX;

		D3DXMATRIX	rtY;
		D3DXMATRIX	rtX;

		// 카메라의 x 축에 대한 회전
		vcZ = m_vcEye - m_vcLook;
		vcY = D3DXVECTOR3(m_mtViw._12, m_mtViw._22, m_mtViw._32);
		vcX = D3DXVECTOR3(m_mtViw._11, m_mtViw._21, m_mtViw._31);

		D3DXMatrixRotationAxis(&rtX, & vcX, m_fPitch);
		D3DXVec3TransformCoord(&vcZ, &vcZ, &rtX);
		D3DXVec3TransformCoord(&vcY, &vcY, &rtX);

		m_vcEye	= m_vcLook + vcZ;
		m_vcUp	= vcY;
		D3DXMatrixLookAtLH(&m_mtViw, &m_vcEye, &m_vcLook, &m_vcUp);
	}
}


void CLcCamA::Zoom(FLOAT v)
{
	if(v !=0.f)
	{
		m_fGap += v;

		if(m_fGap<2.f)
			m_fGap = 2.f;

		D3DXVECTOR3 vcZ	= D3DXVECTOR3(m_mtViw._13, m_mtViw._23, m_mtViw._33);
		m_vcEye			= m_vcLook - vcZ* m_fGap;
	}
}