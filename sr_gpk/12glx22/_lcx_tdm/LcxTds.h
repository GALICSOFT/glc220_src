// Interface for the CLcxTds class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxTds_H_
#define _LcxTds_H_


#include <vector>
using namespace std;


struct TdsPts								// parts list
{
	PLCX_TDM	pTdm;						// Base parts instance
	INT			nRec;						// sub parts count
	PLCX_TDM*	vRec;						// sub parts list

	TdsPts() : pTdm(NULL), nRec(0), vRec(NULL){}
	~TdsPts()
	{
		Destroy();
	}

	void Destroy()
	{
		for(int n=0; n<nRec; ++n)
		{
			PLCX_TDM pRec = vRec[n];
			pRec->Release();
			vRec[n] = NULL;
		}

		SAFE_DELETE_ARRAY(vRec);
		nRec = 0;

		SAFE_RELEASE(	pTdm	);
	}
};


class CLcxTds;

struct TspInf
{
	char		sId[LCXM_TEXNAME];			// Name length(4byte) and Name( File Name with out extension)
	INT			nVer[2]	;					// Version
	INT			nPts	;
	TdsPts*		vPts	;
	CLcxTds*	pOrg	;					// Original TDS Model Pointer

	TspInf() : nPts(0), vPts(NULL), pOrg(NULL)
	{
		memset(sId, 0, LCXM_TEXNAME);
		memset(nVer, 0, sizeof(nVer));
	}

	~TspInf()
	{
		Destroy();
	}

	void Destroy()
	{
		SAFE_DELETE_ARRAY(	vPts );

		memset(sId, 0, LCXM_TEXNAME);

		nPts	= 0;
		nVer[0]	= 0;
		nVer[1]	= 0;
		pOrg	= NULL;
	}
};


class CLcxTds : public ILCX_Tds
{
public:
	PLC_SPRITE_L	m_pSpl		;							// sprite layer
	TspInf*			m_pInf		;
	TdsPts*			m_pPts		;

	LCXTF2D			m_tm		;							// transform

public:
	CLcxTds();
	virtual ~CLcxTds();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual	INT		FrameMove();
	virtual void	Render();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);	// Write Attribute Value
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);	// Read Attribute Value

	virtual INT		SetPosition(const LC_FLOAT2*);			// position: float2
	virtual INT		SetRotation(const LC_FLOAT3*);			// rotation: float3 rotation center(x,y) + rotation angle(z: radian)
	virtual INT		SetScaling (const LC_FLOAT2*);			// scaling : float2

public:
	INT		CreateFromFile  (LC_HANDLE =NULL, LC_HANDLE =NULL
							, LC_HANDLE =NULL, LC_HANDLE =NULL);
	INT		CreateFromMemory(PLC_ARGS pArg);

protected:
	void	Destroy();

	INT		LoadFromXML(char* sFile);
	INT		LoadFromTxt(char* sFile);
	INT		LoadFromBin(char* sFile);
	INT		LoadFromMemory(void* pMem, void* pSize);

	INT		SaveToBin(char* sFile);

	INT		FindPtsIdx(char* sPtsName);
	INT		FindPtsSeq(INT nPtsC/*Current ani index*/, INT dTime);

public:
	TspInf*	GetSptInf();

	static vector<PLCX_TDS>	g_lstTds;
	static INT				Add(CLcxTds* pTds);
	static INT				PopFromList(PLCX_TDS pTds);

};

#endif


