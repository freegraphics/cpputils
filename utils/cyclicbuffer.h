#pragma once 

#include <vector>
#include "utils.h"
#include "UnitTests.h"

template<typename _Type>
struct CyclicBuffer : protected std::vector<_Type>
{
protected:
	template<typename _Type>
	struct IteratorBase
	{
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef _Type value_type;
		typedef size_t difference_type;
		typedef difference_type distance_type;	// retained
		typedef _Type* pointer;
		typedef _Type& reference;

	protected:
		size_t pos;
		CyclicBuffer<_Type>* buffer;

	public:
		IteratorBase(CyclicBuffer<_Type>* _buffer = NULL,size_t _pos = 0)
			:buffer(_buffer)
			,pos(_pos)
		{
		}

		bool eob() const 
		{
			return pos>=buffer->size();
		}

		reference operator * () const
		{
			return buffer->get(pos);
		}

		pointer operator -> () const
		{
			return &buffer->get(pos);
		}

		IteratorBase& operator ++ ()
		{
			if(!eob()) pos++;
			return *this;
		}

		IteratorBase operator ++ (int)
		{
			IteratorBase it(*this);
			operator ++ ();
			return it;
		}

		bool operator == (const IteratorBase& _it) const
		{
			if(buffer!=_it.buffer) return false;
			if(eob() && _it.eob()) return true;
			if(eob() || _it.eob()) return false;
			return pos==_it.pos;
		}

		bool operator != (const IteratorBase& _it) const
		{
			return !operator==(_it);
		}

		bool operator < (const IteratorBase& _it) const
		{
			if(buffer!=_it.buffer) return false;
			if(eob() && _it.eob()) return false;
			if(eob() || _it.eob()) return !eob() && _it.eob();
			return pos<_it.pos;
		}

		bool operator > (const IteratorBase& _it) const
		{
			return _it.operator < (*this);
		}
	};//struct IteratorBase

	template<typename _Type>
	struct ConstsIterator : public IteratorBase<_Type>
	{
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef _Type value_type;
		typedef size_t difference_type;
		typedef difference_type distance_type;	// retained
		typedef const _Type* pointer;
		typedef const _Type& reference;

	public:
		ConstsIterator(const CyclicBuffer<_Type>* _buffer = NULL,size_t _pos = 0)
			:IteratorBase(const_cast<CyclicBuffer<_Type>*>(_buffer),_pos)
		{
		}

		ConstsIterator(const IteratorBase<_Type>& _iterator)
			:IteratorBase(_iterator)
		{
		}

		reference operator * () const
		{
			return buffer->get(pos);
		}

		pointer operator -> () const
		{
			return &buffer->get(pos);
		}
	};
public:
	typedef IteratorBase<_Type> iterator;
	typedef ConstsIterator<_Type> const_iterator;
	typedef _Type value_type;

protected:
	size_t begin_pos;
	size_t end_pos;

	typedef std::vector<_Type> base;
public:

	CyclicBuffer(size_t _buffer_size = 0)
		:begin_pos(0)
		,end_pos(0)
	{
		resize(_buffer_size);
	}

	void resize(size_t _buffer_size)
	{
		base::resize(_buffer_size);
		clear();
	}

	void push(const _Type& _data)
	{
		VERIFY_EXIT(allocated());
		if(full())
		{
			begin_pos++;
		}
		static_cast<base&>(*this)[end_pos%full_size()] = _data;
		end_pos++;
		allign_poses();
	}

	void clear()
	{
		end_pos = begin_pos = 0;
	}

	size_t size() const
	{
		return end_pos - begin_pos;
	}

	size_t full_size() const 
	{
		return base::size();
	}

	const value_type& get(size_t _pos) const
	{
		VERIFY_EXIT1(allocated() && !empty() && _pos<size(),Fish<_Type>::get());
		return static_cast<const base&>(*this)[(begin_pos+_pos)%full_size()];
	}

	void set(size_t _pos,const value_type& _val)
	{
		VERIFY_EXIT(allocated() && !empty() && _pos<size());
		static_cast<base&>(*this)[(begin_pos+_pos)%full_size()] = _val;
	}

