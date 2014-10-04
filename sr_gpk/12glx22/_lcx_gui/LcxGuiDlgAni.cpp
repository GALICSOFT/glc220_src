// Implementation of the CLcxGuiDlgAni class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_PC_LINUX_)
  #include <GL/gl.h>

#elif defined(__APPLE__) || defined(__IPHONE__)
  #import <OpenGLES/ES1/gl.h>

#elif defined(__BADA__)
  #include <FGraphicsOpengl.h>
  using namespace Osp::Graphics::Opengl;

#else
  #include <gles/gl.h>

#endif

#include <LcType.h>
#include <LcAux.h>
#include <LcBase.h>
#include <LcMath.h>

#ifdef _LC_ORG_SRC_
  #include <LcxEntity.h>
  #include "../ILCX_Gui.h"
#else
  #include "../LcxEntity.h"
#endif

#include "LcxGui.h"
#include "LcxGuiSysBtn.h"
#include "LcxGuiDialog.h"
#include "LcxGuiDlgAni.h"



CLcxGuiDlgAni::CLcxGuiDlgAni()
{
	m_nWidth	= 320;
	m_nHeight	= 240;

	m_BgVtx		= 0;
	m_BgPos		= NULL;
	m_BgTex		= NULL;

	m_SysBtn	= NULL;
	m_SysPos	= LCXVECTOR2(0,0);
}


CLcxGuiDlgAni::~CLcxGuiDlgAni()
{
	Destroy();
}


void CLcxGuiDlgAni::Destroy()
{
	// decrease the dialog counter
	--CLcxGui::m_pDtxDlgN;

	if(0 > CLcxGui::m_pDtxDlgN)
	{
		LOGE("Error: The reference count of dialog texture is underflow.\n");
		return;
	}

	if(0 == CLcxGui::m_pDtxDlgN)
	{
		if(CLcxGui::m_pDtxDlg)
		{
			CLcxGui::m_pDtxDlg->Release();
			CLcxGui::m_pDtxDlg = NULL;
		}
	}


	SAFE_DELETE_ARRAY(	m_BgPos	);
	SAFE_DELETE_ARRAY(	m_BgTex	);


	// Release Child
	itPLCXGUI F = m_Chld.begin();
	itPLCXGUI L = m_Chld.end();

	for(; F != L; ++F)
	{
		(*F)->Release();
	}

	m_Chld.clear();
}


INT	CLcxGuiDlgAni::Create(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	INT hr = CLcxGuiDialog::Create(p1, p2, p3, p4);

	return hr;
}


INT CLcxGuiDlgAni::FrameMove()
{
	int hr = LC_OK;

	if(LCXGUI_HIDE == m_State)
		return LC_OK;


	hr = CLcxGuiDialog::FrameMove();
	if(LC_FAILED(hr))
		return hr;


	return LC_OK;
}


void CLcxGuiDlgAni::Render()
{
	CLcxGuiDialog::Render();
}


