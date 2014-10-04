// Implementation of the CLcSpriteX class.
//
////////////////////////////////////////////////////////////////////////////////


#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "LnBase.h"
#include "LnSpriteX.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


CLcSpriteX::CLcSpriteX()
{
	m_pFVF		= NULL;
	m_pEft		= NULL;

	m_nScnW		= 1024;
	m_nScnH		= 768;


	m_pVtx[0].t0 = VEC2(0, 0);
	m_pVtx[1].t0 = VEC2(1, 0);
	m_pVtx[2].t0 = VEC2(1, 1);
	m_pVtx[3].t0 = VEC2(0, 1);

	m_pVtx[0].t1 = VEC2(0, 0);
	m_pVtx[1].t1 = VEC2(1, 0);
	m_pVtx[2].t1 = VEC2(1, 1);
	m_pVtx[3].t1 = VEC2(0, 1);

}


CLcSpriteX::~CLcSpriteX()
{
	Destroy();
}


void CLcSpriteX::Destroy()
{
	SAFE_RELEASE(	m_pEft		);
	SAFE_RELEASE(	m_pFVF		);
}


INT CLcSpriteX::Create(void* p1, void* p2, void* p3, void* p4)
{
	HRESULT	hr=0;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	char	sShader[]=
		" int     m_bTx1;                               \n"
		" int     m_bMono;                              \n"
		" float4  m_Diff;                               \n"
		"                                               \n"
		" sampler smp0 : register(s0);                  \n"
		" sampler smp1 : register(s1);                  \n"
		"                                               \n"
		" void VtxProc( in  float4 inPos0: POSITION     \n"
		"             , in  float2 inTex0: TEXCOORD0    \n"
		"             , in  float2 inTex1: TEXCOORD1    \n"
		"             , out float4 otPos0: POSITION     \n"
		"             , out float2 otTex0: TEXCOORD0    \n"
		"             , out float2 otTex1: TEXCOORD1    \n"
		"             )                                 \n"
		" {                                             \n"
		"     otPos0 = inPos0;                          \n"
		"     otTex0 = inTex0;                          \n"
		"     otTex1 = inTex1;                          \n"
		" }                                             \n"
		"                                               \n"
		" float4 PxlProc(float4 Pos0: POSITION          \n"
		"             , float2 Tex0: TEXCOORD0          \n"
		"             , float2 Tex1: TEXCOORD1):COLOR0  \n"
		" {                                             \n"
		"     float4  Out= 0;                           \n"
		"     float4  t0 = tex2D(smp0, Tex0);           \n"
		"     float4  t1 = tex2D(smp1, Tex1);           \n"
		"                                               \n"
		"     if(0 != m_bTx1)                           \n"
		"         Out = t0 * t1;                        \n"
		"     else                                      \n"
		"         Out = t0;                             \n"
		"                                               \n"
		"     Out *= m_Diff;                            \n"
		"     if(0 != m_bMono)                          \n"
		"     {                                         \n"
		"         Out.a *= m_Diff.a;                    \n"
		"                                               \n"
		"         Out.r= m_Diff.r;                      \n"
		"         Out.g= m_Diff.g;                      \n"
		"         Out.b= m_Diff.b;                      \n"
		"     }                                         \n"
		"                                               \n"
		"     return Out;                               \n"
		" }                                             \n"
		"                                               \n"
		" technique Tech                                \n"
		" {                                             \n"
		"    pass P0                                    \n"
		"    {                                          \n"
		"      VertexShader = compile vs_1_1 VtxProc(); \n"
		"      PixelShader  = compile ps_2_0 PxlProc(); \n"
		"    }                                          \n"
		" };                                            \n"
		;


	LPD3DXBUFFER	pError	= NULL;
	DWORD		dFlag=0;

	#if defined( _DEBUG ) || defined( DEBUG )
	dFlag |= D3DXSHADER_DEBUG;
	#endif

	// 컴파일
	hr = D3DXCreateEffect(	pDev
							, sShader
							, strlen(sShader)-4
							, NULL
							, NULL
							, dFlag
							, NULL
							, &m_pEft
							, &pError);
	if(FAILED(hr))
	{
		MessageBox(GetActiveWindow()
					, (char*)pError->GetBufferPointer()
					, "Error", 0);
		return -1;
	}
	
	// 정점 선언 생성
	D3DVERTEXELEMENT9 pVertexElement[MAX_FVF_DECL_SIZE]={0};
	D3DXDeclaratorFromFVF(CLcSpriteX::VtxRHWUV1::FVF, pVertexElement);
	pDev->CreateVertexDeclaration(pVertexElement, &m_pFVF);


	return 0;
}



INT CLcSpriteX::Begin(DWORD dVal)	{	return 0;	}
INT CLcSpriteX::End(DWORD dVal)	{	return 0;	}

