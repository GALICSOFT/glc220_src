//	Copyright(C) GALICSOFT. All rights reserved.
//
//	Math Library.
//
//	Operator Overriding
//	Dot Product: '*'
//	Cross Product: '^'
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
  #pragma once
#endif


#ifndef _LcMath_H_
#define _LcMath_H_

#include <math.h>
#include "LcType.h"


enum
{
	LC_FIXED_PRECISION	=	(16),
	LC_FIXED_ONE		=	(1 << LC_FIXED_PRECISION),
	LC_FIXED_ZERO		=	(0),
};

#define LCX_PI			3.1415926535897932384626433832795
#define ONE_RADtoDEG	57.295779513082321
#define ONE_DEGtoRAD	0.0174532925199433
#define DEG90toRAD		1.5707963267948966
#define LCXToRadian(d)	(float)((d) * LCX_PI / 180.0)
#define LCXToDegree(r)	(float)((r) * 180.0  / LCX_PI)
#define RADtoDEG(v)		(float)( (v)*ONE_RADtoDEG)
#define DEGtoRAD(v)		(float)( (v)*ONE_DEGtoRAD)

#define LCX_ROUNDING_DELTA	0.00076
#define TOLER				0.00076

LC_INLINE INT LCXIsEqual(FLOAT a, FLOAT b, FLOAT eps=TOLER)   { return (fabs(a-b) < eps ? LC_TRUE:LC_FALSE); }
LC_INLINE INT LCXIsNotEqual(FLOAT a, FLOAT b, FLOAT eps=TOLER){ return (fabs(a-b) >=eps ? LC_TRUE:LC_FALSE); }

LC_INLINE QWORD LCXMAKEQWORD(DWORD l,DWORD h){ return (QWORD)( ((QWORD)l) | ((QWORD)h) << 32); }
LC_INLINE DWORD LCXMAKEDWORD(WORD  l,WORD  h){ return (DWORD)( ((DWORD)l) | ((DWORD)h) << 16); }
LC_INLINE WORD  LCXMAKEWORD (BYTE  l,BYTE  h){ return (WORD )( ((WORD)l ) | ((WORD)h ) <<  8); }
LC_INLINE DWORD LCXHIDWORD  (QWORD t)		{	return (DWORD)((t>>32)& 0xFFFFFFFF);}
LC_INLINE DWORD LCXLODWORD  (QWORD t)		{	return (DWORD)( t )	;				}
LC_INLINE WORD  LCXHIWORD   (DWORD t)		{	return (WORD )( (t>>16) & 0xFFFF);	}
LC_INLINE WORD  LCXLOWORD   (DWORD t)		{	return (WORD )( t )	;				}
LC_INLINE BYTE  LCXHIBYTE   (WORD  t)		{	return (BYTE )((t>>8) & 0xFF)	;	}
LC_INLINE BYTE  LCXLOBYTE   (WORD  t)		{	return (BYTE )( t )	;				}

LC_INLINE LC_FIXED LCXFixed(INT    v)		{	return v<< LC_FIXED_PRECISION;			}
LC_INLINE LC_FIXED LCXFixed(FLOAT  v)		{	return LC_FIXED(v * 65536.0F);			}
LC_INLINE LC_FIXED LCXFixed(DOUBLE v)		{	return LC_FIXED(v * 65536.00);			}

LC_INLINE FLOAT LCXFloatFromFixed(LC_FIXED v){	return (FLOAT)v/(FLOAT)(LC_FIXED_ONE);	}
LC_INLINE INT   LCXIntFromFixed(LC_FIXED v)	{	return v>> LC_FIXED_PRECISION;			}
LC_INLINE FLOAT LCXFloatCutFixed(FLOAT v)	{	return LCXFloatFromFixed(LCXFixed(v));	}

LC_INLINE LC_FIXED LCXFixedMul(LC_FIXED v1, LC_FIXED v2) { return LC_FIXED( ((DINT)v1 * (DINT)v2) >> LC_FIXED_PRECISION ); }
LC_INLINE LC_FIXED LCXFixedMiv(LC_FIXED v1, LC_FIXED v2) { return (LC_FIXED)( (((DINT)v1)<<LC_FIXED_PRECISION)/v2 );       }

LC_INLINE INT LCXQuantum2Power(INT v)		// to make power of 2
{
	INT n=1;
	for(; n<v; n <<= 1);
	return n;
}



////////////////////////////////////////////////////////////////////////////////
// Short Int type vector
// vector2 x=0, y=0
struct LCXVEC2s : public LC_SHORT2
{
	LCXVEC2s();
	LCXVEC2s(const LCXVEC2s&);
	LCXVEC2s(const SHORT*);
	LCXVEC2s(const LCXVEC2s*);
	LCXVEC2s(SHORT,SHORT);
	LCXVEC2s(const LC_SHORT2&);

	// casting
	operator SHORT*();
	operator const SHORT*() const;
	SHORT& operator[](int n);

	// assignment operators
	LCXVEC2s& operator +=(const LCXVEC2s&);
	LCXVEC2s& operator -=(const LCXVEC2s&);
	LCXVEC2s& operator *=(SHORT)	;
	LCXVEC2s& operator /=(SHORT)	;

	// unary operators
	LCXVEC2s operator +() const		;
	LCXVEC2s operator -() const		;

	// binary operators
	LCXVEC2s operator +(const LCXVEC2s&) const;
	LCXVEC2s operator -(const LCXVEC2s&) const;
	LCXVEC2s operator *(SHORT) const	;
	LCXVEC2s operator /(SHORT) const	;
	friend LCXVEC2s operator *(SHORT, const LCXVEC2s&);

	INT operator ==(const LCXVEC2s&) const;
	INT operator !=(const LCXVEC2s&) const;
};


// short3 x=0, y=0, z=0
// pack 2byte
#if defined(_MSC_VER)
  #pragma pack(push, 2)
#endif

struct LCXVEC3s : public LC_SHORT3
{
	LCXVEC3s();
	LCXVEC3s(const LCXVEC3s&);
	LCXVEC3s(const SHORT*);
	LCXVEC3s(const LCXVEC3s*);
	LCXVEC3s(SHORT,SHORT,SHORT);
	LCXVEC3s(const LC_SHORT3&);

	// casting
	operator SHORT*();
	operator const SHORT*() const;
	SHORT& operator[](int n);

	// assignment operators
	LCXVEC3s& operator +=(const LCXVEC3s&);
	LCXVEC3s& operator -=(const LCXVEC3s&);
	LCXVEC3s& operator *=(SHORT)	;
	LCXVEC3s& operator /=(SHORT)	;

	// unary operators
	LCXVEC3s operator +() const		;
	LCXVEC3s operator -() const		;

	// binary operators
	LCXVEC3s operator +(const LCXVEC3s&) const;
	LCXVEC3s operator -(const LCXVEC3s&) const;
	LCXVEC3s operator *(SHORT) const	;
	LCXVEC3s operator /(SHORT) const	;
	friend LCXVEC3s operator *(SHORT, const LCXVEC3s&);

	INT operator ==(const LCXVEC3s&) const;
	INT operator !=(const LCXVEC3s&) const;
}
#if defined(_MSC_VER)
  #pragma pack(pop)
#else
  __attribute__((packed))
#endif
;



struct LCXCOLOR;

// Colors Byte. color range [0, 255]
// Color r = 255, g = 255, b = 255, a = 255
struct LCXCOLORB : public LC_COLORB
{
	LCXCOLORB();
	LCXCOLORB(const LCXCOLORB&);
	LCXCOLORB(const BYTE*);
	LCXCOLORB(const FLOAT*);					// float array is rgba and range is [0,1]; r=v[0]*255, g=v[1]*255, b=v[2]*255, g=v[3]*255
	LCXCOLORB(const LCXCOLOR&);
	LCXCOLORB(BYTE R, BYTE G, BYTE B, BYTE A);
	LCXCOLORB(COLOR32 argb);					// Need ARGB 32 bit
	LCXCOLORB(const LC_COLORB&);
	LCXCOLORB(const LC_COLORF* v);

	// casting
	operator COLOR32 () const;
	operator BYTE* ();
	operator const BYTE* () const;
	BYTE& operator[](int n);

	// assignment operators
	LCXCOLORB& operator +=(const LCXCOLORB&);
	LCXCOLORB& operator -=(const LCXCOLORB&);
	LCXCOLORB& operator *=(const LCXCOLORB&);
	LCXCOLORB& operator *=(BYTE)	;
	LCXCOLORB& operator /=(BYTE)	;

	// unary operators
	LCXCOLORB operator +() const	;
	LCXCOLORB operator -() const	;

	// binary operators
	LCXCOLORB operator+ (const LCXCOLORB&) const;
	LCXCOLORB operator- (const LCXCOLORB&) const;
	LCXCOLORB operator* (const LCXCOLORB&) const;
	LCXCOLORB operator* (BYTE) const			;
	LCXCOLORB operator/ (BYTE) const			;
	friend LCXCOLORB operator* (BYTE, const LCXCOLORB&);

