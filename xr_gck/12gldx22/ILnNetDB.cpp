// ILcxNetDB.cpp: implementation of the ILcxNetDB class.
//
////////////////////////////////////////////////////////////////////////////////


#pragma warning(disable : 4786)


#import "C:\Program Files\Common Files\System\ADO\msado15.dll" \
no_namespace rename("EOF", "EndOfFile")


#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>

#include "LnxNet.h"
#include "LnNetDB.h"
#include "LnNetOdbc.h"
#include "LnNetOledb.h"


INT LcDB_Create(char* sCmd
				, ILcxNetDB** pData
				,	void* p1		// No Use
				,	void* p2		// No Use
				,	void* p3		// No Use
				,	void* p4		// No Use
				)
{
	ILcxNetDB* pObj = NULL;

	*pData = NULL;
	
	if(0==_stricmp("ODBC", sCmd))
	{
		pObj = new CLcNetOdbc;
	}

	else if(0==_stricmp("OLDEDB", sCmd))
	{
		pObj = new CLcNetOledb;
	}
	else
	{
		return -1;
	}



	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}

	*pData = pObj;
	return 0;
}


INT LcDB_CreateAndConnect(char* sCmd, ILcxNetDB** pData, void* p1, void* p2, void* p3, void* p4, void* p5)
{
	*pData = NULL;
	
	if(0==_stricmp("ODBC", sCmd))
	{
		CLcNetOdbc * pDB=NULL;

		pDB = new CLcNetOdbc;

		if(FAILED(pDB->Create(p1, p2, p3, p4)))
		{
			pDB->Close();
			delete pDB;
			return -1;
		}

		*pData = pDB;
		return 0;
	}

	else if(0==_stricmp("OLEDB", sCmd))
	{
		CLcNetOledb * pDB=NULL;

		pDB = new CLcNetOledb;

		if(p5)
			pDB->Query("Set Host", p5);

		if(FAILED(pDB->Create(p1, p2, p3, p4)))
		{
			pDB->Close();
			delete pDB;
			return -1;
		}

		*pData = pDB;
		return 0;
	}

	return -1;
}
