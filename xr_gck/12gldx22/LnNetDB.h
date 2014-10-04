// Interface for the CLcNetDB class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcNetDB_H_
#define _LcNetDB_H_

class CLcNetDB : public ILcxNetDB
{
public:
	enum EDBaseTyte
	{
		DB_NONE=0,
		DB_MDB,
		DB_DSN,
		DB_SQL,
		DB_EXL,
		DB_TOT,
	};

protected:
	EDBaseTyte	m_eType;					// DataBase Type

	char		m_sHst[260];				// Host Name
	char		m_sDsn[260];				// DSN
	char		m_sUid[260];				// User ID
	char		m_sPwd[260];				// Password


public:	
	CLcNetDB();
	virtual ~CLcNetDB();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual	INT		Query(char* sCmd, void* pData);


	virtual INT		Connect(void* pDataType, void* p1=0, void* p2=0, void* p3=0);
	virtual void	Close();

	virtual INT		SqlBind(char* sSQL, char*** sOutputBufc,INT** nDataBufc, INT nBufSize);
	virtual INT		SqlExec();
	virtual INT		SqlClose();	
};


#endif


