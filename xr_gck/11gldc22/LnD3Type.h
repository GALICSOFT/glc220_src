// Typedef for the LcD3Type ...
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcD3Type_H_
#define _LcD3Type_H_

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

// Vector
typedef D3DXVECTOR2						VEC2;
typedef	D3DXVECTOR3						VEC3;
typedef D3DXVECTOR4						VEC4;
typedef D3DXMATRIX						MATA;
typedef D3DXQUATERNION					QUAT;
typedef D3DXPLANE						DPLN;
typedef D3DXCOLOR						DCLR;

typedef	LPDIRECT3D9						PD3D;
typedef LPDIRECT3DDEVICE9				PDEV;
typedef	LPD3DXSPRITE					PDSP;

typedef D3DVIEWPORT9					DVWP;

typedef	D3DLIGHT9						DLGT;
typedef D3DMATERIAL9					DMTL;
typedef LPDIRECT3DTEXTURE9				PDTX;
typedef	D3DXIMAGE_INFO					DIMG;
typedef	D3DSURFACE_DESC					DSDC;
typedef LPDIRECT3DCUBETEXTURE9			PDCB;
typedef LPDIRECT3DVOLUMETEXTURE9		PDVT;
typedef ID3DXRenderToEnvMap*			PDRE;


typedef LPDIRECT3DSURFACE9				PDSF;
typedef LPD3DXRENDERTOSURFACE			PDRS;
typedef	LPDIRECT3DSWAPCHAIN9			PDSW;									//	Swap chain

typedef LPDIRECT3DVERTEXBUFFER9			PDVB;
typedef LPDIRECT3DSTATEBLOCK9			PDBL;
typedef LPDIRECT3DINDEXBUFFER9			PDIB;

typedef LPDIRECT3DVERTEXSHADER9			PDVS;
typedef LPDIRECT3DPIXELSHADER9			PDPS;
typedef LPDIRECT3DVERTEXDECLARATION9	PDVD;
typedef LPD3DXCONSTANTTABLE				PDCT;
typedef	LPD3DXEFFECT					PDEF;

typedef LPD3DXBUFFER					PDBF;
typedef LPD3DXMESH						PDMS;									// Mesh
typedef	LPD3DXFONT						PDFT;									// Dx font

typedef	LPDIRECT3DQUERY9				PDQRY;
typedef D3DVERTEXELEMENT9				DVTE;


#endif

