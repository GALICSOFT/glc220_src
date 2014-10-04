//  For Mathematics for Euclid Field.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _LcEuclid_H_
#define _LcEuclid_H_

#if _MSC_VER >= 1200
	#pragma warning(disable:4201)		// anonymous unions warning
#endif

#pragma warning(disable:4786)
#include <vector>
using namespace std;

#include <math.h>



#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#define LCX_PI		3.1415926535897932384626433832795f
#define PI_RAD		3.1415926535897932f

#define ONE_RADtoDEG	57.295779513082321f
#define ONE_DEGtoRAD	0.0174532925199433f
#define DEG90toRAD		1.5707963267948966f

#define LCXToRadian(deg) ((deg) * (LCX_PI / 180.0f))
#define LCXToDegree(rad) ((rad) * (180.0f / LCX_PI))
#define RADtoDEG(p)	( (p)*ONE_RADtoDEG)
#define DEGtoRAD(p)	( (p)*ONE_DEGtoRAD)

#define LC_ROUNDING_DELTA	0.0001f


typedef int						INT;
typedef int						BOOL;
typedef float					FLOAT;
typedef unsigned long			DWORD;

typedef D3DXVECTOR2				VEC2;
typedef	D3DXVECTOR3				VEC3;
typedef D3DXVECTOR4				VEC4;
typedef D3DXMATRIX				MATA;
typedef D3DXQUATERNION			QUAT;
typedef D3DXPLANE				DPLN;


struct INT2
{
	union { struct { INT x, y;	};	INT m[2];	};
	INT2();
	INT2(INT X,INT Y);
	operator INT* ();
	operator const INT* () const;
};


struct VEC2i : public INT2
{
	VEC2i();
	VEC2i(INT _x,INT _y);
	VEC2i(const VEC2i& rhs );
	VEC2i(const INT *_m );
	VEC2i(D3DXVECTOR2 p);

	operator INT*();
	operator const INT*() const;
	
	bool operator==(const VEC2i& v) const;
	bool operator!=(const VEC2i& v) const;

	const VEC2i& operator=(const VEC2i& rhs);

	// assignment operators
	VEC2i& operator+=(const VEC2i& rhs);
	VEC2i& operator-=(const VEC2i& rhs);
	VEC2i& operator*=(INT f );
	VEC2i& operator/=(INT f );

	// unary operators
	VEC2i operator+() const;
	VEC2i operator-() const;

	// binary operators
	VEC2i operator+(const VEC2i& rhs) const;
	VEC2i operator-(const VEC2i& rhs) const;
	VEC2i operator*(const INT& f) const;
	VEC2i operator/(const INT& f) const;

	friend VEC2i operator*(INT f,const VEC2i& v){	return VEC2i(f * v.x, f * v.y);		}
};


typedef	vector<VEC2i >		lsVEC2i;
typedef	lsVEC2i::iterator	itVEC2i;

struct INT3
{
	union { struct{	INT x, y, z; };	INT m[3];};

	INT3();
	INT3(INT X,INT Y,INT Z);
	operator INT* ();
	operator const INT* () const;
};

struct VEC3i : public INT3
{
	VEC3i();
	VEC3i(const INT *_m);
	VEC3i(const VEC3i& rhs);
	VEC3i(INT _x,INT _y, INT _z);
	VEC3i(const D3DXVECTOR3 p);
	VEC3i(FLOAT _x, FLOAT _y, FLOAT _z);
	
	operator INT*();
	operator const INT*() const;

	bool operator==(const VEC3i& v) const;
	bool operator!=(const VEC3i& v) const;

	const VEC3i& operator=(const VEC3i& rhs);
	
	VEC3i& operator+=(const VEC3i& v);
	VEC3i& operator-=(const VEC3i& v);
	VEC3i& operator*=(INT f );
	VEC3i& operator/=(INT f );
	
	VEC3i operator+() const;
	VEC3i operator-() const;
	
	VEC3i operator+(const VEC3i& v)const;
	VEC3i operator-(const VEC3i& v)const;
	VEC3i operator*(INT f) const		;
	VEC3i operator/(INT f) const		;
	
	friend VEC3i operator*(INT f,const VEC3i& v)
	{	return VEC3i(f * v.x, f * v.y, f * v.z);	}
};


typedef	vector<VEC3i >		lsVEC3i;
typedef	lsVEC3i::iterator	itVEC3i;


struct INT4
{
	union { struct { INT x, y, z, w; };	INT m[4];	};

	INT4();
	INT4(INT X,INT Y,INT Z,INT W);
	operator INT* ();
	operator const INT* () const;
};


struct VEC4i : public INT4
{
	VEC4i();
	VEC4i(const INT *_m );
	VEC4i(const VEC4i& r);
	VEC4i(INT X,INT Y,INT Z,INT W);
	VEC4i(const D3DXVECTOR4 p);
	
	operator INT*();
	operator const INT*() const;

	bool operator==(const VEC4i& v) const;
	bool operator!=(const VEC4i& v) const;

	const VEC4i& operator=(const VEC4i& rhs);
	
	// assignment operators
	VEC4i& operator+=(const VEC4i& v);
	VEC4i& operator-=(const VEC4i& v);
	VEC4i& operator*=(INT f )		;
	VEC4i& operator/=(INT f )		;
	
	// unary operators
	VEC4i operator+() const			;
	VEC4i operator-() const			;
	
	// binary operators
	VEC4i operator+(const VEC4i& v)const;
	VEC4i operator-(const VEC4i& v)const;
	VEC4i operator*(INT f) const		;
	VEC4i operator/(INT f) const		;
	
	friend VEC4i operator*(INT f,const VEC4i& v)
	{	return VEC4i(f*v.x,f*v.y,f*v.z,f*v.w);	}
};


typedef	vector<VEC4i >		lsVEC4i;
typedef	lsVEC4i::iterator	itVEC4i;



struct WORD2
{
	union{	struct{WORD x, y;	};	WORD m[2];	};

	WORD2();
	WORD2(WORD X,WORD Y);
	operator WORD* ();
	operator const WORD* () const;
};

struct VEC2w : public WORD2
{
	VEC2w();
	VEC2w(WORD X,WORD Y);

};

typedef	vector<VEC2w >		lsVEC2w;
typedef	lsVEC2w::iterator	itVEC2w;


struct WORD3
{
	union{	struct{	WORD x, y, z;	};	WORD m[3];};

	WORD3();
	WORD3(WORD X,WORD Y,WORD Z);
	operator WORD* ()			;
	operator const WORD* () const;
};

struct VEC3w : public WORD3
{
	VEC3w();
	VEC3w(WORD X,WORD Y, WORD Z, WORD W);

};

