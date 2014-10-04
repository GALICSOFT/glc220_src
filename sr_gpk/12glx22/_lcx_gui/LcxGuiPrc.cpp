// Implementation of the CLcxGuiT class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__BADA__)
  #include <FBaseSys.h>
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
#include "LcxGuiDialog.h"
#include "LcxGuiSysBtn.h"
#include "LcxGuiBtn.h"
#include "LcxGuiStatic.h"


static lsPLCXGUI		g_DlgList(0);		// dialog list
static lsPLCXGUI		g_DlgRoot(0);		// root dialog list

static TLCX_GUI_EVENT	g_EventGui(0);

static CLcxGui*		g_DlgAct = NULL;	// activating dialog
static CLcxGuiDialog*	g_DlgDrg = NULL;	// draged dialog



lsPLCXGUI* UsGui::_LcxGuiGetDlgList(){	return &g_DlgList;	}
lsPLCXGUI* UsGui::_LcxGuiGetDlgRoot(){	return &g_DlgRoot;	}

void       UsGui::_LcxGuiSetEvent(TLCX_GUI_EVENT* e)
{
	if(!e)	memset(&g_EventGui, 0, sizeof(TLCX_GUI_EVENT) );
	else	memcpy(&g_EventGui, e, sizeof(TLCX_GUI_EVENT) );
}


TLCX_GUI_EVENT* UsGui::_LcxGuiGetEvent(){	return &g_EventGui;	}
PLCX_GUI LcxGui_GetActiveDialog()	{	return g_DlgAct;	}

void LcxGui_SetActiveDialog(PLCX_GUI pDlg)
{
	CLcxGui* pDialog = (CLcxGui*)pDlg;

	if( g_DlgAct  == pDialog || (pDlg && pDlg->GetParent()) )
		return;

	if(NULL == pDialog)
	{
		if(g_DlgAct)
			g_DlgAct->SetAttrib(LCXGUI_ACTIVE, (LC_HANDLE)LCXGUI_DISABLE);

		g_DlgAct = NULL;
		return;
	}

	// old active dialog
	if(g_DlgAct)
		g_DlgAct->SetAttrib(LCXGUI_ACTIVE, (LC_HANDLE)LCXGUI_DISABLE);


	g_DlgAct = pDialog;
	g_DlgAct->SetAttrib(LCXGUI_ACTIVE, (LC_HANDLE)LCXGUI_ENABLE);

	UsGui::_LcxGuiSortDialog(g_DlgAct);
}

void      UsGui::_LcxGuiSetDragDialog(CLcxGui* pDlg){	g_DlgDrg = (CLcxGuiDialog*)pDlg;	}
CLcxGui*  UsGui::_LcxGuiGetDragDialog(){	return g_DlgDrg;	}


