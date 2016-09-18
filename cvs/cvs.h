#pragma once

#include <utils/utils.h>
#include <utils/typelist.h>
#include <utils/convert.h>
#include <utils/strconv.h>
#include <utils/cyclicbuffer.h>
#include <utils/trace.h>

#include "factor.h"
#include "range.h"
#include "selecteditems.h"
#include "index.h"
#include "grouping.h"

template<typename _Iterator>
struct less_by_iterator 
	: public std::binary_function<size_t,size_t,bool>
{
	_Iterator& it;
	less_by_iterator(_Iterator& _it)
		:it(_it)
	{
	}

	bool operator() (size_t _left,size_t _right) const 
	{
		return it[_left]<it[_right];
	}
};

namespace cvs
{
	namespace details
	{
		template<typename _TL>
		struct PackTLHelper
		{
			typedef typename _TL::Item Item;
			typedef typename _TL::Tail Tail;

			static size_t get_columns_count()
			{
				return TypeListSize<_TL>::get;
			}

			static size_t get_col_type_size(size_t _col)
			{
				if(EQL(_col,0)) return sizeof(Item);
				else return PackTLHelper<Tail>::get_col_type_size(_col-1);
			}

			static size_t get_all_columns_size()
			{
				return sizeof(Item) + PackTLHelper<Tail>::get_all_columns_size();
			}

			static size_t get_offs_of_column(size_t _col)
			{
				if(EQL(_col,0)) return 0;
				else return sizeof(Item) + PackTLHelper<Tail>::get_offs_of_column(_col-1);
			}

			static void set_string_value(std::vector<byte>& _vec,size_t _offs,std::vector<bool>& _na,size_t _idx,size_t _col,const CString& _data)
			{
				if(EQL(_col,0)) 
				{
					Item* val = reinterpret_cast<Item*>(&_vec[_offs]);
					if(_data.IsEmpty()) 
					{
						_na[_idx] = true;
						*val = Item();
						return;
					}
					else common::convert(_data,*val);
				}
				else
				{
					PackTLHelper<Tail>::set_string_value(_vec,_offs+sizeof(Item),_na,_idx+1,_col-1,_data);
				}
			}

			static void get_string_value(const std::vector<byte>& _vec,size_t _offs,const std::vector<bool>& _na,size_t _idx,size_t _col,CString& _data)
			{
				if(EQL(_col,0)) 
				{
					if(_na[_idx])
					{
						_data.Empty();
						return;
					}
					const Item* val = reinterpret_cast<const Item*>(&_vec[_offs]);
					common::convert(*val,_data);
					return;
				}
				else
				{
					PackTLHelper<Tail>::get_string_value(_vec,_offs+sizeof(Item),_na,_idx+1,_col-1,_data);
				}
			}

			static void construct_items(std::vector<byte>& _vec,size_t _offs)
			{
				Item* val = reinterpret_cast<Item*>(&_vec[_offs]);
				new (val) Item;
				PackTLHelper<Tail>::construct_items(_vec,_offs+sizeof(Item));
			}

			static void construct_items(std::vector<byte>& _vec,const std::vector<byte>& _src,size_t _offs)
			{
				Item* val = reinterpret_cast<Item*>(&_vec[_offs]);
				const Item* src_val = reinterpret_cast<const Item*>(&_src[_offs]);
				new (val) Item(*src_val);
				PackTLHelper<Tail>::construct_items(_vec,_src,_offs+sizeof(Item));
			}

			static void destruct_items(std::vector<byte>& _vec,size_t _offs)
			{
				Item* val = reinterpret_cast<Item*>(&_vec[_offs]);
				val->~Item();
				PackTLHelper<Tail>::destruct_items(_vec,_offs+sizeof(Item));
			}

