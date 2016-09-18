#pragma once

#include <vector>
#include <list>
#include <math.h>

#include "utils.h"

#if !defined(real)
#define real double
#endif

#if !defined(M_PI)
	#define M_PI       3.14159265358979323846
	#define M_PI_2     1.57079632679489661923
	#define M_PI_4     0.785398163397448309616
	#define M_1_PI     0.318309886183790671538
	#define M_2_PI     0.636619772367581343076
#endif

template<typename _Type>
struct Point2DT
{
	_Type x,y;

	Point2DT(const _Type& _x = 0,const _Type& _y = 0)
		:x(_x),y(_y)
	{}

	template<typename _Type2>
	Point2DT(const Point2DT<_Type2>& _pt)
		:x(_pt.x),y(_pt.y)
	{
	}

	Point2DT(const CPoint& _pt)
		:x(_pt.x),y(_pt.y)
	{
	}

	template<typename _Type2>
	Point2DT<_Type>& operator = (const Point2DT<_Type2>& _pt)
	{
		x = _pt.x;
		y = _pt.y;
		return *this;
	}

	Point2DT<_Type>& operator = (const CPoint& _pt)
	{
		x = _pt.x;
		y = _pt.y;
		return *this;
	}

	template<typename _Type2>
	bool operator == (const Point2DT<_Type2>& _pt) const
	{
		return eql(x, _pt.x)
			&& eql(y, _pt.y)
			;
	}

	template<typename _Type2>
	Point2DT<_Type>& operator += (const Point2DT<_Type2>& _pt)
	{
		x += _pt.x;
		y += _pt.y;
		return *this;
	}

	template<typename _Type2>
	Point2DT<_Type>& operator -= (const Point2DT<_Type2>& _pt)
	{
		x -= _pt.x;
		y -= _pt.y;
		return *this;
	}

	template<typename _Type2>
	Point2DT<_Type>& operator *= (const _Type2& _k)
	{
		x *= _k;
		y *= _k;
		return *this;
	}

	void rand_init(const Point2DT<_Type>& _tl,const Point2DT<_Type>& _rb)
	{
		x = ::rand(_tl.x,_rb.x);
		y = ::rand(_tl.y,_rb.y);
	}

	void norm()
	{
		_Type len = sqrt(sqr(x) + sqr(y));
		if(len!=0.0)
		{
			x/=len;
			y/=len;
		}
	}

	_Type length() const
	{
		return sqrt(sqr(x) + sqr(y));
	}

	template<typename _Type2>
	_Type scalar(const Point2DT<_Type2>& _pt) const
	{
		return x*_pt.x + y*_pt.y;
	}

	operator CPoint () const
	{
		CPoint pt;
		pt.x = (long)::floor(x);
		pt.y = (long)::floor(y);
		return pt;
	}
};//template<_Type> struct PointT

template<typename _Type>
inline
Point2DT<_Type> operator + (const Point2DT<_Type>& _p1,const Point2DT<_Type>& _p2)
{
	Point2DT<_Type> p = _p1;
	p += _p2;
	return p;
}

template<typename _Type>
inline
Point2DT<_Type> operator - (const Point2DT<_Type>& _p1,const Point2DT<_Type>& _p2)
{
	Point2DT<_Type> p = _p1;
	p -= _p2;
	return p;
}

template<typename _Type,typename _Type1>
inline
Point2DT<_Type> operator * (const Point2DT<_Type>& _p1,const _Type1& _k)
{
	Point2DT<_Type> p = _p1;
	p *= _k;
	return p;
}

template<typename _Type,typename _Type1>
inline
Point2DT<_Type> operator * (const _Type1& _k,const Point2DT<_Type>& _p1)
{
	Point2DT<_Type> p = _p1;
	p *= _k;
	return p;
}

template<typename _Type1,typename _Type2>
inline
bool eql(const Point2DT<_Type1>& _p1,const Point2DT<_Type2>& _p2)
{
	return eql(_p1.x,_p2.x) && eql(_p1.y,_p2.y);
}

template<typename _Type>
inline
_Type distance(const Point2DT<_Type>& _p1,const Point2DT<_Type>& _p2)
{
	return sqrt(sqr(_p1.x - _p2.x) + sqr(_p1.y - _p2.y));
}

