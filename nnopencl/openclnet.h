#pragma once 

#include "utils.h"
#include "environment.h"
#include "kernels.h"
#include "databuffer.h"

#include <utils/utils.h>
#include <utils/fileutils.h>
#include <utils/binfile.h>


namespace opencl
{
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

	struct DLNet;

	//////////////////////////////////////////////////////////////////////////
	//
	// opencl::Layer
	//

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

			bool dropout_mode = options.dropout_mode;
			if(options.run_only_net_mode) dropout_mode = false;

			if(dropout_mode && _train_mode)
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
			else if(options.relu_mode && dropout_mode)
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
			else if(options.relu_mode && !dropout_mode)
			{
				_kernels->forward_fc_layer_relu(
					cl::NullRange,cl::NDRange(res.get_N()/8,res.get_M()/4),cl::NDRange(8,8)
					,src.get_M(),res.get_N(),src.get_N()
					,src.get_buffer(),filter.get_buffer(),res.get_buffer()
					,src.get_N(),filter.get_N(),res.get_N()
					);
				res.modify_on_gpu();
			}
			else if(!options.relu_mode && dropout_mode)
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
			else if(!options.relu_mode && !dropout_mode)
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

			real c1 = (real)1.0;
			if(options.run_only_net_mode && options.dropout_mode)
				c1 = (real)1.0 - options.drop_probability;

			for(m=0;m<filter.get_M()-1;m++)
			{
				for(n=0;n<filter.get_N()-1;n++)
				{
					filter.set(m,n,filters[n][m]*c1);
				}
				filter.set(m,n,0);
			}
			VERIFY(m==filter.get_M()-1);
			for(n=0;n<filter.get_N()-1;n++)
				filter.set(m,n,bias[n]*c1);
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

			real c1 = (real)1.0;
			if(options.run_only_net_mode && options.dropout_mode && !eql(options.drop_probability,(real)1.0))
				c1 = (real)1.0/((real)1.0 - options.drop_probability);

			size_t n = 0,m = 0;
			for(m=0;m<filter.get_M()-1;m++)
			{
				for(n=0;n<filter.get_N()-1;n++)
				{
					filters[n][m] = filter.get(m,n)*c1;
				}
			}
			VERIFY(m==filter.get_M()-1);
			for(n=0;n<filter.get_N()-1;n++)
				bias[n] = filter.get(m,n)*c1;

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
			if(!_layer.options.run_only_net_mode && options.run_only_net_mode && options.dropout_mode)
			{
				real c1 = (real)1.0 - options.drop_probability;
				_layer.filter.mul(c1);
			}
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
			if(!_layer.options.run_only_net_mode && options.run_only_net_mode && options.dropout_mode)
			{
				VERIFY_EXIT(NOT_NULL(_kernels->get_env()));
				filter.copy_data_to_host(_kernels->get_env()->get_queue());
				_layer.filter.copy_of(filter);
				real c1 = (real)1.0 - options.drop_probability;
				_layer.filter.mul(c1);
				_layer.filter.copy_data_to_gpu(_kernels->get_env()->get_queue());
			}
			else
			{
				_kernels->copy(
					cl::NullRange,cl::NDRange(filter.get_size()),cl::NDRange(_kernels->get_max_workgroup())
					,filter.get_buffer(),_layer.filter.get_buffer()
					);
				_layer.filter.modify_on_gpu();
			}
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
			real scale = sqrt((real)1.0/(real)(filter.get_M()-1))*_scale_coef;
			if(options.run_only_net_mode && options.dropout_mode)
				scale *= (real)1.0 - options.drop_probability;
			gauss_rand randn;

