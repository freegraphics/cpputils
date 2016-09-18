#pragma once

#include "utils.h"

namespace opencl
{
	struct Data
	{
	protected:
		std::vector<real> host_data;
		size_t M,N;
		cl::Buffer cl_buffer;
		bool m_modified_on_gpu_flag;
		bool m_modified_on_cpu_flag;

	public:
		Data()
			:M(0),N(0)
			,m_modified_on_gpu_flag(false)
			,m_modified_on_cpu_flag(false)
		{
		}

		void modify_on_cpu()
		{
			m_modified_on_cpu_flag = true;
		}

		void create(size_t _M,size_t _N,const cl::Context& _context)
		{
			M = _M;
			N = _N;
			host_data.resize(M*N);
			cl_buffer = cl::Buffer(_context,host_data.begin(),host_data.end(),false,false);
			modify_on_cpu();
		}

		real* get_data()
		{
			modify_on_cpu();
			return &host_data.front();
		}

		const real* get_data() const 
		{
			return &host_data.front();
		}

		size_t get_size() const
		{
			return M*N;
		}

		void create_copy(Data& _copy_of,const cl::Context& _context)
		{
			M = _copy_of.M;
			N = _copy_of.N;
			host_data.resize(M*N);
			cl_buffer = cl::Buffer(_context,host_data.begin(),host_data.end(),false,false);
			modify_on_cpu();
		}

		size_t get_M() const {return M;}
		size_t get_N() const {return N;}

		bool is_any_nan() const 
		{
			size_t m = 0,n = 0;
			for(m=0;m<M;m++)
			for(n=0;n<N;n++)
			{
				real v = get(m,n);
				if(!(v==v))// test is nan for float
				{
					return true;
				}
			}
			return false;
		}

//		bool is_bad_values() const // TODO: rename to is_bad_input
//		{
//			size_t m = 0,n = 0;
//			for(m=0;m<M;m++)
//			for(n=0;n<N;n++)
//			{
//				if(fabs(get(m,n))>1.0) return true;
//			}
//			return false;
//		}

		bool debug_test_isnan(bool _test_values = true) const 
		{
			if(is_any_nan())
			{
#if !defined(_DEBUG)
				static int exception_cnt = 0;
				if(!exception_cnt++) throw NaNException();
#endif
				return true;
			}
			return false;
		}

		size_t get_data_offs(size_t _m,size_t _n) const
		{
			VERIFY_EXIT1(_m<M && _n<N,0);
			return _m*N+_n;
		}

		real get(size_t _m, size_t _n) const 
		{
			VERIFY_EXIT1(_m<M && _n<N,(real)0);
			return host_data[get_data_offs(_m,_n)];
		}

		void set(size_t _m,size_t _n,real _val)
		{
			VERIFY_EXIT(_m<M && _n<N);
			host_data[get_data_offs(_m,_n)] = _val;
			modify_on_cpu();
		}

		void add(size_t _m,size_t _n,real _val)
		{
			VERIFY_EXIT(_m<M && _n<N);
			host_data[get_data_offs(_m,_n)] += _val;
			modify_on_cpu();
		}

		void mul(size_t _m,size_t _n,real _val)
		{
			VERIFY_EXIT(_m<M && _n<N);
			host_data[get_data_offs(_m,_n)] *= _val;
			modify_on_cpu();
		}

		void zero()
		{
			std::fill(host_data.begin(),host_data.end(),(real)0);
			modify_on_cpu();
		}

		void rand()
		{
			size_t m = 0,n = 0;
			for(m=0;m<M;m++)
			for(n=0;n<N;n++)
			{
				set(m,n,::rand((real)1.0));
			}
			modify_on_cpu();
		}

		void rand(real _min,real _max)
		{
			size_t m = 0,n = 0;
			for(m=0;m<M;m++)
			for(n=0;n<N;n++)
			{
				set(m,n,::rand(_min,_max));
			}
			modify_on_cpu();
		}

		void set_uniform_rand(real _max)
		{
			size_t m = 0,n = 0;
			for(m=0;m<M;m++)
			for(n=0;n<N-1;n++)
			{
				set(m,n,::rand(-_max,_max));
			}
			modify_on_cpu();
		}

		void fill(real _val)
		{
			size_t m = 0,n = 0;
			std::fill(host_data.begin(),host_data.end(),_val);
			modify_on_cpu();
		}

		void copy_of(const Data& _d1)
		{
			VERIFY_EXIT(get_M()==_d1.get_M() && get_N()==_d1.get_N());
			std::copy(_d1.host_data.begin(),_d1.host_data.end(),host_data.begin());
			modify_on_cpu();
		}

		void copy_from(const Data& _d1,size_t _width)
		{
			VERIFY_EXIT(get_M()==_d1.get_M() && _width<=get_N() && _width<=_d1.get_N());
			size_t m=0,n=0;
			for(m=0;m<get_M();m++)
			{
				std::copy(_d1.host_data.begin() + get_data_offs(m,0)
					,_d1.host_data.begin() + get_data_offs(m,0) + _width
					,host_data.begin() + get_data_offs(m,0)
					);
			}
			modify_on_cpu();
		}