			static void copy_items(const std::vector<byte>& _src,std::vector<byte>& _dest,size_t _offs)
			{
				const Item* val_src = reinterpret_cast<const Item*>(&_src[_offs]);
				Item* val_dest = reinterpret_cast<Item*>(&_dest[_offs]);
				*val_dest = Item(*val_src);
				PackTLHelper<Tail>::copy_items(_src,_dest,_offs+sizeof(Item));
			}
		};
		template<>
		struct PackTLHelper<NullType>
		{
			static size_t get_col_type_size(size_t _col) {return 0;}
			static size_t get_all_columns_size() {return 0;}
			static size_t get_offs_of_column(size_t _col) {return 0;}
			static void set_string_value(std::vector<byte>& _vec,size_t _offs,std::vector<bool>& _na,size_t _idx,size_t _col,const CString& _data) {}
			static void get_string_value(const std::vector<byte>& _vec,size_t _offs,const std::vector<bool>& _na,size_t _idx,size_t _col,CString& _data) {}
			static void construct_items(std::vector<byte>& _vec,size_t _offs) {}
			static void construct_items(std::vector<byte>& _vec,const std::vector<byte>& _src,size_t _offs) {}
			static void destruct_items(std::vector<byte>& _vec,size_t _offs) {}
			static void copy_items(const std::vector<byte>& _src,std::vector<byte>& _dest,size_t _offs) {}
		};
	};


	template<typename _ColumnsTypeList>
	struct CVS
	{
	public:
		typedef _ColumnsTypeList ColumnsTypeList;
	protected:
		size_t row_size;
		size_t max_col;
		size_t max_row;
		size_t inc_alloc_rows;
		size_t allocated_rows;

		std::vector<byte> data;
		std::vector<bool> na;

	protected:
		template<size_t _column,typename _Indicies>
		struct IteratorBase
		{
		public:
			typedef typename typename TypeAt<_ColumnsTypeList,_column>::res Type; 
			typedef Type value_type;

		protected:
			_Indicies indicies;

		public:
			IteratorBase(const _Indicies& _indicies = _Indicies())
				:indicies(_indicies)
			{
			}
	
			IteratorBase& operator = (const IteratorBase& _iter)	
			{
				indicies = _iter.indicies;
				return *this;
			}

			bool operator == (const IteratorBase<_column,_Indicies>& _iter) const 
			{
				return EQL(indicies,_iter.indicies);
			}
			
			bool operator != (const IteratorBase<_column,_Indicies>& _iter) const 
			{
				return NEQL(*this,_iter);
			}

			bool operator < (const IteratorBase<_column,_Indicies>& _iter) const 
			{
				return indicies < _iter.indicies;
			}

			bool operator > (const IteratorBase<_column,_Indicies>& _iter) const 
			{
				return _iter < *this;
			}

			bool operator <= (const IteratorBase<_column,_Indicies>& _iter) const 
			{
				return !(*this > _iter);
			}

			bool operator >= (const IteratorBase<_column,_Indicies>& _iter) const 
			{
				return !(*this < _iter);
			}

		};//template<> struct IteratorBase

	public:
		template<size_t _column,typename _Indicies>
		struct const_iterator;

		template<size_t _column,typename _Indicies>
		struct iterator : public CVS<_ColumnsTypeList>::IteratorBase<_column,_Indicies>
		{
			typedef std::random_access_iterator_tag iterator_category;
			typedef typename CVS<_ColumnsTypeList>::IteratorBase<_column,_Indicies> base;
			typedef typename base::Type Type;
			typedef Type* pointer;
			typedef Type& reference;
			typedef long difference_type;
			typedef difference_type distance_type;	// retained

		protected:
			CVS<_ColumnsTypeList>* data;

		protected:
			iterator(CVS<_ColumnsTypeList>* _data,const _Indicies& _indicies)
				:IteratorBase(_indicies)
				,data(_data)
			{
			}

		public:
			iterator(const _Indicies& _indicies /*= _Indicies()*/)
				:IteratorBase(_indicies)
				,data(NULL)
			{
			}

			iterator& operator = (const iterator<_column,_Indicies>& _iter)
			{
				data = _iter.data;
				base::operator=(_iter);
				return *this;
			}

			iterator& operator = (bool _available)
			{
				VERIFY_EXIT1(NOT_NULL(data),*this);
				data->set_na<_column>(indicies.get(),!_available);
				return *this;
			}

			reference operator * () const 
			{
				VERIFY_EXIT1(NOT_NULL(data),Fish<Type>::get());
				return data->get_value<_column>(indicies.get());
			}

			pointer operator -> () const
			{
				VERIFY_EXIT1(NOT_NULL(data),&(Fish<Type>::get()));
				return &(data->get_value<_column>(indicies.get()));
			}

			reference operator [] (long _idx_offs) const 
			{
				VERIFY_EXIT1(NOT_NULL(data),Fish<Type>::get());
				return data->get_value<_column>(indicies[_idx_offs]);
			}

