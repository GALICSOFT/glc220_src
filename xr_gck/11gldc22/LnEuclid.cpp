//
//////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <math.h>

#include <d3dx9math.h>

#include "LnEuclid.h"




//2D, or 3D Math
INT2::INT2():x(0),y(0){}
INT2::INT2(INT X,INT Y):x(X),y(Y){}
INT2::operator INT* ()				{	return (INT *) &x;				}
INT2::operator const INT* () const	{	return (const INT *) &x;		}


VEC2i::VEC2i()									{	x = y = 0;							}
VEC2i::VEC2i(INT _x,INT _y)					{	x = _x;			y = _y;				}
VEC2i::VEC2i(const VEC2i& rhs )				{	x = rhs.x;		y = rhs.y;			}
VEC2i::VEC2i(const INT *_m )					{	if(_m){x=_m[0];y=_m[2];}else{x=y=0;}}
VEC2i::VEC2i(D3DXVECTOR2 p)							{	x = INT(p.x);	y = INT(p.y);		}

VEC2i::operator INT*()							{	return (INT *) &x;					}
VEC2i::operator const INT*() const				{	return (const INT *) &x;			}

bool VEC2i::operator==(const VEC2i& v) const	{	return x == v.x && y == v.y;		}
bool VEC2i::operator!=(const VEC2i& v) const	{	return x != v.x || y != v.y;		}

const VEC2i& VEC2i::operator=(const VEC2i& rhs) {	x = rhs.x;	y = rhs.y;	return *this;	}

// assignment operators
VEC2i& VEC2i::operator+=(const VEC2i& rhs){	x += rhs.x;	y += rhs.y;	return *this;	}
VEC2i& VEC2i::operator-=(const VEC2i& rhs){	x -= rhs.x;	y -= rhs.y;	return *this;	}
VEC2i& VEC2i::operator*=(INT f )			{	x *= f;		y *= f;		return *this;	}
VEC2i& VEC2i::operator/=(INT f )			{	x /= f;		y /= f;		return *this;	}

// unary operators
VEC2i VEC2i::operator+() const				{	return *this;					}
VEC2i VEC2i::operator-() const				{	return VEC2i(-x, -y);			}

// binary operators
VEC2i VEC2i::operator+(const VEC2i& rhs) const	{	return VEC2i(x+rhs.x, y+rhs.y);	}
VEC2i VEC2i::operator-(const VEC2i& rhs) const	{	return VEC2i(x-rhs.x, y-rhs.y);	}
VEC2i VEC2i::operator*(const INT& f) const		{	return VEC2i(x*f, y*f);			}
VEC2i VEC2i::operator/(const INT& f) const		{	return VEC2i(x/f, y/f);			}


INT3::INT3():x(0),y(0),z(0){}
INT3::INT3(INT X,INT Y,INT Z):x(X),y(Y),z(Z){}
INT3::operator INT* ()				{	return (INT *) &x;				}
INT3::operator const INT* () const	{	return (const INT *) &x;		}


VEC3i::VEC3i()								{	x =				y =			  z = 0;		}
VEC3i::VEC3i(const INT *_m)				{	if(_m){x=_m[0];y=_m[1];z=_m[2];}else{x=y=z=0;}}
VEC3i::VEC3i(const VEC3i& rhs)				{	x = rhs.x;		y= rhs.y;	  z = rhs.z;	}
VEC3i::VEC3i(INT _x,INT _y, INT _z)		{	x = _x;			y = _y;		  z = _z;		}
VEC3i::VEC3i(const D3DXVECTOR3 p)			{	x = INT(p.x);	y = INT(p.y); z = INT(p.z);	}
VEC3i::VEC3i(FLOAT _x, FLOAT _y, FLOAT _z)	{	x = INT(_x);	y = INT(_y);  z = INT(_z);	}

VEC3i::operator INT*()						{	return (INT *) &x;							}
VEC3i::operator const INT*() const			{	return (const INT *) &x;					}

bool VEC3i::operator==(const VEC3i& v) const	{	return x == v.x && y == v.y && z == v.z;	}
bool VEC3i::operator!=(const VEC3i& v) const	{	return x != v.x || y != v.y || z != v.z;	}

const VEC3i& VEC3i::operator=(const VEC3i& rhs) {	x = rhs.x;	y = rhs.y;	z = rhs.z;	return *this;	}

VEC3i& VEC3i::operator+=(const VEC3i& v)	{	x += v.x; y += v.y;	z += v.z; return *this;	}
VEC3i& VEC3i::operator-=(const VEC3i& v)	{	x -= v.x; y -= v.y;	z -= v.z; return *this;	}
VEC3i& VEC3i::operator*=(INT f )			{	x *= f;	 y *= f;	z *= f;	  return *this;	}
VEC3i& VEC3i::operator/=(INT f )			{	x /= f;	 y /= f;	z /= f;	  return *this;	}

VEC3i VEC3i::operator+() const				{	return *this;								}
VEC3i VEC3i::operator-() const				{	return VEC3i(-x,-y, -z);					}

VEC3i VEC3i::operator+(const VEC3i& v)const{	return VEC3i(x + v.x, y + v.y, z + v.z);	}
VEC3i VEC3i::operator-(const VEC3i& v)const{	return VEC3i(x - v.x, y - v.y, z - v.z);	}
VEC3i VEC3i::operator*(INT f) const		{	return VEC3i(x*f, y*f, z*f);				}
VEC3i VEC3i::operator/(INT f) const		{	return VEC3i(x/f, y/f, z/f);				}



INT4::INT4():x(0),y(0),z(0),w(0){}
INT4::INT4(INT X,INT Y,INT Z,INT W):x(X),y(Y),z(Z),w(W){}
INT4::operator INT* ()				{	return (INT *) &x;				}
INT4::operator const INT* () const	{	return (const INT *) &x;		}



VEC4i::VEC4i()								{	x=y=z=w=0;									}
VEC4i::VEC4i(const INT *_m )				{	if(_m){x=_m[0];y=_m[1];z=_m[2];w=_m[3];}else{x=y=z=w=0;}	}
VEC4i::VEC4i(const VEC4i& r)				{	x=r.x; y=r.y; z=r.z; w= r.w;				}
VEC4i::VEC4i(INT X,INT Y,INT Z,INT W)		{	x=X; y=Y; z=Z; w=W;							}
VEC4i::VEC4i(const D3DXVECTOR4 p)			{	x=INT(p.x);y=INT(p.y);z=INT(p.z);w=INT(p.w);}

VEC4i::operator INT*()						{	return (INT *) &x;							}
VEC4i::operator const INT*() const			{	return (const INT *) &x;					}

bool VEC4i::operator==(const VEC4i& v) const{	return x==v.x&&y==v.y&&z==v.z&&w==v.w;		}
bool VEC4i::operator!=(const VEC4i& v) const{	return x!=v.x||y!=v.y||z!=v.z||w!=v.w;		}

const VEC4i& VEC4i::operator=(const VEC4i& rhs) {	x = rhs.x;	y = rhs.y;	z = rhs.z;	w = rhs.w;	return *this;	}

// assignment operators
VEC4i& VEC4i::operator+=(const VEC4i& v)	{	x+=v.x;y+=v.y;z+=v.z;w+=v.w;return *this;	}
VEC4i& VEC4i::operator-=(const VEC4i& v)	{	x-=v.x;y-=v.y;z-=v.z;w-=v.w;return *this;	}
VEC4i& VEC4i::operator*=(INT f )			{	x*=f; y*=f;	z*=f; w*=f;		return *this;	}
VEC4i& VEC4i::operator/=(INT f )			{	x/=f; y/=f;	z/=f; w/=f;		return *this;	}

// unary operators
VEC4i VEC4i::operator+() const				{	return *this;								}
VEC4i VEC4i::operator-() const				{	return VEC4i(-x,-y,-z,-w);				}

// binary operators
VEC4i VEC4i::operator+(const VEC4i& v)const{	return VEC4i(x+v.x,y+v.y,z+v.z,w+v.w);	}
VEC4i VEC4i::operator-(const VEC4i& v)const{	return VEC4i(x-v.x,y-v.y,z-v.z,w-v.w);	}
VEC4i VEC4i::operator*(INT f) const		{	return VEC4i(x*f,y*f,z*f, w*f);			}
VEC4i VEC4i::operator/(INT f) const		{	return VEC4i(x/f, y/f, z/f, w/f);		}



WORD2::WORD2():x(0),y(0){}
WORD2::WORD2(WORD X,WORD Y):x(X),y(Y){}
WORD2::operator WORD* ()				{	return (WORD *) &x;				}
WORD2::operator const WORD* () const	{	return (const WORD *) &x;		}


VEC2w::VEC2w()									{	x=y=0;		}
VEC2w::VEC2w(WORD X,WORD Y)					{	x=X; y=Y;	}



WORD3::WORD3():x(0),y(0),z(0){}
WORD3::WORD3(WORD X,WORD Y,WORD Z):x(X),y(Y),z(Z){}
WORD3::operator WORD* ()				{	return (WORD *) &x;				}
WORD3::operator const WORD* () const	{	return (const WORD *) &x;		}


VEC3w::VEC3w()									{	x=y=z=0;		}
VEC3w::VEC3w(WORD X,WORD Y, WORD Z, WORD W)	{	x=X; y=Y; z=Z;	}


WORD4::WORD4():x(0),y(0),z(0),w(0){}
WORD4::WORD4(WORD X,WORD Y,WORD Z,WORD W):x(X),y(Y),z(Z),w(W){}
WORD4::operator WORD* ()				{	return (WORD *) &x;				}
WORD4::operator const WORD* () const	{	return (const WORD *) &x;		}



VEC4w::VEC4w()									{	x=y=z=w=0;			}
VEC4w::VEC4w(WORD X,WORD Y, WORD Z, WORD W)	{	x=X; y=Y; z=Z; w=W;	}




DOUBLE2::DOUBLE2():x(0),y(0){}
DOUBLE2::DOUBLE2(DOUBLE X,DOUBLE Y):x(X),y(Y){}


DOUBLE3::DOUBLE3():x(0),y(0),z(0){}
DOUBLE3::DOUBLE3(DOUBLE X,DOUBLE Y,DOUBLE Z,DOUBLE W):x(X),y(Y),z(Z){}



DOUBLE4::DOUBLE4():x(0),y(0),z(0),w(0){}
DOUBLE4::DOUBLE4(DOUBLE X,DOUBLE Y,DOUBLE Z,DOUBLE W):x(X),y(Y),z(Z),w(W){}




LCXVECTOR2::LCXVECTOR2()
{
	x = 0;	y = 0;
}

LCXVECTOR2::LCXVECTOR2(const FLOAT* v)
{
	if(!v)
		return;

	x = v[0];	y = v[1];
}

LCXVECTOR2::LCXVECTOR2(const LCXVECTOR2& v)
{
	x = v.x;	y = v.y;
}

LCXVECTOR2::LCXVECTOR2(const LCXVECTOR2* v)
{
	if(!v)
		return;

	x = v->x;	y = v->y;
}

LCXVECTOR2::LCXVECTOR2(FLOAT X, FLOAT Y) : x(X), y(Y)
{
}

LCXVECTOR2::operator FLOAT*()
{
	return(FLOAT*)&x;
}

LCXVECTOR2::operator const FLOAT*() const
{
	return(const FLOAT*)&x;
}


FLOAT& LCXVECTOR2::operator[](INT n)
{
	return *((&x)+ n);
}

// assignment operators
LCXVECTOR2& LCXVECTOR2::operator +=(const LCXVECTOR2& v)
{
	x += v.x;	y += v.y;
	return *this;
}

LCXVECTOR2& LCXVECTOR2::operator -=(const LCXVECTOR2& v)
{
	x -= v.x;	y -= v.y;
	return *this;
}

LCXVECTOR2& LCXVECTOR2::operator *=(FLOAT v)
{
	x *= v;	y *= v;
	return *this;
}

LCXVECTOR2& LCXVECTOR2::operator /=(FLOAT v)
{
	v = 1.f/v;
	x *= v;	y *= v;
	return *this;
}


// unary operators
LCXVECTOR2 LCXVECTOR2::operator +() const
{
	return *this;
}

LCXVECTOR2 LCXVECTOR2::operator -() const
{
	return LCXVECTOR2(-x, -y);
}

// binary operators
LCXVECTOR2 LCXVECTOR2::operator +(const LCXVECTOR2& v) const	{	return LCXVECTOR2(x + v.x, y + v.y);	}
LCXVECTOR2 LCXVECTOR2::operator -(const LCXVECTOR2& v) const	{	return LCXVECTOR2(x - v.x, y - v.y);	}
LCXVECTOR2 LCXVECTOR2::operator *(FLOAT v) const				{	return LCXVECTOR2(x * v, y * v);		}
LCXVECTOR2 LCXVECTOR2::operator /(FLOAT v) const	{ v = 1.f/v;	return LCXVECTOR2(x * v, y * v);		}

LCXVECTOR2 operator *(FLOAT f, const LCXVECTOR2& v){			return LCXVECTOR2(f * v.x, f * v.y);		}
LCXVECTOR2 operator /(FLOAT f, const LCXVECTOR2& v){f=1.f/f;	return LCXVECTOR2(f * v.x, f * v.y);		}

BOOL LCXVECTOR2::operator ==(const LCXVECTOR2& v) const	{	return x == v.x && y == v.y;			}
BOOL LCXVECTOR2::operator !=(const LCXVECTOR2& v) const	{	return x != v.x || y != v.y;			}

// Dot Product
FLOAT LCXVECTOR2::operator *(const LCXVECTOR2& v)
{
	return x * v.x + y * v.y;
}


// Dot Product
FLOAT operator *(const LCXVECTOR2& v1, const LCXVECTOR2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}


// Cross Product
FLOAT LCXVECTOR2::operator ^(const LCXVECTOR2& v)
{
	return (x * v.y  -  y * v.x	);
}


// Cross Product
FLOAT operator ^(const LCXVECTOR2& v1, const LCXVECTOR2& v2)
{
	return FLOAT(v1.x * v2.y  -  v1.y * v2.x );
}


// Transform: vector * Matrix
LCXVECTOR2 LCXVECTOR2::operator *(const FLOAT* v)
{
	FLOAT X = x * v[0] + y * v[4];
	FLOAT Y = x * v[1] + y * v[5];
	FLOAT W = x * v[3] + y * v[7];

	W = 1.F/W;

	return LCXVECTOR2(X * W, Y * W);
}

// Transform: Matrix * vector;
LCXVECTOR2 operator *(const FLOAT* v1, const LCXVECTOR2& v2)
{
	FLOAT X = v2.x * v1[ 0] + v2.y * v1[ 1];
	FLOAT Y = v2.x * v1[ 4] + v2.y * v1[ 5];
	FLOAT W = v2.x * v1[12] + v2.y * v1[13];

	W = 1.F/W;

	return LCXVECTOR2(X * W, Y * W);
}


// Length
FLOAT LCXVECTOR2::Length()
{
	return sqrtf(x*x + y*y);
}

// Length Square
FLOAT LCXVECTOR2::LengthSq()
{
	return(x*x + y*y);
}


LCXVECTOR2 LCXVECTOR2::Normalize()
{
	FLOAT l  = this->Length();
	if(l == 0)
		return *this;

	x *= l;
	y *= l;
	return *this;
}


LCXVECTOR2 LCXVECTOR2::Normalize(const LCXVECTOR2* v)
{
	FLOAT l  = 0;

	*this = v;
	l  = this->Length();

	if(l == 0)
		return *this;

	l = 1.f/l;
	x *= l;	y *= l;

	return *this;
}



////////////////////////////////////////////////////////////////////////////////
LCXVECTOR3::LCXVECTOR3()
{
	x = 0;	y = 0;	z = 0;
}

LCXVECTOR3::LCXVECTOR3(const FLOAT* v)
{
	if(!v)
		return;

	x = v[0];	y = v[1];	z = v[2];
}

LCXVECTOR3::LCXVECTOR3(const LCXVECTOR3& v)
{
	x = v.x;	y = v.y;	z = v.z;
}

LCXVECTOR3::LCXVECTOR3(const LCXVECTOR3* v)
{
	if(!v)
		return;

	x = v->x;	y = v->y;	z = v->z;
}

LCXVECTOR3::LCXVECTOR3(FLOAT X, FLOAT Y, FLOAT Z) : x(X), y(Y), z(Z)
{
}

LCXVECTOR3::operator FLOAT*()
{
	return(FLOAT*)&x;
}

LCXVECTOR3::operator const FLOAT*() const
{
	return(const FLOAT*)&x;
}


FLOAT& LCXVECTOR3::operator[](INT n)
{
	return *((&x)+ n);
}

// assignment operators
LCXVECTOR3& LCXVECTOR3::operator +=(const LCXVECTOR3& v)
{
	x += v.x;	y += v.y;	z += v.z;
	return *this;
}

LCXVECTOR3& LCXVECTOR3::operator -=(const LCXVECTOR3& v)
{
	x -= v.x;	y -= v.y;	z -= v.z;
	return *this;
}

LCXVECTOR3& LCXVECTOR3::operator *=(FLOAT v)
{
	x *= v;	y *= v;	z *= v;
	return *this;
}

LCXVECTOR3& LCXVECTOR3::operator /=(FLOAT v)
{
	v = 1.f/v;
	x *= v;	y *= v;	z *= v;
	return *this;
}


// unary operators
LCXVECTOR3 LCXVECTOR3::operator +() const
{
	return *this;
}

LCXVECTOR3 LCXVECTOR3::operator -() const
{
	return LCXVECTOR3(-x, -y, -z);
}

// binary operators
LCXVECTOR3 LCXVECTOR3::operator +(const LCXVECTOR3& v) const	{	return LCXVECTOR3(x + v.x, y + v.y, z + v.z);	}
LCXVECTOR3 LCXVECTOR3::operator -(const LCXVECTOR3& v) const	{	return LCXVECTOR3(x - v.x, y - v.y, z - v.z);	}
LCXVECTOR3 LCXVECTOR3::operator *(FLOAT v) const				{	return LCXVECTOR3(x * v, y * v, z * v);			}
LCXVECTOR3 LCXVECTOR3::operator /(FLOAT v) const	{ v = 1.f/v;	return LCXVECTOR3(x * v, y * v, z * v);			}

LCXVECTOR3 operator *(FLOAT f, const LCXVECTOR3& v){			return LCXVECTOR3(f * v.x, f * v.y, f * v.z);	}
LCXVECTOR3 operator /(FLOAT f, const LCXVECTOR3& v){f=1.f/f;	return LCXVECTOR3(f * v.x, f * v.y, f * v.z);	}

BOOL LCXVECTOR3::operator ==(const LCXVECTOR3& v) const	{	return x == v.x && y == v.y && z == v.z;	}
BOOL LCXVECTOR3::operator !=(const LCXVECTOR3& v) const	{	return x != v.x || y != v.y || z != v.z;	}

// Dot Product
FLOAT LCXVECTOR3::operator *(const LCXVECTOR3& v)
{
	return x * v.x + y * v.y + z * v.z;
}