		void add(const Data& _d1,size_t _width)
		{
			VERIFY_EXIT(get_M()==_d1.get_M() && _width<=get_N() && _width<=_d1.get_N());
			size_t m=0,n=0;
			for(m=0;m<get_M();m++)
			for(n=0;n<_width;n++)
			{
				add(m,n,_d1.get(m,n));
			}
			modify_on_cpu();
		}

		void mul(real _val)
		{
			size_t i = 0;
			for(i=0;i<host_data.size();i++)
			{
				host_data[i]*=_val;
			}
			modify_on_cpu();
		}

		void copy_data_to_host(const cl::CommandQueue& _queue)
		{
			cl::copy(_queue,cl_buffer,host_data.begin(),host_data.end());
			m_modified_on_gpu_flag = false;
			m_modified_on_cpu_flag = false;
			debug_test_isnan();
		}

		void copy_data_to_gpu(const cl::CommandQueue& _queue)
		{
			debug_test_isnan();
			cl::copy(_queue,host_data.begin(),host_data.end(),cl_buffer);
			m_modified_on_gpu_flag = false;
			m_modified_on_cpu_flag = false;
		}

		void update_from_gpu(DLNetKernels* _kernels)
		{
			VERIFY_EXIT(NOT_NULL(_kernels));
			if(!is_modified_on_gpu()) return;
			_kernels->wait();
			copy_data_to_host(_kernels->get_env()->get_queue());
		}

		void update_from_cpu(DLNetKernels* _kernels)
		{
			VERIFY_EXIT(NOT_NULL(_kernels));
			if(!is_modified_on_cpu()) return;
			_kernels->wait();
			copy_data_to_gpu(_kernels->get_env()->get_queue());
		}

		void modify_on_gpu()
		{
			m_modified_on_gpu_flag = true;
		}

		bool is_modified_on_gpu() const 
		{
			return m_modified_on_gpu_flag;
		}

		bool is_modified_on_cpu() const 
		{
			return m_modified_on_cpu_flag;
		}

		cl::Buffer& get_buffer()
		{
			return cl_buffer;
		}

		void set_row(size_t _row_idx,const std::vector<real>& _vec)
		{
			VERIFY_EXIT(_vec.size()<=N-1);
			VERIFY_EXIT(_row_idx<M);
			size_t m=0,n=0;
			m = _row_idx;
			std::copy(_vec.begin(),_vec.end()
				,host_data.begin() + get_data_offs(m,0)
				);
			modify_on_cpu();
		}

		void get_row(size_t _row_idx,std::vector<real>& _vec) const 
		{
			VERIFY_EXIT(_vec.size()<=N-1);
			VERIFY_EXIT(_row_idx<M);
			size_t m=0,n=0;
			m = _row_idx;
			std::copy(host_data.begin()+get_data_offs(m,0)
				,host_data.begin()+get_data_offs(m,0) + _vec.size()
				,_vec.begin()
				);
		}

		void set_row(size_t _row_idx,const std::vector<real>& _vec,size_t _offs,size_t _sz)
		{
			VERIFY_EXIT(_offs+_sz<=_vec.size());
			VERIFY_EXIT(_sz<=N-1);
			VERIFY_EXIT(_row_idx<M);
			size_t m=0,n=0;
			m = _row_idx;
			std::copy(_vec.begin() + _offs,_vec.end() + _offs + _sz
				,host_data.begin() + get_data_offs(m,0)
				);
			modify_on_cpu();
		}

		void get_row(size_t _row_idx,std::vector<real>& _vec,size_t _offs,size_t _sz)
		{
			VERIFY_EXIT(_offs+_sz<=_vec.size());
			VERIFY_EXIT(_sz<=N-1);
			VERIFY_EXIT(_row_idx<M);
			size_t m=0,n=0;
			m = _row_idx;
			std::copy(host_data.begin()+get_data_offs(m,0)
				,host_data.begin()+get_data_offs(m,0) + _sz
				,_vec.begin() + _offs
				);
		}

		real get_loss() const 
		{
			real v = 0;
			size_t m = 0,n = 0;
			for(m=0;m<M;m++)
			for(n=0;n<N-1;n++)
			{
				v += sqr(get(m,n));
			}
			return sqrt(v/(real)M);
		}

		real diff(const Data& _d1) const 
		{
			VERIFY_EXIT1(get_M()==_d1.get_M() && get_N()==_d1.get_N(),(real)100.0);
			size_t m=0,n=0;
			real sum = 0;
			for(m=0;m<get_M();m++)
			for(n=0;n<get_N();n++)
			{
				sum += sqr(get(m,n)-_d1.get(m,n));
			}
			return sqrt(sum/(real)M);
		}
	};//struct Data
}