template<typename _Type> inline
_Type angle(const Point2DT<_Type>& _vec1,const Point2DT<_Type>& _vec2)
{
	Point2DT<_Type> vec1 = _vec1;
	Point2DT<_Type> vec2 = _vec2;
	vec1.norm();
	vec2.norm();
	_Type v = scalar(vec1,vec2);	
	v = clamp<_Type>(v,-1,1);
	return acos(v);
}

template<typename _Type> inline
_Type scalar(const Point2DT<_Type>& _p1,const Point2DT<_Type>& _p2)
{
	return _p1.scalar(_p2);
}

typedef Point2DT<real> CFPoint2D;

template<typename _Type>
struct Point3DT
{
	_Type x,y,z,w;

	Point3DT(
		const _Type& _x = _Type()
		,const _Type& _y = _Type()
		,const _Type& _z = _Type()
		,const _Type& _w = 1
		)
		:x(_x),y(_y),z(_z),w(_w)
	{}

	template<typename _Type2>
	Point3DT(const Point3DT<_Type2>& _pt)
		:x(_pt.x),y(_pt.y),z(_pt.z),w(_pt.w)
	{
	}

	template<typename _Type2>
	Point3DT(const Point2DT<_Type2>& _pt)
		:x(_pt.x),y(_pt.y),z(_Type()),w(1)
	{
	}

	Point3DT(const CPoint& _pt)
		:x(_pt.x),y(_pt.y),z(_Type()),w(1)
	{
	}

	template<typename _Type2>
	Point3DT<_Type>& operator = (const Point3DT<_Type2>& _pt)
	{
		x = _pt.x;
		y = _pt.y;
		z = _pt.z;
		w = _pt.w;
		return *this;
	}

	Point3DT<_Type>& operator = (const CPoint& _pt)
	{
		x = _pt.x;
		y = _pt.y;
		z = _Type();
		w = 1;
		return *this;
	}

	template<typename _Type2>
	bool operator == (const Point3DT<_Type2>& _pt) const
	{
		return eql(x/w, _pt.x/_pt.w)
			&& eql(y/w, _pt.y/_pt.w)
			&& eql(z/w, _pt.z/_pt.w)
			;
	}

	template<typename _Type2>
	Point3DT<_Type>& operator += (const Point3DT<_Type2>& _pt)
	{
		x = x/w + _pt.x/_pt.w;
		y = y/w + _pt.y/_pt.w;
		z = z/w + _pt.z/_pt.w;
		w = 1;
		return *this;
	}

	template<typename _Type2>
	Point3DT<_Type>& operator -= (const Point3DT<_Type2>& _pt)
	{
		x = x/w - _pt.x/_pt.w;
		y = y/w - _pt.y/_pt.w;
		z = z/w - _pt.z/_pt.w;
		w = 1;
		return *this;
	}

	template<typename _Type2>
	_Type operator * (const Point3DT<_Type2>& _pt) const
	{
		return x/w*_pt.x/_pt.w + y/w*_pt.y/_pt.w + z/w*_pt.z/_pt.w;
	}

	Point3DT<_Type>& operator *= (const _Type& _k)
	{
		x *= _k;
		y *= _k;
		z *= _k;
		return *this;
	}

	Point3DT<_Type>& norm()
	{
		rew();
		_Type len = sqrt(sqr(x)+sqr(y)+sqr(z));
		if(len!=0.0)
		{
			*this *= (_Type)1.0/len;
		}
		w = 1;
		return *this;
	}

	void rew()
	{
		x /= w;
		y /= w;
		z /= w;
		w = 1;
	}

	_Type& operator [] (size_t _i)
	{
		switch(_i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}
		return Fish<_Type>::get();
	}

	const _Type& operator [] (size_t _i) const
	{
		switch(_i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}
		return Fish<_Type>::get();
	}

	template<typename _Type2>
	Point3DT<_Type> cross(const Point3DT<_Type2>& _b) const
	{
		Point3DT<_Type> pt;
		pt.x = (y*_b.z - z*_b.y)/(w*_b.w);
		pt.y = (z*_b.x - x*_b.z)/(w*_b.w);
		pt.z = (x*_b.y - y*_b.x)/(w*_b.w);
		pt.w = 1;
		return pt;
	}

