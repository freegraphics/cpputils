#if !defined(__UTILS_H__152B8A8E_DCAB_4d9c_8CF0_3505A799795C__)
#define __UTILS_H__152B8A8E_DCAB_4d9c_8CF0_3505A799795C__

#pragma once

#if !defined(real)
#define real double
#endif

#include <math/utils.h>
#include <math/graphics.h>

template<typename _Type>
struct var
{
protected:
	template<typename _Type>
	struct var_obj
	{
		long m_counter;
		_Type* m_ptr;

		var_obj(_Type* _pobj)
			:m_ptr(_pobj)
			,m_counter(1)
		{
		}

		static var_obj<_Type>* alloc(_Type* _ptr)
		{
			return new var_obj<_Type>(_ptr);
		}

		static void free(var_obj<_Type>* _pobjptr)
		{
			if(_pobjptr==NULL) return;
			if(--_pobjptr->m_counter==0)
			{
				delete _pobjptr->m_ptr;
				_pobjptr->m_ptr = NULL;
				delete _pobjptr;
			}
		}

		static void copy(var_obj<_Type>*& _pobjptr1,var_obj<_Type>* _pobjptr2)
		{
			free(_pobjptr1);
			_pobjptr1 = _pobjptr2;
			if(NOT_NULL(_pobjptr1))
				_pobjptr1->m_counter++;
		}
	};
public:
	var()
		:m_shareobj(NULL)
	{
	}

	var(_Type* _pobj)
	{
		m_shareobj = var_obj<_Type>::alloc(_pobj);
	}

	var(const var<_Type>& _var)
		:m_shareobj(NULL)
	{
		var_obj<_Type>::copy(m_shareobj,_var.m_shareobj);
	}

	~var()
	{
		var_obj<_Type>::free(m_shareobj);
	}

	var<_Type>& operator = (const var<_Type>& _var)
	{
		var_obj<_Type>::copy(m_shareobj,_var.m_shareobj);
		return *this;
	}

	var<_Type>& operator = (_Type* _pobj)
	{
		var_obj<_Type>::free(m_shareobj);
		m_shareobj = var_obj<_Type>::alloc(_pobj);
		return *this;
	}

	_Type* operator -> () 
	{
		return m_shareobj->m_ptr;
	}

	const _Type* operator -> () const
	{
		return m_shareobj->m_ptr;
	}

	_Type& operator * ()
	{
		return *m_shareobj->m_ptr;
	}

	const _Type& operator * () const
	{
		return *m_shareobj->m_ptr;
	}

	_Type* get() 
	{
		return m_shareobj->m_ptr;
	}

	const _Type* get() const
	{
		return m_shareobj->m_ptr;
	}
protected:
	var_obj<_Type>* m_shareobj;

private:
	operator _Type& ();					// can`t assign from var<_Type> to _Type. use var<_Type> 
	operator const _Type& () const;		// can`t assign from var<_Type> to _Type. use var<_Type> 
};// template<> struct var


/*
// return max and min of a given non-empty array.
var maxmin = function(w) {
	if(w.length === 0) { return {}; } // ... ;s
	var maxv = w[0];
	var minv = w[0];
	var maxi = 0;
	var mini = 0;
	var n = w.length;
	for(var i=1;i<n;i++) {
		if(w[i] > maxv) { maxv = w[i]; maxi = i; } 
		if(w[i] < minv) { minv = w[i]; mini = i; } 
	}
	return {maxi: maxi, maxv: maxv, mini: mini, minv: minv, dv:maxv-minv};
}
*/

inline
void maxmin(std::vector<real>& _w,size_t& _maxi,real& _maxv,size_t& _mini,real& _minv,real& _dv)
{
	_maxi = 0;
	_maxv = 0;
	_mini = 0;
	_minv = 0;
	_dv = 0;
	if(_w.empty()) return;
	_maxv = _w[0];
	_minv = _w[0];
	size_t i=0,cnt=_w.size();
	for(i=0;i<cnt;i++)
	{
		if(_w[i]>_maxv){_maxv=_w[i];_maxi=i;}
		if(_w[i]<_minv){_minv=_w[i];_mini=i;}
	}
	_dv = _maxv-_minv;
}

template<typename _stream>
inline
void to_stream(_stream& _out,const std::vector<real>& _vec)
{
	_out << _vec.size();
	size_t i=0;
	for(i=0;i<_vec.size();i++)
	{
		_out << _vec[i];
	}
}

template<typename _stream>
inline
void from_stream(const _stream& _in,std::vector<real>& _vec,bool _bcreate=false)
{
	size_t sz = 0;
	_in >> sz;
	if(_bcreate)
		_vec.resize(sz);

	VERIFY_EXIT(sz==_vec.size());
	size_t i=0;
	for(i=0;i<sz;i++)
	{
		_in >> _vec[i];
	}
}

//////////////////////////////////////////////////////////////////////////

inline
void to_vec(const CFPoint2D& _pt,std::vector<real>& _vec,long& _idx)
{
	VERIFY_EXIT(_idx>=0 && _idx+2<=(long)_vec.size());
	_vec[_idx++] = clamp<real>(_pt.x,0,1) - (real)0.5;
	_vec[_idx++] = clamp<real>(_pt.y,0,1) - (real)0.5;
}

inline
void to_vec(const CFPoint3D& _pt,std::vector<real>& _vec,long& _idx)
{
	VERIFY_EXIT(_idx>=0 && _idx+3<=(long)_vec.size());
	_vec[_idx++] = clamp<real>(_pt.x/_pt.w,0,1) - (real)0.5;
	_vec[_idx++] = clamp<real>(_pt.y/_pt.w,0,1) - (real)0.5;
	_vec[_idx++] = clamp<real>(_pt.z/_pt.w,0,1) - (real)0.5;
}

