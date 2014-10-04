// Implementation of the CLcNetOledb class.
//
////////////////////////////////////////////////////////////////////////////////


#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#pragma warning(disable : 4786)


#import "C:\Program Files\Common Files\System\ADO\msado15.dll" \
no_namespace rename("EOF", "EndOfFile")


#include <oledb.h>
#include <ole2.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>

#include "LnxNet.h"
#include "LnNetDB.h"
#include "LnNetOdbc.h"

#include "LnNetOledb.h"


_bstr_t		GetState(int intState);
void		ErrCom(_com_error &e);
void		ErrProvider(_ConnectionPtr m_pCon);

inline INT	ComValidTest(HRESULT hr)
{
	if FAILED(hr)
	{
		_com_issue_error(hr);
		return -1;
	}
	
	return 0;
}


CLcNetOledb::CLcNetOledb()
{
	m_pCon = NULL;
	m_pRsc = NULL;

	m_nField	= 0;
	m_pDataBuf	= NULL;
	m_nDataBuf	= NULL;
}


CLcNetOledb::~CLcNetOledb()
{
	Destroy();
}


INT CLcNetOledb::Create(void* p1, void* p2, void* p3, void* p4)
{
	INT hr=-1;

	if(FAILED(::CoInitialize(NULL)))
		return -1;

	if(p1)
		hr = Connect(p1, p2, p3, p4);

	if (FAILED(hr))
		return -1;

	return 0;
}


void CLcNetOledb::Destroy()
{
	if(m_pRsc)
	{
		m_pRsc->Close();
		m_pRsc = NULL;
	}
	

	if(m_pCon)
	{
		m_pCon->Close();
		m_pCon = NULL;
	}

	::CoUninitialize();
}


INT CLcNetOledb::Query(char* sCmd, void* pData)
{
	if(0==_stricmp("Set Host", sCmd))
	{
		char* sHst = (char*)pData;

		strcpy(m_sHst, sHst);
		return 0;
	}
	
	return -1;
}



void CLcNetOledb::Close()
{
	SqlClose();
	
	if(m_pRsc)
	{
		m_pRsc->Close();
		m_pRsc = NULL;
	}
}



INT CLcNetOledb::Connect(void* pDataType, void* p1, void* p2, void* p3)
{
	char*	sType = (char*)pDataType;

	if(0==_stricmp("mdb", sType))			m_eType	= DB_MDB;
	else if(0==_stricmp("dsn", sType))		m_eType	= DB_DSN;
	else if(0==_stricmp("sql", sType))		m_eType	= DB_SQL;
	else if(0==_stricmp("excel", sType))	m_eType	= DB_EXL;


	if(DB_NONE == m_eType)		return -1;
//	else if(DB_MDB == m_eType)	return ConnectMDB((char*)p1);
//	else if(DB_DSN == m_eType)	return ConnectDSN((char*)p1);
	else if(DB_SQL == m_eType)	return ConnectSql((char*)p1, (char*)p2, (char*)p3);
	else if(DB_EXL == m_eType)	return ConnectExcel((char*)p1);
	
	return -1;
}