typedef	vector<VEC3w >		lsVEC3w;
typedef	lsVEC3w::iterator	itVEC3w;



struct WORD4
{
	union{	struct{	WORD x,y,z,w;	};	WORD m[4];	};

	WORD4();
	WORD4(WORD X,WORD Y,WORD Z,WORD W);
	operator WORD* ();
	operator const WORD* () const;
};

struct VEC4w : public WORD4
{
	VEC4w();
	VEC4w(WORD X,WORD Y, WORD Z, WORD W);

};

typedef	vector<VEC4w >		lsVEC4w;
typedef	lsVEC4w::iterator	itVEC4w;



struct DOUBLE2
{
	union { struct { DOUBLE x, y; };	DOUBLE m[2];	};
	
	DOUBLE2();
	DOUBLE2(DOUBLE X,DOUBLE Y);
};


struct DOUBLE3
{
	union { struct { DOUBLE x, y, z; };	DOUBLE m[3];	};

	DOUBLE3();
	DOUBLE3(DOUBLE X,DOUBLE Y,DOUBLE Z,DOUBLE W);
};


struct DOUBLE4
{
	union { struct { DOUBLE x, y, z, w;	};	DOUBLE m[4];	};

	DOUBLE4();
	DOUBLE4(DOUBLE X,DOUBLE Y,DOUBLE Z,DOUBLE W);
};




struct LCXVECTOR2
{
	union {	struct { FLOAT x; FLOAT y; };	FLOAT m[2];	};

	LCXVECTOR2();
	LCXVECTOR2(const FLOAT* v);
	LCXVECTOR2(const LCXVECTOR2& v);
	LCXVECTOR2(const LCXVECTOR2* v);
	LCXVECTOR2(FLOAT X, FLOAT Y);

	operator FLOAT*();
	operator const FLOAT*() const;

	FLOAT& operator[](int n);

	// assignment operators
	LCXVECTOR2& operator +=(const LCXVECTOR2& v);
	LCXVECTOR2& operator -=(const LCXVECTOR2& v);
	LCXVECTOR2& operator *=(FLOAT v);
	LCXVECTOR2& operator /=(FLOAT v);

	// unary operators
	LCXVECTOR2 operator +() const;
	LCXVECTOR2 operator -() const;

	// binary operators
	LCXVECTOR2 operator +(const LCXVECTOR2& v) const;
	LCXVECTOR2 operator -(const LCXVECTOR2& v) const;
	LCXVECTOR2 operator *(FLOAT v) const;
	LCXVECTOR2 operator /(FLOAT v) const;

	friend LCXVECTOR2 operator *(FLOAT f, const LCXVECTOR2& v);
	friend LCXVECTOR2 operator /(FLOAT f, const LCXVECTOR2& v);

	BOOL operator ==(const LCXVECTOR2& v) const;
	BOOL operator !=(const LCXVECTOR2& v) const;

	FLOAT operator *(const LCXVECTOR2& v);										// Dot Product
	friend FLOAT operator *(const LCXVECTOR2& v1, const LCXVECTOR2& v2);

	FLOAT operator ^(const LCXVECTOR2& v);										// Cross Product(Z-Value)
	friend FLOAT operator ^(const LCXVECTOR2& v1, const LCXVECTOR2& v2);

	LCXVECTOR2 operator *(const FLOAT* v);										// Transform: vector * Matrix
	friend LCXVECTOR2 operator *(const FLOAT* v1, const LCXVECTOR2& v2);			// Transform: Matrix * vector;

	FLOAT		Length();														// Length
	FLOAT		LengthSq();														// Length Square
	LCXVECTOR2	Normalize();
	LCXVECTOR2	Normalize(const LCXVECTOR2* v);
};


struct LCXVECTOR3
{
	union {	struct { FLOAT x; FLOAT y; FLOAT z; };	FLOAT m[3];	};

	LCXVECTOR3();
	LCXVECTOR3(const FLOAT* v);
	LCXVECTOR3(const LCXVECTOR3& v);
	LCXVECTOR3(const LCXVECTOR3* v);
	LCXVECTOR3(FLOAT X, FLOAT Y, FLOAT Z);

	operator FLOAT*();
	operator const FLOAT*() const;

	FLOAT& operator[](int n);

	// assignment operators
	LCXVECTOR3& operator +=(const LCXVECTOR3& v);
	LCXVECTOR3& operator -=(const LCXVECTOR3& v);
	LCXVECTOR3& operator *=(FLOAT v);
	LCXVECTOR3& operator /=(FLOAT v);

	// unary operators
	LCXVECTOR3 operator +() const;
	LCXVECTOR3 operator -() const;

	// binary operators
	LCXVECTOR3 operator +(const LCXVECTOR3& v) const;
	LCXVECTOR3 operator -(const LCXVECTOR3& v) const;
	LCXVECTOR3 operator *(FLOAT v) const;
	LCXVECTOR3 operator /(FLOAT v) const;

	friend LCXVECTOR3 operator *(FLOAT f, const LCXVECTOR3& v);
	friend LCXVECTOR3 operator /(FLOAT f, const LCXVECTOR3& v);

	BOOL operator ==(const LCXVECTOR3& v) const;
	BOOL operator !=(const LCXVECTOR3& v) const;

	FLOAT operator *(const LCXVECTOR3& v);										// Dot Product
	friend FLOAT operator *(const LCXVECTOR3& v1, const LCXVECTOR3& v2);

	LCXVECTOR3 operator ^(const LCXVECTOR3& v);									// Cross Product
	friend LCXVECTOR3 operator ^(const LCXVECTOR3& v1, const LCXVECTOR3& v2);

	LCXVECTOR3 operator *(const FLOAT* v);										// Transform: vector * Matrix
	friend LCXVECTOR3 operator *(const FLOAT* v1, const LCXVECTOR3& v2);		// Transform: Matrix * vector;

	FLOAT		Length();														// Length
	FLOAT		LengthSq();														// Length Square
	LCXVECTOR3	Normalize();
	LCXVECTOR3	Normalize(const LCXVECTOR3* v);

	LCXVECTOR3	Cross(const LCXVECTOR3* v1, const LCXVECTOR3* v2);
};


struct LCXVECTOR4
{
	union {	struct { FLOAT x; FLOAT y; FLOAT z; FLOAT w; };	FLOAT m[4];	};

	LCXVECTOR4();
	LCXVECTOR4(const FLOAT* v);
	LCXVECTOR4(const LCXVECTOR4& v);
	LCXVECTOR4(const LCXVECTOR4* v);
	LCXVECTOR4(FLOAT X, FLOAT Y, FLOAT Z, FLOAT W);

	operator FLOAT*();
	operator const FLOAT*() const;