	value_type& get(size_t _pos)
	{
		VERIFY_EXIT1(allocated() && !empty() && _pos<size(),Fish<_Type>::get());
		return static_cast<base&>(*this)[(begin_pos+_pos)%full_size()];
	}

	const value_type& operator [] (size_t _pos) const
	{
		return get(_pos);
	}

	value_type& operator [] (size_t _pos)
	{
		return get(_pos);
	}

	void pop(size_t _items = 1)
	{
		_items = min(_items,size());
		begin_pos+=_items;
		allign_poses();
	}

	void unpush(size_t _items = 1)
	{
		_items = min(_items,size());
		end_pos -= _items;
		allign_poses();
	}

	void pop_back(size_t _items = 1)
	{
		unpush(_items);
	}

	void pop_front(size_t _items = 1)
	{
		pop(_items);
	}

	bool empty() const
	{
		VERIFY_EXIT1(allocated(),false);
		return end_pos==begin_pos;
	}

	bool full() const
	{
		VERIFY_EXIT1(allocated(),true);
		return end_pos>begin_pos && (end_pos%full_size())==begin_pos;
	}

	bool allocated() const
	{
		return full_size()>0;
	}

	iterator begin() 
	{
		return iterator(this,0);
	}

	iterator for_pos(size_t _pos)
	{
		return iterator(this,_pos);
	}

	iterator end()
	{
		return iterator(this,size());
	}

	const_iterator begin() const 
	{
		return const_iterator(this,0);
	}

	const_iterator for_pos(size_t _pos) const
	{
		return const_iterator(this,pos);
	}

	const_iterator end() const
	{
		return const_iterator(this,size());
	}

	const value_type& front() const 
	{
		return get(0);
	}

	value_type& front()
	{
		return get(0);
	}

	const value_type& back() const 
	{
		return get(end_pos-1-begin_pos);
	}

	value_type& back()
	{
		return get(end_pos-1-begin_pos);
	}

protected:
	void allign_poses()
	{
		if(begin_pos>=full_size())
		{
			begin_pos -= full_size();
			end_pos -= full_size();
		}
	}
public:

	static void unit_tests(IUnitTest* _punittest)
	{
		TEST_SUITE(_punittest,_T("CyclicBuffer"),_T("class tests"));

		long i=0,j=0;

		// pretests
		long sz = 5;
		CyclicBuffer<_Type> cb(sz);

		// test 1 
		for(i=0;i<sz*4;i++)
		{
			cb.push(i);
			long k = max(0,(i+1)-sz);
			j=0;
			for(j=0;j<(long)cb.size();j++,k++)
			{
				ok(cb.get(j)==k,_T("failed to push()/get() data"));
			}
		}

		// test 2
		cb.clear();
		long sz1 = (sz*3)/2; 
		for(i=0;i<3;i++)
		{
			for(j=0;j<sz1;j++)
			{
				cb.push(j+i*sz1);
			}
			long k = (i+1)*sz1 - sz;
			for(j=0;j<sz;j++)
			{
				long k1 = k + j;
				long j1 = 0;
				for(j1=0;j1<(long)cb.size();k1++,j1++)
				{
					ok(cb.get(j1)==k1,_T("failed to pop() data"));
				}
				cb.pop();
			}
			ok(cb.empty(),_T("failed to pop() data"));
		}

		// test 3 
		for(i=0;i<sz*4;i++)
		{
			cb.push(i);
			long k = max(0,(i+1)-sz);
			CyclicBuffer<_Type>::const_iterator 
				it = cb.begin()
				,ite = cb.end()
				;
			for(;it!=ite;++it,k++)
			{
				ok(*it==k,_T("failed to push()/get() data"));
			}
		}

		// test 4 
		for(i=0;i<sz*4;i++)
		{
			cb.push(i);
			long k = max(0,(i+1)-sz);
			CyclicBuffer<_Type>::iterator 
				it = cb.begin()
				,ite = cb.end()
				;
			for(;it!=ite;++it,k++)
			{
				ok(*it==k,_T("failed to push()/get() data"));
			}
		}
	}
};//template<> struct CyclicBuffer

