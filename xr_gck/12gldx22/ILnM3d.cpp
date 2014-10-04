// Implementation of the ILcVtx class.
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "LnxEntity.h"

#include "LnVtx.h"

#include "LnMtl.h"
#include "LnLgt.h"


////////////////////////////////////////////////////////////////////////////////

namespace LcM
{
	
VtxD::VtxD()
{
	p[0]= 0; p[1]= 0; p[2]= 0;
	d	= 0xFFFFFFFF;
}

VtxD::VtxD(FLOAT X,FLOAT Y,FLOAT Z,DWORD D)
{
	p[0]= X; p[1]= Y; p[2]= Z;
	d	= D;
}



VtxDUV1::VtxDUV1()
{
	p[0]= 0; p[1]= 0; p[2]= 0;
	u = 0; v= 0;
	d = 0xFFFFFFFF;
}

VtxDUV1::VtxDUV1(FLOAT X,FLOAT Y,FLOAT Z,FLOAT U,FLOAT V, DWORD D)
{
	p[0]= X; p[1]= Y; p[2]= Z;
	u = U; v = V;
	d = D;
}



VtxN::VtxN()
{
	p[0]= 0; p[1]= 0; p[2]= 0;
	n[0]= 0; n[1]= 0; n[2]= 0;
}

VtxN::VtxN(FLOAT X,FLOAT Y,FLOAT Z,FLOAT nX,FLOAT nY,FLOAT nZ)
{
	p[0]= X; p[1]= Y; p[2]= Z;
	n[0]= nX; n[1]= nY; n[2]= nZ;
}



VtxUV1::VtxUV1()
{
	p[0]= 0; p[1]= 0; p[2]= 0;
	u = 0; v = 0;
}

VtxUV1::VtxUV1(FLOAT X,FLOAT Y,FLOAT Z, FLOAT U,FLOAT V)
{
	p[0]= X; p[1]= Y; p[2]= Z;
	u = U; v = V;
}

VtxNUV1::VtxNUV1()
{
	p[0]= 0; p[1]= 0; p[2]= 0;
	n[0]= 0; n[1]= 0; n[2]= 0;
	u = 0; v = 0;
}

VtxNUV1::VtxNUV1(FLOAT X,FLOAT Y,FLOAT Z,FLOAT nX,FLOAT nY,FLOAT nZ, FLOAT U,FLOAT V)
{
	p[0]= X; p[1]= Y; p[2]= Z;
	n[0]= nX;n[1]= nY;n[2]= nZ;
	u = U; v = V;
}

VtxNDUV1::VtxNDUV1()
{
	p[0]= 0; p[1]= 0; p[2]= 0;
	n[0]= 0; n[1]= 0; n[2]= 0;
	u = 0; v = 0; d	= 0xFFFFFFFF;
}

VtxNDUV1::VtxNDUV1(FLOAT X,FLOAT Y,FLOAT Z,FLOAT nX,FLOAT nY,FLOAT nZ, FLOAT U,FLOAT V,DWORD D)
{
	p[0]= X; p[1]= Y; p[2]= Z;
	n[3]= nX;n[3]= nY;n[3]= nZ;
	u = U;v = V; d = D;
}



VtxNDUV2::VtxNDUV2()
{
	p[0]= 0; p[1]= 0; p[2]= 0;
	n[0]= 0; n[1]= 0; n[2]= 0;
	u0	= 0; v0	= 0; u1	= 0; v1	= 0; 
	d	= 0xFFFFFFFF;
}

VtxNDUV2::VtxNDUV2(FLOAT X,FLOAT Y,FLOAT Z, FLOAT nX,FLOAT nY,FLOAT nZ,FLOAT U0,FLOAT V0, FLOAT U1,FLOAT V1,DWORD D)
{
	p[0]= X; p[1]= Y; p[2]= Z;
	n[3]= nX;n[3]= nY;n[3]= nZ;
	u0 = U0; v0 = V0; u1 = U1; v1 = V1;
	d = D;
}



VtxRHWD::VtxRHWD()
{
	p[0]= 0; p[1]= 0; p[2]= 0; p[3]= 1;
	d = 0xFFFFFFFF;
}

VtxRHWD::VtxRHWD(FLOAT X,FLOAT Y,FLOAT Z, DWORD D)
{
	p[0]= X; p[1]= Y; p[2]= Z; p[3]= 1;
	d = D;
}



VtxRHWDUV1::VtxRHWDUV1()
{
	p[0]= 0; p[1]= 0; p[2]= 0; p[3]= 1;
	u = 0; v = 0;
	d = 0xFFFFFFFF;
}

VtxRHWDUV1::VtxRHWDUV1(FLOAT X,FLOAT Y,FLOAT Z,FLOAT U,FLOAT V, DWORD D)
{
	p[0]= X; p[1]= Y; p[2]= Z; p[3]= 1;
	u = 0; v = 0;
	d = 0xFFFFFFFF;
}

} // end namespace LcM