			iterator& operator ++ ()
			{
				++indicies;
				return *this;
			}

			iterator& operator -- ()
			{
				--indicies;
				return *this;
			}

			iterator& operator += (long _diff)
			{
				indicies += _diff;
				return *this;
			}

			iterator& operator -= (long _diff)
			{
				indicies -= _diff;
				return *this;
			}

			iterator operator ++ (int)
			{
				iterator<_column,_Indicies> iter(*this);
				++(*this);
				return iter;
			}

			iterator operator -- (int)
			{
				iterator<_column,_Indicies> iter(*this);
				--(*this);
				return iter;
			}

			iterator operator + (long _diff) const 
			{
				iterator<_column,_Indicies> iter(*this);
				iter.indicies += _diff;
				return iter;
			}

			iterator operator - (long _diff) const 
			{
				iterator<_column,_Indicies> iter(*this);
				iter.indicies -= _diff;
				return iter;
			}

			difference_type operator - (const iterator<_column,_Indicies>& _iter) const 
			{
				return indicies - _iter.indicies;
			}

			difference_type get_indice() const
			{
				return indicies.get();
			}

			template<typename _TL>
			friend struct CVS;
			template<size_t _column,typename _Indicies>
			friend struct const_iterator;
		};//template<> struct iterator

		template<size_t _column,typename _Indicies>
		struct const_iterator : public CVS<_ColumnsTypeList>::IteratorBase<_column,_Indicies>
		{
			typedef std::random_access_iterator_tag iterator_category;
			typedef typename CVS<_ColumnsTypeList>::IteratorBase<_column,_Indicies> base;
			typedef typename base::Type Type;
			typedef const Type* pointer;
			typedef const Type& reference;
			typedef long difference_type;
			typedef difference_type distance_type;	// retained

		protected:
			const CVS<_ColumnsTypeList>* data;

		protected:
			const_iterator(const CVS<_ColumnsTypeList>* _data,const _Indicies& _indicies)
				:IteratorBase(_indicies)
				,data(_data)
			{
			}

		public:
			const_iterator()
				:data(NULL)
			{
			}

			const_iterator(const iterator<_column,_Indicies>& _iter)
				:IteratorBase(_iter.indicies)
				,data(_iter.data)
			{
			}

			const_iterator& operator = (const const_iterator<_column,_Indicies>& _iter)
			{
				VERIFY_EXIT1(EQL(data,_iter.data),*this);
				base::operator=(_iter);
				return *this;
			}

			reference operator * () const 
			{
				VERIFY_EXIT1(NOT_NULL(data),Fish<Type>::get());
				return data->get_value<_column>(indicies.get());
			}

			pointer operator -> () const
			{
				VERIFY_EXIT1(NOT_NULL(data),&(Fish<Type>::get()));
				return &(data->get_value<_column>(indicies.get()));
			}

			reference operator [] (long _idx_offs) const 
			{
				VERIFY_EXIT1(NOT_NULL(data),Fish<Type>::get());
				return data->get_value<_column>(indicies[_idx_offs]);
			}

			const_iterator& operator ++ ()
			{
				++indicies;
				return *this;
			}

			const_iterator& operator -- ()
			{
				--indicies;
				return *this;
			}

			const_iterator& operator += (long _diff)
			{
				indicies += _diff;
				return *this;
			}

			const_iterator& operator -= (long _diff)
			{
				indicies -= _diff;
				return *this;
			}

			const_iterator operator ++ (int)
			{
				const_iterator<_column,_Indicies> iter(*this);
				++(*this);
				return iter;
			}

			const_iterator operator -- (int)
			{
				const_iterator<_column,_Indicies> iter(*this);
				--(*this);
				return iter;
			}

			const_iterator operator + (long _diff) const 
			{
				const_iterator<_column,_Indicies> iter(*this);
				iter.indicies += _diff;
				return iter;
			}

			const_iterator operator - (long _diff) const 
			{
				const_iterator<_column,_Indicies> iter(*this);
				iter.indicies -= _diff;
				return iter;
			}

			difference_type operator - (const const_iterator<_column,_Indicies>& _iter) const 
			{
				return indicies - _iter.indicies;
			}

			difference_type get_indice() const
			{
				return indicies.get();
			}

			template<typename _TL>
			friend struct CVS;
		};//template<> struct const_iterator

