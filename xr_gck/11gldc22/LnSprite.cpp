// Implementation of the CLcSprite class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "LnBase.h"
#include "LnSprite.h"



CLcSprite::CLcSprite()
{
}

CLcSprite::~CLcSprite()
{
	CLcSprite::Destroy();
}


void CLcSprite::Destroy()
{
}


INT CLcSprite::Create(void* p1, void*, void*, void*)
{
	HRESULT hr=0;
	INT		i=0;
	
	for(i=0; i<4; ++i)
	{
		m_pVtx[i].p.z = (	0.f	);
		m_pVtx[i].p.w = (	1.f	);
	}


	return 0;
}


 INT CLcSprite::Begin(DWORD dVal)	{	return 0;	}
INT CLcSprite::End(DWORD dVal)	{	return 0;	}
INT CLcSprite::OnResetDevice()	{	return 0;	}
INT CLcSprite::OnLostDevice()	{	return 0;	}



INT CLcSprite::Draw(	void*		_tTx			// 0 Stage Texture(<==ILC_Texture)
					  , const RECT*	Rct				// Draw Region 0 State Texture
					  , void* _Pos					// Position
					  , DWORD dColor				// color
					  , DWORD dFilter)				// Filter
{
	if(!_tTx)
		return -1;

	PDTX	pTex = NULL;
	PLC_TEXTURE pLcTx	= (PLC_TEXTURE)_tTx;		// Image Information

	DWORD	dTxW0	= pLcTx->GetWidth();			// Get Width
	DWORD	dTxH0	= pLcTx->GetHeight();			// Get Height
	DWORD	dTxW1	= dTxW0;
	DWORD	dTxH1	= dTxH0;


	// Get Texture Pointer

	if(pLcTx)
		pTex	= (PDTX)pLcTx->GetPointer();		// Get D3DTexture Pointer;
	

	// Get Transform Infomation

	VEC2	vcScl(1.F, 1.F);
	VEC2	vcPos(0.F, 0.F);

	FLOAT	fX0		= FLOAT(dTxW0);
	FLOAT	fY0		= FLOAT(dTxH0);
	FLOAT	fX1		= 0;
	FLOAT	fY1		= 0;

	FLOAT	fMxW0	= (FLOAT) LcMath_MakePower2(dTxW0);
	FLOAT	fMxH0	= (FLOAT) LcMath_MakePower2(dTxH0);
	FLOAT	fMxW1	= 0;
	FLOAT	fMxH1	= 0;


	if(Rct)
	{
		fX0		= FLOAT(Rct->right - Rct->left);
		fY0		= FLOAT(Rct->bottom- Rct->top );

		m_pVtx[0].u0	= (Rct->left+0.5f)/(fMxW0);
		m_pVtx[0].v0	= (Rct->top +0.5f)/(fMxH0);

		m_pVtx[3].u0	= (Rct->right -0.5F)/(fMxW0);
		m_pVtx[3].v0	= (Rct->bottom-0.5F)/(fMxH0);
	}
	else
	{
		m_pVtx[0].u0	= 0.f;
		m_pVtx[0].v0	= 0.f;

		m_pVtx[3].u0	= 1.F;
		m_pVtx[3].v0	= 1.F;
	}



	m_pVtx[1].u0	= m_pVtx[3].u0;
	m_pVtx[1].v0	= m_pVtx[0].v0;

	m_pVtx[2].u0	= m_pVtx[0].u0;
	m_pVtx[2].v0	= m_pVtx[3].v0;


	if(_Pos)
	{
		FLOAT*	vPos = (FLOAT*)_Pos;
		vcPos[0]= vPos[0];
		vcPos[1]= vPos[1];
	}


	m_pVtx[0].p.x = vcPos[0]	-0.5f;
	m_pVtx[0].p.y = vcPos[1]	-0.5f;

	m_pVtx[1].p.x = m_pVtx[0].p.x + fX0	;
	m_pVtx[1].p.y = m_pVtx[0].p.y		;

	m_pVtx[2].p.x = m_pVtx[0].p.x		;
	m_pVtx[2].p.y = m_pVtx[0].p.y + fY0	;

	m_pVtx[3].p.x = m_pVtx[0].p.x + fX0	;
	m_pVtx[3].p.y = m_pVtx[0].p.y + fY0	;


	if(fX0<0.f)
	{
		FLOAT	_t = fabsf(fX0);
		for(int i=0; i<4; ++i)
		{
			m_pVtx[i].p.x += _t;
		}
	}

	if(fY0<0.f)
	{
		FLOAT	_t = fabsf(fY0);
		for(int i=0; i<4; ++i)
		{
			m_pVtx[i].p.y += _t;
		}
	}


	m_pVtx[0].d = dColor;
	m_pVtx[1].d = dColor;
	m_pVtx[2].d = dColor;
	m_pVtx[3].d = dColor;


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);
	pDev->SetRenderState( D3DRS_DITHERENABLE,		FALSE);
	pDev->SetRenderState( D3DRS_ZENABLE,			FALSE);
	pDev->SetRenderState(D3DRS_LIGHTING,			FALSE);
	pDev->SetRenderState( D3DRS_SPECULARENABLE,	FALSE);
	
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
	
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, dFilter);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dFilter);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);


	pDev->SetTexture(0, pTex);


	pDev->SetFVF(CLcSprite::VDRHW::FVF);
	pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_pVtx, sizeof (CLcSprite::VDRHW));


	pDev->SetTexture(0, NULL);

	pDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	return 0;
}