	FLOAT& operator[](int n);

	// assignment operators
	LCXVECTOR4& operator +=(const LCXVECTOR4& v);
	LCXVECTOR4& operator -=(const LCXVECTOR4& v);
	LCXVECTOR4& operator *=(FLOAT v);
	LCXVECTOR4& operator /=(FLOAT v);

	// unary operators
	LCXVECTOR4 operator +() const;
	LCXVECTOR4 operator -() const;

	// binary operators
	LCXVECTOR4 operator +(const LCXVECTOR4& v) const;
	LCXVECTOR4 operator -(const LCXVECTOR4& v) const;
	LCXVECTOR4 operator *(FLOAT v) const;
	LCXVECTOR4 operator /(FLOAT v) const;

	friend LCXVECTOR4 operator *(FLOAT f, const LCXVECTOR4& v);
	friend LCXVECTOR4 operator /(FLOAT f, const LCXVECTOR4& v);

	BOOL operator ==(const LCXVECTOR4& v) const;
	BOOL operator !=(const LCXVECTOR4& v) const;

	FLOAT operator *(const LCXVECTOR4& v);										// Dot Product
	friend FLOAT operator *(const LCXVECTOR4& v1, const LCXVECTOR4& v2);

	LCXVECTOR4 operator *(const FLOAT* v);										// Transform: vector * Matrix
	friend LCXVECTOR4 operator *(const FLOAT* v1, const LCXVECTOR4& v2);		// Transform: Matrix * vector;

	FLOAT		Length();														// Length
	FLOAT		LengthSq();														// Length Square
	LCXVECTOR4	Normalize();
	LCXVECTOR4	Normalize(const LCXVECTOR4* v);
};


struct LCXMATRIX
{
	union
	{
		struct
		{
			FLOAT _11, _12, _13, _14;
			FLOAT _21, _22, _23, _24;
			FLOAT _31, _32, _33, _34;
			FLOAT _41, _42, _43, _44;

		};
		FLOAT m[4][4];
	};

	LCXMATRIX();
	LCXMATRIX(const FLOAT* v);
	LCXMATRIX(const LCXMATRIX& v);
	LCXMATRIX(	FLOAT v11, FLOAT v12, FLOAT v13, FLOAT v14,
		FLOAT v21, FLOAT v22, FLOAT v23, FLOAT v24,
		FLOAT v31, FLOAT v32, FLOAT v33, FLOAT v34,
		FLOAT v41, FLOAT v42, FLOAT v43, FLOAT v44 );


	// access grants
	FLOAT& operator () ( int iRow, int iCol );
	FLOAT  operator () ( int iRow, int iCol ) const;

	// casting operators
	operator FLOAT* ();
	operator const FLOAT* () const;

	// assignment operators
	LCXMATRIX& operator *= (const LCXMATRIX& v);
	LCXMATRIX& operator += (const LCXMATRIX& v);
	LCXMATRIX& operator -= (const LCXMATRIX& v);
	LCXMATRIX& operator *= ( FLOAT v);
	LCXMATRIX& operator /= ( FLOAT v);

	LCXMATRIX operator +() const;												// unary operators
	LCXMATRIX operator -() const;

	LCXMATRIX operator * (const LCXMATRIX& v) const;
	LCXMATRIX operator + (const LCXMATRIX& v) const;
	LCXMATRIX operator - (const LCXMATRIX& v) const;
	LCXMATRIX operator * ( FLOAT v) const;
	LCXMATRIX operator / ( FLOAT v) const;


	friend LCXMATRIX operator * (FLOAT f, const LCXMATRIX& v);
	friend LCXMATRIX operator * (const LCXMATRIX& v, FLOAT f);

	BOOL operator == (const LCXMATRIX& v) const;
	BOOL operator != (const LCXMATRIX& v) const;

	LCXMATRIX&	Identity();
	LCXMATRIX&	Transpose();
	LCXMATRIX&	Transpose(const LCXMATRIX* v);
	LCXMATRIX&	Inverse(INT nQuick=TRUE);
	LCXMATRIX&	Inverse(const LCXMATRIX* v, INT nQuick=TRUE);					// _14, _24, _34, _44 is not zero
	FLOAT		Determinant();

	LCXMATRIX&	SetupScaling(FLOAT X, FLOAT Y, FLOAT Z);
	LCXMATRIX&	SetupViewLH(const LCXVECTOR3* vEye, const LCXVECTOR3* vLook, const LCXVECTOR3* vUp );
	LCXMATRIX&	SetupPerspectiveLH(FLOAT fFOV, FLOAT fAspect, FLOAT fNear, FLOAT fFar);
	LCXMATRIX&	SetupRotationX(FLOAT fRad);
	LCXMATRIX&	SetupRotationY(FLOAT fRad);
	LCXMATRIX&	SetupRotationZ(FLOAT fRad);
	LCXMATRIX&	SetupRotationAxis(const LCXVECTOR3* vAxis, FLOAT fRad);
	LCXMATRIX&	SetupTranslation(const LCXVECTOR3* vTrs, BOOL bInit=TRUE);

	void		TransformCoord(LCXVECTOR3* pOut, const LCXVECTOR3* pIn);
};


struct LCXQUATERNION
{
	union {	struct { FLOAT x; FLOAT y; FLOAT z;	 FLOAT w; }; FLOAT m[4]; };

	LCXQUATERNION();
	LCXQUATERNION( const FLOAT * );
	LCXQUATERNION( const LCXQUATERNION& );
	LCXQUATERNION( FLOAT x, FLOAT y, FLOAT z, FLOAT w );

	// casting
	operator FLOAT* ();
	operator const FLOAT* () const;

	// assignment operators
	LCXQUATERNION& operator += ( const LCXQUATERNION& );
	LCXQUATERNION& operator -= ( const LCXQUATERNION& );
	LCXQUATERNION& operator *= ( const LCXQUATERNION& );
	LCXQUATERNION& operator *= ( FLOAT );
	LCXQUATERNION& operator /= ( FLOAT );

	// unary operators
	LCXQUATERNION  operator + () const;
	LCXQUATERNION  operator - () const;

	// binary operators
	LCXQUATERNION operator + ( const LCXQUATERNION& ) const;
	LCXQUATERNION operator - ( const LCXQUATERNION& ) const;
	LCXQUATERNION operator * ( const LCXQUATERNION& ) const;
	LCXQUATERNION operator * ( FLOAT ) const;
	LCXQUATERNION operator / ( FLOAT ) const;

	friend LCXQUATERNION operator * (FLOAT, const LCXQUATERNION& );

	BOOL operator == ( const LCXQUATERNION& ) const;
	BOOL operator != ( const LCXQUATERNION& ) const;

