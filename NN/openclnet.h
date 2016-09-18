#pragma once

#include <utils/utils.h>
#include <utils/fileutils.h>
#include <utils/binfile.h>
#include <cmath>

#include "utils.h"

#if !defined(real)
#define real float
#endif

#include <math/utils.h>

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

struct DLNetKernels;
struct DLNet;

struct Environment
{
protected:
	std::vector<cl::Platform> platforms;
	cl::Platform platform;	
	cl::Context context;
	std::vector<cl::Device> devices;
	cl::Device device;
	cl::CommandQueue queue;
	cl::Program program;

public:
	Environment()
	{
	}

	void select_platform(size_t _platform_idx)
	{
		cl::Platform::get(&platforms);

		if(_platform_idx>=platforms.size()) return;

		platform = platforms[_platform_idx];
	}

	void select_device(cl_device_type _device_type,size_t _device_idx)
	{
		cl_context_properties cprops[] = {
			CL_CONTEXT_PLATFORM
			,(cl_context_properties)(platform())
			,0
		};
		context = cl::Context(_device_type, cprops);
		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		if(_device_idx>=devices.size()) return;
		device = devices[_device_idx];

		queue = cl::CommandQueue(context,device,0);
	}

	bool build_program(const char* _code,cl_uint& _status,std::string& _log)
	{
		try
		{
			cl::Program::Sources src(
				1
				,std::make_pair(_code,0)
				);
			program = cl::Program(context,src);
			program.build(devices);
		}
		catch(cl::Error& _err)
		{
			if(_err.err()==CL_BUILD_PROGRAM_FAILURE)
			{
				_status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device());
				_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device());
			}
			return false;
		}
		return true;
	}

	cl::Kernel get_kernel(const char* _name)
	{
		return cl::Kernel(program,_name);
	}

	cl::CommandQueue& get_queue()
	{
		return queue;
	}

	cl::Context& get_context()
	{
		return context;
	}

	friend struct DLNetKernels;
	friend struct DLNet;
};//struct OpenClEnv

struct DLNetKernels
{
protected:
	Environment* m_env;
	cl::Kernel kernel_forward_fc_layer_relu_dropout_train;
	cl::Kernel kernel_forward_fc_layer_relu_dropout_run;
	cl::Kernel kernel_forward_fc_layer_relu;
	cl::Kernel kernel_forward_fc_layer_dropout_train;
	cl::Kernel kernel_forward_fc_layer_dropout_run;
	cl::Kernel kernel_forward_fc_layer_regression;
	cl::Kernel kernel_forward_fc_layer;
	cl::Kernel kernel_set_last_column_to_ones;
	cl::Kernel kernel_set_last_column_to_zero;
	cl::Kernel kernel_transpose;
	cl::Kernel kernel_backward_dw_fc_layer_relu;
	cl::Kernel kernel_backward_dw_fc_layer_relu_dropout;
	cl::Kernel kernel_backward_dw_fc_layer_dropout;
	cl::Kernel kernel_backward_dw_fc_layer;
	cl::Kernel kernel_backward_filter_dw_fc_layer;
	cl::Kernel kernel_backward_filter_dw_fc_layer_relu;
	cl::Kernel kernel_backward_filter_dw_fc_layer_dropout;
	cl::Kernel kernel_backward_filter_dw_fc_layer_relu_dropout;
	cl::Kernel kernel_batch_SGD_trainer_momentum;
	cl::Kernel kernel_batch_SGD_trainer;
	cl::Kernel kernel_copy;
	cl::Kernel kernel_add;
	cl::Kernel kernel_copy_mul;
	cl::Kernel kernel_forward_half_tanh;
	cl::Kernel kernel_backward_dw_half_tanh;
	cl::Kernel kernel_forward_single_relu;
	cl::Kernel kernel_backward_single_relu;

	//typedef std::vector<cl::Event> EventsForStep;
	//typedef std::list<EventsForStep> EventsList;
	//EventsList m_events;
	cl::Event m_event;
	bool async_operation;

	size_t max_computer_units;
	size_t max_workgroup;

public:
	DLNetKernels()
		:m_env(NULL)
		,async_operation(false)
	{
	}

	void create_kernels(Environment* _p_opencl_env)
	{
		m_env = _p_opencl_env;
		kernel_forward_fc_layer_relu_dropout_train = _p_opencl_env->get_kernel("forward_fc_layer_relu_dropout_train");
		kernel_forward_fc_layer_relu_dropout_run = _p_opencl_env->get_kernel("forward_fc_layer_relu_dropout_run");
		kernel_forward_fc_layer_relu = _p_opencl_env->get_kernel("forward_fc_layer_relu");
		kernel_forward_fc_layer_dropout_train = _p_opencl_env->get_kernel("forward_fc_layer_dropout_train");
		kernel_forward_fc_layer_dropout_run = _p_opencl_env->get_kernel("forward_fc_layer_dropout_run");
		kernel_forward_fc_layer_regression = _p_opencl_env->get_kernel("forward_fc_layer_regression");
		kernel_forward_fc_layer = _p_opencl_env->get_kernel("forward_fc_layer");
		kernel_set_last_column_to_ones = _p_opencl_env->get_kernel("set_last_column_to_ones");
		kernel_set_last_column_to_zero = _p_opencl_env->get_kernel("set_last_column_to_zero");
		kernel_transpose = _p_opencl_env->get_kernel("transpose");
		kernel_backward_dw_fc_layer_relu = _p_opencl_env->get_kernel("backward_dw_fc_layer_relu");
		kernel_backward_dw_fc_layer_relu_dropout = _p_opencl_env->get_kernel("backward_dw_fc_layer_relu_dropout");
		kernel_backward_dw_fc_layer_dropout = _p_opencl_env->get_kernel("backward_dw_fc_layer_dropout");
		kernel_backward_dw_fc_layer = _p_opencl_env->get_kernel("backward_dw_fc_layer");
		kernel_backward_filter_dw_fc_layer = _p_opencl_env->get_kernel("backward_filter_dw_fc_layer");
		kernel_backward_filter_dw_fc_layer_relu = _p_opencl_env->get_kernel("backward_filter_dw_fc_layer_relu");
		kernel_backward_filter_dw_fc_layer_dropout = _p_opencl_env->get_kernel("backward_filter_dw_fc_layer_dropout");
		kernel_backward_filter_dw_fc_layer_relu_dropout = _p_opencl_env->get_kernel("backward_filter_dw_fc_layer_relu_dropout");
		kernel_batch_SGD_trainer_momentum = _p_opencl_env->get_kernel("batch_SGD_trainer_momentum");
		kernel_batch_SGD_trainer = _p_opencl_env->get_kernel("batch_SGD_trainer");
		kernel_copy = _p_opencl_env->get_kernel("copy");
		kernel_add = _p_opencl_env->get_kernel("add");
		kernel_copy_mul = _p_opencl_env->get_kernel("copy_mul");
		kernel_forward_half_tanh = _p_opencl_env->get_kernel("forward_half_tanh");
		kernel_backward_dw_half_tanh = _p_opencl_env->get_kernel("backward_dw_half_tanh");
		kernel_forward_single_relu = _p_opencl_env->get_kernel("forward_single_relu");
		kernel_backward_single_relu = _p_opencl_env->get_kernel("backward_single_relu");

		max_computer_units = m_env->device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
		max_workgroup = m_env->device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	}

