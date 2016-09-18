#pragma once

#include <utils/utils.h>
#include "utils.h"

namespace sequence
{
	enum OperationEn
	{
		op_equal = 0
		,op_not_equal = 1
	};

};//namespace sequence

namespace common
{
	inline
		void convert(const sequence::OperationEn& _op,CString& _str)
	{
		_str.Empty();
		switch(_op)
		{
		case sequence::op_equal: _str = _T("equal"); break;
		case sequence::op_not_equal: _str = _T("not_equal"); break;
		}
	}

	inline 
		void convert(const CString& _str,sequence::OperationEn& _op)
	{
		_op = sequence::op_equal;
		CString str = _str;
		str.Trim();
		if(!str.CompareNoCase(_T("equal"))) _op = sequence::op_equal;
		else if(!str.CompareNoCase(_T("not_equal"))) _op = sequence::op_not_equal;
	}
};//namespace common

namespace sequence
{
	template<typename _Item>
	struct State;

	namespace Private
	{
		template<typename _Item>
		struct Result
		{
			_Item output;
			real output_weight;

			Result(const _Item& _output = _Item()
				,real _output_weight = (real)0.0
				)
				:output(_output)
				,output_weight(_output_weight)
			{
			}
		};
	};//namespace Private

	template<typename _Item>	
	struct Input
	{
		_Item input;
		OperationEn op;
		CString next_state_name;
		State<_Item>* next_state;
		real next_state_weight;
		_Item output;
		real output_weight;

		Input(
			OperationEn _op = op_equal
			,_Item _input = _Item()
			,const CString& _next_state_name = CString()
			,real _next_state_weight = (real)0.0
			,_Item _output = _Item()
			,real _output_weight = (real)0.0
			)
			:input(_input)
			,op(_op)
			,next_state_name(_next_state_name)
			,next_state(NULL)
			,next_state_weight(_next_state_weight)
			,output(_output)
			,output_weight(_output_weight)
		{
		}
	};// template<> struct Input

	namespace Private
	{
		template<typename _Item>
		inline
			CString get_str(const Input<_Item>& _item)
		{
			CString input_str;
			CString output_str;
			common::convert(_item.input,input_str);
			common::convert(_item.output,output_str);
			return Format(_T("%s->%s (%g,%g)"),(LPCTSTR)input_str,(LPCTSTR)output_str,_item.next_state_weight,_item.output_weight);
		}

		inline
			CString get_str(const Input<char>& _item)
		{
			return Format(_T("%c->%c (%g,%g)"),_item.input,_item.output,_item.next_state_weight,_item.output_weight);
		}

		inline
			CString get_str(const Input<WCHAR>& _item)
		{
			return Format(_T("%c->%c (%g,%g)"),_item.input,_item.output,_item.next_state_weight,_item.output_weight);
		}
	}

	template<typename _Item>
	struct State
	{
	public:
		typedef std::vector<Input<_Item> > Inputs;
	protected:
		CString name;
		Inputs inputs;

	public:
		State(const CString& _name = CString())
			:name(_name)
		{
		}

		const CString& get_name() const 
		{
			return name;
		}

		CString get_inputs() const 
		{
			CString inputs_str;
			Inputs::const_iterator\
				it = inputs.begin()
				,ite = inputs.end()
				;
			for(;it!=ite;++it)
			{
				inputs_str += Private::get_str(*it);
				inputs_str += _T(", ");
			}
			return inputs_str;
		}

		void add_input(const Input<_Item>& _input)
		{
			inputs.push_back(_input);
		}

		typename Inputs::iterator inputs_begin()
		{
			return inputs.begin();
		}

		typename Inputs::iterator inputs_end()
		{
			return inputs.end();
		}

		State<_Item>* next(const _Item& _current,std::list<Private::Result<_Item> >& _results) const
		{
			std::list<const Input<_Item>*> active;
			Inputs::const_iterator 
				inpit = inputs.begin()
				,inpite = inputs.end()
				;
			for(;inpit!=inpite;++inpit)
			{
				const Input<_Item>& input = *inpit;
				if(
					input.op==op_equal && input.input==_current
					|| !input.op!=op_not_equal && input.input!=_current
					)
				{
					active.push_back(&*inpit);
				}
			}
			if(active.empty()) return NULL;
			real sum = (real)0.0;
			std::list<const Input<_Item>*>::iterator
				it = active.begin()
				,ite = active.end()
				;
			for(;it!=ite;++it)
			{
				sum += (*it)->next_state_weight;
			}
			real val = ::rand(sum);
			it = active.begin();
			sum = (real)0.0;
			for(;it!=ite;++it)
			{
				sum += (*it)->next_state_weight;
				if(val<=sum) 
				{
					_results.push_back(Private::Result<_Item>((*it)->output,(*it)->output_weight));
					return (*it)->next_state;
				}
			}
			_results.push_back(Private::Result<_Item>(active.back()->output,active.back()->output_weight));
			return active.back()->next_state;
		}
	};// template<> struct State