INT CLcSprite::Draw(	void* _tTx0					// 0 Stage Texture(<==ILC_Texture)
					  , void* _tTx1					// 1 Stage Texture(<==ILC_Texture)
					  , const RECT* _Rc0			// Draw Region 0 State Texture
					  , const RECT* _Rc1			// Draw Region 1 State Texture
					  , void* _Scl					// Scaling
					  , void* _Pos					// Position
					  , DWORD dColor				// color
					  , DWORD dFilter)				// Filter
{
	if(!_tTx0)
		return -1;

	PDTX	pTex0 = NULL;
	PDTX	pTex1 = NULL;

	PLC_TEXTURE pLcTx0	= (PLC_TEXTURE)_tTx0;				// Image Information
	PLC_TEXTURE pLcTx1	= (PLC_TEXTURE)_tTx1;				// Image Information

	DWORD	dTxW0	= pLcTx0->GetWidth();			// Get Width
	DWORD	dTxH0	= pLcTx0->GetHeight();			// Get Height
	DWORD	dTxW1	= dTxW0;
	DWORD	dTxH1	= dTxH0;

	DWORD	dTxC0	= pLcTx0->GetColorKey();		// Get Creation color Key
	DWORD	dTxF0	= pLcTx0->GetFilter();			// Get Creation Filter


	// Get Texture Pointer

	if(pLcTx0)
		pTex0	= (PDTX)pLcTx0->GetPointer();		// Get D3DTexture Pointer;
	
	if(pLcTx1)
	{
		pTex1	= (PDTX)pLcTx1->GetPointer();

		dTxW1	= pLcTx1->GetWidth();
		dTxH1	= pLcTx1->GetHeight();
	}

	// Get Transform Infomation

	VEC2	vcScl(1.F, 1.F);
	VEC2	vcPos(0.F, 0.F);

	FLOAT	fX0		= FLOAT(_Rc0->right - _Rc0->left);
	FLOAT	fY0		= FLOAT(_Rc0->bottom- _Rc0->top );
	FLOAT	fX1		= 0;
	FLOAT	fY1		= 0;

	FLOAT	fMxW0	= (FLOAT) LcMath_MakePower2(dTxW0);
	FLOAT	fMxH0	= (FLOAT) LcMath_MakePower2(dTxH0);
	FLOAT	fMxW1	= 0;
	FLOAT	fMxH1	= 0;



	m_pVtx[0].u0	= (_Rc0->left+0.5f)/(fMxW0);
	m_pVtx[0].v0	= (_Rc0->top +0.5f)/(fMxH0);

	m_pVtx[3].u0	= (_Rc0->right -0.5F)/(fMxW0);
	m_pVtx[3].v0	= (_Rc0->bottom-0.5F)/(fMxH0);

	m_pVtx[1].u0	= m_pVtx[3].u0;
	m_pVtx[1].v0	= m_pVtx[0].v0;

	m_pVtx[2].u0	= m_pVtx[0].u0;
	m_pVtx[2].v0	= m_pVtx[3].v0;


	if(_Scl)
	{
		FLOAT*	vScl = (FLOAT*)_Scl;
		vcScl[0]= vScl[0];
		vcScl[1]= vScl[1];

		fX0	*= vcScl[0];
		fY0	*= vcScl[1];
	}

	if(_Pos)
	{
		FLOAT*	vPos = (FLOAT*)_Pos;
		vcPos[0]= vPos[0];
		vcPos[1]= vPos[1];
	}


	m_pVtx[0].p.x = vcPos[0]	-0.5f;
	m_pVtx[0].p.y = vcPos[1]	-0.5f;

	m_pVtx[1].p.x = m_pVtx[0].p.x + fX0	;
	m_pVtx[1].p.y = m_pVtx[0].p.y		;

	m_pVtx[2].p.x = m_pVtx[0].p.x		;
	m_pVtx[2].p.y = m_pVtx[0].p.y + fY0	;

	m_pVtx[3].p.x = m_pVtx[0].p.x + fX0	;
	m_pVtx[3].p.y = m_pVtx[0].p.y + fY0	;


	if(fX0<0.f)
	{
		FLOAT	_t = fabsf(fX0);
		for(int i=0; i<4; ++i)
		{
			m_pVtx[i].p.x += _t;
		}
	}

	if(fY0<0.f)
	{
		FLOAT	_t = fabsf(fY0);
		for(int i=0; i<4; ++i)
		{
			m_pVtx[i].p.y += _t;
		}
	}


	m_pVtx[0].d = dColor;
	m_pVtx[1].d = dColor;
	m_pVtx[2].d = dColor;
	m_pVtx[3].d = dColor;


	if(_Rc1)
	{
		fX1		= FLOAT(_Rc1->right - _Rc1->left);
		fY1		= FLOAT(_Rc1->bottom- _Rc1->top );

		fMxW1	= (FLOAT) LcMath_MakePower2(dTxW1);
		fMxH1	= (FLOAT) LcMath_MakePower2(dTxH1);

		m_pVtx[0].u1	= (_Rc1->left+0.5f)/(fMxW1);
		m_pVtx[0].v1	= (_Rc1->top +0.5f)/(fMxH1);

		m_pVtx[3].u1	= (_Rc1->right -0.5F)/(fMxW1);
		m_pVtx[3].v1	= (_Rc1->bottom-0.5F)/(fMxH1);

		m_pVtx[1].u1	= m_pVtx[3].u1;
		m_pVtx[1].v1	= m_pVtx[0].v1;

		m_pVtx[2].u1	= m_pVtx[0].u1;
		m_pVtx[2].v1	= m_pVtx[3].v1;
	}


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);
	pDev->SetRenderState( D3DRS_DITHERENABLE,		FALSE);
	pDev->SetRenderState( D3DRS_ZENABLE,			FALSE);
	pDev->SetRenderState(D3DRS_LIGHTING,			FALSE);
	pDev->SetRenderState( D3DRS_SPECULARENABLE,	FALSE);
	
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
	
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, dFilter);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dFilter);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetSamplerState(1, D3DSAMP_MIPFILTER, dFilter);
	pDev->SetSamplerState(1, D3DSAMP_MAGFILTER, dFilter);
	pDev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	pDev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pDev->SetTexture(0, pTex0);

	if(pTex1)
	{
		pDev->SetTexture(1, pTex1);

		pDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		pDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		
		pDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
		pDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
		pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	}


	pDev->SetFVF(CLcSprite::VDRHW::FVF);
	pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_pVtx, sizeof (CLcSprite::VDRHW));


	pDev->SetTexture(0, NULL);
	pDev->SetTexture(1, NULL);

	pDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	pDev->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	pDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	pDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	
	pDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	pDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	return 0;
}