	void forward_fc_layer_relu_dropout_train(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local
		,cl_uint _result_M
		,cl_uint _result_N
		,cl_uint _input_N
		,cl::Buffer& _input
		,cl::Buffer& _filter
		,cl::Buffer& _result
		,cl_uint _ldi
		,cl_uint _ldf
		,cl_uint _ldr
		,real _drop_probability
		,cl::Buffer& _rand_data
		,cl_uint _ldrd
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_fc_layer_relu_dropout_train.setArg(0,_result_M);
		kernel_forward_fc_layer_relu_dropout_train.setArg(1,_result_N);
		kernel_forward_fc_layer_relu_dropout_train.setArg(2,_input_N);
		kernel_forward_fc_layer_relu_dropout_train.setArg(3,_input);
		kernel_forward_fc_layer_relu_dropout_train.setArg(4,_filter);
		kernel_forward_fc_layer_relu_dropout_train.setArg(5,_result);
		kernel_forward_fc_layer_relu_dropout_train.setArg(6,_ldi);
		kernel_forward_fc_layer_relu_dropout_train.setArg(7,_ldf);
		kernel_forward_fc_layer_relu_dropout_train.setArg(8,_ldr);
		kernel_forward_fc_layer_relu_dropout_train.setArg(9,_drop_probability);
		kernel_forward_fc_layer_relu_dropout_train.setArg(10,_rand_data);
		kernel_forward_fc_layer_relu_dropout_train.setArg(11,_ldrd);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_fc_layer_relu_dropout_train
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void forward_fc_layer_relu_dropout_run(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _result_M
		,cl_uint _result_N
		,cl_uint _input_N
		,cl::Buffer& _input
		,cl::Buffer& _filter
		,cl::Buffer& _result
		,cl_uint _ldi
		,cl_uint _ldf
		,cl_uint _ldr

		,real _drop_probability
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_fc_layer_relu_dropout_run.setArg(0,_result_M);
		kernel_forward_fc_layer_relu_dropout_run.setArg(1,_result_N);
		kernel_forward_fc_layer_relu_dropout_run.setArg(2,_input_N);
		kernel_forward_fc_layer_relu_dropout_run.setArg(3,_input);
		kernel_forward_fc_layer_relu_dropout_run.setArg(4,_filter);
		kernel_forward_fc_layer_relu_dropout_run.setArg(5,_result);
		kernel_forward_fc_layer_relu_dropout_run.setArg(6,_ldi);
		kernel_forward_fc_layer_relu_dropout_run.setArg(7,_ldf);
		kernel_forward_fc_layer_relu_dropout_run.setArg(8,_ldr);
		kernel_forward_fc_layer_relu_dropout_run.setArg(9,_drop_probability);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_fc_layer_relu_dropout_run
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void forward_half_tanh(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl::Buffer& _input
		,cl::Buffer& _result
		,cl_uint _ldi
		,cl_uint _ldr
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_half_tanh.setArg(0,_input);
		kernel_forward_half_tanh.setArg(1,_result);
		kernel_forward_half_tanh.setArg(2,_ldi);
		kernel_forward_half_tanh.setArg(3,_ldr);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_half_tanh
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_dw_half_tanh(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl::Buffer& _result_dw
		,cl::Buffer& _result
		,cl::Buffer& _input_dw
		,cl_uint _ldr_dw
		,cl_uint _ldr
		,cl_uint _ldi_dw
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_dw_half_tanh.setArg(0,_result_dw);
		kernel_backward_dw_half_tanh.setArg(1,_result);
		kernel_backward_dw_half_tanh.setArg(2,_input_dw);
		kernel_backward_dw_half_tanh.setArg(3,_ldr_dw);
		kernel_backward_dw_half_tanh.setArg(4,_ldr);
		kernel_backward_dw_half_tanh.setArg(5,_ldi_dw);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_dw_half_tanh
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void forward_single_relu(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl::Buffer& _input_w
		,cl::Buffer& _result_w
		,cl_uint _ldi
		,cl_uint _ldr
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_single_relu.setArg(0,_input_w);
		kernel_forward_single_relu.setArg(1,_result_w);
		kernel_forward_single_relu.setArg(2,_ldi);
		kernel_forward_single_relu.setArg(3,_ldr);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_single_relu
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_single_relu(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl::Buffer& _result_dw
		,cl::Buffer& _result_w
		,cl::Buffer& _input_dw
		,cl_uint _ldr_dw
		,cl_uint _ldr_w
		,cl_uint _ldi_dw
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_single_relu.setArg(0,_result_dw);
		kernel_backward_single_relu.setArg(1,_result_w);
		kernel_backward_single_relu.setArg(2,_input_dw);
		kernel_backward_single_relu.setArg(3,_ldr_dw);
		kernel_backward_single_relu.setArg(4,_ldr_w);
		kernel_backward_single_relu.setArg(5,_ldi_dw);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_single_relu
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void forward_fc_layer_relu(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _result_M
		,cl_uint _result_N
		,cl_uint _input_N
		,cl::Buffer& _input
		,cl::Buffer& _filter
		,cl::Buffer& _result
		,cl_uint _ldi
		,cl_uint _ldf
		,cl_uint _ldr
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_fc_layer_relu.setArg(0,_result_M);
		kernel_forward_fc_layer_relu.setArg(1,_result_N);
		kernel_forward_fc_layer_relu.setArg(2,_input_N);
		kernel_forward_fc_layer_relu.setArg(3,_input);
		kernel_forward_fc_layer_relu.setArg(4,_filter);
		kernel_forward_fc_layer_relu.setArg(5,_result);
		kernel_forward_fc_layer_relu.setArg(6,_ldi);
		kernel_forward_fc_layer_relu.setArg(7,_ldf);
		kernel_forward_fc_layer_relu.setArg(8,_ldr);
		
		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_fc_layer_relu
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void forward_fc_layer_dropout_train(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _result_M
		,cl_uint _result_N
		,cl_uint _input_N
		,cl::Buffer& _input
		,cl::Buffer& _filter
		,cl::Buffer& _result
		,cl_uint _ldi
		,cl_uint _ldf
		,cl_uint _ldr
		,real _drop_propability
		,cl::Buffer& _rand_data
		,cl_uint _ldrd
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_fc_layer_dropout_train.setArg(0,_result_M);
		kernel_forward_fc_layer_dropout_train.setArg(1,_result_N);
		kernel_forward_fc_layer_dropout_train.setArg(2,_input_N);
		kernel_forward_fc_layer_dropout_train.setArg(3,_input);
		kernel_forward_fc_layer_dropout_train.setArg(4,_filter);
		kernel_forward_fc_layer_dropout_train.setArg(5,_result);
		kernel_forward_fc_layer_dropout_train.setArg(6,_ldi);
		kernel_forward_fc_layer_dropout_train.setArg(7,_ldf);
		kernel_forward_fc_layer_dropout_train.setArg(8,_ldr);
		kernel_forward_fc_layer_dropout_train.setArg(9,_drop_propability);
		kernel_forward_fc_layer_dropout_train.setArg(10,_rand_data);
		kernel_forward_fc_layer_dropout_train.setArg(11,_ldrd);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_fc_layer_dropout_train
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void forward_fc_layer_dropout_run(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _result_M
		,cl_uint _result_N
		,cl_uint _input_N
		,cl::Buffer& _input
		,cl::Buffer& _filter
		,cl::Buffer& _result
		,cl_uint _ldi
		,cl_uint _ldf
		,cl_uint _ldr
		,real _drop_propability
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_fc_layer_dropout_run.setArg(0,_result_M);
		kernel_forward_fc_layer_dropout_run.setArg(1,_result_N);
		kernel_forward_fc_layer_dropout_run.setArg(2,_input_N);
		kernel_forward_fc_layer_dropout_run.setArg(3,_input);
		kernel_forward_fc_layer_dropout_run.setArg(4,_filter);
		kernel_forward_fc_layer_dropout_run.setArg(5,_result);
		kernel_forward_fc_layer_dropout_run.setArg(6,_ldi);
		kernel_forward_fc_layer_dropout_run.setArg(7,_ldf);
		kernel_forward_fc_layer_dropout_run.setArg(8,_ldr);
		kernel_forward_fc_layer_dropout_run.setArg(9,_drop_propability);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_fc_layer_dropout_run
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void forward_fc_layer_regression(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _result_M
		,cl_uint _result_N
		,cl_uint _input_N
		,cl::Buffer& _input
		,cl::Buffer& _filter
		,cl::Buffer& _target
		,cl::Buffer& _target_mask
		,cl::Buffer& _result
		,cl::Buffer& _result_dw
		,cl_uint _ldi
		,cl_uint _ldf
		,cl_uint _ldt
		,cl_uint _ldtm
		,cl_uint _ldr
		,cl_uint _lddw
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_fc_layer_regression.setArg(0,_result_M);
		kernel_forward_fc_layer_regression.setArg(1,_result_N);
		kernel_forward_fc_layer_regression.setArg(2,_input_N);
		kernel_forward_fc_layer_regression.setArg(3,_input);
		kernel_forward_fc_layer_regression.setArg(4,_filter);
		kernel_forward_fc_layer_regression.setArg(5,_target);
		kernel_forward_fc_layer_regression.setArg(6,_target_mask);
		kernel_forward_fc_layer_regression.setArg(7,_result);
		kernel_forward_fc_layer_regression.setArg(8,_result_dw);
		kernel_forward_fc_layer_regression.setArg(9,_ldi);
		kernel_forward_fc_layer_regression.setArg(10,_ldf);
		kernel_forward_fc_layer_regression.setArg(11,_ldt);
		kernel_forward_fc_layer_regression.setArg(12,_ldtm);
		kernel_forward_fc_layer_regression.setArg(13,_ldr);
		kernel_forward_fc_layer_regression.setArg(14,_lddw);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_fc_layer_regression
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void forward_fc_layer(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _result_M
		,cl_uint _result_N
		,cl_uint _input_N
		,cl::Buffer& _input
		,cl::Buffer& _filter
		,cl::Buffer& _result
		,cl_uint _ldi
		,cl_uint _ldf
		,cl_uint _ldr
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_forward_fc_layer.setArg(0,_result_M);
		kernel_forward_fc_layer.setArg(1,_result_N);
		kernel_forward_fc_layer.setArg(2,_input_N);
		kernel_forward_fc_layer.setArg(3,_input);
		kernel_forward_fc_layer.setArg(4,_filter);
		kernel_forward_fc_layer.setArg(5,_result);
		kernel_forward_fc_layer.setArg(6,_ldi);
		kernel_forward_fc_layer.setArg(7,_ldf);
		kernel_forward_fc_layer.setArg(8,_ldr);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_forward_fc_layer
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void set_last_column_to_ones(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local
		,cl_uint _M	
		,cl_uint _N
		,cl::Buffer& _A
		,cl_uint _lda
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_set_last_column_to_ones.setArg(0,_M);
		kernel_set_last_column_to_ones.setArg(1,_N);
		kernel_set_last_column_to_ones.setArg(2,_A);
		kernel_set_last_column_to_ones.setArg(3,_lda);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_set_last_column_to_ones
			,_offset,_global,local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void set_last_column_to_zero(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local
		,cl_uint _M	
		,cl_uint _N
		,cl::Buffer& _A
		,cl_uint _lda
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_set_last_column_to_zero.setArg(0,_M);
		kernel_set_last_column_to_zero.setArg(1,_N);
		kernel_set_last_column_to_zero.setArg(2,_A);
		kernel_set_last_column_to_zero.setArg(3,_lda);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_set_last_column_to_zero
			,_offset,_global,local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void transpose(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local
		,cl::Buffer& _input
		,cl::Buffer& _output
		,cl_uint _ldi
		,cl_uint _ldo
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_transpose.setArg(0,_input);
		kernel_transpose.setArg(1,_output);
		kernel_transpose.setArg(2,_ldi);
		kernel_transpose.setArg(3,_ldo);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_transpose
			,_offset,_global,local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_dw_fc_layer_relu(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _input_M
		,cl_uint _input_N
		,cl_uint _result_N
		,cl::Buffer& _result_dw
		,cl::Buffer& _result
		,cl::Buffer& _filter_t
		,cl::Buffer& _input_dw
		,cl_uint _ldr_dw
		,cl_uint _ldr
		,cl_uint _ldf_t
		,cl_uint _ldi_dw
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_dw_fc_layer_relu.setArg(0,_input_M);
		kernel_backward_dw_fc_layer_relu.setArg(1,_input_N);
		kernel_backward_dw_fc_layer_relu.setArg(2,_result_N);
		kernel_backward_dw_fc_layer_relu.setArg(3,_result_dw);
		kernel_backward_dw_fc_layer_relu.setArg(4,_result);
		kernel_backward_dw_fc_layer_relu.setArg(5,_filter_t);
		kernel_backward_dw_fc_layer_relu.setArg(6,_input_dw);
		kernel_backward_dw_fc_layer_relu.setArg(7,_ldr_dw);
		kernel_backward_dw_fc_layer_relu.setArg(8,_ldr);
		kernel_backward_dw_fc_layer_relu.setArg(9,_ldf_t);
		kernel_backward_dw_fc_layer_relu.setArg(10,_ldi_dw);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_dw_fc_layer_relu
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_dw_fc_layer_relu_dropout(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _input_M
		,cl_uint _input_N
		,cl_uint _result_N
		,cl::Buffer& _result_dw
		,cl::Buffer& _result
		,cl::Buffer& _filter_t
		,cl::Buffer& _input_dw
		,cl_uint _ldr_dw
		,cl_uint _ldr
		,cl_uint _ldf_t
		,cl_uint _ldi_dw
		,real _drop_probability
		,cl::Buffer& _rand_data
		,cl_uint _ldrd
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_dw_fc_layer_relu_dropout.setArg(0,_input_M);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(1,_input_N);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(2,_result_N);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(3,_result_dw);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(4,_result);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(5,_filter_t);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(6,_input_dw);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(7,_ldr_dw);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(8,_ldr);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(9,_ldf_t);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(10,_ldi_dw);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(11,_drop_probability);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(12,_rand_data);
		kernel_backward_dw_fc_layer_relu_dropout.setArg(13,_ldrd);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_dw_fc_layer_relu_dropout
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_dw_fc_layer_dropout(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _input_M
		,cl_uint _input_N
		,cl_uint _result_N
		,cl::Buffer& _result_dw
		,cl::Buffer& _filter_t
		,cl::Buffer& _input_dw
		,cl_uint _ldr_dw
		,cl_uint _ldf_t
		,cl_uint _ldi_dw
		,real _drop_probability
		,cl::Buffer& _rand_data
		,cl_uint _ldrd
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_dw_fc_layer_dropout.setArg(0,_input_M);
		kernel_backward_dw_fc_layer_dropout.setArg(1,_input_N);
		kernel_backward_dw_fc_layer_dropout.setArg(2,_result_N);
		kernel_backward_dw_fc_layer_dropout.setArg(3,_result_dw);
		kernel_backward_dw_fc_layer_dropout.setArg(4,_filter_t);
		kernel_backward_dw_fc_layer_dropout.setArg(5,_input_dw);
		kernel_backward_dw_fc_layer_dropout.setArg(6,_ldr_dw);
		kernel_backward_dw_fc_layer_dropout.setArg(7,_ldf_t);
		kernel_backward_dw_fc_layer_dropout.setArg(8,_ldi_dw);
		kernel_backward_dw_fc_layer_dropout.setArg(9,_drop_probability);
		kernel_backward_dw_fc_layer_dropout.setArg(10,_rand_data);
		kernel_backward_dw_fc_layer_dropout.setArg(11,_ldrd);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_dw_fc_layer_dropout
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_dw_fc_layer(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _input_M
		,cl_uint _input_N
		,cl_uint _result_N
		,cl::Buffer& _result_dw
		,cl::Buffer& _filter_t
		,cl::Buffer& _input_dw
		,cl_uint _ldr_dw
		,cl_uint _ldf_t
		,cl_uint _ldi_dw
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_dw_fc_layer.setArg(0,_input_M);
		kernel_backward_dw_fc_layer.setArg(1,_input_N);
		kernel_backward_dw_fc_layer.setArg(2,_result_N);
		kernel_backward_dw_fc_layer.setArg(3,_result_dw);
		kernel_backward_dw_fc_layer.setArg(4,_filter_t);
		kernel_backward_dw_fc_layer.setArg(5,_input_dw);
		kernel_backward_dw_fc_layer.setArg(6,_ldr_dw);
		kernel_backward_dw_fc_layer.setArg(7,_ldf_t);
		kernel_backward_dw_fc_layer.setArg(8,_ldi_dw);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_dw_fc_layer
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_filter_dw_fc_layer(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _filter_dw_M
		,cl_uint _filter_dw_N
		,cl_uint _input_N
		,cl::Buffer& _input_t
		,cl::Buffer& _result_dw
		,cl::Buffer& _filter_dw
		,cl_uint _ldi_t
		,cl_uint _ldr_dw
		,cl_uint _ldf_dw
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_filter_dw_fc_layer.setArg(0,_filter_dw_M);
		kernel_backward_filter_dw_fc_layer.setArg(1,_filter_dw_N);
		kernel_backward_filter_dw_fc_layer.setArg(2,_input_N);
		kernel_backward_filter_dw_fc_layer.setArg(3,_input_t);
		kernel_backward_filter_dw_fc_layer.setArg(4,_result_dw);
		kernel_backward_filter_dw_fc_layer.setArg(5,_filter_dw);
		kernel_backward_filter_dw_fc_layer.setArg(6,_ldi_t);
		kernel_backward_filter_dw_fc_layer.setArg(7,_ldr_dw);
		kernel_backward_filter_dw_fc_layer.setArg(8,_ldf_dw);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_filter_dw_fc_layer
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_filter_dw_fc_layer_relu(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _filter_dw_M
		,cl_uint _filter_dw_N
		,cl_uint _input_N
		,cl::Buffer& _input_t
		,cl::Buffer& _result_dw
		,cl::Buffer& _filter_dw
		,cl_uint _ldi_t
		,cl_uint _ldr_dw
		,cl_uint _ldf_dw
		,cl::Buffer& _result
		,cl_uint _ldr
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_filter_dw_fc_layer_relu.setArg(0,_filter_dw_M);
		kernel_backward_filter_dw_fc_layer_relu.setArg(1,_filter_dw_N);
		kernel_backward_filter_dw_fc_layer_relu.setArg(2,_input_N);
		kernel_backward_filter_dw_fc_layer_relu.setArg(3,_input_t);
		kernel_backward_filter_dw_fc_layer_relu.setArg(4,_result_dw);
		kernel_backward_filter_dw_fc_layer_relu.setArg(5,_filter_dw);
		kernel_backward_filter_dw_fc_layer_relu.setArg(6,_ldi_t);
		kernel_backward_filter_dw_fc_layer_relu.setArg(7,_ldr_dw);
		kernel_backward_filter_dw_fc_layer_relu.setArg(8,_ldf_dw);
		kernel_backward_filter_dw_fc_layer_relu.setArg(9,_result);
		kernel_backward_filter_dw_fc_layer_relu.setArg(10,_ldr);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_filter_dw_fc_layer_relu
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_filter_dw_fc_layer_dropout(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _filter_dw_M
		,cl_uint _filter_dw_N
		,cl_uint _input_N
		,cl::Buffer& _input_t
		,cl::Buffer& _result_dw
		,cl::Buffer& _filter_dw
		,cl_uint _ldi_t
		,cl_uint _ldr_dw
		,cl_uint _ldf_dw
		,real _drop_probability
		,cl::Buffer& _rand_data
		,cl_uint _ldrd
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_filter_dw_fc_layer_dropout.setArg(0,_filter_dw_M);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(1,_filter_dw_N);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(2,_input_N);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(3,_input_t);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(4,_result_dw);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(5,_filter_dw);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(6,_ldi_t);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(7,_ldr_dw);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(8,_ldf_dw);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(9,_drop_probability);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(10,_rand_data);
		kernel_backward_filter_dw_fc_layer_dropout.setArg(11,_ldrd);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_filter_dw_fc_layer_dropout
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void backward_filter_dw_fc_layer_relu_dropout(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl_uint _filter_dw_M
		,cl_uint _filter_dw_N
		,cl_uint _input_N
		,cl::Buffer& _input_t
		,cl::Buffer& _result_dw
		,cl::Buffer& _filter_dw
		,cl_uint _ldi_t
		,cl_uint _ldr_dw
		,cl_uint _ldf_dw
		,cl::Buffer& _result
		,cl_uint _ldr
		,real _drop_probability
		,cl::Buffer& _rand_data
		,cl_uint _ldrd
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(0,_filter_dw_M);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(1,_filter_dw_N);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(2,_input_N);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(3,_input_t);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(4,_result_dw);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(5,_filter_dw);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(6,_ldi_t);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(7,_ldr_dw);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(8,_ldf_dw);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(9,_result);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(10,_ldr);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(11,_drop_probability);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(12,_rand_data);
		kernel_backward_filter_dw_fc_layer_relu_dropout.setArg(13,_ldrd);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_backward_filter_dw_fc_layer_relu_dropout
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void batch_SGD_trainer_momentum(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl::Buffer& _filter
		,cl::Buffer& _filter_dw
		,cl::Buffer& _last_pg
		,real _learning_rate
		,real _momentum
		,real _l1_decay
		,real _l2_decay
		,cl_uint _batch_size
		,cl_uint _ldf
		,cl_uint _ldf_dw
		,cl_uint _ldlast_pg
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_batch_SGD_trainer_momentum.setArg(0,_filter);
		kernel_batch_SGD_trainer_momentum.setArg(1,_filter_dw);
		kernel_batch_SGD_trainer_momentum.setArg(2,_last_pg);
		kernel_batch_SGD_trainer_momentum.setArg(3,_learning_rate);
		kernel_batch_SGD_trainer_momentum.setArg(4,_momentum);
		kernel_batch_SGD_trainer_momentum.setArg(5,_l1_decay);
		kernel_batch_SGD_trainer_momentum.setArg(6,_l2_decay);
		kernel_batch_SGD_trainer_momentum.setArg(7,_batch_size);
		kernel_batch_SGD_trainer_momentum.setArg(8,_ldf);
		kernel_batch_SGD_trainer_momentum.setArg(9,_ldf_dw);
		kernel_batch_SGD_trainer_momentum.setArg(10,_ldlast_pg);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_batch_SGD_trainer_momentum
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void batch_SGD_trainer(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local

		,cl::Buffer& _filter
		,cl::Buffer& _filter_dw
		,real _learning_rate
		,real _l1_decay
		,real _l2_decay
		,cl_uint _batch_size
		,cl_uint _ldf
		,cl_uint _ldf_dw
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_batch_SGD_trainer.setArg(0,_filter);
		kernel_batch_SGD_trainer.setArg(1,_filter_dw);
		kernel_batch_SGD_trainer.setArg(2,_learning_rate);
		kernel_batch_SGD_trainer.setArg(3,_l1_decay);
		kernel_batch_SGD_trainer.setArg(4,_l2_decay);
		kernel_batch_SGD_trainer.setArg(5,_batch_size);
		kernel_batch_SGD_trainer.setArg(6,_ldf);
		kernel_batch_SGD_trainer.setArg(7,_ldf_dw);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_batch_SGD_trainer
			,_offset,_global,cl::NullRange//local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void copy(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local
		,cl::Buffer& _src
		,cl::Buffer& _dest
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_copy.setArg(0,_src);
		kernel_copy.setArg(1,_dest);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_copy
			,_offset,_global,local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void copy_mul(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local
		,cl::Buffer& _src
		,cl::Buffer& _dest
		,float _k
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_copy_mul.setArg(0,_src);
		kernel_copy_mul.setArg(1,_dest);
		kernel_copy_mul.setArg(2,_k);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_copy_mul
			,_offset,_global,local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void add(
		const cl::NDRange& _offset
		,const cl::NDRange& _global
		,const cl::NDRange& _local
		,cl::Buffer& _A
		,cl::Buffer& _B
		)
	{
		VERIFY_EXIT(NOT_NULL(m_env));
		kernel_add.setArg(0,_A);
		kernel_add.setArg(1,_B);

		cl::NDRange local = min_range(_global,_local);
		create_step();
		m_env->queue.enqueueNDRangeKernel(
			kernel_add
			,_offset,_global,local
			,get_previos_events(),current_event()
			);
		flush();
	}

	void flush()
	{
		m_env->queue.flush();
	}

	size_t get_computer_units() const 
	{
		return max_computer_units;
	}

	size_t get_max_workgroup() const
	{
		return max_workgroup;
	}

	void wait()
	{
		//cl::Event* pevnt = current_event();
		//if(IS_NULL(pevnt)) return;
		//pevnt->wait();
		//m_events.clear();
		if(!async_operation) return;
		m_event.wait();
		async_operation = false;
	}

	Environment* get_env()
	{
		return m_env;
	}

protected:
	std::vector<cl::Event>* get_previos_events()
	{
		return NULL;
		//if(m_events.size()<2) return NULL;
		//EventsList::iterator it = m_events.end();
		//--it;
		//--it;
		//return &*it;
	}

	cl::Event* current_event()
	{
		async_operation = true;
		return &m_event;
		//if(m_events.size()<1) return NULL;
		//EventsList::iterator it = m_events.end();
		//--it;
		//return &(it->front());
	}

	void create_step()
	{
		wait();
		//m_events.push_back(std::vector<cl::Event>());
		//m_events.back().resize(1);
	}
};

struct LayerOptions
{
	bool relu_mode;
	bool dropout_mode;
	real drop_probability;
	bool single_half_tanh_mode;
	bool single_relu_mode;
	bool regression_mode;
	size_t out_depth;
	real l1_decay;
	real l2_decay;
	real bias;
	//real weights_scale_coef;

	bool has_filter() const 
	{
		return !single_half_tanh_mode
			&& !single_relu_mode
			;
	}

	LayerOptions()
		:relu_mode(false)
		,dropout_mode(false)
		,regression_mode(false)
		,single_half_tanh_mode(false)
		,single_relu_mode(false)
		,drop_probability((real)0.5)
		,out_depth(0)
		,l1_decay(0)
		,l2_decay(0)
		,bias((real)0.1)
		//,weights_scale_coef((real)1.0)
	{
	}
};

struct NetOptions
{
	real learning_rate;
	real momentum;
	real l1_decay;
	real l2_decay;
	real weights_scale_coef;
	bool run_only_net_mode;

	NetOptions()
		:learning_rate((real)0.1)
		,momentum((real)0.9)
		,l1_decay(0)
		,l2_decay(0)
		,run_only_net_mode(false)
		,weights_scale_coef((real)1.0)
	{
	}
};

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

	bool is_bad_values() const 
	{
		size_t m = 0,n = 0;
		for(m=0;m<M;m++)
		for(n=0;n<N;n++)
		{
			if(fabs(get(m,n))>1.0) return true;
		}
		return false;
	}

	bool debug_test_isnan(bool _test_values = true) const 
	{
		if(is_any_nan())
		{
			std::tcout << _T("\nsome data is nan\n");
#if !defined(_DEBUG)
			static int exception_cnt = 0;
			if(!exception_cnt++) throw NaNException();
#endif
			return true;
		}
//		if(_test_values && is_bad_values())
//		{
//			std::tcout << _T("bad values\n");
//			return true;
//		}
		return false;
	}

	void rand()
	{
		size_t m = 0,n = 0;
		for(m=0;m<M;m++)
		for(n=0;n<N;n++)
		{
			set(m,n,clamp<real>(::rand((real)1.0),(real)0.0,(real)1.0));
		}
		modify_on_cpu();
	}

	void rand(real _min,real _max)
	{
		size_t m = 0,n = 0;
		for(m=0;m<M;m++)
		for(n=0;n<N;n++)
		{
			set(m,n,clamp<real>(::rand(_min,_max),_min,_max));
		}
		modify_on_cpu();
	}

	void set_uniform_rand(real _max)
	{
		size_t m = 0,n = 0;
		for(m=0;m<M;m++)
		for(n=0;n<N-1;n++)
		{
			set(m,n,clamp<real>(::rand(-_max,_max),-_max,_max));
		}
		modify_on_cpu();
	}

	void fill(real _val)
	{
		size_t m = 0,n = 0;
		for(m=0;m<M;m++)
		for(n=0;n<N;n++)
		{
			set(m,n,_val);
		}
		modify_on_cpu();
	}

	void copy_of(const Data& _d1)
	{
		VERIFY_EXIT(get_M()==_d1.get_M() && get_N()==_d1.get_N());
		size_t m=0,n=0;
		for(m=0;m<get_M();m++)
		for(n=0;n<get_N();n++)
		{
			set(m,n,_d1.get(m,n));
		}
		modify_on_cpu();
	}

	void copy_from(const Data& _d1,size_t _width)
	{
		VERIFY_EXIT(get_M()==_d1.get_M() && _width<=get_N() && _width<=_d1.get_N());
		size_t m=0,n=0;
		for(m=0;m<get_M();m++)
		for(n=0;n<_width;n++)
		{
			set(m,n,_d1.get(m,n));
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

	void set(size_t _m,size_t _n,real _val)
	{
		VERIFY_EXIT(_m<M && _n<N);
		host_data[_m*N+_n] = _val;
		modify_on_cpu();
	}

	void add(size_t _m,size_t _n,real _val)
	{
		VERIFY_EXIT(_m<M && _n<N);
		host_data[_m*N+_n] += _val;
		modify_on_cpu();
	}

	real get(size_t _m, size_t _n) const 
	{
		VERIFY_EXIT1(_m<M && _n<N,(real)0);
		return host_data[_m*N+_n];
	}

	void zero()
	{
		std::fill(host_data.begin(),host_data.end(),(real)0);
		modify_on_cpu();
	}

	void set_row(size_t _row_idx,const std::vector<real>& _vec)
	{
		VERIFY_EXIT(_vec.size()<=N-1);
		VERIFY_EXIT(_row_idx<M);
		size_t m=0,n=0;
		m = _row_idx;
		for(n=0;n<_vec.size();n++)
		{
			host_data[m*N+n] = _vec[n];
		}
//		for(;n<N;n++)
//		{
//			host_data[m*N+n] = 0;
//		}
		modify_on_cpu();
	}

	void get_row(size_t _row_idx,std::vector<real>& _vec) const 
	{
		VERIFY_EXIT(_vec.size()<=N-1);
		VERIFY_EXIT(_row_idx<M);
		size_t m=0,n=0;
		m = _row_idx;
		for(n=0;n<_vec.size();n++)
		{
			_vec[n] = host_data[m*N+n];
		}
	}

	void set_row(size_t _row_idx,const std::vector<real>& _vec,size_t _offs,size_t _sz)
	{
		VERIFY_EXIT(_offs+_sz<=_vec.size());
		VERIFY_EXIT(_sz<=N-1);
		VERIFY_EXIT(_row_idx<M);
		size_t m=0,n=0;
		m = _row_idx;
		for(n=0;n<_sz;n++)
		{
			host_data[m*N+n] = _vec[n+_offs];
		}
//		for(;n<N;n++)
//		{
//			host_data[m*N+n] = 0;
//		}
		modify_on_cpu();
	}
	void get_row(size_t _row_idx,std::vector<real>& _vec,size_t _offs,size_t _sz)
	{
		VERIFY_EXIT(_offs+_sz<=_vec.size());
		VERIFY_EXIT(_sz<=N-1);
		VERIFY_EXIT(_row_idx<M);
		size_t m=0,n=0;
		m = _row_idx;
		for(n=0;n<_sz;n++)
		{
			_vec[n+_offs] = host_data[m*N+n];
		}
	}

	real get_loss() const 
	{
		real v = 0;
		size_t m = 0,n = 0;
		for(m=0;m<M;m++)
		for(n=0;n<N-1;n++)
		{
			v += (real)2.0*sqr(host_data[m*N+n]);
		}
		return v/(real)M;
	}

	real diff(const Data& _d1) const 
	{
		VERIFY_EXIT1(get_M()==_d1.get_M() && get_N()==_d1.get_N(),(real)100.0);
		size_t m=0,n=0;
		real sum = 0;
		for(m=0;m<get_M();m++)
		for(n=0;n<get_N();n++)
		{
			sum += fabs(get(m,n)-_d1.get(m,n));
		}
		return sum;
	}
};//struct Data

struct DLNet;

struct Layer
{
public:
	struct Options: public LayerOptions
	{
		size_t input_size;
		size_t input_count;
		Data* src;
		Data* src_dw;
		bool run_only_net_mode;


		Options()
			:input_size(0)
			,input_count(0)
			,src(NULL)
			,src_dw(NULL)
			,run_only_net_mode(false)
		{
		}
	};
public:
	Layer::Options options;
	Data inp_t;
	Data filter;
	Data filter_t;
	Data filter_dw;
	Data output;
	Data output_dw;
	Data last_pg;
	Data rand_data;

public:
	Layer(const Layer::Options& _options,const cl::Context& _context,const cl::CommandQueue& _queue,real _scale_coef)
	{
		init_layer(_options,_context,_queue,_scale_coef);
	}

	void init_layer(const Layer::Options& _options,const cl::Context& _context,const cl::CommandQueue& _queue,real _scale_coef)
	{
		options = _options;
		create(_context,_queue,_scale_coef);
	}

	void forward(DLNetKernels* _kernels,bool _test_mode = false,bool _train_mode = false,Data* _target = NULL,Data* _target_mask = NULL)
	{
		VERIFY_EXIT(NOT_NULL(_kernels));
		VERIFY_EXIT(NOT_NULL(options.src));
		if(_train_mode || _test_mode)
		{
			VERIFY_EXIT(!options.run_only_net_mode);
		}

		Data& src = *options.src;
		Data& res = output;
		Data& res_dw = output_dw;
		if(options.has_filter())
		{
			VERIFY_EXIT(src.get_N()==filter.get_M() && res.get_M()==src.get_M() && res.get_N()==filter.get_N());
		}
		VERIFY_EXIT(src.get_M()%8==0 && res.get_N()%4==0 && src.get_N()%4==0);

#if defined(DEBUG_OPENCL_NET)
		if(src.is_modified_on_cpu()) src.debug_test_isnan();
#endif

		src.update_from_cpu(_kernels);

		_kernels->set_last_column_to_ones(
			cl::NullRange,cl::NDRange(src.get_M()),cl::NDRange(_kernels->get_max_workgroup())
			,src.get_M(),src.get_N()
			,src.get_buffer()
			,src.get_N()
			);
		src.modify_on_gpu();

#if defined(DEBUG_OPENCL_NET)
		src.update_from_gpu(_kernels);
#endif

		if(options.dropout_mode && _train_mode)
		{
			rand_data.rand();
			rand_data.update_from_cpu(_kernels);
		}

		if(options.single_half_tanh_mode)
		{
			_kernels->forward_half_tanh(
				cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
				,src.get_buffer(),res.get_buffer()
				,src.get_N(),res.get_N()
				);
			res.modify_on_gpu();
		}
		else if(options.single_relu_mode)
		{
			_kernels->forward_single_relu(
				cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
				,src.get_buffer(),res.get_buffer()
				,src.get_N(),res.get_N()
				);
			res.modify_on_gpu();
		}
		else if(options.relu_mode && options.dropout_mode)
		{
			if(_train_mode)
			{
				_kernels->forward_fc_layer_relu_dropout_train(
					cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
					,src.get_M(),res.get_N(),src.get_N()
					,src.get_buffer(),filter.get_buffer(),res.get_buffer()
					,src.get_N(),filter.get_N(),res.get_N()
					,options.drop_probability,rand_data.get_buffer(),rand_data.get_N()
					);
				res.modify_on_gpu();
			}
			else
			{
				_kernels->forward_fc_layer_relu_dropout_run(
					cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
					,src.get_M(),res.get_N(),src.get_N()
					,src.get_buffer(),filter.get_buffer(),res.get_buffer()
					,src.get_N(),filter.get_N(),res.get_N()
					,options.drop_probability
					);
				res.modify_on_gpu();
			}
		}
		else if(options.relu_mode && !options.dropout_mode)
		{
			_kernels->forward_fc_layer_relu(
				cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
				,src.get_M(),res.get_N(),src.get_N()
				,src.get_buffer(),filter.get_buffer(),res.get_buffer()
				,src.get_N(),filter.get_N(),res.get_N()
				);
			res.modify_on_gpu();
		}
		else if(!options.relu_mode && options.dropout_mode)
		{
			if(_train_mode)
			{
				_kernels->forward_fc_layer_dropout_train(
					cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
					,src.get_M(),res.get_N(),src.get_N()
					,src.get_buffer(),filter.get_buffer(),res.get_buffer()
					,src.get_N(),filter.get_N(),res.get_N()
					,options.drop_probability,rand_data.get_buffer(),rand_data.get_N()
					);
				res.modify_on_gpu();
			}else
			{
				_kernels->forward_fc_layer_dropout_run(
					cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
					,src.get_M(),res.get_N(),src.get_N()
					,src.get_buffer(),filter.get_buffer(),res.get_buffer()
					,src.get_N(),filter.get_N(),res.get_N()
					,options.drop_probability
					);
				res.modify_on_gpu();
			}
		}
		else if(!options.relu_mode && !options.dropout_mode)
		{
			if((_test_mode || _train_mode) && options.regression_mode)
			{

#if defined(DEBUG_OPENCL_NET)
				if(_target->is_modified_on_cpu()) _target->debug_test_isnan();
				if(_target_mask->is_modified_on_cpu()) _target_mask->debug_test_isnan();
#endif

				_target->update_from_cpu(_kernels);
				_target_mask->update_from_cpu(_kernels);

				VERIFY_EXIT(NOT_NULL(_target));
				VERIFY_EXIT(NOT_NULL(_target_mask));
				_kernels->forward_fc_layer_regression(
					cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
					,res.get_M(),res.get_N(),src.get_N()
					,src.get_buffer(),filter.get_buffer(),_target->get_buffer(),_target_mask->get_buffer(),res.get_buffer(),res_dw.get_buffer()
					,src.get_N(),filter.get_N(),_target->get_N(),_target_mask->get_N(),res.get_N(),res_dw.get_N()
					);
				res.modify_on_gpu();
				res_dw.modify_on_gpu();

				//VERIFY(res_dw.is_last_column_zero());
			}
			else
			{
				_kernels->forward_fc_layer(
					cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
					,res.get_M(),res.get_N(),src.get_N()
					,src.get_buffer(),filter.get_buffer(),res.get_buffer()
					,src.get_N(),filter.get_N(),res.get_N()
					);
				res.modify_on_gpu();
			}
		}

#if defined(DEBUG_OPENCL_NET)	
		res.update_from_gpu(_kernels);
		res.debug_test_isnan();
		res_dw.update_from_gpu(_kernels);
		res_dw.debug_test_isnan();
#endif
	}

	void backward(DLNetKernels* _kernels)
	{
		VERIFY_EXIT(!options.run_only_net_mode);
		VERIFY_EXIT(NOT_NULL(options.src));
		VERIFY_EXIT(NOT_NULL(options.src_dw));
		Data& inp = *options.src;
		Data& inp_dw = *options.src_dw;
		Data& res = output;
		Data& res_dw = output_dw;

#if defined(DEBUG_OPENCL_NET)	
		if(inp.is_modified_on_cpu()) inp.debug_test_isnan();
		if(res.is_modified_on_cpu()) res.debug_test_isnan();
		if(res_dw.is_modified_on_cpu()) res_dw.debug_test_isnan();
#endif

		inp.update_from_cpu(_kernels);
		res.update_from_cpu(_kernels);
		res_dw.update_from_cpu(_kernels);

		if(!options.has_filter())
		{
			if(options.single_half_tanh_mode)
			{
				_kernels->backward_dw_half_tanh(
					cl::NullRange,cl::NDRange(inp_dw.get_N()/8,inp_dw.get_M()/4),cl::NDRange(8,8)
					,res_dw.get_buffer(),res.get_buffer(),inp_dw.get_buffer()
					,res_dw.get_N(),res.get_N(),inp_dw.get_N()
					);
				inp_dw.modify_on_gpu();
			}
			else if(options.single_relu_mode)
			{
				_kernels->backward_single_relu(
					cl::NullRange,cl::NDRange(inp_dw.get_N()/8,inp_dw.get_M()/4),cl::NDRange(8,8)
					,res_dw.get_buffer(),res.get_buffer(),inp_dw.get_buffer()
					,res_dw.get_N(),res.get_N(),inp_dw.get_N()
					);
				inp_dw.modify_on_gpu();
			}

#if defined(DEBUG_OPENCL_NET)
			inp_dw.update_from_gpu(_kernels);
			inp_dw.debug_test_isnan();
#endif

			return;
		}

		_kernels->transpose(
			cl::NullRange,cl::NDRange(filter.get_N(),filter.get_M()),cl::NDRange(8,8)
			,filter.get_buffer(),filter_t.get_buffer()
			,filter.get_N(),filter_t.get_N()
			);
		filter_t.modify_on_gpu();

		_kernels->set_last_column_to_zero(
			cl::NullRange,cl::NDRange(filter_t.get_M()),cl::NDRange(_kernels->get_max_workgroup())
			,filter_t.get_M(),filter_t.get_N()
			,filter_t.get_buffer()
			,filter_t.get_N()
			);
		filter_t.modify_on_gpu();

		if(options.relu_mode && !options.dropout_mode)
		{
			_kernels->backward_dw_fc_layer_relu(
				cl::NullRange,cl::NDRange(inp_dw.get_N()/8,inp_dw.get_M()/4),cl::NDRange(8,8)
				,inp_dw.get_M(),inp_dw.get_N(),res_dw.get_N()
				,res_dw.get_buffer(),res.get_buffer(),filter_t.get_buffer(),inp_dw.get_buffer()
				,res_dw.get_N(),res.get_N(),filter_t.get_N(),inp_dw.get_N()
				);
			inp_dw.modify_on_gpu();
		}
		else if(options.relu_mode && options.dropout_mode)
		{
			_kernels->backward_dw_fc_layer_relu_dropout(
				cl::NullRange,cl::NDRange(inp_dw.get_N()/8,inp_dw.get_M()/4),cl::NDRange(8,8)
				,inp_dw.get_M(),inp_dw.get_N(),res_dw.get_N()
				,res_dw.get_buffer(),res.get_buffer(),filter_t.get_buffer(),inp_dw.get_buffer()
				,res_dw.get_N(),res.get_N(),filter_t.get_N(),inp_dw.get_N()
				,options.drop_probability,rand_data.get_buffer(),rand_data.get_N()
				);
			inp_dw.modify_on_gpu();
		}
		else if(!options.relu_mode && options.dropout_mode)
		{
			_kernels->backward_dw_fc_layer_dropout(
				cl::NullRange,cl::NDRange(inp_dw.get_N()/8,inp_dw.get_M()/4),cl::NDRange(8,8)
				,inp_dw.get_M(),inp_dw.get_N(),res_dw.get_N()
				,res_dw.get_buffer(),filter_t.get_buffer(),inp_dw.get_buffer()
				,res_dw.get_N(),filter_t.get_N(),inp_dw.get_N()
				,options.drop_probability,rand_data.get_buffer(),rand_data.get_N()
				);
			inp_dw.modify_on_gpu();
		}
		else if(!options.relu_mode && !options.dropout_mode)
		{
			_kernels->backward_dw_fc_layer(
				cl::NullRange,cl::NDRange(inp_dw.get_N()/8,inp_dw.get_M()/4),cl::NDRange(8,8)
				,inp_dw.get_M(),inp_dw.get_N(),res_dw.get_N()
				,res_dw.get_buffer(),filter_t.get_buffer(),inp_dw.get_buffer()
				,res_dw.get_N(),filter_t.get_N(),inp_dw.get_N()
				);
			inp_dw.modify_on_gpu();
		}

		_kernels->set_last_column_to_zero(
			cl::NullRange,cl::NDRange(inp_dw.get_M()),cl::NDRange(_kernels->get_max_workgroup())
			,inp_dw.get_M(),inp_dw.get_N()
			,inp_dw.get_buffer()
			,inp_dw.get_N()
			);
		inp_dw.modify_on_gpu();

		_kernels->set_last_column_to_ones(
			cl::NullRange,cl::NDRange(inp.get_M()),cl::NDRange(_kernels->get_max_workgroup())
			,inp.get_M(),inp.get_N()
			,inp.get_buffer()
			,inp.get_N()
			);
		inp.modify_on_gpu();

		_kernels->set_last_column_to_zero(
			cl::NullRange,cl::NDRange(res_dw.get_M()),cl::NDRange(_kernels->get_max_workgroup())
			,res_dw.get_M(),res_dw.get_N()
			,res_dw.get_buffer()
			,res_dw.get_N()
			);
		res_dw.modify_on_gpu();

		_kernels->transpose(
			cl::NullRange,cl::NDRange(inp.get_N(),inp.get_M()),cl::NDRange(8,8)
			,inp.get_buffer(),inp_t.get_buffer()
			,inp.get_N(),inp_t.get_N()
			);
		inp_t.modify_on_gpu();

		if(!options.relu_mode && !options.dropout_mode)
		{
			_kernels->backward_filter_dw_fc_layer(
				cl::NullRange,cl::NDRange(filter_dw.get_N()/8,filter_dw.get_M()/4),cl::NDRange(8,8)
				,filter_dw.get_M(),filter_dw.get_N(),inp_t.get_N()
				,inp_t.get_buffer(),res_dw.get_buffer(),filter_dw.get_buffer()
				,inp_t.get_N(),res_dw.get_N(),filter_dw.get_N()
				);
			filter_dw.modify_on_gpu();
		}
		else if(options.relu_mode && !options.dropout_mode)
		{
			_kernels->backward_filter_dw_fc_layer_relu(
				cl::NullRange,cl::NDRange(filter_dw.get_N()/8,filter_dw.get_M()/4),cl::NDRange(8,8)
				,filter_dw.get_M(),filter_dw.get_N(),inp_t.get_N()
				,inp_t.get_buffer(),res_dw.get_buffer(),filter_dw.get_buffer()
				,inp_t.get_N(),res_dw.get_N(),filter_dw.get_N()
				,res.get_buffer()
				,res.get_N()
				);
			filter_dw.modify_on_gpu();
		}
		else if(!options.relu_mode && options.dropout_mode)
		{
			_kernels->backward_filter_dw_fc_layer_dropout(
				cl::NullRange,cl::NDRange(filter_dw.get_N()/8,filter_dw.get_M()/4),cl::NDRange(8,8)
				,filter_dw.get_M(),filter_dw.get_N(),inp_t.get_N()
				,inp_t.get_buffer(),res_dw.get_buffer(),filter_dw.get_buffer()
				,inp_t.get_N(),res_dw.get_N(),filter_dw.get_N()
				,options.drop_probability
				,rand_data.get_buffer()
				,rand_data.get_N()
				);
			filter_dw.modify_on_gpu();
		}
		else if(options.relu_mode && options.dropout_mode)
		{
			_kernels->backward_filter_dw_fc_layer_relu_dropout(
				cl::NullRange,cl::NDRange(filter_dw.get_N()/8,filter_dw.get_M()/4),cl::NDRange(8,8)
				,filter_dw.get_M(),filter_dw.get_N(),inp_t.get_N()
				,inp_t.get_buffer(),res_dw.get_buffer(),filter_dw.get_buffer()
				,inp_t.get_N(),res_dw.get_N(),filter_dw.get_N()
				,res.get_buffer()
				,res.get_N()
				,options.drop_probability
				,rand_data.get_buffer()
				,rand_data.get_N()
				);
			filter_dw.modify_on_gpu();
		}

		_kernels->set_last_column_to_zero(
			cl::NullRange,cl::NDRange(filter_dw.get_M()),cl::NDRange(_kernels->get_max_workgroup())
			,filter_dw.get_M(),filter_dw.get_N()
			,filter_dw.get_buffer()
			,filter_dw.get_N()
			);
		filter_dw.modify_on_gpu();

#if defined(DEBUG_OPENCL_NET)
		inp_dw.update_from_gpu(_kernels);
		inp_dw.debug_test_isnan();
		filter_dw.update_from_gpu(_kernels);
		filter_dw.debug_test_isnan(false);
#endif
	}

	void batch_process(DLNetKernels* _kernels,const NetOptions& _net_options)
	{
		if(!options.has_filter()) return;
		VERIFY_EXIT(!options.run_only_net_mode);

		real l1_decay_l = options.l1_decay*_net_options.l1_decay;
		real l2_decay_l = options.l2_decay*_net_options.l2_decay;

#if defined(DEBUG_OPENCL_NET)
		if(filter_dw.is_modified_on_cpu()) filter_dw.debug_test_isnan();
#endif

		filter_dw.update_from_cpu(_kernels);

		if(_net_options.momentum>0)
		{

#if defined(DEBUG_OPENCL_NET)
			if(last_pg.is_modified_on_cpu()) last_pg.debug_test_isnan();
#endif

			last_pg.update_from_cpu(_kernels);

			_kernels->batch_SGD_trainer_momentum(
				cl::NullRange,cl::NDRange(filter.get_N(),filter.get_M()),cl::NDRange(8,8)
				,filter.get_buffer(),filter_dw.get_buffer(),last_pg.get_buffer()
				,_net_options.learning_rate,_net_options.momentum,l1_decay_l,l2_decay_l,options.input_count
				,filter.get_N(),filter_dw.get_N(),last_pg.get_N()
				);
			filter.modify_on_gpu();
			last_pg.modify_on_gpu();
		}
		else
		{
			_kernels->batch_SGD_trainer(
				cl::NullRange,cl::NDRange(filter.get_N(),filter.get_M()),cl::NDRange(8,8)
				,filter.get_buffer(),filter_dw.get_buffer()
				,_net_options.learning_rate,l1_decay_l,l2_decay_l,options.input_count
				,filter.get_N(),filter_dw.get_N()
				);
			filter.modify_on_gpu();
		}
#if defined(DEBUG_OPENCL_NET)
		last_pg.update_from_gpu(_kernels);
		last_pg.debug_test_isnan();
		filter.update_from_gpu(_kernels);
		filter.debug_test_isnan();
#endif
	}

	real get_loss() const 
	{
		return output_dw.get_loss();
	}

	void load(BinFile<real>& _file,const cl::CommandQueue& _queue)
	{
		if(!options.has_filter()) return;
		std::vector<std::vector<real> > filters;
		std::vector<real> bias;
		filters.resize(options.out_depth);
		size_t i = 0;
		for(i=0;i<options.out_depth;i++)
		{
			filters[i].resize(options.input_size);
			_file.get(filters[i].begin(),filters[i].end());
		}
		bias.resize(options.out_depth);
		_file.get(bias.begin(),bias.end());

		VERIFY(filter.get_M() == options.input_size + 1);
		VERIFY(filter.get_N() == options.out_depth + 1);

		size_t n = 0,m = 0;
		for(m=0;m<filter.get_M()-1;m++)
		{
			for(n=0;n<filter.get_N()-1;n++)
			{
				filter.set(m,n,filters[n][m]);
			}
			filter.set(m,n,0);
		}
		VERIFY(m==filter.get_M()-1);
		for(n=0;n<filter.get_N()-1;n++)
			filter.set(m,n,bias[n]);
		filter.set(m,n,0);
		if(!options.run_only_net_mode)
			filter_dw.zero();
#if defined(DEBUG_OPENCL_NET)
		filter.debug_test_isnan();
		if(!options.run_only_net_mode)
			filter_dw.debug_test_isnan();
#endif
		filter.copy_data_to_gpu(_queue);
		if(!options.run_only_net_mode)
			filter_dw.copy_data_to_gpu(_queue);
	}

	void save(BinFile<real>& _file,const cl::CommandQueue& _queue) 
	{
		if(!options.has_filter()) return;
		std::vector<std::vector<real> > filters;
		std::vector<real> bias;
		filters.resize(options.out_depth);
		size_t i = 0;
		for(i=0;i<options.out_depth;i++)
		{
			filters[i].resize(options.input_size);
		}
		bias.resize(options.out_depth);

		if(filter.is_modified_on_gpu()) filter.copy_data_to_host(_queue);

		VERIFY(filter.get_M() == options.input_size + 1);
		VERIFY(filter.get_N() == options.out_depth + 1);

		size_t n = 0,m = 0;
		for(m=0;m<filter.get_M()-1;m++)
		{
			for(n=0;n<filter.get_N()-1;n++)
			{
				filters[n][m] = filter.get(m,n);
			}
		}
		VERIFY(m==filter.get_M()-1);
		for(n=0;n<filter.get_N()-1;n++)
			bias[n] = filter.get(m,n);

		for(i=0;i<options.out_depth;i++)
		{
			_file.put(filters[i].begin(),filters[i].end());
		}
		_file.put(bias.begin(),bias.end());
	}

	void copy_filter_to(Layer& _layer,const cl::CommandQueue& _queue)
	{
		VERIFY_EXIT(!(options.has_filter() ^ _layer.options.has_filter()));
		if(!options.has_filter() || !_layer.options.has_filter()) return;
		VERIFY_EXIT(_layer.filter.get_M()==filter.get_M() 
			&& _layer.filter.get_N()==filter.get_N()
			);
		filter.copy_data_to_host(_queue);
		_layer.filter.copy_of(filter);
#if defined(DEBUG_OPENCL_NET)
		_layer.filter.debug_test_isnan();
#endif
		_layer.filter.copy_data_to_gpu(_queue);
	}

	void copy_filter_to(Layer& _layer,DLNetKernels* _kernels)
	{
		VERIFY_EXIT(!(options.has_filter() ^ _layer.options.has_filter()));
		if(!options.has_filter() || !_layer.options.has_filter()) return;
		VERIFY_EXIT(_layer.filter.get_M()==filter.get_M() 
			&& _layer.filter.get_N()==filter.get_N()
			);
		_kernels->copy(
			cl::NullRange,cl::NDRange(filter.get_size()),cl::NDRange(_kernels->get_max_workgroup())
			,filter.get_buffer(),_layer.filter.get_buffer()
			);
		_layer.filter.modify_on_gpu();
#if defined(DEBUG_OPENCL_NET)
		_layer.filter.update_from_gpu(_kernels);
		_layer.filter.debug_test_isnan();
#endif
	}

protected:
	void create(const cl::Context& _context,const cl::CommandQueue& _queue,real _scale_coef)
	{
		VERIFY_EXIT(NOT_NULL(options.src));
		if(options.has_filter())
			filter.create(options.input_size + 1, options.out_depth + 1, _context);
		output.create(options.input_count + 1, options.out_depth + 1, _context);
		if(!options.run_only_net_mode)
		{
			inp_t.create(options.src->get_N(),options.src->get_M(),_context);
			if(options.has_filter())
			{
				filter_t.create(options.out_depth + 1,options.input_size + 1,_context);
				filter_dw.create(options.input_size + 1,options.out_depth + 1,_context);
				last_pg.create(options.input_size + 1,options.out_depth + 1,_context);
			}
			if(options.dropout_mode)
				rand_data.create(options.input_count+1,options.out_depth+1,_context);
			output_dw.create(options.input_count+1,options.out_depth+1,_context);
		}
		if(options.has_filter())
			rand_init_weightes(_queue,_scale_coef);
	}

	void rand_init_weightes(const cl::CommandQueue& _queue,real _scale_coef)
	{
		VERIFY_EXIT(filter.get_M()>0 && filter.get_N()>0);

		size_t n = 0,m = 0;
		for(n=0;n<filter.get_N()-1;n++)
		{
			real scale = sqrt((real)1.0/(real)(filter.get_M()-1))*_scale_coef;
			//real measure = min_value<real>(scale*(real)3.0,(real)0.2);
			gauss_rand randn;
			for(m=0;m<filter.get_M()-1;m++)
			{
				//filter.set(m,n,clamp<real>(randn((real)0.0,scale),-measure,measure));
				filter.set(m,n,::rand(-scale,scale));
			}
			filter.set(m,n,options.bias);
		}
		n=filter.get_N()-1;
		for(m=0;m<filter.get_M();m++)
			filter.set(m,n,0);
#if defined(DEBUG_OPENCL_NET)
		filter.debug_test_isnan();
#endif
		filter.copy_data_to_gpu(_queue);
	}

	size_t workgroup_size_set_columnt_to_x(DLNetKernels* _kernels,size_t _size)
	{
		return min_value(_kernels->get_max_workgroup(),_size/_kernels->get_computer_units());
	}

	friend struct DLNet;
};

struct DLNet
{
	typedef std::list<Layer> LayersList;
protected:
	NetOptions options;
	size_t batch_size;
	DLNetKernels* kernels;
	LayersList m_layers;
	Data input;
	Data input_dw;
	Data target;
	Data target_mask;
	bool return_result;

	LayersList::iterator forward_from;
	LayersList::iterator forward_to;
	LayersList::iterator backward_from;
	LayersList::iterator backward_to;

public:
	DLNet(DLNetKernels* _kernels)
		:kernels(_kernels)
		,return_result(false)
		,batch_size(0)
	{
		clear_forward_from_to();
		clear_backward_from_to();
	}

	void clear_forward_from_to()
	{
		forward_from = m_layers.end();
		forward_to = m_layers.end();
	}

	void set_forward_from_to(size_t _idxfrom,size_t _idxto)
	{
		VERIFY_EXIT(_idxfrom<=_idxto);
		forward_to = forward_from = m_layers.begin();
		std::advance(forward_from,_idxfrom);
		std::advance(forward_to,_idxto);
	}

	void clear_backward_from_to()
	{
		backward_from = m_layers.end();
		backward_to = m_layers.end();
	}

	void set_backward_from_to(size_t _idxfrom,size_t _idxto)
	{
		VERIFY_EXIT(_idxfrom>=_idxto);
		backward_to = backward_from = m_layers.begin();
		std::advance(backward_from,_idxfrom);
		std::advance(backward_to,_idxto);
	}

	void create(const std::vector<LayerOptions>& _layers,const NetOptions& _net_options)
	{
		options = _net_options;

		std::vector<LayerOptions>::const_iterator
			lit = _layers.begin()
			,lite = _layers.end()
			;
		for(;lit!=lite;++lit)
		{
			Layer::Options opts;
			static_cast<LayerOptions&>(opts) = *lit;
			opts.src = m_layers.empty()?&input:&(m_layers.back().output);
			opts.src_dw = m_layers.empty()?&input_dw:&(m_layers.back().output_dw);
			opts.input_size = m_layers.empty()?input.get_N()-1:m_layers.back().options.out_depth;
			opts.input_count = m_layers.empty()?input.get_M()-1:m_layers.back().output.get_M()-1;
			opts.run_only_net_mode = options.run_only_net_mode;
			m_layers.push_back(Layer(opts,kernels->get_env()->context,kernels->get_env()->queue,_net_options.weights_scale_coef));
		}
		if(!options.run_only_net_mode)
		{
			target.create_copy(m_layers.back().output,kernels->get_env()->context);
			target_mask.create_copy(target,kernels->get_env()->context);
		}
	}

	size_t get_batch_size() const 
	{
		return batch_size;
	}

	void create_input(size_t _input_size,size_t _input_count)
	{
		batch_size = _input_count;
		input.create(_input_count,_input_size+1,kernels->get_env()->context);
		input_dw.create(_input_count,_input_size+1,kernels->get_env()->context);
	}

	void set_target_mask(size_t _regression_begin,size_t _regression_end)
	{
		VERIFY_EXIT(!options.run_only_net_mode);

		size_t m= 0,n = 0;
		for(m=0;m<target_mask.get_M();m++)
		for(n=0;n<target_mask.get_N();n++)
		{
			target_mask.set(m,n,((n>=_regression_begin && n<_regression_end)?(real)1:(real)0));
		}
		target_mask.copy_data_to_gpu(kernels->get_env()->queue);
	}

	void save(const CString& _file_name)
	{
		BinFile<real> input;
		input.open_to_write(_file_name);

		LayersList::iterator
			it = m_layers.begin()
			,ite = m_layers.end()
			;
		for(;it!=ite;++it)
		{
			Layer& layer = *it;
			layer.save(input,kernels->get_env()->queue);
		}
	}

	void load(const CString& _file_name) 
	{
		if(!FileUtils::IsFileExist(_file_name)) return;

		BinFile<real> input;
		input.open_to_read(_file_name);

		LayersList::iterator
			it = m_layers.begin()
			,ite = m_layers.end()
			;
		for(;it!=ite;++it)
		{
			Layer& layer = *it;
			layer.load(input,kernels->get_env()->queue);
		}
	}

	void async_run()
	{
		VERIFY_EXIT(NOT_NULL(kernels));

		return_result = true;

#if defined(DEBUG_OPENCL_NET)
		input.debug_test_isnan();
#endif

		input.copy_data_to_gpu(kernels->get_env()->queue);

		LayersList::iterator
			it = m_layers.begin()
			,ite = m_layers.end()
			;
		for(;it!=ite;++it)
		{
			Layer& layer = *it;
			layer.forward(kernels);
		}
		//kernels->flush();
	}

	void async_test()
	{
		VERIFY_EXIT(NOT_NULL(kernels));

		return_result = true;

#if defined(DEBUG_OPENCL_NET)
		input.debug_test_isnan();
		target.debug_test_isnan();
#endif

		input.copy_data_to_gpu(kernels->get_env()->queue);
		target.copy_data_to_gpu(kernels->get_env()->queue);

		LayersList::iterator
			it = m_layers.begin()
			,ite = m_layers.end()
			;
		for(;it!=ite;++it)
		{
			Layer& layer = *it;
			layer.forward(kernels,true,false,&target,&target_mask);
			//kernels->flush();
		}
		//kernels->flush();
	}

	void async_forward(bool _train_mode)
	{
		VERIFY_EXIT(!options.run_only_net_mode || (options.run_only_net_mode && !_train_mode));
		VERIFY_EXIT(NOT_NULL(kernels));

		return_result = true;

#if defined(DEBUG_OPENCL_NET)
		input.debug_test_isnan();
#endif

		LayersList::iterator
			it = m_layers.begin()
			,ite = m_layers.end()
			;
		if(NEQL(forward_from,m_layers.end()))
			it = forward_from;
		if(NEQL(forward_to,m_layers.end()))
			ite = forward_to;

		if(EQL(it,m_layers.begin()))
			input.copy_data_to_gpu(kernels->get_env()->queue);

		if(_train_mode) 
		{

#if defined(DEBUG_OPENCL_NET)
			target.debug_test_isnan();
			target_mask.debug_test_isnan();
#endif
			if(EQL(ite,m_layers.end()))
			{
				target.copy_data_to_gpu(kernels->get_env()->queue);
				target_mask.copy_data_to_gpu(kernels->get_env()->queue);
			}
		}

		for(;it!=ite;++it)
		{
			Layer& layer = *it;
			layer.forward(kernels,false,_train_mode,&target,&target_mask);
			//kernels->flush();
		}
	}

	void async_backward()
	{
		VERIFY_EXIT(!m_layers.empty());

		LayersList::iterator
			it = m_layers.end()
			,ite = m_layers.begin()
			;
		if(NEQL(backward_from,m_layers.end()))
			it = backward_from;
		if(NEQL(backward_to,m_layers.end()))
			ite = backward_to;

		if(EQL(it,m_layers.end()))
			m_layers.back().output_dw.copy_data_to_gpu(kernels->get_env()->queue);

		do{
			--it;
			Layer& layer = *it;
			layer.backward(kernels);
			//kernels->flush();
		}while(it!=ite);
	}

	void async_batch()
	{
		VERIFY_EXIT(!options.run_only_net_mode);
		LayersList::iterator
			it = m_layers.begin()
			,ite = m_layers.end()
			;

		if(NEQL(backward_from,m_layers.end()))
			ite = backward_from;
		if(NEQL(backward_to,m_layers.end()))
			it = backward_to;

		for(;it!=ite;++it)
		{
			Layer& layer = *it;
			layer.batch_process(kernels,options);
			//kernels->flush();
		}
	}

	void async_train()
	{
		VERIFY_EXIT(!options.run_only_net_mode);
		VERIFY_EXIT(NOT_NULL(kernels));

		return_result = false;

#if defined(DEBUG_OPENCL_NET)
		if(input.is_modified_on_cpu()) input.debug_test_isnan();
		if(target.is_modified_on_cpu()) target.debug_test_isnan();
		if(target_mask.is_modified_on_cpu()) target_mask.debug_test_isnan();
#endif


		input.copy_data_to_gpu(kernels->get_env()->queue);
		target.copy_data_to_gpu(kernels->get_env()->queue);
		target_mask.update_from_cpu(kernels);

		LayersList::iterator
			it = m_layers.begin()
			,ite = m_layers.end()
			;
		for(;it!=ite;++it)
		{
			Layer& layer = *it;
			layer.forward(kernels,false,true,&target,&target_mask);
			//kernels->flush();
		}

		VERIFY_EXIT(it == ite);
		do{
			--it;
			Layer& layer = *it;
			layer.backward(kernels);
			//kernels->flush();
		}while(it!=m_layers.begin());

		it = m_layers.begin();
		for(;it!=ite;++it)
		{
			Layer& layer = *it;
			layer.batch_process(kernels,options);
			//kernels->flush();
		}
		//kernels->flush();
	}

	void wait()
	{
		kernels->wait();
		if(return_result)
		{
			VERIFY_EXIT(!m_layers.empty());
			m_layers.back().output.copy_data_to_host(kernels->get_env()->queue);
		}
	}

	real get_loss()
	{
		VERIFY_EXIT1(!m_layers.empty(),(real)0.0);
		if(NEQL(backward_from,m_layers.end()))
		{
			LayersList::iterator it = forward_to;
			--it;
			it->output_dw.update_from_gpu(kernels);
			return it->get_loss();
		}
		else
		{
			m_layers.back().output_dw.update_from_gpu(kernels);
			return m_layers.back().get_loss();
		}
	}

	template<typename _Input>
	void set_input(size_t _idx,const _Input& _data)
	{
		std::vector<real> vec;
		vec.resize(_Input::size());
		_data.to_vec(vec);
		set_input_vec(_idx,vec);
	}

	template<typename _Target>
	void get_result(size_t _idx,_Target& _result)
	{
		std::vector<real> vec;
		vec.resize(_Target::size());
		get_result_vec(_idx,vec);
		_result.from_vec(vec);
	}

	template<typename _Target>
	void set_target(size_t _idx,_Target& _target)
	{
		VERIFY_EXIT(!options.run_only_net_mode);

		std::vector<real> vec;
		vec.resize(_Target::size());
		_target.to_vec(vec);
		set_target_vec(_idx,vec);
	}

	void set_input_vec(size_t _idx,const std::vector<real>& _vec)
	{
		input.set_row(_idx,_vec);
	}

	void set_target_vec(size_t _idx,const std::vector<real>& _vec)
	{
		VERIFY_EXIT(!options.run_only_net_mode);
		target.set_row(_idx,_vec);
	}

	void set_target_mask_vec(size_t _idx,const std::vector<real>& _vec)
	{
		VERIFY_EXIT(!options.run_only_net_mode);
		target_mask.set_row(_idx,_vec);
	}

	void set_output_dw_vec(size_t _idx,const std::vector<real>& _vec_dw)
	{
		VERIFY_EXIT(!options.run_only_net_mode);
		VERIFY_EXIT(!m_layers.empty());
		m_layers.back().output_dw.set_row(_idx,_vec_dw);
	}

	void get_result_vec(size_t _idx,std::vector<real>& _vec)
	{
		VERIFY_EXIT(!m_layers.empty());
		m_layers.back().output.update_from_gpu(kernels);
		m_layers.back().output.get_row(_idx,_vec);
	}

	void copy_filters_to(DLNet& _net)
	{
		VERIFY_EXIT(NOT_NULL(kernels));
		VERIFY_EXIT(NOT_NULL(kernels->get_env()));
		VERIFY_EXIT(m_layers.size()==_net.m_layers.size());
		LayersList::iterator
			itsrc = m_layers.begin()
			,itesrc = m_layers.end()
			,itdest = _net.m_layers.begin()
			,itedest = _net.m_layers.end()
			;
		for(;itsrc!=itesrc && itdest!=itedest;++itsrc,++itdest)
		{
			itsrc->copy_filter_to(*itdest,kernels->get_env()->get_queue());
		}
	}
};//struct DLNet

inline
bool load_kernel_from_resource(HMODULE _hModule,LPCTSTR _ResourceID,CString& _kernel_code)
{
	HRSRC hrsrc = ::FindResource(_hModule,_ResourceID,_T("GPUKERNEL"));
	if(IS_NULL(hrsrc)) return false;
	HGLOBAL hgbl = ::LoadResource(_hModule,hrsrc);
	if(IS_NULL(hgbl)) return false;
	DWORD sz = ::SizeofResource(_hModule,hrsrc);
	if(EQL(sz,0)) return false;
	LPVOID ptr = ::LockResource(hgbl);
	if(IS_NULL(ptr)) return false;
	string_converter<char,TCHAR> kernel_codeT((const char*)ptr,sz,CP_ACP);
	_kernel_code = CString(kernel_codeT,sz);
	return true;
}

inline 
bool init(
	IN Environment& _environment,IN DLNetKernels& _kernels
	,const CString& _kernel_code 
	,IN size_t _platform_id,IN cl_device_type _device_type,IN size_t _device_id
	,OUT cl_uint& _status,OUT CString& _log
	)
{
	_environment.select_platform(_platform_id);
	_environment.select_device(_device_type,_device_id);
	string_converter<TCHAR,char> kernels_codeA(_kernel_code,CP_ACP);
	std::string log;
	bool bret = _environment.build_program((LPCSTR)kernels_codeA,_status,log);
	string_converter<char,TCHAR> logT(log.c_str(),log.size(),CP_ACP);
	_log = (LPCTSTR)logT;
	if(!bret) return bret;
	_kernels.create_kernels(&_environment);
	return bret;
}

template<typename _InputState,typename _OutputState>
struct NetT : public DLNet
{
public:
	typedef DLNet::LayersList Layers;
protected:
	typedef DLNet base;

public:
	NetT(DLNetKernels* _kernels)
		:base(_kernels)
	{
	}

	void load(const CString& _file_name)
	{
		base::load(_file_name);
	}

	void save(const CString& _file_name)
	{
		base::save(_file_name);
	}

	void init_net(
		size_t _batch_size
		,size_t _hidden_layer_size,size_t _hidden_layers_count
		,real _learning_rate,real _momentum,real _l2_decay
		,bool _dropout_mode,real _dropout_prob
		,bool _last_layer_half_tanh
		,bool _last_layer_single_relu
		,bool _last_layer_regression
		,bool _run_only_mode
		,real _weights_scale_coef
		,size_t _dropout_layer_count = 1
		)
	{
		create_input(divisible_8(_InputState::size())-1,_batch_size);

		std::vector<LayerOptions> layers;
		{
			LayerOptions opts;
			opts.out_depth = _hidden_layer_size;
			opts.relu_mode = true;
			size_t i = 0;
			for(i=0;i<_hidden_layers_count;i++)
			{
				if(i%_dropout_layer_count==0)
				{
					opts.dropout_mode = _dropout_mode;
					opts.drop_probability = _dropout_prob;
				}
				else
				{
					opts.dropout_mode = false;
					opts.drop_probability = (real)0.0;
				}
				layers.push_back(opts);
			}
		}
		if(_last_layer_half_tanh)
		{
			LayerOptions opts1;
			opts1.out_depth = divisible_8(_OutputState::size())-1;
			layers.push_back(opts1);
			LayerOptions opts2;
			opts2.out_depth = opts1.out_depth;
			opts2.single_half_tanh_mode = true;
			layers.push_back(opts2);
		}
		else if(_last_layer_single_relu)
		{
			LayerOptions opts1;
			opts1.out_depth = divisible_8(_OutputState::size())-1;
			layers.push_back(opts1);
			LayerOptions opts2;
			opts2.out_depth = opts1.out_depth;
			opts2.single_relu_mode = true;
			layers.push_back(opts2);
		}
		else if(_last_layer_regression)
		{
			LayerOptions opts;
			opts.out_depth = divisible_8(_OutputState::size())-1;
			opts.regression_mode = true;
			layers.push_back(opts);
		}
		else
		{
			LayerOptions opts;
			opts.out_depth = divisible_8(_OutputState::size())-1;
			layers.push_back(opts);
		}
		NetOptions net_options;
		net_options.run_only_net_mode = _run_only_mode;
		net_options.learning_rate = _learning_rate;
		net_options.momentum = _momentum;
		net_options.l2_decay = _l2_decay;
		net_options.weights_scale_coef = _weights_scale_coef;

		create(layers,net_options);
	}

	void init_autoencoder(
		size_t _batch_size
		,size_t _hidden_layer_size,size_t _hidden_layers_count
		,size_t _encoded_size
		,real _learning_rate,real _momentum,real _l2_decay
		,bool _dropout_mode,real _dropout_prob
		,bool _run_only_mode
		,real _weights_scale_coef
		,size_t _dropout_layer_count = 1
		)
	{
		VERIFY_EXIT(EQL(divisible_8(_encoded_size)-1,_encoded_size));
		create_input(divisible_8(_InputState::size())-1,_batch_size);
		std::vector<LayerOptions> layers;
		// 1st part
		{
			LayerOptions opts;
			opts.out_depth = _hidden_layer_size;
			opts.relu_mode = true;
			size_t i = 0;
			for(i=0;i<_hidden_layers_count;i++)
			{
				if(i%_dropout_layer_count==0)
				{
					opts.dropout_mode = _dropout_mode;
					opts.drop_probability = _dropout_prob;
				}
				else
				{
					opts.dropout_mode = false;
					opts.drop_probability = (real)0.0;
				}
				layers.push_back(opts);
			}
		}
		// encoded
		{
			LayerOptions opts;
			opts.out_depth = divisible_8(_encoded_size)-1;
			layers.push_back(opts);
		}
		// 2nd part
		{
			LayerOptions opts;
			opts.out_depth = _hidden_layer_size;
			opts.relu_mode = true;
			size_t i = 0;
			for(i=0;i<_hidden_layers_count;i++)
			{
				if(i%_dropout_layer_count==0)
				{
					opts.dropout_mode = _dropout_mode;
					opts.drop_probability = _dropout_prob;
				}
				else
				{
					opts.dropout_mode = false;
					opts.drop_probability = (real)0.0;
				}
				layers.push_back(opts);
			}
		}
		// regression
		{
			LayerOptions opts;
			opts.out_depth = divisible_8(_OutputState::size())-1;
			opts.regression_mode = true;
			layers.push_back(opts);
		}
		NetOptions net_options;
		net_options.run_only_net_mode = _run_only_mode;
		net_options.learning_rate = _learning_rate;
		net_options.momentum = _momentum;
		net_options.l2_decay = _l2_decay;
		net_options.weights_scale_coef = _weights_scale_coef;

		create(layers,net_options);
	}

	Data& get_input_dw()
	{
		return input_dw;
	}

	Layers& get_layers()
	{
		return m_layers;
	}

//	cl::CommandQueue& get_queue()
//	{
//		VERIFY_EXIT1(NOT_NULL(kernels),Fish<cl::CommandQueue>::get());
//		return kernels->get_env()->get_queue();
//	}
	DLNetKernels* get_kernels()
	{
		return kernels;
	}
};// template<> struct NetT

template<typename _InputState,typename _OutputState>
struct NetDataBaseT
{
public:
	typedef std::vector<real> WT;
	typedef _InputState InputState;
	typedef _OutputState OutputState;

	InputState input;
	OutputState output;

protected:
	WT input_w;
	WT result_w;
	WT target_w;
	WT target_mask_w;

public:
	NetDataBaseT()
	{
		input_w.resize(InputState::size());
		result_w.resize(OutputState::size());
		target_w.resize(OutputState::size());
		target_mask_w.resize(OutputState::size());
	}

	void to_vec(bool _train_mode)
	{
		input.to_vec(input_w);
		if(_train_mode)
		{
			output.to_vec(target_w);
			output.set_target_mask(target_mask_w);
		}
	}

	void from_vec()
	{
		output.from_vec(result_w);
	}

	const WT& get_input() const {return input_w;}
	const WT& get_target() const {return target_w;}
	const WT& get_target_mask() const {return target_mask_w;}
	WT& get_result() {return result_w;}

	void copy_to_net(NetT<InputState,OutputState>& _net,size_t _idx,bool _train_mode,bool _forward_mode)
	{
	}

	void copy_from_net(NetT<InputState,OutputState>& _net,size_t _idx,bool _train_mode,bool _forward_mode)
	{
	}
};//template<> struct NetDataBaseT

namespace detail
{
	struct NetFunction 
	{
		virtual bool is_empty_queue() const = 0;
		virtual bool process() = 0;
		virtual void merge_queues() = 0;
		virtual void begin_function() = 0;
		virtual void end_function() = 0;
	};
};//namespace detail

struct NetFunctionTrainStartegy;
struct NetFunctionForwardBackwardStartegy;
struct NetFunctionForwardNoTrainStartegy;
struct NetFunctionForwardTrainStartegy;
struct NetFunctionBackwardTrainStartegy;
struct NetFunctionBackwardNoTrainStartegy;

namespace Private
{
	template<class _NetData,class _Function>
	struct NetFunctionBaseImpl
	{
	protected:
		typedef NetT<typename _NetData::InputState,typename _NetData::OutputState> NetType;
		NetType* m_net;

		std::vector<_NetData*> m_data;
		Queue<_NetData*> m_input_queue;
		Queue<_NetData*> m_process_queue;
		size_t m_batch_size;
		real loss;
		bool sum_loss_mode;
		size_t sum_loss_cnt;

	public:
		NetFunctionBaseImpl()
			:m_net(NULL)
			,m_batch_size(0)
			,loss(0)
			,sum_loss_mode(false)
			,sum_loss_cnt(0)
		{
		}

		void assign(NetType* _net)
		{
			VERIFY_EXIT(IS_NULL(m_net) && NOT_NULL(_net));
			m_net = _net;
			m_batch_size = m_net->get_batch_size();
			m_data.resize(m_batch_size,NULL);
			m_input_queue.resize(m_batch_size);
			m_process_queue.resize(m_batch_size);
		}

		void push_back(_NetData* _cd)
		{
			m_input_queue.push_back(_cd);
		}

		bool empty() const 
		{
			return m_input_queue.empty() && m_process_queue.empty();
		}

		void set_sum_loss_mode(bool _sum_loss_mode)
		{
			sum_loss_mode = _sum_loss_mode;
		}

		void clear_sum_loss()
		{
			sum_loss_cnt = 0;
			loss = 0;
		}

		real get_loss() const 
		{
			if(sum_loss_mode) return loss/max_value(sum_loss_cnt,(size_t)1);
			else return loss;
		}

		size_t get_batch_size() const 
		{
			VERIFY_EXIT1(NOT_NULL(m_net),0);
			return m_net->get_batch_size();
		}

		size_t get_items_count() const 
		{
			return m_input_queue.size() + m_process_queue.size();
		}

	protected:
		void merge_queues()
		{
			Queue<_NetData*>::iterator
				it = m_input_queue.begin()
				,ite = m_input_queue.end()
				;
			for(;it!=ite;++it) {m_process_queue.push_back(*it);}
			m_input_queue.clear();
		}

		bool train()
		{
			bool train_mode = true;
			bool forward_mode = false;
			VERIFY_EXIT1(EQL(m_data.size(),m_batch_size),false);
			size_t i = 0;
			for(i=0;i<m_batch_size;i++)
			{
				if(m_data[i]==NULL)
				{
					if(m_process_queue.empty()) return false; // nothing to process
					m_data[i] = m_process_queue.front();
					m_process_queue.pop_front();
				}
			}
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_data[i]),continue);
				m_data[i]->to_vec(train_mode = true);
				m_net->set_input_vec(i,m_data[i]->get_input());
				m_net->set_target_vec(i,m_data[i]->get_target());
				m_net->set_target_mask_vec(i,m_data[i]->get_target_mask());
				m_data[i]->copy_to_net(*m_net,i,train_mode = true,forward_mode = false);
			}
			// train net
			m_net->async_train();
			m_net->wait();
			if(sum_loss_mode)
			{
				sum_loss_cnt++;
				loss += m_net->get_loss();
			}
			else
			{
				if(::eql(loss,(real)0.0)) loss = m_net->get_loss();
				else loss += (m_net->get_loss() - loss)*Consts::get().loss_k;
			}
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_data[i]),continue);
				m_data[i]->copy_from_net(*m_net,i,train_mode = true,forward_mode = false);
				static_cast<_Function&>(*this).on_processed(m_data[i],forward_mode = false);
				m_data[i] = NULL;
			}
			return true;
		}

		bool forward_backward()
		{
			bool train_mode = false;
			bool forward_mode = true;
			VERIFY_EXIT1(EQL(m_data.size(),m_batch_size),false);
			size_t i = 0;
			for(i=0;i<m_batch_size;i++)
			{
				if(m_data[i]==NULL)
				{
					if(m_process_queue.empty()) return false; // nothing to process
					m_data[i] = m_process_queue.front();
					m_process_queue.pop_front();
				}
			}
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_data[i]),continue);
				m_data[i]->to_vec(train_mode = true);
				m_net->set_input_vec(i,m_data[i]->get_input());
				m_net->set_target_vec(i,m_data[i]->get_target());
				m_net->set_target_mask_vec(i,m_data[i]->get_target_mask());
				m_data[i]->copy_to_net(*m_net,i,train_mode = false,forward_mode = true);
			}
			// forward net
			train_mode = true;
			m_net->async_forward(train_mode = true);
			m_net->wait();
			//if(train_mode = true)
			//{
			if(sum_loss_mode)
			{
				sum_loss_cnt++;
				loss += m_net->get_loss();
			}
			else
			{
				if(::eql(loss,(real)0.0)) loss = m_net->get_loss();
				else loss += (m_net->get_loss() - loss)*Consts::get().loss_k;
			}
			//}
			// backward net
			train_mode = false;
			forward_mode = false;
			m_net->async_backward();
			m_net->wait();
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_data[i]),continue);
				m_data[i]->copy_from_net(*m_net,i,train_mode = false,forward_mode = false);
				static_cast<_Function&>(*this).on_processed(m_data[i],forward_mode = false);
				m_data[i] = NULL;
			}
			return true;
		}

		bool forward(bool _train_mode) 
		{
			bool forward_mode = true;
			VERIFY_EXIT1(EQL(m_data.size(),m_batch_size),false);
			size_t i = 0;
			for(i=0;i<m_batch_size;i++)
			{
				if(m_data[i]==NULL)
				{
					if(m_process_queue.empty()) return false; // nothing to process
					m_data[i] = m_process_queue.front();
					m_process_queue.pop_front();
				}
			}
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_data[i]),continue);
				m_data[i]->to_vec(_train_mode);
				m_net->set_input_vec(i,m_data[i]->get_input());
				if(_train_mode)
				{
					m_net->set_target_vec(i,m_data[i]->get_target());
					m_net->set_target_mask_vec(i,m_data[i]->get_target_mask());
				}
				m_data[i]->copy_to_net(*m_net,i,_train_mode,forward_mode = true);
			}
			// run net
			m_net->async_forward(_train_mode);
			m_net->wait();
			if(_train_mode)
			{
				if(sum_loss_mode)
				{
					sum_loss_cnt++;
					loss += m_net->get_loss();
				}
				else
				{
					if(::eql(loss,(real)0.0)) loss = m_net->get_loss();
					else loss += (m_net->get_loss() - loss)*Consts::get().loss_k;
				}
			}
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_data[i]),continue);
				if(!_train_mode)
				{
					// copy to m_data[i]->output
					m_net->get_result_vec(i,m_data[i]->get_result());
					m_data[i]->from_vec();
					// copy from net any data
				}
				m_data[i]->copy_from_net(*m_net,i,_train_mode,forward_mode = true);
				// do on processed
				static_cast<_Function&>(*this).on_processed(m_data[i],forward_mode = true);
				// clear item
				m_data[i] = NULL;
			}
			return true;
		}

		bool backward(bool _train_mode) 
		{
			bool forward_mode = false;
			VERIFY_EXIT1(EQL(m_data.size(),m_batch_size),false);
			size_t i = 0;
			for(i=0;i<m_batch_size;i++)
			{
				if(m_data[i]==NULL)
				{
					if(m_process_queue.empty()) return false; // nothing to process
					m_data[i] = m_process_queue.front();
					m_process_queue.pop_front();
				}
			}
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_data[i]),continue);
				//m_data[i]->to_vec(_train_mode);
				//m_net->set_input_vec(i,m_data[i].get_input());
				m_data[i]->copy_to_net(*m_net,i,_train_mode,forward_mode = false);
			}
			// run net
			m_net->async_backward();
			if(_train_mode) m_net->async_batch();
			m_net->wait();
			if(sum_loss_mode)
			{
				sum_loss_cnt++;
				loss += m_net->get_loss();
			}
			else
			{
				if(::eql(loss,(real)0.0)) loss = m_net->get_loss();
				else loss += (m_net->get_loss() - loss)*Consts::get().loss_k;
			}
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_data[i]),continue);
				// copy from net any data
				m_data[i]->copy_from_net(*m_net,i,_train_mode,forward_mode = false);
				// do on processed
				static_cast<_Function&>(*this).on_processed(m_data[i],forward_mode = false);
				// clear item
				m_data[i] = NULL;
			}
			return true;
		}

		friend struct NetFunctionTrainStartegy;
		friend struct NetFunctionForwardBackwardStartegy;
		friend struct NetFunctionForwardNoTrainStartegy;
		friend struct NetFunctionForwardTrainStartegy;
		friend struct NetFunctionBackwardTrainStartegy;
		friend struct NetFunctionBackwardNoTrainStartegy;
	};//template<> struct NetFunctionBaseImpl
};//namespace Private

template<class _Strategy,class _NetData,class _MessageHandle>
struct NetFunctionT 
	: public detail::NetFunction
	, public Private::NetFunctionBaseImpl<_NetData,NetFunctionT<_Strategy,_NetData,_MessageHandle> >
{
protected:
	typedef Private::NetFunctionBaseImpl<_NetData,NetFunctionT<_Strategy,_NetData,_MessageHandle> > base_impl;
public:
	typedef void (_MessageHandle::*OnProcessedFuncPtr)(_NetData* _cd,bool _forward_mode);
	typedef void (_MessageHandle::*OnEventFuncPtr)();
	//typedef OnProcessedFunc* OnProcessedFuncPtr;
protected:
	OnProcessedFuncPtr m_on_processed;
	OnEventFuncPtr m_on_begin_function;
	OnEventFuncPtr m_on_end_function;
	_MessageHandle* m_message_handle;

public:
	NetFunctionT()
		:m_on_processed(NULL)
		,m_on_begin_function(NULL)
		,m_on_end_function(NULL)
		,m_message_handle(NULL)
	{
	}

	void set_on_processed(_MessageHandle* _message_handle,OnProcessedFuncPtr _func)
	{
		m_message_handle = _message_handle;
		m_on_processed = _func;
	}

	void set_on_begin_end_function(OnEventFuncPtr _on_begin_func,OnEventFuncPtr _on_end_funct)
	{
		VERIFY_EXIT(NOT_NULL(m_message_handle));
		m_on_begin_function = _on_begin_func;
		m_on_end_function = _on_end_funct;
	}

	void on_processed(_NetData* _cd,bool _forward_mode)
	{
		if(IS_NULL(m_message_handle)) return;
		(m_message_handle->*m_on_processed)(_cd,_forward_mode);
	}

public:
	//struct NetFunction
	//{
	virtual bool is_empty_queue() const
	{
		return empty();
	}

	virtual bool process()
	{
		return _Strategy::process(static_cast<base_impl&>(*this));
	}

	virtual void merge_queues()
	{
		base_impl::merge_queues();
	}

	virtual void begin_function()
	{
		if(IS_NULL(m_on_begin_function)) return;
		(m_message_handle->*m_on_begin_function)();
	}

	virtual void end_function()
	{
		if(IS_NULL(m_on_end_function)) return;
		(m_message_handle->*m_on_end_function)();
	}
	//}
};//template<> struct NetFunctionT

struct NetFunctionTrainStartegy
{
	template<class _NetData,class _Function>
	static bool process(Private::NetFunctionBaseImpl<_NetData,_Function>& _function_impl)
	{
		return _function_impl.train();
	}
};

struct NetFunctionForwardBackwardStartegy
{
	template<class _NetData,class _Function>
	static bool process(Private::NetFunctionBaseImpl<_NetData,_Function>& _function_impl)
	{
		return _function_impl.forward_backward();
	}
};

struct NetFunctionForwardTrainStartegy
{
	template<class _NetData,class _Function>
	static bool process(Private::NetFunctionBaseImpl<_NetData,_Function>& _function_impl)
	{
		bool train_mode = true;
		return _function_impl.forward(train_mode = true);
	}
};

struct NetFunctionForwardNoTrainStartegy
{
	template<class _NetData,class _Function>
	static bool process(Private::NetFunctionBaseImpl<_NetData,_Function>& _function_impl)
	{
		bool train_mode = false;
		return _function_impl.forward(train_mode = false);
	}
};

struct NetFunctionBackwardTrainStartegy
{
	template<class _NetData,class _Function>
	static bool process(Private::NetFunctionBaseImpl<_NetData,_Function>& _function_impl)
	{
		bool train_mode = true;
		return _function_impl.backward(train_mode = true);
	}
};

struct NetFunctionBackwardNoTrainStartegy
{
	template<class _NetData,class _Function>
	static bool process(Private::NetFunctionBaseImpl<_NetData,_Function>& _function_impl)
	{
		bool train_mode = false;
		return _function_impl.backward(train_mode = false);
	}
};


template<typename _InputState,typename _OutputState,class _ChainData>
struct ChainDataBaseT
{
public:
	typedef std::vector<real> WT;
	typedef std::vector<_InputState> Inputs;
	typedef std::vector<_OutputState> Outputs;

	Inputs inputs;
	Outputs outputs;

protected:
	long pos;
	WT input_w;
	WT target_w;
	WT target_mask_w;
	WT output_dw;

public:
	ChainDataBaseT()
		:pos(0)
	{
		input_w.resize(_InputState::size());
		target_w.resize(_OutputState::size());
		target_mask_w.resize(_OutputState::size());
		output_dw.resize(_OutputState::size());
	}

	void next()
	{
		if(eoc()) return;
		pos++;
	}

	void prev()
	{
		if(soc()) return;
		pos--;
	}

	bool eoc() const
	{
		return pos>=(long)inputs.size() || pos>=(long)outputs.size();
	}

	bool soc() const 
	{
		return pos<0;
	}	

	void to_vec(bool _train_mode)
	{
		VERIFY_EXIT(!eoc() && !soc());
		inputs[pos].to_vec(input_w);
		if(_train_mode)
		{
			outputs[pos].to_vec(target_w);
			outputs[pos].set_target_mask(target_mask_w);
		}
	}

	void forward_w()
	{
		VERIFY_EXIT(!eoc() && !soc());
		inputs[pos].forward_w(input_w,static_cast<_ChainData&>(*this),pos);
	}

	void backward_dw()
	{
		VERIFY_EXIT(!soc() && !eoc());
		outputs[pos].backward_dw(output_dw,static_cast<_ChainData&>(*this),pos);
	}

	const WT& get_input() const {return input_w;}
	const WT& get_target() const {return target_w;}
	const WT& get_target_mask() const {return target_mask_w;}
	const WT& get_output_dw() const {return output_dw;}

	//void copy_from_net(NetT<_InputState,_OutputState>& _net,size_t _idx,bool _train_mode,bool _forward_mode);
	//void copy_to_net(NetT<_InputState,_OutputState>& _net,size_t _idx,bool _train_mode,bool _forward_mode);
};//template<> struct ChainDataBaseT

template<typename _ChainData>
struct NetDataHolder : protected std::list<_ChainData>
{
protected:
	typedef std::list<_ChainData> base;

public:
	NetDataHolder()
	{
	}

	_ChainData* create()
	{
		push_back(_ChainData());
		return &back();
	}

	_ChainData* create(const _ChainData& _cd)
	{
		push_back(_ChainData(_cd));
		return &back();
	}

	bool remove(_ChainData* _cd)
	{
		base::iterator 
			it = begin()
			,ite = end()
			;
		for(;it!=ite;++it)
		{
			if(EQL(&*it,_cd))
			{
				erase(it);
				return true;
			}
		}
		return false;
	}
};//template<> struct ChainDataHolder

struct FunctionRunner
{
protected:
	typedef std::vector<detail::NetFunction*> Functions;
	Functions functions;

public:
	FunctionRunner()
	{
	}

	void add_function(detail::NetFunction* _func)
	{
		functions.push_back(_func);
	}

	bool remove_function(detail::NetFunction* _func)
	{
		size_t i = 0;
		for(i=0;i<functions.size();i++)
		{
			VERIFY_DO(NOT_NULL(functions[i]),continue);
			if(EQL(functions[i],_func)) break;
		}
		if(i>=functions.size()) return false;
		functions.erase(functions.begin()+i);
		return true;
	}

	bool process(size_t _max_steps)
	{
		std::vector<size_t> runs;
		size_t i = 0;
		for(i=0;i<functions.size();i++)
		{
			VERIFY_DO(NOT_NULL(functions[i]),continue);
			functions[i]->merge_queues();
		}
		bool all_empty = true;
		for(i=0;i<functions.size();i++)
		{
			VERIFY_DO(NOT_NULL(functions[i]),continue);
			size_t j = 0;
			if(!functions[i]->is_empty_queue())
			{
				functions[i]->begin_function();
				for(j=0;j<_max_steps;j++)
				{
					if(!functions[i]->process()) break;
				}
				functions[i]->end_function();
			}
			all_empty &= functions[i]->is_empty_queue();
		}
		return !all_empty;
	}
};//struct FunctionRunner

struct RNNFunctionForwardTrainStrategy;
struct RNNFunctionBackwardTrainStrategy;
struct RNNFunctionBackwardNoTrainStrategy;
struct RNNFunctionRunStrategy;

namespace Private
{
	template<class _ChainData,class _Function>
	struct RNNFunctionBaseImpl
	{
	protected:
		typedef NetT<typename _ChainData::InputState,typename _ChainData::OutputState> RNNType;
		RNNType* m_net;

		std::vector<_ChainData*> m_chaindata;
		Queue<_ChainData*> m_input_queue;
		Queue<_ChainData*> m_process_queue;
		size_t m_batch_size;
		real loss;

	public:
		RNNFunctionBaseImpl()
			:m_net(NULL)
			,m_batch_size(0)
			,loss(0)
		{
		}

		void assign(RNNType* _net)
		{
			VERIFY_EXIT(IS_NULL(m_net) && NOT_NULL(_net));
			m_net = _net;
			m_batch_size = m_net->get_batch_size();
			m_chaindata.resize(m_batch_size,NULL);
			m_input_queue.resize(m_batch_size);
			m_process_queue.resize(m_batch_size);
		}

		void push_back(_ChainData* _cd)
		{
			m_input_queue.push_back(_cd);
		}

		size_t get_items_count() const 
		{
			return m_input_queue.size() + m_process_queue.size();
		}

		bool empty() const 
		{
			return m_input_queue.empty() && m_process_queue.empty();
		}

		real get_loss() const 
		{
			return loss;
		}

		size_t get_batch_size() const 
		{
			VERIFY_EXIT1(NOT_NULL(m_net),0);
			return m_net->get_batch_size();
		}

	protected:
		void merge_queues()
		{
			Queue<_ChainData*>::iterator
				it = m_input_queue.begin()
				,ite = m_input_queue.end()
				;
			for(;it!=ite;++it) {m_process_queue.push_back(*it);}
			m_input_queue.clear();
		}

		bool forward(bool _train_mode)
		{
			bool forward_mode = true;
			VERIFY_EXIT1(EQL(m_chaindata.size(),m_batch_size),false);
			size_t i = 0;
			// create chain data if is needed
			for(i=0;i<m_batch_size;i++)
			{
				if(m_chaindata[i]==NULL)
				{
					if(m_process_queue.empty()) return false; // nothing to process
					m_chaindata[i] = m_process_queue.front();
					m_process_queue.pop_front();
				}
			}
			// initialize net 
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_chaindata[i]),continue);
				VERIFY_DO(!m_chaindata[i]->eoc(),continue);
				m_chaindata[i]->to_vec(_train_mode);
				m_chaindata[i]->forward_w();
				m_net->set_input_vec(i,m_chaindata[i]->get_input());
				if(_train_mode)
				{
					m_net->set_target_vec(i,m_chaindata[i]->get_target());
					m_net->set_target_mask_vec(i,m_chaindata[i]->get_target_mask());
				}
			}
			// run net
			m_net->async_forward(_train_mode);
			m_net->wait();
			if(_train_mode)
			{
				if(::eql(loss,(real)0.0)) loss = m_net->get_loss();
				else loss += (m_net->get_loss() - loss)*Consts::get().loss_k;
			}
			// copy from net to chain
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_chaindata[i]),continue);
				if(!_train_mode)
				{
					// copy to m_data[i]->output
					m_net->get_result_vec(i,m_chaindata[i]->get_result());
					m_chaindata[i]->from_vec();
				}
				// copy from net any data
				m_chaindata[i]->copy_from_net(*m_net,i,_train_mode,forward_mode = true);
			}
			// post step
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_chaindata[i]),continue);
				m_chaindata[i]->next();
				if(m_chaindata[i]->eoc())
				{
					static_cast<_Function&>(*this).on_processed(m_chaindata[i],forward_mode = true);
					m_chaindata[i] = NULL;
				}
			}
			return true;
		}

		bool backward(bool _train_mode)
		{
			bool forward_mode = false;
			VERIFY_EXIT1(EQL(m_chaindata.size(),m_batch_size),false);
			size_t i = 0;
			// create chain data if is needed
			for(i=0;i<m_batch_size;i++)
			{
				if(m_chaindata[i]==NULL)
				{
					if(m_process_queue.empty()) return false; // nothing to process
					m_chaindata[i] = m_process_queue.front();
					m_process_queue.pop_front();
					m_chaindata[i]->prev();
				}
			}
			// initialize net 
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_chaindata[i]),continue);
				VERIFY_DO(!m_chaindata[i]->soc(),continue);
				m_chaindata[i]->copy_to_net(*m_net,i,_train_mode,forward_mode = false);
				m_chaindata[i]->backward_dw();
				m_net->set_output_dw_vec(i,m_chaindata[i]->get_output_dw());
			}
			// run net
			m_net->async_backward();
			if(_train_mode) m_net->async_batch();
			m_net->wait();
			if(::eql(loss,(real)0.0)) loss = m_net->get_loss();
			else loss += (m_net->get_loss() - loss)*Consts::get().loss_k;
			// copy from net to chain
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_chaindata[i]),continue);
				m_chaindata[i]->copy_from_net(*m_net,i,_train_mode,forward_mode = false);
			}
			// post step
			for(i=0;i<m_batch_size;i++)
			{
				VERIFY_DO(NOT_NULL(m_chaindata[i]),continue);
				m_chaindata[i]->prev();
				if(m_chaindata[i]->soc())
				{
					static_cast<_Function&>(*this).on_processed(m_chaindata[i],forward_mode = false);
					m_chaindata[i] = NULL;
				}
			}
			return true;
		}

		friend struct RNNFunctionForwardTrainStrategy;
		friend struct RNNFunctionBackwardTrainStrategy;
		friend struct RNNFunctionBackwardNoTrainStrategy;
		friend struct RNNFunctionRunStrategy;
	};

};//namespace Private