	template<typename _Item>
	struct StateMachine
	{
	protected:
		typedef std::list<State<_Item> > States;
		States states;
		typedef std::map<CString,State<_Item>*> StatesMap;
		StatesMap statesmap;
		State<_Item>* current_state;

	public:
		StateMachine()
			:current_state(NULL)
		{
		}

		void compile()
		{
			States::iterator
				sit = states.begin()
				,site = states.end()
				;
			for(;sit!=site;++sit)
			{
				State<_Item>& state = *sit;
				State<_Item>::Inputs::iterator
					inpit = state.inputs_begin()
					,inpite = state.inputs_end()
					;
				for(;inpit!=inpite;++inpit)
				{
					Input<_Item>& input = *inpit;
					StatesMap::iterator fit = statesmap.find(input.next_state_name);
					VERIFY_DO(fit!=statesmap.end(),continue);
					input.next_state = fit->second;
				}
			}
		}

		void add_state(const CString& _name)
		{
			VERIFY_EXIT(statesmap.find(_name)==statesmap.end());
			states.push_back(State<_Item>(_name));
			statesmap.insert(StatesMap::value_type(_name,&(states.back())));
		}

		void add_input(const Input<_Item>& _input)
		{
			VERIFY_EXIT(!states.empty());
			states.back().add_input(_input);
		}

		void reset()
		{
			current_state = NULL;
			StatesMap::iterator
				it = statesmap.begin()
				,ite = statesmap.end()
				;
			for(;it!=ite;++it)
			{
				if(!it->first.CompareNoCase(_T("main")))
				{
					current_state = it->second;
					return;
				}
			}
		}

		void next(const _Item& _current,std::list<Private::Result<_Item> >& _results)
		{
			VERIFY_EXIT(NOT_NULL(current_state));
			State<_Item>* next_item = current_state->next(_current,_results);
			if(NOT_NULL(next_item))
			{
				current_state = next_item;
			}
#if defined(TRACE_SEQUENCE_GENERATOR)
			std::tcout << (LPCTSTR)current_state->get_name() << _T(" ") << (LPCTSTR)current_state->get_inputs() << _T("\t");
#endif
		}
	};// template<> struct StateMachine

	template<typename _Item>
	struct Generator
	{
	protected:
		typedef std::list<StateMachine<_Item> > StateMachines;
		StateMachines machines;

		void compile()
		{
			StateMachines::iterator
				it = machines.begin()
				,ite = machines.end()
				;
			for(;it!=ite;++it)
			{
				it->compile();
			}
		}

		void add_machine()
		{
			machines.push_back(StateMachine<_Item>());
		}

		void add_state(const CString& _name)
		{
			VERIFY_EXIT(!machines.empty());
			machines.back().add_state(_name);
		}

		void add_input(const Input<_Item>& _input)
		{
			VERIFY_EXIT(!machines.empty());
			machines.back().add_input(_input);
		}

	public:
		void reset()
		{
			StateMachines::iterator
				it = machines.begin()
				,ite = machines.end()
				;
			for(;it!=ite;++it)
			{
				it->reset();
			}
		}

		_Item next(const _Item& _current)
		{
			std::list<Private::Result<_Item> > results;
			StateMachines::iterator 
				smit = machines.begin()
				,smite = machines.end()
				;
			for(;smit!=smite;++smit)
			{
				smit->next(_current,results);
#if defined(TRACE_SEQUENCE_GENERATOR)
				std::tcout << _T("|\t");
#endif
			}
#if defined(TRACE_SEQUENCE_GENERATOR)
			std::tcout << _T("\n");
#endif
			real sum = (real)0.0;
			std::list<Private::Result<_Item> >::const_iterator
				it = results.begin()
				,ite = results.end()
				;
			for(;it!=ite;++it)
			{
				const Private::Result<_Item>& result = *it;
				sum += result.output_weight;
			}
			real val = ::rand(sum);
			sum = 0;
			it = results.begin();
			for(;it!=ite;++it)
			{
				const Private::Result<_Item>& result = *it;
				sum += result.output_weight;
				if(val<=sum) return result.output;
			}
			return results.back().output;
		}
	};// template<> struct Generator

};//namespace sequence

namespace char_sequence
{
	typedef sequence::Input<char> Input;
	typedef sequence::State<char> State;
	typedef sequence::StateMachine<char> StateMachine;
	typedef sequence::Generator<char> Generator;
};//namespace char_sequence

namespace TCHAR_sequence
{
	typedef sequence::Input<TCHAR> Input;
	typedef sequence::State<TCHAR> State;
	typedef sequence::StateMachine<TCHAR> StateMachine;
	typedef sequence::Generator<TCHAR> Generator;
};//namespace TCHAR_sequence

namespace int_sequence
{
	typedef sequence::Input<int> Input;
	typedef sequence::State<int> State;
	typedef sequence::StateMachine<int> StateMachine;
	typedef sequence::Generator<int> Generator;
};//namespace int_sequence
