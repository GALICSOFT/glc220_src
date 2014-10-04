// Interface for the CLcNetOledb class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcNetOledb_H_
#define _LcNetOledb_H_

class CLcNetOledb : public CLcNetDB
{
protected:
	_ConnectionPtr	m_pCon;
	_RecordsetPtr	m_pRsc;




	LONG			m_nField;
	char**			m_pDataBuf;
	INT*			m_nDataBuf;

public:	
	CLcNetOledb();
	virtual ~CLcNetOledb();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual	INT		Query(char* sCmd, void* pData);

	
	virtual INT		Connect(void* pDataType, void* p1=0, void* p2=0, void* p3=0);
	virtual void	Close();

	virtual INT		SqlBind(char* sSQL, char*** sOutputBufc,INT** nDataBufc, INT nBufSize);
	virtual INT		SqlExec();
	virtual INT		SqlClose();

protected:
	INT		ConnectSql(char* sDataBase, char* sUID, char* sPWD);
	INT 	ConnectExcel(char* sFile);
	INT 	ConnectDsnOdbc(char* sDSN, char* sUID, char* sPWD);
	INT 	ConnectDsnOledb(char* sDSN, char* sUID, char* sPWD);
	INT 	ConnectDsnInd(char* sDSN, char* sUID, char* sPWD);
};


#endif


