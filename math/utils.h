#pragma once

#include <math.h>
#include <intrin.h>
#include <float.h>

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

//inline
//bool test_isnan(float _v)
//{
//	return _isnanf(_v)!=0;
//}

inline
bool test_isnan(double _v)
{
	return _isnan(_v)!=0;
}

template<typename _Type> inline
_Type clamp(const _Type& _v,const _Type& _min,const _Type& _max)
{
	_Type v = _v;
	if(v<_min) v = _min;
	if(v>_max) v = _max;
	return v;
}

template<typename _Type> inline
_Type sqr(const _Type& _d)
{
	return _d*_d;
}

template<class T> inline 
T cube(T a)
{
	return a*sqr(a);
}

template<typename _Type> inline
signed char sign(const _Type& _d)
{
	return _d<0?-1
		:_d>0?1
		:0
		;
}

extern real EPS;

template<typename _Type> inline
bool eql(const _Type& _d1,const _Type& _d2)
{
	return _d1==_d2;
}

inline
bool eql(real _d1,real _d2)
{
	return fabs(_d2-_d1)<=EPS;
}

inline
bool eql(real _d1,int _d2)
{
	return fabs(_d2-_d1)<=EPS;
}

inline
bool eql(real _d1,long _d2)
{
	return fabs(_d2-_d1)<=EPS;
}

template<typename _Type>
inline
_Type linear(const _Type& _from,const _Type& _to,real _t)
{
	real t = ::clamp<real>(_t,(real)0,(real)1);
	return (_Type)(_from*((real)1.0 - t) + _to*t);
}

template<typename _Type>
inline
_Type linear(const _Type& _from,const _Type& _to,real _tmin,real _tmax,real _t)
{
	real t = !eql(_tmax - _tmin,(real)0)?(_t-_tmin)/(_tmax - _tmin):(real)0.0;
	return linear(_from,_to,t);
}

inline
__int64 rand64()
{
	__int64 val = ::rand();
	val <<= 15;
	val |= ::rand();
	val <<= 15;
	val |= ::rand();
	val <<= 15;
	val |= ::rand();
	val <<= 15;
	val |= ::rand();
	val <<= 3;
	val |= ::rand() & 0x7;
	return val;
}

inline 
__int32 rand32()
{
	__int32 val = ::rand();
	val <<= 15;
	val |= ::rand();
	val <<= 1;
	val |= ::rand() & 0x1;
	return val;
}

template<typename _Type>
inline
_Type rand(_Type _min,_Type _max)
{
	return ::clamp<_Type>((_Type)(((__int64)::rand32()*(_max-_min))/MAXINT32 + _min),_min,_max);
}

template<typename _Type>
inline
_Type rand(_Type _max)
{
	return ::rand(_Type(),_max);
}

inline
real rand(real _min,real _max)
{
	return ::clamp<real>(((real)::rand32()/(real)MAXINT32)*(_max-_min) + _min,_min,_max);
}

inline
real rand(real _max)
{
	return rand((real)0.0,_max);
}

inline 
bool rand(bool _max)
{
	return (::rand()&1)==0;
}

struct gauss_rand
{
	gauss_rand()
		:m_bNext(false)
		,m_val(0.0)
	{
	}

	real operator () ()
	{
		if(m_bNext)
		{
			m_bNext = false;
			return m_val;
		}
		real u = 0,v = 0,r=0;
		do 
		{
			u = ::rand((real)-1.0,(real)1.0);
			v = ::rand((real)-1.0,(real)1.0);
			r = u*u + v*v;
		} while(r == (real)0.0 || r > (real)1.0);
		real c = sqrt(-2*log(r)/r);
		m_val = v*c; // cache this
		m_bNext = true;
		return u*c; 
	}

	real operator() (real _mu,real _std)
	{
		return operator()()*_std+_mu;
	}
protected:
	bool m_bNext;
	real m_val;
};

inline
real align_value(real _val,real _max_val)
{
	return tanh(_val/_max_val)*_max_val;
}

inline 
real norm_angle(real _angle)
{
	if(_angle<0 || _angle>(real)2*M_PI)
	{
		_angle = fmod(_angle,(real)(2*M_PI));
		if(_angle<0) _angle += (real)(2*M_PI);
	}
	return _angle;
}