////////////////////////////////////////////////////////////////////////////////
// Remove All Gui Dialog
INT LcxGui_DestroyAllDialog()
{
	INT i = 0;
	CLcxGuiDialog* pDlg = NULL;

	//int iSize = (int)g_DlgRoot.size();
	//for(i=0; i<iSize; ++i)

	// dialog의 release를 호출하면 root에서 제거됨. 따라서 size로 비교해야 함
	for(i=0; i<(int)g_DlgRoot.size(); ++i)
	{
		pDlg = (CLcxGuiDialog*)g_DlgRoot[i];
		pDlg->Release();
	}

	g_DlgRoot.clear();
	g_DlgList.clear();


	memset(&g_EventGui, 0, sizeof(g_EventGui) );
	g_DlgAct = NULL;
	g_DlgDrg = NULL;

	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////
//for dialog

void LcxGui_AddToDialogList(PLCX_GUI pDlg)
{
	itPLCXGUI F = g_DlgList.begin();
	itPLCXGUI L = g_DlgList.end();

	if(pDlg->GetParent())
		return;

	for(; F != L; ++F)
	{
		if( pDlg == (*F))
			return;
	}

	g_DlgList.push_back( (CLcxGui*)pDlg);
}

void LcxGui_AddToRootDialogList(PLCX_GUI pDlg)
{
	itPLCXGUI F = g_DlgRoot.begin();
	itPLCXGUI L = g_DlgRoot.end();

	if(pDlg->GetParent())
		return;

	for(; F != L; ++F)
	{
		if( pDlg == (*F))
			return;
	}

	g_DlgRoot.push_back((CLcxGui*)pDlg);
}


void LcxGui_RemoveDialog(PLCX_GUI pGui)
{
	itPLCXGUI F;
	itPLCXGUI L;

	PLCX_GUI prnt = (PLCX_GUI)pGui->GetParent();
	if(prnt)
		prnt->Detatch(pGui);


	F = g_DlgRoot.begin();
	L = g_DlgRoot.end();

	for(; F != L; ++F)
	{
		PLCX_GUI pDlg = (*F);
		if( pGui == pDlg)
		{
			g_DlgRoot.erase(F);
			break;
		}
	}


	F = g_DlgList.begin();
	L = g_DlgList.end();

	for(; F != L; ++F)
	{
		PLCX_GUI pDlg = (*F);
		if( pGui == pDlg)
		{
			g_DlgList.erase(F);
			break;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Gui Process
INT  LcxGui_Process(TLCX_GUI_EVENT* pEventInOut)
{
	INT hr = 0;

	// receive event
	INT       EvMsg = pEventInOut->_m;
	CLcxGui* ActDlgOld = g_DlgAct;

	itPLCXGUI F;
	itPLCXGUI L;


//	LCXVECTOR2 ptEvnt;	// Event position

//1. 활성화 비활성화 다이얼로그 결정.
//   1.1 먼저 마우스가 활성화된 다이얼로그 영역에 있는지 검사. 영역에 없으면
//       이 다이얼로그는 비활성화.
//   1.2 비화성화 다이얼로그에서 순서대로 다이얼로그 영역에 있는지
//       비활성화 다이얼로그의 처음부터 검사. 영역에 있으면 이 다이얼로그를 활성화
//
//2. 활성화된 다이얼로그가 없으면 함수(loop)를 빠져 나감.
//   2.1 활성화 다이얼로그를 맨 앞에 둔다.(sorting)
//
//3. 활성화된 다이얼로그에서 해당 이벤트를 적용할 수 있는 GUI 검사.
//   3.1 있다면 해당 이벤트를 적용할 수 있는 GUI 검사.
//   3.2 gui에서 이벤트 처리
//   3.3 loop를 빠져 나감.

	// set the parent dialog is activating and


//	// Clear Root Dialogs
//	itPLCXGUI F = g_DlgList.begin();
//	itPLCXGUI L = g_DlgList.end();
//
//	g_DlgRoot.clear();
//
//	// gather the root dialogs
//	// 이부분은 다이얼로그 생성할 때만 필요
//	for(; F != L; ++F)
//	{
//		if( NULL == (*F)->GetParent() )
//			g_DlgRoot.push_back((*F));
//	}



	// setup the event position
	LCXVECTOR2* EvPos  = (LCXVECTOR2*)&pEventInOut->_sf[0];


	// Clear Event Object
	UsGui::_LcxGuiSetEvent(NULL);


	// 다이얼로그 이벤트 적용 알고리즘은 여기서부터 시작
	F = g_DlgRoot.begin();
	L = g_DlgRoot.end();


	// Drag mode
	if(g_DlgDrg)
	{
		// Set the drag dialog to active dialog
		LcxGui_SetActiveDialog(g_DlgDrg);
		g_DlgDrg->SetPositionRef(pEventInOut->_sf);

		if(0 == EvMsg)
		{
			g_DlgDrg = NULL;
			return LC_OK;
		}


		hr = (EvMsg & LCXGUI_EVNT) &&
			 (EvMsg & 0x0F) == LCXLOWORD(LCXGUI_EVNT_UP);

		if(hr)
		{
			g_DlgDrg = NULL;
			return LC_OK;
		}

		pEventInOut->SetUp(g_DlgDrg, LCXGUI_DIALOG, LCXGUI_DRAG);
		return 1;
	}



	//1. 활성화 비활성화 다이얼로그 결정.
	if(g_DlgAct &&
		(EvMsg & 0x0F) == LCXLOWORD(LCXGUI_EVNT_DOWN)  )
	{
		CLcxGui*  pGui = g_DlgAct;


		LCXRECT*   pRc = (LCXRECT*)pGui->GetRectEvent();
		LCXVECTOR2 pos = pGui->GetPositionAbs();

		hr = LCXIsInRect(*EvPos, pos.x, pos.y, pRc->w, pRc->h);

		// Remove Active Dialog
		if(LC_FAILED(hr))
			LcxGui_SetActiveDialog(NULL);
	}


//   1.1 먼저 마우스가 활성화된 다이얼로그 영역에 있는지 검사. 영역에 없으면
//       이 다이얼로그는 비활성화.

	if(NULL == g_DlgAct &&
		(EvMsg & 0x0F) == LCXLOWORD(LCXGUI_EVNT_DOWN)  )
	{
		// 다이얼로그 이벤트 적용 알고리즘은 여기서부터 시작
		F = g_DlgRoot.begin();
		L = g_DlgRoot.end();

		for(; F != L; ++F)
		{
			CLcxGui* pGui = (*F);
			INT		  State = pGui->GetState();

			if( LCXGUI_HIDE == State)
				continue;


			LCXRECT*   pRc = (LCXRECT*)pGui->GetRectEvent();
			LCXVECTOR2 pos = pGui->GetPositionAbs();

			hr = LCXIsInRect(*EvPos, pos.x, pos.y, pRc->w, pRc->h);

			//LOGE("Active DialogF: %f    %.f %.f    %.f %.f    %x \n"
			//		,pGui->m_dcColorA, pos.x, pos.y, pRc->w, pRc->h, pGui);

			if( LC_SUCCEEDED(hr) )
			{
				LcxGui_SetActiveDialog(pGui);
				break;
			}
		}
	}

	F = g_DlgRoot.begin();
	L = g_DlgRoot.end();

	//2. 활성화된 다이얼로그가 없으면 함수(loop)를 빠져 나감.
	if(NULL == g_DlgAct)
		return LC_OK;


	// 2.1 활성화 다이얼로그를 맨 앞에 둔다.(sorting)
	// sort dialog: 이미 LcxGui_SetActiveDialog()함수에서 처리됨.


	//3. 활성화된 다이얼로그에서 해당 이벤트를 적용할 수 있는 GUI 검사.
	g_DlgAct->SetAttrib(LCXGUI_EVNT, pEventInOut);

	hr = g_DlgAct->FrameMove();


	//4. 활성화된 다이얼로그의 종료 이벤트
	// 자신의 핸들을 반환.
	if(LCXGUI_EVNT_CLOSE == hr)
	{
		pEventInOut->SetUp(g_DlgAct, LCXGUI_DIALOG, LCXGUI_EVNT_CLOSE);
		LcxGui_SetActiveDialog(NULL);
		return 1;
	}

	// receive Gui Event in Dialog
	hr = g_DlgAct->GetAttrib(LCXGUI_EVNT, pEventInOut);

	if(LC_SUCCEEDED(hr) && pEventInOut->_o)
		return 1;

	if(g_DlgAct && g_DlgAct != ActDlgOld)
	{
		pEventInOut->SetUp(g_DlgAct, LCXGUI_DIALOG, LCXGUI_ACTIVE);
		return 1;
	}

	return LC_OK;
}


// Gui Process
void LcxGui_Render()
{
	rtPLCXGUI F = g_DlgRoot.rbegin();
	rtPLCXGUI L = g_DlgRoot.rend();

	for(; F != L; ++F)
	{
		CLcxGui* pGui = (*F);
		INT		  State = pGui->GetState();

		if( LCXGUI_HIDE == State)
			continue;

		pGui->Render();
	}
}




////////////////////////////////////////////////////////////////////////////////
// Gui Create/Destroy

INT LcxEnt_CreateGui(char*       sCmd		// Command(sCmd).
					, PLCX_GUI*   pOut		// Output data
					, LC_HANDLE    p1		// Gui Info TLCX_GUI* Pointer
					, LC_HANDLE    p2		// Gui extern data
					, LC_HANDLE    p3		// No Use
					, LC_HANDLE    p4		// No Use
					)
{
	INT		 hr   = 0;
	PLCX_GUI pObj = NULL;

	*pOut	= NULL;



	if(NULL == sCmd || strlen(sCmd)<4)
		return LC_EFAIL;


	// Original
	if(0 == LcStr_Stricmp(sCmd, LCXCMD_GUI_DIALOG))
	{
		pObj = new CLcxGuiDialog;
		hr = ((CLcxGuiDialog*)pObj)->Create(p1, p2, p3, p4);

		if(LC_SUCCEEDED(hr))
		{
			LcxGui_AddToDialogList(pObj);
			LcxGui_AddToRootDialogList(pObj);
			LcxGui_SetActiveDialog(pObj);
		}
	}

	else if(0 == LcStr_Stricmp(sCmd, LCXCMD_GUI_SYSBTN))
	{
		pObj = new CLcxGuiSysBtn;
		hr = ((CLcxGuiSysBtn*)pObj)->Create(p1, p2, p3, p4);
	}

	else if(0 == LcStr_Stricmp(sCmd, LCXCMD_GUI_BUTTON))
	{
		pObj = new CLcxGuiBtn;
		hr = ((CLcxGuiBtn*)pObj)->Create(p1, p2, p3, p4);
	}

	else if(0 == LcStr_Stricmp(sCmd, LCXCMD_GUI_STATIC))
	{
		pObj = new CLcxGuiStatic;
		hr = ((CLcxGuiStatic*)pObj)->Create(p1, p2, p3, p4);
	}

	else
		return LC_EFAIL;



	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_EFAIL;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}




INT LcxEnt_CreateGuiListFromFile(char*	  sCmd	// No Use: just null
							, PLCX_GUI**  pLst	// Output data List. Return value is the count of List
							, LC_HANDLE   sTex	// output texture file
							, LC_HANDLE   File	// file name
							, LC_HANDLE    p3	// No Use
							, LC_HANDLE    p4	// No Use
							)
{
	PLCX_GUI* pObj = NULL;
	int		i=0, j=0, hr =0;

	char	sPath[LC_MAX_PATH ]={0};
	char	drive[LC_MAX_DRIVE]={0};
	char	dir  [LC_MAX_DIR  ]={0};
	char	fname[LC_MAX_FNAME]={0};
	char	exet [LC_MAX_EXT  ]={0};


	*pLst = NULL;


	std::vector<PLCX_GUI >			vTmpGui(0);
	int								nTmpGui(0);
	std::vector<UsGui::TguiRscInf >	GuiRscLst(0);
	char	tTex[LC_MAX_PATH2] ={0};


	// split
	LcStr_SplitPath((const char*)File, drive, dir, fname, exet);
	sprintf(sPath, "%s%s", drive, dir);


	hr = UsGui::_LcxGui_Parse(GuiRscLst, tTex, (char*)File);
	if(LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("It needs sprite and file name.\n");
		return LC_EFAIL;
	}

	if(GuiRscLst.empty())
		return LC_EFAIL;

	int iSizeRsc = GuiRscLst.size();
	int iSizeRec = 0;


	// create texture
	TLC_IMAGE	txInf={0};
	PLC_IMAGE	pImg= NULL;

	sprintf((char*)sTex, "%s%s",  sPath, tTex);
	hr = LcDev_CreateImageFromFile(NULL, &pImg, (char*)sTex);

	if(LC_FAILED(hr))
	{
		LOGE("Err: %d, %s \t", __LINE__, __FILE__);
		LOGE("Can't find image file.\n");
		return LC_EFAIL;
	}


	txInf.ImgW = pImg->GetImgW();
	txInf.ImgH = pImg->GetImgH();
	txInf.ImgD = pImg->GetImgD();
	SAFE_RELEASE(	pImg	);


	// create dialog
	for(i=0; i<iSizeRsc; ++i)
	{
		UsGui::TguiRscInf* Rsc = &GuiRscLst[i];
		iSizeRec = Rsc->rec.size();

		if(LCXGUI_DIALOG != Rsc->Type)
			continue;

		// Create Dialog
		TLCX_GUI_INF	inf;
		TLCX_GUI_EXTERN ext;

		PLCX_GUI     Dlg  = NULL;			// Gui Dialog

		// Create Dialogs
		inf.sName = Rsc->Name;


		LCXRECT  Rects[32];
		Rects[ 0] = Rsc->Rts[1];	// title
		Rects[12] = Rsc->Rts[2];	// body


		// setup system button
		for(j=0; j<iSizeRec; ++j)
		{
			UsGui::TguiRec*    prec = &Rsc->rec[j];				// record
			UsGui::TguiRscInf* prsc = &GuiRscLst[prec->ix];		// resource

			if(LCXGUI_SYSBUTTON == prsc->Type)
			{
				Rects[ 4] = prec->rc;						// system button position(음수면 button의 위치는 변하지 않음.) LCXRect2Region()에서 계산 되므로 음수가 안되도록 아주 큰 값을 선택
				Rects[ 5] = prsc->Rts[0];					// system button normal
				Rects[ 6] = prsc->Rts[1];					// system button over
				Rects[ 7] = prsc->Rts[2];					// system button pressed
				break;
			}
		}

		ext.txInf = &txInf;
		ext.rEvnt = &Rsc->Rts[0];	// event
		ext.rRcts =  Rects;

		// create dialog with extern data
		hr = LcxEnt_CreateGui(LCXCMD_GUI_DIALOG, &Dlg, &inf, &ext);
		vTmpGui.push_back(Dlg);



		// other record (button, static) and attatch to dialog
		for(j=0; j<iSizeRec; ++j)
		{
			UsGui::TguiRec*    prec = &Rsc->rec[j];				// record
			UsGui::TguiRscInf* prsc = NULL;


			prsc = &GuiRscLst[prec->ix];		// resource


			TLCX_GUI_INF    _inf;
			TLCX_GUI_EXTERN _ext;
			PLCX_GUI	pGui=NULL;

			if(LCXGUI_SYSBUTTON == prsc->Type)
				continue;


			if(LCXGUI_BUTTON == prsc->Type)
			{
				_inf.sName = (char*)"button3";
				_inf.nId   = prec->id;

				_ext.txInf = &txInf;
				_ext.rEvnt = &prec->rc;
				_ext.rRcts= prsc->Rts;

				hr = LcxEnt_CreateGui(LCXCMD_GUI_BUTTON, &pGui, &_inf, &_ext);
				Dlg->Attach(pGui);

				vTmpGui.push_back(pGui);
			}
		}


		// Create Resouce and attatch to dialog
//		for(int j=0; j<iSizeRec; ++j)
//		{
//			TguiRec* Rec = &Rsc->rec[j];
//			vTmpGui.push_back(pGui);
//
//		}
	}


	// setup the return value
	nTmpGui = vTmpGui.size();

	pObj = new PLCX_GUI[nTmpGui];
	for(i=0; i<nTmpGui; ++i)
		pObj[i] = vTmpGui[i];


	GuiRscLst.clear();
	vTmpGui.clear();

	*pLst = pObj;

	return nTmpGui;
}

