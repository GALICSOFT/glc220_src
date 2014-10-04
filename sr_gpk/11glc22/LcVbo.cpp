// Implementation of the ILC_VB, ILC_IB.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// caution!!!
//#if defined(_PC_LINUX_)
//  #include <GL/gl.h>
//#elif defined(__APPLE__) || defined(__IPHONE__)
//  #import <OpenGLES/ES1/gl.h>
//#else
//  #include <gles/gl.h>
//#endif

#if defined(__APPLE__) || defined(__IPHONE__)
  #import <OpenGLES/ES1/gl.h>

#elif defined(__BADA__)
  #include <FGraphicsOpengl.h>
  using namespace Osp::Graphics::Opengl;

#else
  #include <gles/gl.h>

#endif

#include <LcType.h>
#include <LcMath.h>

#include "LcBase.h"
#include "LcAux.h"



////////////////////////////////////////////////////////////////////////////////
//
// Vertex Buffer

class CLcVtxBuf : public ILC_VB
{
protected:
	UINT			m_nID;
	UINT			m_dFVF;

public:
	CLcVtxBuf();
	virtual ~CLcVtxBuf();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);// inherit from the ILC_Object
	virtual INT		Release();							// inherit from the ILC_Object


	virtual INT		Fill(LC_HANDLE Buf,INT BufSize,INT ReCreate=LC_FALSE);
	virtual INT		Bind(INT bBind=LC_TRUE);
public:
	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	void	Destroy();

protected:
//	INT		Invalid();
//	INT		Restore();
};


// Index Buffer
class CLcIdxBuf : public ILC_IB
{
protected:
	UINT			m_nID;
	UINT			m_dFVF;

public:
	CLcIdxBuf();
	virtual ~CLcIdxBuf();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);// inherit from the ILC_Object
	virtual INT		Release();							// inherit from the ILC_Object


	virtual INT		Fill(LC_HANDLE Buf,INT BufSize,INT ReCreate=LC_FALSE);
	virtual INT		Bind(INT bBind=LC_TRUE);
	virtual UINT	GetFmt()			{	return m_dFVF;	}
public:
	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	void	Destroy();
};






////////////////////////////////////////////////////////////////////////////////
//
// Vertex Buffer
CLcVtxBuf::CLcVtxBuf()
{
	m_nID		= 0;
	m_dFVF		= 0;
}


CLcVtxBuf::~CLcVtxBuf()
{
	Destroy();
}


