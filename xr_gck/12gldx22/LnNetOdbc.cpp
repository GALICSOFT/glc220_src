// Implementation of the CLcNetOdbc class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>

#include "LnxNet.h"
#include "LnNetDB.h"
#include "LnNetOdbc.h"



CLcNetOdbc::CLcNetOdbc()
{
	m_nField	= 0;
	m_pDataBuf	= NULL;
	m_nDataBuf	= NULL;
}


CLcNetOdbc::~CLcNetOdbc()
{
	Destroy();
}


INT CLcNetOdbc::Create(void* p1, void* p2, void* p3, void* p4)
{
	SQLRETURN	hr;
	
	hr = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&m_hEnv);				// 환경 핸들을 할당하고 버전 속성을 설정한다.
	
	if (FAILED(hr))
		return -1;
	
	hr = SQLSetEnvAttr(m_hEnv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);
	
	if (FAILED(hr))
		return -1;
	
	hr = SQLAllocHandle(SQL_HANDLE_DBC,m_hEnv,&m_hDbc);							// 연결 핸들을 할당하고 연결한다.
	
	if (FAILED(hr))
		return -1;

	if(p1)
		hr = Connect(p1, p2, p3, p4);

	if (FAILED(hr))
		return -1;
	
	return 0;
}


void CLcNetOdbc::Destroy()
{
	if (m_hEnv)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,m_hEnv);
		m_hEnv = 0;
	}
}



INT CLcNetOdbc::Query(char*, void*)
{
	return -1;
}


void CLcNetOdbc::Close()
{
	if (m_hStm)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hStm);
		m_hStm = 0;
	}
	
	if (m_hDbc)
	{
		SQLDisconnect(m_hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC,m_hDbc);
		m_hDbc = 0;
	}
}



INT CLcNetOdbc::Connect(void* pDataType, void* p1, void* p2, void* p3)
{
	SQLRETURN	hr;
	char*	sType = (char*)pDataType;

	hr = SQLAllocHandle(SQL_HANDLE_DBC,m_hEnv,&m_hDbc);							// 연결 핸들을 할당하고 연결한다.
	
	if (FAILED(hr))
		return -1;

	if(0==_stricmp("mdb", sType))			m_eType	= DB_MDB;
	else if(0==_stricmp("dsn", sType))		m_eType	= DB_DSN;
	else if(0==_stricmp("sql", sType))		m_eType	= DB_SQL;
	else if(0==_stricmp("excel", sType))	m_eType	= DB_EXL;


	if(DB_NONE == m_eType)
		return -1;

	else if(DB_MDB == m_eType)
		return ConnectMdb((char*)p1);

	else if(DB_DSN == m_eType)
		return ConnectDsn((char*)p1);

	else if(DB_SQL == m_eType)
		return ConnectSql((char*)p1, (char*)p2, (char*)p3);

	else if(DB_EXL == m_eType)
		return ConnectExcel((char*)p1);
	
	return -1;
}



// DSN 연결
INT CLcNetOdbc::ConnectDsn(char* sDsn)
{
	SQLCHAR		InCon[260]={0};
	SQLRETURN	hr;

	sprintf((char*)InCon, sDsn);
	
	hr=SQLConnect(m_hDbc, InCon, SQL_NTS, (SQLCHAR*)"", SQL_NTS, (SQLCHAR*)"", SQL_NTS);
	
	if (FAILED(hr))
		return -1;
	
	// 명령 핸들을 할당한다.
	hr = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStm);
	
	if (FAILED(hr))
		return -1;
	
	return 0;
}


// SQL 서버 연결
INT CLcNetOdbc::ConnectSql(char* sDsn, char* sUID, char* sPWD)
{
	SQLCHAR		InCon[260]={0};
	SQLCHAR		sqUID[128]={0};
	SQLCHAR		sqPWD[128]={0};
	SQLRETURN	hr;

	sprintf((char*)InCon, sDsn);
	sprintf((char*)sqUID, sUID);
	sprintf((char*)sqPWD, sPWD);
	
	hr=SQLConnect(m_hDbc, InCon, SQL_NTS, sqUID, SQL_NTS, sqPWD, SQL_NTS);
	
	if (FAILED(hr))
		return -1;
	
	// 명령 핸들을 할당한다.
	hr = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStm);
	
	if (FAILED(hr))
		return -1;
	
	
	return 0;
}


// 엑셀 파일 연결
INT CLcNetOdbc::ConnectExcel(char*	sFile)
{
	SQLCHAR		InCon[260];
	SQLCHAR		OutCon[260];
	SQLSMALLINT cbOutCon;

	SQLRETURN	hr;

	sprintf((char*)InCon,"DRIVER={Microsoft Excel Driver (*.xls)};DBQ=%s;", sFile);
	
	hr=SQLDriverConnect(m_hDbc,NULL,InCon,sizeof(InCon),OutCon,sizeof(OutCon),&cbOutCon, SQL_DRIVER_NOPROMPT);
	
	if (FAILED(hr))
		return -1;
	
	// 명령 핸들을 할당한다.
	hr = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStm);
	
	if (FAILED(hr))
		return -1;
	
	return 0;
}


