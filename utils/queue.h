#pragma once 

#include "utils.h"
#include "cyclicbuffer.h"

template<typename _Type>
struct Queue : public CyclicBuffer<_Type>
{
protected:
	typedef CyclicBuffer<_Type> base;
	size_t grow_size;
public:
	Queue()
		:grow_size(0)
	{
	}

	void resize(size_t _buffer_size,size_t _grow_size = 0)
	{
		grow_size = (_grow_size!=0)?
			_grow_size
			:grow_size
			;
		base::resize(_buffer_size);
	}

	void set_size(const Queue<_Type>& _from)
	{
		grow_size = _from.grow_size;
		base::resize(_from.full_size());
	}

	void grow(size_t _grow_size)
	{
		if(_grow_size==0) _grow_size = 1;

		size_t buff_last_sz = base::base::size();
		base::base::resize(buff_last_sz+_grow_size);
		size_t buff_cur_sz = base::base::size();
		if(end_pos%buff_last_sz<=begin_pos%buff_last_sz && end_pos>begin_pos)
		{
			size_t bp = begin_pos%buff_last_sz;
			size_t block_sz = buff_last_sz-bp;
			if(buff_cur_sz-block_sz>buff_last_sz)
			{
				std::copy(base::base::begin()+bp,base::base::begin()+buff_last_sz
					,base::base::begin()+buff_cur_sz-block_sz
					);
			}
			else
			{
				size_t i=0,ps=0,pd=0;
				for(i=0,ps=buff_last_sz-1,pd=buff_cur_sz-1;i<block_sz;i++,ps--,pd--)
				{
					base::base::operator[](pd) = base::base::operator[](ps);
				}
			}
			end_pos %= buff_last_sz;
			end_pos += buff_cur_sz;
			begin_pos = buff_cur_sz-block_sz;
		}
	}

	void push(const _Type& _data)
	{
		VERIFY_EXIT(allocated());
		if(full())
		{
			grow(grow_size!=0?grow_size:base::base::size());
		}
		base::push(_data);
	}

	void push_back(const _Type& _data)
	{
		push(_data);
	}

	template<typename _Iterator>
	void push(_Iterator _it,_Iterator _ite)
	{
		for(;_it!=_ite;++_it)
		{	
			push(*_it);
		}
	}

	template<typename _Iterator>
	size_t copy_to(_Iterator _it,_Iterator _ite)
	{
		size_t i=0;
		for(i=0;_it!=_ite && i<size();++_it,i++)
		{	
			*_it = get(i);
		}
		return i;
	}

	template<typename _Iterator>
	size_t move_to(_Iterator _it,_Iterator _ite)
	{
		size_t i=0;
		for(i=0;_it!=_ite && !empty();++_it,i++)
		{	
			*_it = get(0);
			pop();
		}
		return i;
	}

	size_t move_to(std::vector<_Type>& _buf)
	{
		size_t sz = size();
		_buf.resize(sz);
		size_t i=0;
		for(i=0;i<sz;i++)
		{
			_buf[i] = get(i);
		}
		pop(sz);
		return sz;
	}
};//template<> struct Queue
