#pragma once

namespace cvs
{
	struct NullRange
	{
		NullRange begin() const
		{
			return *this;
		}

		NullRange end() const
		{
			return *this;
		}

		bool operator == (const NullRange& _range) const 
		{
			return true;
		}

		bool operator < (const NullRange& _range) const 
		{
			return false;
		}

		NullRange& operator ++ ()
		{
			return *this;
		}

		NullRange& operator -- ()
		{
			return *this;
		}

		NullRange& operator += (long _diff)
		{
			return *this;
		}

		NullRange& operator -= (long _diff)
		{
			return *this;
		}

		long operator - (const NullRange& _r) const
		{
			return 0;
		}

		size_t get() const 
		{
			return 0;
		}

		size_t operator[] (long _idx) const 
		{
			return 0;
		}

		long get_length() const 
		{
			return 0;
		}

		size_t size() const 
		{
			return get_length();
		}
	};//struct NullRange

	struct IndiciesRange
	{
	protected:
		long m_start;
		long m_end;
		long pos;

	public:
		IndiciesRange(long _start = 0,long _end = 0,long _pos = 0)
			:m_start(_start),m_end(_end)
			,pos(_pos)
		{
			m_start = min_value(_start,_end);
			m_end = max_value(_start,_end);
		}

		IndiciesRange& operator = (const IndiciesRange& _range)
		{
			//VERIFY_EXIT1(EQL(m_start,_range.m_start) 
			//	&& EQL(m_end,_range.m_end)
			//	,*this
			//	);
			m_start = _range.m_start;
			m_end = _range.m_end;
			pos = _range.pos;
			return *this;
		}

		IndiciesRange begin() const
		{
			return IndiciesRange(m_start,m_end,m_start);
		}

		IndiciesRange end() const
		{
			return IndiciesRange(m_start,m_end,m_end);
		}

		bool operator == (const IndiciesRange& _range) const 
		{
			return same_range(_range)
				&& EQL(pos,_range.pos)
				;
		}

		bool operator < (const IndiciesRange& _range) const 
		{
			if(!same_range(_range)) return false;
			return pos<_range.pos;
		}

		IndiciesRange& operator ++ ()
		{
			++pos;
			arange();
			return *this;
		}

		IndiciesRange& operator -- ()
		{
			--pos;
			arange();
			return *this;
		}

		IndiciesRange& operator += (long _diff)
		{
			pos += _diff % (get_length() + 1);
			arange();
			return *this;
		}

		IndiciesRange& operator -= (long _diff)
		{
			pos -= _diff % (get_length() + 1);
			arange();
			return *this;
		}

		long operator - (const IndiciesRange& _range) const
		{
			return pos - _range.pos;
		}

		size_t get() const 
		{
			return pos;
		}

		size_t operator[] (long _idx) const 
		{
			return pos + _idx;
		}

		long get_length() const 
		{
			return m_end - m_start;
		}

		size_t size() const 
		{
			return get_length();
		}

	protected:
		bool same_range(const IndiciesRange& _range) const 
		{
			return EQL(m_start,_range.m_start)
				&& EQL(m_end,_range.m_end)
				;
		}

		void arange()
		{
			pos = ((pos - m_start) % (get_length() + 1)) + m_start;
		}
	};//struct IndicesRange
};//namespace cvs