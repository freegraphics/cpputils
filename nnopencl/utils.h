#pragma once 

#if !defined(real)
#define real float
#endif

#include <math/utils.h>
#include <nn/utils.h>

#if !defined(__CL_ENABLE_EXCEPTIONS)
#define __CL_ENABLE_EXCEPTIONS
#endif

#include <CL/cl.hpp>
#pragma comment(lib,"opencl.lib")

struct NaNException{};

namespace opencl
{

	inline 
		size_t divisible_8(size_t _n)
	{
		return ((_n+1)+7) & ~7;
	}


	inline
		size_t find_dividable(size_t _val,size_t _max_devidable)
	{
		size_t min_groups = _val/_max_devidable + 1;
		while(_val/min_groups>1)
		{
			if(_val%min_groups==0)
				return _val/min_groups;
			min_groups++;
		}
		return 1;
	}

	inline 
		cl::NDRange min_range(const cl::NDRange& _global,const cl::NDRange& _local)
	{
		VERIFY_EXIT1(_global.dimensions()==_local.dimensions(),_local);
		switch(_global.dimensions())
		{
		case 0:
			return cl::NullRange;
		case 1:
			{
				size_t global = ((const ::size_t*)_global)[0];
				size_t local = ((const ::size_t*)_local)[0];
				if(global<local)
					return cl::NDRange(global);
				return cl::NDRange(find_dividable(global,local));
			}
		case 2:
			{
				size_t global_0 = ((const ::size_t*)_global)[0];
				size_t local_0 = ((const ::size_t*)_local)[0];
				size_t global_1 = ((const ::size_t*)_global)[1];
				size_t local_1 = ((const ::size_t*)_local)[1];
				if(global_0<local_0 && global_1<local_1)
					return cl::NDRange(global_0,global_1);
				return cl::NDRange(local_0,local_1); // we use local size that always divide global size 
				//return cl::NDRange(
				//	find_dividable(global_0,local_0)
				//	,find_dividable(global_1,local_1)
				//	);
			}
		case 3:
			{
				size_t global_0 = ((const ::size_t*)_global)[0];
				size_t local_0 = ((const ::size_t*)_local)[0];
				size_t global_1 = ((const ::size_t*)_global)[1];
				size_t local_1 = ((const ::size_t*)_local)[1];
				size_t global_2 = ((const ::size_t*)_global)[1];
				size_t local_2 = ((const ::size_t*)_local)[1];
				if(global_0<local_0 && global_1<local_1 && global_2<local_2)
					return cl::NDRange(global_0,global_1,global_2);
				return cl::NDRange(local_0,local_1,local_2);
				//return cl::NDRange(
				//	find_dividable(global_0,local_0)
				//	,find_dividable(global_1,local_1)
				//	,find_dividable(global_2,local_2)
				//	);
			}
		}
		return cl::NullRange;
	}
}

