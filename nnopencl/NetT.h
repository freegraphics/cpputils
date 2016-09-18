#pragma once 

#include "openclnet.h"

namespace opencl
{
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
			,size_t _dropout_layer_count = 0
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
					if(_dropout_layer_count  && (i%_dropout_layer_count==_dropout_layer_count/2))
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
			,size_t _dropout_layer_count = 0
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
					if(_dropout_layer_count && i%_dropout_layer_count==_dropout_layer_count/2)
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
					if(_dropout_layer_count && i%_dropout_layer_count==_dropout_layer_count/2)
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

	template<typename _NetData>
	struct NetDataHolder : protected std::list<_NetData>
	{
	protected:
		typedef std::list<_NetData> base;

	public:
		NetDataHolder()
		{
		}

		_NetData* create()
		{
			push_back(_NetData());
			return &back();
		}

		_NetData* create(const _NetData& _cd)
		{
			push_back(_NetData(_cd));
			return &back();
		}

		bool remove(_NetData* _cd)
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
	};//template<> struct NetDataHolder

}