INT CLcVtxBuf::Query(char* sCmd, LC_HANDLE pData)
{
	if( 0 == LcStr_Stricmp(sCmd, LCQUERY_GET_OWNER))
	{
		if(NULL == m_Own)
			return LC_EFAIL;

		if(pData)
			*((LC_HANDLE*)pData) = m_Own;
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcVtxBuf::Release()
{
	INT hr = 0;

	void* pOwn = NULL;
	hr = this->Query(LCQUERY_GET_OWNER, &pOwn);
	if(LC_SUCCEEDED(hr))	// if there is owner then this cannot be released
		return LC_EFAIL;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}



void CLcVtxBuf::Destroy()
{
	if(0 == m_nID)
		return;

	glDeleteBuffers(1, &m_nID);
	m_nID = 0;
}


INT CLcVtxBuf::Create(void* p1, void* p2, void* p3, void* p4)
{
	INT		hr	= LC_OK;

	char*	pBuf = NULL;
	INT		nBuf = 0;

	if(p1)	m_dFVF = *(UINT*)p1;
	if(p2)	pBuf   = (char*)p2;
	if(p3)	nBuf   = *(INT*)p3;

	glGenBuffers(1, &m_nID);

	if(0 == m_nID)
		return LC_CANNT_ALLOC;


	if(pBuf && nBuf)
		hr = Fill(pBuf, nBuf);

	return hr;
}


INT CLcVtxBuf::Fill(void* pBuf, INT nBufSize, INT bReCreateVB)
{
	UINT nID = m_nID;

	if(bReCreateVB)
		glGenBuffers(1, &nID);

	else
	{
		if(0 == nID)
			return LC_CANNT_ALLOC;
	}


	glBindBuffer(GL_ARRAY_BUFFER, nID);											// VBO 연결 시작

	if(pBuf)
		glBufferData(GL_ARRAY_BUFFER, nBufSize, pBuf, GL_STATIC_DRAW);			// 시스템 메모리에 저장된 정점 데이터를 VBO에 연결

	//	INT nSize = 0;
	//	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &nSize);

	if(bReCreateVB)
	{
		if(m_nID)
			glDeleteBuffers(1, &m_nID);

		m_nID = nID;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);											// VBO 연결 release
	return LC_OK;
}



INT CLcVtxBuf::Bind(INT bBind)
{
	if(bBind)
		glBindBuffer(GL_ARRAY_BUFFER, m_nID);
	else
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	return LC_OK;
}




INT LcDev_CreateVB(char* sCmd
			   , PLC_VB* pOut		// Output instance
			   , LC_HANDLE p1		// FVF
			   , LC_HANDLE p2		// Input Buffer
			   , LC_HANDLE p3		// Input Buffer Size(Byte)
			   , LC_HANDLE p4		// No Use
				)
{
	INT		hr = 0;
	CLcVtxBuf*	pObj = NULL;

	*pOut	= NULL;
	pObj	= new CLcVtxBuf;

	hr = pObj->Create(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}






////////////////////////////////////////////////////////////////////////////////
//
// Index Buffer

CLcIdxBuf::CLcIdxBuf()
{
	m_nID	= 0;
	m_dFVF	= 0;
}


CLcIdxBuf::~CLcIdxBuf()
{
	Destroy();
}



INT CLcIdxBuf::Query(char* sCmd, LC_HANDLE pData)
{
	if( 0 == LcStr_Stricmp(sCmd, LCQUERY_GET_OWNER))
	{
		if(NULL == m_Own)
			return LC_EFAIL;

		if(pData)
			*((LC_HANDLE*)pData) = m_Own;
	}
	else
		return LC_EFAIL;


	return LC_OK;
}


INT CLcIdxBuf::Release()
{
	INT hr = 0;

	void* pOwn = NULL;
	hr = this->Query(LCQUERY_GET_OWNER, &pOwn);
	if(LC_SUCCEEDED(hr))	// if there is owner then this cannot be released
		return LC_EFAIL;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}




void CLcIdxBuf::Destroy()
{
	if(0 == m_nID)
		return;

	glDeleteBuffers(1, &m_nID);
	m_nID = 0;
}


INT CLcIdxBuf::Create(void* p1, void* p2, void* p3, void* p4)
{
	INT		hr	= LC_OK;

	char*	pBuf = NULL;
	INT		nBuf = 0;

	if(p1)	m_dFVF = *(UINT*)p1;
	if(p2)	pBuf   = (char*)p2;
	if(p3)	nBuf   = *(INT*)p3;

	glGenBuffers(1, &m_nID);

	if(0 == m_nID)
		return LC_CANNT_ALLOC;


	if(pBuf && nBuf)
		hr = Fill(pBuf, nBuf);

	return hr;
}


INT CLcIdxBuf::Fill(void* pBuf, INT nBufSize, INT bReCreateIB)
{
	UINT nID = m_nID;

	if(bReCreateIB)
		glGenBuffers(1, &nID);

	else
	{
		if(0 == nID)
			return LC_CANNT_ALLOC;
	}


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nID);									// IBO 연결 시작

	if(pBuf)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nBufSize, pBuf, GL_STATIC_DRAW);	// 시스템 메모리에 저장된 정점 데이터를 IBO에 연결

	//	INT nSize = 0;
	//	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &nSize);

	if(bReCreateIB)
	{
		if(m_nID)
			glDeleteBuffers(1, &m_nID);

		m_nID = nID;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);									// IBO 연결 release
	return LC_OK;
}



INT CLcIdxBuf::Bind(INT bBind)
{
	if(bBind)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nID);
	else
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return LC_OK;
}




INT LcDev_CreateIB(char* sCmd
			   , PLC_IB* pOut		// Output instance
			   , LC_HANDLE p1		// Input FVF
			   , LC_HANDLE p2		// Input Buffer pointer
			   , LC_HANDLE p3		// Input Buffer Size(Byte)
			   , LC_HANDLE p4		// No Use
				)
{
	INT		hr = 0;
	CLcIdxBuf*	pObj = NULL;

	*pOut	= NULL;
	pObj	= new CLcIdxBuf;

	hr = pObj->Create(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