	void SLerp(const LCXQUATERNION* q1, const LCXQUATERNION* q2, FLOAT t);
	void RotationMatrix(LCXMATRIX* pOut, BOOL bDX=TRUE);
};


// Colors
struct LCXCOLOR
{
	union {	struct { FLOAT r; FLOAT g; FLOAT b;	 FLOAT a; }; FLOAT m[4]; };

	LCXCOLOR();
	LCXCOLOR( DWORD argb );
	LCXCOLOR( const FLOAT * );
	LCXCOLOR( const LCXCOLOR& );
	LCXCOLOR( FLOAT r, FLOAT g, FLOAT b, FLOAT a );

	// casting
	operator DWORD () const;

	operator FLOAT* ();
	operator const FLOAT* () const;

	// assignment operators
	LCXCOLOR& operator += ( const LCXCOLOR& );
	LCXCOLOR& operator -= ( const LCXCOLOR& );
	LCXCOLOR& operator *= ( FLOAT );
	LCXCOLOR& operator /= ( FLOAT );

	// unary operators
	LCXCOLOR operator + () const;
	LCXCOLOR operator - () const;

	// binary operators
	LCXCOLOR operator + ( const LCXCOLOR& ) const;
	LCXCOLOR operator - ( const LCXCOLOR& ) const;
	LCXCOLOR operator * ( FLOAT ) const;
	LCXCOLOR operator / ( FLOAT ) const;

	friend LCXCOLOR operator * (FLOAT, const LCXCOLOR& );

	BOOL operator == ( const LCXCOLOR& ) const;
	BOOL operator != ( const LCXCOLOR& ) const;
};


// Planes
struct LCXPLANE
{
	union {	struct { FLOAT a; FLOAT b; FLOAT c;	 FLOAT d; }; FLOAT m[4]; };

	LCXPLANE();
	LCXPLANE( const FLOAT* );
	LCXPLANE( const LCXPLANE& );
	LCXPLANE( FLOAT a, FLOAT b, FLOAT c, FLOAT d );

	// casting
	operator FLOAT* ();
	operator const FLOAT* () const;

	// unary operators
	LCXPLANE operator + () const;
	LCXPLANE operator - () const;

	// binary operators
	BOOL operator == ( const LCXPLANE& ) const;
	BOOL operator != ( const LCXPLANE& ) const;

	void SetupFromPointNormal(const LCXVECTOR3* point, const LCXVECTOR3* normal);
	void SetupFromPoints(const LCXVECTOR3* p0, const LCXVECTOR3* p1, const LCXVECTOR3* p2);
};


// Rect
struct LCXRECT
{
	union	{	struct	{	FLOAT x0, y0, x1, y1;	};	FLOAT m[4];		};
	
	LCXRECT();
	LCXRECT(const FLOAT*_m);
	LCXRECT(const LCXRECT& _rh );
	
	LCXRECT( FLOAT fx0,FLOAT fy0,FLOAT fx1,FLOAT fy1);
	
	LCXRECT( D3DXVECTOR2 p0, D3DXVECTOR2 p1);
	
	D3DXVECTOR2  Get00()	 const	;
	D3DXVECTOR2  Get10()	 const	;
	D3DXVECTOR2  Get01()	 const	;
	D3DXVECTOR2  Get11()	 const	;

	RECT   GetRECT()				;
	D3DRECT GetRectD3()				;
	
	FLOAT	GetWidth()				;
	FLOAT	GetHeight()				;
	D3DXVECTOR2	GetCenter() const	;
	
	// casting
	operator FLOAT*()				;
	operator const FLOAT*() const	;

//	const LCXRECT& operator=(const LCXRECT& v)	;

	bool operator==(const LCXRECT& v) const	;
	bool operator!=(const LCXRECT& v) const	;
	
	// assignment operators
	LCXRECT& operator+=(const LCXRECT& v)	;
	LCXRECT& operator-=(const LCXRECT& v)	;
	LCXRECT& operator*=(FLOAT f )			;
	LCXRECT& operator/=(FLOAT f )			;
	
	// unary operators
	LCXRECT operator+() const				;
	LCXRECT operator-() const				;
	
	// binary operators
	LCXRECT operator+(const LCXRECT& v)const	;
	LCXRECT operator-(const LCXRECT& v)const	;
	LCXRECT operator*(FLOAT f) const			;
	LCXRECT operator/(FLOAT f) const			;
	
	friend LCXRECT operator*(FLOAT f,const LCXRECT& v){	return LCXRECT(f * v.x0, f * v.y0, f * v.x1, f * v.y1);			}
	
	LCXRECT operator+(const D3DXVECTOR2& v)		;
	LCXRECT operator-(const D3DXVECTOR2& v)		;
	LCXRECT operator+=(const D3DXVECTOR2& v)	;
	LCXRECT operator-=(const D3DXVECTOR2& v)	;

	bool IsValid() const;
	bool IsCollided(const D3DXVECTOR2& v) const;
	bool IsCollided(const LCXRECT& v) const;
	void ClipAgainst(const LCXRECT& v);
	void SetCorrection();

};


// Line
struct LCXLINE
{
	// Start Pointer, Tranverse Vector
	union { struct { D3DXVECTOR3 p;	D3DXVECTOR3 t;	};	FLOAT m[6];	};
	
	LCXLINE();
	LCXLINE(const FLOAT*_m)		;
	LCXLINE(const LCXLINE& rhs)	;

	LCXLINE(FLOAT Px,FLOAT Py,FLOAT Pz,FLOAT Tx,FLOAT Ty,FLOAT Tz);
	LCXLINE(const D3DXVECTOR3& P, const D3DXVECTOR3& T);
	
	// casting
	operator FLOAT*()						;
	operator const FLOAT*() const			;
	
	// unary operators
	LCXLINE operator+() const				;
	LCXLINE operator-() const				;

	bool operator==(const LCXLINE& v) const	;
	bool operator!=(const LCXLINE& v) const	;
};


// Sphere
struct LCXSPHERE
{
	// center, Radius
	union { struct { D3DXVECTOR3 p;	FLOAT r;};	FLOAT m[4];	};
	
	LCXSPHERE();
	LCXSPHERE(const FLOAT*_m)				;
	LCXSPHERE(const LCXSPHERE& _rh)			;
	LCXSPHERE( FLOAT X,FLOAT Y,FLOAT Z,FLOAT R);
	
	// casting
	operator FLOAT*()						;
	operator const FLOAT*() const			;

	// unary operators
	LCXSPHERE operator+() const				;
	LCXSPHERE operator-() const				;

	bool operator==(const LCXSPHERE& v) const;
	bool operator!=(const LCXSPHERE& v) const;
};