INT CLcNetOledb::SqlBind(char* sQuery, char*** sDataBufc,INT** nDataBufc, INT nBufSize)
{
	HRESULT	hr = -1;

	try
	{
		LONG	i=0;
		char	sSQL[1024]={0};

		sprintf(sSQL, sQuery);

		// Recordset 积己
		hr = ComValidTest(m_pRsc.CreateInstance(__uuidof(Recordset)));
		
		// Recordset open
		hr = m_pRsc->Open(sSQL, m_pCon.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);


		// Print Column Name
		//long limit = m_pRsc->GetFields()->Count;
		
		FieldsPtr pFldLoop= NULL;
		
		pFldLoop	= m_pRsc->GetFields();
		m_nField	= pFldLoop->GetCount();


		
		
		// ODBC客 悼老茄 备炼肺 父电促.		
		m_pDataBuf = new char*[m_nField];
		m_nDataBuf = new INT  [m_nField];

		for(i=0; i<m_nField; ++i)
		{
			m_pDataBuf[i] = new char[nBufSize];
			memset(m_pDataBuf[i], 0, nBufSize);
		}

		memset(m_nDataBuf, 0, m_nField * sizeof(INT));


		for(i=0; i<m_nField; ++i)
		{
			_variant_t		var = pFldLoop->Item[i]->Name;
			DataTypeEnum	nType = pFldLoop->Item[i]->Type;

			m_nDataBuf[i]	= nType;

//			 _bstr_t strMsg;
//
//			switch(nType)
//			{
//				case adBigInt:			strMsg = "adBigInt";		break;
//				case adBinary:			strMsg = "adBinary";		break;
//				case adBoolean:			strMsg = "adBoolean";		break;
//				case adBSTR:			strMsg = "adBSTR";			break;
//				case adChapter:			strMsg = "adChapter";		break;
//				case adChar:			strMsg = "adChar";			break;
//				case adCurrency:		strMsg = "adCurrency";		break;
//				case adDate:			strMsg = "adDate";			break;
//				case adDBDate:			strMsg = "adDBDate";		break;
//				case adDBTime:			strMsg = "adDBTime";		break;
//				case adDBTimeStamp:		strMsg = "adDBTimeStamp";	break;
//				case adDecimal:			strMsg = "adDecimal";		break;
//				case adDouble:			strMsg = "adDouble";		break;
//				case adEmpty:			strMsg = "adEmpty";			break;
//				case adError:			strMsg = "adError";			break;
//				case adFileTime:		strMsg = "adFileTime";		break;
//				case adGUID:			strMsg = "adGUID";			break;
//				case adIDispatch:		strMsg = "adIDispatch";		break;
//				case adInteger:			strMsg = "adInteger";		break;
//				case adIUnknown:		strMsg = "adIUnknown";		break;
//				case adLongVarBinary:	strMsg= "adLongVarBinary";	break;
//				case adLongVarChar:		strMsg = "adLongVarChar";	break;
//				case adLongVarWChar:	strMsg = "adLongVarWChar";	break;
//				case adNumeric:			strMsg = "adNumeric";		break;
//				case adPropVariant:		strMsg = "adPropVariant";	break;
//				case adSingle:			strMsg = "adSingle";		break;
//				case adSmallInt:		strMsg = "adSmallInt";		break;
//				case adTinyInt:			strMsg = "adTinyInt";		break;
//				case adUnsignedBigInt:	strMsg = "adUBigInt";		break;
//				case adUnsignedInt:		strMsg = "adUnsignedInt";	break;
//				case adUnsignedSmallInt:strMsg = "adUSmallInt"	;	break;
//				case adUnsignedTinyInt:	strMsg = "adUTinyInt"	;	break;
//				case adUserDefined:		strMsg = "adUserDefined";	break;
//				case adVarBinary:		strMsg = "adVarBinary";		break;
//				case adVarChar:			strMsg = "adVarChar";		break;
//				case adVariant:			strMsg = "adVariant";		break;
//				case adVarNumeric:		strMsg = "adVarNumeric";	break;
//				case adVarWChar:		strMsg = "adVarWChar";		break;
//				case adWChar:			strMsg = "adWChar";			break;
//				default:				strMsg = "*UNKNOWN*";		break;
//			}

		}

//		for(i=0; i<m_nField; ++i)
//			printf("%s ", m_vField[i].c_str());
//
//		printf("\n");

		*sDataBufc = m_pDataBuf;
		*nDataBufc = m_nDataBuf;
		
		return m_nField;
	}
	
	catch(_com_error &e)
	{
		ErrCom(e);
		SqlClose();

		return -1;
	}

	return 0;
}


