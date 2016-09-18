#pragma once

namespace cvs
{
	template<typename _ColumnsTypeList>
	struct CVS;

	//template<typename _ColumnsTypeList,size_t _column>
	//struct index_sort_pred;

	struct Index
	{
	protected:
		typedef std::vector<size_t> IndiciesVec;
		IndiciesVec indicies;
		SelectedItems selected;

	protected:

	public:
		struct Indicies
		{
		protected:
			const SelectedItems& si; 
			IndiciesVec& indicies;
			IndiciesVec::iterator pos;

		public:
			Indicies(Index& _index)
				:si(_index.selected)
				,indicies(_index.indicies)
			{
				_index.selected.generate_indicies();
				pos = indicies.begin();
			}

			Indicies& operator = (const Indicies& _indicies)
			{
				VERIFY_EXIT1(EQL(si,_indicies.si) && EQL(indicies,_indicies.indicies),*this);
				pos = _indicies.pos;
				return *this;
			}

			Indicies begin() const
			{
				Indicies res(*this);
				res.pos = indicies.begin();
				return res;
			}

			Indicies end() const
			{
				Indicies res(*this);
				res.pos = indicies.end();
				return res;
			}

			bool operator == (const Indicies& _range) const 
			{
				return EQL(pos,_range.pos);
			}

			bool operator < (const Indicies& _range) const 
			{
				return pos<_range.pos;
			}

			Indicies& operator ++ ()
			{
				++pos;
				return *this;
			}

			Indicies& operator -- ()
			{
				--pos;
				return *this;
			}

			Indicies& operator += (long _diff)
			{
				pos += _diff % (get_length() + 1);
				return *this;
			}

			Indicies& operator -= (long _diff)
			{
				pos -= _diff % (get_length() + 1);
				return *this;
			}

			long operator - (const Indicies& _range) const
			{
				return pos - _range.pos;
			}

			size_t get() const 
			{
				return si.indicies[*pos];
			}

			size_t operator[] (long _idx) const 
			{
				return si.indicies[pos[_idx]];
			}

			long get_length() const
			{
				return std::distance(pos,indicies.end());
			}

			size_t size() const 
			{
				return get_length();
			}
		};//struct Indicies

	public:
		Index()
		{
		}

		Indicies get_indicies() 
		{
			return Indicies(*this);
		}

		operator Indicies () 
		{
			return get_indicies();
		}

		template<typename _ColumnsTypeList>
		friend struct CVS;

		//template<typename _ColumnsTypeList,size_t _column>
		//friend struct index_sort_pred;

	};//struct Index
};//namespace cvs