	protected:
		template<typename _ColumnsTypeList,size_t _column>
		struct IndexSortPred: public std::binary_function<size_t,size_t,bool>
		{
		protected:
			typedef typename TypeAt<_ColumnsTypeList,_column>::res column_type;
		protected:
			Index& index;
			const CVS<_ColumnsTypeList>& data;

		public:
			IndexSortPred(Index& _index,const CVS<_ColumnsTypeList>& _data)
				:index(_index)
				,data(_data)
			{
				index.selected.generate_indicies();
			}

			bool operator() (size_t _left,size_t _right) const 
			{
				return get_value(_left)<get_value(_right);
			}

		protected:
			column_type get_value(size_t _idx) const
			{
				size_t row = index.selected.indicies[_idx];
				return data.get_value<_column>(row);
			}
		};

	public:

		CVS()
			:row_size(0)
			,max_col(0)
			,max_row(0)
			,inc_alloc_rows(1024*16)
			,allocated_rows(0)
		{
			row_size = details::PackTLHelper<_ColumnsTypeList>::get_all_columns_size();
			max_col = details::PackTLHelper<_ColumnsTypeList>::get_columns_count();
		}

		~CVS()
		{
			free_rows(allocated_rows);
		}

		size_t get_cols() const {return max_col;}
		size_t get_rows() const {return max_row;}


		template<size_t _column,typename _Indicies>
		iterator<_column,_Indicies> begin(const _Indicies& _indicies)
		{
			return iterator<_column,_Indicies>(this,_indicies.begin());
		}

		template<size_t _column,typename _Indicies>
		iterator<_column,_Indicies> end(const _Indicies& _indicies)
		{
			return iterator<_column,_Indicies>(this,_indicies.end());
		}

		template<size_t _column,typename _Indicies>
		const_iterator<_column,_Indicies> begin(const _Indicies& _indicies) const
		{
			return const_iterator<_column,_Indicies>(this,_indicies.begin());
		}

		template<size_t _column,typename _Indicies>
		const_iterator<_column,_Indicies> end(const _Indicies& _indicies) const
		{
			return const_iterator<_column,_Indicies>(this,_indicies.end());
		}

		template<size_t _column>
		const typename TypeAt<_ColumnsTypeList,_column>::res& 
			get_value(size_t _row) const 
		{
			typedef typename typename TypeAt<_ColumnsTypeList,_column>::res column_type; 
			if(_row>=max_row || _column>=max_col) return Fish<column_type>::get();
			size_t offs = 
				_row*row_size
				+ details::PackTLHelper<_ColumnsTypeList>::get_offs_of_column(_column)
				;
			VERIFY_EXIT1(
				offs
				+details::PackTLHelper<_ColumnsTypeList>::get_col_type_size(_column)<=data.size()
				,Fish<column_type>::get()
				);
			return *reinterpret_cast<const column_type*>(&data[offs]);
		}

		template<size_t _column>
		typename TypeAt<_ColumnsTypeList,_column>::res& 
			get_value(size_t _row) 
		{
			typedef typename TypeAt<_ColumnsTypeList,_column>::res column_type; 
			if(_row>=max_row || _column>=max_col) return Fish<column_type>::get();
			size_t offs = 
				_row*row_size
				+ details::PackTLHelper<_ColumnsTypeList>::get_offs_of_column(_column)
				;
			VERIFY_EXIT1(
				offs
				+details::PackTLHelper<_ColumnsTypeList>::get_col_type_size(_column)<=data.size()
				,Fish<column_type>::get()
				);
			return *reinterpret_cast<column_type*>(&data[offs]);
		}

		template<size_t _column>
		void set_value(size_t _row,const typename TypeAt<_ColumnsTypeList,_column>::res& _value) 
		{
			typedef typename TypeAt<_ColumnsTypeList,_column>::res column_type; 
			if(_row>=max_row || _column>=max_col) return;
			size_t offs = _row*row_size
				+ details::PackTLHelper<_ColumnsTypeList>::get_offs_of_column(_column)
				;
			VERIFY_EXIT(
				offs+details::PackTLHelper<_ColumnsTypeList>::get_col_type_size(_column)
				<=data.size()
				);
			*reinterpret_cast<column_type*>(&data[offs]) = _value;
			offs = _row*max_col+_column;
			VERIFY_EXIT(offs<na.size());
			na[offs] = false;
		}