	template<typename _Type2>
	_Type scalar(const Point3DT<_Type2>& _pt) const
	{
		return (*this)*_pt;
	}

	_Type length() const
	{
		return sqrt(sqr(x/w) + sqr(y/w) + sqr(z/w));
	}

	Point3DT<_Type>& mirror_x()
	{
		x = -x;
		return *this;
	}
};//template<_Type> struct PointT


template<typename _Type>
inline
Point3DT<_Type> operator + (const Point3DT<_Type>& _p1,const Point3DT<_Type>& _p2)
{
	Point3DT<_Type> p = _p1;
	p += _p2;
	return p;
}

template<typename _Type>
inline
Point3DT<_Type> operator - (const Point3DT<_Type>& _p1,const Point3DT<_Type>& _p2)
{
	Point3DT<_Type> p = _p1;
	p -= _p2;
	return p;
}

template<typename _Type>
inline
Point3DT<_Type> operator * (const _Type& _k,const Point3DT<_Type>& _p)
{
	Point3DT<_Type> p = _p;
	p *= _k;
	return p;
}

template<typename _Type>
inline
Point3DT<_Type> operator * (const Point3DT<_Type>& _p,const _Type& _k)
{
	Point3DT<_Type> p = _p;
	p *= _k;
	return p;
}

template<typename _Type>
inline
_Type distance(const Point3DT<_Type>& _p1,const Point3DT<_Type>& _p2)
{
	return sqrt(
		sqr(_p1.x/_p1.w - _p2.x/_p2.w) 
		+ sqr(_p1.y/_p1.w - _p2.y/_p2.w) 
		+ sqr(_p1.z/_p1.w - _p2.z/_p2.w)
		);
}

template<typename _Type> inline
Point3DT<_Type> cross(const Point3DT<_Type>& _a,const Point3DT<_Type>& _b)
{
	return _a.cross(_b);
}

template<typename _Type> inline
_Type scalar(const Point3DT<_Type>& _a,const Point3DT<_Type>& _b)
{
	return _a.scalar(_b);
}

template<typename _Type1,typename _Type2>
inline
bool eql(const Point3DT<_Type1>& _p1,const Point3DT<_Type2>& _p2)
{
	return 
		eql(_p1.x/_p1.w,_p2.x/_p2.w) 
		&& eql(_p1.y/_p1.w,_p2.y/_p2.w) 
		&& eql(_p1.z/_p1.w,_p2.z/_p2.w) 
		;
}


typedef Point3DT<real> CFPoint3D;


template<typename _Type>
struct RectT
{
	Point2DT<_Type> m_tl;
	Point2DT<_Type> m_br;

	RectT(const Point2DT<_Type>& _tl,const Point2DT<_Type>& _br)
		:m_tl(_tl),m_br(_br)
	{
	}

	bool inside(const Point2DT<_Type>& _pt)
	{
		return 
			_pt.x>=m_tl.x && _pt.x<=m_br.x
			&& _pt.y>=m_tl.y && _pt.y<=m_br.y
			;
	}
};

typedef RectT<real> CFRect;

template<typename _Type>
struct CArr
{
	std::list<_Type>& m_list;
	CArr(std::list<_Type>& _list)
		:m_list(_list)
	{}

	_Type& operator [] (size_t _i) const
	{
		std::list<_Type>::iterator 
			it = m_list.begin()
			;
		if(_i>=m_list.size()) return Fish<_Type>::get();
		std::advance(it,_i);
		return *it;
	}
};

template<typename _Type>
struct CArrCnst
{
	const std::list<_Type>& m_list;
	CArrCnst(const std::list<_Type>& _list)
		:m_list(_list)
	{}

	const _Type& operator [] (size_t _i) const
	{
		std::list<_Type>::const_iterator 
			it = m_list.begin()
			;
		if(_i>=m_list.size()) return Fish<_Type>::get();
		std::advance(it,_i);
		return *it;
	}

};

