// Interface for the the Geometry Object classes.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _LcxGeoObj_H_
#define _LcxGeoObj_H_


class CLcxGeoAxis : public ILCX_Entity
{
public:
	struct Vtx : public LC3D::VxfD
	{
		Vtx()
		{
			p.x = 0;
			p.y = 0;
			p.z = 0;
			d   =	LCXCOLORB(UINT(0XFFFFFFFF));
		}

		Vtx(FLOAT X, FLOAT Y, FLOAT Z, UINT D=0xFFFFFFFF)
		{
			p.x = X;
			p.y = Y;
			p.z = Z;
			d   =	LCXCOLORB(UINT(D));
		}
	};


protected:
	Vtx				m_pLine[12];

public:
	CLcxGeoAxis();
	virtual ~CLcxGeoAxis();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	virtual	void	Destroy()						{	return;		}
	virtual	INT		FrameMove()						{	return 0;	}

	virtual	void	Render();
};





class CLcxGeoGrid : public ILCX_Entity
{
public:
	struct Vtx : public LC3D::VxfD
	{
		Vtx()
		{
			p.x = 0;
			p.y = 0;
			p.z = 0;
			d   =	LCXCOLORB(UINT(0xFFFFFFFF));
		}

		Vtx(FLOAT X, FLOAT Y, FLOAT Z, UINT D=0xFFFFFFFF)
		{
			p.x = X;
			p.y = Y;
			p.z = Z;
			d   =	LCXCOLORB(UINT(D));
		}
	};


protected:
	Vtx				m_pLine[72];

public:
	CLcxGeoGrid();
	virtual ~CLcxGeoGrid();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	virtual	void	Destroy()						{	return;		}
	virtual	INT		FrameMove()						{	return 0;	}

	virtual	void	Render();
};


#endif