		template<size_t _column>
		void set_na(size_t _row,bool _na)
		{
			size_t offs = _row*max_col+_column;
			VERIFY_EXIT(offs<na.size());
			na[offs] = _na;
		}

		template<typename _ColumnsTL>
		void get_values(size_t _row,std::vector<real>& _vec,long& _idx,_ColumnsTL) const
		{
			typedef typename _ColumnsTL::Item Item;
			typedef typename _ColumnsTL::Tail Tail;
			VERIFY_EXIT(_idx<(long)_vec.size());
			_vec[_idx++] = get_value<Type2Int<Item>::res>(_row);
			get_values(_row,_vec,_idx,Tail());
		}

		void get_values(size_t _row,std::vector<real>& _vec,long& _idx,NullType) const
		{
		}

		template<size_t _column>
		bool is_na(size_t _row) const
		{
			if(_row>=max_row || _column>=max_col) return true;
			size_t offs = _row*max_col+_column;
			VERIFY_EXIT1(offs<na.size(),true);
			return na[offs];
		}

		void get_all(OUT IndiciesRange& _range)
		{
			_range = IndiciesRange(0,max_row,0);
		}

		void get_range(size_t _min_row,size_t _max_row,OUT IndiciesRange& _range)
		{
			_range = IndiciesRange(_min_row,_max_row,_min_row);
		}

		void select_all(SelectedItems& _si) const
		{
			_si.clear_indicies();
			_si.items.clear();
			_si.items.resize(max_row,true);
		}

		template<size_t _column>
		void select_na(SelectedItems& _si) const
		{
			_si.clear_indicies();
			_si.items.clear();
			_si.items.resize(max_row,false);
			size_t i = 0;
			for(i=0;i<max_row;i++) 
			{
				_si.items[i] = is_na<_column>(i);
			}
		}

		template<size_t _column>
		void select_available(SelectedItems& _si) const 
		{
			_si.clear_indicies();
			_si.items.clear();
			_si.items.resize(max_row,false);
			size_t i = 0;
			for(i=0;i<max_row;i++) 
			{
				_si.items[i] = !is_na<_column>(i);
			}
		}

		template<size_t _column,typename _Filter>
		bool filter(
			_Filter& _filter
			,OUT SelectedItems& _si
			,bool _skip_na = true
			,const typename TypeAt<_ColumnsTypeList,_column>::res& _defvalue 
				= typename TypeAt<_ColumnsTypeList,_column>::res()
			)
		{
			_si.clear_indicies();
			_si.items.clear();
			_si.items.resize(max_row,false);
			size_t i = 0;
			size_t cnt = 0;
			if(_skip_na)
			{
				for(i=0;i<max_row;i++) 
				{
					_si.items[i] = !is_na<_column>(i);
					if(_si.items[i]) _si.items[i] = _filter(get_value<_column>(i));
					if(_si.items[i]) cnt++;
				}
			}
			else
			{
				bool def_ok = _filter(_defvalue);
				for(i=0;i<max_row;i++)
				{
					_si.items[i] = is_na<_column>(i)?def_ok:_filter(get_value<_column>(i));
					if(_si.items[i]) cnt++;
				}
			}
			return NEQL(cnt,0);
		}

		template<size_t _column,typename _Filter>
		bool filter(
			SelectedItems& _si_base
			,_Filter& _filter
			,SelectedItems& _si
			,bool _skip_na = true
			,const typename TypeAt<_ColumnsTypeList,_column>::res& _defvalue 
				= (typename TypeAt<_ColumnsTypeList,_column>::res)()
			)
		{
			_si_base.generate_indicies();
			_si.clear_indicies();
			_si.items.clear();
			_si.items.resize(max_row,false);
			size_t i = 0;
			size_t cnt = 0;
			if(_skip_na)
			{
				SelectedItems::IndiciesVec::const_iterator
					it = _si_base.indicies.begin()
					,ite = _si_base.indicies.end()
					;
				for(;it!=*ite;++it) 
				{
					_si.items[*it] = !is_na<_column>(*it);
					if(_si.items[*it]) _si.items[*it] = _filter(get_value<_column>(*it));
					if(_si.items[*it]) cnt++;
				}
			}
			else
			{
				bool def_ok = _filter(_defvalue);
				SelectedItems::IndiciesVec::const_iterator
					it = _si_base.indicies.begin()
					,ite = _si_base.indicies.end()
					;
				for(;it!=*ite;++it) 
				{
					_si.items[*it] = is_na<_column>(*it)?def_ok:_filter(get_value<_column>(*it));
					if(_si.items[*it]) cnt++;
				}
			}
			return NEQL(cnt,0);
		}