// Cylinder
struct LCXCYLINDER
{
	union{ struct{	VEC3 p;	VEC3 t;	FLOAT r; FLOAT h; };	FLOAT m[8];	};
	
	LCXCYLINDER();	
	LCXCYLINDER(const FLOAT*_m);
	LCXCYLINDER(const LCXCYLINDER& v);
	LCXCYLINDER(FLOAT Px,FLOAT Py,FLOAT Pz,FLOAT Tx,FLOAT Ty,FLOAT Tz, FLOAT R, FLOAT H);
	LCXCYLINDER(const VEC3& P, const VEC3& T, FLOAT R, FLOAT H);
	
	// casting
	operator FLOAT*();
	operator const FLOAT*() const;
	
	// unary operators
	bool operator==(const LCXCYLINDER& v) const;
	bool operator!=(const LCXCYLINDER& v) const;

	// 임의의 두점과 반경과 높이
	void Set(const VEC3& p0, const VEC3& p1, FLOAT R, FLOAT H);
};



////////////////////////////////////////////////////////////////////////////////
//
// Hexahedron
//
//					Max
//		5------------7
//	   /|           /|
//	  / |          / |
//	 /  |         /  |
//	1---+--------3   |
//	|   |        |   |
//	|   4--------+---6
//	|  /         |  /
//	| /          | /
//	|/           |/
//	0------------2
//	Min
//
//
////////////////////////////////////////////////////////////////////////////////

struct LCXCUBEH
{
	union
	{
		struct
		{
			VEC3 _0; VEC3 _1; VEC3 _2; VEC3 _3;
			VEC3 _4; VEC3 _5; VEC3 _6; VEC3 _7;
		};// struct

		D3DVECTOR	m[8];

	}; // union

	
	LCXCUBEH();
	LCXCUBEH(const VEC3* _m);
	LCXCUBEH(	  const VEC3& __0
				, const VEC3& __1
				, const VEC3& __2
				, const VEC3& __3
				, const VEC3& __4
				, const VEC3& __5
				, const VEC3& __6
				, const VEC3& __7);

	operator FLOAT*();
	operator const FLOAT*() const;

//	VEC3* operator[](int nIdx) const
//	{
//		return (VEC3*)(&_0.x + nIdx * sizeof( VEC3));
//	}

	BOOL operator==(const LCXCUBEH& v) const;
	BOOL operator !=(const LCXCUBEH& v) const;

	void Set(const VEC3* _m);
	void Set(const LCXCUBEH& r);
};




////////////////////////////////////////////////////////////////////////////////
//
// Cuboid Axis Aligned Bounding Box
//
//	Edges
//
//					 
//		5------------7(Max)
//	y  /|           /|
//	| / |   z      / |
//	|/  |  /      /  |
//	1------------3   |
//	|   |/       |   |
//	|   4--------|---6
//	|  /         |  /
//	| /          | /
//	|/           |/
//	0(Min)-------2----- x
//	
//
////////////////////////////////////////////////////////////////////////////////

struct LCXCUBEAA
{
	union
	{
		struct
		{
			VEC3 vcMin;
			VEC3 vcMax;
		};// struct

		D3DVECTOR	m[2];

	}; // union

	
	LCXCUBEAA();
	LCXCUBEAA(const VEC3* _m);
	LCXCUBEAA(const VEC3& _min, const VEC3& _max);
	LCXCUBEAA(const VEC3& vc0);
	LCXCUBEAA(FLOAT* _p);
	LCXCUBEAA(FLOAT x0, FLOAT y0, FLOAT z0, FLOAT x1, FLOAT y1, FLOAT z1);
	
	BOOL operator==(const LCXCUBEAA& v) const { return vcMin == v.vcMin && v.vcMax == vcMax;}
	BOOL operator!=(const LCXCUBEAA& v) const { return vcMin != v.vcMin || v.vcMax != vcMax;}
	
	void AddPoint(const VEC3* pV);
	void AddPoint(FLOAT x, FLOAT y, FLOAT z);	
	void AddPoint(const LCXCUBEAA* b);
	
	void Set(FLOAT x, FLOAT y, FLOAT z);	
	void Set(const VEC3* pV);
	void Set(const LCXCUBEAA& pV);
	
	BOOL IsInside(const VEC3* pV);
	VEC3 GetCenter() const;
	VEC3 GetExtent() const;
	
	void GetEdge(VEC3* pvcEdge);
	void GetLine(VEC3* pvcLine);	
	BOOL IsEmpty() const;	
	void Repair();
};



////////////////////////////////////////////////////////////////////////////////
//
// Simple Triangle
//
////////////////////////////////////////////////////////////////////////////////

struct LCXTRI
{
	union
	{
		struct
		{
			VEC3	_0;
			VEC3	_1;
			VEC3	_2;
		};// struct

		D3DVECTOR	m[3];

	}; // union
	

	LCXTRI();	
	LCXTRI(const VEC3*_m);
	LCXTRI(VEC3 __0,VEC3 __1,VEC3 __2);

	LCXTRI(const LCXTRI& _r);
	LCXTRI(const FLOAT* _p);
	
	LCXTRI(FLOAT _0x,FLOAT _0y,FLOAT _0z
		, FLOAT _1x,FLOAT _1y,FLOAT _1z
		, FLOAT _2x,FLOAT _2y,FLOAT _2z);
	
	// casting
	operator FLOAT*();
	operator const FLOAT*() const;
	
	bool operator==(const LCXTRI& v) const;
	bool operator!=(const LCXTRI& v) const;

	void Set(VEC3 __0, VEC3 __1, VEC3 __2);
};



////////////////////////////////////////////////////////////////////////////////
//
// Picking Trangle
//
////////////////////////////////////////////////////////////////////////////////

struct TpckTri
{
	union
	{
		struct
		{
			VEC3	p0;
			VEC3	p1;
			VEC3	p2;
		};

		D3DVECTOR	vcT3[3];			// Picking Triangle Positions		
	};

	VEC3		vcPck;					// Picking Position or Bound Center
	FLOAT		fStlSrtR;				// Distance from Camera for Sorting or Bound Radius
	
	// Attributessss.
	INT			nId;					// Triangle Index
	INT			nAt;					// Attribute. Floor, Stair, Wall, Ceiling...
	

	TpckTri();
	TpckTri(FLOAT fR, VEC3* vcC, VEC3* vcT0, VEC3* vcT1, VEC3* vcT2);
	TpckTri(INT nIdx, INT nAttrib, FLOAT fR, VEC3* vcC, VEC3* vcT0, VEC3* vcT1, VEC3* vcT2);

	FLOAT	GetBoundRadius();
	VEC3	GetBoundCenter();
};


