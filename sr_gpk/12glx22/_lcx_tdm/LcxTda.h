// Interface for the CLcxTda class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcxTda_H_
#define _LcxTda_H_


#include <vector>
#include <map>
#include <string>
using namespace std;


class CLcxTda : public ILCX_Tda
{
public:
	struct TtdaOrg
	{
		char		sName[LCXM_ANINAME];					// Sequence Name
		LCXRECT		rcD;									// rect Dest
		LCXRECT		rcO;									// rect Original
		FLOAT		rot;
	};

public:
	char			m_sId[LCXM_TEXNAME];					// Name length(4byte) and Name( File Name with out extension)

protected:
	INT				m_ver[2]	;							// Version
	INT				m_nLst		;							// sprite list
	INT				m_imgW		;							// Image Width
	INT				m_imgH		;							// Image Height
	char			m_sTex[LCXM_TEXNAME];					// texture file name

	TLC_TDAREC*		m_pRec		;
	PLC_TEXTURE		m_pTex		;							// get the texture

public:
	CLcxTda();
	virtual ~CLcxTda();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		SetAttrib(UINT cmd,LC_HANDLE v);		// Write Attribute Value
	virtual INT		GetAttrib(UINT cmd,LC_HANDLE v);		// Read Attribute Value

	virtual INT		GetRecIdx(char* recName);				// get the record index. if NULL == recName then return the count of Records

	virtual TLC_TDAREC* GetRec(char* recName);				// get the record pointer with name
	virtual TLC_TDAREC* GetRec(INT recIdx);					// get the record pointer with index

	virtual PLC_TEXTURE GetTexture() { return m_pTex; }		// get the texture

public:
	INT		CreateFromFile  (LC_HANDLE =NULL, LC_HANDLE =NULL
							, LC_HANDLE =NULL, LC_HANDLE =NULL);
	INT		CreateFromMemory(PLC_ARGS pArg);

protected:
	void	Destroy();

	INT		LoadFromXML(char* sFile);
	INT		LoadFromCsv(char* sFile);
	INT		LoadFromBin(char* sFile);
	INT		LoadFromMemory(void* pMem, void* pSize);

	INT		SaveToBin(char* sFile);
	INT		SetupVtx(std::vector<TtdaOrg* >* pOrg);

public:
	static vector<PLCX_TDA>	g_lstTda;
	static INT				Add(CLcxTda* pTda);
	static INT				PopFromList(PLCX_TDA pTda);
};

#endif

