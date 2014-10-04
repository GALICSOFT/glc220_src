// Implementation of the CLcxGuiT class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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



////////////////////////////////////////////////////////////////////////////////
//for dialog


void UsGui::_LcxGuiSortDialog(CLcxGui* pDlg, INT bFront)
{
	lsPLCXGUI* pDlgRoot= UsGui::_LcxGuiGetDlgRoot();

	lsPLCXGUI tRoot;
	itPLCXGUI F = pDlgRoot->begin();
	itPLCXGUI L = pDlgRoot->end();

	// copy root dialog
	for(; F != L; ++F)
	{
		if( pDlg == (*F))
			continue;

		tRoot.push_back( *F );
	}

	// clear root list
	pDlgRoot->clear();


	// front에 복사.
	if(bFront)
		pDlgRoot->push_back(pDlg);

	// copy root dialog
	pDlgRoot->insert(pDlgRoot->end(), tRoot.begin(), tRoot.end() );


	// end에 복사.
	if(!bFront)
		pDlgRoot->push_back(pDlg);
}


void UsGui::_LcxGuiSubstractRootDialog(CLcxGui* pDlg)
{
	lsPLCXGUI* pDlgRoot= UsGui::_LcxGuiGetDlgRoot();

	itPLCXGUI F = pDlgRoot->begin();
	itPLCXGUI L = pDlgRoot->end();


	for(; F != L; ++F)
	{
		if( pDlg == (*F))
		{
			pDlgRoot->erase(F);
			break;
		}
	}
}



