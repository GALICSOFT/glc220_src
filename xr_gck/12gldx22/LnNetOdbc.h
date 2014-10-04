// Interface for the CLcNetOdbc class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcNetOdbc_H_
#define _LcNetOdbc_H_

class CLcNetOdbc : public CLcNetDB
{
protected:
	HANDLE			m_hEnv;						// DataBase Allocation Handle(SQLHENV)

	HANDLE			m_hDbc;						// ODBC Handle(SQLHDBC)
	HANDLE			m_hStm;						// ODBC Statement Handle(SQLHSTMT)
	
	LONG			m_nField;
	char**			m_pDataBuf;
	INT*			m_nDataBuf;

public:	
	CLcNetOdbc();
	virtual ~CLcNetOdbc();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual	INT		Query(char* sCmd, void* pData);


	virtual INT		Connect(void* pDataType, void* p1=0, void* p2=0, void* p3=0);
	virtual void	Close();

	virtual INT		SqlBind(char* sSQL, char*** sOutputBufc,INT** nDataBufc, INT nBufSize);
	virtual INT		SqlExec();
	virtual INT		SqlClose();
	
protected:
	INT		ConnectSql(char* sDsn, char* sUID, char* sPWD);
	INT		ConnectExcel(char* sFile);
	INT		ConnectMdb(char* sFile);
	INT		ConnectDsn(char* sDsn);

	INT		GetColumNum();
};

#endif


