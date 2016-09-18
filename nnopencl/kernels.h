#pragma once 

#include "environment.h"

namespace opencl
{
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

	protected:
		void create_base_kernels(Environment* _p_opencl_env)
		{
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
		}

		void init_env(Environment* _p_opencl_env)
		{
			m_env = _p_opencl_env;
			max_computer_units = m_env->device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
			max_workgroup = m_env->device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
		}

	public:
		DLNetKernels()
			:m_env(NULL)
			,async_operation(false)
			,max_computer_units(0)
			,max_workgroup(0)
		{
		}

		virtual void create_kernels(Environment* _p_opencl_env)
		{
			init_env(_p_opencl_env);
			create_base_kernels(_p_opencl_env);
		}

		void flush()
		{
			VERIFY_EXIT(NOT_NULL(m_env));
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
		IN Environment& _environment
		,IN DLNetKernels& _kernels
		,const CString& _kernel_code 
		,IN size_t _platform_id
		,IN cl_device_type _device_type
		,IN size_t _device_id
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
		(&_kernels)->create_kernels(&_environment); // call virtual function
		return bret;
	}

	template<typename _Kernels>
	struct EnvironmetAndKernelsT
	{
	protected:
		_Kernels kernels;
		Environment environment;
		cl_uint init_status;
		CString err_log;

	public:
		EnvironmetAndKernelsT()
			:init_status(0)
		{
		}

		_Kernels* get_kernels()
		{
			return &kernels;
		}

		Environment* get_environment()
		{
			return &environment;
		}

		bool init_kernel_code_from_resource(
			IN HMODULE _hmodule,IN LPCTSTR _ResourceID
			,IN size_t _platform_id
			,IN cl_device_type _device_type
			,IN size_t _device_id
			)
		{	
			CString kernel_code;
			if(!opencl::load_kernel_from_resource(
				_hmodule,_ResourceID,kernel_code)
				) 
			{
				err_log = _T("Can`t load the kernel code from the resource.");
				return false;
			}
			return init_with_kernel_code(
				kernel_code
				,_platform_id,_device_type,_device_id
				);
		}

		bool init_with_kernel_code(
			const CString& _kernel_code 
			,IN size_t _platform_id
			,IN cl_device_type _device_type
			,IN size_t _device_id
			)
		{
			if(!opencl::init(
				environment,kernels,_kernel_code
				,_platform_id,_device_type,_device_id
				,init_status,err_log)
				) return false;
			return true;
		}

		cl_uint get_init_status() const 
		{
			return init_status;
		}

		const CString& get_error_log() const 
		{
			return err_log;
		}

	};//template<> struct EnvironmetAndKernelsT 

	typedef EnvironmetAndKernelsT<DLNetKernels> EnvironmetAndDefKernels;
}