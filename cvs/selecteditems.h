#pragma once

#include <math/utils.h>

namespace cvs
{
	template<typename _ColumnsTypeList>
	struct CVS;

	struct Index;

	struct SelectedItems
	{
	protected:
		typedef std::vector<bool> ItemsVec;
		typedef std::vector<size_t> IndiciesVec;

		ItemsVec items;
		bool is_indicies_exist;
		IndiciesVec indicies;

	public:
		void generate_indicies()
		{
			if(is_indicies_exist) return;
			indicies.clear();
			indicies.reserve(items.size());
			ItemsVec::const_iterator
				it = items.begin()
				,ite = items.end()
				;
			size_t i = 0;
			for(i=0;it!=ite;++it,i++)
			{
				if(*it) indicies.push_back(i);
			}
			is_indicies_exist = true;
		}

		void clear_indicies()
		{
			is_indicies_exist = false;
			indicies.clear();
		}

	public:
	public:
		struct Indicies
		{
		protected:
			SelectedItems::IndiciesVec::iterator it,ite; 
			SelectedItems::IndiciesVec::iterator pos;

		protected:
			Indicies()
			{
			}

		public:
			Indicies(SelectedItems& _si)
			{
				_si.generate_indicies();
				it = _si.indicies.begin();
				ite = _si.indicies.end();
				pos = it;
			}

			Indicies& operator = (const Indicies& _indicies)
			{
				it = _indicies.it;
				ite = _indicies.ite;
				pos = _indicies.pos;
				return *this;
			}

			Indicies begin() const
			{
				Indicies res;
				res.it = it;
				res.ite = ite;
				res.pos = it;
				return res;
			}

			Indicies end() const
			{
				Indicies res;
				res.it = it;
				res.ite = ite;
				res.pos = ite;
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
				return *pos;
			}

			size_t operator[] (long _idx) const 
			{
				return pos[_idx];
			}

			long get_length() const
			{
				return std::distance(it,ite);
			}

			size_t size() const 
			{
				return get_length();
			}
		};//struct Indicies

	public:
		struct SelectedItems()
			:is_indicies_exist(false)
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

		bool operator == (const SelectedItems& _si) const 
		{
			return EQL(items,_si.items);
		}

		bool operator != (const SelectedItems& _si) const 
		{
			return NEQL(*this,_si);
		}

		void not()
		{
			clear_indicies();
			items.flip();
		}

		void or(const SelectedItems& _si)
		{
			clear_indicies();
			VERIFY_EXIT(EQL(items.size(),_si.items.size()));
			ItemsVec::iterator
				it = items.begin()
				,ite = items.end()
				;
			ItemsVec::const_iterator
				it1 = _si.items.begin()
				,it1e = _si.items.end()
				;
			size_t i = 0;
			for(i=0;it!=ite && it1!=it1e;++it,++it1,i++)
			{
				*it = *it | *it1;
			}
		}

		void and(const SelectedItems& _si)
		{
			clear_indicies();
			VERIFY_EXIT(EQL(items.size(),_si.items.size()));
			ItemsVec::iterator
				it = items.begin()
				,ite = items.end()
				;
			ItemsVec::const_iterator
				it1 = _si.items.begin()
				,it1e = _si.items.end()
				;
			size_t i = 0;
			for(i=0;it!=ite && it1!=it1e;++it,++it1,i++)
			{
				*it = *it & *it1;
			}
		}

		void xor(const SelectedItems& _si)
		{
			clear_indicies();
			VERIFY_EXIT(EQL(items.size(),_si.items.size()));
			ItemsVec::iterator
				it = items.begin()
				,ite = items.end()
				;
			ItemsVec::const_iterator
				it1 = _si.items.begin()
				,it1e = _si.items.end()
				;
			size_t i = 0;
			for(i=0;it!=ite && it1!=it1e;++it,++it1,i++)
			{
				*it = *it ^ *it1;
			}
		}

		void rand(real _prob)
		{
			clear_indicies();
			ItemsVec::iterator
				it = items.begin()
				,ite = items.end()
				;
			for(;it!=ite;++it)
			{
				if(!*it) continue;
				*it = (::rand((real)1.0) <= _prob);
			}
		}

		void save(const CString& _file_name)
		{
			std::ofstream file;
			file.open(_file_name);
			generate_indicies();
			size_t i = 0;
			for(i=0;i<indicies.size();i++)
			{
				file << indicies[i] << _T("\n");
			}
		}

		template<typename _ColumnsTypeList>
		friend struct CVS;

		friend struct Index;
	};
};//namespace cvs