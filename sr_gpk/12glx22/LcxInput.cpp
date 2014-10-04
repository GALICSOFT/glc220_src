// Implementation of the ILCX_Input.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__BADA__)
  #include <FBaseSys.h>
#endif

#include <LcType.h>
#include <LcBase.h>
#include <LcStdev.h>
#include <LcMath.h>
#include <LcAux.h>

#include "LcxMedia.h"


class CLcxInput : public ILCX_Input
{
protected:
	INT			m_bAct;

	BYTE		m_KeyCur[LC_MAX_KEY];											// Current Keyboard
	BYTE		m_KeyOld[LC_MAX_KEY];											// Old Keyboard
	BYTE		m_KeyMap[LC_MAX_KEY];											// Key Map down: INPUTST_DOWN, up: INPUTST_UP, Press INPUTST_PRESS

	BYTE		m_MtsCur[LC_MAX_TSB];											// Multi-touch Screen Current Button
	BYTE		m_MtsOld[LC_MAX_TSB];											// Old Bttton
	BYTE		m_MtsMap[LC_MAX_TSB];											// Button Map down: INPUTST_DOWN, up: INPUTST_UP, Press INPUTST_PRESS

	LCXVECTOR3	m_vTsCur[LC_MAX_TSB];											// Tsb Current Position
	LCXVECTOR3	m_vTsOld[LC_MAX_TSB];											// Tsb Old Position
	LCXVECTOR3	m_vTsGrd[LC_MAX_TSB];											// Delta Tsb Position

public:
	CLcxInput();
	virtual ~CLcxInput();


	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();


	virtual INT		Update();													// Input Update
	virtual void	Reset();
	virtual void	SetActive(INT bAct=LC_TRUE){	m_bAct = bAct;	}
	virtual INT		GetActive()					{	return m_bAct;	}


	virtual const BYTE* const GetKeyMap()const
	{
		return (const BYTE*)&m_KeyMap[0];
	}

	virtual const BYTE* const GetMtsMap()const
	{
		return (const BYTE*)&m_MtsMap[0];
	}

	virtual LC_STATE	KeyDown(INT nKey)
	{
		if(!m_bAct)
			return 0;
		return (LCIK_KEYDOWN == m_KeyMap[nKey])? 1: 0;
	}

	virtual LC_STATE	KeyUp(INT nKey)
	{
		if(!m_bAct)
			return 0;
		return (LCIK_KEYUP   == m_KeyMap[nKey])? 1: 0;
	}

	virtual LC_STATE	KeyPress(INT nKey)
	{
		if(!m_bAct)
			return 0;
		return (LCIK_KEYPRESS== m_KeyMap[nKey])? 1: 0;
	}

	virtual LC_STATE	KeyState(INT nKey)
	{
		return m_KeyMap[nKey];
	}

	virtual LC_STATE	MtsDown(INT nMultiTouch=0)
	{
		if(!m_bAct)
			return 0;

		return (LCIK_KEYDOWN == m_MtsMap[nMultiTouch])? 1: 0;
	}

	virtual LC_STATE	MtsUp(INT nMultiTouch=0)
	{
		if(!m_bAct)
			return 0;

		return (LCIK_KEYUP   == m_MtsMap[nMultiTouch])? 1: 0;
	}

	virtual LC_STATE	MtsPress(INT nMultiTouch=0)
	{
		if(!m_bAct)
			return 0;

		return (LCIK_KEYPRESS== m_MtsMap[nMultiTouch])? 1: 0;
	}

	virtual LC_STATE	MtsState(INT nMultiTouch=0)
	{
		return m_MtsMap[nMultiTouch];
	}

	virtual const FLOAT* const GetMtsPos(int nMultiTouch=0)const
	{
		return (const FLOAT*)&m_vTsCur[nMultiTouch].x;
	}

	virtual const FLOAT* const GetMtsGrd(int nMultiTouch=0)const
	{
		return (const FLOAT*)&m_vTsGrd[nMultiTouch].x;
	}

	virtual LC_STATE	IsMtsMove()
	{
		if(!m_bAct)
			return 0;

		return (m_vTsCur != m_vTsOld)? 1:0;
	}