// Dot Product
FLOAT operator *(const LCXVECTOR3& v1, const LCXVECTOR3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


// Cross Product
LCXVECTOR3 LCXVECTOR3::operator ^(const LCXVECTOR3& v)
{
	return LCXVECTOR3(y * v.z  -  z * v.y
		, z * v.x  -  x * v.z
		, x * v.y  -  y * v.x	);
}


// Cross Product
LCXVECTOR3 operator ^(const LCXVECTOR3& v1, const LCXVECTOR3& v2)
{
	return LCXVECTOR3(v1.y * v2.z  -  v1.z * v2.y
		, v1.z * v2.x  -  v1.x * v2.z
		, v1.x * v2.y  -  v1.y * v2.x	);
}


// Transform: vector * Matrix
LCXVECTOR3 LCXVECTOR3::operator *(const FLOAT* v)
{
	FLOAT X = x * v[0] + y * v[4] + z * v[ 8] + v[12];
	FLOAT Y = x * v[1] + y * v[5] + z * v[ 9] + v[13];
	FLOAT Z = x * v[2] + y * v[6] + z * v[10] + v[14];
	FLOAT W = x * v[3] + y * v[7] + z * v[11] + v[15];

	W = 1.F/W;

	return LCXVECTOR3(X * W, Y * W, Z* W);
}

// Transform: Matrix * vector;
LCXVECTOR3 operator *(const FLOAT* v1, const LCXVECTOR3& v2)
{
	FLOAT X = v2.x * v1[ 0] + v2.y * v1[ 1] + v2.z * v1[ 2] + v1[ 3];
	FLOAT Y = v2.x * v1[ 4] + v2.y * v1[ 5] + v2.z * v1[ 6] + v1[ 7];
	FLOAT Z = v2.x * v1[ 8] + v2.y * v1[ 9] + v2.z * v1[10] + v1[11];
	FLOAT W = v2.x * v1[12] + v2.y * v1[13] + v2.z * v1[14] + v1[15];

	W = 1.F/W;

	return LCXVECTOR3(X * W, Y * W, Z* W);
}


// Length
FLOAT LCXVECTOR3::Length()
{
	return sqrtf(x*x + y*y + z*z);
}

// Length Square
FLOAT LCXVECTOR3::LengthSq()
{
	return(x*x + y*y + z*z);
}


LCXVECTOR3 LCXVECTOR3::Normalize()
{
	FLOAT l  = this->Length();
	if(l == 0)
		return *this;

	l = 1.f/l;
	x *= l;	y *= l;	z *= l;

	return *this;
}


LCXVECTOR3 LCXVECTOR3::Normalize(const LCXVECTOR3* v)
{
	FLOAT l  = 0;

	*this = v;
	l  = this->Length();

	if(l == 0)
		return *this;

	l = 1.f/l;
	x *= l;	y *= l;	z *= l;

	return *this;
}


LCXVECTOR3 LCXVECTOR3::Cross(const LCXVECTOR3* v1, const LCXVECTOR3* v2)
{
	x  = (v1->y * v2->z  -  v1->z * v2->y);
	y  = (v1->z * v2->x  -  v1->x * v2->z);
	z  = (v1->x * v2->y  -  v1->y * v2->x);

	return *this;
}







////////////////////////////////////////////////////////////////////////////////
LCXVECTOR4::LCXVECTOR4() : x(0), y(0), z(0), w(0)
{
}

LCXVECTOR4::LCXVECTOR4(const FLOAT* v)
{
	if(!v)
		return;

	x = v[0]; y = v[1];	z = v[2]; z = v[3];
}

LCXVECTOR4::LCXVECTOR4(const LCXVECTOR4& v)
{
	x = v.x;	y = v.y;	z = v.z;	w = v.w;
}

LCXVECTOR4::LCXVECTOR4(const LCXVECTOR4* v)
{
	if(!v)
		return;

	x = v->x;	y = v->y;	z = v->z;	w = v->w;
}

LCXVECTOR4::LCXVECTOR4(FLOAT X, FLOAT Y, FLOAT Z, FLOAT W) : x(X), y(Y), z(Z), w(W)
{
}

LCXVECTOR4::operator FLOAT*()
{
	return(FLOAT*)&x;
}

LCXVECTOR4::operator const FLOAT*() const
{
	return(const FLOAT*)&x;
}


FLOAT& LCXVECTOR4::operator[](INT n)
{
	return *((&x)+ n);
}

// assignment operators
LCXVECTOR4& LCXVECTOR4::operator +=(const LCXVECTOR4& v)
{
	x += v.x;	y += v.y;	z += v.z;	w += v.w;
	return *this;
}

LCXVECTOR4& LCXVECTOR4::operator -=(const LCXVECTOR4& v)
{
	x -= v.x;	y -= v.y;	z -= v.z;	w -= v.w;
	return *this;
}

LCXVECTOR4& LCXVECTOR4::operator *=(FLOAT v)
{
	x *= v;	y *= v;	z *= v;	w *= v;
	return *this;
}

LCXVECTOR4& LCXVECTOR4::operator /=(FLOAT v)
{
	v = 1.f/v;
	x *= v;	y *= v;	z *= v;	w *= v;
	return *this;
}


// unary operators
LCXVECTOR4 LCXVECTOR4::operator +() const
{
	return *this;
}

LCXVECTOR4 LCXVECTOR4::operator -() const
{
	return LCXVECTOR4(-x, -y, -z, -w);
}

// binary operators
LCXVECTOR4 LCXVECTOR4::operator +(const LCXVECTOR4& v) const	{	return LCXVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w);	}
LCXVECTOR4 LCXVECTOR4::operator -(const LCXVECTOR4& v) const	{	return LCXVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w);	}
LCXVECTOR4 LCXVECTOR4::operator *(FLOAT v) const				{	return LCXVECTOR4(x * v, y * v, z * v, w * v);			}
LCXVECTOR4 LCXVECTOR4::operator /(FLOAT v) const	{ v = 1.f/v;	return LCXVECTOR4(x * v, y * v, z * v, w * v);			}

LCXVECTOR4 operator *(FLOAT f, const LCXVECTOR4& v){			return LCXVECTOR4(f * v.x, f * v.y, f * v.z, f * v.w);	}
LCXVECTOR4 operator /(FLOAT f, const LCXVECTOR4& v){f=1.f/f;	return LCXVECTOR4(f * v.x, f * v.y, f * v.z, f * v.w);	}

BOOL LCXVECTOR4::operator ==(const LCXVECTOR4& v) const	{	return x == v.x && y == v.y && z == v.z && w == v.w;	}
BOOL LCXVECTOR4::operator !=(const LCXVECTOR4& v) const	{	return x != v.x || y != v.y || z != v.z || w != v.w;	}

// Dot Product
FLOAT LCXVECTOR4::operator *(const LCXVECTOR4& v)
{
	return x * v.x + y * v.y + z * v.z + w * v.w;
}


// Dot Product
FLOAT operator *(const LCXVECTOR4& v1, const LCXVECTOR4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}




// Transform: vector * Matrix
LCXVECTOR4 LCXVECTOR4::operator *(const FLOAT* v)
{
	FLOAT X = x * v[0] + y * v[4] + z * v[ 8] + w * v[12];
	FLOAT Y = x * v[1] + y * v[5] + z * v[ 9] + w * v[13];
	FLOAT Z = x * v[2] + y * v[6] + z * v[10] + w * v[14];
	FLOAT W = x * v[3] + y * v[7] + z * v[11] + w * v[15];

	return LCXVECTOR4(X, Y, Z, W);
}

// Transform: Matrix * vector;
LCXVECTOR4 operator *(const FLOAT* v1, const LCXVECTOR4& v2)
{
	FLOAT X = v2.x * v1[ 0] + v2.y * v1[ 1] + v2.z * v1[ 2] +  v2.w * v1[ 3];
	FLOAT Y = v2.x * v1[ 4] + v2.y * v1[ 5] + v2.z * v1[ 6] +  v2.w * v1[ 7];
	FLOAT Z = v2.x * v1[ 8] + v2.y * v1[ 9] + v2.z * v1[10] +  v2.w * v1[11];
	FLOAT W = v2.x * v1[12] + v2.y * v1[13] + v2.z * v1[14] +  v2.w * v1[15];

	return LCXVECTOR4(X, Y, Z, W);
}


// Length
FLOAT LCXVECTOR4::Length()
{
	return sqrtf(x*x + y*y + z*z + w*w);
}

// Length Square
FLOAT LCXVECTOR4::LengthSq()
{
	return(x*x + y*y + z*z + w*w);
}


LCXVECTOR4 LCXVECTOR4::Normalize()
{
	FLOAT l  = this->Length();
	if(l == 0)
		return *this;

	l = 1.f/l;
	x *= l;	y *= l;	z *= l;	w *= l;

	return *this;
}


LCXVECTOR4 LCXVECTOR4::Normalize(const LCXVECTOR4* v)
{
	FLOAT l  = 0;

	*this = v;
	l  = this->Length();

	if(l == 0)
		return *this;

	l = 1.f/l;
	x *= l;	y *= l;	z *= l;	w *= l;

	return *this;
}




////////////////////////////////////////////////////////////////////////////////

LCXMATRIX::LCXMATRIX()
{
	_11 = 1;  _12 = 0;  _13 = 0;  _14 = 0;
	_21 = 0;  _22 = 1;  _23 = 0;  _24 = 0;
	_31 = 0;  _32 = 0;  _33 = 1;  _34 = 0;
	_41 = 0;  _42 = 0;  _43 = 0;  _44 = 1;
}

LCXMATRIX::LCXMATRIX(const FLOAT* v)
{
	if(!v)
		return;

	_11 = v[ 0]; _12 = v[ 1]; _13 = v[ 2]; _14 = v[ 3];
	_21 = v[ 4]; _22 = v[ 5]; _23 = v[ 6]; _24 = v[ 7];
	_31 = v[ 8]; _32 = v[ 9]; _33 = v[10]; _34 = v[11];
	_41 = v[12]; _42 = v[13]; _43 = v[14]; _44 = v[15];
}

LCXMATRIX::LCXMATRIX(const LCXMATRIX& v)
{
	_11 = v._11; _12 = v._12; _13 = v._13; _14 = v._14;
	_21 = v._21; _22 = v._22; _23 = v._23; _24 = v._24;
	_31 = v._31; _32 = v._32; _33 = v._33; _34 = v._34;
	_41 = v._41; _42 = v._42; _43 = v._43; _44 = v._44;
}

LCXMATRIX::LCXMATRIX(	FLOAT v11, FLOAT v12, FLOAT v13, FLOAT v14,
					 FLOAT v21, FLOAT v22, FLOAT v23, FLOAT v24,
					 FLOAT v31, FLOAT v32, FLOAT v33, FLOAT v34,
					 FLOAT v41, FLOAT v42, FLOAT v43, FLOAT v44 )
{
	_11 = v11; _12 = v12; _13 = v13; _14 = v14;
	_21 = v21; _22 = v22; _23 = v23; _24 = v24;
	_31 = v31; _32 = v32; _33 = v33; _34 = v34;
	_41 = v41; _42 = v42; _43 = v43; _44 = v44;
}


// access grants
FLOAT& LCXMATRIX::operator()(INT iRow, INT iCol )
{
	return m[iRow][iCol];
}

FLOAT LCXMATRIX::operator()(INT iRow, INT iCol ) const
{
	return m[iRow][iCol];
}

// casting operators
LCXMATRIX::operator FLOAT*()
{
	return(FLOAT*)&_11;
}

LCXMATRIX::operator const FLOAT*() const
{
	return(FLOAT*)&_11;
}

// assignment operators
LCXMATRIX& LCXMATRIX::operator *=(const LCXMATRIX& v)
{
	LCXMATRIX t = *this;
	_11 = t._11 * v._11 +  t._12 * v._21 +  t._13 * v._31 +  t._14 * v._41;
	_12 = t._11 * v._12 +  t._12 * v._22 +  t._13 * v._32 +  t._14 * v._42;
	_13 = t._11 * v._13 +  t._12 * v._23 +  t._13 * v._33 +  t._14 * v._43;
	_14 = t._11 * v._14 +  t._12 * v._24 +  t._13 * v._34 +  t._14 * v._44;

	_21 = t._21 * v._11 +  t._22 * v._21 +  t._23 * v._31 +  t._24 * v._41;
	_22 = t._21 * v._12 +  t._22 * v._22 +  t._23 * v._32 +  t._24 * v._42;
	_23 = t._21 * v._13 +  t._22 * v._23 +  t._23 * v._33 +  t._24 * v._43;
	_24 = t._21 * v._14 +  t._22 * v._24 +  t._23 * v._34 +  t._24 * v._44;

	_31 = t._31 * v._11 +  t._32 * v._21 +  t._33 * v._31 +  t._34 * v._41;
	_32 = t._31 * v._12 +  t._32 * v._22 +  t._33 * v._32 +  t._34 * v._42;
	_33 = t._31 * v._13 +  t._32 * v._23 +  t._33 * v._33 +  t._34 * v._43;
	_34 = t._31 * v._14 +  t._32 * v._24 +  t._33 * v._34 +  t._34 * v._44;

	_41 = t._41 * v._11 +  t._42 * v._21 +  t._43 * v._31 +  t._44 * v._41;
	_42 = t._41 * v._12 +  t._42 * v._22 +  t._43 * v._32 +  t._44 * v._42;
	_43 = t._41 * v._13 +  t._42 * v._23 +  t._43 * v._33 +  t._44 * v._43;
	_44 = t._41 * v._14 +  t._42 * v._24 +  t._43 * v._34 +  t._44 * v._44;

	return * this;
}


LCXMATRIX& LCXMATRIX::operator +=(const LCXMATRIX& v)
{
	_11 += v._11;	_12 += v._12;	_13 += v._13;	_14 += v._14;
	_21 += v._21;	_22 += v._22;	_23 += v._23;	_24 += v._24;
	_31 += v._31;	_32 += v._32;	_33 += v._33;	_34 += v._34;
	_41 += v._41;	_42 += v._42;	_43 += v._43;	_44 += v._44;

	return * this;
}

LCXMATRIX& LCXMATRIX::operator -=(const LCXMATRIX& v)
{
	_11 -= v._11;	_12 -= v._12;	_13 -= v._13;	_14 -= v._14;
	_21 -= v._21;	_22 -= v._22;	_23 -= v._23;	_24 -= v._24;
	_31 -= v._31;	_32 -= v._32;	_33 -= v._33;	_34 -= v._34;
	_41 -= v._41;	_42 -= v._42;	_43 -= v._43;	_44 -= v._44;

	return * this;
}

LCXMATRIX& LCXMATRIX::operator *=(FLOAT v)
{
	_11 *= v;	_12 *= v;	_13 *= v;	_14 *= v;
	_21 *= v;	_22 *= v;	_23 *= v;	_24 *= v;
	_31 *= v;	_32 *= v;	_33 *= v;	_34 *= v;
	_41 *= v;	_42 *= v;	_43 *= v;	_44 *= v;

	return * this;
}

LCXMATRIX& LCXMATRIX::operator /=(FLOAT v)
{
	v = 1.f/v;
	_11 *= v;	_12 *= v;	_13 *= v;	_14 *= v;
	_21 *= v;	_22 *= v;	_23 *= v;	_24 *= v;
	_31 *= v;	_32 *= v;	_33 *= v;	_34 *= v;
	_41 *= v;	_42 *= v;	_43 *= v;	_44 *= v;

	return * this;
}


// unary operators
LCXMATRIX LCXMATRIX::operator +() const
{
	return *this;
}

LCXMATRIX LCXMATRIX::operator -() const
{
	return LCXMATRIX(	-_11, -_12, -_13, -_14,
		-_21, -_22, -_23, -_24,
		-_31, -_32, -_33, -_34,
		-_41, -_42, -_43, -_44);
}

// binary operators
LCXMATRIX LCXMATRIX::operator *(const LCXMATRIX& v) const
{
	LCXMATRIX t = *this;
	LCXMATRIX r;
	r._11 = t._11 * v._11 +  t._12 * v._21 +  t._13 * v._31 +  t._14 * v._41;
	r._12 = t._11 * v._12 +  t._12 * v._22 +  t._13 * v._32 +  t._14 * v._42;
	r._13 = t._11 * v._13 +  t._12 * v._23 +  t._13 * v._33 +  t._14 * v._43;
	r._14 = t._11 * v._14 +  t._12 * v._24 +  t._13 * v._34 +  t._14 * v._44;

	r._21 = t._21 * v._11 +  t._22 * v._21 +  t._23 * v._31 +  t._24 * v._41;
	r._22 = t._21 * v._12 +  t._22 * v._22 +  t._23 * v._32 +  t._24 * v._42;
	r._23 = t._21 * v._13 +  t._22 * v._23 +  t._23 * v._33 +  t._24 * v._43;
	r._24 = t._21 * v._14 +  t._22 * v._24 +  t._23 * v._34 +  t._24 * v._44;

	r._31 = t._31 * v._11 +  t._32 * v._21 +  t._33 * v._31 +  t._34 * v._41;
	r._32 = t._31 * v._12 +  t._32 * v._22 +  t._33 * v._32 +  t._34 * v._42;
	r._33 = t._31 * v._13 +  t._32 * v._23 +  t._33 * v._33 +  t._34 * v._43;
	r._34 = t._31 * v._14 +  t._32 * v._24 +  t._33 * v._34 +  t._34 * v._44;

	r._41 = t._41 * v._11 +  t._42 * v._21 +  t._43 * v._31 +  t._44 * v._41;
	r._42 = t._41 * v._12 +  t._42 * v._22 +  t._43 * v._32 +  t._44 * v._42;
	r._43 = t._41 * v._13 +  t._42 * v._23 +  t._43 * v._33 +  t._44 * v._43;
	r._44 = t._41 * v._14 +  t._42 * v._24 +  t._43 * v._34 +  t._44 * v._44;

	return r;
}


LCXMATRIX LCXMATRIX::operator +(const LCXMATRIX& v) const
{
	return LCXMATRIX(	_11 + v._11, _12 + v._12, _13 + v._13, _14 + v._14,
		_21 + v._21, _22 + v._22, _23 + v._23, _24 + v._24,
		_31 + v._31, _32 + v._32, _33 + v._33, _34 + v._34,
		_41 + v._41, _42 + v._42, _43 + v._43, _44 + v._44);
}

LCXMATRIX LCXMATRIX::operator -(const LCXMATRIX& v) const
{
	return LCXMATRIX(	_11 - v._11, _12 - v._12, _13 - v._13, _14 - v._14,
		_21 - v._21, _22 - v._22, _23 - v._23, _24 - v._24,
		_31 - v._31, _32 - v._32, _33 - v._33, _34 - v._34,
		_41 - v._41, _42 - v._42, _43 - v._43, _44 - v._44);
}

LCXMATRIX LCXMATRIX::operator *(FLOAT v) const
{
	return LCXMATRIX(	_11 * v, _12 * v, _13 * v, _14 * v,
		_21 * v, _22 * v, _23 * v, _24 * v,
		_31 * v, _32 * v, _33 * v, _34 * v,
		_41 * v, _42 * v, _43 * v, _44 * v);
}

LCXMATRIX LCXMATRIX::operator /(FLOAT v) const
{
	v = 1.f / v;
	return LCXMATRIX(	_11 * v, _12 * v, _13 * v, _14 * v,
		_21 * v, _22 * v, _23 * v, _24 * v,
		_31 * v, _32 * v, _33 * v, _34 * v,
		_41 * v, _42 * v, _43 * v, _44 * v);
}


LCXMATRIX operator *(FLOAT f, const LCXMATRIX& v)
{
	return LCXMATRIX(	f * v._11, f * v._12, f * v._13, f * v._14,
		f * v._21, f * v._22, f * v._23, f * v._24,
		f * v._31, f * v._32, f * v._33, f * v._34,
		f * v._41, f * v._42, f * v._43, f * v._44);
}

LCXMATRIX operator *(const LCXMATRIX& v, FLOAT f)
{
	return LCXMATRIX(	f * v._11, f * v._12, f * v._13, f * v._14,
		f * v._21, f * v._22, f * v._23, f * v._24,
		f * v._31, f * v._32, f * v._33, f * v._34,
		f * v._41, f * v._42, f * v._43, f * v._44);
}


BOOL LCXMATRIX::operator ==(const LCXMATRIX& v) const
{
	return	(
		_11 == v._11 && _12 == v._12 && _13 == v._13 && _14 == v._14 &&
		_21 == v._21 && _22 == v._22 && _23 == v._23 && _24 == v._24 &&
		_31 == v._31 && _32 == v._32 && _33 == v._33 && _34 == v._34 &&
		_41 == v._41 && _42 == v._42 && _43 == v._43 && _44 == v._44
		);
}

BOOL LCXMATRIX::operator !=(const LCXMATRIX& v) const
{
	return	(
		_11 != v._11 || _12 != v._12 || _13 != v._13 || _14 != v._14 ||
		_21 != v._21 || _22 != v._22 || _23 != v._23 || _24 != v._24 ||
		_31 != v._31 || _32 != v._32 || _33 != v._33 || _34 != v._34 ||
		_41 != v._41 || _42 != v._42 || _43 != v._43 || _44 != v._44
		);
}


// Length
LCXMATRIX& LCXMATRIX::Identity()
{
	_11=1; _12=0; _13=0; _14=0;
	_21=0; _22=1; _23=0; _24=0;
	_31=0; _32=0; _33=1; _34=0;
	_41=0; _42=0; _43=0; _44=1;
	return *this;
}


LCXMATRIX& LCXMATRIX::Transpose()
{
	FLOAT t;

	t   = _12;	_12 = _21;	_21 = t;
	t   = _13;	_13 = _31;	_31 = t;
	t   = _14;	_14 = _41;	_41 = t;

	t   = _23;	_23 = _32;	_32 = t;
	t   = _24;	_24 = _42;	_42 = t;

	t   = _34;	_34 = _43;	_43 = t;

	return *this;
}


LCXMATRIX& LCXMATRIX::Transpose(const LCXMATRIX* v)
{
	FLOAT t =0;
	*this = *v;

	t   = _12;	_12 = _21;	_21 = t;
	t   = _13;	_13 = _31;	_31 = t;
	t   = _14;	_14 = _41;	_41 = t;

	t   = _23;	_23 = _32;	_32 = t;
	t   = _24;	_24 = _42;	_42 = t;

	t   = _34;	_34 = _43;	_43 = t;

	return *this;
}


LCXMATRIX& LCXMATRIX::Inverse(INT nQuick)
{
	LCXMATRIX	t = *this;
	this->Inverse(&t, nQuick);
	return *this;
}