	INT operator == (const LCXCOLORB&) const	;
	INT operator != (const LCXCOLORB&) const	;
};


////////////////////////////////////////////////////////////////////////////////
// INT type vector

// vector2 x=0, y=0
struct LCXVEC2i : public LC_INT2
{
	LCXVEC2i();
	LCXVEC2i(const LCXVEC2i&);
	LCXVEC2i(const INT*);
	LCXVEC2i(const LCXVEC2i*);
	LCXVEC2i(INT,INT);
	LCXVEC2i(FLOAT,FLOAT);
	LCXVEC2i(const FLOAT*);
	LCXVEC2i(const LC_INT2&);

	// casting
	operator INT*();
	operator const INT*() const;
	INT& operator[](int n);
	const LCXVEC2i& operator=(const LCXVEC2i&);

	// assignment operators
	LCXVEC2i& operator +=(const LCXVEC2i&);
	LCXVEC2i& operator -=(const LCXVEC2i&);
	LCXVEC2i& operator *=(INT)	;
	LCXVEC2i& operator /=(INT)	;

	// unary operators
	LCXVEC2i operator +() const	;
	LCXVEC2i operator -() const	;

	// binary operators
	LCXVEC2i operator +(const LCXVEC2i&) const;
	LCXVEC2i operator -(const LCXVEC2i&) const;
	LCXVEC2i operator *(INT) const;
	LCXVEC2i operator /(INT) const;
	friend LCXVEC2i operator* (INT, const LCXVEC2i&);

	INT operator ==(const LCXVEC2i&) const;
	INT operator !=(const LCXVEC2i&) const;
};

// vector3 x=0, y=0, z=0
struct LCXVEC3i : public LC_INT3
{
	LCXVEC3i();
	LCXVEC3i(const LCXVEC3i&);
	LCXVEC3i(const INT*);
	LCXVEC3i(const LCXVEC3i*);
	LCXVEC3i(INT,INT,INT);
	LCXVEC3i(FLOAT,FLOAT,FLOAT);
	LCXVEC3i(const FLOAT*);
	LCXVEC3i(const LC_INT3&);

	// casting
	operator INT*();
	operator const INT*() const;
	INT& operator[](int n);
	const LCXVEC3i& operator=(const LCXVEC3i&);

	// assignment operators
	LCXVEC3i& operator +=(const LCXVEC3i&);
	LCXVEC3i& operator -=(const LCXVEC3i&);
	LCXVEC3i& operator *=(INT)	;
	LCXVEC3i& operator /=(INT)	;

	// unary operators
	LCXVEC3i operator +() const	;
	LCXVEC3i operator -() const	;

	// binary operators
	LCXVEC3i operator +(const LCXVEC3i&) const;
	LCXVEC3i operator -(const LCXVEC3i&) const;
	LCXVEC3i operator *(INT) const		;
	LCXVEC3i operator /(INT) const		;
	friend LCXVEC3i operator*(INT f,const LCXVEC3i& v);

	INT operator==(const LCXVEC3i&) const;
	INT operator!=(const LCXVEC3i&) const;
};


// vector4 x=0, y=0, z=0, w=0
struct LCXVEC4i : public LC_INT4
{
	LCXVEC4i();
	LCXVEC4i(const LCXVEC4i& r);
	LCXVEC4i(const INT*);
	LCXVEC4i(const LCXVEC4i*);
	LCXVEC4i(INT,INT,INT,INT);
	LCXVEC4i(FLOAT,FLOAT,FLOAT,FLOAT);
	LCXVEC4i(const LC_INT4&);

	// casting
	operator INT*();
	operator const INT*() const;
	INT& operator[](int n);
	const LCXVEC4i& operator=(const LCXVEC4i&);

	// assignment operators
	LCXVEC4i& operator +=(const LCXVEC4i&);
	LCXVEC4i& operator -=(const LCXVEC4i&);
	LCXVEC4i& operator *=(INT)	;
	LCXVEC4i& operator /=(INT)	;

	// unary operators
	LCXVEC4i operator +() const	;
	LCXVEC4i operator -() const	;

	// binary operators
	LCXVEC4i operator +(const LCXVEC4i&) const;
	LCXVEC4i operator -(const LCXVEC4i&) const;
	LCXVEC4i operator *(INT) const	;
	LCXVEC4i operator /(INT) const	;
	friend LCXVEC4i operator*(INT, const LCXVEC4i&);

	INT operator==(const LCXVEC4i& v) const;
	INT operator!=(const LCXVEC4i& v) const;
};


////////////////////////////////////////////////////////////////////////////////
// FLOAT type vector

struct LCXMATRIX;
struct LCXMATRIX4X3;
struct LCXMATRIX3X3;
struct LCXMATRIX3X2;

// vector2 x=0, y=0
struct LCXVECTOR2 : public LC_FLOAT2
{
	LCXVECTOR2();
	LCXVECTOR2(const LCXVECTOR2&);
	LCXVECTOR2(const FLOAT*);
	LCXVECTOR2(const LCXVECTOR2*);
	LCXVECTOR2(FLOAT,FLOAT);
	LCXVECTOR2(const LC_FLOAT2&);

	// casting
	operator FLOAT*();
	operator const FLOAT*() const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXVECTOR2& operator +=(const LCXVECTOR2&);
	LCXVECTOR2& operator -=(const LCXVECTOR2&);
	LCXVECTOR2& operator *=(FLOAT);
	LCXVECTOR2& operator /=(FLOAT);

	// unary operators
	LCXVECTOR2 operator +() const;
	LCXVECTOR2 operator -() const;

	// binary operators
	LCXVECTOR2 operator +(const LCXVECTOR2&) const;
	LCXVECTOR2 operator -(const LCXVECTOR2&) const;
	LCXVECTOR2 operator *(FLOAT) const;
	LCXVECTOR2 operator /(FLOAT) const;
	friend LCXVECTOR2 operator *(FLOAT, const LCXVECTOR2&);

	INT operator ==(const LCXVECTOR2&) const;
	INT operator !=(const LCXVECTOR2&) const;

	// Dot Product
	FLOAT operator *(const LCXVECTOR2&);
	friend FLOAT operator *(const LCXVECTOR2&, const LCXVECTOR2&);

	// Cross Product(Z-Value)
	FLOAT operator ^(const LCXVECTOR2&);
	friend FLOAT operator ^(const LCXVECTOR2&, const LCXVECTOR2&);

	// Transform: vector * Matrix3X3
	LCXVECTOR2 operator *(const LCXMATRIX3X3&);
	// Transform: Matrix3X3 * vector;
	friend LCXVECTOR2 operator *(const LCXMATRIX3X3&, const LCXVECTOR2&);

	FLOAT		Length();													// Length
	FLOAT		LengthSq();													// Length Square
	LCXVECTOR2	Normalize();												// Magnitude = 1
	LCXVECTOR2	Normalize(const LCXVECTOR2*);								// Copy and Normalize
	LCXVECTOR2& Negative();													// x= -x, y= -y
	LCXVECTOR2& Lerp(const LCXVECTOR2* v1, const LCXVECTOR2* v2, FLOAT t);	// this = (1-t)*v1 + t* v2
};


// vector3 x=0, y=0, z=0
struct LCXVECTOR3 : public LC_FLOAT3
{
	LCXVECTOR3();
	LCXVECTOR3(const LCXVECTOR3&);
	LCXVECTOR3(const FLOAT*);
	LCXVECTOR3(const LCXVECTOR3*);
	LCXVECTOR3(FLOAT,FLOAT,FLOAT);
	LCXVECTOR3(const LC_FLOAT3&);

	// casting
	operator FLOAT*();
	operator const FLOAT*() const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXVECTOR3& operator +=(const LCXVECTOR3&);
	LCXVECTOR3& operator -=(const LCXVECTOR3&);
	LCXVECTOR3& operator *=(FLOAT);
	LCXVECTOR3& operator /=(FLOAT);

	// unary operators
	LCXVECTOR3 operator +() const;
	LCXVECTOR3 operator -() const;

	// binary operators
	LCXVECTOR3 operator +(const LCXVECTOR3&) const;
	LCXVECTOR3 operator -(const LCXVECTOR3&) const;
	LCXVECTOR3 operator *(FLOAT) const;
	LCXVECTOR3 operator /(FLOAT) const;
	friend LCXVECTOR3 operator *(FLOAT, const LCXVECTOR3&);

	INT operator ==(const LCXVECTOR3&) const;
	INT operator !=(const LCXVECTOR3&) const;

	// Dot Product
	FLOAT operator *(const LCXVECTOR3&);
	friend FLOAT operator *(const LCXVECTOR3&, const LCXVECTOR3&);

	// Cross Product
	LCXVECTOR3 operator ^(const LCXVECTOR3&);
	friend LCXVECTOR3 operator ^(const LCXVECTOR3&, const LCXVECTOR3&);