template<typename _Type> inline
CArr<_Type> arr(std::list<_Type>& _lst)
{
	return CArr<_Type>(_lst);
}

template<typename _Type> inline
CArrCnst<_Type> arr(const std::list<_Type>& _lst)
{
	return CArrCnst<_Type>(_lst);
}


template<typename _Type>
inline
void swap_data(_Type& _d1,_Type& _d2)
{
	_Type d = _d1;
	_d1 = _d2;
	_d2 = d;
}

template<typename _Type> inline
bool between_values(const _Type& _d,const _Type& _d1,const _Type& _d2)
{
	return _d1<=_d && _d<=_d2;
}

template<typename _Type> inline 
_Type get_angle(const Point2DT<_Type>& _vec1,const Point2DT<_Type>& _vec2)
{
	_Type v = 0;
	_Type len = _vec1.length()*_vec2.length();
	if(!eql(len,0))
	{
		v = scalar(_vec1,_vec2)/len;
		v = clamp<_Type>(v,-1,1);
		v = acos(v);
	}
	else
	{
		v = 0;
	}
	v *= sign(cross(Point3DT<_Type>(_vec1),Point3DT<_Type>(_vec2)).z);
	return v;
}

//inline
//void trace(LPCTSTR _szFormat, ...)
//{
//	va_list args;
//	va_start(args,_szFormat);
//	FILE* f=NULL;
//	fopen_s(&f,"trace.txt","at");
//	vfwprintf_s(f,_szFormat,args);
//	fclose(f);
//}

struct CMatrix
{
	real d[16];


//	struct DefaultLayout : public Layout<CMatrix>
//	{
//		DefaultLayout()
//		{
//			add_array(_T("d"),&CMatrix::d,get_primitivelayout<double>());
//		}
//	};

	CMatrix()
	{
		zero();
	}

	void zero()
	{
		size_t i=0;
		for(i=0;i<_countof(d);i++) d[i] = 0;
	}

	void identity()
	{
		set(
			1,	0,	0,	0,
			0,	1,	0,	0,
			0,	0,	1,	0,
			0,	0,	0,	1
			);
	}

	void translate(real _x,real _y,real _z)
	{
		zero();
		set(
			1,	0,	0,	_x,
			0,	1,	0,	_y,
			0,	0,	1,	_z,
			0,	0,	0,	1
			);
	}

	void rotate(real _angle,real _x,real _y,real _z)
	{
		zero();
		CFPoint3D pt(_x,_y,_z);
		pt.norm();
		real x = pt.x;
		real y = pt.y;
		real z = pt.z;
		real c = cos(_angle);
		real s = sin(_angle);

		set(
			x*x*(1-c) + c,		x*y*(1-c) - z*s,	x*z*(1-c) + y*s,	0,
			y*x*(1-c) + z*s,	y*y*(1-c) + c,		y*z*(1-c) - x*s,	0,
			x*z*(1-c) - y*s,	y*z*(1-c) + x*s,	z*z*(1-c) + c,		0,
			0,					0,					0,					1
			);
	}

	real& m(size_t _y,size_t _x)
	{
		return *(d + _y*4+_x);
	}

	real& m1(size_t _x,size_t _y)
	{
		return m(_y,_x);
	}

	const real& m(size_t _y,size_t _x) const
	{
		return *(d + _y*4+_x);
	}

	real& m(real* _m,size_t _y,size_t _x)
	{
		return *(_m + _y*4+_x);
	}

	void set(
		real _a00,real _a01,real _a02,real _a03
		,real _a10,real _a11,real _a12,real _a13
		,real _a20,real _a21,real _a22,real _a23
		,real _a30,real _a31,real _a32,real _a33
		)
	{
		m(0,0) = _a00;
		m(0,1) = _a01;
		m(0,2) = _a02;
		m(0,3) = _a03;
		m(1,0) = _a10;
		m(1,1) = _a11;
		m(1,2) = _a12;
		m(1,3) = _a13;
		m(2,0) = _a20;
		m(2,1) = _a21;
		m(2,2) = _a22;
		m(2,3) = _a23;
		m(3,0) = _a30;
		m(3,1) = _a31;
		m(3,2) = _a32;
		m(3,3) = _a33;
	}