// MDB 파일 연결
INT CLcNetOdbc::ConnectMdb(char*	sFile)
{
	SQLCHAR		InCon[260];
	SQLCHAR		OutCon[260];
	SQLSMALLINT cbOutCon;
	SQLRETURN	hr;
	
	sprintf((char*)InCon,"DRIVER={Microsoft Access Driver (*.mdb)};DBQ=%s;", sFile);
	
	hr=SQLDriverConnect(m_hDbc,NULL,InCon,sizeof(InCon),OutCon,sizeof(OutCon),&cbOutCon, SQL_DRIVER_NOPROMPT);
	
	if (FAILED(hr))
		return -1;
	
	// 명령 핸들을 할당한다.
	hr = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStm);
	
	if (FAILED(hr))
		return -1;
	
	return 0;
}


INT CLcNetOdbc::SqlBind(char* sQuery, char*** sDataBufc,INT** nDataBufc, INT nBufSize)
{
	SQLRETURN	hr;
	INT			i=0;
	SQLCHAR		sSQL[1024];
	SQLSMALLINT nCol=0;
	
	sprintf((char*)sSQL, sQuery);
	
	// EXCUTE SQL
	hr = SQLExecDirect(m_hStm, sSQL, SQL_NTS);
	
	if (FAILED(hr))
		return -1;
	
	// 열의 숫자를 받아 온다.
	SQLNumResultCols(m_hStm, &nCol);

	m_nField = nCol;

	if(nCol<=0)
		return -1;

	
	m_pDataBuf = new char*[m_nField];
	m_nDataBuf = new INT  [m_nField];

	for(i=0; i<m_nField; ++i)
	{
		m_pDataBuf[i] = new char[nBufSize];
		memset(m_pDataBuf[i], 0, nBufSize);
	}

	memset(m_nDataBuf, 0, m_nField * sizeof(INT));


	// 결과를 돌려받기 위해 바인딩하되 전부 문자열로 받는다.
	for(i=0; i<m_nField; ++i)
	{
		char*	sDataBuf= m_pDataBuf[i];
		hr = SQLBindCol(m_hStm, i+1, SQL_C_CHAR, (SQLPOINTER)(sDataBuf), nBufSize, (SQLINTEGER*)(&m_nDataBuf[i]));

		if(FAILED(hr))
			break;
	}

	if( SQL_SUCCESS != hr)
	{
		for(i=0; i<m_nField; ++i)
		{
			delete [] m_pDataBuf[i];
		}

		delete [] m_pDataBuf;
		delete [] m_nDataBuf;

		m_pDataBuf = NULL;
		m_nDataBuf = NULL;
		*sDataBufc = NULL;
		*nDataBufc = NULL;

		return -1;
	}

	*sDataBufc = m_pDataBuf;
	*nDataBufc = m_nDataBuf;
	
	return m_nField;
}


INT CLcNetOdbc::SqlExec()
{
	SQLRETURN	hr;

	hr = SQLFetch(m_hStm);
	
	if(SQL_NO_DATA ==hr)
		return -1;
		
	return 0;
}



INT CLcNetOdbc::SqlClose()
{
	SQLRETURN	hr=-1;
	hr = SQLCloseCursor(m_hStm);

	if(m_pDataBuf)
	{
		for(int i=0; i<m_nField; ++i)
			delete [] m_pDataBuf[i];

		delete [] m_pDataBuf;
		m_pDataBuf = NULL;
	}

	if(m_nDataBuf)
	{
		delete [] m_nDataBuf;
		m_nDataBuf = NULL;
	}

	m_nField  = 0;

	return hr;
}







INT CLcNetOdbc::GetColumNum()
{
	SQLCHAR		sCol[1024];
	SQLINTEGER	iLen;
	SQLRETURN	hr;
	INT			iNum=0;
	char		sqExc[256];
	
	SQLBindCol(m_hStm, 1, SQL_C_CHAR, sCol, sizeof(sCol), &iLen);
	
	hr=SQLExecDirect(m_hStm, (SQLCHAR*)sqExc, SQL_NTS);
	
	if(FAILED(hr))
		return -1;
	
	
	hr=SQLFetch(m_hStm);
	
	if(SQL_NO_DATA ==hr || SQL_ERROR==hr)
		return -1;
	
	printf("Column Num: %s\n", sCol);
	
	SQLCloseCursor(m_hStm);
	
	iNum = atoi((char*)sCol);
	
	return iNum;
}