	// Transform: vector * Matrix4x4
	LCXVECTOR3 operator *(const LCXMATRIX&);

	// Transform: Matrix4x4 * vector;
	friend LCXVECTOR3 operator *(const LCXMATRIX&, const LCXVECTOR3&);

	FLOAT		Length();													// Length
	FLOAT		LengthSq();													// Length Square
	LCXVECTOR3	Normalize();												// Magnitude = 1
	LCXVECTOR3	Normalize(const LCXVECTOR3*);								// Copy and Normalize
	LCXVECTOR3	Cross(const LCXVECTOR3*, const LCXVECTOR3*);				// this = A x B
	LCXVECTOR3& Negative();													// x= -x, y= -y, z= -z
	LCXVECTOR3& Lerp(const LCXVECTOR3* v1, const LCXVECTOR3* v2, FLOAT t);	// this = (1-t)*v1 + t* v2
};


// vector4 x=0, y=0, z=0, w=0
struct LCXVECTOR4 : public LC_FLOAT4
{
	LCXVECTOR4();
	LCXVECTOR4(const LCXVECTOR4&);
	LCXVECTOR4(const FLOAT*);
	LCXVECTOR4(const LCXVECTOR4*);
	LCXVECTOR4(FLOAT,FLOAT,FLOAT,FLOAT);
	LCXVECTOR4(const LC_FLOAT4& v);

	// casting
	operator FLOAT*();
	operator const FLOAT*() const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXVECTOR4& operator +=(const LCXVECTOR4&);
	LCXVECTOR4& operator -=(const LCXVECTOR4&);
	LCXVECTOR4& operator *=(FLOAT v);
	LCXVECTOR4& operator /=(FLOAT v);

	// unary operators
	LCXVECTOR4 operator +() const;
	LCXVECTOR4 operator -() const;

	// binary operators
	LCXVECTOR4 operator +(const LCXVECTOR4&) const;
	LCXVECTOR4 operator -(const LCXVECTOR4&) const;
	LCXVECTOR4 operator *(FLOAT) const;
	LCXVECTOR4 operator /(FLOAT) const;
	friend LCXVECTOR4 operator *(FLOAT, const LCXVECTOR4&);

	INT operator ==(const LCXVECTOR4&) const;
	INT operator !=(const LCXVECTOR4&) const;

	// Dot Product
	FLOAT operator *(const LCXVECTOR4&);
	friend FLOAT operator *(const LCXVECTOR4&, const LCXVECTOR4&);

	// Transform: vector * Matrix4x4
	LCXVECTOR4 operator *(const LCXMATRIX&);

	// Transform: Matrix4x4 * vector;
	friend LCXVECTOR4 operator *(const LCXMATRIX&, const LCXVECTOR4&);

	FLOAT		Length();													// Length
	FLOAT		LengthSq();													// Length Square
	LCXVECTOR4	Normalize();												// Magnitude = 1
	LCXVECTOR4	Normalize(const LCXVECTOR4*);								// Copy and Normalize
	LCXVECTOR4& Negative();													// x= -x, y = -y, z = -z, w = -w
	LCXVECTOR4& Lerp(const LCXVECTOR4* v1, const LCXVECTOR4* v2, FLOAT t);	// this = (1-t)*v1 + t* v2
};



////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 ==> Identity Matrix

struct LCXMATRIX : public LC_MATRIX
{
	LCXMATRIX();
	LCXMATRIX(const LCXMATRIX&);
	LCXMATRIX(const FLOAT*);
	LCXMATRIX(const LCXMATRIX*);
	LCXMATRIX(	FLOAT _11,FLOAT _12,FLOAT _13,FLOAT _14,
				FLOAT _21,FLOAT _22,FLOAT _23,FLOAT _24,
				FLOAT _31,FLOAT _32,FLOAT _33,FLOAT _34,
				FLOAT _41,FLOAT _42,FLOAT _43,FLOAT _44 );

	LCXMATRIX(const LC_MATRIX&);

	// access grants
	FLOAT& operator () ( int iRow, int iCol );
	FLOAT  operator () ( int iRow, int iCol ) const;

	// casting operators
	operator FLOAT* ();
	operator const FLOAT* () const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXMATRIX& operator *=(const LCXMATRIX&);
	LCXMATRIX& operator +=(const LCXMATRIX&);
	LCXMATRIX& operator -=(const LCXMATRIX&);
	LCXMATRIX& operator *=(FLOAT);
	LCXMATRIX& operator /=(FLOAT);

	// unary operators
	LCXMATRIX operator +() const;
	LCXMATRIX operator -() const;

	LCXMATRIX operator *(const LCXMATRIX&) const;
	LCXMATRIX operator +(const LCXMATRIX&) const;
	LCXMATRIX operator -(const LCXMATRIX&) const;
	LCXMATRIX operator *(FLOAT) const;
	LCXMATRIX operator /(FLOAT) const;

	// const * Matrix
	friend LCXMATRIX operator *(FLOAT, const LCXMATRIX&);						// number * matrix
	friend LCXMATRIX operator *(const LCXMATRIX&, FLOAT);						// matrix * number
	friend LCXVECTOR4 operator *(LCXVECTOR4&, const LCXMATRIX&);				// vector4 * matrix
	friend LCXVECTOR4 operator *(const LCXMATRIX&, LCXVECTOR4&);				// matrix * vector4
	friend LCXVECTOR3 operator *(LCXVECTOR3&, const LCXMATRIX&);				// vector3 * matrix. return vector is x, y, z, w => x/w, y/w, z/w
	friend LCXVECTOR3 operator *(const LCXMATRIX&, LCXVECTOR3&);				// matrix * vector3

	INT operator ==(const LCXMATRIX&) const;
	INT operator !=(const LCXMATRIX&) const;

	// basic operation
	LCXMATRIX&	Identity();														// Identity (1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1)
	LCXMATRIX&	Transpose();													// Transpose: M[i,j] -> M[j,i]
	LCXMATRIX&	Transpose(const LCXMATRIX*);
	LCXMATRIX&	Inverse(FLOAT* det, INT bQuick=LC_TRUE);						// if the 'bQuick' is not false then this function will be calculating the inverse matrix  except _14=0, _24=0, _34=0, _44=1
	LCXMATRIX&	Inverse(FLOAT* det, const LCXMATRIX* In, INT bQuick=LC_TRUE);	//
	FLOAT		Determinant() const;											//

	// scaling Matrix
	LCXMATRIX&	SetupScaling(FLOAT X, FLOAT Y, FLOAT Z);
	LCXMATRIX&	SetupScaling(const LCXVECTOR3* vScl);

	// Viewing Matrix
	LCXMATRIX&	SetupViewD3dLH(const LCXVECTOR3* Eye, const LCXVECTOR3* LookAt, const LCXVECTOR3* Up);	// D3D LH
	LCXMATRIX&	SetupViewD3dRH(const LCXVECTOR3* Eye, const LCXVECTOR3* LookAt, const LCXVECTOR3* Up);	// D3D RH
	LCXMATRIX&	SetupViewGl   (const LCXVECTOR3* Eye, const LCXVECTOR3* LookAt, const LCXVECTOR3* Up);	// OpenGL RH it's same to SetupViewD3dRH
	LCXMATRIX&	SetupView3DMax(const LCXVECTOR3* Eye, const LCXVECTOR3* LookAt, const LCXVECTOR3* Up	// It likes 3D Max system. If you use this function, you have to change x and z and upvector will be (0,0,1). see ex1 projection sample.	// It likes 3D Max system. If you use this function, you have to change x and z and upvector will be (0,0,1). see ex1 projection sample.
													, INT bExchangeYZ=LC_FALSE);						// if bExChangeYZis not false, this function will be exchanging the Y and Z axis values.

	// Projection Matrix
	LCXMATRIX&	SetupPerspectiveD3dLH(FLOAT FOV, FLOAT Aspect, FLOAT fNear, FLOAT fFar);				// Direct3D Perspective Left Handle Rule Matrix x and y: [-1, 1], z: [0, 1]
	LCXMATRIX&	SetupPerspectiveD3dRH(FLOAT FOV, FLOAT Aspect, FLOAT fNear, FLOAT fFar);				// Direct3D Perspective Light Handle Rule Matrix
	LCXMATRIX&	SetupPerspectiveGl   (FLOAT FOV, FLOAT Aspect, FLOAT fNear, FLOAT fFar);				// OpenGL Perspective Matrix. It's same to glFrustum
	LCXMATRIX&	SetupOrthoGl(FLOAT Left, FLOAT Right, FLOAT Bottom, FLOAT Top, FLOAT fNear, FLOAT fFar);// OpenGL Orthogonal Matrix.  It's same to glOrtho and D3DXMatrixOrthoOffCenterRH