		template<size_t _column>
		void get_index(
			IN const SelectedItems& _si
			,OUT Index& _index
			) const
		{
			_index.selected = _si;
			_index.selected.generate_indicies();
			_index.indicies.resize(_index.selected.indicies.size());
			size_t i = 0;
			Index::IndiciesVec::iterator
				it = _index.indicies.begin()
				,ite = _index.indicies.end()
				;
			for(i=0;it!=ite;++it,i++) *it = i;
			//for(i=0;i<index.indicies.size();i++) index.indicies[i] = i;
			std::sort(
				_index.indicies.begin()
				,_index.indicies.end()
				,IndexSortPred<_ColumnsTypeList,_column>(_index,*this)
				);
			return;
		}

		template <size_t _column>
		void get_grouping(
			IN const SelectedItems& _si
			,OUT Groupnig& _grouping
			) const
		{
			typedef typename TypeAt<_ColumnsTypeList,_column>::res column_type;

			get_index<_column>(IN _si,OUT _grouping.index);
			const_iterator<_column,Index::Indicies>
				it = begin<_column,Index::Indicies>(_grouping.index.get_indicies())
				,ite = end<_column,Index::Indicies>(_grouping.index.get_indicies())
				;
			_grouping.groups.clear();
			size_t i = 0;
			for(i=0;it!=ite;)
			{
				column_type value = *it;
				for(;it!=ite && value==*it;++it,i++){}
				_grouping.groups.push_back(i);
			}
		}

		template<size_t _column>
		bool find(
			IN const typename TypeAt<_ColumnsTypeList,_column>::res& _value
			,IN Index& _index
			,OUT iterator<_column,Index::Indicies>& _lower_bound
			,OUT iterator<_column,Index::Indicies>& _upper_bound
			)
		{
			iterator<_column,Index::Indicies> 
				it = begin<_column>(_index.get_indicies())
				,ite = end<_column>(_index.get_indicies())
				;
			_lower_bound = std::lower_bound(it,ite,_value);
			_upper_bound = _lower_bound;//std::upper_bound(it,ite,_value);
			for(;_upper_bound!=ite && *_upper_bound==_value;++_upper_bound){}
			return NEQL(_lower_bound,_upper_bound);
		}

		template<size_t _column>
		bool find(
			IN const typename TypeAt<_ColumnsTypeList,_column>::res& _value
			,IN Index& _index
			)
		{
			iterator<_column,Index::Indicies> lower_bound;
			iterator<_column,Index::Indicies> upper_bound;
			return find<_column>(_value,_index,lower_bound,upper_bound);
		}

		template<size_t _column,typename _Iterator>
		bool select_by_values(
			IN Index& _index
			,IN _Iterator _it,IN const _Iterator& _ite
			,OUT SelectedItems& _si
			)	
		{
			typedef _Iterator::value_type  value_type;

			_si.clear_indicies();
			_si.items.clear();
			_si.items.resize(max_row,false);

			std::vector<size_t> input;
			size_t i = 0;
			size_t cnt = std::distance(_it,_ite);
			if(EQL(cnt,0)) return false;

			input.resize(cnt);
			for(i=0;i<cnt;i++) input[i] = i;
			std::sort(input.begin(),input.end(),less_by_iterator<_Iterator>(_it));
			value_type val = _it[input[0]];
			const_iterator<_column,Index::Indicies> 
				it = begin<_column,Index::Indicies>(_index.get_indicies())
				,ite = end<_column,Index::Indicies>(_index.get_indicies())
				,it_lower = std::lower_bound(it,ite,val)
				;

			//std::vector<size_t> indicies;
			//indicies.reserve(max_row);
			bool any_selected = false;

			for(i=0;it_lower!=ite && i<input.size();)
			{
				for(;it_lower!=ite && *it_lower==val;++it_lower)
				{
					//VERIFY_DO(*it_lower<_si.items.size(),continue);
					_si.items[it_lower.get_indice()] = true;
					//indicies.push_back(*it_lower);
					any_selected = true;
				}
				++i;
				if(i<input.size())
				{
					val = _it[input[i]];
					if(*it_lower!=val)
						it_lower = std::lower_bound(it_lower,ite,val);
				}
			}
			//std::sort(indicies.begin(),indicies.end());
			//std::vector<size_t>::const_iterator
			//	it1 = indicies.begin()
			//	,it1e = indicies.end()
			//	;
			//for(;it1!=it1e;++it1)
			//{
			//	_si.items[*it1] = true;
			//}
			//return NEQL(indicies.size(),0);
			return any_selected;
		}

