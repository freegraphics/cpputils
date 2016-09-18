#pragma once

#include <utils/utils.h>
#include "utils.h"

template<typename _Iterator> inline
typename _Iterator::value_type max_of_range(_Iterator _it,const _Iterator& _ite)
{
	typedef typename _Iterator::value_type value_type;
	if(EQL(_it,_ite)) return value_type();
	value_type res = *_it;
	++_it;
	for(;_it!=_ite;++_it)
	{
		if(*_it>res) res = *_it;
	}
	return res;
}

template<typename _Iterator> inline
typename _Iterator::value_type min_of_range(_Iterator _it,const _Iterator& _ite)
{
	typedef typename _Iterator::value_type value_type;
	if(EQL(_it,_ite)) return value_type();
	value_type res = *_it;
	++_it;
	for(;_it!=_ite;++_it)
	{
		if(*_it<res) res = *_it;
	}
	return res;
}

template<typename _Iterator> inline
typename _Iterator::value_type avg_of_range(_Iterator _it,const _Iterator& _ite)
{
	typedef typename _Iterator::value_type value_type;
	if(EQL(_it,_ite)) return value_type();
	value_type res = value_type();
	long cnt = std::distance(_it,_ite);
	for(;_it!=_ite;++_it)
	{
		res += *_it;
	}
	return res/(value_type)cnt;
}

template<typename _Iterator> inline
void get_histogram_of_range(_Iterator _it,const _Iterator& _ite,std::vector<size_t>& _hist)
{
	typedef typename _Iterator::value_type value_type;
	for(;_it!=_ite;++_it)
	{
		if(*_it>=_hist.size())
			_hist.resize(*_it+1);
		_hist[*_it]++;
	}
}

