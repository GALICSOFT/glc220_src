// Implementation of the LctDBcsv.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcMath.h>
#include <LcBase.h>
#include <LcAux.h>


#if !defined(_LC_ORG_SRC_)
  #include "LctEntity.h"
#else
  #include "LctDBcsv.h"
#endif



struct TcsvTxt
{
	char* s;

	TcsvTxt();
	TcsvTxt(char* v);
	~TcsvTxt();

	void Set(const char* v);
	void Set(const TcsvTxt* v);
};

struct TcsvField
{
	char		FldN[LCTDB_MAX_NAME];		// Field Name
	INT			FldT;						// Field Type

	char*		RecB;						// field array for BOOL
	INT*		RecI;						// field array for INT
	FLOAT*		RecF;						// field array for FLOAT
	DOUBLE*		RecD;						// field array for DOUBLE
	TcsvTxt*	RecS;						// field array for text

	TcsvField();
	void Destroy();
	void Copy(int dst, int src);
	void Clear(int row);
};


class CLctDBcsv : public ILCT_DBcsv
{
protected:
	char		m_File[LC_MAX_PATH2];		// CSV File Name
	INT			m_FldN;						// Field Number
	INT			m_RcdN;						// Record Number
	INT			m_RcdR;						// Reserve Record Number

	TcsvField	m_Fld[LCTDB_MAX_FIELD];		// Filed

	int			m_cDlm;						// delimiter

public:
	CLctDBcsv();
	virtual ~CLctDBcsv();

	virtual INT	Query(char* sCmd, LC_HANDLE pData);
	virtual INT	Release();


	virtual INT	SetAttrib(UINT dCmd, LC_HANDLE pData);	// Write Attribute Value
	virtual INT	GetAttrib(UINT dCmd, LC_HANDLE pData);	// Read Attribute Value


	virtual INT	Select(TCSV_CELL* pOut, INT row, INT col);
	virtual INT	Select(TCSV_CELL* pOut, INT row, char* sFieldName);
	virtual INT	Select(TCSV_REC * pOut, INT row);

	virtual INT	Insert(TCSV_REC * pIn);

	virtual INT	Update(TCSV_CELL* pIn, INT row, INT col);
	virtual INT	Update(TCSV_CELL* pIn, INT row, char* sFieldName);

	virtual INT	Delete(INT row);


protected:

	INT ValidRec(INT row){	return (0 > row || row >= m_RcdN)? LC_EFAIL: 0;	}
	INT ValidFld(INT col){	return (0 > col || col >= m_FldN)? LC_EFAIL: 0;	}

	INT FindField(const char* sFieldName)
	{
		for(INT col=0; col<m_FldN; ++col)
		{
			if(0 == LcStr_Stricmp( m_Fld[col].FldN, sFieldName) )
				return col;
		}

		return LC_EFAIL;
	}

	char* FieldName(int col)
	{
		if(0>col || col >= m_FldN)
			return NULL;

		return m_Fld[col].FldN;
	}


	INT	CopyTo(char* sDst);
	INT	Save(char* sDst);

public:
	INT		CreateFromFile(LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0);
	INT		CreateFromMemory(LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0, LC_HANDLE =0);
	void	Destroy();
};


////////////////////////////////////////////////////////////////////////////////


TcsvTxt::TcsvTxt()
{
	s	= NULL;
}

TcsvTxt::TcsvTxt(char* v)
{
	INT l = strlen(v);
	s = new char[l+4];
	memset(s, 0, l+4);
	strcpy(s, v);
}

TcsvTxt::~TcsvTxt()
{
	SAFE_DELETE_ARRAY(s);
}


void TcsvTxt::Set(const char* v)
{
	INT l = strlen(v);

	if(s)
		delete [] s;


	s = new char[l+4];
	memset(s, 0, l+4);
	strcpy(s, v);
}

void TcsvTxt::Set(const TcsvTxt* v)
{
	INT l = strlen(v->s);

	if(s)
		delete [] s;


	s = new char[l+4];
	memset(s, 0, l+4);
	strcpy(s, v->s);
}



TcsvField::TcsvField()
{
	memset(FldN, 0, LCTDB_MAX_NAME);

	FldT	= 0;
	RecB	= NULL;
	RecI	= NULL;
	RecF	= NULL;
	RecD	= NULL;
	RecS	= NULL;
}


