// Implementation of the CLcGeoAxis class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcGeoAxis_H_
#define _LcGeoAxis_H_

class CLcGeoAxis : public ILcEnt
{
public:
	struct VtxD
	{
		D3DXVECTOR3	p;
		D3DCOLOR	d;
		
		VtxD()							: p(0,0,0),d(0XFFFFFFFF){}
		VtxD(FLOAT X,FLOAT Y,FLOAT Z
			,D3DCOLOR D=0xFFFFFFFF)		: p(X,Y,Z),d(D){}

		enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE) };
	};


protected:
	CLcGeoAxis::VtxD	m_pLine[12];
	
public:
	CLcGeoAxis();
	virtual ~CLcGeoAxis();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual	void	Destroy();
	virtual	INT		FrameMove();
	virtual	void	Render();

	virtual INT		Query(char* sCmd, void* pData);
};


#endif