INT UsGui::_LcxGui_Parse(std::vector<UsGui::TguiRscInf >& GuiRscLst, char* Texture, char* sFile)
{
	// Parsing Keyword
	static TguiParse Keywords[] =
	{
		{	"Rsc*"		,	strlen("Rsc*"		)	},	// ELCXGUI_RSC
		{	"Rec*"		,	strlen("Rec*"		)	},	// ELCXGUI_REC
		{	"GuiButton"	,	strlen("GuiButton"	)	},	// ELCXGUI_BUTTON
		{	"GuiSysBtn"	,	strlen("GuiSysBtn"	)	},	// ELCXGUI_SYSBUTTON
		{	"GuiDialog"	,	strlen("GuiDialog"	)	},	// ELCXGUI_DIALOG

		{	"RectLRBT"	,	strlen("RectLRBT"	)	},	// ELCXGUI_RCLRBT
		{	"Texture"	,	strlen("Texture"	)	},	// ELCXGUI_TEXTURE
		{	"RscId"		,	strlen("RscId"		)	},	// ELCXGUI_ID
		{	"Index"		,	strlen("Index"		)	},	// ELCXGUI_INDEX
	};


	FILE*	fp;

	fp = fopen(sFile, "rt");

	if(NULL == fp)
		return LC_CANNT_FIND_RESOURCE;


	//
	char	sLine[MAX_PARSE_LINE];
	char*	nRead = NULL;

	int		bRectLRBT = 0;


	// gather the resource
	while(!feof(fp))
	{
		nRead = fgets(sLine, MAX_PARSE_LINE, fp);
		if(NULL == nRead)	// error or EOF
			break;

		LcStr_Trim(sLine);

		// Read Rect LRBT Option
		if( 0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_RCLRBT].sKey, Keywords[ELCXGUI_RCLRBT].iKey))
		{
			sscanf(sLine, "%*s   %d", &bRectLRBT);
		}


		// Read Texture File
		else if( 0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_TEXTURE].sKey, Keywords[ELCXGUI_TEXTURE].iKey))
		{
			LcStr_Quot(Texture, sLine);
		}

		// Read Button Resource
		else if( 0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_BUTTON].sKey, Keywords[ELCXGUI_BUTTON].iKey))
		{
			char	sName[64]={0};
			int		Index=-1;
			int		Rsc  =-1;

			TguiRscInf	guiRsc;
			memset(&guiRsc, 0, sizeof(TguiRscInf) );


			LcStr_Quot(sName, sLine);
			strcpy(guiRsc.Name, sName);
			guiRsc.Type = LCXGUI_BUTTON;


			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				if(NULL == nRead)	// error or EOF
					break;

				LcStr_Trim(sLine);

				if('}' == sLine[0])
					break;


				if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_INDEX].sKey, Keywords[ELCXGUI_INDEX].iKey) )
					sscanf(sLine, "%*s   %d", &Index);


				else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_RSC].sKey, Keywords[ELCXGUI_RSC].iKey) )
				{
					Rsc++;

					FLOAT l, t, r, b;
					sscanf(sLine, "%*s   %f %f %f %f", &l, &t, &r, &b);
					guiRsc.Rts[Rsc] = LCXRECT(l, t, r, b);

					if(bRectLRBT)
						guiRsc.Rts[Rsc].ToRegion();
				}
			}


			// add the guiRsc to list and increase the guiRsc index
			GuiRscLst.push_back(guiRsc);
		}

		// Read System Button Resource
		else if( 0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_SYSBTN].sKey, Keywords[ELCXGUI_SYSBTN].iKey))
		{
			char	sName[64]={0};
			int		Index=-1;
			int		Rsc  =-1;

			TguiRscInf	guiRsc;
			memset(&guiRsc, 0, sizeof(TguiRscInf) );


			LcStr_Quot(sName, sLine);
			strcpy(guiRsc.Name, sName);
			guiRsc.Type = LCXGUI_SYSBUTTON;


			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				LcStr_Trim(sLine);

				if('}' == sLine[0])
					break;


				if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_INDEX].sKey, Keywords[ELCXGUI_INDEX].iKey) )
					sscanf(sLine, "%*s   %d", &Index);


				else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_RSC].sKey, Keywords[ELCXGUI_RSC].iKey) )
				{
					Rsc++;

					FLOAT l, t, r, b;
					sscanf(sLine, "%*s   %f %f %f %f", &l, &t, &r, &b);
					guiRsc.Rts[Rsc] = LCXRECT(l, t, r, b);

					if(bRectLRBT)
						guiRsc.Rts[Rsc].ToRegion();
				}
			}


			// add the guiRsc to list and increase the guiRsc index
			GuiRscLst.push_back(guiRsc);
		}


		// Read Dialog Resource
		else if( 0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_DIALOG].sKey, Keywords[ELCXGUI_DIALOG].iKey) )
		{
			char	sName[64]={0};
			int		RscId = -1;
			int		Index=-1;
			int		Rsc  =-1;

			TguiRscInf	guiRsc;
			memset(&guiRsc, 0, sizeof(TguiRscInf) );


			LcStr_Quot(sName, sLine);
			strcpy(guiRsc.Name, sName);
			guiRsc.Type = LCXGUI_DIALOG;


			while(!feof(fp))
			{
				nRead = fgets(sLine, MAX_PARSE_LINE, fp);
				LcStr_Trim(sLine);

				if('}' == sLine[0])
					break;



				if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_ID].sKey, Keywords[ELCXGUI_ID].iKey) )
					sscanf(sLine, "%*s   %d", &RscId);

				else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_INDEX].sKey, Keywords[ELCXGUI_INDEX].iKey) )
					sscanf(sLine, "%*s   %d", &Index);

				else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_RSC].sKey, Keywords[ELCXGUI_RSC].iKey) )
				{
					Rsc++;

					FLOAT l, t, r, b;
					sscanf(sLine, "%*s   %f %f %f %f", &l, &t, &r, &b);
					guiRsc.Rts[Rsc] = LCXRECT(l, t, r, b);

					if(bRectLRBT && 0 != Rsc)	// Rsc = 0 is the Event Region. Event region is x,y, width, height.
						guiRsc.Rts[Rsc].ToRegion();
				}


				else if(0 == LcStr_Strnicmp(sLine, Keywords[ELCXGUI_REC].sKey, Keywords[ELCXGUI_REC].iKey) )
				{
					TguiRec rec;


					int id = -1;
					int ndx =-1;
					FLOAT x, y;
					sscanf(sLine, "%*s  %d %d    %f %f", &id, &ndx, &x, &y);

					rec.id = id;
					rec.ix = ndx;
					rec.rc = LCXRECT(x, y, 10000, 10000);

					if(bRectLRBT)
						rec.rc.ToRegion();

					guiRsc.rec.push_back(rec);

				}
			}


			guiRsc.Ix = Index;
			guiRsc.Id = RscId;
			// add the guiRsc to list and increase the guiRsc index
			GuiRscLst.push_back(guiRsc);
		}

	}

	fclose(fp);


	return LC_OK;
}