INT	LcM3d_CreateVtx(char* sCmd		// Command
				, ILcVtx** pData		//
				, void* p1				// Vertex Structure
				, void* p2				// Need D3D Device
				, void* p3				// Primitive Type
				, void* p4				// Primitive Count
				, void* p5				// Total Vertices
				, void* p6				// Vertex memory copy source.
				, void* p7				// Source Total Size
				, void* p8				// No Use
				, void* p9				// No Use
				)
{
	(*pData)= NULL;

	if(0==_stricmp("PC", sCmd))
	{
		return LcM3d_CreateVtx_PC(pData, p1, p2, p3, p4, p5, p6, p7, p8, p9);
	}

	return -1;
}

INT	LcM3d_CreateMtl(char* sCmd			// Command
					, ILcMtl** pData		//
					, void* p1				// d3dDevice
					, void* p2				// Ambient
					, void* p3				// Diffuse
					, void* p4				// Specular
					, void* p5				// Emissive
					, void* p6				// Power
					)
{
	(*pData)= NULL;

	void*	pDev	= p1;
	FLOAT*	pAmb	= (FLOAT*  )p2;
	FLOAT*	pDff	= (FLOAT*  )p3;
	FLOAT*	pSpc	= (FLOAT*  )p4;
	FLOAT*	pEms	= (FLOAT*  )p5;
	FLOAT	fPow	= *((FLOAT*)p6);


	if(0==_stricmp("PC", sCmd))
	{
		CLcMtl* pObj	= new CLcMtl;

		pObj->SetAmbient (pAmb);
		pObj->SetDiffuse (pDff);
		pObj->SetSpecular(pSpc);
		pObj->SetEmissive(pEms);
		pObj->SetPower	 (fPow);

		if(FAILED(pObj->Create(p1, NULL, NULL, NULL)))
		{
			// Return Error Notification
			delete pObj;
			return -1;
		}

		(*pData) = pObj;
		return 0;
	}

	return -1;
}




INT	LcM3d_CreateLgt(char* sCmd				// Command
					, ILcLgt** pData		//
					, void* p1				// d3dDevice
					, void* p2				// Type
					, void* p3				// Ambient
					, void* p4				// Diffuse
					, void* p5				// Specular
					, void* p6				// Position
					, void* p7				// Direction
					, void* p8				// Etc array. {Range, Falloff, Attenuation0, Attenuation1, Attenuation2, Theta, Phi, }
					)
{
	(*pData)= NULL;

	void*	pDev	= p1;
	DWORD	dType	= *((DWORD*)p2);
	FLOAT*	pAmb	= (FLOAT*  )p3;
	FLOAT*	pDff	= (FLOAT*  )p4;
	FLOAT*	pSpc	= (FLOAT*  )p5;
	FLOAT*	pPos	= (FLOAT*  )p6;
	FLOAT*	pDir	= (FLOAT*  )p7;

	FLOAT*	fEtc	= (FLOAT*  )p8;


	if(0==_stricmp("PC", sCmd))
	{
		CLcLgt* pObj	= new CLcLgt;

		pObj->SetLgtType		(dType);
		pObj->SetLgtAmbient		(pAmb);
		pObj->SetLgtDiffuse		(pDff);
		pObj->SetLgtSpecular	(pSpc);
		pObj->SetLgtPosition	(pPos);
		pObj->SetLgtDirection	(pDir);

		pObj->SetLgtRange		(fEtc[0]);										// Cutoff range
		pObj->SetLgtFalloff		(fEtc[1]);										// Falloff

		pObj->SetLgtAttenuation0(fEtc[2]);										// Constant attenuation
		pObj->SetLgtAttenuation1(fEtc[3]);										// Linear attenuation
		pObj->SetLgtAttenuation2(fEtc[4]);										// Quadratic attenuation
		pObj->SetLgtTheta		(fEtc[5]);										// Inner angle of spotlight cone
		pObj->SetLgtPhi			(fEtc[6]);										// Outer angle of spotlight cone


		if(FAILED(pObj->Create(p1, NULL, NULL, NULL)))
		{
			// Return Error Notification
			delete pObj;
			return -1;
		}

		(*pData) = pObj;
		return 0;
	}

	return -1;
}


