// Interface for the CGpPhase class.
//
//  Created by Heesung Oh on 2011. 01. 19.
//  Copyright GALIC SOFT Inc. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _GpPhase_H_
#define _GpPhase_H_


class CGpPhase : public ILC_AppFrame
{
protected:
	INT			m_nPhase	;		// Phase
	LC_HANDLE	m_pL		;		// Lua Instance
	LC_HANDLE	m_pAppMain	;		// Main Program Pointer
	char		m_sF[LC_MAX_PATH];	// Script File Name

public:
	CGpPhase();
	virtual ~CGpPhase();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		Create(PLC_ARG =NULL);
	virtual INT		Cleanup()			{	return 0;	}

	virtual INT		Run(LC_HANDLE)		{	return 0;	}
	virtual INT		Proc(LC_HANDLE)		{	return 0;	}

	virtual INT		SetAttrib(UINT, LC_HANDLE){	return 0;}
	virtual INT		GetAttrib(UINT, LC_HANDLE){	return 0;}

	virtual INT		Init()				{	return 0;	}
	virtual INT		Destroy();
	virtual INT		FrameMove();
	virtual INT		Render();
};

#endif

