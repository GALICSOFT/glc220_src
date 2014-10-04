// Interface for the CLcxTdm class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxTdm_H_
#define _LcxTdm_H_


#include <vector>
using namespace std;


// tda list
struct TsptRec
{
	INT				Time;					// Animation Name
	PLCX_TDA		pTda;					// Tda Instance
	TLC_TDAREC*		pTsc;					// Sprite Frame Number

	TsptRec() : Time(-1), pTda(NULL), pTsc(NULL){}
	~TsptRec()
	{
		Destroy();
	}

	void Destroy()
	{
		Time	= -1;
		pTda	= NULL;
		pTsc	= NULL;
	}
};


// animation list
struct TsptAni
{
	char			sName[LCXM_ANINAME];	// Animation Name
	INT				nRpt;					// Repeat
	INT				nNxt;					// Next
	INT				nRec;
	TsptRec*		vRec;					// Sprite Frame Number

	TsptAni() : nRpt(0), nNxt(-1), nRec(0), vRec(NULL)
	{
		memset(sName, 0, LCXM_ANINAME);
	}

	TsptAni(char* s) : nRpt(0), nNxt(-1), nRec(0), vRec(NULL)
	{
		memset(sName, 0, LCXM_ANINAME);
		strcpy(sName, s);
	}

	~TsptAni()
	{
		Destroy();
	}

	void Destroy()
	{
		memset(sName, 0, LCXM_ANINAME);
		SAFE_DELETE_ARRAY(vRec);

		nRpt = 0;
		nRec = 0;
		nNxt = -1;
	}
};


class CLcxTdm;

struct TsptInf
{
	char		sId[LCXM_TEXNAME];			// Name length(4byte) and Name( File Name with out extension)
	INT			nVer[2]	;					// Version
	INT			nAni	;
	TsptAni*	vAni	;
	CLcxTdm*	pOrg	;					// Original TDM Model Pointer

	TsptInf() : nAni(0), vAni(NULL), pOrg(NULL)
	{
		memset(sId, 0, LCXM_TEXNAME);
		memset(nVer, 0, sizeof(nVer));
	}

	~TsptInf()
	{
		Destroy();
	}

	void Destroy()
	{
		SAFE_DELETE_ARRAY(	vAni );

		memset(sId, 0, LCXM_TEXNAME);

		nAni	= 0;
		nVer[0]	= 0;
		nVer[1]	= 0;
		pOrg	= NULL;
	}
};


class CLcxTdm : public ILCX_Tdm
{
public:
	PLC_SPRITE_L	m_pSpl		;							// sprite layer
	TsptInf*		m_pInf		;
	PLCX_TDM		m_pTsp		;							// time sequence parent

	LCXTF2D			m_tm		;							// transform
	TLCX_ANI_TIME2D	m_at		;							// animation time infomation


protected:
	INT				m_nRpt		;							// Repeat
	INT				m_nSts		;							// Status for update and rendering

public:
	CLcxTdm();
	virtual ~CLcxTdm();

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

	INT		FindAniIdx(char* sAniName);
	INT		FindAniSeq(INT nAniC/*Current ani index*/, INT dTime);
	INT		FindRecIdx(PLCX_TDA pTda, TLC_TDAREC* pRec);

public:
	TsptInf*	GetSptInf();

	void		SetTsp(PLCX_TDM v) {        m_pTsp = v; }
	PLCX_TDM	GetTsp()           { return m_pTsp    ; }


	static vector<PLCX_TDM>	g_lstTdm;
	static INT				Add(CLcxTdm* pTdm);
	static INT				PopFromList(PLCX_TDM pTdm);

};

#endif