inline
void to_vec(const std::vector<real>& _data,std::vector<real>& _vec,long& _idx,real _offs = 0.5)
{
	VERIFY_EXIT(_idx>=0 && _idx+(long)_data.size()<=(long)_vec.size());
	if(eql(_offs,(real)0.0))
	{
		std::copy(_data.begin(),_data.end(),_vec.begin()+_idx);
		_idx += _data.size();
	}	
	else
	{
		size_t i=0;
		for(i=0;i<_data.size();i++)
		{
			_vec[_idx++] = _data[i] - _offs;
		}
	}
}

inline
void to_vec(const real& _val,std::vector<real>& _vec,long& _idx,real _offs = 0.5)
{
	VERIFY_EXIT(_idx>=0 && _idx+1<=(long)_vec.size());
	_vec[_idx++] = _val - _offs;
}

template<typename _Iterator> inline 
void to_vec(_Iterator _it,_Iterator _ite,std::vector<real>& _vec,long& _idx)
{
	for(;_it!=_ite;+_it)
	{
		to_vec(*_it,_vec,_idx);
	}
}

template<typename _Type> inline
void to_vec(const std::vector<_Type>& _data,std::vector<real>& _vec,long& _idx)
{
	to_vec(_data.begin(),_data.end(),_vec,_idx);
}

inline 
void from_vec(CFPoint2D& _pt,const std::vector<real>& _vec,long& _idx,real _offs = 0.5)
{
	VERIFY_EXIT(_idx>=0 && _idx+2<=(long)_vec.size());
	_pt.x = clamp<real>(_vec[_idx++] + _offs, (real)0,(real)1);
	_pt.y = clamp<real>(_vec[_idx++] + _offs, (real)0,(real)1);
}

inline 
void from_vec(CFPoint3D& _pt,const std::vector<real>& _vec,long& _idx,real _offs = 0.5)
{
	VERIFY_EXIT(_idx>=0 && _idx+3<=(long)_vec.size());
	_pt.x = clamp<real>(_vec[_idx++] + _offs, (real)0,(real)1);
	_pt.y = clamp<real>(_vec[_idx++] + _offs, (real)0,(real)1);
	_pt.z = clamp<real>(_vec[_idx++] + _offs, (real)0,(real)1);
}

inline
void from_vec(std::vector<real>& _data,const std::vector<real>& _vec,long& _idx,real _offs = 0.5)
{
	VERIFY_EXIT(_idx>=0 && _idx+(long)_data.size()<=(long)_vec.size());
	if(eql(_offs,(real)0))
	{	
		std::copy(_vec.begin()+_idx,_vec.begin()+_idx+_data.size(),_data.begin());
		_idx += _data.size();
	}
	else
	{
		size_t i=0;
		for(i=0;i<_data.size();i++)
		{
			_data[i] = clamp<real>(_vec[_idx++] + _offs, (real)0,(real)1);
		}
	}
}

inline
void from_vec(real& _val,const std::vector<real>& _vec,long& _idx,real _offs = 0.5)
{
	VERIFY_EXIT(_idx>=0 && _idx+1<=(long)_vec.size());
	_val = _vec[_idx++] + _offs;
}

template<typename _Iterator> inline 
void from_vec(_Iterator _it,_Iterator _ite,const std::vector<real>& _vec,long& _idx)
{
	for(;_it!=_ite;+_it)
	{
		from_vec(*_it,_vec,_idx);
	}
}

template<typename _Type> inline
void from_vec(std::vector<_Type>& _data,const std::vector<real>& _vec,long& _idx)
{
	from_vec(_data.begin(),_data.end(),_vec,_idx);
}

template<typename _Base>
struct Endpoint_ToVec_Func_Impl
{
	void to_vec(std::vector<real>& _vec) const
	{
		long idx = 0;
		static_cast<const _Base&>(*this).to_vec(_vec,idx);
	}
};

template<typename _Base>
struct Endpoint_FromVec_Func_Impl
{
	void from_vec(const std::vector<real>& _vec)
	{
		long idx = 0;
		static_cast<_Base&>(*this).from_vec(_vec,idx);
	}
};

#define VERIFY_VEC_SIZE() VERIFY_EXIT(_idx>=0 && _idx+size()<=_vec.size());


template<typename _Type>
inline 
_Type align_value_to_vec(_Type _val,_Type _max_val)
{
	return tanh(_val/_max_val) - (_Type)0.5;
}

template<typename _Type>
inline 
_Type align_value_to_vec(_Type _val,_Type _min_val,_Type _max_val)
{
	return tanh((_val-_min_val)/(_max_val-_min_val)-(_Type)0.5);
}

template<typename _Type>
inline
_Type angle_to_vec(_Type _angle)
{
	if(_angle<0 || _angle>(_Type)2*(_Type)M_PI)
	{
		_angle = fmod(_angle,(_Type)2*(_Type)M_PI);
		if(_angle<0) _angle += (_Type)2*(_Type)M_PI;
	}
	return _angle/((_Type)2*(_Type)M_PI) - (_Type)0.5;
}

template<typename _Type>
inline
_Type vec_to_angle(_Type _vec_value)
{
	return (_vec_value+(_Type)0.5)*((_Type)2*(_Type)M_PI);
}



#endif //#if !defined(__UTILS_H__152B8A8E_DCAB_4d9c_8CF0_3505A799795C__)