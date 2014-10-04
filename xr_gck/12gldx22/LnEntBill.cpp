// Implementation of the CLcEntBill classes.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnBase.h>

#include "LnxEntity.h"
#include "LnEntBill.h"


CLcEntBill::CLcEntBill()
{
	m_dPtNum	= 0;

	m_nBill		= 0;
	m_pBill		= NULL;
}


CLcEntBill::~CLcEntBill()
{
	Destroy();
}


void CLcEntBill::Destroy()
{
	if(m_pBill)
	{
		delete [] m_pBill;
		m_pBill = NULL;
	}

	m_nBill		= 0;
}


INT CLcEntBill::Create(void* p1, void* p2, void* p3, void* p4)
{
	m_nBill	= (INT)p1;

	m_pBill = new CLcEntBill::Billboard[m_nBill];

	return 0;
}



INT CLcEntBill::FrameMove()
{
	INT	i=0;
	
	D3DXVECTOR3	vcDir(0,0,0);

	for(i=0; i<m_nBill; ++i)
	{
		vcDir.x = m_pBill[i].vcP.x - m_vcCamP.x;
		vcDir.y = m_pBill[i].vcP.y - m_vcCamP.y;
		vcDir.z = m_pBill[i].vcP.z - m_vcCamP.z;

		m_pBill[i].fStlSrtR = D3DXVec3Dot(&vcDir, &m_vcCamZ);
	}

	qsort( m_pBill, m_nBill, sizeof(Billboard), CLcEntBill::Sort );

	return 0;
}



void CLcEntBill::Render()
{
	INT		i=0;

	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)LcDev_GetD3Device();

	pDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);


	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	pDev->SetFVF(CLcEntBill::VtxDUV1::FVF);

	for(i=0; i<m_nBill; ++i)
	{

		pDev->SetTexture( 0, (LPDIRECT3DTEXTURE9)m_pBill[i].pTx );

		// Translate the billboard into place
		m_mtBill._41 = m_pBill[i].vcP.x;
		m_mtBill._42 = m_pBill[i].vcP.y;
		m_mtBill._43 = m_pBill[i].vcP.z;

		pDev->SetTransform( D3DTS_WORLD, &m_mtBill );

		// Render the billboard
		pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP
								, 2
								, m_pBill[i].Vtx
								, sizeof(CLcEntBill::VtxDUV1) );
	}

	static	D3DXMATRIX mtI(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);
	pDev->SetTransform( D3DTS_WORLD, &mtI);
}


INT CLcEntBill::Query(char* sCmd, void* pData)
{
	if(0==_stricmp(sCmd, "Get Number"))
	{
		*((INT*)pData) = m_nBill;
		return 0;
	}

	else if(0==_stricmp(sCmd, "Lock"))
	{
		return 0;
	}

	else if(0==_stricmp(sCmd, "Unlock"))
	{
		return 0;
	}

	else if(0==_stricmp(sCmd, "Set Dimension"))
	{
		struct _T
		{
			INT		n;
			FLOAT*	p;
		};

		_T*	t = (_T*)pData;

		SetDimension(t->n, (D3DXVECTOR4*)(t->p));
		return 0;
	}

	else if(0==_stricmp(sCmd, "Set Dimension"))
	{
		struct _T
		{
			INT		n;
			FLOAT*	p;
		};

		_T*	t = (_T*)pData;

		SetDimension(t->n, (D3DXVECTOR4*)(t->p));
		return 0;
	}

	else if(0==_stricmp(sCmd, "Set Position"))
	{
		struct _T
		{
			INT		n;
			FLOAT*	p;
		};

		_T*	t = (_T*)pData;

		SetPos(t->n, (D3DXVECTOR3*)(t->p));
		return 0;
	}

	else if(0==_stricmp(sCmd, "Set Diffuse"))
	{
		struct _T
		{
			INT		n;
			DWORD	d;
		};

		_T*	t = (_T*)pData;

		SetDiffuse(t->n, t->d);
		return 0;
	}

	else if(0==_stricmp(sCmd, "Set UV"))
	{
		struct _T
		{
			INT		n;
			INT		tx;
			FLOAT*	p;
		};

		_T*	t = (_T*)pData;

		SetUV(t->n, t->tx, (D3DXVECTOR2*)(t->p));
	}

	else if(0==_stricmp(sCmd, "Set Texture"))
	{
		struct _T
		{
			INT		n;
			void*	p;
		};

		_T*	t = (_T*)pData;

		SetTexture(t->n, t->p);
	}

	else if(0==_stricmp(sCmd, "Get Position"))
	{
		struct _T
		{
			INT		n;
			FLOAT*	p;
		};

		_T*	t = (_T*)pData;

		GetPos(t->n, (D3DXVECTOR3*)(t->p));
		return 0;
	}

	else if(0==_stricmp(sCmd, "Set Inverse View Matrix"))
	{
		D3DXMATRIX*	p = (D3DXMATRIX*)pData;
		SetBillMatrix(p);
		return 0;
	}

	return -1;
}