	// Rotation Matrix
	LCXMATRIX&	SetupRotationX(FLOAT fRad);
	LCXMATRIX&	SetupRotationY(FLOAT fRad);
	LCXMATRIX&	SetupRotationZ(FLOAT fRad);
	LCXMATRIX&	SetupRotationAxis(FLOAT x, FLOAT y, FLOAT z, FLOAT fRad);		// x, y, z is axis
	LCXMATRIX&	SetupRotationAxis(const LCXVECTOR3* vAxis, FLOAT fRad);
	LCXMATRIX&	SetupRotationAxis(const LCXVECTOR4* vAxis);						// vAxis.w is radian
	LCXMATRIX&	SetupTranslation(FLOAT x,FLOAT y,FLOAT z,INT bIdentity=LC_TRUE);
	LCXMATRIX&	SetupTranslation(const LCXVECTOR3* vTrs, INT bIdentity=LC_TRUE);

	LCXMATRIX&	SetupFrom4X3(const LCXMATRIX4X3* tm);							// 4x3 matrix must be transform matrix. => _14=_24=_34=0, _44=1
	LCXMATRIX&	SetupFrom3X3(const LCXMATRIX3X3* tm);							// 3x3 matrix must be transform matrix. => _31=_32=_34=_43=0,         _33= 1, _41=tm->_31. _42=tm->_32, _44=tm->_33
	LCXMATRIX&	SetupFrom3X2(const LCXMATRIX3X2* tm);							// 3x2 matrix must be transform matrix. => _14=_24=_31=_32=_34=_43=0, _33= 1, _41=tm->_31. _42=tm->_32, _44=1


	// calculating the transform of vector by this matrix.
	void		TransformCoord(LCXVECTOR3* Out, const LCXVECTOR3* In);			// Out = In * M

	LCXMATRIX&  Negative();														// Mij = - Mij

	// Kinear interpolation between m1 and m2 with weight.
	LCXMATRIX&  Lerp(const LCXMATRIX* m1, const LCXMATRIX* m2, FLOAT t);		// this = (1-t)*v1 + t* v2
	LCXMATRIX&  Lerp(const LCXMATRIX4X3* m1, const LCXMATRIX4X3* m2, FLOAT t);	// this = (1-t)*v1 + t* v2
};

typedef LCXMATRIX LCXMAT4X4;



////////////////////////////////////////////////////////////////////////////////
// Quaternion: x=0, y=0, z=0, w=1
struct LCXQUATERNION : public LC_FLOAT4
{
	LCXQUATERNION();
	LCXQUATERNION(const LCXQUATERNION&);
	LCXQUATERNION(const FLOAT*);
	LCXQUATERNION(const LCXQUATERNION*);
	LCXQUATERNION(FLOAT x,FLOAT y,FLOAT z,FLOAT w);
	LCXQUATERNION(const LC_FLOAT4&);

	// casting
	operator FLOAT* ();
	operator const FLOAT* () const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXQUATERNION& operator +=(const LCXQUATERNION&);
	LCXQUATERNION& operator -=(const LCXQUATERNION&);
	LCXQUATERNION& operator *=(const LCXQUATERNION&);
	LCXQUATERNION& operator *=(FLOAT);
	LCXQUATERNION& operator /=(FLOAT);

	// unary operators
	LCXQUATERNION  operator +() const;
	LCXQUATERNION  operator -() const;

	// binary operators
	LCXQUATERNION operator +(const LCXQUATERNION&) const;
	LCXQUATERNION operator -(const LCXQUATERNION&) const;
	LCXQUATERNION operator *(const LCXQUATERNION&) const;
	LCXQUATERNION operator *(FLOAT) const;
	LCXQUATERNION operator /(FLOAT) const;
	friend LCXQUATERNION operator *(FLOAT,const LCXQUATERNION&);

	INT operator ==(const LCXQUATERNION&) const;
	INT operator !=(const LCXQUATERNION&) const;

	LCXQUATERNION&	Identity();
	LCXQUATERNION&	Conjugate();
	LCXQUATERNION&	Negative();													// x=-x, y=-y, z=-z, w=-w

	void SLerp(const LCXQUATERNION* q1, const LCXQUATERNION* q2, FLOAT t);		//Q = (1/sin¥è)[  sin (¥è*(1-t)) * Q1 + sin (¥è*t) * Q2]
	void RotationMatrix(LCXMATRIX* Out, INT bLHR = LC_TRUE);					// LHR = Left Hand Rule. ex)Direct3D
};


////////////////////////////////////////////////////////////////////////////////
// Colors r=1.0F, g=1.0F, b=1.0F, a=1.0F
struct LCXCOLOR : LC_COLORF
{
	LCXCOLOR();
	LCXCOLOR(const LCXCOLOR&);
	LCXCOLOR(const FLOAT *);
	LCXCOLOR(const LCXCOLOR*);
	LCXCOLOR(COLOR32 argb);
	LCXCOLOR(const BYTE* rgba/*r[0],g[1],b[2],a[3]*/);
	LCXCOLOR(FLOAT r,FLOAT g,FLOAT b,FLOAT a);
	LCXCOLOR(const LC_COLORF&);

	// casting
	operator COLOR32 () const;
	operator FLOAT* ();
	operator const FLOAT* () const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXCOLOR& operator +=(const LCXCOLOR&);
	LCXCOLOR& operator -=(const LCXCOLOR&);
	LCXCOLOR& operator *=(FLOAT);
	LCXCOLOR& operator /=(FLOAT);

	// unary operators
	LCXCOLOR operator +() const;
	LCXCOLOR operator -() const;								// Negative: return LCXCOLOR(1.F-r, 1.F-g, 1.F-b, a)

	// binary operators
	LCXCOLOR operator +(const LCXCOLOR&) const;					// Add
	LCXCOLOR operator -(const LCXCOLOR&) const;					// Substract
	LCXCOLOR operator *(FLOAT) const;							// Modulate
	LCXCOLOR operator /(FLOAT) const;							// Devide
	friend LCXCOLOR operator *(FLOAT,const LCXCOLOR&);			// Scaling

	INT operator == (const LCXCOLOR&) const;
	INT operator != (const LCXCOLOR&) const;

	LCXCOLOR& Negative();												// r= 1.F-r, g = 1.F-g, b=1.F-b, a=a
	LCXCOLOR& Lerp(const LCXCOLOR* v1, const LCXCOLOR* v2, FLOAT t);	// this = (1-t)*v1 + t* v2
};


////////////////////////////////////////////////////////////////////////////////
// Plane (3D)
struct LCXPLANE : public LC_FLOAT4
{
	LCXPLANE();
	LCXPLANE(const LCXPLANE&);
	LCXPLANE(const LCXVECTOR3&, const LCXVECTOR3&, const LCXVECTOR3&);	// Like setup from points
	LCXPLANE(const FLOAT*);
	LCXPLANE(FLOAT a,FLOAT b,FLOAT c,FLOAT d);

	// casting
	operator FLOAT* ();
	operator const FLOAT* () const;
	FLOAT& operator[](int n);

	// unary operators
	LCXPLANE operator + () const;
	LCXPLANE operator - () const;

	// binary operators
	INT operator == ( const LCXPLANE& ) const;
	INT operator != ( const LCXPLANE& ) const;

	void SetupFromPointNormal(const LCXVECTOR3* point, const LCXVECTOR3* normal);
	void SetupFromPoints(const LCXVECTOR3* p0, const LCXVECTOR3* p1, const LCXVECTOR3* p2);
};

// Plane 2D(z=0)
struct LCXPLANE2
{
	union {	struct { FLOAT a; FLOAT b; FLOAT d; }; FLOAT m[3]; };

	LCXPLANE2();
	LCXPLANE2(const LCXPLANE2&);
	LCXPLANE2(const LCXVECTOR2&, const LCXVECTOR2&);
	LCXPLANE2(const FLOAT*);
	LCXPLANE2(FLOAT a,FLOAT b,FLOAT d);

	// casting
	operator FLOAT* ();
	operator const FLOAT* () const;
	FLOAT& operator[](int n);

	// unary operators
	LCXPLANE2 operator + () const;
	LCXPLANE2 operator - () const;

	// binary operators
	INT operator == ( const LCXPLANE2& ) const;
	INT operator != ( const LCXPLANE2& ) const;

	void SetupFromPointNormal(const LCXVECTOR2* point, const LCXVECTOR2* normal);	// z value of all vectors is 0
	void SetupFromPoints(const LCXVECTOR2* p0, const LCXVECTOR2* p1);				// N = (p1-p0) X Z(0,0,1)
};


////////////////////////////////////////////////////////////////////////////////
// 4X3 Matrix

struct LCXMATRIX4X3 : public LC_MATRIX4X3
{
	LCXMATRIX4X3();
	LCXMATRIX4X3(const LCXMATRIX4X3&);
	LCXMATRIX4X3(const FLOAT*);
	LCXMATRIX4X3(const LCXMATRIX4X3*);
	LCXMATRIX4X3(const LCXVECTOR3*);
	LCXMATRIX4X3(const LCXMATRIX&);
	LCXMATRIX4X3(const LCXMATRIX*);

