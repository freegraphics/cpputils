#pragma once

namespace cvs
{
	template<typename _ColumnsTypeList>
	struct CVS;

	struct Groupnig
	{
	protected:
		Index index;	// column index to group
		typedef std::vector<size_t> GroupsVec;
		GroupsVec groups;
	public:
		Groupnig()
		{
		}

	public:
		struct Indicies
		{
		protected:
			Index::Indicies indicies;
			IndiciesRange group_range;
		public:
			Indicies(Groupnig& _grouping = Groupnig(),size_t _group_id = 0)
				:indicies(_grouping.index)
			{
				VERIFY_EXIT(_group_id<=_grouping.groups.size());
				indicies = _grouping.index.get_indicies();
				if(_group_id<=_grouping.groups.size())
				{
					size_t from_indice = _group_id==0?0:_grouping.groups[_group_id-1];
					size_t to_indice = _grouping.groups[_group_id];
					group_range = IndiciesRange(from_indice,to_indice,from_indice);
				}
			}

			Indicies& operator = (const Indicies& _indicies)
			{
				indicies = _indicies.indicies;
				group_range = _indicies.group_range;
				return *this;
			}

			Indicies begin() const
			{
				Indicies res(*this);
				res.group_range = res.group_range.begin();
				return res;
			}

			Indicies end() const
			{
				Indicies res(*this);
				res.group_range = res.group_range.end();
				return res;
			}

			bool operator == (const Indicies& _range) const 
			{
				VERIFY_EXIT1(EQL(indicies,_range.indicies),false);
				return EQL(group_range,_range.group_range);
			}

			bool operator < (const Indicies& _range) const 
			{
				VERIFY_EXIT1(EQL(indicies,_range.indicies),false);
				return group_range<_range.group_range;
			}

			Indicies& operator ++ ()
			{
				++group_range;
				return *this;
			}

			Indicies& operator -- ()
			{
				--group_range;
				return *this;
			}

			Indicies& operator += (long _diff)
			{
				group_range += _diff % (get_length() + 1);
				return *this;
			}

			Indicies& operator -= (long _diff)
			{
				group_range -= _diff % (get_length() + 1);
				return *this;
			}

			long operator - (const Indicies& _range) const
			{
				return group_range - _range.group_range;
			}

			size_t get() const 
			{
				return indicies[group_range.get()];
			}

			size_t operator[] (long _idx) const 
			{
				return indicies[group_range[_idx]];
			}

			long get_length() const
			{
				return group_range.get_length();
			}

			size_t size() const 
			{
				return get_length();
			}
		};//struct Indicies

	protected:
		struct IteratorBase
		{
		public:
			typedef Groupnig::Indicies Type;
		protected:
			Groupnig* grouping;
			Groupnig::GroupsVec::iterator it;
		public:
			IteratorBase(Groupnig* _grouping,Groupnig::GroupsVec::iterator _it)
				:grouping(_grouping)
				,it(_it)
			{
			}

			IteratorBase& operator = (const IteratorBase& _iter)	
			{
				grouping = _iter.grouping;
				it = _iter.it;
				return *this;
			}

			bool operator == (const IteratorBase& _iter) const 
			{
				VERIFY_EXIT1(EQL(grouping,_iter.grouping),false);
				return EQL(it,_iter.it);
			}

			bool operator != (const IteratorBase& _iter) const 
			{
				return NEQL(*this,_iter);
			}

			bool operator < (const IteratorBase& _iter) const 
			{
				VERIFY_EXIT1(EQL(grouping,_iter.grouping),false);
				return it < _iter.it;
			}

			bool operator > (const IteratorBase& _iter) const 
			{
				return _iter < *this;
			}

			bool operator <= (const IteratorBase& _iter) const 
			{
				return !(*this > _iter);
			}

			bool operator >= (const IteratorBase& _iter) const 
			{
				return !(*this < _iter);
			}
		};

	public:
		struct iterator : public IteratorBase
		{
		public:
			typedef std::random_access_iterator_tag iterator_category;
			typedef Groupnig::IteratorBase base;
			typedef base::Type Type;
			typedef Type value_type;
			typedef Type reference;
			typedef Type* pointer;
			typedef long difference_type;
			typedef difference_type distance_type;	// retained

		protected:
			iterator(Groupnig* _grouping,Groupnig::GroupsVec::iterator _it)
				:IteratorBase(_grouping,_it)
			{
			}

		public:
			iterator& operator = (const iterator& _iter)
			{
				base::operator=(_iter);
				return *this;
			}

			reference operator * () const 
			{
				VERIFY_EXIT1(NOT_NULL(grouping),Fish<Type>::get());
				return Groupnig::Indicies(*grouping,std::distance(grouping->groups.begin(),it));
			}

			reference operator [] (long _idx_offs) const 
			{
				VERIFY_EXIT1(NOT_NULL(grouping),Fish<Type>::get());
				return Groupnig::Indicies(*grouping,_idx_offs);
			}

			iterator& operator ++ ()
			{
				++it;
				return *this;
			}

			iterator& operator -- ()
			{
				--it;
				return *this;
			}

			iterator& operator += (long _diff)
			{
				it += _diff;
				return *this;
			}

			iterator& operator -= (long _diff)
			{
				it -= _diff;
				return *this;
			}

			iterator operator ++ (int)
			{
				iterator iter(*this);
				++(*this);
				return iter;
			}

			iterator operator -- (int)
			{
				iterator iter(*this);
				--(*this);
				return iter;
			}

			iterator operator + (long _diff) const 
			{
				iterator iter(*this);
				iter.it += _diff;
				return iter;
			}

			iterator operator - (long _diff) const 
			{
				iterator iter(*this);
				iter.it -= _diff;
				return iter;
			}

			difference_type operator - (const iterator& _iter) const 
			{
				return it - _iter.it;
			}

			friend Groupnig;
		};

		typedef iterator const_iterator;

	public:
		// pseudo iterator -- it don`t contain operator ->
		iterator groups_begin()
		{
			return iterator(this,groups.begin());
		}

		// pseudo iterator -- it don`t contain operator ->
		iterator groups_end()
		{
			return iterator(this,groups.end());
		}

		size_t group_size() const 
		{
			return groups.size();
		}

		Indicies get_group_indicies(size_t _group_indice)
		{
			return Indicies(*this,_group_indice);
		}

		template<typename _ColumnsTypeList>
		friend struct CVS;
	};
};//namespace cvs