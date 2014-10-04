//	Copyright(C) GaLic Soft. All rights reserved.
//	GaLic Game Library Collection
//
// Interface for the DGT Entities.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LctEntity_H_
#define _LctEntity_H_




////////////////////////////////////////////////////////////////////////////////
// for simple xml

DCL_INTERFACE ILCT_Xml : public ILC_Object
{
	DCL_CLASS_DESTROYER(	ILCT_Xml	);

	virtual INT	Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT	Release()=0;

	virtual LC_HANDLE	ElmList(const char* sElm, LC_HANDLE pNodeParent)=0;
	virtual LC_HANDLE	ElmNode(LC_HANDLE pNode, INT nIdx)=0;
	virtual INT			ElmSize(LC_HANDLE pNode)=0;
	virtual INT			ElmText(char* sOutput, LC_HANDLE pNode)=0;
	virtual INT			ElmAttrb(char* sOutput, int sOutputSize, LC_HANDLE pNode, const char* sAttName)=0;
};

typedef	ILCT_Xml*	PLCT_XML;

INT LctXml_Create(char* sCmd
					, PLCT_XML* pData
					, LC_HANDLE p1			// XML File
					, LC_HANDLE p2 = NULL	// No Use
					, LC_HANDLE p3 = NULL	// No Use
					, LC_HANDLE p4 = NULL	// No Use
				 );




#if !defined(_LC_ORG_SRC_)

////////////////////////////////////////////////////////////////////////////////
// for csv file

enum ELctDBcsv
{
	// Field Type
	LCTDB_BOOL			= 0x00000011,		// boolean (1byte)
	LCTDB_SMALL			= 0x00000012,		// int (4byte)
	LCTDB_FLOAT			= 0x00000013,		// FLOAT (4byte)
	LCTDB_BIG			= 0x00000014,		// double (8byte)
	LCTDB_TEXT			= 0x00000015,		// string
	LCTDB_VARIANT		= 0x00000016,		// not defined

	LCTDB_MAX_FIELD		= 128,
	LCTDB_MAX_NAME		= 64,
	LCTDB_MAX_TEXT		= 512,


	// for Get Attribute
	LCTDB_RECORD_NUM	= 0x00001001,		// Get the record number
	LCTDB_FIELD_NUM		= 0x00001002,		// Get the field number

	LCTDB_FIELD_INDEX	= 0x00001003,		// Get the field index. it needs string
	LCTDB_FIELD_NAME	= 0x00001004,		// Get the field name. return the field name
	LCTDB_FIELD_TYPE	= 0x00001005,		// Get the field type.
};

// Query Command
#define LCTDB_QUERY_COPY_TO		(char*)"Copy to"
#define LCTDB_QUERY_SAVE_AS		(char*)"Save As"


DCL_INTERFACE ILCT_DBcsv : public ILC_Object
{
	struct TCSV_CELL
	{
		INT			t;					// cell type
		union							// value
		{
			char	b;					// for Boolean
			INT		n;					// for Small
			FLOAT	f;					// for Float
			DOUBLE	d;					// for Big
			char*	s;					// string
		};

		TCSV_CELL() : t(0), n(-1){}
	};

	struct TCSV_REC
	{
		INT			cnt;
		TCSV_CELL	rec[LCTDB_MAX_FIELD];
	};


	DCL_CLASS_DESTROYER(	ILCT_DBcsv	);


	virtual INT	Query(char* sCmd, LC_HANDLE pData)=0;
	virtual INT	Release()=0;

	virtual INT	Select(TCSV_CELL* pOut, INT row, INT col)=0;
	virtual INT	Select(TCSV_CELL* pOut, INT row, char* FieldName)=0;
	virtual INT	Select(TCSV_REC * pOut, INT row)=0;

	virtual INT	Insert(TCSV_REC * pIn)=0;

	virtual INT	Update(TCSV_CELL* pIn, INT row, INT col)=0;
	virtual INT	Update(TCSV_CELL* pIn, INT row, char* FieldName)=0;

	virtual INT	Delete(INT row)=0;
};

typedef	ILCT_DBcsv*		PLCT_DBCSV;

// Create DB from CSV
INT LctDB_CreateCsv(char* sCmd
				   , PLCT_DBCSV* pOut		// Output instance
				   , LC_HANDLE p1			// CSV File Name or Memory Address
				   , LC_HANDLE p2 = NULL	// Memory Size Address(int*). if it will be used from file then it must be null or zero
				   , LC_HANDLE p3 = NULL	// Delimiter. ',' is default if change this value int dlm =',' and set the argument to &dlm
				   , LC_HANDLE p4 = NULL	// No Use
					 );

#endif //#if !defined(_LC_ORG_SRC_)




#endif