	virtual void		SetCurKey(BYTE* pKey, INT iLen = LC_MAX_KEY);
	virtual void		SetCurTsb(BYTE* pTsb, INT iLen = LC_MAX_TSB);
	virtual void		SetTsbPos(FLOAT* vcPos, int nMultiTouch=0);
	virtual LC_STATE	IsInRect(INT left, INT top, INT right, INT bottom, int nMultiTouch=0);
	virtual LC_STATE	IsInRect2(INT left, INT top, INT right, INT bottom);

public:
	INT		Create(LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL, LC_HANDLE =NULL);
	void	Destroy();
};


CLcxInput::CLcxInput()
{
	memset(m_KeyCur, 0, LC_MAX_KEY );
	memset(m_KeyOld, 0, LC_MAX_KEY );
	memset(m_KeyMap, 0, LC_MAX_KEY );

	memset(m_MtsCur, 0, LC_MAX_TSB );
	memset(m_MtsOld, 0, LC_MAX_TSB );
	memset(m_MtsMap, 0, LC_MAX_TSB );

	memset(&m_vTsCur, 0, sizeof m_vTsCur );
	memset(&m_vTsOld, 0, sizeof m_vTsOld );

	m_bAct = LC_TRUE;
}


CLcxInput::~CLcxInput()
{
	Destroy();
}

INT CLcxInput::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcxInput::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcxInput::Destroy()
{
	LcStv_InputClose();
}



INT CLcxInput::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT	hr = 0;
	//LC_HWND	hWnd	= (LC_HWND)p2;
	//LC_HANDLE* pTmp	= (LC_HANDLE*)p2;
	//printf("Recv: 0x%x 0x%x\n", pTmp[0], pTmp[1]);

	hr = LcStv_InputOpen();
	if(LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("LCX_InputOpen() Failed\n");
		return LC_CANNT_ACQUIRE_DEVICE;
	}

	LcVar_WriteP(LC_VAR_APP_INPUT, this);

	return LC_OK;
}


INT	CLcxInput::Update()
{
	INT			hr = LC_OK;
	INT			n = 0;

	// copy current to old and set the state to zero
	memcpy(m_KeyOld, m_KeyCur,	LC_MAX_KEY);
	memset(m_KeyCur,	 0,		LC_MAX_KEY);
	memset(m_KeyMap,	 0,		LC_MAX_KEY);

	memcpy(m_MtsOld, m_MtsCur,	LC_MAX_TSB);
	memset(m_MtsCur, 0,			LC_MAX_TSB);
	memset(m_MtsMap, 0,			LC_MAX_TSB);



	////////////////////////////////////////////////////////////////////////////
	// keypad process

	// Read keys
	hr = LcStv_InputReadKey(m_KeyCur);

	// Update keyboard data
	for(n=0; n<LC_MAX_KEY; ++n)
	{
		INT nOld = m_KeyOld[n];
		INT nCur = 0;

		//update current key
		m_KeyCur[n] = m_KeyCur[n] ? 1: 0;


		nCur = m_KeyCur[n];
		if		(0 == nOld && 1 ==nCur)	m_KeyMap[n] = LCIK_KEYDOWN;		// Down
		else if	(1 == nOld && 0 ==nCur)	m_KeyMap[n] = LCIK_KEYUP;		// Up
		else if	(1 == nOld && 1 ==nCur)	m_KeyMap[n] = LCIK_KEYPRESS;	// Press
	}



	////////////////////////////////////////////////////////////////////////////
	// Multi-touch Screen Data

	// Read
	for(n=0; n<LC_MAX_TSB; ++n)
	{
		TLC_INPUT_TS	ts = {0,};

		hr = LcStv_InputReadMts(n, &ts);

#if defined(_MSC_VER)
		// Lost the focus
		if(LC_CANNT_ACQUIRE_DEVICE == hr)
			continue;
#endif

		m_vTsCur[n].x = (FLOAT)ts.x;
		m_vTsCur[n].y = (FLOAT)ts.y;

		m_MtsCur[n]   = (ts.t) ? 1: 0;


		INT nOld = m_MtsOld[n];
		INT nCur = m_MtsCur[n];

		if(0 == nOld && 1 ==nCur)
			m_MtsMap[n] = LCIK_KEYDOWN;		// Down

		else if(1 == nOld && 0 ==nCur)
			m_MtsMap[n] = LCIK_KEYUP;		// Up

		else if(1 == nOld && 1 ==nCur)
			m_MtsMap[n] = LCIK_KEYPRESS;	// Press
	}


	// delta of Multi-touch Screen Data
	for(n=0; n<LC_MAX_TSB; ++n)
	{
		m_vTsGrd[n] = m_vTsCur[n] - m_vTsOld[n];
		m_vTsOld[n] = m_vTsCur[n];
	}

#if defined(_MSC_VER)

	return LC_OK;
#else

	return hr;

#endif
}