	LCXMATRIX4X3(
				FLOAT _11,FLOAT _12,FLOAT _13,
				FLOAT _21,FLOAT _22,FLOAT _23,
				FLOAT _31,FLOAT _32,FLOAT _33,
				FLOAT _41,FLOAT _42,FLOAT _43 );

	LCXMATRIX4X3(const LC_MATRIX&);

	// access grants
	FLOAT& operator () ( int iRow, int iCol );
	FLOAT  operator () ( int iRow, int iCol ) const;

	// casting operators
	operator FLOAT* ();
	operator const FLOAT* () const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXMATRIX4X3& operator +=(const LCXMATRIX4X3&);
	LCXMATRIX4X3& operator -=(const LCXMATRIX4X3&);
	LCXMATRIX4X3& operator *=(FLOAT);
	LCXMATRIX4X3& operator /=(FLOAT);

	// unary operators
	LCXMATRIX4X3 operator +() const;
	LCXMATRIX4X3 operator -() const;

	// binary operators
	LCXMATRIX4X3 operator +(const LCXMATRIX4X3&) const;
	LCXMATRIX4X3 operator -(const LCXMATRIX4X3&) const;
	LCXMATRIX4X3 operator *(FLOAT) const;
	LCXMATRIX4X3 operator /(FLOAT) const;

	// const * Matrix
	friend LCXMATRIX4X3 operator *(FLOAT, const LCXMATRIX4X3&);
	friend LCXMATRIX4X3 operator *(const LCXMATRIX4X3&, FLOAT);

	// const * Matrix
	friend LCXMATRIX operator *(FLOAT, const LCXMATRIX&);						// number * matrix
	friend LCXMATRIX operator *(const LCXMATRIX&, FLOAT);						// matrix * number
	friend LCXVECTOR4 operator *(LCXVECTOR4&, const LCXMATRIX&);				// vector4 * matrix
	friend LCXVECTOR4 operator *(const LCXMATRIX&, LCXVECTOR4&);				// matrix * vector4
	friend LCXVECTOR3 operator *(LCXVECTOR3&, const LCXMATRIX&);				// vector3 * matrix. return vector is x, y, z, w => x/w, y/w, z/w
	friend LCXVECTOR3 operator *(const LCXMATRIX&, LCXVECTOR3&);				// matrix * vector3


	INT operator ==(const LCXMATRIX4X3&) const;
	INT operator !=(const LCXMATRIX4X3&) const;

	LCXMATRIX4X3& Identity();
	LCXMATRIX4X3& SetupFrom4X4(const LCXMATRIX*);
	void          TransformCoord(LCXVECTOR3* Out, const LCXVECTOR3* In);

	LCXMATRIX4X3& Negative();													// Mij = - Mij
	LCXMATRIX4X3& Lerp(const LCXMATRIX4X3* v1,const LCXMATRIX4X3* v2, FLOAT t);	// this = (1-t)*v1 + t* v2
};

typedef LCXMATRIX4X3 LCXMAT4X3;



////////////////////////////////////////////////////////////////////////////////
// 3X3 Matrix

struct LCXMATRIX3X3 : public LC_MATRIX3X3
{
	LCXMATRIX3X3();
	LCXMATRIX3X3(const LCXMATRIX3X3&);
	LCXMATRIX3X3(const FLOAT*);
	LCXMATRIX3X3(const LCXMATRIX3X3*);
	LCXMATRIX3X3(
				FLOAT _11,FLOAT _12,FLOAT _13,
				FLOAT _21,FLOAT _22,FLOAT _23,
				FLOAT _31,FLOAT _32,FLOAT _33  );

	LCXMATRIX3X3(const LCXMATRIX&);

	// access grants
	FLOAT& operator () ( int iRow, int iCol );
	FLOAT  operator () ( int iRow, int iCol ) const;

	// casting operators
	operator FLOAT* ();
	operator const FLOAT* () const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXMATRIX3X3& operator *=(const LCXMATRIX3X3&);
	LCXMATRIX3X3& operator +=(const LCXMATRIX3X3&);
	LCXMATRIX3X3& operator -=(const LCXMATRIX3X3&);
	LCXMATRIX3X3& operator *=(FLOAT);
	LCXMATRIX3X3& operator /=(FLOAT);

	// unary operators
	LCXMATRIX3X3 operator +() const;
	LCXMATRIX3X3 operator -() const;

	LCXMATRIX3X3 operator *(const LCXMATRIX3X3&) const;
	LCXMATRIX3X3 operator +(const LCXMATRIX3X3&) const;
	LCXMATRIX3X3 operator -(const LCXMATRIX3X3&) const;
	LCXMATRIX3X3 operator *(FLOAT) const;
	LCXMATRIX3X3 operator /(FLOAT) const;

	// const * Matrix
	friend LCXMATRIX3X3 operator *(FLOAT, const LCXMATRIX3X3&);
	friend LCXMATRIX3X3 operator *(const LCXMATRIX3X3&, FLOAT);
	friend LCXVECTOR3 operator *(LCXVECTOR3&, const LCXMATRIX3X3&);				// vector3 * matrix
	friend LCXVECTOR3 operator *(const LCXMATRIX3X3&, LCXVECTOR3&);				// matrix * vector3
	friend LCXVECTOR2 operator *(LCXVECTOR2&, const LCXMATRIX3X3&);				// vector2 * matrix. return vector is x, y, z => x/z, y/z
	friend LCXVECTOR2 operator *(const LCXMATRIX3X3&, LCXVECTOR2&);				// matrix * vector2

	INT operator ==(const LCXMATRIX3X3&) const;
	INT operator !=(const LCXMATRIX3X3&) const;

	LCXMATRIX3X3& Identity();
	LCXMATRIX3X3& Transpose();
	LCXMATRIX3X3& Transpose(const LCXMATRIX3X3*);
	LCXMATRIX3X3& Inverse(FLOAT* det);
	LCXMATRIX3X3& Inverse(FLOAT* det, const LCXMATRIX3X3* In);
	FLOAT		  Determinant();

	LCXMATRIX3X3& SetupScaling(FLOAT X, FLOAT Y);
	LCXMATRIX3X3& SetupScaling(const LCXVECTOR2* vScl);
	LCXMATRIX3X3& SetupRotation(FLOAT fRad);
	LCXMATRIX3X3& SetupTranslation(FLOAT x, FLOAT y, INT bIdentity=LC_TRUE);
	LCXMATRIX3X3& SetupTranslation(const LCXVECTOR2* vTrs, INT bIdentity=LC_TRUE);

	void		  TransformCoord(LCXVECTOR2* Out, const LCXVECTOR2* In);
	LCXMATRIX3X3& Negative();													// Mij = - Mij
	LCXMATRIX3X3& Lerp(const LCXMATRIX3X3* v1,const LCXMATRIX3X3* v2, FLOAT t);	// this = (1-t)*v1 + t* v2
};

typedef LCXMATRIX3X3 LCXMAT3X3;


////////////////////////////////////////////////////////////////////////////////
// 3X2 Matrix
// LCXMATRIX3X2 matrix is the specialized LCXMATRIX3X3(_11, _12, 0,  _21, _22, 0,   _31, _32, 1);
struct LCXMATRIX3X2 : public LC_MATRIX3X2
{
	LCXMATRIX3X2();
	LCXMATRIX3X2(const LCXMATRIX3X2&);
	LCXMATRIX3X2(const FLOAT*);
	LCXMATRIX3X2(const LCXMATRIX3X2*);
	LCXMATRIX3X2(
				FLOAT _11,FLOAT _12,
				FLOAT _21,FLOAT _22,
				FLOAT _31,FLOAT _32  );

	LCXMATRIX3X2(const LCXMATRIX3X3&);

	// access grants
	FLOAT& operator () ( int iRow, int iCol );
	FLOAT  operator () ( int iRow, int iCol ) const;

	// casting operators
	operator FLOAT* ();
	operator const FLOAT* () const;
	FLOAT& operator[](int n);

	// assignment operators
	LCXMATRIX3X2& operator *=(const LCXMATRIX3X2&);								//M[3,2] * M[3,2] is not allowed in matrix but it will be run same to [3,3(0,0,1)] * [3,3(0,0,1)]
	LCXMATRIX3X2& operator +=(const LCXMATRIX3X2&);
	LCXMATRIX3X2& operator -=(const LCXMATRIX3X2&);
	LCXMATRIX3X2& operator *=(FLOAT);
	LCXMATRIX3X2& operator /=(FLOAT);

	// unary operators
	LCXMATRIX3X2 operator +() const;
	LCXMATRIX3X2 operator -() const;