typedef vector<TpckTri >	lsPckT;
typedef	TpckTri				BndTri;




////////////////////////////////////////////////////////////////////////////////
//
// Pikcing Quad from Outdoor Map
//
////////////////////////////////////////////////////////////////////////////////

struct TpckRc
{
	VEC3	vcQ[4];				// Picking Quad
};

typedef vector<TpckRc >		lsMcRect;
typedef lsMcRect::iterator	itMcRect;


////////////////////////////////////////////////////////////////////////////////
//
// Pikcing ETC
//
////////////////////////////////////////////////////////////////////////////////

struct Tpck
{
	FLOAT		fStlSrtR;				// Distance from Camera for Sorting or Bound Radius
	VEC3		vcPck;					// Picking Position or Bound Center
	
	UINT		nId;					// Index
	DWORD		nAt;					// Etc

	Tpck(){};
	Tpck(const D3DXVECTOR3& r, FLOAT f, UINT s, DWORD n)
	{
		fStlSrtR = f;
		vcPck	= r;
		nId		= s;
		nAt		= n;
	}
};


//   Redefinition
//typedef LCXVECTOR2		VEC2;
//typedef LCXVECTOR3		VEC3;
//typedef LCXVECTOR4		VEC4;
//typedef LCXQUATERNION		QUAT;
//typedef LCXQUATERNION		QUAT;
//typedef LCXPLANE			DPLN;
//typedef LCXCOLOR			DCLR;











typedef D3DXVECTOR2				VEC2;
typedef D3DXVECTOR3				VEC3;
typedef D3DXMATRIX				MATA;


INT		LcMath_MakePower2(INT a);

void	LcMath_InterpolateFloat2(FLOAT* pOut, FLOAT* p1, FLOAT* p2, FLOAT d);
void	LcMath_InterpolateFloat3(FLOAT* pOut, FLOAT* p1, FLOAT* p2, FLOAT d);
void	LcMath_InterpolateFloat4(FLOAT* pOut, FLOAT* p1, FLOAT* p2, FLOAT d);
void	LcMath_InterpolateFloat	(FLOAT* pOut, FLOAT* p1, FLOAT* p2, FLOAT d, INT iCnt);
INT		LcMath_Equals			(FLOAT a, FLOAT b);

FLOAT	LcMath_Vec3Length		(FLOAT *pV1, FLOAT *pV2 );
FLOAT	LcMath_Vec3LengthSq		(FLOAT *pV1, FLOAT *pV2 );
FLOAT	LcMath_Vec3Dot			(FLOAT *pV1, FLOAT *pV2 );
void	LcMath_Vec3Cross		(FLOAT* pOut, FLOAT *pV1, FLOAT *pV2 );
void	LcMath_Vec3NormalVector	(VEC3* pOut, VEC3* p1, VEC3* p2, VEC3* p3);

void	LcMath_Vec3Normalize	(FLOAT* pOut, FLOAT *pV1, FLOAT *pV2 );
void	LcMath_Vec3Minimize		(FLOAT *pOut, FLOAT *pV1, FLOAT *pV2 );
void	LcMath_Vec3Maximize		(FLOAT *pOut, FLOAT *pV1, FLOAT *pV2 );

void	LcMath_Mat4X4Identity		(FLOAT* pM);
FLOAT	LcMath_QuaternionLength		(FLOAT *pV/*x,y,z,w*/);
FLOAT	LcMath_QuaternionLengthSq	(FLOAT *pV/*x,y,z,w*/);
FLOAT	LcMath_QuaternionDot		(FLOAT *pV1, FLOAT *pV2);
void	LcMath_QuaternionIdentity	(FLOAT *pV1);




// Collision

INT		LcMath_LineCross2D(VEC2 * p);

INT		LcMath_Point3DtoScreen(								// 3차원 점을 카메라의 Near 평면에 투영 하기
								VEC3* pOut					// Output
							,	const VEC3* vcIn			// Input
							,	const VEC3* vcCam			// Camera pos
							,	const VEC3*	vcZ				// Camera Z vector
							,	const MATA*	mtVP			// View matrix * Projection Matrix
							,	FLOAT fScnW					// Screen Width
							,	FLOAT fScnH					// Screen Height
						  );


INT		LcMath_MousePositionTo3D(VEC3* vcOut, MATA matView, MATA matProj, FLOAT fScnW, FLOAT fScnH, POINT ptMouse);

FLOAT	LcMath_PointToPlaneDistance(FLOAT* pPn/*VECTOR4*/, FLOAT* p1 /*VECTOR3*/);							// 점과 평면사이의 최단거리

FLOAT	LcMath_LineToPointDistance(const LCXLINE* pL, const D3DXVECTOR3* pP);								//  점과 직선의 최단거리


INT		LcMath_LineToSphere			(																		// 선과 구혹은 점의 충돌
										VEC3* pOut															// 선위에 있는 점과 선의 최단거리 지점
									,	FLOAT* fDst															// 선과 점의 최단거리
									,	const LCXLINE* pLine
									,	const VEC3* pC														// 점 혹은 구의 중심 점
									,	const FLOAT fR														// 구의 반경
									);


FLOAT	LcMath_LineToLineDistance(																			// 직선과 직선 사이의 최단 거리
									FLOAT* pSinTheta														// 직선과 직선 사이의 각도
								,	VEC3* pOut																// 직선과 직선사이의 노말벡터
								,	const LCXLINE* pV1														// 직선 1
								,	const LCXLINE* pV2														// 직선 2
								);


INT		LcMath_LineToLineIntersection(																		// 직선과 직선 사이의 관계 구하기: RETURN VALUE:겹처 있다면 -1, 평행 하면 -2 교차하면 1, 교차하지 않으면 최단거리의 점들을 돌려줌
										FLOAT* pSinTheta													// 직선과 직선 사이의 각도
									,	FLOAT* pDst															// 직선가 직선 사이의 최단 거리
									,	VEC3* pOut															// 최단 거리의 점들
									,	const LCXLINE* pV1													// 직선 1
									,	const LCXLINE* pV2													// 직선 2
									);


FLOAT	LcMath_LineToPlaneKValue(const DPLN* p1, const VEC3* p2, const VEC3* p3);						// K 값 구하기.


INT		LcMath_LineToTriangle(
									VEC3* pOut				// Pick Position
								,	FLOAT* fD				// Distance from vcRayPos to pOut
								,	VEC3* vcRayPos			// Starting Ray Position
								,	VEC3* vcRayDir			// Ray Direction(It must be Normalization.)
								,	VEC3* pTri
								,	BOOL bUseCull=TRUE);