	void multiply(const CMatrix& _matrix)
	{
		real res[16];
		size_t i=0,j=0;
		for(i=0;i<_countof(res);i++) res[i] = 0;
		for(i=0;i<4;i++)
		for(j=0;j<4;j++)
		{
			size_t k=0;
			for(k=0;k<4;k++)
				m(res,i,j) += m(i,k)*_matrix.m(k,j);
		}
		for(i=0;i<_countof(res);i++) d[i] = res[i];
	}

	CMatrix& operator = (const CMatrix& _m)
	{
		size_t i=0;
		for(i=0;i<_countof(d);i++) d[i] = _m.d[i];
		return *this;
	}

	CFPoint3D operator * (const CFPoint3D& _pt) const
	{
		CFPoint3D pt(0,0,0,0);
		size_t i=0;
		for(i=0;i<4;i++)
		{
			size_t k=0;
			for(k=0;k<4;k++)
				pt[i] += m(i,k)*_pt[k];
		}
		return pt;
	}
};

struct CCoordinatesTransform
{
	std::list<CMatrix> m_stack;

	CCoordinatesTransform()
	{
		CMatrix m;
		m.identity();
		m_stack.push_back(m);
	}

	CMatrix& top()
	{
		VERIFY_EXIT1(m_stack.size()>0,Fish<CMatrix>::get());
		return m_stack.front();
	}

	const CMatrix& top() const
	{
		VERIFY_EXIT1(m_stack.size()>0,Fish<CMatrix>::get());
		return m_stack.front();
	}

	void push()
	{
		m_stack.push_front(m_stack.front());
	}

	void pop()
	{
		VERIFY_EXIT(m_stack.size()>1);
		m_stack.erase(m_stack.begin());
	}

	void set(const CMatrix& _m)
	{
		VERIFY_EXIT(m_stack.size()>0);
		m_stack.front() = _m;
	}

	void zero()
	{
		VERIFY_EXIT(m_stack.size()>0);
		m_stack.front().zero();
	}

	void identity()
	{
		VERIFY_EXIT(m_stack.size()>0);
		m_stack.front().identity();
	}

	void translate(const CFPoint3D& _pt)
	{
		translate(_pt.x,_pt.y,_pt.z);
	}

	void translate(real _x,real _y,real _z)
	{
		VERIFY_EXIT(m_stack.size()>0);
		CMatrix m;
		m.translate(_x,_y,_z);
		m_stack.front().multiply(m);
	}

	void rotate(real _angle,real _x,real _y,real _z)
	{
		VERIFY_EXIT(m_stack.size()>0);
		CMatrix m;
		m.rotate(_angle,_x,_y,_z);
		m_stack.front().multiply(m);
	}

	CFPoint3D transform(const CFPoint3D& _pt)
	{
		VERIFY_EXIT1(m_stack.size()>0,_pt);
		return m_stack.front()*_pt;
	}
};//struct CCoordinatesTransform

struct CCoordinatesTests
{
	void test()
	{
		CCoordinatesTransform view;
		CFPoint3D pt1(0,0,0);
		CFPoint3D pt2 = view.transform(pt1);
		ATLASSERT(pt2==pt1);
		view.push();
		view.translate(2,0,0);
		pt2 = view.transform(pt1);
		ATLASSERT(pt2==CFPoint3D(2,0,0));
		view.pop();
		view.push();
		view.translate(0,2,0);
		pt2 = view.transform(pt1);
		ATLASSERT(pt2==CFPoint3D(0,2,0));
		view.pop();
		view.push();
		view.translate(0,0,2);
		pt2 = view.transform(pt1);
		ATLASSERT(pt2==CFPoint3D(0,0,2));
		view.pop();
		view.push();
		view.rotate((real)(-M_PI/2),1,0,0);
		view.translate(0,0,2);
		pt2 = view.transform(pt1);
		ATLASSERT(pt2==CFPoint3D(0,2,0));

		view.pop();
		view.push();
		view.translate(3,0,0);
		view.translate(1,0,0);
		view.translate(1,0,0);
		pt2 = view.transform(pt1);
		ATLASSERT(pt2==CFPoint3D(5,0,0));
	}
};