void CLcxInput::Reset()
{
	memset(m_KeyCur, 0, sizeof(m_KeyCur));
	memset(m_KeyOld, 0, sizeof(m_KeyOld));
	memset(m_KeyMap, 0, sizeof(m_KeyMap));

	memset(m_MtsCur, 0, sizeof(m_MtsCur));
	memset(m_MtsOld, 0, sizeof(m_MtsOld));
	memset(m_MtsMap, 0, sizeof(m_MtsMap));

	memset(&m_vTsCur, 0, sizeof(m_vTsCur));
	memset(&m_vTsOld, 0, sizeof(m_vTsOld));
}



void CLcxInput::SetCurKey(BYTE* pKey, INT iLen)
{
	INT i =0;
	INT n = LC_MAX_KEY;

	if(n> iLen)
		n= iLen;

	for(i=0; i<n; ++i)
		m_KeyCur[i] = pKey[i];
}

void CLcxInput::SetCurTsb(BYTE* pTsb, INT iLen)
{
	INT i =0;
	INT n = LC_MAX_KEY;

	if(n> iLen)
		n= iLen;

	for(i=0; i<n; ++i)
		m_MtsCur[i] = pTsb[i];
}


void CLcxInput::SetTsbPos(FLOAT* vcPos, int nMultiTouch)
{
	m_vTsCur[nMultiTouch].x = vcPos[0];
	m_vTsCur[nMultiTouch].y = vcPos[1];
	m_vTsCur[nMultiTouch].z = vcPos[2];
}



LC_STATE CLcxInput::IsInRect(INT left, INT top, INT right, INT bottom, int nMultiTouch)
{
	if(!m_bAct)
		return 0;

	return (	m_vTsCur[nMultiTouch].x>=left
			&&	m_vTsCur[nMultiTouch].y>=top
			&&	m_vTsCur[nMultiTouch].x<=right
			&&	m_vTsCur[nMultiTouch].y<=bottom);
}


LC_STATE CLcxInput::IsInRect2(INT left, INT top, INT right, INT bottom)
{
	if(!m_bAct)
		return 0;

	LCXVECTOR2 vcMin(+999999.0F, +999999.0F);
	LCXVECTOR2 vcMax(-999999.0F, -999999.0F);


	if(m_vTsCur[0].x < vcMin.x)
		vcMin.x = m_vTsCur[0].x;

	if(m_vTsCur[1].x < vcMin.x)
		vcMin.x = m_vTsCur[1].x;


	if(m_vTsCur[0].y < vcMin.y)
		vcMin.y = m_vTsCur[0].y;

	if(m_vTsCur[1].y < vcMin.y)
		vcMin.y = m_vTsCur[1].y;


	if(m_vTsCur[0].x > vcMax.x)
		vcMax.x = m_vTsCur[0].x;

	if(m_vTsCur[1].x > vcMax.x)
		vcMax.x = m_vTsCur[1].x;


	if(m_vTsCur[0].y > vcMax.y)
		vcMax.y = m_vTsCur[0].y;

	if(m_vTsCur[1].y > vcMax.y)
		vcMax.y = m_vTsCur[1].y;


	return (	vcMax.x >= left
			&&	vcMin.x <= right
			&&	vcMax.y >= top
			&&	vcMin.y <= bottom);
}



INT LcxEnt_CreateInput(char* sCmd
				   , PLCX_INPUT* pOut	// Output instance
				   , LC_HANDLE p1		// Window Handle
				   , LC_HANDLE p2		// No Use
				   , LC_HANDLE p3		// No Use
				   , LC_HANDLE p4		// No Use
					 )
{
	INT			hr = 0;
	CLcxInput* pObj = NULL;

	*pOut = NULL;
	pObj = new CLcxInput;

	hr = pObj->Create(p1, p2, p3, p4);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}


