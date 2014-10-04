// Interface for the CMpSnd3D class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MpSnd3D_H_
#define _MpSnd3D_H_

class CMpSnd3D : public CLcSmd
{
protected:
	LPDIRECTSOUND3DBUFFER	m_pDsrc;											// 3D sound buffer
	LPDIRECTSOUND3DLISTENER	m_pDlsn;											// 3D listener object

	DS3DBUFFER				m_DsSrc;											// 3D buffer properties (Source)
	DS3DLISTENER			m_DsLsn;											// Listener properties (Dest)

	ILcSmd*					m_pSnd;												// CMpSnd* pointer
	
public:
	CMpSnd3D();
	virtual ~CMpSnd3D();
	
//	INT		Create(LPDIRECTSOUND8 pDS, TCHAR* sFile);
//	void	Destroy();

	virtual INT		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();
	virtual INT		Query(char* sCmd, void* pData);

	virtual DWORD	GetType();
	virtual void	Play();
	virtual void	Stop();
	virtual void	Reset();
	virtual void	Pause();
	virtual void	SetVolume(LONG dVol);
	virtual LONG	GetVolume();
	virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*INFINITE. DSBPLAY_LOOPING */);
	virtual DWORD	GetStatus();

public:
	D3DXVECTOR3	GetPosSrc();
	D3DXVECTOR3	GetPosLsn();
	void		SetPosSrc(const D3DXVECTOR3* vcP);								// Sound Source Position
	void		SetPosLsn(const D3DXVECTOR3* vcP);								// Sound Listener Position
};



INT		LnDms_Create3DSoundFromFile(
							ILcSmd**	ppSound
						,	void*		pL
						,	char*		sFile
					   );

#endif