template<class _Strategy,class _ChainData,class _MessageHandle>
struct RNNFunctionT 
	: public detail::NetFunction
	, public Private::RNNFunctionBaseImpl<_ChainData,RNNFunctionT<_Strategy,_ChainData,_MessageHandle> >
{
protected:
	typedef Private::RNNFunctionBaseImpl<_ChainData,RNNFunctionT<_Strategy,_ChainData,_MessageHandle> > base_impl;
public:
	typedef void (_MessageHandle::*OnProcessedFuncPtr)(_ChainData* _cd,bool _forward_mode);
	typedef void (_MessageHandle::*OnEventFuncPtr)();
protected:
	OnProcessedFuncPtr m_on_processed;
	OnEventFuncPtr m_on_begin_function;
	OnEventFuncPtr m_on_end_function;
	_MessageHandle* m_message_handle;

public:
	RNNFunctionT()
		:m_on_processed(NULL)
		,m_on_begin_function(NULL)
		,m_on_end_function(NULL)
		,m_message_handle(NULL)
	{
	}

	void set_on_processed(_MessageHandle* _message_handle,OnProcessedFuncPtr _func)
	{
		m_message_handle = _message_handle;
		m_on_processed = _func;
	}

	void set_on_begin_end_function(OnEventFuncPtr _on_begin_func,OnEventFuncPtr _on_end_funct)
	{
		VERIFY_EXIT(NOT_NULL(m_message_handle));
		m_on_begin_function = _on_begin_func;
		m_on_end_function = _on_end_funct;
	}

	void on_processed(_ChainData* _cd,bool _forward_mode)
	{
		if(IS_NULL(m_message_handle)) return;
		(m_message_handle->*m_on_processed)(_cd,_forward_mode);
	}

public:
	//struct NetFunction
	//{
	virtual bool is_empty_queue() const
	{
		return empty();
	}

	virtual bool process()
	{
		return _Strategy::process(static_cast<base_impl&>(*this));
	}

	virtual void merge_queues()
	{
		base_impl::merge_queues();
	}

	virtual void begin_function()
	{
		if(IS_NULL(m_on_begin_function)) return;
		(m_message_handle->*m_on_begin_function)();
	}

	virtual void end_function()
	{
		if(IS_NULL(m_on_end_function)) return;
		(m_message_handle->*m_on_end_function)();
	}
	//}
};// template<> struct RNNFunctionT

