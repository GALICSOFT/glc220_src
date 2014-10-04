// Implementation of the CLcSmd class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <d3dx9.h>

#include "LnxEntity.h"
#include "LnSmd.h"


CLcSmd::CLcSmd()
{
}

CLcSmd::~CLcSmd()
{
	CLcSmd::Destroy();
}


INT CLcSmd::Create(void*, void*, void*, void*)
{
	return 0;
}

void CLcSmd::Destroy()
{
//	printf("CLcSmd Destroy\n");
}


INT CLcSmd::Query(char*, void*)
{
//	printf("CLcSmd Query:%s\n", sCmd);
	return 0;
}


DWORD  CLcSmd::GetType()
{
//	printf("CLcSmd GetType\n");
	return 0;
}

void  CLcSmd::Play()
{
//	printf("CLcSmd Play\n");
}

void  CLcSmd::Stop()
{
//	printf("CLcSmd Stop\n");
}


void  CLcSmd::Reset()
{
//	printf("CLcSmd Reset\n");
}


void  CLcSmd::Pause()
{
//	printf("CLcSmd Pause\n");
}


void  CLcSmd::SetVolume(LONG dVol)
{
//	printf("CLcSmd SetVolume\n");
}


LONG  CLcSmd::GetVolume()
{
//	printf("CLcSmd GetVolume\n");
	return -1;
}


void  CLcSmd::SetRepeat(DWORD dRepeat)
{
//	printf("CLcSmd SetRepeat\n");
}


DWORD  CLcSmd::GetStatus()
{
//	printf("CLcSmd GetStatus\n");
	return 0xFFFFFFFF;
}



void*  CLcSmd::GetTexture()
{
//	printf("CLcSmd GetTexture\n");
	return 0;
}


INT  CLcSmd::GetVideoW()
{
//	printf("CLcSmd GetVideoW\n");
	return -1;
}


INT  CLcSmd::GetVideoH()
{
//	printf("CLcSmd GetVideoH\n");
	return -1;
}