	LCXMATRIX3X2 operator *(const LCXMATRIX3X2&) const;
	LCXMATRIX3X2 operator +(const LCXMATRIX3X2&) const;
	LCXMATRIX3X2 operator -(const LCXMATRIX3X2&) const;
	LCXMATRIX3X2 operator *(FLOAT) const;
	LCXMATRIX3X2 operator /(FLOAT) const;

	// const * Matrix
	friend LCXMATRIX3X2 operator *(FLOAT, const LCXMATRIX3X2&);
	friend LCXMATRIX3X2 operator *(const LCXMATRIX3X2&, FLOAT);

	friend LCXVECTOR2 operator *(LCXVECTOR2&, const LCXMATRIX3X2&);				// vector2 * matrix. return vector is x, y, z => x/z, y/z
	//friend LCXVECTOR2 operator *(const LCXMATRIX3X2&, LCXVECTOR2&);			// matrix * vector2 is not allowed

	INT operator ==(const LCXMATRIX3X2&) const;
	INT operator !=(const LCXMATRIX3X2&) const;

	LCXMATRIX3X2& Identity();
	LCXMATRIX3X2& Inverse(FLOAT* det);
	LCXMATRIX3X2& Inverse(FLOAT* det, const LCXMATRIX3X2* In);
	FLOAT		  Determinant();

	LCXMATRIX3X2& SetupScaling(FLOAT X, FLOAT Y);
	LCXMATRIX3X2& SetupScaling(const LCXVECTOR2* vScl);
	LCXMATRIX3X2& SetupRotation(FLOAT fRad);
	LCXMATRIX3X2& SetupTranslation(FLOAT x, FLOAT y, INT bIdentity=LC_TRUE);
	LCXMATRIX3X2& SetupTranslation(const LCXVECTOR2* vTrs, INT bIdentity=LC_TRUE);

	void		  TransformCoord(LCXVECTOR2* Out, const LCXVECTOR2* In);
	LCXMATRIX3X2& Negative();													// Mij = - Mij
	LCXMATRIX3X2& Lerp(const LCXMATRIX3X2* v1,const LCXMATRIX3X2* v2, FLOAT t);	// this = (1-t)*v1 + t* v2
};

typedef LCXMATRIX3X2 LCXMAT3X2;


////////////////////////////////////////////////////////////////////////////////
// LCXRECT x=0, y=0, w=0, h=0
struct LCXRECT : public LC_RECTF
{
	LCXRECT();
	LCXRECT(const LCXRECT&);
	LCXRECT(const FLOAT*);
	LCXRECT(const LCXRECT*);
	LCXRECT(FLOAT,FLOAT,FLOAT,FLOAT);
	LCXRECT(const LC_RECTF& v);

	// casting
	operator FLOAT*();
	operator const FLOAT*() const;
	FLOAT& operator[](int n);

	// unary operators
	LCXRECT operator +() const;
	LCXRECT operator -() const;

	INT operator ==(const LCXRECT&) const;
	INT operator !=(const LCXRECT&) const;


	void ToRegion();						// right, bottom to w and height
	void ToRegion(LCXRECT* v);
	void ToRegion(FLOAT tLeft, FLOAT tTop, FLOAT tRight, FLOAT tBottom);

	void ToRect();							// width, height to right and bottom
	void ToRect(LCXRECT* v);

	INT IsInRect(const FLOAT* v);			// is a vector2 in rect?
	INT IsInRect(const LCXVECTOR2* v);
	INT IsInRect(const LCXRECT* v);

	INT Collision(const FLOAT* v);
	INT Collision(const LCXRECT* v);
};


// collision for RECT in 2d program
template<class T>
void LCXRect2Region(T* v)		// right, bottom to w and height
{
	v->w = v->right  - v->left;
	v->h = v->bottom - v->top;
}

template<class T>
void LCXRegion2Rect(T* v)		// width, height to right and bottom
{
	v->right = v->left + v->_w;
	v->bottom= v->top  + v->_h;
}


template<class T1, class T2>
INT LCXIsInRect(const T1& p, const T2& rc)
{
	if( rc.x > p.x || p.x > rc.x + rc.w ||
		rc.y > p.y || p.y > rc.y + rc.h )
		return LC_EFAIL;

	return LC_OK;
}

template<class T>
INT LCXIsInRect(const T& p, FLOAT x, FLOAT y, FLOAT w, FLOAT h)	// x: left, y: top, w: width, h: height
{
	if( x > p.x || p.x > x + w ||
		y > p.y || p.y > y + h )
		return LC_EFAIL;

	return LC_OK;
}


template<class T>				// rect must be x, y, w, h
INT LCXCollisionRect2Rect(const T& v1, const T& v2)
{
	if(	v1.x        > v2.x + v2.w ||	// ! v1.left   <= v2.right
		v1.x + v1.w < v2.x        ||	// ! v1.right  >= v2.left
		v1.y        > v2.y + v2.h ||	// ! v1.top    <= v2.bottom
		v1.y + v1.h < v2.y		)		// ! v1.bottom >= v2.top
		return LC_EFAIL;

	return LC_OK;
}


template<class T>				// Collision: lt: (left, top), rb: (right,bottom)
INT LCXCollisionRect2Rect(const T& v1_lt, const T& v1_rb, const T& v2_lt, const T& v2_rb)
{
	if( v1_lt.x > v2_rb.x ||			// ! v1.left   <= v2.right
		v1_rb.x < v2_lt.x ||			// ! v1.right  >= v2.left
		v1_lt.y > v2_rb.y ||			// ! v1.top    <= v2.bottom
		v1_rb.y < v2_lt.y  )			// ! v1.bottom >= v2.top
		return LC_EFAIL;

	return LC_OK;
}

template<class T>				// Collision :  (left, top, width, height)
INT LCXCollisionRect2RectLTWH(T x1, T y1, T w1, T h1
						, T x2, T y2, T w2, T h2)
{
	if( x1      > x2 + w2 ||			// ! left1   <= right2
		x1 + w1 < x2      ||			// ! right1  >= left2
		y1      > y2 + h2 ||			// ! top1    <= bottom2
		y1 + h1 < y2       )			// ! bottom1 >= top2
		return LC_EFAIL;

	return LC_OK;
}