void TcsvField::Destroy()
{
	 SAFE_DELETE_ARRAY(	RecB	);
	 SAFE_DELETE_ARRAY(	RecI	);
	 SAFE_DELETE_ARRAY(	RecF	);
	 SAFE_DELETE_ARRAY(	RecD	);
	 SAFE_DELETE_ARRAY(	RecS	);
}


void TcsvField::Copy(int dst, int src)
{
	if(     LCTDB_BOOL == this->FldT)	RecB[dst] = RecB[src];
	else if(LCTDB_SMALL== this->FldT)	RecI[dst] = RecI[src];
	else if(LCTDB_FLOAT== this->FldT)	RecF[dst] = RecF[src];
	else if(LCTDB_BIG  == this->FldT)	RecD[dst] = RecD[src];
	else if(LCTDB_TEXT == this->FldT)	RecS[dst] = RecS[src];
}

void TcsvField::Clear(int row)
{
	if(     LCTDB_BOOL == this->FldT)	RecB[row] = 0  ;
	else if(LCTDB_SMALL== this->FldT)	RecI[row] = 0  ;
	else if(LCTDB_FLOAT== this->FldT)	RecF[row] = 0.F;
	else if(LCTDB_BIG  == this->FldT)	RecD[row] = 0. ;
	else if(LCTDB_TEXT == this->FldT)	RecS[row].Set("");
}


////////////////////////////////////////////////////////////////////////////////


CLctDBcsv::CLctDBcsv()
{
	memset(m_File, 0, LC_DEFAULT_NAME);

	m_FldN	= 0;
	m_RcdN	= 0;
	m_RcdR	= 0;

	m_cDlm  = ',';			// delimiter
}


CLctDBcsv::~CLctDBcsv()
{
	Destroy();
}


INT CLctDBcsv::Query(char* sCmd, LC_HANDLE pData)
{
	INT hr = LC_OK;

	if( 0 == LcStr_Stricmp(sCmd, LCTDB_QUERY_COPY_TO))
		hr = CopyTo((char*)pData);

	else if( 0 == LcStr_Stricmp(sCmd, LCTDB_QUERY_SAVE_AS))
		hr = Save((char*)pData);

	else
		return LC_EFAIL;


	return hr;
}


