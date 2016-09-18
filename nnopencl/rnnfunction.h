#pragma once

#include "netfunction.h"

namespace opencl
{
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
			real loss_k;

		public:
			RNNFunctionBaseImpl()
				:m_net(NULL)
				,m_batch_size(0)
				,loss(0)
				,loss_k((real)1-3)
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

			void set_loss_k(real _loss_k)
			{
				loss_k = _loss_k;
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
					else loss += (m_net->get_loss() - loss)*loss_k;
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
				else loss += (m_net->get_loss() - loss)*loss_k;
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

};//namespace opencl