INT CLcNetOledb::SqlExec()
{
	HRESULT		hr = -1;

	_variant_t	var;
	char		sField[256]={0};
	char*		sDataBuf= NULL;
	INT			nType	= 0;

	LONG		i =0;

	try
	{
		// Print Recoredset Name
		if(m_pRsc->EndOfFile)
			return -1;

		for(i=0; i<m_nField; ++i)
		{
			memset(sField, 0, sizeof sField);
			sDataBuf= m_pDataBuf[i];
			nType	= m_nDataBuf[i];

			var = m_pRsc->Fields->Item[i]->GetValue();


			if(
				adTinyInt			== nType ||
				adSmallInt			== nType ||
				adInteger			== nType ||
				adBigInt			== nType ||
				adUnsignedTinyInt	== nType ||
				adUnsignedSmallInt	== nType ||
				adUnsignedInt		== nType ||
				adUnsignedBigInt== nType)
			{
				sprintf(sDataBuf, "%d", (_bstr_t(var.lVal)));
			}

			else if(adSingle == nType)
			{
				sprintf(sDataBuf, "%f", var.fltVal);
			}

			else if(adDouble == nType)
			{
				sprintf(sDataBuf, "%f", var.dblVal);
			}

			else if(adWChar == nType || adVarChar == nType || adLongVarChar == nType || adVarWChar == nType)
				sprintf(sDataBuf, "%s", (char*)(_bstr_t(var.bstrVal)));
		}
		
		m_pRsc->MoveNext();
	}

	catch(_com_error &e)
	{
		ErrCom(e);
		SqlClose();

		return -1;
	}

	return 0;
}



INT CLcNetOledb::SqlClose()
{
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

	
	if(m_pRsc)
	{
		m_pRsc->Close();
		m_pRsc = NULL;
	}


	return 0;
}



// 1. Open a connection using OLE DB syntax.
INT CLcNetOledb::ConnectSql(char* sDataBase, char* sUID, char* sPWD)
{
	char	sCon[512]={0};

	strcpy(m_sDsn, sDataBase);
	strcpy(m_sUid, sUID);
	strcpy(m_sPwd, sPWD);

	if(strlen(m_sUid) && strlen(m_sPwd))
		sprintf(sCon, "provider=SQLOLEDB; Data Source=%s; Initial Catalog=%s; User Id=%s;Password=%s; ", m_sHst, m_sDsn, m_sUid, m_sPwd);

	else
		sprintf(sCon, "provider=SQLOLEDB; Data Source=%s; Initial Catalog=%s; User Id=%s;Password=; ", m_sHst, m_sDsn, m_sUid);

	try
	{
		ComValidTest(m_pCon.CreateInstance(__uuidof(Connection)));

		m_pCon->ConnectionString = sCon;
		m_pCon->ConnectionTimeout = 30;
		m_pCon->Open("","","",adConnectUnspecified);
		printf("cnn1 state: %s\n", (LPCTSTR)GetState(m_pCon->State));
	}
	
	catch(_com_error &e)
	{
		ErrProvider(m_pCon);
		ErrCom(e);

		return -1;
	}

	return 0;
}


// 2. Open a connection using OLE DB syntax from Excel.
INT CLcNetOledb::ConnectExcel(char* sFile)
{
	char	sCon[512]={0};

	sprintf(sCon
		, 	"provider=Microsoft.Jet.OLEDB.4.0; "
			"Data Source= %s ; "
			"Extended Properties=\"Excel 8.0; HDR=No; ReadOnly=False\" ", sFile);

	try
	{
		ComValidTest(m_pCon.CreateInstance(__uuidof(Connection)));
		
		m_pCon->ConnectionString = sCon;
		m_pCon->ConnectionTimeout = 10;
		m_pCon->Open("","","",adConnectUnspecified);
		printf("cnn1 state: %s\n", (LPCTSTR)GetState(m_pCon->State));
	}
	
	catch(_com_error &e)
	{
		ErrProvider(m_pCon);
		ErrCom(e);

		return -1;
	}

	return 0;
}