INT		LcMath_LineToTriangle(
									VEC3* vcPck				// Pick Position
								,	FLOAT* D				// Distance from vcRayPos to pOut
								,	const VEC3* vcRayPos	// Starting Ray Position
								,	const VEC3* vcRayDir	// Starting Ray Position
								,	const VEC3* p0			// Triangle 0
								,	const VEC3* p1			// Triangle 1
								,	const VEC3* p2			// Triangle 2
								,	BOOL bUseCull=TRUE);


INT		LcMath_LineToTriangle		(									// 직선과 삼각형의 충돌
										VEC3* pOut						// 충돌 점
									,	const	VEC3* pTri				// 삼각형
									,	const	LCXLINE* pLine			// 직선
									,	BOOL bRectangle=FALSE			// 충돌영역을 삼각형의 확장  평행사변형으로 쓸것인지..
									,	BOOL bPositiveDirection=FALSE	// 직선의 방향으로만 충돌점을 구하기
									,	const VEC3* pBoundLine = NULL	// 유한 직선인 경우 양 끝 지점
									,	const VEC3* pTriNormal= NULL	// 삼각형의 Normal Vector
									);

INT		LcMath_TriangleToTriangle	(VEC3* pOut, VEC3* pV1, VEC3* pV2);	//삼각형 VS 삼각형 충돌

FLOAT	LcMath_CircumcircleCorrect	(VEC3* pOut,const VEC3* pTri);		// 정확한 삼각형의 외접원
FLOAT	LcMath_TriangleBoundSphere	(VEC3* pOut,const VEC3* pTri);		// 부정확한 삼각형의 외접원

INT		LcMath_IsInPoints	(const VEC3* p1, const VEC3* p2);
INT		LcMath_Vec3Greater	(const VEC3* p1, const VEC3* p2);
INT		LcMath_Vec3Less		(const VEC3* p1, const VEC3* p2);

void	LcMath_PlaneSetD	(DPLN* pOut, const VEC3* v);				// 평면의 d값 세팅
void	LcMath_Plane		(DPLN* pOut, const VEC3* n, FLOAT d);		// 법선 벡터와 원점에서의 최단거리로 평면 만들기


void	LcMath_PlaneSet						(DPLN* pOut, const VEC3* p0, const VEC3* n);					// 법선벡터와 점으로 평면 만들기
void	LcMath_PlaneSet						(DPLN* pOut, const VEC3* p0, const VEC3* p1, const VEC3* p2);	// 세점으로 평면 만들기
INT		LcMath_PlaneIntersectionWithLine	(VEC3* pOut, const DPLN* pPn, const VEC3* p1, const VEC3* vcT);	// 직선과 평면과의 충돌점
INT		LcMath_PlaneIntersectionWithPoints	(VEC3* pOut, const DPLN* pPn, const VEC3* p1, const VEC3* p2);	// 2점사이의 평면


FLOAT	LcMath_PlaneToPlaneDistance			(const LCXPLANE* pP1, const LCXPLANE* pP2);						// 평행한 평면사이의 최단거리

INT		LcMath_PlaneToPointRelation			(DPLN* pPn, const VEC3* p1);									// 임의의 점이 평면의 위(뒤)에 있는 가 아래(앞)에 있는가?
INT		LcMath_PlaneToPointIsFront			(const DPLN* pPn, const VEC3* p1, FLOAT fEpsilon=0.f);			// 점(구)이 평면의 앞에 있는가?
VEC3	LcMath_PlaneMemberPoint				(DPLN* pPn);													// Return 값의 크기는 d값, 리턴값의 크기를 d로 나누면 평면의 법선벡터
INT		LcMath_PlaneToPlaneExistsInterSect	(const DPLN* p1, const DPLN* p2);								//평행하면 Dot product는 1

INT		LcMath_PlaneToPlaneIntersectLine(	VEC3* pOutP	// Line Start Point									// 두 평면이 교차하는 직선의 방정식 구하기...
										  , VEC3* pOutT	// Line Direction
										  , const DPLN* pPn1
										  , const DPLN* pPn2);




FLOAT	LcMath_PlaneToPointMinDistance	(const FLOAT* pPlane/*VECTOR4*/, const FLOAT* pPoint /*VECTOR3*/);	// 평면과 점사이의 최단 거리
FLOAT	LcMath_PlaneToPointMinDistance	(const DPLN* pPn, const VEC3* p1);									// 평면과 점사이의 최단 거리

INT		LcMath_PointFromPlanes(VEC3* pOut, const DPLN* p1, const DPLN* p2, const DPLN* p3);					// 세평면이 겹치는 점 구하기



void	LcMath_MatrixRotationRadians		(MATA* pOut, const VEC3* pRot);
void	LcMath_MatrixRotationDegrees		(MATA* pOut, const VEC3* pRot);
void	LcMath_MatrixInverseRotationRadians	(MATA* pOut, const VEC3* pRot);
void	LcMath_MatrixInverseRotationDegrees	(MATA* pOut, const VEC3* pRot);

void	LcMath_Vec3RotationDegrees	(VEC3* pOut, const MATA* pIn);

void	LcMath_MatrixTranspose		(MATA* pOut, const MATA* pIn);
void	LcMath_Vec3Translate		(VEC3* pOut, const MATA* pT, const VEC3* pIn);
void	LcMath_Vec3InverseTranslate	(VEC3* pOut, const MATA* pT, const VEC3* pIn);
void	LcMath_Vec3Rotate			(VEC3* pOut, const MATA* pR, const VEC3* pIn);
void	LcMath_Vec3InverseRotate	(VEC3* pOut, const MATA* pR, const VEC3* pIn);
void	LcMath_Vec3Transform		(VEC3*pOut, const MATA* pT, const VEC3* pIn);
void	LcMath_PlaneTransform		(DPLN*pOut, const MATA* pT, DPLN* pIn);
void	LcMath_Vec4Transform		(VEC4*pOut, const MATA* pT, const VEC4* pIn);



INT	LcMath_UVFromTri(															// 삼각형 내부에 있는 점의 U V 값구하기 U V는 P0에서의 상대적인 값이다.
						FLOAT* fU
					,	FLOAT* fV
					,	const VEC3* p
					,	const VEC3* p0
					,	const VEC3* p1
					,	const VEC3* p2);

INT LcMath_SphereToTri(		VEC3* pOut											// 구와 삼각형의 최단거리의 지점
						,	FLOAT* fDist										// 구와 삼각형의 최단거리
						,	const VEC3* pSphereCenter							// 구의 중앙
						,	const FLOAT* pSphereRadius							// 구의 반경
						,	const VEC3* pTri									// 삼각형의 세점
						,	VEC3* pTriCenter	= NULL							// 삼각형의 바운드구의 중심
						,	FLOAT* pTriRadius	= NULL							// 삼각형의 바운드구의 반경
						,	VEC3* pTriNormal	= NULL							// 삼각형의 노말멕터
						);


