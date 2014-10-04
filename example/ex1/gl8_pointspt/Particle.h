// Interface for the CParticle class.
//
////////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif


#ifndef _Particle_H_
#define _Particle_H_


class CParticle
{
public:
	struct Tvector2
	{
		float x, y;
	};

	struct Tcolor
	{
		float r, g, b, a;
	};

	struct Tptc	// particle
	{
		Tvector2	pos;		// Position
		Tvector2	vec;		// velocity
		Tcolor		col;		// color
	};


protected:
	unsigned int	m_TexID;

	int				m_PtcN;		// Number of Particle
	Tptc*			m_Ptc;		// Particle

public:
	CParticle();
	virtual ~CParticle();

	int		Create(char* TextureFileName, int ParticleNumber);
	void	Destroy();

	int		FrameMove();
	void	Render();

protected:
	void	Reset(int n);
	void	InitPtc(int n);
};

#endif