// 3. Open a connection using a DSN and ODBC tags.
INT CLcNetOledb::ConnectDsnOdbc(char* sDSN, char* sUID, char* sPWD)
{
	char	sCon[512]={0};

	strcpy(m_sDsn, sDSN);
	strcpy(m_sUid, sUID);
	strcpy(m_sPwd, sPWD);

	if(strlen(m_sUid) && strlen(m_sPwd))
		sprintf(sCon, "DSN=%s;UID=%s;PWD=%s;", m_sDsn, m_sUid, m_sPwd);

	else
		sprintf(sCon, "DSN=%s;UID=%s;PWD=;", m_sDsn, m_sUid);
	
	try
	{
		ComValidTest(m_pCon.CreateInstance(__uuidof(Connection)));

		m_pCon->ConnectionString = sCon;
		m_pCon->Open("","","",adConnectUnspecified);
		printf("cnn2 state: %s\n", (LPCTSTR)GetState(m_pCon->State));
	}
	
	catch(_com_error &e)
	{
		ErrProvider(m_pCon);
		ErrCom(e);

		return -1;
	}

	return 0;
}


// 4. Open a connection using a DSN and OLE DB tags.
INT CLcNetOledb::ConnectDsnOledb(char* sDSN, char* sUID, char* sPWD)
{
	char	sCon[512]={0};

	strcpy(m_sDsn, sDSN);
	strcpy(m_sUid, sUID);
	strcpy(m_sPwd, sPWD);

	if(strlen(m_sUid) && strlen(m_sPwd))
		sprintf(sCon, "Data Source=%s;User ID=%s;Password=%s;", m_sDsn, m_sUid, m_sPwd);

	else
		sprintf(sCon, "Data Source=%s;User ID=%s;Password=;", m_sDsn, m_sUid);

	try
	{
		ComValidTest(m_pCon.CreateInstance(__uuidof(Connection)));

		m_pCon->ConnectionString = m_sDsn;
		m_pCon->Open("","","",adConnectUnspecified);
		printf("cnn3 state: %s\n", (LPCTSTR)GetState(m_pCon->State));
	}
	
	catch(_com_error &e)
	{
		ErrProvider(m_pCon);
		ErrCom(e);

		return -1;
	}

	return 0;
}


// 5. Open a connection using a DSN and individual arguments instead of a connection string.
INT CLcNetOledb::ConnectDsnInd(char* sDSN, char* sUID, char* sPWD)
{
	strcpy(m_sDsn, sDSN);
	strcpy(m_sUid, sUID);
	strcpy(m_sPwd, sPWD);

	try
	{
		ComValidTest(m_pCon.CreateInstance(__uuidof(Connection)));
		m_pCon->Open( m_sDsn, m_sUid, m_sPwd, adConnectUnspecified);
		printf("cnn4 state: %s\n", (LPCTSTR)GetState(m_pCon->State));
	}
	
	catch(_com_error &e)
	{
		ErrProvider(m_pCon);
		ErrCom(e);

		return -1;
	}

	return 0;
}


// GetState Function
_bstr_t GetState(int intState) 
{
	_bstr_t strState; 
	switch(intState) 
	{
	case adStateClosed:
		strState = "adStateClosed";
		break;
	case adStateOpen:
		strState = "adStateOpen";
		break;
	default:
		;
	}
	return strState;
}


// ErrProvider Function
void ErrProvider(_ConnectionPtr m_pCon)
{
	ErrorPtr  pErr = NULL;
	
	if( (m_pCon->Errors->Count) > 0)
	{
		long nCount = m_pCon->Errors->Count;
		
		// Collection ranges from 0 to nCount -1.
		for(long i = 0; i < nCount; i++)
		{
			pErr = m_pCon->Errors->GetItem(i);
			printf("Error number: %x\t%s\n", pErr->Number, (LPCSTR)pErr->Description);
		}
	}
}


// ErrCom Function
void ErrCom(_com_error &e)
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	
	// Print Com errors.  
	printf("Error\n");
	printf("\tCode = %08lx\n", e.Error());
	printf("\tCode meaning = %s\n", e.ErrorMessage());
	printf("\tSource = %s\n", (LPCSTR) bstrSource);
	printf("\tDescription = %s\n", (LPCSTR) bstrDescription);
}