INT CLcSpriteX::OnResetDevice()
{
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	LPDIRECT3DSURFACE9	pSfc;
	D3DSURFACE_DESC		desc;

	pDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSfc);
	pSfc->GetDesc(&desc);

	m_nScnW = desc.Width;
	m_nScnH = desc.Height;
	pSfc->Release();


	return m_pEft->OnResetDevice();
}

INT CLcSpriteX::OnLostDevice()
{
	return m_pEft->OnLostDevice();
}

INT CLcSpriteX::DrawEx( void* pTx0					// 0 Stage Texture(<==ILcTex)
						, void* pTx1				// 1 Stage Texture(<==ILcTex)
						, const RECT* pRC0			// Draw Region 0 State Texture
						, const RECT* pRC1			// Draw Region 1 State Texture
						, void* pScl				// Scaling
						, void* pPos				// Position
						, void* pRot				// Rotation Center
						, FLOAT	fAngle				// Rotation Angle(Radian)
						, DWORD dColor				// color
						, DWORD dFilter				// Filter(<==D3DMTEXF_POINT)
						)
{
	return DrawExt(pTx0, pTx1, pRC0, pRC1, (VEC2*)pScl, (VEC2*)pRot, fAngle, (VEC2*)pPos, dColor, (INT)dFilter);
}