INT CLcSprite::DrawEx( void* _tTx0				// 0 Stage Texture(<==ILC_Texture)
						, void* _tTx1				// 1 Stage Texture(<==ILC_Texture)
						, const RECT* _Rc0			// Draw Region 0 State Texture
						, const RECT* _Rc1			// Draw Region 1 State Texture
						, void* _Scl				// Scaling
						, void* _Pos				// Position
						, void* pRot				// Rotation Center
						, FLOAT	fAngle				// Rotation Angle(Radian)
						, DWORD dColor				// color
						, DWORD dFilter				// Filter(<==D3DMTEXF_POINT)
						)
{
	if(!_tTx0)
		return -1;

	PDTX	pTex0 = NULL;
	PDTX	pTex1 = NULL;

	PLC_TEXTURE pLcTx0	= (PLC_TEXTURE)_tTx0;				// Image Information
	PLC_TEXTURE pLcTx1	= (PLC_TEXTURE)_tTx1;				// Image Information

	DWORD	dTxW0	= pLcTx0->GetWidth();			// Get Width
	DWORD	dTxH0	= pLcTx0->GetHeight();			// Get Height
	DWORD	dTxW1	= dTxW0;
	DWORD	dTxH1	= dTxH0;

	DWORD	dTxC0	= pLcTx0->GetColorKey();		// Get Creation color Key
	DWORD	dTxF0	= pLcTx0->GetFilter();			// Get Creation Filter


	// Get Texture Pointer
	
	if(pLcTx0)
		pTex0	= (PDTX)pLcTx0->GetPointer();		// Get D3DTexture Pointer;
	
	if(pLcTx1)
	{
		pTex1	= (PDTX)pLcTx1->GetPointer();

		dTxW1	= pLcTx1->GetWidth();
		dTxH1	= pLcTx1->GetHeight();
	}


	// Get Transform Infomation
	VEC2	vcScl(1.F, 1.F);
	VEC2	vcPos(0.F, 0.F);
	VEC2	vcRot(0.F, 0.F);
	FLOAT	fRad	= fAngle;


	FLOAT	fX0		= FLOAT(_Rc0->right - _Rc0->left);
	FLOAT	fY0		= FLOAT(_Rc0->bottom- _Rc0->top );
	FLOAT	fX1		= 0;
	FLOAT	fY1		= 0;

	FLOAT	fMxW0	= (FLOAT) LcMath_MakePower2(dTxW0);
	FLOAT	fMxH0	= (FLOAT) LcMath_MakePower2(dTxH0);
	FLOAT	fMxW1	= 0;
	FLOAT	fMxH1	= 0;



	// Setup Texture UV
	m_pVtx[0].u0	= (_Rc0->left+0.5f)/(fMxW0);
	m_pVtx[0].v0	= (_Rc0->top +0.5f)/(fMxH0);

	m_pVtx[3].u0	= (_Rc0->right -0.5F)/(fMxW0);
	m_pVtx[3].v0	= (_Rc0->bottom-0.5F)/(fMxH0);

	m_pVtx[1].u0	= m_pVtx[3].u0;
	m_pVtx[1].v0	= m_pVtx[0].v0;

	m_pVtx[2].u0	= m_pVtx[0].u0;
	m_pVtx[2].v0	= m_pVtx[3].v0;


	// Setup Transformation: Scaling -> Rotation -> Translation
	if(_Scl)
	{
		FLOAT*	vScl = (FLOAT*)_Scl;
		vcScl[0]= vScl[0];
		vcScl[1]= vScl[1];

		fX0	*= vcScl[0];
		fY0	*= vcScl[1];
	}

	if(pRot)
	{
		FLOAT*	vRot = (FLOAT*)pRot;
		vcRot[0]= vRot[0];
		vcRot[1]= vRot[1];
	}

	if(_Pos)
	{
		FLOAT*	vPos = (FLOAT*)_Pos;
		vcPos[0]= vPos[0];
		vcPos[1]= vPos[1];
	}


	// 회전 없는 화면상의 좌표를 설정
	m_pVtx[0].p.x = vcPos[0]	-0.5f;
	m_pVtx[0].p.y = vcPos[1]	-0.5f;

	m_pVtx[1].p.x = m_pVtx[0].p.x + fX0	;
	m_pVtx[1].p.y = m_pVtx[0].p.y		;

	m_pVtx[2].p.x = m_pVtx[0].p.x		;
	m_pVtx[2].p.y = m_pVtx[0].p.y + fY0	;

	m_pVtx[3].p.x = m_pVtx[0].p.x + fX0	;
	m_pVtx[3].p.y = m_pVtx[0].p.y + fY0	;


	if(fX0<0.f)
	{
		FLOAT	_t = fabsf(fX0);
		for(int i=0; i<4; ++i)
		{
			m_pVtx[i].p.x += _t;
		}
	}

	if(fY0<0.f)
	{
		FLOAT	_t = fabsf(fY0);
		for(int i=0; i<4; ++i)
		{
			m_pVtx[i].p.y += _t;
		}
	}




	// 회전을 적용한다.
	// Vertex Positon = VEC2(Pos - Rot Center) ==> Rotation ==>  add Rot Center
	if(pRot)
	{
		FLOAT	cosTheta = cosf(-fRad);
		FLOAT	sinTheta = sinf(-fRad);

		VEC2	_vT;
		VEC2	vcT;

		for(int i=0; i<4; ++i)
		{
			vcT	 = VEC2(0,0);
			_vT.x  = m_pVtx[i].p.x - vcRot.x;
			_vT.y  = m_pVtx[i].p.y - vcRot.y;

			vcT.x = _vT.x * cosTheta - _vT.y * sinTheta;
			vcT.y = _vT.x * sinTheta + _vT.y * cosTheta;

			vcT += vcRot;

			m_pVtx[i].p.x = vcT.x;
			m_pVtx[i].p.y = vcT.y;
		}
	}



	m_pVtx[0].d = dColor;
	m_pVtx[1].d = dColor;
	m_pVtx[2].d = dColor;
	m_pVtx[3].d = dColor;


	if(_Rc1)
	{
		fX1		= FLOAT(_Rc1->right - _Rc1->left);
		fY1		= FLOAT(_Rc1->bottom- _Rc1->top );

		fMxW1	= (FLOAT) LcMath_MakePower2(dTxW1);
		fMxH1	= (FLOAT) LcMath_MakePower2(dTxH1);

		m_pVtx[0].u1	= (_Rc1->left+0.5f)/(fMxW1);
		m_pVtx[0].v1	= (_Rc1->top +0.5f)/(fMxH1);

		m_pVtx[3].u1	= (_Rc1->right -0.5F)/(fMxW1);
		m_pVtx[3].v1	= (_Rc1->bottom-0.5F)/(fMxH1);

		m_pVtx[1].u1	= m_pVtx[3].u1;
		m_pVtx[1].v1	= m_pVtx[0].v1;

		m_pVtx[2].u1	= m_pVtx[0].u1;
		m_pVtx[2].v1	= m_pVtx[3].v1;
	}


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);
	pDev->SetRenderState( D3DRS_DITHERENABLE,		FALSE);
	pDev->SetRenderState( D3DRS_ZENABLE,			FALSE);
	pDev->SetRenderState(D3DRS_LIGHTING,			FALSE);
	pDev->SetRenderState( D3DRS_SPECULARENABLE,	FALSE);
	
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
	
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, dFilter);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dFilter);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetSamplerState(1, D3DSAMP_MIPFILTER, dFilter);
	pDev->SetSamplerState(1, D3DSAMP_MAGFILTER, dFilter);
	pDev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	pDev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pDev->SetTexture(0, pTex0);

	if(pTex1)
	{
		pDev->SetTexture(1, pTex1);

		pDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		pDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		
		pDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
		pDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
		pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	}


	pDev->SetFVF(CLcSprite::VDRHW::FVF);
	pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_pVtx, sizeof (CLcSprite::VDRHW));


	pDev->SetTexture(0, NULL);
	pDev->SetTexture(1, NULL);

	pDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	
	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	pDev->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	pDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	pDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	pDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	
	pDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	pDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	pDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	return 0;
}




inline INT CLcSprite::LcMath_MakePower2(INT a)
{
	int n=0;
	int t=a;

	while(t)
	{
		t>>=1;
		++n;
	}

	return (0x1<<(n-1) ^ a) ? 0x1<<n : 0x1<<(n-1);
};









INT	LcDev_CreateSprite(char* sCmd	
					, ILC_Sprite** pData	
					, void* p1			// No Use
					, void* p2			// No Use
					, void* p3			// No Use
					, void* p4			// No Use
					)
{
	CLcSprite* pObj = NULL;

	(*pData) = NULL;

	if(NULL == sCmd)
		return -1;


	if(0==_stricmp("LC", sCmd))
	{
		pObj = new CLcSprite;

	}
	else
	{
		return -1;
	}

	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	(*pData) = pObj;
	return 0;
}

