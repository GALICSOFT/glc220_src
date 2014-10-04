// Implementation of the Geometry Object classes.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcMath.h>
#include <LcBase.h>

#include "LcxEntity.h"
#include "LcxGeoObj.h"


////////////////////////////////////////////////////////////////////////////////
// Geometry Axis

CLcxGeoAxis::CLcxGeoAxis()
{
}

CLcxGeoAxis::~CLcxGeoAxis()
{
	Destroy();
}


INT CLcxGeoAxis::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxGeoAxis::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


INT CLcxGeoAxis::Create(LC_HANDLE p1, LC_HANDLE, LC_HANDLE, LC_HANDLE)
{
	FLOAT	fMax = 5000;

	m_pLine[ 0] = CLcxGeoAxis::Vtx(-fMax,     0,     0, 0xFF990000);
	m_pLine[ 1] = CLcxGeoAxis::Vtx(    0,     0,     0, 0xFF990000);
	m_pLine[ 2] = CLcxGeoAxis::Vtx(    0,     0,     0, 0xFFFF0000);
	m_pLine[ 3] = CLcxGeoAxis::Vtx( fMax,     0,     0, 0xFFFF0000);

	m_pLine[ 4] = CLcxGeoAxis::Vtx(    0, -fMax,     0, 0xFF009900);
	m_pLine[ 5] = CLcxGeoAxis::Vtx(    0,     0,     0, 0xFF009900);
	m_pLine[ 6] = CLcxGeoAxis::Vtx(    0,     0,     0, 0xFF00FF00);
	m_pLine[ 7] = CLcxGeoAxis::Vtx(    0,  fMax,     0, 0xFF00FF00);

	m_pLine[ 8] = CLcxGeoAxis::Vtx(    0,     0, -fMax, 0xFF000099);
	m_pLine[ 9] = CLcxGeoAxis::Vtx(    0,     0,     0, 0xFF000099);
	m_pLine[10] = CLcxGeoAxis::Vtx(    0,     0,     0, 0xFF0000FF);
	m_pLine[11] = CLcxGeoAxis::Vtx(    0,     0,  fMax, 0xFF0000FF);

	return 0;
}


void CLcxGeoAxis::Render()
{
	PLC_DEVICE	pDev = (PLC_DEVICE)LcVar_ReadP(LC_VAR_DEVICE);

	pDev->SetFVF(CLcxGeoAxis::Vtx::FVF);
	pDev->DrawPrimitiveUP(LC_PT_LINE_LIST, 0, 12, m_pLine, sizeof(CLcxGeoAxis::Vtx) );
}



////////////////////////////////////////////////////////////////////////////////
// Geometry Grid

CLcxGeoGrid::CLcxGeoGrid()
{
}

CLcxGeoGrid::~CLcxGeoGrid()
{
	Destroy();
}


INT CLcxGeoGrid::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxGeoGrid::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


INT CLcxGeoGrid::Create(LC_HANDLE p1, LC_HANDLE, LC_HANDLE, LC_HANDLE)
{
	INT		i;

	for(i=0; i<8; ++i)
	{
		m_pLine[8*i +0 ] = CLcxGeoGrid::Vtx(-128.0F,  16.0F* (i+1), -2.0F, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +1 ] = CLcxGeoGrid::Vtx( 128.0F,  16.0F* (i+1), -2.0F, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +2 ] = CLcxGeoGrid::Vtx(-128.0F, -16.0F* (i+1), -2.0F, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +3 ] = CLcxGeoGrid::Vtx( 128.0F, -16.0F* (i+1), -2.0F, (i%2)? 0xFF999999 : 0xFF666666);

		m_pLine[8*i +4 ] = CLcxGeoGrid::Vtx( 16.0F* (i+1),-128.0F, -2.0F, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +5 ] = CLcxGeoGrid::Vtx( 16.0F* (i+1), 128.0F, -2.0F, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +6 ] = CLcxGeoGrid::Vtx(-16.0F* (i+1),-128.0F, -2.0F, (i%2)? 0xFF999999 : 0xFF666666);
		m_pLine[8*i +7 ] = CLcxGeoGrid::Vtx(-16.0F* (i+1), 128.0F, -2.0F, (i%2)? 0xFF999999 : 0xFF666666);
	}

	return 0;
}


void CLcxGeoGrid::Render()
{
	PLC_DEVICE	pDev = (PLC_DEVICE)LcVar_ReadP(LC_VAR_DEVICE);

	pDev->SetFVF(CLcxGeoGrid::Vtx::FVF);
	pDev->DrawPrimitiveUP(LC_PT_LINE_LIST, 0, 72, m_pLine, sizeof(CLcxGeoGrid::Vtx) );
}

