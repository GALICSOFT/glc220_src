// Implementation of the CLcpDataB class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <stdio.h>
#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <LnEuclid.h>
#include <LnxEntity.h>

#include "LntEntity.h"

#include "LnpDataB.h"


#define SAFE_DELETE(p)	{	if(p){delete (p);p = NULL;	}}


CLcpDataB::~CLcpDataB()
{
}

CLcpDataB::CLcpDataB()
{
	m_ID	= 0;
	memset(m_Name, 0, sizeof m_Name);

	m_vcTrn	= VEC3(0,0,0);
	m_vcRot	= VEC4(0,1,0, 0);
	m_vcScl	= VEC3(1,1,1);


	m_vcDir	= VEC3(0,0,0);
	m_vcAcl	= VEC3(0,0,0);
	m_fSped	= 0;

	m_nHP	= 0;
	m_nMp	= 0;
}


INT CLcpDataB::Query(char* sCmd, void* pVal)
{
	if(0 == _stricmp(sCmd, "Set ID"))
	{
		m_ID = *((INT64*)pVal);
	}

	else if(0 == _stricmp(sCmd, "Set Name"))
	{
		strcpy(m_Name, (char*)pVal);
	}

	else if(0 == _stricmp(sCmd, "Get ID"))
	{
		*((INT64*)pVal) = m_ID;
	}

	else if(0 == _stricmp(sCmd, "Get Name"))
	{
		strcpy((char*)pVal, m_Name);
	}

	else
	{
		return -1;
	}

	return 0;
}


INT CLcpDataB::SetAttrib(char* sCmd, void* pVal)
{
	if(0 == _stricmp(sCmd, "Position"))
	{
		m_vcTrn = *((VEC3*)pVal);
	}
	
	else if(0 == _stricmp(sCmd, "Direction"))
	{
		m_vcDir = *((VEC3*)pVal);
	}

	else if(0 == _stricmp(sCmd, "Rotation"))
	{
		m_vcRot = *((VEC4*)pVal);
	}

	else if(0 == _stricmp(sCmd, "Scaling"))
	{
		m_vcScl = *((VEC3*)pVal);
	}

	else if(0 == _stricmp(sCmd, "Accelation"))
	{
		m_vcAcl	= *((VEC3*)pVal);
	}

	else if(0 == _stricmp(sCmd, "Speed"))
	{
		m_fSped = *((FLOAT*)pVal);
	}

	else if(0 == _stricmp(sCmd, "HP"))
	{
		m_nHP = *((INT*)pVal);
	}

	else if(0 == _stricmp(sCmd, "MP"))
	{
		m_nMp = *((INT*)pVal);
	}

	else
	{
		return -1;
	}

	return 0;
}


INT CLcpDataB::GetAttrib(char* sCmd, void* pVal)
{
	if(0 == _stricmp(sCmd, "Position"))
	{
		*((VEC3*)pVal) = m_vcTrn;
	}
	
	else if(0 == _stricmp(sCmd, "Direction"))
	{
		*((VEC3*)pVal) = m_vcDir;
	}

	else if(0 == _stricmp(sCmd, "Rotation"))
	{
		*((VEC4*)pVal) = m_vcRot;
	}

	else if(0 == _stricmp(sCmd, "Scaling"))
	{
		*((VEC3*)pVal) = m_vcScl;
	}

	else if(0 == _stricmp(sCmd, "Accelation"))
	{
		*((VEC3*)pVal) = m_vcAcl;
	}

	else if(0 == _stricmp(sCmd, "Speed"))
	{
		*((FLOAT*)pVal) = m_fSped;
	}

	else if(0 == _stricmp(sCmd, "HP"))
	{
		*((INT*)pVal) = m_nHP;
	}

	else if(0 == _stricmp(sCmd, "MP"))
	{
		*((INT*)pVal) = m_nMp;
	}

	else
	{
		return -1;
	}

	return 0;
}