		bool load_from_file(
			const CString& _file_name
			,bool _use_quotes = true
			,TCHAR _quote = _T('\"')
			,const CString& _column_delim = _T(";")
			,const CString& _new_line_delim = _T("\n")
			,UINT _codepage = CP_UTF8
			,const CString& _fmt = CString()
			,ITraceOutput* _output = NULL
			)
		{
			FileTokenizer lines(_file_name,_new_line_delim);
			FileTokenizer::PositionPairType linebe;
			size_t rowi = 0;
			for(;lines.next(linebe);rowi++)
			{
				if(rowi>=allocated_rows)
					allocate_rows(inc_alloc_rows);
				LPCTSTR lb = lines.begin(linebe);
				LPCTSTR le = lines.end(linebe);
				string_converter<CHAR,WCHAR> linew(lb,le-lb,_codepage);
				string_converter<WCHAR,TCHAR> linet((LPWSTR)linew,linew.get_length(),CP_ACP);
				CString line((LPCTSTR)linet,linet.get_length());
				set_line(rowi,line,_use_quotes,_quote,_column_delim);
				if(NOT_NULL(_output)) _output->process(_fmt,rowi);
			}
			lines.close();
			max_row = rowi;
			return max_row>0;
		}

		bool save_to_file(
			const CString& _file_name
			,bool _use_quotes = true
			,TCHAR _quote = _T('\"')
			,const CString& _column_delim = _T(";")
			,const CString& _new_line_delim = _T("\n")
			,const CString& _fmt = CString()
			,ITraceOutput* _output = NULL
			)
		{
			CFile file;
			file.Open(_file_name,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite|CFile::typeBinary);
			if(EQL(max_row,0))
			{
				file.Close();
				return false;
			}
			size_t i = 0;
			for(i=0;i<max_row;i++)
			{
				CString line;
				size_t j = 0;
				for(j=0;j<max_col;j++)
				{
					CString str;
					details::PackTLHelper<_ColumnsTypeList>::get_string_value(data,i*row_size,na,i*max_col,j,str);
					if(_use_quotes)
					{
						line += _quote;
						line += replace_all(str,CString(_quote),CString(_quote)+CString(_quote));
						line += _quote;
					}
					else
						line += str;
					if(j+1<max_col)
						line += _column_delim;
				}
				line += _new_line_delim;
				file.Write((LPCTSTR)line,line.GetLength()*sizeof(TCHAR));
				if(NOT_NULL(_output)) _output->process(_fmt,i);
			}
			file.Close();
			return true;
		}

		bool save_to_file_selected(
			SelectedItems& _si
			,const CString& _file_name
			,bool _use_quotes = true
			,TCHAR _quote = _T('\"')
			,const CString& _column_delim = _T(";")
			,const CString& _new_line_delim = _T("\n")
			)
		{
			CFile file;
			file.Open(_file_name,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite|CFile::typeBinary);
			_si.generate_indicies();
			//if(EQL(max_row,0))
			//{
			//	file.Close();
			//	return false;
			//}
			SelectedItems::Indicies indicies = _si.get_indicies();
			size_t i = 0;
			for(i=0;i<indicies.size();i++)
			{
				size_t row = indicies[i];
				CString line;
				size_t j = 0;
				for(j=0;j<max_col;j++)
				{
					CString str;
					details::PackTLHelper<_ColumnsTypeList>::get_string_value(data,row*row_size,na,row*max_col,j,str);
					if(_use_quotes)
					{
						line += _quote;
						line += replace_all(str,CString(_quote),CString(_quote)+CString(_quote));
						line += _quote;
					}
					else
						line += str;
					if(j+1<max_col)
						line += _column_delim;
				}
				line += _new_line_delim;
				file.Write((LPCTSTR)line,line.GetLength()*sizeof(TCHAR));
			}
			file.Close();
			return true;
		}

