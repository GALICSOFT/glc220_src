// Interface for the CLcpDataB class.
// Basic Data
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcpDataB_H_
#define _LcpDataB_H_

typedef signed __int64      INT64;


class CLcpDataB : public ILcpData
{
protected:
	// ID
	INT64	m_ID;					// ID
	char	m_Name[LCP_DATA_NAME];	// Name

	// ����
	INT		m_nHP;				// Heal Point
	INT		m_nMp;				// Mana Point


	// ��ġ & ....
	VEC3	m_vcTrn	;			// Translation: ��ġ
	VEC4	m_vcRot	;			// Rotation: Axis and angle
	VEC3	m_vcScl	;			// Scaling

	VEC3	m_vcDir;			// ����
	VEC3	m_vcAcl;			// ���ӵ�
	FLOAT	m_fSped;			// �̵� ���ǵ�

public:
	CLcpDataB();
	virtual ~CLcpDataB();

	virtual INT		Query(char*, void*);
	virtual INT		SetAttrib(char*, void*);
	virtual INT		GetAttrib(char*, void*);
	virtual INT		GetType()	{	return ILcpData::LCP_TYPE_INTER;	};
};


#endif


