// Implementation of the CGpApp class.
//
//  Created by Heesung Oh on 2011. 01. 19.
//  Copyright GALIC SOFT Inc. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1200
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>
#include <LcStdev.h>
#include <LcxEntity.h>
#include <LcxMedia.h>

#include "GpApp.h"


INT CGpApp::Init()
{
	LOGI("CGpApp::Init::Begin\n");

	INT hr = 0;

	
	// Create Texture
	UINT colorKey = 0xFF00FFFF;
	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pGameTex, (LC_HANDLE)"data/bg512x256.png", &colorKey)))
		return LC_EFAIL;


	// sound: Default- 22050 Hz, 16 bit mono

	if(LC_FAILED(LcxSmd_CreateSoundFromFile(NULL, &m_pGameSnd, (LC_HANDLE)"data/theme.ogg")))
		return LC_EFAIL;


	// font
	TLC_FONT	tFont("data/mwg_b.otf", 14);
	if(LC_FAILED(LcDev_CreateFontFromFile(NULL, &m_pGameFnt, m_pSpt, &tFont)))
		return LC_EFAIL;


	m_pGameSnd->SetRepeat();
	m_pGameSnd->Play();

	

	// read score from file
	m_score	= 0;

	PLC_FILE fp = NULL;
	hr = LcFile_Open(NULL, &fp, (char*)"data/save_info.txt", (char*)"rt");
	if(LC_FAILED(hr))
		return LC_EFAIL;

	char sline[128]={0};
	fp->ReadLine(sline, 120);
	sscanf(sline, "%d", &m_score);
	SAFE_RELEASE(	fp	);

	LOGI("CGpApp::Init::End\n");

	return LC_OK;
}


INT CGpApp::Destroy()
{

	// write score to file
	m_score	+= 100000;
	m_score	*= 4;
	PLC_FILE fp = NULL;
	INT hr = 0;
	
	hr = LcFile_Open(NULL, &fp, (char*)"data/save_info.txt", (char*)"wt");
	if(LC_FAILED(hr))
		return LC_EFAIL;

	char sline[128]={0};
	sprintf(sline, "%d\n", m_score);
	fp->WriteByte(sline, strlen(sline));
	SAFE_RELEASE(	fp	);


	// release the game resource
	SAFE_RELEASE(	m_pGameFnt	);
	SAFE_RELEASE(	m_pGameTex	);
	SAFE_RELEASE(	m_pGameSnd	);

	return LC_OK;
}


INT CGpApp::FrameMove()
{
	INT hr =0;

	// Input Update
	hr = m_pInput->Update();

	if (LC_FAILED(hr))
		return LC_EFAIL;


	LCXVECTOR3	vcMtsPos = m_pInput->GetMtsPos();
	//LC_STATE	nMtsState= m_pInput->MtsState();
	const BYTE*	pKey = m_pInput->GetKeyMap();
	const BYTE*	pMts = m_pInput->GetMtsMap();

	// Home Key
	if(pKey[LCIK_ESCAPE])
		return LC_EFAIL;


	
	if(m_bShowSt)
	{
		char sbuf[128]={0};
		DOUBLE fFPS		   = m_TimeFPS;
		DOUBLE fElasedTime = m_TimeElps;

		sprintf(sbuf, "°¥¸¯: m: %.f %.f %d, %.f(%.1f)F"
				, vcMtsPos.x, vcMtsPos.y, pMts[0], fFPS, fElasedTime);

		m_pFont->SetString(sbuf);
	}

	return LC_OK;
}



INT CGpApp::Render()
{
	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, m_dClearCol);
	
	m_pDev->BeginScene();	// Prepare Rendering...
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);	// Clear Backbuffer

	
	m_pSpt->Begin();				// Sprite Begin


	m_pSpt->Draw(m_pGameTex, NULL, NULL, NULL);

	{
		static FLOAT c=0;
		c +=GetElapsedTimeD();

		LC_RECTF	rc;
		LCXVECTOR2	s;
		LCXVECTOR2	r;
		LCXVECTOR2	t;
		LCXCOLOR	d;

		float fRadian = LCXToRadian(c);


		rc  = LC_RECTF(50,50, 150, 150);
		s	= LCXVECTOR2(2.f, 2.f);
		r	= LCXVECTOR2(0, 0);
		t	= LCXVECTOR2(300, 25);
		d	= LCXCOLOR(1,1,1,0.4F);

		r  = t + LCXVECTOR2(rc.w/2.F * s.x, rc.h/2.F * s.y);

		FLOAT fzbias = -0.5F;
		m_pSpt->SetRenderState(LC_RS_ZBIAS, *(INT*)&fzbias);
		m_pSpt->DrawEx(m_pGameTex, &rc, &t, &s, &r, fRadian, &d);
		fzbias = 0.0F;
		m_pSpt->SetRenderState(LC_RS_ZBIAS, *(INT*)&fzbias);
	}

	{
		LCXVECTOR2 pos(200, 200);
		LCXVECTOR2 scl(1.3F, 1.2F);

		m_pSpt->DrawScaling(m_pGameTex, NULL, &pos, &scl, NULL);

		m_pGameFnt->DrawTxt();
	}



	if(m_bShowSt)
		m_pFont->DrawTxt();

	m_pSpt->End();				// Sprite End

	m_pDev->EndScene();			// Post Rendering...


	return LC_OK;
}


INT CGpApp::Invalid()
{
	m_pFont->Query(LCQUERY_INVALID, NULL);

	m_pGameFnt->Query(LCQUERY_INVALID, NULL);
	m_pGameTex->Query(LCQUERY_INVALID, NULL);
	m_pGameSnd->Query(LCQUERY_INVALID, NULL);

	return LC_OK;
}


INT CGpApp::Restore()
{
	m_pFont->Query(LCQUERY_RESTORE, NULL);

	m_pGameFnt->Query(LCQUERY_RESTORE, NULL);
	m_pGameTex->Query(LCQUERY_RESTORE, NULL);
	m_pGameSnd->Query(LCQUERY_RESTORE, NULL);

	return LC_OK;
}
