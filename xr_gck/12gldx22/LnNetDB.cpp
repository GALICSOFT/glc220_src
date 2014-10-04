// Implementation of the CLcNetDB class.
//
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>

#include "LnxNet.h"
#include "LnNetDB.h"



CLcNetDB::CLcNetDB()
{
	m_eType		= DB_NONE;			// DataBase Type

	memset(	m_sHst, 0, sizeof m_sHst);
	memset(	m_sDsn, 0, sizeof m_sDsn);
	memset(	m_sUid, 0, sizeof m_sUid);
	memset(	m_sPwd, 0, sizeof m_sPwd);
}


CLcNetDB::~CLcNetDB()
{
	Destroy();
}


INT CLcNetDB::Create(void*, void*, void*, void*)
{
	return -1;
}


void CLcNetDB::Destroy()
{
}


INT CLcNetDB::Query(char*, void*)
{
	return -1;
}



void CLcNetDB::Close()
{
}



INT CLcNetDB::Connect(void* pDataType, void* p1, void* p2, void* p3)
{
	return -1;
}


INT CLcNetDB::SqlBind(char* sQuery, char*** sDataBufc,INT** nDataBufc, INT nBufSize)
{
	return -1;
}


INT CLcNetDB::SqlExec()
{
	return -1;
}



INT CLcNetDB::SqlClose()
{
	return -1;
}

