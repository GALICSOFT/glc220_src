// Implementation of the CLctXml class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcMath.h>
#include <LcBase.h>
#include <LcAux.h>


#include "LctEntity.h"



class CLctXml : public ILCT_Xml
{
protected:

public:
	CLctXml();
	virtual ~CLctXml();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		SetAttrib(UINT dCmd, LC_HANDLE pData);	// Write Attribute Value
	virtual INT		GetAttrib(UINT dCmd, LC_HANDLE pData);	// Read Attribute Value


	virtual LC_HANDLE	ElmList(const char* sElm, LC_HANDLE pNodeParent);
	virtual LC_HANDLE	ElmNode(LC_HANDLE pNode, INT nIdx);
	virtual INT			ElmSize(LC_HANDLE pNode);
	virtual INT			ElmText(char* sOutput, LC_HANDLE pNode);
	virtual INT			ElmAttrb(char* sOutput, int sOutputSize, LC_HANDLE pNode, const char* sAttName);

public:
	INT		Create(LC_HANDLE p1=NULL, LC_HANDLE p2=NULL, LC_HANDLE p3=NULL, LC_HANDLE p4=NULL);
	void	Destroy();

	INT		FileLoad(const char* sFile);
};




CLctXml::CLctXml()
{
}


CLctXml::~CLctXml()
{
	Destroy();
}


void CLctXml::Destroy()
{
}



INT CLctXml::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT	hr=-1;


	return hr;
}


INT CLctXml::FileLoad(const char* sFile)
{
	INT	hr=-1;
	return hr;
}


LC_HANDLE CLctXml::ElmList(const char* sElm, LC_HANDLE pNode)
{
	return NULL;
}


LC_HANDLE CLctXml::ElmNode(LC_HANDLE pNode, INT nIdx)
{
	return NULL;
}


INT CLctXml::ElmSize(LC_HANDLE pNode)
{
	long	nCount=0;

	return nCount;
}



INT	CLctXml::ElmText(char* sOutput, LC_HANDLE pNode)
{
	return LC_OK;
}

INT CLctXml::ElmAttrb(char* sOutput, int sOutputSize, LC_HANDLE pNode, const char* sAttName)
{
	return LC_OK;
}







INT LcXml_Create(char* sCmd
				 , PLCT_XML* pOut
				 , LC_HANDLE p1			// ..
				 , LC_HANDLE p2			// ..
				 , LC_HANDLE p3			// No Use
				 , LC_HANDLE p4			// No Use
				 )
{
	INT			hr = 0;
	CLctXml*	pObj = NULL;

	*pOut = NULL;
	pObj = new CLctXml;

	hr = pObj->Create(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_EFAIL;
	}


	*pOut = pObj;
	return LC_OK;
}


