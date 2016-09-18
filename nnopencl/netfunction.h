#pragma once

#include "NetT.h"

#include <utils/queue.h>

namespace opencl
{
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
			real loss_k;
			bool sum_loss_mode;
			size_t sum_loss_cnt;

		public:
			NetFunctionBaseImpl()
				:m_net(NULL)
				,m_batch_size(0)
				,loss(0)
				,sum_loss_mode(false)
				,sum_loss_cnt(0)
				,loss_k((real)1e-3)
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

			void set_loss_k(real _loss_k)
			{
				loss_k = _loss_k;
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
					else loss += (m_net->get_loss() - loss)*loss_k;
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
					else loss += (m_net->get_loss() - loss)*loss_k;
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
						else loss += (m_net->get_loss() - loss)*loss_k;
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
					else loss += (m_net->get_loss() - loss)*loss_k;
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

};//namespace opencl