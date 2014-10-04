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
#include <memory.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcMath.h>
#include <LcStdev.h>
#include <LcxEntity.h>
#include <LcxMedia.h>

#include "GameObject.h"
#include "GpApp.h"


INT CGpApp::Init()
{
	LOGI("CGpApp::Init::Begin\n");

	INT hr = 0;

	
	////////////////////////////////////////////////////////////////////////////
	// Create Texture from File: Mipmap Level = 0, Color Key = 0x0

	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex[0], (LC_HANDLE)"data/atx_background.bmp")))
		return LC_EFAIL;

	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex[1], (LC_HANDLE)"data/atx_ship.png")))
		return LC_EFAIL;

	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex[2], (LC_HANDLE)"data/atx_photon.png")))
		return LC_EFAIL;

	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex[3], (LC_HANDLE)"data/atx_villain.png")))
		return LC_EFAIL;

	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex[4], (LC_HANDLE)"data/atx_shield.png")))
		return LC_EFAIL;

	if(LC_FAILED(LcDev_CreateTextureFromFile(NULL, &m_pTex[5], (LC_HANDLE)"data/atx_bar.png")))
		return LC_EFAIL;


	////////////////////////////////////////////////////////////////////////////
	// Create Background, Effect Sound Instance.
	// (Default- 22050 Hz, 16 bit stereo)

	if(LC_FAILED(LcxSmd_CreateSoundFromFile(NULL, &m_pSnd[0], (LC_HANDLE)"data/asm_bgm.wav")))
		return LC_EFAIL;

	if(LC_FAILED(LcxSmd_CreateSoundFromFile(NULL, &m_pSnd[1], (LC_HANDLE)"data/asm_fire.wav")))
		return LC_EFAIL;

	if(LC_FAILED(LcxSmd_CreateSoundFromFile(NULL, &m_pSnd[2], (LC_HANDLE)"data/asm_explosion.wav")))
		return LC_EFAIL;


	////////////////////////////////////////////////////////////////////////////
	// Create Font Instance with Sprite

	TLC_FONT	tFont("data/mwg_b.otf", 14);		// font name: 문화고딕, Height: 14
	if(LC_FAILED(LcDev_CreateFontFromFile(NULL, &m_pFontScore, m_pSpt, &tFont)))
		return LC_EFAIL;


	m_pSnd[0]->SetRepeat();
	m_pSnd[0]->Play();


	LoadHighScore();
	ObjectInitialize();
	DrawScore();

	LOGI("CGpApp::Init::End\n");

	return LC_OK;
}


INT CGpApp::Destroy()
{
	SaveHighScore();

	INT i;

	// Release Texture
	for(i=0; i<GAME_MAX_TEXTURE; ++i)
		SAFE_RELEASE(	m_pTex[i]	);

	// Release Sound Instance
	for(i=0; i<GAME_MAX_SOUND; ++i)
		SAFE_RELEASE(	m_pSnd[i]	);


	SAFE_RELEASE(	m_pFontScore	);		// Release Font Instance

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


	if(8>m_nSkip)
	{
		++m_nSkip;
		return 0;
	}


	SystemControl();

	if (PLAYER_STATUS_DEAD != m_PlayerStatus)
		UpdatePlayer();

	UpdatePlayerMissiles();
	UpdateEnemies();
	UpdateEnemyMissiles();

	CollisionPlayerMissilesAndEnemies();
	CollisionPlayerAndEnemies();
	CollisionEnemyMissilesAndPlayer();
	ScrollBackground();

	ShieldPulse();


	if(0>=m_Player.nAlive)
		return LC_EFAIL;


	
	if(m_bShowSt)
	{
		char sbuf[128]={0};
		DOUBLE fFPS		   = m_TimeFPS;
		DOUBLE fElasedTime = m_TimeElps;

		sprintf(sbuf, "갈릭: m: %.f %.f %d, %.f(%.1f)F"
				, vcMtsPos.x, vcMtsPos.y, pMts[0], fFPS, fElasedTime);

		m_pFont->SetString(sbuf);
	}

	return LC_OK;
}



INT CGpApp::Render()
{
	if(8>m_nSkip)
		return 0;

	m_pDev->SetRenderState(LC_RS_CLEAR_COLOR, m_dClearCol);
	
	m_pDev->BeginScene();	// Prepare Rendering...
	m_pDev->Clear(LC_CLEAR_COLOR_BUFFER | LC_CLEAR_DEPTH_BUFFER);	// Clear Backbuffer

	
	m_pSpt->Begin();				// Sprite Begin

	// Draw Game Play

	DrawBackground();
	DrawMissile();
	DrawShield();
	DrawPlayer();
	DrawEnergyBar();
	DrawPlayerLife();

	m_pFontScore->DrawTxt();


	if(m_bShowSt)
		m_pFont->DrawTxt();

//	LCXRECT	rc;
//	LCXVECTOR2 vcPos;
//
//	vcPos = m_pFont->GetPosition();
//	m_pFont->GetAttrib(LC_FONT_RECT_SCALEUP, &rc);
//	vcPos.x += rc.w;
//
//	m_pFont->GetRect(&rc, 10);
//	vcPos.x = rc.x;
//	vcPos.y = rc.y;
//	m_pFont->DrawTxt(NULL, 0, -1, &vcPos);


	m_pSpt->End();				// Sprite End

	m_pDev->EndScene();			// Post Rendering...


	return LC_OK;
}


INT CGpApp::Invalid()
{
	INT i;

	m_pFont->Query(LCQUERY_INVALID, NULL);


	for(i=0; i<GAME_MAX_TEXTURE; ++i)
	{
		if(!m_pTex[i])
			continue;

		m_pTex[i]->Query(LCQUERY_INVALID, NULL);
	}

	for(i=0; i<GAME_MAX_SOUND; ++i)
	{
		if(!m_pSnd[i])
			continue;

		m_pSnd[i]->Query(LCQUERY_INVALID, NULL);
	}


	m_pFontScore->Query(LCQUERY_INVALID, NULL);

	return LC_OK;
}


INT CGpApp::Restore()
{
	INT i;

	m_pFont->Query(LCQUERY_RESTORE, NULL);


	for(i=0; i<GAME_MAX_TEXTURE; ++i)
	{
		if(!m_pTex[i])
			continue;

		m_pTex[i]->Query(LCQUERY_RESTORE, NULL);
	}

	for(i=0; i<GAME_MAX_SOUND; ++i)
	{
		if(!m_pSnd[i])
			continue;

		m_pSnd[i]->Query(LCQUERY_RESTORE, NULL);
	}


	m_pFontScore->Query(LCQUERY_RESTORE, NULL);

	return LC_OK;
}