struct RNNFunctionForwardTrainStrategy
{
	template<class _ChainData,class _Function>
	static bool process(Private::RNNFunctionBaseImpl<_ChainData,_Function>& _function_impl)
	{
		bool train_mode = true;
		return _function_impl.forward(train_mode = true);
	}
};//struct RNNFunctionForwardTrainStrategy

struct RNNFunctionBackwardTrainStrategy
{
	template<class _ChainData,class _Function>
	static bool process(Private::RNNFunctionBaseImpl<_ChainData,_Function>& _function_impl)
	{
		bool train_mode = true;
		return _function_impl.backward(train_mode = true);
	}
};//struct RNNFunctionBackwardTrainStrategy

struct RNNFunctionBackwardNoTrainStrategy
{
	template<class _ChainData,class _Function>
	static bool process(Private::RNNFunctionBaseImpl<_ChainData,_Function>& _function_impl)
	{
		bool train_mode = false;
		return _function_impl.backward(train_mode = false);
	}
};//struct RNNFunctionBackwardNoTrainStrategy

struct RNNFunctionRunStrategy
{
	template<class _ChainData,class _Function>
	static bool process(Private::RNNFunctionBaseImpl<_ChainData,_Function>& _function_impl)
	{
		bool train_mode = false;
		return _function_impl.forward(train_mode = false);
	}
};//struct RNNFunctionRunStrategy

};//namespace openlc