void CLcEntBill::SetTexture(INT nIdx, void* pTx)
{
	if(nIdx<0 || nIdx >=m_nBill)
		return;

	m_pBill[nIdx].pTx = (LPDIRECT3DTEXTURE9)pTx;
}


void CLcEntBill::SetDimension(INT nIdx, D3DXVECTOR4* p)
{
	m_pBill[nIdx].Vtx[0].p.x = p->x;
	m_pBill[nIdx].Vtx[0].p.y = p->y;

	m_pBill[nIdx].Vtx[1].p.x = p->x;
	m_pBill[nIdx].Vtx[1].p.y = p->w;

	m_pBill[nIdx].Vtx[2].p.x = p->z;
	m_pBill[nIdx].Vtx[2].p.y = p->y;

	m_pBill[nIdx].Vtx[3].p.x = p->z;
	m_pBill[nIdx].Vtx[3].p.y = p->w;
}



void CLcEntBill::SetPos(INT nIdx, D3DXVECTOR3* p)
{
	m_pBill[nIdx].vcP = *p;
}


void CLcEntBill::SetDiffuse(INT nIdx, DWORD d)
{
	m_pBill[nIdx].SetDiffuse(d);
}


void CLcEntBill::SetUV(INT nIdx, INT nUV, D3DXVECTOR2* p)
{
	m_pBill[nIdx].SetUV(nUV, p->x, p->y);
}



void CLcEntBill::GetPos(INT nIdx, D3DXVECTOR3* pOut)
{
	*pOut = m_pBill[nIdx].vcP;
}



void CLcEntBill::GetDiffuse(INT nIdx, D3DXCOLOR* p)
{
	D3DXCOLOR dColor(1, 1, 1, 1);

	dColor = m_pBill[nIdx].Vtx[0].d;
	*p = dColor;
}




void CLcEntBill::GetUV(INT nIdx, INT nUV, D3DXVECTOR2* p)
{
	p->x = m_pBill[nIdx].Vtx[nUV].u;
	p->y = m_pBill[nIdx].Vtx[nUV].v;
}



void CLcEntBill::SetBillMatrix(D3DXMATRIX* mtBill)
{
	memcpy(&m_mtBill, mtBill, sizeof m_mtBill);

	m_vcCamX = D3DXVECTOR3( m_mtBill._11, m_mtBill._12, m_mtBill._13);
	m_vcCamY = D3DXVECTOR3( m_mtBill._21, m_mtBill._22, m_mtBill._23);
	m_vcCamZ = D3DXVECTOR3( m_mtBill._31, m_mtBill._32, m_mtBill._33);
	m_vcCamP = D3DXVECTOR3( m_mtBill._41, m_mtBill._42, m_mtBill._43);

	m_mtBill._41 = 0;
	m_mtBill._42 = 0;
	m_mtBill._43 = 0;
}


INT CLcEntBill::Sort( const void* arg1, const void* arg2 )
{
	Billboard* p1 = (Billboard*)arg1;
	Billboard* p2 = (Billboard*)arg2;

	FLOAT d1 = p1->fStlSrtR;
	FLOAT d2 = p2->fStlSrtR;
	
	if (d1 < d2)
		return +1;
	else if( d1 == d2)
		return 0;
	
	
	return -1;
}