template<class T>				// Collision:  (left, top, right ,bottom)
INT LCXCollisionRect2RectLTRB(T l1, T r1, T t1, T b1
						, T l2, T r2, T t2, T b2)
{
	if( l1 > r2 ||						// ! left1   <= right2
		r1 < l2 ||						// ! right1  >= left2
		t1 > b2 ||						// ! top1    <= bottom2
		b1 < t2  )						// ! bottom1 >= top2
		return LC_EFAIL;

	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////
// Math Table

FLOAT LCXMathCos(FLOAT Radian);			// sin value from sine table
FLOAT LCXMathSin(FLOAT Radian);			// cos value from cosine table
FLOAT LCXMathTan(FLOAT Radian);			// tangent value value from LCX_Sin/LCX_Cos
FLOAT LCXMathAcos(FLOAT v);				// acos value from arccosine table


////////////////////////////////////////////////////////////////////////////////
// Vector2

FLOAT LCXVec2Dot(const LCXVECTOR2* v1, const LCXVECTOR2* v2);					// dot2 product
FLOAT LCXVec2Length(const LCXVECTOR2* v);										// vector2 Length
FLOAT LCXVec2LengthSq(const LCXVECTOR2* v);										// vector2 Square Length
FLOAT LCXVec2Normalize(LCXVECTOR2* Out, const LCXVECTOR2* In);					// normalize return length
FLOAT LCXVec2Cross(const LCXVECTOR2* v1, const LCXVECTOR2* v2);					// return the z value
void  LCXVec2Lerp(LCXVECTOR2* Out
				  , const LCXVECTOR2* v1, const LCXVECTOR2* v2, FLOAT t);		// Out = (1-t)*v1 + t* v2

void LCXVec2MakeBoundingBox(LCXVECTOR2* vcMin, LCXVECTOR2* vcMax				// make out min, max of the bounding box from the List
							, const LCXVECTOR2* pList, INT ListCount);


////////////////////////////////////////////////////////////////////////////////
// Vector3

FLOAT LCXVec3Dot(const LCXVECTOR3* v1, const LCXVECTOR3* v2);					// dot3 product
FLOAT LCXVec3Length(const LCXVECTOR3* v);										// vector3 Length
FLOAT LCXVec3LengthSq(const LCXVECTOR3* v);										// vector3 Square Length
FLOAT LCXVec3Normalize(LCXVECTOR3* Out, const LCXVECTOR3* In);					// normalize return length
void  LCXVec3Cross(LCXVECTOR3* Out, const LCXVECTOR3* v);						// Out = cross(Out, v);
void  LCXVec3Cross(LCXVECTOR3* Out, const LCXVECTOR3* v1, const LCXVECTOR3* v2);// Out = cross(v1, v2);
void  LCXVec3Lerp(LCXVECTOR3* Out
				  , const LCXVECTOR3* v1, const LCXVECTOR3* v2, FLOAT t);		// Out = (1-t)*v1 + t* v2

void LCXVec3MakeBoundingBox(LCXVECTOR3* vcMin, LCXVECTOR3* vcMax				// make out min, max of the bounding box from the List
							, const LCXVECTOR3* pList, INT ListCount);


////////////////////////////////////////////////////////////////////////////////
// Vector4

FLOAT LCXVec4Dot(const LCXVECTOR4* v1, const LCXVECTOR4* v2);					// dot4 product
FLOAT LCXVec4Length(const LCXVECTOR4* v);										// vector4 Length
FLOAT LCXVec4LengthSq(const LCXVECTOR4* v);										// vector4 Square Length
FLOAT LCXVec4Normalize(LCXVECTOR4* Out, const LCXVECTOR4* In);					// normalize return length
void  LCXVec4Lerp(LCXVECTOR4* Out
				  , const LCXVECTOR4* v1, const LCXVECTOR4* v2, FLOAT t);		// Out = (1-t)*v1 + t* v2


////////////////////////////////////////////////////////////////////////////////
// Transform of Vector3

// w = 1
void  LCXVec3TransformCoord(LCXVECTOR3* Out, const LCXVECTOR3* In, const LCXMATRIX* tm);		// transform of vector by 4x4 matrix
void  LCXVec3TransformCoord(LCXVECTOR3* Out, const LCXVECTOR3* In, const LCXMATRIX4X3* tm);		// transform of vector by 4x3 matrix

// w = 0
void  LCXVec3TransformNormal(LCXVECTOR3* Out, const LCXVECTOR3* In, const LCXMATRIX* tm);		// transform of vector by 4x4 matrix
void  LCXVec3TransformNormal(LCXVECTOR3* Out, const LCXVECTOR3* In, const LCXMATRIX4X3* tm);	// transform of vector by 4x3 matrix



////////////////////////////////////////////////////////////////////////////////
// Transform of Vector2

void  LCXVec2TransformCoord(LCXVECTOR2* Out, const LCXVECTOR2* In, const LCXMATRIX3X3* tm);		// transform of vector by 3x3 matrix. z = 1
void  LCXVec3TransformNormal(LCXVECTOR2* Out, const LCXVECTOR2* In, const LCXMATRIX3X3* tm);	// transform of vector by 3x3 matrix. z = 0



////////////////////////////////////////////////////////////////////////////////
// Plane

FLOAT LCXPlaneDot(const LCXPLANE* Out, const LCXVECTOR4* v);									// same to a*x + b*y + c*z + d*w
FLOAT LCXPlaneDotCoord(const LCXPLANE* Out, const LCXVECTOR3* v);								// same to a*x + b*y + c*z + d*1
FLOAT LCXPlane2DotCoord(const LCXPLANE2* Out, const LCXVECTOR2* v);								// same to a*x + b*y + d*1
void  LCXPlaneFromPointNormal(LCXPLANE* Out, const LCXVECTOR3* p, const LCXVECTOR3* normal);	// constructs a plane by normal and point in plane
void  LCXPlaneFromPoints(LCXPLANE* Out,const LCXVECTOR3*,const LCXVECTOR3*,const LCXVECTOR3*);	// constructs a plane by 3 points
void  LCXPlaneTransform(LCXPLANE* Out, const LCXPLANE* In, const LCXMATRIX* tm);				// It's similar to the D3DXPlaneTransform which needs inverse transpose of tm. but it does not need inverse transpose.



////////////////////////////////////////////////////////////////////////////////
// Quaternion and Matrix.

void  LCXMatrixIdentity(LCXMATRIX* m);
void  LCXMatrixAdd(LCXMATRIX* m, const LCXMATRIX* a, const LCXMATRIX* b);		// m= a+b
void  LCXMatrixSub(LCXMATRIX* m, const LCXMATRIX* a, const LCXMATRIX* b);		// m= a-b
void  LCXMatrixMul(LCXMATRIX* m, const LCXMATRIX* a, const LCXMATRIX* b);		// m = a*b

FLOAT LCXMatrixInverse(LCXMATRIX* m, INT bQuick=LC_TRUE);						// if the 'bQuick' is not false then this function will be calculating the inverse matrix  except _14=0, _24=0, _34=0, _44=1
FLOAT LCXMatrixDeterminant(const LCXMATRIX* m);
void  LCXMatrixScaling(LCXMATRIX* m, const LCXVECTOR3* scl);
void  LCXMatrixLerp(LCXMATRIX* m, const LCXMATRIX* m1							// m = (1-t)*m1 + t* m2
								, const LCXMATRIX* m2, FLOAT t);


void  LCXQuaternionSLerp(LCXQUATERNION* Out										// qOut = (1/sin T)[ sin ( T *(1-t) ) * q1 + sin ( T * t ) * q2]
						 , const LCXQUATERNION* q1
						 , const LCXQUATERNION* q2
						 , FLOAT t);

void  LCXMatrixFromQuat(LCXMATRIX* Out, const LCXQUATERNION* q, INT bLHR = LC_TRUE);			// LHR = Left Hand Rule. ex)Direct3D
void  LCXMatrix4X4From4X3(LCXMATRIX* Out, const LCXMATRIX4X3* tm);
void  LCXMatrix4X4From3X3Tm(LCXMATRIX* Out, const LCXMATRIX3X3* tm);							// It's same to LCXMATRIX::SetupFrom3X3Tm
void  LCXMatrix4X3From4X4(LCXMATRIX4X3* Out, const LCXMATRIX* tm);




////////////////////////////////////////////////////////////////////////////////
// Color
// bit order Blue, Green, Red, Alpha(ARGB: DirectX System)

LC_INLINE COLOR8  LCXColAlphaFrom32ARGB(COLOR32 c)	{	return COLOR8( (c>>24) & 0xFF);	}
LC_INLINE COLOR8  LCXColRedFrom32ARGB  (COLOR32 c)	{	return COLOR8( (c>>16) & 0xFF);	}
LC_INLINE COLOR8  LCXColGreenFrom32ARGB(COLOR32 c)	{	return COLOR8( (c>> 8) & 0xFF);	}
LC_INLINE COLOR8  LCXColBlueFrom32ARGB (COLOR32 c)	{	return COLOR8( (c>> 0) & 0xFF);	}

LC_INLINE COLOR8  LCXColAlphaFrom24ARGB(COLOR32 c)	{	return COLOR8( (c>>24) & 0xFF);	}
LC_INLINE COLOR8  LCXColRedFrom24ARGB  (COLOR32 c)	{	return COLOR8( (c>>16) & 0xFF);	}
LC_INLINE COLOR8  LCXColGreenFrom24ARGB(COLOR32 c)	{	return COLOR8( (c>>8 ) & 0xFF);	}
LC_INLINE COLOR8  LCXColBlueFrom24ARGB (COLOR32 c)	{	return COLOR8( (c    ) & 0xFF);	}


LC_INLINE void    LCXColAlphaTo32ARGB  (COLOR32& c, COLOR32 a){ c = (a<<24) | (c & 0x00FFFFFF); }
LC_INLINE void    LCXColRedTo32ARGB    (COLOR32& c, COLOR32 r){ c = (r<<16) | (c & 0xFF00FFFF); }
LC_INLINE void    LCXColGreenTo32ARGB  (COLOR32& c, COLOR32 g){ c = (g<< 8) | (c & 0xFFFF00FF); }
LC_INLINE void    LCXColBlueTo32ARGB   (COLOR32& c, COLOR32 b){ c = (b<< 0) | (c & 0xFFFFFF00); }

LC_INLINE COLOR16 LCXColR5G6B5     (INT r,INT g,INT b)      { return (r<<8 & 0xF800) | (g<<3 & 0x7E0) | (b>>3 & 0x1F);					}
LC_INLINE COLOR16 LCXColX1R5G5B5   (INT r,INT g,INT b)      { return (r>>7 & 0x7C00) | (g<<2 & 0x3E0) | (b>>3 & 0x1F) | (0x8000);		}
LC_INLINE COLOR16 LCXColA1R5G5B5   (INT r,INT g,INT b,INT a){ return (r>>7 & 0x7C00) | (g<<2 & 0x3E0) | (b>>3 & 0x1F) | (a<<8 & 0x8000);}

LC_INLINE COLOR16 LCXColR5G6B5FromARGB  (COLOR32 c){ return (COLOR16)( (c>>8 & 0xF800) | (c>>5 & 0x7E0) | (c>>3 & 0x1F)                    ); }
LC_INLINE COLOR16 LCXColX1R5G5B5FromARGB(COLOR32 c){ return (COLOR16)( (c>>9 & 0x7C00) | (c<<6 & 0x3E0) | (c>>3 & 0x1F) | (0x8000)         ); }
LC_INLINE COLOR16 LCXColA1R5G5B5FromARGB(COLOR32 c){ return (COLOR16)( (c>>9 & 0x7C00) | (c<<6 & 0x3E0) | (c>>3 & 0x1F) | (c>>16 & 0x8000) ); }

// Returns value from A1R5G5B5 color
LC_INLINE COLOR8  LCXColAlphaFrom1555(COLOR16 c){ return (COLOR8 )(c>>15 & 0x01); }
LC_INLINE COLOR8  LCXColRedFrom1555  (COLOR16 c){ return (COLOR8 )(c>>10 & 0x1F); }
LC_INLINE COLOR8  LCXColGreenFrom1555(COLOR16 c){ return (COLOR8 )(c>> 5 & 0x1F); }
LC_INLINE COLOR8  LCXColBlueFrom1555 (COLOR16 c){ return (COLOR8 )(c>> 0 & 0x1F); }

LC_INLINE COLOR8  LCXColRedFrom565   (COLOR16 c){ return (COLOR8 )(c>>11 & 0x1F); }
LC_INLINE COLOR8  LCXColGreenFrom565 (COLOR16 c){ return (COLOR8 )(c>> 5 & 0x3F); }
LC_INLINE COLOR8  LCXColBlueFrom565  (COLOR16 c){ return (COLOR8 )(c>> 0 & 0x1F); }


LC_INLINE COLOR16 LCXColLuminanceFrom1555(COLOR16 c){ return  ( (c>>15 & 0x01) + (c>>10 & 0x1F) + (c>> 5 & 0x1F) + (c>> 0 & 0x1F) ) << 3 / 3; }

// 16 -> 32 ARGB
LC_INLINE COLOR32 LCXColA8R8G8B8From1555(COLOR16 c){	return (c&0x8000)<<16 | (c&0x7C00)<<9 | (c&0x3E0)<<6 | (c&0x1F)<<3;	}
LC_INLINE COLOR32 LCXColR8G8B8From565 (COLOR16 c){	return	   0xFF000000 | (c&0xF800)<<8 | (c&0x7E0)<<5 | (c&0x1F)<<3;	}



// bit order Red, Green, Blue, Alpha (ABGR or RGBA: OpenGL System)
LC_INLINE COLOR8  LCXColAlphaFrom32RGBA(COLOR32 c)	{	return COLOR8( (c>>24) & 0xFF);	}
LC_INLINE COLOR8  LCXColRedFrom32RGBA  (COLOR32 c)	{	return COLOR8( (c>> 0) & 0xFF);	}
LC_INLINE COLOR8  LCXColGreenFrom32RGBA(COLOR32 c)	{	return COLOR8( (c>> 8) & 0xFF);	}
LC_INLINE COLOR8  LCXColBlueFrom32RGBA (COLOR32 c)	{	return COLOR8( (c>>16) & 0xFF);	}

LC_INLINE void    LCXColAlphaTo32RGBA  (COLOR32& c, COLOR32 a){ c = (a<<24) | (c & 0x00FFFFFF); }
LC_INLINE void    LCXColRedTo32RGBA    (COLOR32& c, COLOR32 r){ c = (r<< 0) | (c & 0xFFFFFF00); }
LC_INLINE void    LCXColGreenTo32RGBA  (COLOR32& c, COLOR32 g){ c = (g<< 8) | (c & 0xFFFF00FF); }
LC_INLINE void    LCXColBlueTo32RGBA   (COLOR32& c, COLOR32 b){ c = (b<<16) | (c & 0xFF00FFFF); }

LC_INLINE COLOR16 LCXColR5G5B5X1   (INT r,INT g,INT b)      { return (r<<8 & 0xF800) | (g<<3 & 0x7C0) | (b>>2 & 0x3E) | (0x1);  }
LC_INLINE COLOR16 LCXColR5G5B5A1   (INT r,INT g,INT b,INT a){ return (r<<8 & 0xF800) | (g<<3 & 0x7C0) | (b>>2 & 0x3E) | (a>>7 & 0x1); }

LC_INLINE COLOR16 LCXColR5G6B5FromABGR  (COLOR32 c){ return (COLOR16)( (c<<8 & 0xF800) | (c>>5 & 0x7E0) | (c>>19& 0x1F)                 ); }
LC_INLINE COLOR16 LCXColR5G5B5X1FromABGR(COLOR32 c){ return (COLOR16)( (c<<8 & 0xF800) | (c>>5 & 0x7C0) | (c>>18& 0x1F) | (0x8000)      ); }
LC_INLINE COLOR16 LCXColR5G5B5A1FromABGR(COLOR32 c){ return (COLOR16)( (c<<8 & 0xF800) | (c>>5 & 0x7C0) | (c>>18& 0x1F) | (c>>31 & 0x1) ); }

// Returns value from R5G5B5A1 color
LC_INLINE COLOR8  LCXColAlphaFrom5551(COLOR16 c){ return (COLOR8 )(c>> 0 & 0x01); }
LC_INLINE COLOR8  LCXColRedFrom5551  (COLOR16 c){ return (COLOR8 )(c>>11 & 0x1F); }
LC_INLINE COLOR8  LCXColGreenFrom5551(COLOR16 c){ return (COLOR8 )(c>> 6 & 0x1F); }
LC_INLINE COLOR8  LCXColBlueFrom5551 (COLOR16 c){ return (COLOR8 )(c>> 1 & 0x1F); }

LC_INLINE COLOR16 LCXColLuminanceFrom5551(COLOR16 c){ return  ( (c>>0 & 0x01) + (c>>11 & 0x1F) + (c>>6 & 0x1F) + (c>>1 & 0x1F) ) << 3 / 3; }

// 16 -> 32 RGBA
LC_INLINE COLOR32 LCXColA8B8G8R8From5551(COLOR16 c){	return (c&0x1)<<31| (c&0x3E)<<18 | (c&0x7C0)<<5 | (c&0xF800)>>8; }
LC_INLINE COLOR32 LCXColB8G8R8From565 (COLOR16 c){	return 0xFF000000 | (c&0x1F)<<19 | (c&0x7E0)<<5 | (c&0xF800)>>8; }






////////////////////////////////////////////////////////////////////////////////
// Euclid Geomety Method

struct LCXELLIPSOID
{
	FLOAT		a;		// radius along the x
	FLOAT		b;		// radius along the y
	LCXVECTOR2	c;		// center

	INT Collision(const LCXVECTOR2* p);		// return LC_EFAIL when it is not collison
};


INT LCX_CollisionEllipseToPoint(LCXELLIPSOID* e, const LCXVECTOR2* p);



template<class T>
INT LCXLine2Line(const T& l0, const T& l1, const T& s0, const T& s1)
{
	return LC_OK;
}


template<class T>
INT LCXCollisionCircle2Circle(const T& c1, const T& c2, FLOAT r1, FLOAT r2)	// center 1, center2, radius 1, radius 2
{
	if( (c1.x - c2.x) * (c1.x - c2.x) +
		(c1.y - c2.y) * (c1.y - c2.y) - (r1 + r2) * (r1 + r2)  > 0
		)
		return LC_EFAIL;

	return LC_OK;
}



////////////////////////////////////////////////////////////////////////////////
// ETC Method

// transform for 3D
class LCXTF
{
protected:
	LCXMATRIX		m;						// Output World Matrix
	LCXVECTOR4		r;						// Rotation axis(x,y,z), and angle(radian) z
	LCXVECTOR3		s;						// Scaling
	//LCXVECTOR3	t;						// Translation

public:
	LCXTF();

	const LCXMATRIX * const GetTM() ;		// get TM Matrix
	const LCXVECTOR3* const GetPos();
	const LCXVECTOR4* const GetRot();
	const LCXVECTOR3* const GetScl();

	void SetPos(const LCXVECTOR3*);			// position: float3
	void SetRot(const LCXVECTOR4*);			// rotation: float4 rotation axis(x,y,z) + rotation angle(w: radian)
	void SetScl(const LCXVECTOR3*);			// scaling : float3
};


// transform for 2D
class LCXTF2D
{
protected:
	LCXMATRIX		m;						// Output World Matrix
	LCXVECTOR3		r;						// Rotation
	LCXVECTOR2		s;						// Scaling
	//LCXVECTOR3	t;						// Translation

public:
	LCXTF2D();

	const LCXMATRIX * const GetTM() ;		// get TM Matrix
	const LCXVECTOR2* const GetPos();
	      FLOAT             GetZ  ();
	const LCXVECTOR3* const GetRot();
	const LCXVECTOR2* const GetScl();

	void SetPos(const LCXVECTOR2*);			// position: float2
	void SetZ  (const FLOAT      );			// setup the z value
	void SetRot(const LCXVECTOR3*);			// rotation: float3 rotation center(x,y) + rotation angle(z: radian)
	void SetScl(const LCXVECTOR2*);			// scaling : float2
};



// restrict region
template<class T>				// Boundary in [v1, v2]
T LCXClamp(T v0, T v1, T v2)
{
	if     (v0 < v1) return v1;
	else if(v0 > v2) return v2;
	return v0;
}



#endif	//_LC_Math_H_

