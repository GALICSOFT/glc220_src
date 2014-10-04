// Interface for the LctEntity class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _ILctEntity_H_
#define _ILctEntity_H_

#pragma warning(disable : 4786)
#include <vector>
using namespace std;


#ifndef interface
#define interface struct
#endif


#ifndef LC_CLASS_DESTROYER
#define LC_CLASS_DESTROYER( CLASS_NAME )	\
virtual ~CLASS_NAME(){}
#endif



interface ILcTbl
{
	LC_CLASS_DESTROYER(	ILcTbl	);

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;

	virtual	INT		Select(void** pOut, INT nM/*In*/, INT nS/*In*/)=0;
	virtual	INT		Select(void** pOut, INT* nM/*Out*/, INT* nS/*Out*/, char* sFile)=0;
	virtual	INT		SelectInfo(void** pOut, INT nM/*In*/, INT nS/*In*/)=0;
	virtual	INT		SelectName(void** pOut, INT nM/*In*/, INT nS/*In*/)=0;
};


typedef	vector<ILcTbl* >		lsILcTbl;
typedef lsILcTbl::iterator		itILcTbl;


INT LcTbl_Create(char* sCmd
				 , ILcTbl** pData
				 , void* p1			// File Name
				 , void* p2 = 0		// Etc
				 , void* p3 = 0		// No Use
				 , void* p4 = 0		// No Use
				 );





// 게임 오브젝트란 게임 진행에서 상호 작용을 하는 모든 객체이다.
// 텍스처, 폰트, 사운드, 폴리곤 모델 등은 게임 객체를 구성하는 원소라 할 수 있다.
// 게임 객체는 살아 있는 객체이다. 현실에서 바위는 무생물 이지만 게임에서
//  상호작용하는 바위는 생명체와 다름이 없다.
// 게임 데이터는 일종의 영혼으로 게임의 객체를 제어하는 요소이다.
// 이 게임 객체는 화면에 모델링 등을 이용해서 렌더링 한다.
// 게임 객체의 속성은 기본적으로 생명체를 모델로 구성한다. 이 속성은 또한
//  온라인 기본적으로 이동을 하며 제한된 생명 시간을 갖는다.



interface ILcpObj
{
	enum EObjState
	{
		LCPOBJ_NONE_		= 0x00000000,
		LCPOBJ_BIT_NON_MOVE	= 0x00000001,
		LCPOBJ_BIT_MOVE		= 0x00010000,

		LCPOBJ_IDLE			= 0x00000011,
		LCPOBJ_ATTACK		= 0x00000021,
		LCPOBJ_DEATH		= 0x00000031,

		LCPOBJ_WORK			= 0x00110000,
		LCPOBJ_RUN			= 0x00210000,
	};

	LC_CLASS_DESTROYER(	ILcpObj	);
	
	virtual INT		Create(void* =0, void* =0, void* =0, void* =0)=0;
	virtual void	Destroy()=0;
	virtual INT		FrameMove()=0;
	virtual void	Render()=0;

	virtual INT		Query(char* sCmd, void* pVal)=0;
	virtual INT		SetAttrib(char* sCmd, void* pVal)=0;
	virtual INT		GetAttrib(char* sCmd, void* pVal)=0;

	virtual INT		SetAnimation(char* sCmd, void* pVal)=0;
	virtual INT		GetAnimation(char* sCmd, void* pVal)=0;
};


INT LcpObj_Create(char* sCmd
				 , ILcpObj** pData
				 , void* p1=0		// Basic Data
				 , void* p2=0		// Extended Data
				 , void* p3=0		// Basic Model
				 , void* p4=0		// Extended Model
				 );



interface ILcpData
{
	LC_CLASS_DESTROYER(	ILcpData	);
	
	virtual INT		GetType()=0;
	virtual INT		Query(char* sCmd, void* pVal)=0;
	virtual INT		SetAttrib(char* sCmd, void* pVal)=0;
	virtual INT		GetAttrib(char* sCmd, void* pVal)=0;

	enum
	{
		LCP_TYPE_INTER	= 1,
		LCP_TYPE_OUTER	= 2,
		LCP_DATA_NAME	= 64,
	};
};


// Basic Data Create
INT LcpData_CreateBasic(char* sCmd
				 , ILcpData** pData
				 , void* p1=0		// No Use
				 , void* p2=0		// No Use
				 , void* p3=0		// No Use
				 , void* p4=0		// No Use
				 );













interface ILcmFld						// Outdoor Map
{
	LC_CLASS_DESTROYER(	ILcmFld	);

	virtual	INT		Create(void* p1=0, void* p2=0, void* p3=0, void* p4=0)=0;
	virtual	void	Destroy()=0;
	virtual	INT		FrameMove()=0;
	virtual	void	Render()=0;

	virtual INT		Query(char* sCmd, void* pData)=0;
	virtual	INT		GetHeight(float* vcOut, const float* vcIn)=0;
};



INT LcMap_CreateOutdoor(char* sCmd
				, ILcmFld** pData
				, void* p1			// Map File Name
				, void* p2			// Texture Table
				, void* p3			// Model Table
				, void* p4			// D3DXEffect
				, void* p5=0		// No Use
				, void* p6=0		// No Use
				);


extern	void LcTool_SetupObject(void*);








#endif