		void resize(size_t _rows)
		{
			if(_rows>allocated_rows) allocate_rows(_rows-allocated_rows);
			else if(_rows<allocated_rows) free_rows(allocated_rows-_rows);
		}

	protected:
		void free_rows(size_t _rows)
		{
			VERIFY_DO(_rows<=allocated_rows,_rows=allocated_rows);
			std::vector<byte> new_data;
			new_data.resize(data.size()-_rows*row_size);
			na.resize(na.size()-_rows*max_col,false);
			size_t new_allocated_rows = allocated_rows-_rows;
			size_t i = 0;
			for(i=0;i<new_allocated_rows;i++) {details::PackTLHelper<_ColumnsTypeList>::construct_items(new_data,data,i*row_size);}
			for(;i<allocated_rows;i++) {details::PackTLHelper<_ColumnsTypeList>::destruct_items(data,i*row_size);}
			data.swap(new_data);
			allocated_rows = new_allocated_rows;
		}

		void allocate_rows(size_t _rows_to_allocate)
		{
			std::vector<byte> new_data;
			new_data.resize(data.size()+_rows_to_allocate*row_size);
			size_t new_allocated_rows = allocated_rows+_rows_to_allocate;
			size_t i = 0;
			for(i=0;i<allocated_rows;i++) {details::PackTLHelper<_ColumnsTypeList>::construct_items(new_data,data,i*row_size);}
			for(;i<new_allocated_rows;i++) {details::PackTLHelper<_ColumnsTypeList>::construct_items(new_data,i*row_size);}
			for(i=0;i<allocated_rows;i++) {details::PackTLHelper<_ColumnsTypeList>::destruct_items(data,i*row_size);}
			data.swap(new_data);
			allocated_rows = new_allocated_rows;
			na.resize(na.size()+_rows_to_allocate*max_col,false);
		}

		void set_line(
			size_t _row,const CString& _line
			,bool _use_quotes,TCHAR _quote,const CString& _column_delim
			)
		{
			size_t col = 0;
			size_t pos = 0;
			for(col=0;col<max_col && pos<(size_t)_line.GetLength();col++)
			{
				CString value;
				value.GetBuffer(_line.GetLength());
				value.ReleaseBuffer(0);
				bool quotes = false;
				if(_use_quotes)
				{
					if(_line.GetAt(pos)==_quote) 
					{
						quotes = true;
						pos++;
					}
				}
				for(;pos<(size_t)_line.GetLength();)
				{
					if(quotes && _line.GetAt(pos)==_quote)
					{
						if(pos+1<(size_t)_line.GetLength() && _line.GetAt(pos+1)==_quote)
						{
							value += _quote;
							pos += 2;
							continue;
						}
						pos++;
						break;
					}
					if(_line.Mid(pos,_column_delim.GetLength())==_column_delim)
						break;
					value += _line.GetAt(pos);
					pos++;
				}
				for(;pos<(size_t)_line.GetLength();)
				{
					if(_line.Mid(pos,_column_delim.GetLength())==_column_delim)
					{
						pos += _column_delim.GetLength();
						break;
					}
					pos++;
				}
				details::PackTLHelper<_ColumnsTypeList>::set_string_value(data,_row*row_size,na,_row*max_col,col,value);
			}
		}

		CString replace_all(const CString& _str,const CString& _find,const CString& _replace)
		{
			CString res = _str;
			int start = 0, pos = -1;
			for(;(pos=res.Find(_find,start))>=0;)
			{
				res = res.Mid(0,pos) + _replace + res.Mid(pos);
				start = pos + _replace.GetLength();
			}
			return res;
		}
	};//template<> struct CVS

}//namespace cvs

template<typename _ColumnsTypeList,size_t _column,typename _Indicies>
typename cvs::CVS<_ColumnsTypeList>::IteratorBase<_column,_Indicies> operator + (
	long _diff
	,const typename cvs::CVS<_ColumnsTypeList>::IteratorBase<_column,_Indicies>& _iter
	)
{
	typedef typename cvs::CVS<_ColumnsTypeList>::IteratorBase<_column,_Indicies> Iterator;
	Iterator iter(_iter);
	iter += _diff;
	return iter;
}
