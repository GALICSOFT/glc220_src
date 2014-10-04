// Interface for the CLcSmd class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcSmd_H_
#define _LcSmd_H_

class CLcSmd : public ILcSmd
{
protected:

public:
	CLcSmd();
	virtual ~CLcSmd();
	
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

	virtual void*	GetTexture();
	virtual INT		GetVideoW();
	virtual INT		GetVideoH();
};

#endif