LCXMATRIX& LCXMATRIX::Inverse(const LCXMATRIX* pIn, INT nQuick)
{
	if(TRUE == nQuick)
	{
		FLOAT fDet = 1;

	// 4X4 행렬은 Determinant를 1로 정한다.
		fDet =	pIn->_11 * ( pIn->_22 * pIn->_33 - pIn->_23 * pIn->_32 ) +
				pIn->_12 * ( pIn->_23 * pIn->_31 - pIn->_21 * pIn->_33 ) +
				pIn->_13 * ( pIn->_21 * pIn->_32 - pIn->_22 * pIn->_31 );

		fDet = 1.f / fDet;

		this->_11 =  fDet * ( pIn->_22 * pIn->_33 - pIn->_23 * pIn->_32 );
		this->_12 = -fDet * ( pIn->_12 * pIn->_33 - pIn->_13 * pIn->_32 );
		this->_13 =  fDet * ( pIn->_12 * pIn->_23 - pIn->_13 * pIn->_22 );
		this->_14 = 0.0f;

		this->_21 = -fDet * ( pIn->_21 * pIn->_33 - pIn->_23 * pIn->_31 );
		this->_22 =  fDet * ( pIn->_11 * pIn->_33 - pIn->_13 * pIn->_31 );
		this->_23 = -fDet * ( pIn->_11 * pIn->_23 - pIn->_13 * pIn->_21 );
		this->_24 = 0.0f;

		this->_31 =  fDet * ( pIn->_21 * pIn->_32 - pIn->_22 * pIn->_31 );
		this->_32 = -fDet * ( pIn->_11 * pIn->_32 - pIn->_12 * pIn->_31 );
		this->_33 =  fDet * ( pIn->_11 * pIn->_22 - pIn->_12 * pIn->_21 );
		this->_34 = 0.0f;

		this->_41 = -( pIn->_41 * this->_11 + pIn->_42 * this->_21 + pIn->_43 * this->_31 );
		this->_42 = -( pIn->_41 * this->_12 + pIn->_42 * this->_22 + pIn->_43 * this->_32 );
		this->_43 = -( pIn->_41 * this->_13 + pIn->_42 * this->_23 + pIn->_43 * this->_33 );
		this->_44 = 1.0f;

		return *this;
	}

	// this code came from Intel.

	float*	v = (float*)pIn;
	float*	p = &m[0][0];

	float tmp[12]; /* temp array for pairs */
	float src[16]; /* array of transpose source matrix */
	float det; /* determinant */
	/* transpose matrix */
	for (int i = 0; i < 4; i++)
	{
		src[i] = v[i*4];
		src[i + 4 ] = v[i*4 + 1];
		src[i + 8 ] = v[i*4 + 2];
		src[i + 12] = v[i*4 + 3];
	}
	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[ 9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[ 9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[ 8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[ 8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10]= src[ 8] * src[13];
	tmp[11]= src[ 9] * src[12];
	/* calculate first 8 elements (cofactors) */
	p[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[ 4]*src[7];
	p[0]-= tmp[1]*src[5] + tmp[2]*src[6] + tmp[ 5]*src[7];
	p[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[ 9]*src[7];
	p[1]-= tmp[0]*src[4] + tmp[7]*src[6] + tmp[ 8]*src[7];
	p[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	p[2]-= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	p[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	p[3]-= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	p[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[ 5]*src[3];
	p[4]-= tmp[0]*src[1] + tmp[3]*src[2] + tmp[ 4]*src[3];
	p[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[ 8]*src[3];
	p[5]-= tmp[1]*src[0] + tmp[6]*src[2] + tmp[ 9]*src[3];
	p[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	p[6]-= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	p[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	p[7]-= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
	
	
	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];
	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10]= src[0]*src[5];
	tmp[11]= src[1]*src[4];
	/* calculate second 8 elements (cofactors) */
	p[ 8] = tmp[ 0]*src[13] + tmp[ 3]*src[14] + tmp[ 4]*src[15];
	p[ 8]-= tmp[ 1]*src[13] + tmp[ 2]*src[14] + tmp[ 5]*src[15];
	p[ 9] = tmp[ 1]*src[12] + tmp[ 6]*src[14] + tmp[ 9]*src[15];
	p[ 9]-= tmp[ 0]*src[12] + tmp[ 7]*src[14] + tmp[ 8]*src[15];
	p[10] = tmp[ 2]*src[12] + tmp[ 7]*src[13] + tmp[10]*src[15];
	p[10]-= tmp[ 3]*src[12] + tmp[ 6]*src[13] + tmp[11]*src[15];
	p[11] = tmp[ 5]*src[12] + tmp[ 8]*src[13] + tmp[11]*src[14];
	p[11]-= tmp[ 4]*src[12] + tmp[ 9]*src[13] + tmp[10]*src[14];
	p[12] = tmp[ 2]*src[10] + tmp[ 5]*src[11] + tmp[ 1]*src[ 9];
	p[12]-= tmp[ 4]*src[11] + tmp[ 0]*src[ 9] + tmp[ 3]*src[10];
	p[13] = tmp[ 8]*src[11] + tmp[ 0]*src[ 8] + tmp[ 7]*src[10];
	p[13]-= tmp[ 6]*src[10] + tmp[ 9]*src[11] + tmp[ 1]*src[ 8];
	p[14] = tmp[ 6]*src[ 9] + tmp[11]*src[11] + tmp[ 3]*src[ 8];
	p[14]-= tmp[10]*src[11] + tmp[ 2]*src[ 8] + tmp[ 7]*src[ 9];
	p[15] = tmp[10]*src[10] + tmp[ 4]*src[ 8] + tmp[ 9]*src[ 9];
	p[15]-= tmp[ 8]*src[ 9] + tmp[11]*src[10] + tmp[ 5]*src[ 8];
	/* calculate determinant */
	det=src[0]*p[0]+src[1]*p[1]+src[2]*p[2]+src[3]*p[3];
	/* calculate matrix inverse */
	det = 1/det;
	for (int j = 0; j < 16; j++)
		p[j] *= det;


	return *this;
}


FLOAT LCXMATRIX::Determinant()
{
	FLOAT t =0;

	t += (_24 * _11 - _14 * _21) * (_32 * _43 - _33 * _42);
	t += (_24 * _12 - _14 * _22) * (_33 * _41 - _31 * _43);
	t += (_24 * _13 - _14 * _23) * (_31 * _42 - _32 * _41);

	t += (_44 * _31 - _34 * _41) * (_12 * _23 - _13 * _22);
	t += (_44 * _32 - _34 * _42) * (_13 * _21 - _11 * _23);
	t += (_44 * _33 - _34 * _43) * (_11 * _22 - _12 * _21);

	return t;
}


LCXMATRIX& LCXMATRIX::SetupScaling(FLOAT X, FLOAT Y, FLOAT Z)
{
	this->Identity();
	_11 = X;    _22 = Y;    _33 = Z;
	return *this;
}




LCXMATRIX& LCXMATRIX::SetupViewLH(const LCXVECTOR3* pEye, const LCXVECTOR3* pLook, const LCXVECTOR3* pUp)
{
	LCXVECTOR3 vcX;
	LCXVECTOR3 vcY;
	LCXVECTOR3 vcZ;

	vcZ = *pLook - *pEye;
	vcZ.Normalize();

	vcX	= *pUp ^ vcZ;
	vcX.Normalize();

	vcY = vcZ ^ vcX;

	this->Identity();

	_11 = vcX.x;    _12 = vcY.x;    _13 = vcZ.x;
	_21 = vcX.y;    _22 = vcY.y;    _23 = vcZ.y;
	_31 = vcX.z;    _32 = vcY.z;    _33 = vcZ.z;

	_41 = - *pEye * vcX;
	_42 = - *pEye * vcY;
	_43 = - *pEye * vcZ;

	return *this;
}


LCXMATRIX& LCXMATRIX::SetupPerspectiveLH(FLOAT fFov, FLOAT fAsp, FLOAT fNear, FLOAT fFar)
{
	fFov	*=0.5f;
	FLOAT cot = (FLOAT)cos(fFov)/(FLOAT)sin(fFov);

	FLOAT h = cot;
	FLOAT w = cot/fAsp;
	FLOAT Q = fFar / ( fFar - fNear );

	this->Identity();

	_11 = w;
	_22 = h;
	_33 = Q;
	_34 = 1.f;
	_43 = -fNear * Q;
	_44 = 0.f;

	return *this;
}


LCXMATRIX& LCXMATRIX::SetupRotationX(FLOAT fRad)
{
	this->Identity();
	_22 =  (FLOAT)cos( fRad );
	_23 =  (FLOAT)sin( fRad );
	_32 = -(FLOAT)sin( fRad );
	_33 =  (FLOAT)cos( fRad );
	return *this;
}


LCXMATRIX& LCXMATRIX::SetupRotationY(FLOAT fRads)
{
	this->Identity();
	_11 =  (FLOAT)cos( fRads );
	_13 = -(FLOAT)sin( fRads );
	_31 =  (FLOAT)sin( fRads );
	_33 =  (FLOAT)cos( fRads );

	return *this;
}


LCXMATRIX& LCXMATRIX::SetupRotationZ(FLOAT fRads)
{
	this->Identity();
	_11  =  (FLOAT)cos( fRads );
	_12  =  (FLOAT)sin( fRads );
	_21  = -(FLOAT)sin( fRads );
	_22  =  (FLOAT)cos( fRads );

	return *this;
}


LCXMATRIX& LCXMATRIX::SetupRotationAxis(const LCXVECTOR3* vAxis, FLOAT fRad)
{
	FLOAT     fCos = (FLOAT)cos( fRad );
	FLOAT     fSin = (FLOAT)sin( fRad );
	LCXVECTOR3 v    = *vAxis;

	v.Normalize();

	this->Identity();

	_11 = ( v.x * v.x ) * ( 1 - fCos ) + fCos;
	_12 = ( v.x * v.y ) * ( 1 - fCos ) - (v.z * fSin);
	_13 = ( v.x * v.z ) * ( 1 - fCos ) + (v.y * fSin);

	_21 = ( v.y * v.x ) * ( 1 - fCos ) + (v.z * fSin);
	_22 = ( v.y * v.y ) * ( 1 - fCos ) + fCos ;
	_23 = ( v.y * v.z ) * ( 1 - fCos ) - (v.x * fSin);

	_31 = ( v.z * v.x ) * ( 1 - fCos ) - (v.y * fSin);
	_32 = ( v.z * v.y ) * ( 1 - fCos ) + (v.x * fSin);
	_33 = ( v.z * v.z ) * ( 1 - fCos ) + fCos;

	return *this;
}


LCXMATRIX& LCXMATRIX::SetupTranslation(const LCXVECTOR3* vTrs, BOOL bInit)
{
	if(bInit)
		this->Identity();

	_41 = vTrs->x;
	_42 = vTrs->y;
	_43 = vTrs->z;

	return *this;
}


void LCXMATRIX::TransformCoord(LCXVECTOR3* pOut, const LCXVECTOR3* pIn)
{
	FLOAT	_x, _y, _z;
	FLOAT	x, y, z, w;

	_x = pIn->x;
	_y = pIn->y;
	_z = pIn->z;

	x = _11 * _x +  _21 * _y + _31 * _z   +  _41;
	y = _12 * _x +  _22 * _y + _32 * _z   +  _42;
	z = _13 * _x +  _23 * _y + _33 * _z   +  _43;
	w = _14 * _x +  _24 * _y + _34 * _z   +  _44;

	x /=w;
	y /=w;
	z /=w;
	w =1;

	pOut->x =x;
	pOut->y =y;
	pOut->z =z;

}



////////////////////////////////////////////////////////////////////////////////
// Quaternion

LCXQUATERNION::LCXQUATERNION()
{
	x = 0;	y= 0; z = 0; w = 1;
}


LCXQUATERNION::LCXQUATERNION(const FLOAT* pf )
{
	x = pf[0];	y = pf[1];	z = pf[2];	w = pf[3];
}


LCXQUATERNION::LCXQUATERNION(const LCXQUATERNION& v)
{
	x = v.x;	y = v.y;	z = v.z;	w = v.w;
}


LCXQUATERNION::LCXQUATERNION( FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw )
{
	x = fx;	y = fy;	z = fz;	w = fw;
}


// casting
LCXQUATERNION::operator FLOAT* ()
{
	return (FLOAT *) &x;
}


LCXQUATERNION::operator const FLOAT* () const
{
	return (const FLOAT *) &x;
}


// assignment operators
LCXQUATERNION& LCXQUATERNION::operator += (const LCXQUATERNION& q )
{
	x += q.x;	y += q.y;	z += q.z;	w += q.w;
	return *this;
}

LCXQUATERNION& LCXQUATERNION::operator -= (const LCXQUATERNION& q )
{
	x -= q.x;	y -= q.y;	z -= q.z;	w -= q.w;
	return *this;
}

LCXQUATERNION& LCXQUATERNION::operator *= (const LCXQUATERNION& q )
{
	LCXQUATERNION q1 = *this;
	LCXQUATERNION q2 = q;

	this->w = q1.w * q2.w - ( q1.x * q2.x + q1.y * q2.y + q1.z * q2.z);

	this->x = q1.w * q2.x + q2.w * q1.x + (q1.y * q2.z - q1.z * q2.y);
	this->y = q1.w * q2.y + q2.w * q1.y + (q1.z * q2.x - q1.x * q2.z);
	this->z = q1.w * q2.z + q2.w * q1.z + (q1.x * q2.y - q1.y * q2.x);

	return *this;
}

LCXQUATERNION& LCXQUATERNION::operator *= ( FLOAT f )
{
	x *= f;	y *= f;	z *= f;	w *= f;
	return *this;
}

LCXQUATERNION& LCXQUATERNION::operator /= ( FLOAT f )
{
	FLOAT t = 1.f / f;
	x *= t;	y *= t;	z *= t;	w *= t;
	return *this;
}


// unary operators
LCXQUATERNION LCXQUATERNION::operator + () const
{
	return *this;
}

LCXQUATERNION LCXQUATERNION::operator - () const
{
	return LCXQUATERNION(-x, -y, -z, -w);
}


// binary operators
LCXQUATERNION LCXQUATERNION::operator + (const LCXQUATERNION& q ) const
{
	return LCXQUATERNION(x + q.x, y + q.y, z + q.z, w + q.w);
}

LCXQUATERNION LCXQUATERNION::operator - (const LCXQUATERNION& q ) const
{
	return LCXQUATERNION(x - q.x, y - q.y, z - q.z, w - q.w);
}

LCXQUATERNION LCXQUATERNION::operator * (const LCXQUATERNION& q ) const
{
	LCXQUATERNION q1 = *this;
	LCXQUATERNION q2 = q;

	LCXQUATERNION qT;

	qT.w = q1.w * q2.w - ( q1.x * q2.x + q1.y * q2.y + q1.z * q2.z);

	qT.x = q1.w * q2.x + q2.w * q1.x + (q1.y * q2.z - q1.z * q2.y);
	qT.y = q1.w * q2.y + q2.w * q1.y + (q1.z * q2.x - q1.x * q2.z);
	qT.z = q1.w * q2.z + q2.w * q1.z + (q1.x * q2.y - q1.y * q2.x);

	return qT;
}

LCXQUATERNION LCXQUATERNION::operator * ( FLOAT f ) const
{
	return LCXQUATERNION(x * f, y * f, z * f, w * f);
}

LCXQUATERNION LCXQUATERNION::operator / ( FLOAT f ) const
{
	FLOAT t = 1.f / f;
	return LCXQUATERNION(x * t, y * t, z * t, w * t);
}


LCXQUATERNION operator * (FLOAT f, const LCXQUATERNION& q )
{
	return LCXQUATERNION(f * q.x, f * q.y, f * q.z, f * q.w);
}


BOOL LCXQUATERNION::operator == (const LCXQUATERNION& q ) const
{
	return x == q.x && y == q.y && z == q.z && w == q.w;
}

BOOL LCXQUATERNION::operator != (const LCXQUATERNION& q ) const
{
	return x != q.x || y != q.y || z != q.z || w != q.w;
}

void LCXQUATERNION::SLerp(const LCXQUATERNION* q1, const LCXQUATERNION* q2, FLOAT t)
{
	LCXQUATERNION	q;

	//Q = (1/sinθ)[  sin (θ*(1-t)) * Q1 + sin (θ*t) * Q2] 

	float w1;
	float w2;
	float dt = q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q1->w * q2->w;


	if( dt > 0.9999f || dt<-0.9999f)
	{
		w1 = 1 - t;
		w2 = t;
	}
	else
	{ 
		float fTheta  = (float)acos( fabsf(dt) );
		float fSin    = (float)sin( fTheta );

		w1 = (float)sin( (1 - t) * fTheta) / fSin;
		w2 = (float)sin(      t  * fTheta) / fSin;		
	}

	if( dt < 0 )
		w2 = -w2;

	q = *q1 * w1 + *q2 * w2;

	*this = q;
}


void LCXQUATERNION::RotationMatrix(LCXMATRIX* pOut, BOOL bDX)
{
	float	xx = x * x;
	float	yy = y * y;
	float	zz = z * z;

	float	xy = x * y;
	float	yz = y * z;
	float	xz = z * x;

	float	xw = x * w;
	float	yw = y * w;
	float	zw = z * w;

	pOut->Identity();


	if(bDX)
	{
		// Direct3D
		pOut->_11 = 1 - 2*yy - 2*zz;	pOut->_21 =     2*xy - 2*zw;	pOut->_31 =     2*xz + 2*yw;
		pOut->_12 =     2*xy + 2*zw;	pOut->_22 = 1 - 2*xx - 2*zz;	pOut->_32 =     2*yz - 2*xw;
		pOut->_13 =     2*xz - 2*yw;	pOut->_23 =     2*yz + 2*xw;	pOut->_33 = 1 - 2*xx - 2*yy;
	}
	else
	{
		pOut->_11 = 1 - 2*yy - 2*zz;	pOut->_12 =     2*xy - 2*zw;	pOut->_13 =     2*xz + 2*yw;
		pOut->_21 =     2*xy + 2*zw;	pOut->_22 = 1 - 2*xx - 2*zz;	pOut->_23 =     2*yz - 2*xw;
		pOut->_31 =     2*xz - 2*yw;	pOut->_32 =     2*yz + 2*xw;	pOut->_33 = 1 - 2*xx - 2*yy;
	}
}




////////////////////////////////////////////////////////////////////////////////
// Color
LCXCOLOR::LCXCOLOR()
{
	r = 1;	g = 1;	b = 1;	a = 1;
}



LCXCOLOR::LCXCOLOR( DWORD dw )
{
	const FLOAT f = 1.f / 255.f;
	r = f * (FLOAT) (unsigned char) (dw >> 16);
	g = f * (FLOAT) (unsigned char) (dw >>  8);
	b = f * (FLOAT) (unsigned char) (dw >>  0);
	a = f * (FLOAT) (unsigned char) (dw >> 24);
}


LCXCOLOR::LCXCOLOR(const FLOAT* pf )
{
	r = pf[0];	g = pf[1];	b = pf[2];	a = pf[3];
}


LCXCOLOR::LCXCOLOR(const LCXCOLOR& v )
{
	r = v.r;	g = v.g;	b = v.b;	a = v.a;
}



LCXCOLOR::LCXCOLOR( FLOAT fr, FLOAT fg, FLOAT fb, FLOAT fa )
{
	r = fr;	g = fg;	b = fb;	a = fa;
}


// casting

LCXCOLOR::operator DWORD () const
{
	DWORD dwR = r >= 1.f ? 0xff : r <= 0.f ? 0x00 : (DWORD) (r * 255.f + 0.5f);
	DWORD dwG = g >= 1.f ? 0xff : g <= 0.f ? 0x00 : (DWORD) (g * 255.f + 0.5f);
	DWORD dwB = b >= 1.f ? 0xff : b <= 0.f ? 0x00 : (DWORD) (b * 255.f + 0.5f);
	DWORD dwA = a >= 1.f ? 0xff : a <= 0.f ? 0x00 : (DWORD) (a * 255.f + 0.5f);

	return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
}



LCXCOLOR::operator FLOAT * ()
{
	return (FLOAT *) &r;
}


LCXCOLOR::operator const FLOAT * () const
{
	return (const FLOAT *) &r;
}


// assignment operators
LCXCOLOR& LCXCOLOR::operator += (const LCXCOLOR& c )
{
	r += c.r;	g += c.g;	b += c.b;	a += c.a;
	return *this;
}

LCXCOLOR& LCXCOLOR::operator -= (const LCXCOLOR& c )
{
	r -= c.r;	g -= c.g;	b -= c.b;	a -= c.a;
	return *this;
}

LCXCOLOR& LCXCOLOR::operator *= ( FLOAT f )
{
	r *= f;	g *= f;	b *= f;	a *= f;
	return *this;
}

LCXCOLOR& LCXCOLOR::operator /= ( FLOAT f )
{
	FLOAT t = 1.f / f;
	r *= t;	g *= t;	b *= t;	a *= t;
	return *this;
}


// unary operators
LCXCOLOR LCXCOLOR::operator + () const
{
	return *this;
}

LCXCOLOR LCXCOLOR::operator - () const
{
	return LCXCOLOR(-r, -g, -b, -a);
}


// binary operators
LCXCOLOR LCXCOLOR::operator + (const LCXCOLOR& c ) const
{
	return LCXCOLOR(r + c.r, g + c.g, b + c.b, a + c.a);
}

LCXCOLOR LCXCOLOR::operator - (const LCXCOLOR& c ) const
{
	return LCXCOLOR(r - c.r, g - c.g, b - c.b, a - c.a);
}

LCXCOLOR LCXCOLOR::operator * ( FLOAT f ) const
{
	return LCXCOLOR(r * f, g * f, b * f, a * f);
}

LCXCOLOR LCXCOLOR::operator / ( FLOAT f ) const
{
	FLOAT t = 1.f/ f;
	return LCXCOLOR(r * t, g * t, b * t, a * t);
}


LCXCOLOR operator * (FLOAT f, const LCXCOLOR& c )
{
	return LCXCOLOR(f * c.r, f * c.g, f * c.b, f * c.a);
}


BOOL LCXCOLOR::operator == (const LCXCOLOR& c ) const
{
	return r == c.r && g == c.g && b == c.b && a == c.a;
}

BOOL LCXCOLOR::operator != (const LCXCOLOR& c ) const
{
	return r != c.r || g != c.g || b != c.b || a != c.a;
}





////////////////////////////////////////////////////////////////////////////////
// Plane

LCXPLANE::LCXPLANE()
{
	a = 0;	b = 0;	c = 0;	d = 0;
}


LCXPLANE::LCXPLANE(const FLOAT* pf )
{
	a = pf[0];	b = pf[1];	c = pf[2];	d = pf[3];
}


LCXPLANE::LCXPLANE(const LCXPLANE& v )
{
	a = v.a;	b = v.b;	c = v.c;	d = v.d;
}


LCXPLANE::LCXPLANE( FLOAT fa, FLOAT fb, FLOAT fc, FLOAT fd )
{
	a = fa;	b = fb;	c = fc;	d = fd;
}


// casting
LCXPLANE::operator FLOAT* ()
{
	return (FLOAT *) &a;
}


LCXPLANE::operator const FLOAT* () const
{
	return (const FLOAT *) &a;
}


// unary operators
LCXPLANE LCXPLANE::operator + () const
{
	return *this;
}

LCXPLANE LCXPLANE::operator - () const
{
	return LCXPLANE(-a, -b, -c, -d);
}


// binary operators
BOOL LCXPLANE::operator == (const LCXPLANE& p) const
{
	return a == p.a && b == p.b && c == p.c && d == p.d;
}

BOOL LCXPLANE::operator != (const LCXPLANE& p ) const
{
	return a != p.a || b != p.b || c != p.c || d != p.d;
}



void LCXPLANE::SetupFromPointNormal(const LCXVECTOR3* point, const LCXVECTOR3* normal)
{
	this->a  = normal->x;
	this->b  = normal->y;
	this->c  = normal->z;
	this->d  = - *point * *normal;
}


void LCXPLANE::SetupFromPoints(const LCXVECTOR3* p0, const LCXVECTOR3* p1, const LCXVECTOR3* p2)
{
	LCXVECTOR3	n = (*p1 - *p0) ^ (*p2 - *p0);
	n.Normalize();

	this->a  = n.x;
	this->b  = n.y;
	this->c  = n.z;
	this->d  = - *p0 * n;
}





LCXRECT::LCXRECT()								{	x0 = y0 = x1 = y1 = 0.f;										}
LCXRECT::LCXRECT(const FLOAT*_m)				{	if(!_m){x0=y0=x1=y1=0;}else{x0=_m[0];y0=_m[1];x1=_m[2];y1=_m[3];}}
LCXRECT::LCXRECT(const LCXRECT& _rh )			{	x0 = _rh.x0; y0 = _rh.y0; x1 = _rh.x1;	y1 = _rh.y1;			}

LCXRECT::LCXRECT( FLOAT fx0,FLOAT fy0,FLOAT fx1,FLOAT fy1)
{	x0 = fx0;	y0 = fy0;	x1 = fx1;	y1 = fy1;				}

LCXRECT::LCXRECT( D3DXVECTOR2 p0, D3DXVECTOR2 p1){	x0 = p0.x;	y0 = p0.y;	x1 = p1.x;	y1 = p1.y;					}

D3DXVECTOR2  LCXRECT::Get00()	 const			{	return D3DXVECTOR2(x0, y0);										}
D3DXVECTOR2  LCXRECT::Get10()	 const			{	return D3DXVECTOR2(x1, y0);										}
D3DXVECTOR2  LCXRECT::Get01()	 const			{	return D3DXVECTOR2(x0, y1);										}
D3DXVECTOR2  LCXRECT::Get11()	 const			{	return D3DXVECTOR2(x1, y1);										}

RECT   LCXRECT::GetRECT()						{	RECT r={LONG(x0),LONG(y0),LONG(x1),LONG(y1)};	return r;		}
D3DRECT LCXRECT::GetRectD3()					{	D3DRECT r={LONG(x0),LONG(y0),LONG(x1), LONG(y1)};	return r;	}

FLOAT	LCXRECT::GetWidth()						{	return (x1 - x0);												}
FLOAT	LCXRECT::GetHeight()					{	return (y1 - y0);												}
D3DXVECTOR2	LCXRECT::GetCenter() const			{	return D3DXVECTOR2((x0 + x1) / 2.f,	(y0 + y1) / 2.f);			}

// casting
LCXRECT::operator FLOAT*()						{	return (FLOAT *) &x0;											}
LCXRECT::operator const FLOAT*() const			{	return (const FLOAT *) &x0;										}

//const LCXRECT& LCXRECT::operator=(const LCXRECT& v){	x0 = v.x0; y0 = v.y0; x1 = v.x1; y1 = v.y1; return *this;	}

bool LCXRECT::operator==(const LCXRECT& v) const{	return x0 == v.x0 && y0 == v.y0 && x1 == v.x1 && y1 == v.y1;	}
bool LCXRECT::operator!=(const LCXRECT& v) const{	return x0 != v.x0 || y0 != v.y0 || x1 != v.x1 || y0 != v.y1;	}

// assignment operators
LCXRECT& LCXRECT::operator+=(const LCXRECT& v)	{	x0 += v.x0;	y0 += v.y0;	x1 += v.x1;	y1 += v.y1;	return *this;	}
LCXRECT& LCXRECT::operator-=(const LCXRECT& v)	{	x0 -= v.x0;	y0 -= v.y0;	x1 -= v.x1;	y1 -= v.y1;	return *this;	}
LCXRECT& LCXRECT::operator*=(FLOAT f )			{	x0 *= f;	y0 *= f;	x1 *= f;	y1 *= f;	return *this;	}
LCXRECT& LCXRECT::operator/=(FLOAT f )			{	FLOAT t=1.f/f; x0*=t; y0*=t; x1*=t;	y1 *= t;	return *this;	}

// unary operators
LCXRECT LCXRECT::operator+() const				{	return *this;													}
LCXRECT LCXRECT::operator-() const				{	return LCXRECT(-x0, -y0, -x1, -y1);								}

// binary operators
LCXRECT LCXRECT::operator+(const LCXRECT& v)const{	return LCXRECT(x0 + v.x0, y0 + v.y0, x1 + v.x1, y1 + v.y1);		}
LCXRECT LCXRECT::operator-(const LCXRECT& v)const{	return LCXRECT(x0 - v.x0, y0 - v.y0, x1 - v.x1, y1 - v.y1);		}
LCXRECT LCXRECT::operator*(FLOAT f) const		{	return LCXRECT(x0 * f, y0 * f, x1 * f, y1 * f);					}
LCXRECT LCXRECT::operator/(FLOAT f) const		{	FLOAT t=1.f/f; return LCXRECT(x0*t, y0*t, x1*t, y1*t);			}


LCXRECT LCXRECT::operator+(const D3DXVECTOR2& v){	return LCXRECT(x0 + v.x, y0 + v.y, x1 + v.x, y1 + v.y);			}
LCXRECT LCXRECT::operator-(const D3DXVECTOR2& v){	return LCXRECT(x0 - v.x, y0 - v.y, x1 - v.x, y1 - v.y);			}
LCXRECT LCXRECT::operator+=(const D3DXVECTOR2& v){	return LCXRECT(x0 += v.x, y0 += v.y, x1 += v.x, y1 += v.y);		}
LCXRECT LCXRECT::operator-=(const D3DXVECTOR2& v){	return LCXRECT(x0 -= v.x, y0 -= v.y, x1 -= v.x, y1 -= v.y);		}

bool LCXRECT::IsValid() const
{
	FLOAT tx = x1 - x0;
	FLOAT ty = y1 - y0;
	
	return !(tx <= 0.f || ty <= 0.f);
}


bool LCXRECT::IsCollided(const D3DXVECTOR2& v) const
{
	return	x0 <= v.x && y0 <= v.y &&
			x1 >= v.x && y1 >= v.y;
}

bool LCXRECT::IsCollided(const LCXRECT& v) const
{
	return (x0 < v.x1 && x1 > v.x0 &&
			y0 < v.y1 && y1 > v.y0);
}

void LCXRECT::ClipAgainst(const LCXRECT& v)
{
	if (v.x1 < x1)
		x1 = v.x1;

	if (v.y1 < y1)
		y1 = v.y1;
	
	if (v.x0 > x0)
		x0 = v.x0;

	if (v.y0 > y0)
		y0 = v.y0;
	
	if (y0 > y1)
		y0 = y1;

	if (x0 > x1)
		x0 = x1;
}

void LCXRECT::SetCorrection()
{
	if(x1 < x0)
	{
		FLOAT _t = x1;
		x1 = x0;
		x0 = _t;
	}
	
	if (y1 < y0)
	{
		FLOAT _t = y1;
		y1 = y0;
		y0 = _t;
	}
}


LCXLINE::LCXLINE(){}
LCXLINE::LCXLINE(const FLOAT*_m)		{	if(_m){m[0]=_m[0];m[1]=_m[1];m[2]=_m[2];m[3]=_m[3];m[4]=_m[4];m[5]=_m[5];}else{m[0]=m[1]=m[2]=m[3]=m[4]=m[5]=0;}}
LCXLINE::LCXLINE(const LCXLINE& rhs)	{	p = rhs.p; t = rhs.t;	}
LCXLINE::LCXLINE(FLOAT Px,FLOAT Py,FLOAT Pz,FLOAT Tx,FLOAT Ty,FLOAT Tz) : p(Px,Py,Pz), t(Tx,Ty,Tz){}
LCXLINE::LCXLINE(const D3DXVECTOR3& P, const D3DXVECTOR3& T) : p(P), t(T){}

LCXLINE::operator FLOAT*()						{	return (FLOAT *) &p.x;			}
LCXLINE::operator const FLOAT*() const			{	return (const FLOAT *) &p.x;	}
LCXLINE LCXLINE::operator+() const				{	return LCXLINE(p.x, p.y, p.z, t.x, t.y, t.z);		}
LCXLINE LCXLINE::operator-() const				{	return LCXLINE(-p.x, -p.y, -p.z, -t.x, -t.y, -t.z);	}
bool LCXLINE::operator==(const LCXLINE& v) const{	return p.x == v.p.x && p.y == v.p.y && p.z == v.p.z && t.x == v.t.x && t.y == v.t.y && t.z == v.t.z;	}
bool LCXLINE::operator!=(const LCXLINE& v) const{	return p.x != v.p.x || p.y != v.p.y || p.z != v.p.z || t.x != v.t.x || t.y != v.t.y || t.z != v.t.z;	}


LCXSPHERE::LCXSPHERE(){}
LCXSPHERE::LCXSPHERE(const FLOAT*_m)			{	if(!_m){m[0]=m[1]=m[2]=m[3]=0;}else{m[0]=_m[0];m[1]=_m[1];m[2]=_m[2];m[3]=_m[3];}}
LCXSPHERE::LCXSPHERE(const LCXSPHERE& _rh)		{	p = _rh.p; r = _rh.r;	}
LCXSPHERE::LCXSPHERE( FLOAT X,FLOAT Y,FLOAT Z,FLOAT R){	m[0]=X;m[1]=Y;m[2]=Z;m[3]=R;}

LCXSPHERE::operator FLOAT*()					{	return (FLOAT *) &p.x;				}
LCXSPHERE::operator const FLOAT*() const		{	return (const FLOAT *) &p.x;		}
LCXSPHERE LCXSPHERE::operator+() const			{	return *this;													}
LCXSPHERE LCXSPHERE::operator-() const			{	return LCXSPHERE(-p.x, -p.y, -p.z, -r);								}
bool LCXSPHERE::operator==(const LCXSPHERE& v) const{	return p.x == v.p.x && p.y == v.p.y && p.z == v.p.z && r == v.r;	}
bool LCXSPHERE::operator!=(const LCXSPHERE& v) const{	return p.x != v.p.x || p.y != v.p.y || p.z != v.p.z || p.y != v.r;	}












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


LCXCUBEH::LCXCUBEH()
{
}

LCXCUBEH::LCXCUBEH(const VEC3* _m)
{
	if(_m)
	{
		_0 = _m[0];
		_1 = _m[1];
		_2 = _m[2];
		_3 = _m[3];
		_4 = _m[4];
		_5 = _m[5];
		_6 = _m[6];
		_7 = _m[7];
	}
	else
	{
		_0.x = 0; _0.y = 0; _0.z = 0;
		_1.x = 0; _1.y = 0; _1.z = 0;
		_2.x = 0; _2.y = 0; _2.z = 0;
		_3.x = 0; _3.y = 0; _3.z = 0;
		_4.x = 0; _4.y = 0; _4.z = 0;
		_5.x = 0; _5.y = 0; _5.z = 0;
		_6.x = 0; _6.y = 0; _6.z = 0;
		_7.x = 0; _7.y = 0; _7.z = 0;
	}
}

LCXCUBEH::LCXCUBEH(	  const VEC3& __0
			, const VEC3& __1
			, const VEC3& __2
			, const VEC3& __3
			, const VEC3& __4
			, const VEC3& __5
			, const VEC3& __6
			, const VEC3& __7)
{
	_0 = __0;	_1 = __1;	_2 = __2;	_3 = __3;
	_4 = __4;	_5 = __5;	_6 = __6;	_7 = __7;
}


LCXCUBEH::operator FLOAT*()
{
	return (FLOAT *) &_0.x;
}

LCXCUBEH::operator const FLOAT*() const
{
	return (const FLOAT *) &_0.x;
}


BOOL LCXCUBEH::operator==(const LCXCUBEH& v) const
{
	return	_0 == v._0 &&
			_1 == v._1 &&
			_2 == v._2 &&
			_3 == v._3 &&
			_4 == v._4 &&
			_5 == v._5 &&
			_6 == v._6 &&
			_7 == v._7;
}

BOOL LCXCUBEH::operator !=(const LCXCUBEH& v) const
{
	return	_0 != v._0 ||
			_1 != v._1 ||
			_2 != v._2 ||
			_3 != v._3 ||
			_4 != v._4 ||
			_5 != v._5 ||
			_6 != v._6 ||
			_7 != v._7;
}

void LCXCUBEH::Set(const VEC3* _m)
{
	if(_m)
	{
		_0 = _m[0];
		_1 = _m[1];
		_2 = _m[2];
		_3 = _m[3];
		_4 = _m[4];
		_5 = _m[5];
		_6 = _m[6];
		_7 = _m[7];
	}
	else
	{
		_0.x = 0; _0.y = 0; _0.z = 0;
		_1.x = 0; _1.y = 0; _1.z = 0;
		_2.x = 0; _2.y = 0; _2.z = 0;
		_3.x = 0; _3.y = 0; _3.z = 0;
		_4.x = 0; _4.y = 0; _4.z = 0;
		_5.x = 0; _5.y = 0; _5.z = 0;
		_6.x = 0; _6.y = 0; _6.z = 0;
		_7.x = 0; _7.y = 0; _7.z = 0;
	}
}

void LCXCUBEH::Set(const LCXCUBEH& r)
{
	_0 = r._0;
	_1 = r._1;
	_2 = r._2;
	_3 = r._3;
	_4 = r._4;
	_5 = r._5;
	_6 = r._6;
	_7 = r._7;
}









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


LCXCUBEAA::LCXCUBEAA()
	: vcMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, vcMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
{
}

LCXCUBEAA::LCXCUBEAA(const VEC3* _m)
{
	if(_m)
	{
		vcMin=_m[0];
		vcMax=_m[1];
	}
	else
	{
		vcMin = VEC3(FLT_MAX,FLT_MAX,FLT_MAX);
		vcMax = VEC3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	}
}

LCXCUBEAA::LCXCUBEAA(const VEC3& _min, const VEC3& _max)
	: vcMin(_min)
	, vcMax(_max)
{
	
}

LCXCUBEAA::LCXCUBEAA(const VEC3& vc0)
	: vcMin(vc0)
	, vcMax(vc0)
{
}


LCXCUBEAA::LCXCUBEAA(FLOAT* _p)
{
	vcMin.x =_p[0]; vcMin.y =_p[1]; vcMin.z =_p[2];
	vcMax.x =_p[3]; vcMax.y =_p[4]; vcMax.z =_p[5];
}


LCXCUBEAA::LCXCUBEAA(FLOAT x0, FLOAT y0, FLOAT z0, FLOAT x1, FLOAT y1, FLOAT z1)
	: vcMin(x0, y0, z0)
	, vcMax(x1, y1, z1)
{
}

void LCXCUBEAA::AddPoint(const VEC3* pV)
{
	AddPoint(pV->x, pV->y, pV->z);
}

void LCXCUBEAA::AddPoint(FLOAT x, FLOAT y, FLOAT z)
{
	if (x>vcMax.x) vcMax.x = x;
	if (y>vcMax.y) vcMax.y = y;
	if (z>vcMax.z) vcMax.z = z;
	
	if (x<vcMin.x) vcMin.x = x;
	if (y<vcMin.y) vcMin.y = y;
	if (z<vcMin.z) vcMin.z = z;
}

void LCXCUBEAA::AddPoint(const LCXCUBEAA* b)
{
	AddPoint(&b->vcMax);
	AddPoint(&b->vcMin);
}

void LCXCUBEAA::Set(FLOAT x, FLOAT y, FLOAT z)
{
	vcMax =VEC3(x,y,z);
	vcMin = vcMax;
}


void LCXCUBEAA::Set(const VEC3* pV)
{
	vcMax = *pV;
	vcMin = *pV;
}


void LCXCUBEAA::Set(const LCXCUBEAA& pV)
{
	*this = pV;
}


BOOL LCXCUBEAA::IsInside(const VEC3* pV)
{
	return (pV->x >= vcMin.x && pV->x <= vcMax.x &&
			pV->y >= vcMin.y && pV->y <= vcMax.y &&
			pV->z >= vcMin.z && pV->z <= vcMax.z);
}


VEC3 LCXCUBEAA::GetCenter() const
{
	return (vcMin + vcMax) / 2.f;
}

VEC3 LCXCUBEAA::GetExtent() const
{
	return vcMax - vcMin;
}


void LCXCUBEAA::GetEdge(VEC3* pvcEdge)
{
	FLOAT fX = vcMax.x - vcMin.x;
	FLOAT fY = vcMax.y - vcMin.y;
	FLOAT fZ = vcMax.z - vcMin.z;
	
	pvcEdge[0] = vcMin;
	pvcEdge[1] = vcMin;	pvcEdge[1].y +=fY;
	pvcEdge[2] = vcMin;	pvcEdge[2].x +=fX;
	pvcEdge[3] = vcMax;	pvcEdge[3].z -=fZ;
	pvcEdge[4] = vcMin;	pvcEdge[4].z +=fZ;
	pvcEdge[5] = vcMax;	pvcEdge[5].x -=fX;
	pvcEdge[6] = vcMax;	pvcEdge[6].y -=fY;
	pvcEdge[7] = vcMax;

}


void LCXCUBEAA::GetLine(VEC3* pvcLine)
{
	VEC3 vcEdge[8];

	this->GetEdge(vcEdge);

	pvcLine[ 0] = vcEdge[0], pvcLine[ 1] = vcEdge[1];
	pvcLine[ 2] = vcEdge[0], pvcLine[ 3] = vcEdge[2];
	pvcLine[ 4] = vcEdge[1], pvcLine[ 5] = vcEdge[3];
	pvcLine[ 6] = vcEdge[2], pvcLine[ 7] = vcEdge[3];
	
	pvcLine[ 8] = vcEdge[4], pvcLine[ 9] = vcEdge[5];
	pvcLine[10] = vcEdge[4], pvcLine[11] = vcEdge[6];
	pvcLine[12] = vcEdge[5], pvcLine[13] = vcEdge[7];
	pvcLine[14] = vcEdge[6], pvcLine[15] = vcEdge[7];

	pvcLine[16] = vcEdge[0], pvcLine[17] = vcEdge[4];
	pvcLine[18] = vcEdge[1], pvcLine[19] = vcEdge[5];
	pvcLine[20] = vcEdge[3], pvcLine[21] = vcEdge[7];
	pvcLine[22] = vcEdge[2], pvcLine[23] = vcEdge[6];
}

BOOL LCXCUBEAA::IsEmpty() const
{
	VEC3 d = vcMin - vcMax;
	if (d.x < 0) d.x = -d.x;
	if (d.y < 0) d.y = -d.y;
	if (d.z < 0) d.z = -d.z;
	
	return (d.x < LC_ROUNDING_DELTA &&
		d.y < LC_ROUNDING_DELTA &&
		d.z < LC_ROUNDING_DELTA);
}

void LCXCUBEAA::Repair()
{
	FLOAT t;
	
	if (vcMin.x > vcMax.x)
	{
		t=vcMin.x; vcMin.x = vcMax.x; vcMax.x=t;
	}
	
	if (vcMin.y > vcMax.y)
	{
		t=vcMin.y; vcMin.y = vcMax.y; vcMax.y=t;
	}
	
	if (vcMin.z > vcMax.z)
	{
		t=vcMin.z; vcMin.z = vcMax.z; vcMax.z=t;
	}
}





////////////////////////////////////////////////////////////////////////////////

LCXTRI::LCXTRI()
{
}
	
LCXTRI::LCXTRI(const VEC3*_m)
{
	if(_m)
	{
		_0=_m[0];
		_1=_m[1];
		_2=_m[2];
	}
	else
	{
		_0.x =0; _0.y =0; _0.z =0;
		_1.x =0; _1.y =0; _1.z =0;
		_2.x =0; _2.y =0; _2.z =0;
	}
}

LCXTRI::LCXTRI(const LCXTRI& _r)
{
	_0 = _r._0;
	_1 = _r._1;
	_2 = _r._2;
}

LCXTRI::LCXTRI(const FLOAT* _p)
{
	_0.x =_p[0]; _0.y =_p[1]; _0.z =_p[2];
	_1.x =_p[3]; _1.y =_p[4]; _1.z =_p[5];
	_2.x =_p[6]; _2.y =_p[7]; _2.z =_p[8];
}

LCXTRI::LCXTRI(VEC3 __0,VEC3 __1,VEC3 __2)
{
	_0 = __0;
	_1 = __1;
	_2 = __2;
}


LCXTRI::LCXTRI(FLOAT _0x,FLOAT _0y,FLOAT _0z
	, FLOAT _1x,FLOAT _1y,FLOAT _1z
	, FLOAT _2x,FLOAT _2y,FLOAT _2z)
{
	_0.x =_0x; _0.y =_0y; _0.z =_0z;
	_1.x =_1x; _1.y =_1y; _1.z =_1z;
	_2.x =_2x; _2.y =_2y; _2.z =_2z;
}



void LCXTRI::Set(VEC3 __0, VEC3 __1, VEC3 __2)
{
	_0 = __0;
	_1 = __1;
	_2 = __2;
}


// casting
LCXTRI::operator FLOAT*()
{
	return (FLOAT *) &_0.x;
}

LCXTRI::operator const FLOAT*() const
{
	return (const FLOAT *) &_0.x;
}

bool LCXTRI::operator==(const LCXTRI& v) const
{
	return	   _0.x == v._0.x && _0.y == v._0.y && _0.z == v._0.z
			&& _1.x == v._1.x && _1.y == v._1.y && _1.z == v._1.z
			&& _2.x == v._2.x && _2.y == v._2.y && _2.z == v._2.z
			;
}

bool LCXTRI::operator!=(const LCXTRI& v) const
{
	return	   _0.x != v._0.x || _0.y != v._0.y || _0.z != v._0.z
			|| _1.x != v._1.x || _1.y != v._1.y || _1.z != v._1.z
			|| _2.x != v._2.x || _2.y != v._2.y || _2.z != v._2.z
			;
}






////////////////////////////////////////////////////////////////////////////////

TpckTri::TpckTri()
{
	nId	= 0xFFFFFFFF;
	nAt	= 0xFFFFFFFF;
}


TpckTri::TpckTri(FLOAT fR, VEC3* vcC, VEC3* vcT0, VEC3* vcT1, VEC3* vcT2)
{
	nId	= 0xFFFFFFFF;
	nAt	= 0xFFFFFFFF;

	fStlSrtR	= fR;
	
	if(vcC)	vcPck	= *vcC;
	if(vcT0)	p0	= *vcT0;
	if(vcT1)	p1	= *vcT1;
	if(vcT2)	p2	= *vcT2;
}

TpckTri::TpckTri(INT nIdx, INT nAttrib, FLOAT fR, VEC3* vcC, VEC3* vcT0, VEC3* vcT1, VEC3* vcT2)
{
	 nId	= nIdx;
	 nAt	= nAttrib;

	fStlSrtR	= fR;
	
	if(vcC)	vcPck	= *vcC;
	if(vcT0)	p0	= *vcT0;
	if(vcT1)	p1	= *vcT1;
	if(vcT2)	p2	= *vcT2;
}

FLOAT TpckTri::GetBoundRadius()
{
	return fStlSrtR;
}

VEC3 TpckTri::GetBoundCenter()
{
	return vcPck;
}



//
// Cylinder
//
////////////////////////////////////////////////////////////////////////////////

LCXCYLINDER::LCXCYLINDER(){}

LCXCYLINDER::LCXCYLINDER(const FLOAT*_m)
{
	if(_m)
	{
		m[0]=_m[0];
		m[1]=_m[1];
		m[2]=_m[2];
		m[3]=_m[3];
		m[4]=_m[4];
		m[5]=_m[5];
		m[6]=_m[6];
		m[7]=_m[7];		
	}
	else
	{
		m[0]=m[1]=m[2]=m[3]=m[4]=m[5]=m[6]=m[7]=0;
	}
}

LCXCYLINDER::LCXCYLINDER(const LCXCYLINDER& v)
{
	p = v.p;
	t = v.t;
	r = v.r;
	h = v.h;
}

LCXCYLINDER::LCXCYLINDER(FLOAT Px,FLOAT Py,FLOAT Pz,FLOAT Tx,FLOAT Ty,FLOAT Tz, FLOAT R, FLOAT H)
{
	p.x = Px;
	p.y = Py;
	p.z = Pz;
	t.x = Tx;
	t.y = Ty;
	t.z = Tz;
	r	= R;
	h	= H;
}

LCXCYLINDER::LCXCYLINDER(const VEC3& P, const VEC3& T, FLOAT R, FLOAT H)
{
	p = P;
	t = T;
	r = R;
	h = H;
}

LCXCYLINDER::operator FLOAT*()
{
	return (FLOAT *) &p.x;
}

LCXCYLINDER::operator const FLOAT*() const
{
	return (const FLOAT *) &p.x;
}

bool LCXCYLINDER::operator==(const LCXCYLINDER& v) const
{
	return	p.x == v.p.x && p.y == v.p.y && p.z == v.p.z &&
			t.x == v.t.x && t.y == v.t.y && t.z == v.t.z &&
			r == v.r &&
			h == v.h;
}

bool LCXCYLINDER::operator!=(const LCXCYLINDER& v) const
{
	return	p.x != v.p.x || p.y != v.p.y || p.z != v.p.z ||
			t.x != v.t.x || t.y != v.t.y || t.z != v.t.z ||
			r != v.r ||
			h != v.h;
}



void LCXCYLINDER::Set(const VEC3& p0, const VEC3& p1, FLOAT R, FLOAT H)
{
	p = p0;
	t = p1 - p;
	r = R;
	h = H;

	D3DXVec3Normalize(&t, &t);
}








////////////////////////////////////////////////////////////////////////////////

INT LcMath_MakePower2(INT a)
{
	int n=0;
	int t=a;

	while(t)
	{
		t>>=1;
		++n;
	}

	return (0x1<<(n-1) ^ a) ? 0x1<<n : 0x1<<(n-1);
};


void LcMath_InterpolateFloat2(FLOAT* pOut, FLOAT* p1, FLOAT* p2, FLOAT d)
{
	FLOAT inv = 1.f - d;
	
	pOut[0] = p1[0] * inv + p2[0] * d;
	pOut[1] = p1[1] * inv + p2[1] * d;
}


void LcMath_InterpolateFloat3(FLOAT* pOut, FLOAT* p1, FLOAT* p2, FLOAT d)
{
	FLOAT inv = 1.f - d;
	
	pOut[0] = p1[0] * inv + p2[0] * d;
	pOut[1] = p1[1] * inv + p2[1] * d;
	pOut[2] = p1[2] * inv + p2[2] * d;
}

void LcMath_InterpolateFloat4(FLOAT* pOut, FLOAT* p1, FLOAT* p2, FLOAT d)
{
	FLOAT inv = 1.f - d;
	
	pOut[0] = p1[0] * inv + p2[0] * d;
	pOut[1] = p1[1] * inv + p2[1] * d;
	pOut[2] = p1[2] * inv + p2[2] * d;
	pOut[3] = p1[3] * inv + p2[3] * d;
}


void LcMath_InterpolateFloat(FLOAT* pOut, FLOAT* p1, FLOAT* p2, FLOAT d, INT iCnt)
{
	FLOAT inv = 1.f - d;
	
	for(int i=0; i<iCnt; ++i)
		pOut[i] = p1[i] * inv + p2[i] * d;
}

INT	LcMath_Equals(FLOAT a, FLOAT b)
{
	return (
				(a + LC_ROUNDING_DELTA > b) &&
				(a - LC_ROUNDING_DELTA < b)
			) ? 1: -1;
}





FLOAT LcMath_Vec3Length(FLOAT *pV)
{
#ifdef __cplusplus
    return sqrtf(pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2]);
#else
    return (FLOAT) sqrt(pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2]);
#endif
}

FLOAT LcMath_Vec3LengthSq(FLOAT *pV)
{
	return  pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2];
}


FLOAT LcMath_Vec3Dot(FLOAT *pV1, FLOAT *pV2 )
{
	return  pV1[0] * pV2[0] + pV1[1] * pV2[1] + pV1[2] * pV2[2];
}


void	LcMath_Vec3Cross(FLOAT* pOut, FLOAT *pV1, FLOAT *pV2 )
{
	pOut[0] = pV1[1] * pV2[2] - pV1[2] * pV2[1];
	pOut[1] = pV1[2] * pV2[0] - pV1[0] * pV2[2];
	pOut[2] = pV1[0] * pV2[1] - pV1[1] * pV2[0];
}


void LcMath_Vec3NormalVector(VEC3* pOut, VEC3* p1, VEC3* p2, VEC3* p3)
{
	VEC3 a, b;

	a = *p2 - *p1;
	b = *p3 - *p1;

	D3DXVec3Cross(pOut, &a, &b);
	D3DXVec3Normalize(pOut, pOut);
}



void	LcMath_Vec3Normalize(FLOAT* pOut)
{
	FLOAT fL = LcMath_Vec3Length(pOut);

	pOut[1] /= fL;
	pOut[1] /= fL;
	pOut[2] /= fL;
}



void LcMath_Vec3Minimize(FLOAT *pOut, FLOAT *pV1, FLOAT *pV2 )
{
	pOut[0] = (pV1[0] < pV2[0]) ? pV1[0] : pV2[0];
    pOut[1] = (pV1[1] < pV2[1]) ? pV1[1] : pV2[1];
    pOut[2] = (pV1[2] < pV2[2]) ? pV1[2] : pV2[2];
}


void LcMath_Vec3Maximize(FLOAT *pOut, FLOAT *pV1, FLOAT *pV2 )
{
	pOut[0] = (pV1[0] > pV2[0]) ? pV1[0] : pV2[0];
    pOut[1] = (pV1[1] > pV2[1]) ? pV1[1] : pV2[1];
    pOut[2] = (pV1[2] > pV2[2]) ? pV1[2] : pV2[2];
}


void LcMath_Mat4X4Identity(FLOAT* pM)
{
	pM[ 0] = 1.f;	pM[ 1] = 0.f;	pM[ 2] = 0.f;	pM[ 3] = 0.f;
	pM[ 4] = 0.f;	pM[ 5] = 1.f;	pM[ 6] = 0.f;	pM[ 7] = 0.f;
	pM[ 8] = 0.f;	pM[ 9] = 0.f;	pM[10] = 1.f;	pM[11] = 0.f;
	pM[12] = 0.f;	pM[13] = 0.f;	pM[14] = 0.f;	pM[15] = 1.f;
}


FLOAT LcMath_QuatLength(FLOAT *pV/*x,y,z,w*/)
{
#ifdef __cplusplus
    return sqrtf(pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2] + pV[3] * pV[3]);
#else
    return (FLOAT)sqrt(pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2] + pV[3] * pV[3]);
#endif
}


FLOAT LcMath_QuatLengthSq(FLOAT *pV/*x,y,z,w*/)
{
	return (pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2] + pV[3] * pV[3]);
}


FLOAT LcMath_QuaternionDot(FLOAT *pV1, FLOAT *pV2)
{
	return (pV1[0] * pV2[0] + pV1[1] * pV2[1] + pV1[2] * pV2[2] + pV1[3] * pV2[3]);
}


void LcMath_QuaternionIdentity(FLOAT *pOut)
{
	pOut[0] = 0.f;
	pOut[1] = 0.f;
	pOut[2] = 0.f;
	pOut[3] = 1.f;
}




INT LcMath_LineCross2D(VEC2 * p)
{
	VEC2 L1 = p[1] - p[0];
	VEC2 L2 = p[3] - p[2];
	VEC2 L3 = p[2] - p[0];
	
	
	FLOAT fAlpha = L2.x * L1.y - L2.y * L1.x;
	
	if(0.f == fAlpha)
		return -1;
	
	FLOAT fBeta = fAlpha;
	
	fAlpha = (L2.x * L3.y - L2.y * L3.x)/fAlpha;
	
	if( (0.f > fAlpha) || (fAlpha > 1.f) )
		return -1;
	
	fBeta = (L2.x * L3.y - L1.y * L3.x)/fBeta;
	
	if( (0.f > fAlpha) || (fAlpha > 1.f) )
		return -1;
	
	return 0;
}

INT LcMath_Point3DtoScreen(	VEC3* pOut				// Output
						  ,	const VEC3* vcIn		// In put
						  , const VEC3* vcCam		// Camera pos
						  ,	const VEC3*	vcZ			// Camera Z vector
						  , const MATA*	mtVP		// View matrix * Projection Matrix
						  , FLOAT fScnW				// Screen Width
						  , FLOAT fScnH				// Screen Height
				  )
{
	FLOAT	fNear=1.f;
	FLOAT	beta;
	VEC3	vcB;
	VEC3	vcT	= (*vcIn - *vcCam);
	
	beta	= D3DXVec3Dot(vcZ, &vcT);
	
	if(beta<=fNear)
		return -1;			// 프로젝션 평면 뒤에 있음.

	beta = fNear/beta;
	vcB = beta * vcT;
	vcB -= fNear * (*vcZ);
	
	pOut->x	= vcB.x * mtVP->_11 + vcB.y * mtVP->_21 + vcB.z * mtVP->_31;			// 다음 Model view Matrix와 연산한다.
	pOut->y	= vcB.x * mtVP->_12 + vcB.y * mtVP->_22 + vcB.z * mtVP->_32;
	pOut->z	= vcB.x * mtVP->_13 + vcB.y * mtVP->_23 + vcB.z * mtVP->_33;

	pOut->x	=  fScnW * (pOut->x +1.f) * 0.5f;
	pOut->y	= -fScnH * (pOut->y -1.f) * 0.5f;

	return 0;
}





INT LcMath_MousePositionTo3D(VEC3* vcOut, MATA matView, MATA matProj, FLOAT fScnW, FLOAT fScnH, POINT ptMouse)
{
	MATA matViewProj;
	MATA matViewInv;
	
	MATA matViewProjInv;
	
	D3DXMatrixIdentity(&matViewProj);
	
	matViewProj = matView * matProj;
	D3DXMatrixInverse(&matViewProjInv, NULL, &matViewProj);
	
	
	VEC3 vecBefore( 2.f * ptMouse.x/fScnW - 1,  -2.f * ptMouse.y/fScnH + 1,	0);
	
	VEC3 vecP(0,0,0);
	
	
	// 다음 Model view Matrix와 연산한다.
	
	vecP.x = vecBefore.x * matViewProjInv._11 + vecBefore.y * matViewProjInv._21 + vecBefore.z * matViewProjInv._31;
	vecP.y = vecBefore.x * matViewProjInv._12 + vecBefore.y * matViewProjInv._22 + vecBefore.z * matViewProjInv._32;
	vecP.z = vecBefore.x * matViewProjInv._13 + vecBefore.y * matViewProjInv._23 + vecBefore.z * matViewProjInv._33;
	
	//camera position
	D3DXMatrixInverse(&matViewInv, NULL, &matView);
	VEC3 vecCamPos(matViewInv._41, matViewInv._42, matViewInv._43);
	
	//Normalvector
	VEC3 vecZ( matView._13, matView._23, matView._33);
	
	FLOAT beta =(vecZ.y + vecP.y);
	
	if (0.f == beta)
		return -1;
	
	beta =  -vecCamPos.y/beta;
	
	*vcOut = beta * (vecZ + vecP) + vecCamPos;
	
	return 0;
}




FLOAT LcMath_PointToPlaneDistance(FLOAT* pPn/*VECTOR4*/, FLOAT* p1 /*VECTOR3*/)
{
	return D3DXPlaneDotCoord((DPLN*)pPn, (VEC3*)p1);
}


FLOAT LcMath_LineToPointDistance(const LCXLINE* pL, const D3DXVECTOR3* pP)
{
	D3DXVECTOR3 L;
	D3DXVECTOR3 N;
	L = *pP - pL->p;
	D3DXVec3Cross(&N, &L, &pL->t);

	return D3DXVec3Length(&N);
}



INT	LcMath_LineToSphere(VEC3* pOut,FLOAT* fDst, const LCXLINE* pLine,const VEC3* pC,const FLOAT fR)
{
	VEC3	vcP;
	VEC3	vcT;
	FLOAT	fT;
	FLOAT	fN;

	vcT = *pC - pLine->p;
	fT = D3DXVec3Dot(&pLine->t, &vcT);

	vcP = pLine->p + fT * pLine->t;

	fN = D3DXVec3LengthSq(&vcT);
	fN -= fT * fT;

	if(fDst)
		*fDst = sqrtf(fN);

	if(pOut)
		*pOut = vcP;

	if(fN < fR*fR)
		return 0;
	
	return -1;
}





//FLOAT LcMath_LineToLineDistance(const LCXLINE* pL1, const LCXLINE* pL2)
//{
//	D3DXVECTOR3	n;
//	D3DXVec3Cross(&n, &pL1->t, &pL2->t);
//	D3DXVec3Normalize(&n,&n);
//
//	return fabs(D3DXVec3Dot(&(pL2->p - pL1->p), &n));
//}



// 두직선 사이의 최단 거리 구하기
FLOAT	LcMath_LineToLineDistance(																			// 직선과 직선 사이의 최단 거리
									FLOAT* pSinTheta														// 직선과 직선 사이의 각도
								,	VEC3* pOut																// 직선과 직선사이의 노말벡터
								,	const LCXLINE* pV1														// 직선 1
								,	const LCXLINE* pV2														// 직선 2
								)
{
	VEC3	vcN;
	VEC3	vcL1 = pV1->t;
	VEC3	vcL2 = pV2->t;
	VEC3	vcP1P2=pV1->p - pV2->p;

	FLOAT	fD =0.f;
	FLOAT	fL = 0.f;
	
	D3DXVec3Cross(&vcN, &vcL1, &vcL2);

	fL = D3DXVec3Length(&vcN);

	if(pSinTheta)
		*pSinTheta = fL;

	fL = 1.f/fL;

	vcN.x *= fL;
	vcN.y *= fL;
	vcN.z *= fL;

	fD = D3DXVec3Dot(&vcN, &vcP1P2);

	*pOut = vcN;

	return fD;
}




// 두 직선 사이의 상호 작용
INT LcMath_LineToLineIntersection(FLOAT* pSinTheta, FLOAT* pDst,VEC3* pOut,const LCXLINE* pV1,const LCXLINE* pV2)
{
	VEC3	vcN;
	VEC3	vcL1 = pV1->t;
	VEC3	vcL2 = pV2->t;
	VEC3	vcP1 = pV1->p;
	VEC3	vcP2 = pV2->p;
	
	VEC3	vcP2P1 = vcP2 - vcP1;
	
	FLOAT	fD		= 0.f;
	FLOAT	fL1L2   = D3DXVec3Dot(&vcL1, &vcL2);
	FLOAT	fP2P1L1 = D3DXVec3Dot(&vcP2P1, &vcL1);
	FLOAT	fP2P1L2 = D3DXVec3Dot(&vcP2P1, &vcL2);

	FLOAT	fDet1 = -1 + fL1L2 * fL1L2;
	FLOAT	fDet2 = fP2P1L2 - fL1L2 * fP2P1L1;
	FLOAT	fBeta =0.f;

	// 평행이다.
	if( fabsf(fL1L2)>0.9999f)
	{
		VEC3 vcT;
		
		D3DXVec3Cross(&vcT, &vcL1, &vcP2P1);
		fD = D3DXVec3LengthSq(&vcT);

		if(pDst)
			*pDst = fD;

		// 겹쳐 있다면
		// 부정( 정할 수 없다.)
		if(fD<0.0001f)
			return -1;

		// 겹쳐 있지 않다면
		// 불능(구할 수 없다.)
		return -2;
	}


	fD=LcMath_LineToLineDistance(pSinTheta, &vcN, pV1, pV2);

	if(pDst)
		*pDst = fD;


	// 충돌
	if(fabsf(fD)<0.0001f)
	{
		fBeta = fDet2/ fDet1;
		pOut[0] = vcP2 + fBeta * vcL2;
		pOut[1] = pOut[0];
		
		// 충돌한 점의 갯수를 돌려준다.
		return 1;
	}


	fBeta = fDet2/ fDet1;
	pOut[1] = vcP2 + fBeta * vcL2;
	pOut[0] = fD * vcN + pOut[1];
	
	// 충돌한 점의 갯수를 돌려준다.
	return 2;
}





// 무한 직선과 무한 평면사이..
// p = p1 + k * (p2 - p1)
// n . p + d = 0
// n . p1 + k *( p2 - p1) . p + d = 0
//
// k = - (d + n . p1)/( ( p2 - p1) . p )
//

FLOAT LcMath_LineToPlaneKValue(const DPLN* p1, const VEC3* p2, const VEC3* p3)
{
	VEC3 vcT = *p3 - *p2;
	FLOAT k = D3DXPlaneDotNormal(p1, &vcT);
	return -(D3DXPlaneDotCoord(p1, p2) / k);
}




INT	LcMath_LineToTriangle(VEC3* pOut, FLOAT* fD, VEC3* vcRayPos, VEC3* vcRayDir, VEC3* pTri, BOOL bUseCull)
{
	FLOAT	U, V;

	if(bUseCull)
	{
		VEC3 vcN;
		D3DXVec3Cross(&vcN, &(pTri[1]-pTri[0]), &(pTri[2]-pTri[0]));

		if( D3DXVec3Dot(&vcN, vcRayDir)>0)
			return -1;
	}

	if(D3DXIntersectTri( &pTri[0], &pTri[1], &pTri[2], vcRayPos, vcRayDir, &U, &V, fD))
	{
		if(pOut)
			*pOut = pTri[0] + U * (pTri[1] - pTri[0]) + V * (pTri[2] - pTri[0]);

		return 0;
	}

	return -1;
}


INT	LcMath_LineToTriangle(	VEC3* vcPck				// Pick Position
						,	FLOAT* D				// Distance from vcRayPos to pOut
						,	const VEC3* vcRayPos	// Starting Ray Position
						,	const VEC3* vcRayDir	// Starting Ray Position
						,	const VEC3* p0			// Triangle 0
						,	const VEC3* p1			// Triangle 1
						,	const VEC3* p2			// Triangle 2
						,	BOOL bUseCull)
{
	VEC3	vcCamPos= *vcRayPos;
	VEC3	vcA		= *p1-*p0;
	VEC3	vcB		= *p2-*p0;

	FLOAT	tU, tV, tD;

	if(bUseCull)
	{
		VEC3 vcN;
		D3DXVec3Cross(&vcN, &vcA, &vcB);

		if( D3DXVec3Dot(&vcN, vcRayDir)>0)
			return -1;
	}

	if(D3DXIntersectTri( p0, p1, p2, vcRayPos, vcRayDir, &tU, &tV, &tD))
	{
		if(vcPck)
			*vcPck = *p0 + tU * vcA + tV * vcB;
		
		if(D)
			*D = tD;
		
		return 0;
	}

	return -1;
}



INT LcMath_LineToTriangle(
							VEC3* vcOut
						,	const VEC3* pTri
						,	const LCXLINE* pLine
						,	BOOL bRectangle
						,	BOOL bPositiveDirection
						,	const VEC3* pBoundLine
						,	const VEC3* pTriNormal
						)
{
	VEC3		vcN;
	FLOAT		fD;

	VEC3 vcA = pTri[1] - pTri[0];
	VEC3 vcB = pTri[2] - pTri[0];

	if(NULL == pTriNormal)
	{
		D3DXVec3Cross(&vcN, &vcA, &vcB);
		D3DXVec3Normalize(&vcN,&vcN);
	}
	else
	{
		vcN = *pTriNormal;
	}

	fD = -D3DXVec3Dot(&vcN, &pTri[0]);

	if(pBoundLine)																// 직선이 유한일 경우
	{
		D3DXPLANE	plane = D3DXPLANE(vcN.x, vcN.y, vcN.z, fD);
		FLOAT fD1 = D3DXPlaneDotCoord(&plane, &pBoundLine[0]);
		FLOAT fD2 = D3DXPlaneDotCoord(&plane, &pBoundLine[1]);

		if(fD1 * fD2>0)																// Not Collision
			return -1;
	}

	VEC3		vcC;
	VEC3		vcD;

	FLOAT		fT;
	FLOAT		fAC;
	FLOAT		fBC;
	FLOAT		fAB;

	FLOAT		fA;
	FLOAT		fB;
	FLOAT		fU;
	FLOAT		fV;

	VEC3 vcL = pLine->t;

	fT = D3DXVec3Dot(&vcL, &vcN);
	fT = -( fD+ D3DXVec3Dot(&vcN, &pLine->p))/fT;

	if(bPositiveDirection && fT<=0)
		return -1;

	vcC = pLine->p + fT * vcL;
	vcC -= pTri[0];

	fA = D3DXVec3Length(&vcA);
	fB = D3DXVec3Length(&vcB);

	fAC = D3DXVec3Dot(&vcA, &vcC);
	fBC = D3DXVec3Dot(&vcB, &vcC);
	fAB = D3DXVec3Dot(&vcA, &vcB);

	fD = fA * fA * fB * fB - fAB*fAB+0.000001f;
	fU = (fAC * fB*  fB - fAB * fBC)/fD;
	fV = (fA * fA * fBC - fAB * fAC)/fD;

	vcD = fU * vcA + fV * vcB;
	vcD += pTri[0];
	*vcOut = vcD;

	if(bRectangle)
	{
		if(fU>=-0.0001f && fU<=1.0001f && fV>=-0.0001f && fV<=1.0001f)
			return 0;
	}
	else
	{
		if(fU>=-0.0001f && fU<=1.0001f && fV>=-0.0001f && fV<=1.0001f && (fU+fV)<1.0001f)
			return 0;
	}
	
	return -1;
}


INT LcMath_TriangleToTriangle(VEC3* pOut, VEC3* pV1, VEC3* pV2)
{
	//삼각형과 삼각형의 충돌은 최소1점이상 이어야...
	// 직선과 삼각형 충돌을 응용한다.

	INT		iPoint=0;
	VEC3*	pTri1 = NULL;
	VEC3*	pTri2 = NULL;

	VEC3	pLine[2];
	VEC3	vcPick;
	INT		i=0;
	

	pTri1 = pV1;
	pTri2 = pV2;

	for(i=0; i<3; ++i)
	{
		pLine[0] = pTri2[(i+0)%3];
		pLine[1] = pTri2[(i+1)%3];

		VEC3	vcLineT = pLine[1] - pLine[0];
		D3DXVec3Normalize(&vcLineT, &vcLineT);

		LCXLINE Line(pLine[0], vcLineT);

		if(SUCCEEDED(LcMath_LineToTriangle(&vcPick, pTri1, &Line, FALSE, FALSE, pLine)))
		{
			if(pOut)
				*(pOut + iPoint) = vcPick;

			++iPoint;
		}
	}

	if(2==iPoint)
		return iPoint;

	pTri1 = pV2;
	pTri2 = pV1;

	for(i=0; i<3; ++i)
	{
		pLine[0] = pTri2[(i+0)%3];
		pLine[1] = pTri2[(i+1)%3];

		VEC3	vcLineT = pLine[1] - pLine[0];
		D3DXVec3Normalize(&vcLineT, &vcLineT);

		LCXLINE Line(pLine[0], vcLineT);

		if(SUCCEEDED(LcMath_LineToTriangle(&vcPick, pTri1, &Line, FALSE, FALSE, pLine)))
		{
			if(pOut)
				*(pOut + iPoint) = vcPick;
			
			++iPoint;
		}
	}

	if(iPoint<1)
		return -1;

	return iPoint;
}


// 삼각형의 정교한 외접원
FLOAT LcMath_CircumcircleCorrect(VEC3* pOut,const VEC3* pTri)
{
	VEC3	p1, p2, p3;
	VEC3	t1, t2, t3;
	VEC3	 n, n1, n2;
	VEC3	q1, q2, q3;
	VEC3	s1;

	FLOAT	a1=0.f;
	FLOAT	fR=0.f;
	VEC3	vcT;

	FLOAT	fD=0.f;

	p1 = pTri[0];
	p2 = pTri[1];
	p3 = pTri[2];

	t1 = (p2 - p1) * .5f;
	t2 = (p3 - p2) * .5f;
	t3 = (p1 - p3) * .5f;

	D3DXVec3Cross(&n, &t1, &t2);
	n *=-1.f;
	D3DXVec3Normalize(&n, &n);

	D3DXVec3Cross(&n1, &n, &t1);
	D3DXVec3Cross(&n2, &n, &t2);

	q1 = p1 + t1;
	q2 = p2 + t2;
	q3 = p3 + t3;

	s1 = q2 - q1;

	FLOAT fN1 = D3DXVec3LengthSq(&n1);
	FLOAT fN2 = D3DXVec3LengthSq(&n2);

	FLOAT fN12 = D3DXVec3Dot(&n1, &n2);
	
	fD	= fN12 * fN12 - fN1 * fN2;



	FLOAT fN1S1 = D3DXVec3Dot(&n1, &s1);
	FLOAT fN2S1 = D3DXVec3Dot(&n2, &s1);

	a1 = fN12 * fN2S1 - fN2 * fN1S1;
	a1 /= fD;

	vcT		= t1 + a1 * n1;
	*pOut	= p1 + vcT;

	fR = D3DXVec3Length(&vcT);

	// 반경을 리턴
	return fR;
}



// 삼각형의 비정교한 외접원
// 무게중심을 가운데 두고, 이 점에서 각꼭지점 사이의 거리가 가장 큰 값을 반경으로 결정.
FLOAT LcMath_TriangleBoundSphere(VEC3* pOut,const VEC3* pTri)
{
	FLOAT	fR=0.f,	fR2=0.f, fR3=0.f;
	VEC3	q1, q2, q3;

	*pOut =pTri[0];
	*pOut +=pTri[1];
	*pOut +=pTri[2];

	*pOut /= 3.f;

	q1 = *pOut;
	q2 = *pOut;
	q3 = *pOut;

	q1 -= pTri[0];
	q2 -= pTri[1];
	q3 -= pTri[2];
	
	fR = D3DXVec3Length(&q1);
	fR2 = D3DXVec3Length(&q2);
	fR3 = D3DXVec3Length(&q3);

	if(fR2>fR)
		fR= fR2;

	if(fR3>fR)
		fR= fR3;

	// 반경을 리턴
	return fR;
}









INT LcMath_IsInPoints(const VEC3* p1, const VEC3* p2) 
{
	VEC3	_p = (*p1 - *p2);
	FLOAT f = D3DXVec3LengthSq(&_p);
	
	return (D3DXVec3LengthSq(p1) < f && D3DXVec3LengthSq(p2) < f) ? 1: -1;
}

INT LcMath_Vec3Greater(const VEC3* p1, const VEC3* p2)
{
	return (p1->x <= p2->x && p1->y <= p2->y && p1-> z <= p2->z) ? 1: -1;
}

INT	LcMath_Vec3Less(const VEC3* p1, const VEC3* p2)
{
	return (p1->x >= p2->x && p1->y >= p2->y && p1->z >= p2->z) ? 1: -1;
}


// 평면의 d값 세팅
void LcMath_PlaneSetD(DPLN* pOut, const VEC3* v)
{
	pOut->d = - D3DXVec3Dot(&VEC3(pOut->a, pOut->b, pOut->c), v);
}


// 법선 벡터와 원점에서의 최단거리로 평면 만들기
void LcMath_Plane(DPLN* pOut, const VEC3* n, FLOAT d)
{
	pOut->a = n->x;
	pOut->b = n->y;
	pOut->c = n->z;
	pOut->d = d;
}


// 법선벡터와 점으로 평면 만들기
void LcMath_PlaneSet(DPLN* pOut, const VEC3* p0, const VEC3* n)
{
	VEC3 nT = *n;
	D3DXVec3Normalize(&nT, &nT);
	
	pOut->a = nT.x;
	pOut->b = nT.y;
	pOut->c = nT.z;
	pOut->d = - D3DXVec3Dot(&nT, p0);
}


// 세점으로 평면 만들기
void LcMath_PlaneSet(DPLN* pOut, const VEC3* p0, const VEC3* p1, const VEC3* p2)
{
	VEC3 tp1 = *p1 - *p0;
	VEC3 tp2 = *p2 - *p0;
	
	VEC3 nT;
	D3DXVec3Cross(&nT, &tp1, &tp2);
	D3DXVec3Normalize(&nT, &nT);
	
	pOut->a = nT.x;
	pOut->b = nT.y;
	pOut->c = nT.z;
	pOut->d = - D3DXVec3Dot(&nT, p0);
}


// 직선과 평면과의 충돌점
INT LcMath_PlaneIntersectionWithLine(VEC3* pOut, const DPLN* pPn, const VEC3* p1, const VEC3* vcT)
{
	FLOAT k = D3DXPlaneDotNormal(pPn, vcT);
	
	if (k == 0)
		return -1;
	
	k =1.f/k;
	k *= (- D3DXPlaneDotCoord(pPn, p1) );
	
	if(pOut)
		*pOut = *p1 + k * (*vcT);
	
	return 0;
}





// 2점사이의 평면
INT LcMath_PlaneIntersectionWithPoints(VEC3* pOut, const DPLN* pPn, const VEC3* p1, const VEC3* p2)
{
	FLOAT d1 = D3DXPlaneDotCoord(pPn, p1);
	FLOAT d2 = D3DXPlaneDotCoord(pPn, p2);
	
	d1 *=  d2;
	
	if(pOut)
	{
		VEC3 vcT = *p2 - *p1;
		FLOAT k = D3DXPlaneDotNormal(pPn, &vcT);
		k =1.f/k;
		*pOut = *p1 + k * vcT;
	}
	
	return d1<0.f? 1: -1;
}




FLOAT	LcMath_PlaneToPlaneDistance(const LCXPLANE* pP1, const LCXPLANE* pP2)	// 평행한 평면사이의 최단거리
{
//	FLOAT fP1;
//	FLOAT fP2;
//
//	fP1 = D3DXVec3Length(&pP1->n);
//	fP1 = pP1->d/fP1;
//
//	fP2 = D3DXVec3Length(&pP2->n);
//	fP2 = pP2->d/fP2;
//
//	return fabs(fP1 - fP2);

	return fabs(pP1->d - pP2->d);
}



//	D3DXINLINE FLOAT D3DXPlaneDotCoord( const D3DXPLANE *pP, const VEC3 *pV)
//	{
//	#ifdef D3DX_DEBUG
//		if(!pP || !pV)
//			return 0.0f;
//	#endif
//
//		return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;
//	}


// 임의의 점이 평면의 위(뒤)에 있는 가 아래(앞)에 있는가?
INT LcMath_PlaneToPointRelation(DPLN* pPn, const VEC3* p1)
{
	FLOAT d = D3DXPlaneDotCoord(pPn, p1);

	// 평면의 아래
	if (d < -LC_ROUNDING_DELTA)
		return -1;
	
	// 평면의 위에
	if (d > LC_ROUNDING_DELTA)
		return 1;
	
	// 평면 안에
	return 0;
}



INT LcMath_PlaneToPointIsFront(const DPLN* pPn, const VEC3* p1, FLOAT fEpsilon)
{
	return (pPn->a * p1->x +
			pPn->b * p1->y +
			pPn->c * p1->z +
			pPn->d + fEpsilon ) > 0.f ? 1: -1;
}




VEC3 LcMath_PlaneMemberPoint(DPLN* pPn)
{
	return -pPn->d * VEC3(pPn->a, pPn->b, pPn->c);
}



//평행하면 Dot product는 1
INT LcMath_PlaneToPlaneExistsInterSect(const DPLN* p1, const DPLN* p2)
{
	FLOAT fD = D3DXVec3Dot(&VEC3(p1->a, p1->b, p1->c), &VEC3(p2->a, p2->b, p2->c));
	return fabsf(fD)>0.9999f? -1: 1;
}

// 두 평면이 교차하는 직선의 방정식 구하기...
// 이것은 의외로 쉽다.
//
// N1 . P + d1 = 0
// N2 . P + d2 = 0
//
// 에서
//
// P = a * N1 + b * N2
//
// 로 표현할 수 있으면 된다.
//
// N1 . P = a * N1 . N1 + b * N1 . N2
// N2 . P = a * N1 . N2 + b * N2 . N2
//
// 나머지는 행렬을 이용하면 끝!!!

INT LcMath_PlaneToPlaneIntersectLine(VEC3* pOutP/*Start Point*/, VEC3* pOutT/*Direction*/, const DPLN* pPn1, const DPLN* pPn2)
{
	VEC3	N1(pPn1->a, pPn1->b, pPn1->c);
	VEC3	N2(pPn2->a, pPn2->b, pPn2->c);
	VEC3	N;
	
	FLOAT f0 = D3DXVec3Length(&N1);
	FLOAT f2 = D3DXVec3Length(&N2);
	
	FLOAT f1 = D3DXVec3Dot(&N1,&N2);
	
	FLOAT D = f0*f2 - f1*f1;
	
	if (fabsf(D) < 0.0001f)
		return -1;
	
	D3DXVec3Cross(&N, &N1, &N2);
	D3DXVec3Normalize(&N, &N);
	*pOutT = N;
	
	D = 1.f / D;
	FLOAT _t0 = (-f2 * pPn1->d + f1 * pPn2->d) * D;
	FLOAT _t1 = (-f0 * pPn2->d + f1 * pPn1->d) * D;
	
	*pOutP = N1*_t0 + N2*_t1;
	
	return 0;
}



FLOAT LcMath_PlaneToPointMinDistance(const FLOAT* pPlane/*VECTOR4*/, const FLOAT* pPoint /*VECTOR3*/)
{
	// plane ==> n * x + d =0
	return pPlane[0] * pPoint[0] + pPlane[1] * pPoint[1] + pPlane[2] * pPoint[2] + pPlane[3];
}

FLOAT LcMath_PlaneToPointMinDistance(const DPLN* pPn, const VEC3* p1)
{
	return D3DXPlaneDotCoord(pPn, p1);
}





INT	LcMath_PointFromPlanes(VEC3* pOut, const DPLN* p1, const DPLN* p2, const DPLN* p3)
{
	FLOAT	X;
	FLOAT	Y;
	FLOAT	Z;
	FLOAT	D;

	MATA	mtX(	-p1->d,	p1->b,	p1->c,	0,
					-p2->d,	p2->b,	p2->c,	0,
					-p3->d,	p3->b,	p3->c,	0,
					0,		0,		0,		1);

	MATA	mtY(	p1->a,	-p1->d,	p1->c,	0,
					p2->a,	-p2->d,	p2->c,	0,
					p3->a,	-p3->d,	p3->c,	0,
					0,		0,		0,		1);

	MATA	mtZ(	p1->a,	p1->b,	-p1->d,	0,
					p2->a,	p2->b,	-p2->d,	0,
					p3->a,	p3->b,	-p3->d,	0,
					0,		0,		0,		1);

	MATA	mtD(	p1->a,	p1->b,	p1->c,	0,
					p2->a,	p2->b,	p2->c,	0,
					p3->a,	p3->b,	p3->c,	0,
					0,		0,		0,		1);

	D = D3DXMatrixDeterminant(&mtD);
	X = D3DXMatrixDeterminant(&mtX);
	Y = D3DXMatrixDeterminant(&mtY);
	Z = D3DXMatrixDeterminant(&mtZ);

	if(0.f == D)
		return -1;

	pOut->x = X/D;
	pOut->y = Y/D;
	pOut->z = Z/D;

	return 0;
}








void LcMath_MatrixRotationRadians(MATA* pOut, const VEC3* pRot)
{
	D3DXMatrixIdentity(pOut);
	
	FLOAT cr = cosf( pRot->x );
	FLOAT sr = sinf( pRot->x );
	FLOAT cp = cosf( pRot->y );
	FLOAT sp = sinf( pRot->y );
	FLOAT cy = cosf( pRot->z );
	FLOAT sy = sinf( pRot->z );
	
	pOut->m[0][0] =  cp*cy;
	pOut->m[0][1] =  cp*sy;
	pOut->m[0][2] =  -sp  ;
	
	FLOAT srsp = sr*sp;
	FLOAT crsp = cr*sp;
	
	pOut->m[1][0] = srsp*cy-cr*sy;
	pOut->m[1][1] = srsp*sy+cr*cy;
	pOut->m[1][2] = sr*cp;
	
	pOut->m[2][0] = crsp*cy+sr*sy;
	pOut->m[2][1] = crsp*sy-sr*cy;
	pOut->m[2][2] = cr*cp;
	
	//	D3DXMatrixRotationX(&mtX, pRot.x);
	//	D3DXMatrixRotationY(&mtY, pRot.y);
	//	D3DXMatrixRotationZ(&mtZ, pRot.z);
	//	*pOut = mtX * mtY * mtZ;
}


void LcMath_MatrixRotationDegrees(MATA* pOut, const VEC3* pRot)
{
	LcMath_MatrixRotationRadians(pOut, &((*pRot) * ONE_DEGtoRAD) );
}



void LcMath_MatrixInverseRotationRadians(MATA* pOut, const VEC3* pRot)
{
	D3DXMatrixIdentity(pOut);
	
	FLOAT cr = cosf( pRot->x );
	FLOAT sr = sinf( pRot->x );
	FLOAT cp = cosf( pRot->y );
	FLOAT sp = sinf( pRot->y );
	FLOAT cy = cosf( pRot->z );
	FLOAT sy = sinf( pRot->z );
	
	pOut->m[0][0] = cp*cy;
	pOut->m[1][0] = cp*sy;
	pOut->m[2][0] = -sp  ;
	
	FLOAT srsp = sr*sp;
	FLOAT crsp = cr*sp;
	
	pOut->m[0][1] = srsp*cy-cr*sy;
	pOut->m[1][1] = srsp*sy+cr*cy;
	pOut->m[2][1] = sr*cp ;
	
	pOut->m[0][2] = crsp*cy+sr*sy;
	pOut->m[1][2] = crsp*sy-sr*cy;
	pOut->m[2][2] = cr*cp ;
}


void LcMath_MatrixInverseRotationDegrees(MATA* pOut, const VEC3* pRot)
{
	LcMath_MatrixInverseRotationRadians(pOut, &((*pRot) * ONE_DEGtoRAD) );
}







//
void LcMath_Vec3RotationDegrees(VEC3* pOut, const MATA* pIn)
{
	FLOAT y = -asinf(pIn->m[2][0]);
	FLOAT D = y;
	FLOAT C = cosf(y);
	y *= ONE_RADtoDEG;
	
	FLOAT rotx, roty, x, z;
	
	if (fabsf(C)>0.0005f)
	{
		rotx = pIn->m[2][2] / C;
		roty = pIn->m[1][2] / C;
		x = atan2f( roty, rotx ) * ONE_RADtoDEG;
		rotx = pIn->m[0][0] / C;
		roty = pIn->m[0][1] / C;
		z = atan2f( roty, rotx ) * ONE_RADtoDEG;
	}
	else
	{
		x  = 0.f;
		rotx = pIn->m[1][1];
		roty =-pIn->m[1][0];
		z  = atan2f( roty, rotx ) * ONE_RADtoDEG;
	}
	
	// fix values that Get below zero
	// before it would set (!) values to 360
	// that where above 360:
	if (x < 0.f) x += 360.f;
	if (y < 0.f) y += 360.f;
	if (z < 0.f) z += 360.f;
	
	pOut->x = x;
	pOut->y = y;
	pOut->z = z;
}


// Matrix Transpose
void LcMath_MatrixTranspose(MATA* pOut, const MATA* pIn)
{
	for (INT i=0; i<4; ++i)
		for (INT j=0; j<4; ++j)
			pOut->m[i][j] = pIn->m[j][i];
}


void LcMath_Vec3Translate(VEC3* pOut, const MATA* pT, const VEC3* pIn)
{
	pOut->x = pIn->x + pT->_41;
	pOut->y = pIn->y + pT->_42;
	pOut->z = pIn->z + pT->_43;
}



void LcMath_Vec3InverseTranslate(VEC3* pOut, const MATA* pT, const VEC3* pIn)
{
	pOut->x = pIn->x - pT->_41;
	pOut->y = pIn->y - pT->_42;
	pOut->z = pIn->z - pT->_43;
}


void LcMath_Vec3Rotate(VEC3* pOut, const MATA* pR, const VEC3* pIn)
{
	VEC3 vcT = *pIn;
	pOut->x = vcT.x*pR->_11 + vcT.y*pR->_21 + vcT.z*pR->_31;
	pOut->y = vcT.x*pR->_12 + vcT.y*pR->_22 + vcT.z*pR->_32;
	pOut->z = vcT.x*pR->_13 + vcT.y*pR->_23 + vcT.z*pR->_33;
}

void LcMath_Vec3InverseRotate(VEC3* pOut, const MATA* pR, const VEC3* pIn)
{
	VEC3 vcT = *pIn;
	pOut->x = vcT.x*pR->_11 + vcT.y*pR->_12 + vcT.z*pR->_13;
	pOut->y = vcT.x*pR->_21 + vcT.y*pR->_22 + vcT.z*pR->_33;
	pOut->z = vcT.x*pR->_31 + vcT.y*pR->_32 + vcT.z*pR->_33;
}


void LcMath_Vec3Transform(VEC3*pOut, const MATA* pT, const VEC3* pIn)
{
	VEC3 vcT = *pIn;
	pOut->x = vcT.x * pT->_11 + vcT.y * pT->_21 + vcT.z * pT->_31 + pT->_41;
	pOut->y = vcT.x * pT->_12 + vcT.y * pT->_22 + vcT.z * pT->_32 + pT->_42;
	pOut->z = vcT.x * pT->_13 + vcT.y * pT->_23 + vcT.z * pT->_33 + pT->_43;
}



void LcMath_PlaneTransform(DPLN*pOut, const MATA* pT, DPLN* pIn)
{
	VEC3 origin(0,0,0);
	VEC3 member=LcMath_PlaneMemberPoint(pIn);
	
	LcMath_Vec3Transform(&origin, pT, &origin);
	LcMath_Vec3Transform(&member, pT, &member);
	
	LcMath_Vec3Transform((VEC3*)&pOut->a, pT, (VEC3*)&pIn->a);
	pOut->a -= origin.x;
	pOut->b -= origin.y;
	pOut->c -= origin.z;
	
	pOut->d = - D3DXPlaneDotNormal(pOut, &member);
}



void LcMath_Vec4Transform(VEC4*pOut, const MATA* pT, const VEC4* pIn)
{
	pOut->x = pIn->x*pT->_11 + pIn->y*pT->_21 + pIn->z*pT->_31 + pIn->w*pT->_41;
	pOut->y = pIn->x*pT->_12 + pIn->y*pT->_22 + pIn->z*pT->_32 + pIn->w*pT->_42;
	pOut->z = pIn->x*pT->_13 + pIn->y*pT->_23 + pIn->z*pT->_33 + pIn->w*pT->_43;
	pOut->w = pIn->x*pT->_14 + pIn->y*pT->_24 + pIn->z*pT->_34 + pIn->w*pT->_44;
}







// Triangle
//////////////////////////////////////////////////////////////////////////////

void	LcMath_TriTransform	(LCXTRI* pOut,const MATA* pT,const LCXTRI* pIn)
{
	LCXTRI Tri = *pIn;
	LcMath_Vec3Transform(&pOut->_0, pT, &Tri._0);
	LcMath_Vec3Transform(&pOut->_1, pT, &Tri._1);
	LcMath_Vec3Transform(&pOut->_2, pT, &Tri._2);
}

void	LcMath_TriMove(LCXTRI* pOut,const VEC3* pV,const LCXTRI* pIn)
{
	LCXTRI Tri = *pIn;

	pOut->_0 = *pV + Tri._0;
	pOut->_1 = *pV + Tri._1;
	pOut->_2 = *pV + Tri._2;
}








void LcMath_CubeHMove(LCXCUBEH* pOut,const VEC3* pV,const LCXCUBEH* pIn)
{
	pOut->_0	= *pV + pIn->_0;
	pOut->_1	= *pV + pIn->_1;
	pOut->_2	= *pV + pIn->_2;
	pOut->_3	= *pV + pIn->_3;

	pOut->_4	= *pV + pIn->_4;
	pOut->_5	= *pV + pIn->_5;
	pOut->_6	= *pV + pIn->_6;
	pOut->_7	= *pV + pIn->_7;
}


void LcMath_CubeHTransform(LCXCUBEH* pOut,const MATA* pT, const LCXCUBEH* pIn)
{
	int i=0;

	LCXCUBEH pCubeT = *pIn;

	for(i=0; i<8; ++i)
		LcMath_Vec3Transform((VEC3*)&pOut->m[i], pT, (VEC3*)&pCubeT.m[i]);
}



void LcMath_CubeHBoxLine(void* pOut, INT nSizeChunk,const LCXCUBEH* pIn)
{
	*( (VEC3*) ((char*)(pOut) +  0 * nSizeChunk)  ) = pIn->_1;
	*( (VEC3*) ((char*)(pOut) +  1 * nSizeChunk)  ) = pIn->_0;
	*( (VEC3*) ((char*)(pOut) +  2 * nSizeChunk)  ) = pIn->_2;
	*( (VEC3*) ((char*)(pOut) +  3 * nSizeChunk)  ) = pIn->_0;
	*( (VEC3*) ((char*)(pOut) +  4 * nSizeChunk)  ) = pIn->_1;
	*( (VEC3*) ((char*)(pOut) +  5 * nSizeChunk)  ) = pIn->_3;
	*( (VEC3*) ((char*)(pOut) +  6 * nSizeChunk)  ) = pIn->_2;
	*( (VEC3*) ((char*)(pOut) +  7 * nSizeChunk)  ) = pIn->_3;
	
	*( (VEC3*) ((char*)(pOut) +  8 * nSizeChunk)  ) = pIn->_5;
	*( (VEC3*) ((char*)(pOut) +  9 * nSizeChunk)  ) = pIn->_4;
	*( (VEC3*) ((char*)(pOut) + 10 * nSizeChunk)  ) = pIn->_6;
	*( (VEC3*) ((char*)(pOut) + 11 * nSizeChunk)  ) = pIn->_4;
	*( (VEC3*) ((char*)(pOut) + 12 * nSizeChunk)  ) = pIn->_5;
	*( (VEC3*) ((char*)(pOut) + 13 * nSizeChunk)  ) = pIn->_7;
	*( (VEC3*) ((char*)(pOut) + 14 * nSizeChunk)  ) = pIn->_6;
	
	*( (VEC3*) ((char*)(pOut) + 15 * nSizeChunk)  ) = pIn->_7;
	*( (VEC3*) ((char*)(pOut) + 16 * nSizeChunk)  ) = pIn->_0;
	*( (VEC3*) ((char*)(pOut) + 17 * nSizeChunk)  ) = pIn->_4;
	*( (VEC3*) ((char*)(pOut) + 18 * nSizeChunk)  ) = pIn->_1;
	*( (VEC3*) ((char*)(pOut) + 19 * nSizeChunk)  ) = pIn->_5;
	*( (VEC3*) ((char*)(pOut) + 20 * nSizeChunk)  ) = pIn->_2;
	*( (VEC3*) ((char*)(pOut) + 21 * nSizeChunk)  ) = pIn->_6;
	*( (VEC3*) ((char*)(pOut) + 22 * nSizeChunk)  ) = pIn->_3;
	*( (VEC3*) ((char*)(pOut) + 23 * nSizeChunk)  ) = pIn->_7;
}


void LcMath_CubeHFromA(LCXCUBEH* pOut,const LCXCUBEAA* pIn)
{
	VEC3	vcP = pIn->vcMin;
	VEC3	vcT = pIn->vcMax - pIn->vcMin;
	FLOAT	w = vcT.x;
	FLOAT	h = vcT.y;
	FLOAT	d = vcT.z;

	pOut->_0	= vcP + VEC3(0, 0, 0);
	pOut->_1	= vcP + VEC3(0, h, 0);
	pOut->_2	= vcP + VEC3(w, 0, 0);
	pOut->_3	= vcP + VEC3(w, h, 0);

	pOut->_4	= vcP + VEC3(0, 0, d);
	pOut->_5	= vcP + VEC3(0, h, d);
	pOut->_6	= vcP + VEC3(w, 0, d);
	pOut->_7	= vcP + VEC3(w, h, d);
}



void LcMath_CubeAMove(LCXCUBEAA* pOut,const VEC3* pV,const LCXCUBEAA* pIn)
{
	pOut->vcMin	= *pV + pIn->vcMin;
	pOut->vcMax	= *pV + pIn->vcMax;
}



void LcMath_CubeATransform(LCXCUBEAA*pOut,const MATA* pT,const LCXCUBEAA* pIn)
{
	LCXCUBEAA CubeT = *pIn;
	LcMath_Vec3Transform(&pOut->vcMin, pT, &CubeT.vcMin);
	LcMath_Vec3Transform(&pOut->vcMax, pT, &CubeT.vcMax);
	pOut->Repair();
}


INT LcMath_CubeAToLine(VEC3* vcOut, LCXCUBEAA* pCube, LCXLINE* pLine, BOOL bPositiveDirection)
{
	VEC3 pVcBox[2];

	pVcBox[0] = pCube->vcMin;
	pVcBox[1] = pCube->vcMax;

	return LcMath_CubeAToLine(vcOut, pVcBox, pLine, bPositiveDirection);
}



INT LcMath_CubeAToLine(VEC3* vcOut, VEC3* pVcBox, LCXLINE* pLine, BOOL bPositiveDirection)
{
	BOOL	hr= -1;
	FLOAT	w;
	FLOAT	h;
	FLOAT	d;

	VEC3	vcP[8];
	VEC3	vcT;
	LCXTRI	Tri;
	VEC3	vcBuf[3];
	INT		nCollision = FALSE;
	BOOL	bRectangle = TRUE;
	
	vcP[0] = pVcBox[0];
	vcP[7] = pVcBox[1];

	vcT = vcP[7]- vcP[0];
	w = vcT.x;
	h = vcT.y;
	d = vcT.z;

	vcP[1] = vcP[0] + VEC3(w, 0, 0);
	vcP[2] = vcP[0] + VEC3(0, h, 0);
	vcP[3] = vcP[0] + VEC3(w, h, 0);
	vcP[4] = vcP[0] + VEC3(0, 0, d);
	vcP[5] = vcP[0] + VEC3(w, 0, d);
	vcP[6] = vcP[0] + VEC3(0, h, d);

	

	static const int nIdxTri[][3] =
	{
		{ 0, 1, 2},
		{ 2, 3, 6},
		{ 6, 7, 4},
		{ 4, 5, 0},
		
		{ 1, 5, 3},
		{ 4, 0, 6},
	};


	for(int i=0; i<6; ++i)
	{
		Tri.Set(vcP[ nIdxTri[i][0] ], vcP[ nIdxTri[i][1] ], vcP[ nIdxTri[i][2] ]);
		
		hr = LcMath_LineToTriangle(&vcT, (VEC3*)&Tri, pLine, bRectangle, bPositiveDirection);

		if(SUCCEEDED(hr))
		{
			vcBuf[nCollision] = vcT;
			++nCollision;
		}

		if(nCollision>=2)
			break;
	}

	if(1 == nCollision)
	{
		*vcOut = vcBuf[0];
		return nCollision;
	}

	else if(2 == nCollision)
	{
		FLOAT fD1 = D3DXVec3LengthSq( &(vcBuf[0] - pLine->p));
		FLOAT fD2 = D3DXVec3LengthSq( &(vcBuf[1] - pLine->p));

		if(fD1<fD2)
		{
			vcOut[0] = vcBuf[0];
			vcOut[1] = vcBuf[1];
		}
		else
		{
			vcOut[0] = vcBuf[1];
			vcOut[1] = vcBuf[0];
		}

		return nCollision;
	}


	return -1;
}



void LcMath_CubeABoxLine(void* pOut, INT nSizeChunk,const LCXCUBEAA* pIn)
{
	LCXCUBEH		pHexa;
	LcMath_CubeHFromA(&pHexa, pIn);

	*( (VEC3*) ((char*)(pOut) +  0 * nSizeChunk)  ) = pHexa._1;
	*( (VEC3*) ((char*)(pOut) +  1 * nSizeChunk)  ) = pHexa._0;
	*( (VEC3*) ((char*)(pOut) +  2 * nSizeChunk)  ) = pHexa._2;
	*( (VEC3*) ((char*)(pOut) +  3 * nSizeChunk)  ) = pHexa._0;
	*( (VEC3*) ((char*)(pOut) +  4 * nSizeChunk)  ) = pHexa._1;
	*( (VEC3*) ((char*)(pOut) +  5 * nSizeChunk)  ) = pHexa._3;
	*( (VEC3*) ((char*)(pOut) +  6 * nSizeChunk)  ) = pHexa._2;
	*( (VEC3*) ((char*)(pOut) +  7 * nSizeChunk)  ) = pHexa._3;
	
	*( (VEC3*) ((char*)(pOut) +  8 * nSizeChunk)  ) = pHexa._5;
	*( (VEC3*) ((char*)(pOut) +  9 * nSizeChunk)  ) = pHexa._4;
	*( (VEC3*) ((char*)(pOut) + 10 * nSizeChunk)  ) = pHexa._6;
	*( (VEC3*) ((char*)(pOut) + 11 * nSizeChunk)  ) = pHexa._4;
	*( (VEC3*) ((char*)(pOut) + 12 * nSizeChunk)  ) = pHexa._5;
	*( (VEC3*) ((char*)(pOut) + 13 * nSizeChunk)  ) = pHexa._7;
	*( (VEC3*) ((char*)(pOut) + 14 * nSizeChunk)  ) = pHexa._6;
	
	*( (VEC3*) ((char*)(pOut) + 15 * nSizeChunk)  ) = pHexa._7;
	*( (VEC3*) ((char*)(pOut) + 16 * nSizeChunk)  ) = pHexa._0;
	*( (VEC3*) ((char*)(pOut) + 17 * nSizeChunk)  ) = pHexa._4;
	*( (VEC3*) ((char*)(pOut) + 18 * nSizeChunk)  ) = pHexa._1;
	*( (VEC3*) ((char*)(pOut) + 19 * nSizeChunk)  ) = pHexa._5;
	*( (VEC3*) ((char*)(pOut) + 20 * nSizeChunk)  ) = pHexa._2;
	*( (VEC3*) ((char*)(pOut) + 21 * nSizeChunk)  ) = pHexa._6;
	*( (VEC3*) ((char*)(pOut) + 22 * nSizeChunk)  ) = pHexa._3;
	*( (VEC3*) ((char*)(pOut) + 23 * nSizeChunk)  ) = pHexa._7;
}







INT LcMath_CubeHToSphere(const LCXCUBEH* pHexa, const VEC3* pPoint,  /*In*/ FLOAT fRadius)
{
	INT		i=0, j=0;
	BOOL	bColl = FALSE;

	// 2. 두 육면체의 점이 각각 전부 안에 있는가 혹은 밖에 있는가?

	static const int nIdxNrm[][3] =
	{
		{ 0, 1, 2},
		{ 2, 3, 6},
		{ 6, 7, 4},

		{ 4, 5, 0},
		{ 1, 5, 3},
		{ 4, 0, 6},
	};

	D3DXPLANE	Plane1[6];
	INT			hr[6];


	for(i=0;i<6; ++i)
	{
		LcMath_PlaneSet(	&Plane1[i]
					, (VEC3*)&pHexa->m[ nIdxNrm[i][0] ]
					, (VEC3*)&pHexa->m[ nIdxNrm[i][1] ]
					, (VEC3*)&pHexa->m[ nIdxNrm[i][2] ]);
	}



	for(i=0;i<6; ++i)
	{
		hr[i] = LcMath_PlaneToPointIsFront(&Plane1[i], pPoint, -fRadius);
	}

	if( hr[0]<0 &&
		hr[1]<0 &&
		hr[2]<0 &&
		hr[3]<0 &&
		hr[4]<0 &&
		hr[5]<0
		)
	{
		bColl = TRUE;
	}

	if( TRUE == bColl)
		return 0;

	return -1;
}






INT LcMath_CubeHToTri(const LCXCUBEH* pHexa, const LCXTRI* pTri)
{
	INT		i=0;
	BOOL	bCollision = FALSE;

	
	// 2. 두 육면체의 점이 각각 전부 안에 있는 가 혹은 밖에 있는가?

	for(i=0; i<3; ++i)
	{
		VEC3	vcP = pTri->m[i];

		bCollision = LcMath_CubeHToSphere(pHexa, &vcP, 0);

		if( TRUE == bCollision)
			return 0;
	}


	// 3. 두 육면체가 교차하지만 포함하는 점이 없다.
	// 삼각형 vs 삼각형 충돌로 검사.

	const static int nIdxTri[][3] =
	{
		{ 0, 1, 2},
		{ 3, 2, 1},

		{ 2, 3, 6},
		{ 7, 6, 3},

		{ 6, 7, 4},
		{ 5, 4, 7},

		{ 4, 5, 0},
		{ 1, 0, 5},

		{ 1, 5, 3},
		{ 7, 3, 5},

		{ 4, 0, 6},
		{ 2, 6, 0},
	};


	for(i=0; i<12; ++i)
	{
		VEC3	pTri1[3];
		VEC3	pOut[3];


		pTri1[0] = pHexa->m[nIdxTri[i][0] ];
		pTri1[1] = pHexa->m[nIdxTri[i][1] ];
		pTri1[2] = pHexa->m[nIdxTri[i][2] ];


		INT hr = LcMath_TriangleToTriangle(pOut, pTri1, (VEC3*)pTri);

		if(SUCCEEDED(hr))
		{
			bCollision = TRUE;
			break;
		}
	}

	if(TRUE == bCollision)
		return 0;


	return -1;

}







INT LcMath_CubeHToH(const LCXCUBEH* pHexagon1, const LCXCUBEH* pHexagon2)
{
	INT		i=0, j=0;
	BOOL	bCollision = FALSE;

	// 1. 최대 Bound Sphere로 체크

	FLOAT	fBnd1 = 0.f;
	FLOAT	fBnd2 = 0.f;
	VEC3	vcCenter1(0,0,0);
	VEC3	vcCenter2(0,0,0);

	VEC3	vcT;
	FLOAT	fB=0;


	for(i=0;i<8; ++i)
	{
		vcCenter1 += pHexagon1->m[i];
		vcCenter2 += pHexagon2->m[i];
	}

	vcCenter1 /= 8.f;
	vcCenter2 /= 8.f;


	for(i=0;i<8; ++i)
	{
		vcT = vcCenter1 - pHexagon1->m[i];
		fB = D3DXVec3Length(&vcT);

		if(fB>fBnd1)
			fBnd1 = fB;


		vcT = vcCenter2 - pHexagon2->m[i];
		fB = D3DXVec3Length(&vcT);

		if(fB>fBnd2)
			fBnd2 = fB;
	}


	vcT = vcCenter1 - vcCenter2;
	fB = D3DXVec3Length(&vcT);

	// 박스의 중심 거리와 바운드 구와의 비교

	if(fB>=(fBnd1 + fBnd2))
		return -1;




	// 2. 두 육면체의 점이 각각 전부 안에 있는 가 혹은 밖에 있는가?

	static const int nIdxNrm[][3] =
	{
		{ 0, 1, 2},
		{ 2, 3, 6},
		{ 6, 7, 4},

		{ 4, 5, 0},
		{ 1, 5, 3},
		{ 4, 0, 6},
	};

	D3DXPLANE	Plane1[6];
	D3DXPLANE	Plane2[6];
	INT			hr[6];


	for(i=0;i<6; ++i)
	{
		LcMath_PlaneSet(	&Plane1[i]
					, (VEC3*)&pHexagon1->m[ nIdxNrm[i][0] ]
					, (VEC3*)&pHexagon1->m[ nIdxNrm[i][1] ]
					, (VEC3*)&pHexagon1->m[ nIdxNrm[i][2] ]);


		LcMath_PlaneSet(	&Plane2[i]
					, (VEC3*)&pHexagon2->m[ nIdxNrm[i][0] ]
					, (VEC3*)&pHexagon2->m[ nIdxNrm[i][1] ]
					, (VEC3*)&pHexagon2->m[ nIdxNrm[i][2] ]);
	}



	for(j=0; j<8; ++j)
	{
		for(i=0;i<6; ++i)
		{
			hr[i] = LcMath_PlaneToPointIsFront(&Plane1[i], (VEC3*)&pHexagon2->m[j]);
		}

		if( hr[0]<0 &&
			hr[1]<0 &&
			hr[2]<0 &&
			hr[3]<0 &&
			hr[4]<0 &&
			hr[5]<0
			)
		{
			bCollision = TRUE;
			break;
		}
	}

	if( TRUE == bCollision)
		return 0;


	for(j=0; j<8; ++j)
	{
		for(i=0;i<6; ++i)
		{
			hr[i] = LcMath_PlaneToPointIsFront(&Plane2[i], (VEC3*)&pHexagon1->m[j]);
		}

		if( hr[0]<0 &&
			hr[1]<0 &&
			hr[2]<0 &&
			hr[3]<0 &&
			hr[4]<0 &&
			hr[5]<0
			)
		{
			bCollision = TRUE;
			break;
		}
	}

	if( TRUE == bCollision)
		return 0;



	// 3. 두 육면체가 교차하지만 포함하는 점이 없다.
	// 삼각형 vs 삼각형 충돌로 검사.

	static const int nIdxTri[][3] =
	{
		{ 0, 1, 2},
		{ 3, 2, 1},

		{ 2, 3, 6},
		{ 7, 6, 3},

		{ 6, 7, 4},
		{ 5, 4, 7},

		{ 4, 5, 0},
		{ 1, 0, 5},

		{ 1, 5, 3},
		{ 7, 3, 5},

		{ 4, 0, 6},
		{ 2, 6, 0},
	};


	for(i=0; i<12; ++i)
	{
		VEC3	pTri1[3];
		VEC3	pTri2[3];

		VEC3	pOut[3];


		pTri1[0] = pHexagon1->m[nIdxTri[i][0] ];
		pTri1[1] = pHexagon1->m[nIdxTri[i][1] ];
		pTri1[2] = pHexagon1->m[nIdxTri[i][2] ];


		for(j=0; j<12; ++j)
		{
			pTri2[0] = pHexagon2->m[nIdxTri[j][0] ];
			pTri2[1] = pHexagon2->m[nIdxTri[j][1] ];
			pTri2[2] = pHexagon2->m[nIdxTri[j][2] ];

			INT hr = LcMath_TriangleToTriangle(pOut, pTri1, pTri2);

			if(SUCCEEDED(hr))
				bCollision = TRUE;
		}

		if(TRUE == bCollision)
			break;
	}

	if(TRUE == bCollision)
		return 0;


	return -1;

}










INT LcMath_CylinderToLineIntersection(	VEC3* pOut,	const LCXCYLINDER* pV1, const LCXLINE* pV2, const VEC3* pV3)
{
	INT iInsc=0;

	VEC3 vcInsc[2];			// 실린더 중심선과 직선이 교차하는 지점
	FLOAT	fD=0.f;					// 실린더와 직선의 거리
	FLOAT	fR=pV1->r;				// 실린더의 반지름
	FLOAT	fA=0.f;					// 장반경
	VEC3	vcL1 = pV2->t;
	VEC3	vcL2 = pV1->t;
	FLOAT	fSinTheta=0.f;

	iInsc = LcMath_LineToLineIntersection(&fSinTheta, &fD, vcInsc, pV2, (LCXLINE*)pV1);

	// 부정이나 불능 두가지 중 하나.
	if(FAILED(iInsc))
	{
		// 원통의 반경과 비교한다.
		// 직선이 원통 안에 완전히 있다.
		if(fD <= pV1->r)
			return 0;	// 점을 정할 수가 없다.

		// 직선이 원통 바깥에 있다.
		else
			return -1;
	}

	// 직선이 원통 바깥에 있다.
	if(fabsf(fD)>=fR)
		return -1;


	// 충돌한 두 점의 위치를 찾는다.
	fA = fR/fabsf(fSinTheta);

	FLOAT fPi = fD/fR;
	fPi = asinf(fPi);

	FLOAT fK = fA * cosf( fPi);

	pOut[0] = vcInsc[0] - fK * vcL1;
	pOut[1] = vcInsc[0] + fK * vcL1;

	
	if(NULL == pV3)
		return 2;


	//
	FLOAT fDot[2];

	VEC3 vcDot0;
	VEC3 vcDot1;

	vcDot0 = pV3[0] - pOut[0];
	vcDot1 = pV3[1] - pOut[0];
	fDot[0] = D3DXVec3Dot(&vcDot0, &vcDot1);

	vcDot0 = pV3[0] - pOut[1];
	vcDot1 = pV3[1] - pOut[1];
	fDot[1] = D3DXVec3Dot(&vcDot0, &vcDot1);


	// 0번째만 영역 안에 있다.
	if( fDot[0] <0 && fDot[1] >0)
	{
		return 0;
	}

	// 1번째만 영역 안에 있다.
	else if( fDot[0] >0 && fDot[1] <0)
	{
		return 1;
	}

	// 둘다 영역 안에 있다.
	else if( fDot[0] <0 && fDot[1] <0)
	{
		return 2;
	}



	return -1;
}





// 원통과 삼각형의 충돌
// 삼각형이 원통안에 있으면 삼ㄱ각형과 원통의 최단거리 지점을 반환
// 원통의 중심선이 삼각형과 교차하면 교차 지점을 반환
// 나머지... 삼각형과 원통의 충돌 지점을 반환
INT LcMath_CylinderToTriIntersection(	VEC3* pOut				// Intersection point
									  ,	const LCXCYLINDER* pV1	// Input Cylinder
									  , const VEC3* pV2			// Input Triangle
									  )
{

	// 1. 충돌하지 않는 삼각형들은 제외시키낟.
	// 간단하게 삼각형을 둘러싸고 있는 구와 원통의 충돌을 구한다.
	// 원통 반지름 + 삼각형 충돌 반지름  < 원통와 삼각형의 최단 거리 이면 충돌이 아님.

	VEC3	TriCenter;
	FLOAT	TriRadius = LcMath_TriangleBoundSphere(&TriCenter, pV2);
	VEC3	pTriToSphere(0,0,0);

	// 원통과 삼각형 중심의 최단거리
	FLOAT	fDstLineFromPoint;

	LcMath_LineToSphere(NULL, &fDstLineFromPoint, (LCXLINE*)pV1, &TriCenter, TriRadius);

	if( fDstLineFromPoint>= (TriRadius + pV1->r))
		return -1;


	// 2. 삼각형이 원통 내부에 있을 경우
	// 완전히 있다면 충돌이며 중돌 포인트는 삼각형의 중심으로 한다.

	if(TriRadius<pV1->r)
	{
		// 삼각형이 원통 안에 완전히 있는가?
		if(fDstLineFromPoint<= (pV1->r - TriRadius))
		{
			*pOut = TriCenter;
			return 0;
		}
	}


	// 나머지 충돌은 두개가 남아 있다. 하나는 원통의 중심 직선이
	// 삼각형과 충돌 여부,  다른 하나는 삼각형의 세 변과 원통이 충돌하는 경우다.


	// 3. 원통의 중심 직선이 삼각형과 충돌 하는가?
	if(SUCCEEDED(LcMath_LineToTriangle(pOut, pV2, (LCXLINE*)pV1)))
		return 0;


	// 4. 나머지...
	// 삼각형을 이루고 있는 세 유한 직선이 원동과 충돌하는 지점을 구한다.

	VEC3	vcCol[6];
	INT		nCnt=-1;

	for(int i=0; i<3; ++i)
	{
		VEC3	vcOutT[2];
		INT		hr;
		VEC3	vcBnd[2];
		LCXLINE	Line;


		INT		nTriIdx0 = (i+0)%3;
		INT		nTriIdx1 = (i+1)%3;

		vcBnd[0] = pV2[nTriIdx0];
		vcBnd[1] = pV2[nTriIdx1];

		Line.p = vcBnd[0];
		Line.t = vcBnd[1] - vcBnd[0];
		D3DXVec3Normalize(&Line.t, &Line.t);

		hr = LcMath_CylinderToLineIntersection(vcOutT, pV1, &Line, vcBnd);

		if(FAILED(hr))
			continue;

		if(0 == hr)
		{
			++nCnt;
			vcCol[nCnt] = vcOutT[0];
		}

		else if(  1== hr)
		{
			++nCnt;
			vcCol[nCnt] = vcOutT[1];
		}

		else if( 2 == hr)
		{
			++nCnt;
			vcCol[nCnt] = vcOutT[0];

			++nCnt;
			vcCol[nCnt] = vcOutT[1];
		}
	}

	if(nCnt>=0)
	{
		memcpy(pOut, vcCol, (nCnt+1) * sizeof (VEC3));
		return (nCnt+1);
	}

	return -1;
}




// 원통과 삼각형의 충돌
// 삼각형이 원통안에 있으면 삼각형과 원통의 최단거리 지점을 반환
// 원통의 중심선이 삼각형과 교차하면 교차 지점을 반환
// 나머지... 삼각형과 원통의 충돌 지점을 반환
INT LcMath_CylinderToTriIntersection(	VEC3* pOut				// Intersection point
									,	const LCXCYLINDER* pV1	// Input Cylinder
									,	BndTri* pV2				// Input Bound Triangle
									,	BOOL bRectangle			// 충돌영역을 삼각형의 확장 평행사변형으로 쓸것인지..
									,	BOOL bPositiveDirection	// 직선의 방향으로만 충돌점을 구하기
									,	const VEC3* pBoundLine	// 유한 직선인 경우 양 끝 지점
									,	const VEC3* pTriNormal	// 삼각형의 Normal Vector
									)
{

	// 1. 충돌하지 않는 삼각형들은 제외시키낟.
	// 간단하게 삼각형을 둘러싸고 있는 구와 원통의 충돌을 구한다.
	// 원통 반지름 + 삼각형 충돌 반지름  < 원통와 삼각형의 최단 거리 이면 충돌이 아님.

	VEC3	TriCenter = pV2->GetBoundCenter();
	FLOAT	TriRadius = pV2->GetBoundRadius();
	VEC3	pTriToSphere(0,0,0);

	// 원통과 삼각형 중심의 최단거리
	FLOAT	fDstLineFromPoint;

	LcMath_LineToSphere(NULL, &fDstLineFromPoint, (LCXLINE*)pV1, &TriCenter, TriRadius);

	if( fDstLineFromPoint>= (TriRadius + pV1->r))
		return -1;


	// 2. 삼각형이 원통 내부에 있을 경우
	// 완전히 있다면 충돌이며 중돌 포인트는 삼각형의 중심으로 한다.

	if(TriRadius<pV1->r)
	{
		// 삼각형이 원통 안에 완전히 있는가?
		if(fDstLineFromPoint<= (pV1->r - TriRadius))
		{
			*pOut = TriCenter;
			return 0;
		}
	}


	// 나머지 충돌은 두개가 남아 있다. 하나는 원통의 중심 직선이
	// 삼각형과 충돌 여부,  다른 하나는 삼각형의 세 변과 원통이 충돌하는 경우다.


	// 3. 원통의 중심 직선이 삼각형과 충돌 하는가?
	if(SUCCEEDED(LcMath_LineToTriangle(pOut, &pV2->p0, (LCXLINE*)pV1, bRectangle, bPositiveDirection, pBoundLine, pTriNormal)))
		return 0;


	// 4. 나머지...
	// 삼각형을 이루고 있는 세 유한 직선이 원동과 충돌하는 지점을 구한다.

	VEC3	vcCol[6];
	INT		nCnt=-1;

	for(int i=0; i<3; ++i)
	{
		VEC3	vcOutT[2];
		INT		hr;
		VEC3	vcBnd[2];
		LCXLINE	Line;


		INT		nTriIdx0 = (i+0)%3;
		INT		nTriIdx1 = (i+1)%3;

		vcBnd[0] = pV2->vcT3[nTriIdx0];
		vcBnd[1] = pV2->vcT3[nTriIdx1];

		Line.p = vcBnd[0];
		Line.t = vcBnd[1] - vcBnd[0];
		D3DXVec3Normalize(&Line.t, &Line.t);

		hr = LcMath_CylinderToLineIntersection(vcOutT, pV1, &Line, vcBnd);

		if(FAILED(hr))
			continue;

		if(0 == hr)
		{
			++nCnt;
			vcCol[nCnt] = vcOutT[0];
		}

		else if(  1== hr)
		{
			++nCnt;
			vcCol[nCnt] = vcOutT[1];
		}

		else if( 2 == hr)
		{
			++nCnt;
			vcCol[nCnt] = vcOutT[0];

			++nCnt;
			vcCol[nCnt] = vcOutT[1];
		}
	}

	if(nCnt>=0)
	{
		memcpy(pOut, vcCol, (nCnt+1) * sizeof (VEC3));

		// 다른 것과 구분하기 위해서 <<2한다.
		return (nCnt+1)<<2;
	}

	return -1;
}




INT	LcMath_UVFromTri(	FLOAT* fU
					,	FLOAT* fV
					,	const VEC3* p
					,	const VEC3* p0
					,	const VEC3* p1
					,	const VEC3* p2)
{
	VEC3	vcA = *p1 - *p0;
	VEC3	vcB = *p2 - *p0;
	VEC3	vcP = *p  - *p0;

	FLOAT	fPA = D3DXVec3Dot( &vcA, &vcP);
	FLOAT	fPB = D3DXVec3Dot( &vcB, &vcP);
	FLOAT	fAB = D3DXVec3Dot( &vcA, &vcB);
	FLOAT	fAA = D3DXVec3Dot( &vcA, &vcA);
	FLOAT	fBB = D3DXVec3Dot( &vcB, &vcB);

	FLOAT fD = fAA*fBB - fAB*fAB;

	if(fabsf(fD)<0.00001f)
		return -1;

	*fU = (fPA*fBB - fPB*fAB)/fD;
	*fV = (fPB*fAA - fPA*fAB)/fD;

	return 0;
}



INT LcMath_SphereToTri(		VEC3* pOut					// 구와 삼각형의 최단거리의 지점
						,	FLOAT* fDist				// 구와 삼각형의 최단거리
						,	const VEC3* pSphereCenter	// 구의 중앙
						,	const FLOAT* pSphereRadius	// 구의 반경
						,	const VEC3* pTri			// 삼각형의 세점
						,	VEC3* pTriCenter			// 삼각형의 바운드구의 중심
						,	FLOAT* pTriRadius			// 삼각형의 바운드구의 반경
						,	VEC3* pTriNormal			// 삼각형의 노말멕터
						)
{
	VEC3	vTbndR;
	FLOAT	fTbndR;

	int		i=0;

	// 1. 삼각형의 충돌 바운드가 있다면..
	if(NULL == pTriCenter)
	{
		fTbndR = LcMath_TriangleBoundSphere(&vTbndR, pTri);
	}
	else
	{
		vTbndR	= *pTriCenter;
		fTbndR	= *pTriRadius;
	}

	VEC3	vcR = vTbndR - *pSphereCenter;
	FLOAT	fRadiusA = D3DXVec3LengthSq( &vcR);


	FLOAT fRadius12 = *pSphereRadius + fTbndR;

	fRadius12 = fRadius12 * fRadius12;

	if(fRadius12 < fRadiusA)
	{
		return -1;
	}


	// 1. 삼각형 만드는 평면에서 구의 중심까지의 최단거리를 만드는 지점과 거리를 구한다.
	// 최단거리가 구의 반경보다 크면 충돌을 안함.
	DPLN plane;
	LcMath_PlaneSet(&plane, &pTri[0], &pTri[1], &pTri[2]);

	*fDist = LcMath_PlaneToPointMinDistance((FLOAT*)&plane, (FLOAT*)pSphereCenter);

	if( fRadiusA < (*fDist) * (*fDist))
		return -1;

	FLOAT k = D3DXVec3Dot( (VEC3*)&plane, &vcR);

	*pOut = *pSphereCenter + k *  *((VEC3*)&plane);






	FLOAT fU, fV;

	if(FAILED(LcMath_UVFromTri(&fU, &fV, pOut, &pTri[0], &pTri[1], &pTri[2])))
		return -1;

	// 2. 앞서 구한 지점이 삼각형 내부에 있다면 무조건 충돌.
	if(fU>=-0.0001f && fU<=1.0001f && fV>=-0.0001f && fV<=1.0001f && (fU+fV)<1.0001f)
		return 0;

	// 3. 삼각형의 세 점우 하나라도 구 안에 있는가 검사.
	for(i=0; i<3; ++i)
	{
		VEC3	vcTc = pTri[i] - *pSphereCenter;
		FLOAT	fTr	= D3DXVec3LengthSq( &vcTc);

		if( fTr< (*pSphereRadius) * (*pSphereRadius))
			return 0;
	}


	// 4. 나머지는 삼각형이 구에 걸치는 형태
	// 앞서 구한 지점이 삼각형 외부에 존재한다면 삼각형이 만드는 세직선과 구의 충돌을 구한다.

	for(i=0; i<3; ++i)
	{
		VEC3 vcTstOut;
		FLOAT fTstDst;
		LCXLINE pLine;

		INT n0 = (i+0)%3;
		INT n1 = (i+1)%3;

		pLine.p = pTri[n0];
		pLine.t = pTri[n1] - pTri[n0];

		D3DXVec3Normalize(&pLine.t, &pLine.t);

		// 구와 충돌한 지점을 구하고...
		if(SUCCEEDED(LcMath_LineToSphere(&vcTstOut, &fTstDst, &pLine, pSphereCenter, *pSphereRadius)))
		{
			// 이 점이 유한 직선 안에 있는가 검사.

			VEC3 vcTA = pTri[n0] - vcTstOut;
			VEC3 vcTB = pTri[n1] - vcTstOut;

			FLOAT fTst = D3DXVec3Dot(&vcTA, &vcTB);

			//만약 안에 있다면 fTst<0이어야 한다.
			if(fTst<0.f)
				return 0;

		}
	}



	return -1;
}




INT LcMath_SphereToSphere(	const VEC3* pSphereCenter1							// 구의 중앙1
						,	const FLOAT* pSphereRadius1							// 구의 반경1
						,	const VEC3* pSphereCenter2							// 구의 중앙2
						,	const FLOAT* pSphereRadius2							// 구의 반경2
						)
{
	FLOAT fDst;
	VEC3 vcAB = *pSphereCenter2 - *pSphereCenter1;
	fDst = D3DXVec3Length(&vcAB);

	if( fDst < ( *pSphereRadius1 + *pSphereRadius2))
		return 0;

	return -1;
}




BOOL LcMath_RcCollision(LCXRECT* v1, LCXRECT* v2)
{
	return (v1->x0 <= v2->x1 && v1->x1 >= v2->x0 &&
			v1->y0 <= v2->y1 && v1->y1 >= v2->y0);
}







////////////////////////////////////////////////////////////////////////////////
// Color
////////////////////////////////////////////////////////////////////////////////


DWORD	LcColor_GetAlpha(DWORD c)	{	return (c>>24) & 0xff;	}
DWORD	LcColor_GetRed(DWORD c)		{	return (c>>16) & 0xff;	}
DWORD	LcColor_GetGreen(DWORD c)	{	return (c>>8) & 0xff;	}
DWORD	LcColor_GetBlue(DWORD c)	{	return c & 0xff;		}

void	LcColor_SetAlpha(DWORD& c, DWORD a)		{ c = ((a & 0xff)<<24) | (((c>>16)& 0xff)<<16) | ((c>>8 & 0xff)<<8) | (c & 0xff); }
void	LcColor_SetRed(DWORD& c,DWORD r)		{ c = (((c>>24) & 0xff)<<24) | ((r & 0xff)<<16) | ((c>>8 & 0xff)<<8) | (c & 0xff); }
void	LcColor_SetGreen(DWORD& c,DWORD g)		{ c = (((c>>24) & 0xff)<<24) | (((c>>16)& 0xff)<<16) | ((g & 0xff)<<8) | (c & 0xff); }
void	LcColor_SetBlue(DWORD& c,DWORD b)		{ c = (((c>>24) & 0xff)<<24) | (((c>>16)& 0xff)<<16) | ((c>>8 & 0xff)<<8) | (b & 0xff); }

WORD	LcColor_RGB16(INT r,INT g,INT b)		{	return (((r>>3) & 0x1F)<<10) | (((g>>3) & 0x1F)<<5) | ((b>>3) & 0x1F);							}
WORD	LcColor_RGBA16(INT r,INT g,INT b,INT a)	{	return  (((a>>7) & 0x1)<<15) | (((r>>3) & 0x1F)<<10) | (((g>>3) & 0x1F)<<5) | ((b>>3) & 0x1F);	}
WORD	LcColor_GetA1R5G5B5(DWORD c)			{	return LcColor_RGBA16(c>>16, c>>8, c, c>>24);													}
WORD	LcColor_X8R8G8B8toA1R5G5B5(INT color)	{	return LcColor_RGB16(color>>16, color>>8, color);												}

// Returns value from A1R5G5B5 color	
INT		LcColor_GetAlpha16(WORD color)			{	return ((color >> 11)&0x1);		}
INT		LcColor_GetRed16(WORD color)			{	return ((color >> 10)&0x1F);	}
INT		LcColor_GetGreen16(WORD color)			{	return ((color >> 5)&0x1F);		}
INT		LcColor_GetBlue16(WORD color)			{	return ((color)&0x1F);			}

INT		LcColor_GetLuminance(WORD color)		{	return ((LcColor_GetRed16(color)<<3) + (LcColor_GetGreen16(color)<<3) + (LcColor_GetBlue16(color)<<3)) / 3;		}
INT		LcColor_GetA1R5G5B5toA8R8G8B8(WORD color){	return	(((color >> 15)&0x1)<<31) |	(((color >> 10)&0x1F)<<19) |(((color >> 5)&0x1F)<<11) |	(color&0x1F)<<3;	}
INT		LcColor_GetR5G6B5toA8R8G8B8(WORD color)	{	return	0xFF000000 & ((((color >> 11)&0x1F)<<19) |	(((color >> 5)&0x3F)<<11) |	(color&0x1F)<<3);				}	