INT LcMath_SphereToSphere(	const VEC3* pSphereCenter1							// 구의 중앙1
						,	const FLOAT* pSphereRadius1							// 구의 반경1
						,	const VEC3* pSphereCenter2							// 구의 중앙2
						,	const FLOAT* pSphereRadius2							// 구의 반경2
						);












////////////////////////////////////////////////////////////////////////////////
//
// Simple Triangle
//
////////////////////////////////////////////////////////////////////////////////


void	LcMath_TriTransform	(LCXTRI* pOut,const MATA* pT,const LCXTRI* pIn);
void	LcMath_TriMove		(LCXTRI* pOut,const VEC3* pV,const LCXTRI* pIn);	// Move Triangle







////////////////////////////////////////////////////////////////////////////////
//
// Hexahedron
//
//					Max
//		5------------7
//	   /|           /|
//	  / |          / |
//	 /  |         /  |
//	1---+--------3   |
//	|   |        |   |
//	|   4--------+---6
//	|  /         |  /
//	| /          | /
//	|/           |/
//	0------------2
//	Min
//
//
//
// Cuboid Axis Aligned Bounding Box
//
//	Edges
//
//					 
//		5------------7(Max)
//	y  /|           /|
//	| / |   z      / |
//	|/  |  /      /  |
//	1------------3   |
//	|   |/       |   |
//	|   4--------|---6
//	|  /         |  /
//	| /          | /
//	|/           |/
//	0(Min)-------2----- x
//	
//
////////////////////////////////////////////////////////////////////////////////


void	LcMath_CubeHMove		(LCXCUBEH* pOut,const VEC3* pV,const LCXCUBEH* pIn);	// Move Hexagon
void	LcMath_CubeHTransform	(LCXCUBEH* pOut,const MATA* pT,const LCXCUBEH* pIn);	// Transform Hexagon
void	LcMath_CubeHBoxLine		(void* pOut, INT nSizeChunk,const LCXCUBEH* pIn);		// Get Box Line
void	LcMath_CubeHFromA		(LCXCUBEH* pOut,const LCXCUBEAA* pIn);					// Cube Hexa from AABB

void	LcMath_CubeAMove		(LCXCUBEAA* pOut,const VEC3* pV,const LCXCUBEAA* pIn);	// Move Hexagon
void	LcMath_CubeATransform	(LCXCUBEAA*pOut,const MATA* pT,const LCXCUBEAA* pIn);

INT		LcMath_CubeAToLine		(	VEC3* vcOut
								 ,	VEC3* pVcBox
								 ,	LCXLINE* pLine
								 ,	BOOL bPositiveDirection=TRUE						// Line의 진행 방항으로 충돌할 것인가?
								 );

INT		LcMath_CubeAToLine		(	VEC3* vcOut
								 ,	LCXCUBEAA* pCube
								 ,	LCXLINE* pLine
								 ,	BOOL bPositiveDirection=TRUE
								 );

void	LcMath_CubeABoxLine		(void* pOut, INT nSizeChunk,const LCXCUBEAA* pIn);		// Get Hexagon Line

INT		LcMath_CubeHToSphere	(	const LCXCUBEH* pHexa								// Hexgagon To Sphere
								,	const VEC3* pPoint									// 중심점
								,	FLOAT fRadius=0.f									// 구의 반경
								);

INT		LcMath_CubeHToTri		(const LCXCUBEH* pHexa, const LCXTRI* pTri);				// Collision Hexa to Tri

INT		LcMath_CubeHToH			(const LCXCUBEH* pH1, const LCXCUBEH* pH2);				// Collision Hexagon to Hexagon





//
// Cylinder
//
////////////////////////////////////////////////////////////////////////////////

INT		LcMath_CylinderToLineIntersection(										// 원통과 직선의 충돌
												VEC3* pOut						// Intersection point
											,	const LCXCYLINDER* pV1			// Input Cylinder
											,	const LCXLINE* pV2				// Input Line
											,	const VEC3* pV3 = NULL			// Input Line Boundary
										  );

INT		LcMath_CylinderToTriIntersection(										// 원통과 삼각형의 충돌
											VEC3* pOut							// Intersection point
										,	const LCXCYLINDER* pV1				// Input Cylinder
										,	const VEC3* pV2						// Input Triangle
									  );

INT		LcMath_CylinderToTriIntersection(										// 원통과 삼각형의 충돌 원통의 중심선이 삼각형과 충돌안하면 return (nCnt+1)<<2;을 함으로 주의!!!
											VEC3* pOut							// Intersection point
										,	const LCXCYLINDER* pV1				// Input Cylinder
										,	BndTri* pV2							// Input Bound Triangle
										,	BOOL bRectangle=FALSE				// 충돌영역을 삼각형의 확장 평행사변형으로 쓸것인지..
										,	BOOL bPositiveDirection=FALSE		// 직선의 방향으로만 충돌점을 구하기
										,	const VEC3* pBoundLine = NULL		// 유한 직선인 경우 양 끝 지점
										,	const VEC3* pTriNormal= NULL		// 삼각형의 Normal Vector
									  );

BOOL LcMath_RcCollision(LCXRECT* v1, LCXRECT* v2);


//
// Color
//
////////////////////////////////////////////////////////////////////////////////

DWORD	LcColor_GetAlpha	(DWORD c);
DWORD	LcColor_GetRed		(DWORD c);
DWORD	LcColor_GetGreen	(DWORD c);
DWORD	LcColor_GetBlue		(DWORD c);

void	LcColor_SetAlpha	(DWORD& c, DWORD a);
void	LcColor_SetRed		(DWORD& c,DWORD r);
void	LcColor_SetGreen	(DWORD& c,DWORD g);
void	LcColor_SetBlue		(DWORD& c,DWORD b);

WORD	LcColor_RGB16		(INT r,INT g,INT b);
WORD	LcColor_RGBA16		(INT r,INT g,INT b,INT a);
WORD	LcColor_GetA1R5G5B5	(DWORD c);

WORD	LcColor_X8R8G8B8toA1R5G5B5(INT color);

// Returns value from A1R5G5B5 color	
INT		LcColor_GetAlpha16	(WORD color);
INT		LcColor_GetRed16	(WORD color);
INT		LcColor_GetGreen16	(WORD color);
INT		LcColor_GetBlue16	(WORD color);

INT		LcColor_GetLuminance(WORD color);

INT		LcColor_GetA1R5G5B5toA8R8G8B8(WORD color);
INT		LcColor_GetR5G6B5toA8R8G8B8(WORD color);

















#endif

