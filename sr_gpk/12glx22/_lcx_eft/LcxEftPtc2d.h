// Interface for the CLcxEftPtc2d class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxEftPtc2d_H_
#define _LcxEftPtc2d_H_


class CLcxEftPtc2d : public CLcxEffect
{
protected:
	TLCX_PRT2dINF	m_Inf;

	TLCX_PRT2d*		m_Ptc;		// Particle
	INT				m_VtxN;		// Rendering Vertex number
	LC3D::Vx2DUV*	m_Vtx;		// Rendering Vertex

public:
	CLcxEftPtc2d();
	virtual ~CLcxEftPtc2d();

	virtual	INT		FrameMove();
	virtual void	Render();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);

	virtual INT		GetRscType()	{	return LCXEFT_2D_PARTICLE;	}

public:
	virtual INT		Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4);
	virtual void	Destroy();

protected:
	void	AssembleVertex(INT* VtxN, TLCX_PRT2d*);
};


#endif