			for(n=0;n<filter.get_N()-1;n++)
			{
				for(m=0;m<filter.get_M()-1;m++)
				{
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
	};//struct Layer

	//////////////////////////////////////////////////////////////////////////
	//
	// opencl::DLNet
	//

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
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

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
				m_layers.push_back(Layer(opts,kernels->get_env()->get_context(),kernels->get_env()->queue,_net_options.weights_scale_coef));
			}
			if(!options.run_only_net_mode)
			{
				target.create_copy(m_layers.back().output,kernels->get_env()->get_context());
				target_mask.create_copy(target,kernels->get_env()->get_context());
			}
		}

		size_t get_batch_size() const 
		{
			return batch_size;
		}

		void create_input(size_t _input_size,size_t _input_count)
		{
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

			batch_size = _input_count;
			input.create(_input_count,_input_size+1,kernels->get_env()->get_context());
			input_dw.create(_input_count,_input_size+1,kernels->get_env()->get_context());
		}

		void set_target_mask(size_t _regression_begin,size_t _regression_end)
		{
			VERIFY_EXIT(!options.run_only_net_mode);
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

			size_t m= 0,n = 0;
			for(m=0;m<target_mask.get_M();m++)
			for(n=0;n<target_mask.get_N();n++)
			{
				target_mask.set(m,n,((n>=_regression_begin && n<_regression_end)?(real)1:(real)0));
			}
			target_mask.copy_data_to_gpu(kernels->get_env()->get_queue());
		}

		void save(const CString& _file_name)
		{
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

			BinFile<real> input;
			input.open_to_write(_file_name);

			wait_kernels();

			LayersList::iterator
				it = m_layers.begin()
				,ite = m_layers.end()
				;
			for(;it!=ite;++it)
			{
				Layer& layer = *it;
				layer.save(input,kernels->get_env()->get_queue());
			}
		}

		void load(const CString& _file_name) 
		{
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

			if(!FileUtils::IsFileExist(_file_name)) return;

			BinFile<real> input;
			input.open_to_read(_file_name);

			wait_kernels();

			LayersList::iterator
				it = m_layers.begin()
				,ite = m_layers.end()
				;
			for(;it!=ite;++it)
			{
				Layer& layer = *it;
				layer.load(input,kernels->get_env()->get_queue());
			}
		}

		void async_run()
		{
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

			return_result = true;

#if defined(DEBUG_OPENCL_NET)
			input.debug_test_isnan();
#endif

			input.copy_data_to_gpu(kernels->get_env()->get_queue());

			LayersList::iterator
				it = m_layers.begin()
				,ite = m_layers.end()
				;
			for(;it!=ite;++it)
			{
				Layer& layer = *it;
				layer.forward(kernels);
			}
		}

		void async_test()
		{
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

			return_result = true;

#if defined(DEBUG_OPENCL_NET)
			input.debug_test_isnan();
			target.debug_test_isnan();
#endif

			input.copy_data_to_gpu(kernels->get_env()->get_queue());
			target.copy_data_to_gpu(kernels->get_env()->get_queue());

			LayersList::iterator
				it = m_layers.begin()
				,ite = m_layers.end()
				;
			for(;it!=ite;++it)
			{
				Layer& layer = *it;
				layer.forward(kernels,true,false,&target,&target_mask);
			}
		}

		void async_forward(bool _train_mode)
		{
			VERIFY_EXIT(!options.run_only_net_mode || (options.run_only_net_mode && !_train_mode));
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

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
				input.copy_data_to_gpu(kernels->get_env()->get_queue());

			if(_train_mode) 
			{

#if defined(DEBUG_OPENCL_NET)
				target.debug_test_isnan();
				target_mask.debug_test_isnan();
#endif
				if(EQL(ite,m_layers.end()))
				{
					target.copy_data_to_gpu(kernels->get_env()->get_queue());
					target_mask.copy_data_to_gpu(kernels->get_env()->get_queue());
				}
			}

			for(;it!=ite;++it)
			{
				Layer& layer = *it;
				layer.forward(kernels,false,_train_mode,&target,&target_mask);
			}
		}

		void async_backward()
		{
			VERIFY_EXIT(!m_layers.empty());
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

			LayersList::iterator
				it = m_layers.end()
				,ite = m_layers.begin()
				;
			if(NEQL(backward_from,m_layers.end()))
				it = backward_from;
			if(NEQL(backward_to,m_layers.end()))
				ite = backward_to;

			if(EQL(it,m_layers.end()))
				m_layers.back().output_dw.copy_data_to_gpu(kernels->get_env()->get_queue());

			do{
				--it;
				Layer& layer = *it;
				layer.backward(kernels);
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
			}
		}

		void async_train()
		{
			VERIFY_EXIT(!options.run_only_net_mode);
			VERIFY_EXIT(NOT_NULL(kernels));
			VERIFY_EXIT(NOT_NULL(kernels->get_env()));

			return_result = false;

#if defined(DEBUG_OPENCL_NET)
			if(input.is_modified_on_cpu()) input.debug_test_isnan();
			if(target.is_modified_on_cpu()) target.debug_test_isnan();
			if(target_mask.is_modified_on_cpu()) target_mask.debug_test_isnan();
#endif


			input.copy_data_to_gpu(kernels->get_env()->get_queue());
			target.copy_data_to_gpu(kernels->get_env()->get_queue());
			target_mask.update_from_cpu(kernels);

			LayersList::iterator
				it = m_layers.begin()
				,ite = m_layers.end()
				;
			for(;it!=ite;++it)
			{
				Layer& layer = *it;
				layer.forward(kernels,false,true,&target,&target_mask);
			}

			VERIFY_EXIT(it == ite);
			do{
				--it;
				Layer& layer = *it;
				layer.backward(kernels);
			}while(it!=m_layers.begin());

			it = m_layers.begin();
			for(;it!=ite;++it)
			{
				Layer& layer = *it;
				layer.batch_process(kernels,options);
			}
		}

		void wait()
		{
			VERIFY_EXIT(NOT_NULL(kernels));
			wait_kernels();
			if(return_result)
			{
				VERIFY_EXIT(!m_layers.empty());
				VERIFY_EXIT(NOT_NULL(kernels->get_env()));
				m_layers.back().output.copy_data_to_host(kernels->get_env()->get_queue());
			}
		}

		real get_loss()
		{
			VERIFY_EXIT1(!m_layers.empty(),(real)0.0);
			VERIFY_EXIT1(NOT_NULL(kernels),(real)0.0);

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

			wait_kernels();

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
	protected:
		void wait_kernels()
		{
			VERIFY_EXIT(NOT_NULL(kernels));
			kernels->wait();
		}
	};//struct DLNet

}