INT CLcSpriteX::DrawExt(  void* pLcTex0
						, void* pLcTex1
						, const RECT* pRc1
						, const RECT* pRc2
						, VEC2* pScl
						, VEC2* pRot
						, FLOAT fRot
						, VEC2* pTrn
						, DWORD _dCol
						, INT _bMono)
{
	HRESULT hr=0;

	INT		bTex1 = 0;
	INT		bMono = 0;
	DCOL	dDiff = 0XFFFFFFFF;
	VEC2	vScl(1,1);
	VEC2	vRot(0,0);
	VEC2	vTrn(0,0);

	RECT	rc1	 = {0, 0, 2048, 2048};
	RECT	rc2	 = {0, 0, 2048, 2048};

	VEC2	uv00(0,0);
	VEC2	uv01(1,1);
	VEC2	uv10(0,0);
	VEC2	uv11(1,1);

	FLOAT	rcW1 = 0;
	FLOAT	rcH1 = 0;
	FLOAT	rcW2 = 0;
	FLOAT	rcH2 = 0;
	
	FLOAT	PosL = 0;
	FLOAT	PosT = 0;
	FLOAT	PosR = 0;
	FLOAT	PosB = 0;

	LcTexture*	pTex0= (LcTexture*)pLcTex0;
	LcTexture*	pTex1= (LcTexture*)pLcTex1;

	PDTX	pDXTx0 = NULL;
	PDTX	pDXTx1 = NULL;

	// 텍스처가 없으면 빠져 나감.
	if(NULL == pTex0 || NULL == pTex0->pTex)
		return 0;



	// 1. 입력 값 복사
	dDiff = _dCol;
	bMono = _bMono;


	// 1.1 이미지 소스 1 영역 복사
	if(pRc1)
	{
		rc1.left  = pRc1->left  ;
		rc1.right = pRc1->right ;
		rc1.top	  = pRc1->top	 ;
		rc1.bottom= pRc1->bottom;
	}
	else
	{
		rc1.right = pTex0->ImgW;
		rc1.bottom= pTex0->ImgH;
	}


	// 영역 재 지정
	if(rc1.left<0)				rc1.left  = 0;
	if(rc1.right>pTex0->ImgW)	rc1.right = pTex0->ImgW;
	if(rc1.top<0)				rc1.top   = 0;
	if(rc1.bottom>pTex0->ImgH)	rc1.bottom= pTex0->ImgH;


	// 영역을 잘못 설정한 경우임
	if(rc1.top>=rc1.bottom || rc1.left>=rc1.right)
		return 0;


	// uv 설정
	uv00.x = FLOAT(rc1.left  )/FLOAT(pTex0->TexW);
	uv00.y = FLOAT(rc1.top   )/FLOAT(pTex0->TexH);
	uv01.x = FLOAT(rc1.right )/FLOAT(pTex0->TexW);
	uv01.y = FLOAT(rc1.bottom)/FLOAT(pTex0->TexH);

	// 1.2 이미지 소스 2 영역 복사
	if(pRc2)
	{
		rc2.left  = pRc2->left  ;
		rc2.right = pRc2->right ;
		rc2.top	  = pRc2->top	;
		rc2.bottom= pRc2->bottom;
	}
	else if(pTex1)
	{
		rc2.right = pTex1->ImgW;
		rc2.bottom= pTex1->ImgH;
	}


	// 영역 재 지정
	if(rc2.left<0)						rc2.left  = 0;
	if(pTex1 &&rc2.right>pTex1->ImgW)	rc2.right = pTex1->ImgW;
	if(rc2.top<0)						rc2.top   = 0;
	if(pTex1 &&rc2.bottom>pTex1->ImgH)	rc2.bottom= pTex1->ImgH;

	if(pTex1)
	{
		uv10.x = FLOAT(rc2.left  )/ FLOAT(pTex1->TexW);
		uv10.y = FLOAT(rc2.top   )/ FLOAT(pTex1->TexH);
		uv11.x = FLOAT(rc2.right )/ FLOAT(pTex1->TexW);
		uv11.y = FLOAT(rc2.bottom)/ FLOAT(pTex1->TexH);
	}
	

	if(pScl)	vScl = *pScl;
	if(pRot)	vRot = *pRot;
	if(pTrn)	vTrn = *pTrn;


	rcW1	= FLOAT(rc1.right - rc1.left);
	rcH1	= FLOAT(rc1.bottom- rc1.top );

	rcW2	= FLOAT(rc2.right - rc2.left);
	rcH2	= FLOAT(rc2.bottom- rc2.top );



	// Scaling 적용
	if(vScl.x>=0.f)
	{
		PosL =  vTrn.x;
		PosR = PosL + rcW1 * vScl.x;
	}
	else
	{
		PosR =  vTrn.x;
		PosL = PosR - vScl.x;
	}


	if(vScl.y>=0.f)
	{
		PosT = vTrn.y;
		PosB = PosT + rcH1 * vScl.y;
	}
	else
	{
		PosB = vTrn.y;
		PosT = PosB - rcH1 * vScl.y;
	}


	// 정점 설정
	m_pVtx[0].p = VEC2(PosL, PosT);
	m_pVtx[1].p = VEC2(PosR, PosT);
	m_pVtx[2].p = VEC2(PosR, PosB);
	m_pVtx[3].p = VEC2(PosL, PosB);

	m_pVtx[0].t0= VEC2(uv00.x, uv00.y);
	m_pVtx[1].t0= VEC2(uv01.x, uv00.y);
	m_pVtx[2].t0= VEC2(uv01.x, uv01.y);
	m_pVtx[3].t0= VEC2(uv00.x, uv01.y);

	m_pVtx[0].t1= VEC2(uv10.x, uv10.y);
	m_pVtx[1].t1= VEC2(uv11.x, uv10.y);
	m_pVtx[2].t1= VEC2(uv11.x, uv11.y);
	m_pVtx[3].t1= VEC2(uv10.x, uv11.y);



	// 회전 설정
	if(pRot)
	{
		FLOAT	fCos = cosf(-fRot);
		FLOAT	fSin = sinf(-fRot);

		VEC2	t;

		for(int i=0; i<4; ++i)
		{
			t = m_pVtx[i].p - vRot;

			m_pVtx[i].p.x = t.x * fCos - t.y * fSin + vRot.x;
			m_pVtx[i].p.y = t.x * fSin + t.y * fCos + vRot.y;
		}
	}



	bTex1 = (INT)pTex1;
	pDXTx0 = pTex0->pTex;

	if(pTex1)
		pDXTx1 = pTex1->pTex;


	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();


	for(INT i=0; i<2; ++i)
	{
		pDev->SetSamplerState(i,D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDev->SetSamplerState(i,D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		// 단색일 경우 부드럽게 처리
		if(bMono)
		{
			pDev->SetSamplerState(i,D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			pDev->SetSamplerState(i,D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pDev->SetSamplerState(i,D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		}
		else
		{
			pDev->SetSamplerState(i,D3DSAMP_MAGFILTER, D3DTEXF_NONE);
			pDev->SetSamplerState(i,D3DSAMP_MINFILTER, D3DTEXF_NONE);
			pDev->SetSamplerState(i,D3DSAMP_MIPFILTER, D3DTEXF_NONE);	
		}
	}


	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	pDev->SetTexture(0, pDXTx0);
	pDev->SetTexture(1, pDXTx1);

	hr = pDev->SetVertexDeclaration(m_pFVF);

	hr = m_pEft->SetTechnique("Tech");
	hr = m_pEft->SetInt("m_bTx1" , bTex1);
	hr = m_pEft->SetInt("m_bMono", bMono);
	hr = m_pEft->SetVector("m_Diff", (D3DXVECTOR4*)&dDiff);
	
	hr = m_pEft->Begin(NULL, 0);
	hr = m_pEft->Pass(0);
	hr = pDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_pVtx, sizeof(CLcSpriteX::VtxRHWUV1));

	hr = m_pEft->End();


	pDev->SetTexture(0, NULL);
	pDev->SetTexture(1, NULL);
	pDev->SetVertexDeclaration(NULL);
	pDev->SetVertexShader(NULL);
	pDev->SetPixelShader(NULL);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return 0;
}