INT	CLctDBcsv::SetAttrib(UINT dCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT	CLctDBcsv::GetAttrib(UINT dCmd, LC_HANDLE pData)
{
	INT hr = LC_OK;

	if(     LCTDB_RECORD_NUM  == dCmd)
		*((int*)pData) = m_RcdN;

	else if(LCTDB_FIELD_NUM   == dCmd)
		*((int*)pData) = m_FldN;

	else if(LCTDB_FIELD_INDEX == dCmd)
	{
		int col = FindField( (const char*)pData);
		*((int*)pData) = col;
	}

	else if(LCTDB_FIELD_NAME  == dCmd)
	{
		int col = *((int*)pData);
		strcpy((char*)pData, FieldName(col));
	}

	else if(LCTDB_FIELD_TYPE  == dCmd)
	{
		int col = *((int*)pData);
		if(0 > ValidFld(col))
			return LC_EFAIL;

		*((int*)pData) = m_Fld[col].FldT;
	}
	else
		return LC_EFAIL;


	return hr;
}


INT CLctDBcsv::Release()
{
	INT hr = 0;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}



void CLctDBcsv::Destroy()
{
	for(INT col= 0; col<m_FldN; ++col)
		m_Fld[col].Destroy();

	m_RcdN	= 0;
	m_FldN	= 0;
}



////////////////////////////////////////////////////////////////////////////////


INT CLctDBcsv::CreateFromFile(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	char	Data[LCTDB_MAX_TEXT+4]={0};
	char*	L= NULL;
	char	Old= 0;
	INT		FldIdx = 0;
	INT		RecIdx = 0;

	INT		n = 0;
	long	lF = 0;


	if(!p1)
		return LC_EFAIL;

	strcpy(m_File, (char*)p1);


	if(p3)
		m_cDlm = *((INT*)p3);


	FILE* fp = fopen(m_File, "rb");
	if(NULL == fp)
		return LC_EFAIL;


	// read field name
	memset(Data, 0, LCTDB_MAX_NAME);
	FldIdx = 0;
	L = Data;

	while(!feof(fp) )
	{
		fread(L, 1, 1, fp);

		if('\r' == *L)
			continue;

		if(m_cDlm == *L)
		{
			*L = 0;
			strcpy(m_Fld[FldIdx].FldN, Data);


			memset(Data, 0, LCTDB_MAX_NAME);
			L = Data;
			++FldIdx;

			continue;
		}


		if('\n' == *L)
		{
			*L = 0;
			strcpy(m_Fld[FldIdx].FldN, Data);

			break;
		}

		++L;
	}


	m_FldN = (FldIdx+1);

	// read type
	memset(Data, 0, LCTDB_MAX_NAME);
	FldIdx = 0;
	L = Data;

	while(!feof(fp) )
	{
		fread(L, 1, 1, fp);

		if('\r' == *L)
			continue;

		else if(m_cDlm == *L)
		{
			*L = 0;

			if(FldIdx < m_FldN)
			{
				if(     0 == LcStr_Strcmp(Data, "bool"))
					m_Fld[FldIdx].FldT = LCTDB_BOOL;

				else if(0 == LcStr_Strcmp(Data, "small"))
					m_Fld[FldIdx].FldT = LCTDB_SMALL;

				else if(0 == LcStr_Strcmp(Data, "big"))
					m_Fld[FldIdx].FldT = LCTDB_BIG;

				else if(0 == LcStr_Strcmp(Data, "text"))
					m_Fld[FldIdx].FldT = LCTDB_TEXT;

				else
					m_Fld[FldIdx].FldT = LCTDB_VARIANT;
			}


			memset(Data, 0, LCTDB_MAX_NAME);
			L = Data;
			++FldIdx;

			continue;
		}


		else if('\n' == *L)
		{
			*L = 0;

			if(FldIdx < m_FldN)
			{
				if(     0 == LcStr_Strcmp(Data, "bool"))
					m_Fld[FldIdx].FldT = LCTDB_BOOL;

				else if(0 == LcStr_Strcmp(Data, "small"))
					m_Fld[FldIdx].FldT = LCTDB_SMALL;

				else if(0 == LcStr_Strcmp(Data, "big"))
					m_Fld[FldIdx].FldT = LCTDB_BIG;

				else if(0 == LcStr_Strcmp(Data, "text"))
					m_Fld[FldIdx].FldT = LCTDB_TEXT;

				else
					m_Fld[FldIdx].FldT = LCTDB_VARIANT;
			}

			break;
		}

		++L;
	}


	lF = ftell(fp);

	// count record
	L = Data;
	*L = 0;

	while(!feof(fp) )
	{
		Old = *L;
		fread(L, 1, 1, fp);

		if('\n' == *L)
		{
			++RecIdx;
		}
	}

	// record number
	if('\n' == Old && '\n' == *L)
	--RecIdx;


	m_RcdN = RecIdx;


	if( m_RcdN < 16)
		m_RcdR = 32;
	else
		m_RcdR = m_RcdN * 2;


	// alloc record
	for(n=0; n<m_FldN; ++n)
	{
		if(     LCTDB_BOOL == m_Fld[n].FldT)	m_Fld[n].RecB = new char   [m_RcdR];
		else if(LCTDB_SMALL== m_Fld[n].FldT)	m_Fld[n].RecI = new INT    [m_RcdR];
		else if(LCTDB_FLOAT== m_Fld[n].FldT)	m_Fld[n].RecF = new FLOAT  [m_RcdR];
		else if(LCTDB_BIG  == m_Fld[n].FldT)	m_Fld[n].RecD = new DOUBLE [m_RcdR];
		else if(LCTDB_TEXT == m_Fld[n].FldT)	m_Fld[n].RecS = new TcsvTxt[m_RcdR];
	}

	// read record
	fseek(fp, lF, SEEK_SET);

	memset(Data, 0, LCTDB_MAX_TEXT);
	FldIdx = 0;
	RecIdx = 0;
	L = Data;

	while(RecIdx < m_RcdN)
	{
		fread(L, 1, 1, fp);

		if('\r' == *L)
			continue;

		if(m_cDlm == *L)
		{
			*L = 0;

			if(FldIdx < m_FldN)
			{
				if(    LCTDB_BOOL  == m_Fld[FldIdx].FldT)
				{
					if(0 == LcStr_Stricmp(Data, (char*)"TRUE") )
						m_Fld[FldIdx].RecB[RecIdx] = 1;
					else
						m_Fld[FldIdx].RecB[RecIdx] = 0;
				}

				else if(LCTDB_SMALL== m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecI[RecIdx] = atoi(Data);

				else if(LCTDB_FLOAT== m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecF[RecIdx] = (FLOAT)atof(Data);

				else if(LCTDB_BIG  == m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecD[RecIdx] = atof(Data);

				else if(LCTDB_TEXT == m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecS[RecIdx].Set(Data);
			}


			memset(Data, 0, LCTDB_MAX_TEXT);
			L = Data;
			++FldIdx;

			continue;
		}


		if('\n' == *L)
		{
			*L = 0;

			if(FldIdx < m_FldN)
			{
				if(    LCTDB_BOOL  == m_Fld[FldIdx].FldT)
				{
					if(0 == LcStr_Stricmp(Data, (char*)"TRUE") )
						m_Fld[FldIdx].RecB[RecIdx] = 1;
					else
						m_Fld[FldIdx].RecB[RecIdx] = 0;
				}

				else if(LCTDB_SMALL== m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecI[RecIdx] = atoi(Data);

				else if(LCTDB_FLOAT== m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecF[RecIdx] = (FLOAT)atof(Data);

				else if(LCTDB_BIG  == m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecD[RecIdx] = atof(Data);

				else if(LCTDB_TEXT == m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecS[RecIdx].Set(Data);
			}


			memset(Data, 0, LCTDB_MAX_TEXT);
			L = Data;
			FldIdx = 0;

			++RecIdx;

			continue;
		}

		++L;
	}


	// confirm
//	for(n= 0; n<m_RcdN; ++n)
//	{
//		for(INT n = 0; n<m_FldN; ++n)
//		{
//			if(    LCTDB_SMALL == m_Fld[n].FldT)
//				printf("%4d,", m_Fld[n].RecI[n]);
//
//			else if( LCTDB_FLOAT== m_Fld[n].FldT)
//				printf("%6.f,", m_Fld[n].RecF[n]);
//
//			else if( LCTDB_BIG == m_Fld[n].FldT)
//				printf("%6.f,", m_Fld[n].RecD[n]);
//
//			else if(LCTDB_TEXT == m_Fld[n].FldT)
//				printf("%s,", m_Fld[n].RecS[n]);
//		}
//
//		printf("\n");
//	}


	fclose(fp);

	return LC_OK;
}



INT CLctDBcsv::CreateFromMemory(LC_HANDLE p1, LC_HANDLE p2, LC_HANDLE p3, LC_HANDLE p4)
{
	char*	pBgn = 0;
	char*	pCur = 0;
	int		nSize = 0;

	char	Data[LCTDB_MAX_TEXT+4]={0};
	char*	L= NULL;
	char	Old= 0;
	INT		FldIdx = 0;
	INT		RecIdx = 0;

	INT		n = 0;
	long	lF = 0;


	if(!p1 || !p2)
		return LC_EFAIL;

	pBgn = (char*)p1;
	pCur = pBgn;
	nSize= *((int*)p2);

	if(0 >= nSize)
		return LC_EFAIL;


	if(p3)
		m_cDlm = *((INT*)p3);




	// read field name
	memset(Data, 0, LCTDB_MAX_NAME);
	FldIdx = 0;
	L = Data;

	while( (pCur-pBgn) <= nSize )
	{
		*L = *pCur;		++pCur;		//fread(L, 1, 1, fp);

		if('\r' == *L)
			continue;

		if(m_cDlm == *L)
		{
			*L = 0;
			strcpy(m_Fld[FldIdx].FldN, Data);


			memset(Data, 0, LCTDB_MAX_NAME);
			L = Data;
			++FldIdx;

			continue;
		}


		if('\n' == *L)
		{
			*L = 0;
			strcpy(m_Fld[FldIdx].FldN, Data);

			break;
		}

		++L;
	}


	m_FldN = (FldIdx+1);

	// read type
	memset(Data, 0, LCTDB_MAX_NAME);
	FldIdx = 0;
	L = Data;

	while( (pCur-pBgn) <= nSize )
	{
		*L = *pCur; 	++pCur;		//fread(L, 1, 1, fp);

		if('\r' == *L)
			continue;

		else if(m_cDlm == *L)
		{
			*L = 0;

			if(FldIdx < m_FldN)
			{
				if(     0 == LcStr_Strcmp(Data, "bool"))
					m_Fld[FldIdx].FldT = LCTDB_BOOL;

				else if(0 == LcStr_Strcmp(Data, "small"))
					m_Fld[FldIdx].FldT = LCTDB_SMALL;

				else if(0 == LcStr_Strcmp(Data, "big"))
					m_Fld[FldIdx].FldT = LCTDB_BIG;

				else if(0 == LcStr_Strcmp(Data, "text"))
					m_Fld[FldIdx].FldT = LCTDB_TEXT;

				else
					m_Fld[FldIdx].FldT = LCTDB_VARIANT;
			}


			memset(Data, 0, LCTDB_MAX_NAME);
			L = Data;
			++FldIdx;

			continue;
		}


		else if('\n' == *L)
		{
			*L = 0;

			if(FldIdx < m_FldN)
			{
				if(     0 == LcStr_Strcmp(Data, "bool"))
					m_Fld[FldIdx].FldT = LCTDB_BOOL;

				else if(0 == LcStr_Strcmp(Data, "small"))
					m_Fld[FldIdx].FldT = LCTDB_SMALL;

				else if(0 == LcStr_Strcmp(Data, "big"))
					m_Fld[FldIdx].FldT = LCTDB_BIG;

				else if(0 == LcStr_Strcmp(Data, "text"))
					m_Fld[FldIdx].FldT = LCTDB_TEXT;

				else
					m_Fld[FldIdx].FldT = LCTDB_VARIANT;
			}

			break;
		}

		++L;
	}



	lF = pCur-pBgn;


	// count record
	L = Data;
	*L = 0;

	while( (pCur-pBgn) <= nSize )
	{
		Old = *L;
		*L = *pCur;		++pCur;		//fread(L, 1, 1, fp);

		if('\n' == *L)
		{
			++RecIdx;
		}
	}

	// record number
	if('\n' == Old && '\n' == *L)
	--RecIdx;


	m_RcdN = RecIdx;


	if( m_RcdN < 16)
		m_RcdR = 32;
	else
		m_RcdR = m_RcdN * 2;


	// alloc record
	for(n=0; n<m_FldN; ++n)
	{
		if(     LCTDB_BOOL == m_Fld[n].FldT)	m_Fld[n].RecB = new char   [m_RcdR];
		else if(LCTDB_SMALL== m_Fld[n].FldT)	m_Fld[n].RecI = new INT    [m_RcdR];
		else if(LCTDB_FLOAT== m_Fld[n].FldT)	m_Fld[n].RecF = new FLOAT  [m_RcdR];
		else if(LCTDB_BIG  == m_Fld[n].FldT)	m_Fld[n].RecD = new DOUBLE [m_RcdR];
		else if(LCTDB_TEXT == m_Fld[n].FldT)	m_Fld[n].RecS = new TcsvTxt[m_RcdR];
	}

	// read record
	pCur = pBgn + lF;	// fseek(fp, lF, SEEK_SET);

	memset(Data, 0, LCTDB_MAX_TEXT);
	FldIdx = 0;
	RecIdx = 0;
	L = Data;

	while(RecIdx < m_RcdN)
	{
		*L = *pCur;		++pCur;		//fread(L, 1, 1, fp);

		if('\r' == *L)
			continue;

		if(m_cDlm == *L)
		{
			*L = 0;

			if(FldIdx < m_FldN)
			{
				if(    LCTDB_BOOL  == m_Fld[FldIdx].FldT)
				{
					if(0 == LcStr_Stricmp(Data, (char*)"TRUE") )
						m_Fld[FldIdx].RecB[RecIdx] = 1;
					else
						m_Fld[FldIdx].RecB[RecIdx] = 0;
				}

				else if(LCTDB_SMALL== m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecI[RecIdx] = atoi(Data);

				else if(LCTDB_FLOAT== m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecF[RecIdx] = (FLOAT)atof(Data);

				else if(LCTDB_BIG  == m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecD[RecIdx] = atof(Data);

				else if(LCTDB_TEXT == m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecS[RecIdx].Set(Data);
			}


			memset(Data, 0, LCTDB_MAX_TEXT);
			L = Data;
			++FldIdx;

			continue;
		}


		if('\n' == *L)
		{
			*L = 0;

			if(FldIdx < m_FldN)
			{
				if(    LCTDB_BOOL  == m_Fld[FldIdx].FldT)
				{
					if(0 == LcStr_Stricmp(Data, (char*)"TRUE") )
						m_Fld[FldIdx].RecB[RecIdx] = 1;
					else
						m_Fld[FldIdx].RecB[RecIdx] = 0;
				}

				else if(LCTDB_SMALL== m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecI[RecIdx] = atoi(Data);

				else if(LCTDB_FLOAT== m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecF[RecIdx] = (FLOAT)atof(Data);

				else if(LCTDB_BIG  == m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecD[RecIdx] = atof(Data);

				else if(LCTDB_TEXT == m_Fld[FldIdx].FldT)
					m_Fld[FldIdx].RecS[RecIdx].Set(Data);
			}


			memset(Data, 0, LCTDB_MAX_TEXT);
			L = Data;
			FldIdx = 0;

			++RecIdx;

			continue;
		}

		++L;
	}


	return LC_OK;
}




INT CLctDBcsv::Select(TCSV_CELL* pOut, INT row, INT col)
{
	if(0 > ValidFld(col) || 0 > ValidRec(row))
		return LC_EFAIL;

	pOut->t = m_Fld[col].FldT;

	if(     LCTDB_BOOL == m_Fld[col].FldT)	pOut->b = m_Fld[col].RecB[row];
	else if(LCTDB_SMALL== m_Fld[col].FldT)	pOut->n = m_Fld[col].RecI[row];
	else if(LCTDB_FLOAT== m_Fld[col].FldT)	pOut->f = m_Fld[col].RecF[row];
	else if(LCTDB_BIG  == m_Fld[col].FldT)	pOut->d = m_Fld[col].RecD[row];
	else if(LCTDB_TEXT == m_Fld[col].FldT)	pOut->s = m_Fld[col].RecS[row].s;

	return LC_OK;
}



INT CLctDBcsv::Select(TCSV_CELL* pOut, INT row, char* sFieldName)
{
	INT col = FindField(sFieldName);

	if(0 > ValidFld(col) || 0 > ValidRec(row))
		return LC_EFAIL;

	pOut->t = m_Fld[col].FldT;

	if(     LCTDB_BOOL == m_Fld[col].FldT)	pOut->b = m_Fld[col].RecB[row];
	else if(LCTDB_SMALL== m_Fld[col].FldT)	pOut->n = m_Fld[col].RecI[row];
	else if(LCTDB_FLOAT== m_Fld[col].FldT)	pOut->f = m_Fld[col].RecF[row];
	else if(LCTDB_BIG  == m_Fld[col].FldT)	pOut->d = m_Fld[col].RecD[row];
	else if(LCTDB_TEXT == m_Fld[col].FldT)	pOut->s = m_Fld[col].RecS[row].s;

	return LC_OK;
}


INT CLctDBcsv::Select(TCSV_REC* pOut, INT row)
{
	if(0 > ValidRec(row) )
		return LC_EFAIL;


	pOut->cnt = m_FldN;

	for(INT col=0; col<m_FldN; ++col)
	{
		pOut->rec[col].t = m_Fld[col].FldT;

		if(     LCTDB_BOOL == m_Fld[col].FldT)	pOut->rec[col].b = m_Fld[col].RecB[row];
		else if(LCTDB_SMALL== m_Fld[col].FldT)	pOut->rec[col].n = m_Fld[col].RecI[row];
		else if(LCTDB_FLOAT== m_Fld[col].FldT)	pOut->rec[col].f = m_Fld[col].RecF[row];
		else if(LCTDB_BIG  == m_Fld[col].FldT)	pOut->rec[col].d = m_Fld[col].RecD[row];
		else if(LCTDB_TEXT == m_Fld[col].FldT)	pOut->rec[col].s = m_Fld[col].RecS[row].s;
	}

	return LC_OK;
}


INT CLctDBcsv::Insert(TCSV_REC* pIn)
{
	int row = m_RcdN;

	if(m_RcdR <= row+1)
	{
		m_RcdR = (row + 1) * 2;

		for(int col=0; col<m_FldN; ++col)
		{
			if(     LCTDB_BOOL == m_Fld[col].FldT)
			{
				char*	v = new char   [m_RcdR];
				memcpy(v, m_Fld[col].RecB, m_RcdN* sizeof(char  ) );
				delete [] m_Fld[col].RecB;
				m_Fld[col].RecB = v;
			}

			else if(LCTDB_SMALL== m_Fld[col].FldT)
			{
				INT*	v = new INT    [m_RcdR];
				memcpy(v, m_Fld[col].RecI, m_RcdN* sizeof(INT   ) );
				delete [] m_Fld[col].RecI;
				m_Fld[col].RecI = v;
			}

			else if(LCTDB_FLOAT== m_Fld[col].FldT)
			{
				FLOAT*	v = new FLOAT   [m_RcdR];
				memcpy(v, m_Fld[col].RecF, m_RcdN* sizeof(FLOAT) );
				delete [] m_Fld[col].RecF;
				m_Fld[col].RecF = v;
			}

			else if(LCTDB_BIG  == m_Fld[col].FldT)
			{
				DOUBLE*	v = new DOUBLE [m_RcdR];
				memcpy(v, m_Fld[col].RecD, m_RcdN* sizeof(DOUBLE) );
				delete [] m_Fld[col].RecD;
				m_Fld[col].RecD = v;
			}

			else if(LCTDB_TEXT == m_Fld[col].FldT)
			{
				TcsvTxt*v = new TcsvTxt[m_RcdR];


				for(int n=0; n<m_RcdN; ++n)
					v[n].Set(m_Fld[col].RecS[n].s);

				delete [] m_Fld[col].RecS;
				m_Fld[col].RecS = v;
			}
		}
	}


	for(int col=0; col<m_FldN; ++col)
	{
		if(     LCTDB_BOOL == m_Fld[col].FldT)	m_Fld[col].RecB[row] =   pIn->rec[col].b;
		else if(LCTDB_SMALL== m_Fld[col].FldT)	m_Fld[col].RecI[row] =   pIn->rec[col].n;
		else if(LCTDB_FLOAT== m_Fld[col].FldT)	m_Fld[col].RecF[row] =   pIn->rec[col].f;
		else if(LCTDB_BIG  == m_Fld[col].FldT)	m_Fld[col].RecD[row] =   pIn->rec[col].d;
		else if(LCTDB_TEXT == m_Fld[col].FldT)	m_Fld[col].RecS[row].Set(pIn->rec[col].s);
	}

	++m_RcdN;

	return LC_OK;
}


INT CLctDBcsv::Update(TCSV_CELL* pIn, INT row, INT col)
{
	if(0 > ValidFld(col) || 0 > ValidRec(row))
		return LC_EFAIL;

	if(     LCTDB_BOOL == m_Fld[col].FldT)	m_Fld[col].RecB[row] =   pIn->b;
	else if(LCTDB_SMALL== m_Fld[col].FldT)	m_Fld[col].RecI[row] =   pIn->n;
	else if(LCTDB_FLOAT== m_Fld[col].FldT)	m_Fld[col].RecF[row] =   pIn->f;
	else if(LCTDB_BIG  == m_Fld[col].FldT)	m_Fld[col].RecD[row] =   pIn->d;
	else if(LCTDB_TEXT == m_Fld[col].FldT)	m_Fld[col].RecS[row].Set(pIn->s);

	return LC_OK;
}


INT CLctDBcsv::Update(TCSV_CELL* pIn, INT row, char* sFieldName)
{
	INT col = FindField(sFieldName);

	if(0 > ValidFld(col) || 0 > ValidRec(row))
		return LC_EFAIL;

	if(     LCTDB_BOOL == m_Fld[col].FldT)	m_Fld[col].RecB[row] =   pIn->b;
	else if(LCTDB_SMALL== m_Fld[col].FldT)	m_Fld[col].RecI[row] =   pIn->n;
	else if(LCTDB_FLOAT== m_Fld[col].FldT)	m_Fld[col].RecF[row] =   pIn->f;
	else if(LCTDB_BIG  == m_Fld[col].FldT)	m_Fld[col].RecD[row] =   pIn->d;
	else if(LCTDB_TEXT == m_Fld[col].FldT)	m_Fld[col].RecS[row].Set(pIn->s);

	return LC_OK;
}


INT CLctDBcsv::Delete(INT _row)
{
	int row = _row;

	if( 0 > ValidRec(row))
		return LC_EFAIL;

	for(int col=0; col<m_FldN; ++col)
	{
		for(; row<m_RcdN; ++row)
		{
			m_Fld[row].Copy(row, row+1);
		}

		m_Fld[col].Clear(m_RcdN-1);
	}

	--m_RcdN;

	return LC_OK;
}



INT CLctDBcsv::CopyTo(char* sDst)
{
	FILE*	fps = NULL;
	FILE*	fpd = NULL;
	char	sbuf[256]={0};
	int		r =0;

	fps = fopen(m_File, "rb");
	if(!fps)
		return LC_EFAIL;


	fpd = fopen(sDst, "wb");
	if(!fpd)
	{
		fclose(fps);
		return LC_EFAIL;
	}


	while(!feof(fps) )
	{
		r = fread(sbuf, 1, 256, fps);

		if(0 >=r)
			continue;

		fwrite(sbuf, 1, r, fpd);
	}

	fclose(fps);
	fclose(fpd);

	return LC_OK;
}


INT	CLctDBcsv::Save(char* sDst)
{
	FILE*	fp = NULL;
	INT		row=0, col=0;

	if(sDst)
		fp = fopen(sDst, "wb");
	else
		fp = fopen(m_File, "wb");


	if(!fp)
		return LC_EFAIL;


	// write field name
	for(col=0; col<m_FldN; ++col)
	{
		fprintf(fp, "%s", m_Fld[col].FldN);

		if( col != (m_FldN-1) )
			fprintf(fp, "%c", m_cDlm);
		else
			fprintf(fp, "\n");
	}



	// write type
	for(col=0; col<m_FldN; ++col)
	{
		if(     LCTDB_BOOL == m_Fld[col].FldT)
			fprintf(fp, "bool");

		else if(LCTDB_SMALL== m_Fld[col].FldT)
			fprintf(fp, "small");

		else if(LCTDB_BIG  == m_Fld[col].FldT)
			fprintf(fp, "big");

		else if(LCTDB_TEXT == m_Fld[col].FldT)
			fprintf(fp, "text");


		if( col != (m_FldN-1) )
			fprintf(fp, "%c", m_cDlm);
		else
			fprintf(fp, "\n");
	}


	for(row=0; row<m_RcdN; ++row)
	{
		for(col=0; col<m_FldN; ++col)
		{
			if(     LCTDB_BOOL == m_Fld[col].FldT)
			{
				if(m_Fld[col].RecB[row])
					fprintf(fp, "TRUE");
				else
					fprintf(fp, "FALSE");
			}

			else if(LCTDB_SMALL== m_Fld[col].FldT)
				fprintf(fp, "%d", m_Fld[col].RecI[row]);

			else if(LCTDB_FLOAT== m_Fld[col].FldT)
				fprintf(fp, "%f", m_Fld[col].RecF[row]);

			else if(LCTDB_BIG  == m_Fld[col].FldT)
				fprintf(fp, "%f", m_Fld[col].RecD[row]);


			else if(LCTDB_TEXT == m_Fld[col].FldT)
				fprintf(fp, "%s", m_Fld[col].RecS[row].s);


			if( col != (m_FldN-1) )
				fprintf(fp, "%c", m_cDlm);
			else
				fprintf(fp, "\n");
		}
	}


	fclose(fp);

	return LC_OK;
}



// Create DB from CSV
INT LctDB_CreateCsv(char* sCmd
				   , PLCT_DBCSV* pOut		// Output instance
				   , LC_HANDLE p1			// CSV File Name or Memory Address
				   , LC_HANDLE p2			// Memory Size Address(unsigned int*). if it will be used from file then it must be null or zero
				   , LC_HANDLE p3			// Delimiter. ',' is default if change this value int dlm =',' and set the argument to &dlm
				   , LC_HANDLE p4			// No Use
					 )
{
	INT			hr = 0;
	CLctDBcsv*	pObj = 0;
	INT			iSize = 0;

	if(p2)
		iSize = *((INT*)p2);

	*pOut		= 0;
	pObj = new CLctDBcsv;

	if(0<iSize)	hr = pObj->CreateFromMemory(p1, p2, p3, p4);
	else		hr = pObj->CreateFromFile(p1, p2, p3, p4);

	if(LC_FAILED(hr))
	{
		delete pObj;
		return LC_CANNT_ALLOC;
	}


	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}

