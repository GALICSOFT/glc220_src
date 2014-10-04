// Interface for the CMpMp3 class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MpMp3_H_
#define _MpMp3_H_

class CMpMp3 : public CLcSmd
{
protected:
	char	m_sFile[MAX_PATH];
	
	void*	m_pGB		;														// IGraphBuilder*
	void*	m_pMC		;														// IMediaControl*
	void*	m_pMS		;														// IMediaSeeking*
	void*	m_pSC		;														// IBaseFilter*		Source Current
	void*	m_pBA		;														// IBasicAudio*
	void*	m_pME		;														// IMediaEventEx*

	LONGLONG m_Duration	;
	DWORD	m_dRepeat	;
	DWORD	m_dStatus	;

	HWND	m_hWnd		;
	INT		m_nEventId	;

public:
	CMpMp3();
	virtual ~CMpMp3();

//	virtual INT		Create(HWND hWnd, DWORD nEventId, char* sFile);

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
	virtual void	SetRepeat(DWORD dRepeat= 0xFFFFFFFF/*INFINITE*/);
	virtual DWORD	GetStatus();

protected:
	INT		InitialGraph();
	INT		ProcessEvent();
};


INT		LnDms_CreateMp3FromFile(ILcSmd** ppMp3,	HWND hWnd, INT nEvntId, char* sFile);


#endif


