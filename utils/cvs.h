#pragma once

#include "utils.h"
#include "typelist.h"
#include "convert.h"
#include "strconv.h"

template<typename _Type>
struct always_true : std::unary_function<_Type,bool>
{
	bool operator() (const _Type& _data) const 
	{
		return true;
	}
};

namespace cvs
{
	//////////////////////////////////////////////////////////////////////////
	// factor classes

	static const size_t UnknownFactorID = -1;

	template<size_t _FactorClassID>
	struct Factor;

	namespace details
	{
		struct FactorClass
		{
		protected:
			typedef std::map<CString,size_t> FactorNameToIDMap;
			typedef std::map<size_t,CString> FactorIDToNameMap;

		protected:
			FactorNameToIDMap name_to_id;
			FactorIDToNameMap id_to_name;

		public:
			FactorClass()
			{
			}

			bool is_exist(size_t _id) const 
			{
				return NEQL(id_to_name.find(_id),id_to_name.end());
			}

			bool is_exist(const CString& _name) const 
			{
				return NEQL(name_to_id.find(_name),name_to_id.end());
			}

			size_t get_id(const CString& _name) const 
			{
				VERIFY_EXIT1(is_exist(_name),UnknownFactorID);
				return name_to_id.find(_name)->second;
			}

			CString get_name(size_t _id) const 
			{
				VERIFY_EXIT1(is_exist(_id),CString());
				return id_to_name.find(_id)->second;
			}

			size_t add_name(const CString& _name)
			{
				VERIFY_EXIT1(!is_exist(_name),get_id(_name));
				size_t id = name_to_id.size();
				name_to_id.insert(FactorNameToIDMap::value_type(_name,id));
				id_to_name.insert(FactorIDToNameMap::value_type(id,_name));
				return id;
			}
		};// struct FactorClass
	};//namespace details

	struct FactorDescription
	{
	protected:
		typedef std::map<size_t,details::FactorClass> FactorClassesMap;

	protected:
		FactorClassesMap factor_classes;

	protected:	
		FactorDescription()
		{
		}

		static FactorDescription& get_instance()
		{
			static FactorDescription _;
			return _;
		}

		details::FactorClass& get_factor_class(size_t _factor_class_id)
		{
			FactorClassesMap::iterator fit = factor_classes.find(_factor_class_id);
			if(EQL(fit,factor_classes.end()))
			{
				FactorClassesMap::_Pairib insp = factor_classes.insert(FactorClassesMap::value_type(_factor_class_id,details::FactorClass()));
				return insp.first->second;
			}
			return fit->second;
		}

	public:
		static FactorDescription& get()
		{
			return get_instance();
		}

		bool is_factor_exist(size_t _factor_class_id,size_t _factor_id) 
		{
			return get_factor_class(_factor_class_id).is_exist(_factor_id);
		}

		bool is_factor_exist(size_t _factor_class_id,const CString& _factor_name)
		{
			return get_factor_class(_factor_class_id).is_exist(_factor_name);
		}

		CString get_factor_name(size_t _factor_class_id,size_t _factor_id)
		{
			return get_factor_class(_factor_class_id).get_name(_factor_id);
		}

		size_t get_factor_id(size_t _factor_class_id,const CString& _factor_name)
		{
			if(get_factor_class(_factor_class_id).is_exist(_factor_name))
				return get_factor_class(_factor_class_id).get_id(_factor_name);
			return get_factor_class(_factor_class_id).add_name(_factor_name);
		}

		void free_factor_class(size_t _factor_class_id)
		{
			FactorClassesMap::iterator fit = factor_classes.find(_factor_class_id);
			VERIFY_EXIT(NEQL(fit,factor_classes.end()));
			factor_classes.erase(fit);
		}
	};//struct FactorDescription

	template<size_t _FactorClassID>
	struct Factor
	{
	protected:
		size_t value;
	public:
		Factor()
			:value(UnknownFactorID)
		{
		}

		operator size_t () const 
		{
			return value;
		}

		Factor& operator = (size_t _factor_id)
		{
			VERIFY_EXIT1(FactorDescription::get().is_factor_exist(_FactorClassID,_factor_id),*this);
			value = _factor_id;
			return *this;
		}

		CString get_name() const 
		{
			VERIFY_EXIT1(NEQL(value,UnknownFactorID),CString());
			return FactorDescription::get().get_factor_name(_FactorClassID,value);
		}

		Factor& operator = (const CString& _name)
		{
			value = FactorDescription::get().get_factor_id(_FactorClassID,_name);
			return *this;
		}

		bool is_na() const 
		{
			return EQL(value,UnknownFactorID);
		}
	};//template<> Factor

};//namespace cvs

namespace common
{
	template<size_t _FactorClassID> inline 
		void convert(const cvs::Factor<_FactorClassID>& _factor,CString& _str)
	{
		_str = _factor.get_name();
	}

	template<size_t _FactorClassID> inline 
		void convert(const CString& _str,cvs::Factor<_FactorClassID>& _factor)
	{
		_factor = _str;
	}
}

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
	protected:
		size_t row_size;
		size_t max_col;
		size_t max_row;
		size_t inc_alloc_rows;
		size_t allocated_rows;

		std::vector<byte> data;
		std::vector<bool> na;
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

		template<typename _Type>
		const _Type& get_value(size_t _row,size_t _col) const 
		{
			if(_row>=max_row || _col>=max_col) return Fish<_Type>::get();
			size_t offs = _row*row_size+details::PackTLHelper<_ColumnsTypeList>::get_offs_of_column(_col);
			VERIFY_EXIT1(offs+details::PackTLHelper<_ColumnsTypeList>::get_col_type_size(_col)<=data.size(),Fish<_Type>::get());
			return *reinterpret_cast<const _Type*>(&data[offs]);
		}

		template<typename _Type>
		void set_value(size_t _row,size_t _col,const _Type& _value) 
		{
			if(_row>=max_row || _col>=max_col) return;
			size_t offs = _row*row_size+details::PackTLHelper<_ColumnsTypeList>::get_offs_of_column(_col);
			VERIFY_EXIT(offs+details::PackTLHelper<_ColumnsTypeList>::get_col_type_size(_col)<=data.size());
			*reinterpret_cast<_Type*>(&data[offs]) = _value;
			offs = _row*max_col+_col;
			VERIFY_EXIT(offs<na.size());
			na[offs] = false;
		}

		template<typename _Type,typename _Func>
		void modify_value(size_t _row,size_t _col,_Func& _func)
		{
			if(_row>=max_row || _col>=max_col) return Fish<_Type>::get();
			size_t offs = _row*row_size+details::PackTLHelper<_ColumnsTypeList>::get_offs_of_column(_col);
			VERIFY_EXIT(offs+details::PackTLHelper<_ColumnsTypeList>::get_col_type_size(_col)<=data.size());
			_Type* val = reinterpret_cast<_Type*>(&data[offs]);
			*val = _func(*val);
		}

		bool is_na(size_t _row,size_t _col) const
		{
			if(_row>=max_row || _col>=max_col) return true;
			size_t offs = _row*max_col+_col;
			VERIFY_EXIT1(offs<na.size(),true);
			return na[offs];
		}

		template<typename _Type,typename _Filter>
		bool get_slice(
			size_t _col
			,const _Filter& _filter
			,std::vector<_Type>& _slice
			,bool _skip_na = true,const _Type& _defvalue = _Type()
			) const
		{
			std::vector<bool> filtered;
			filtered.resize(max_row);
			size_t i = 0;
			if(_skip_na)
			{
				for(i=0;i<max_row;i++) 
				{
					filtered[i] = is_na(i,_col);
					if(!filtered[i]) filtered[i] = !_filter(get_value<_Type>(i,_col));
				}
			}
			else
			{
				bool def_ok = !_filter(_defvalue);
				for(i=0;i<max_row;i++)
				{
					filtered[i] = is_na(i,_col)?def_ok:!_filter(get_value<_Type>(i,_col));
				}
			}
			size_t cnt = std::count(filtered.begin(),filtered.end(),false);
			if(EQL(cnt,0)) return false;
			_slice.resize(cnt);
			size_t j = 0;
			for(i=0;i<max_row;i++)
			{
				if(filtered[i]) continue;
				_slice[j++] = get_value<_Type>(i,_col);
			}
			return true;
		}

		template<typename _Type,typename _Func>
		_Func for_each_in_column(size_t _col,const _Func& _func,_Type type_val = _Type()) const 
		{
			_Func func = _func;
			size_t i = 0;
			for(i=0;i<max_row;i++)
			{
				if(!is_na(i,_col))
				{
					func(get_value<_Type>(i,_col));
				}
			}
			return func;
		}

		template<typename _Type,typename _Func>
		_Func modify_in_column(size_t _col,const _Func& _func,_Type type_val = _Type())
		{
			_Func func = _func;
			size_t i = 0;
			for(i=0;i<max_row;i++)
			{
				if(!is_na(i,_col))
				{
					modify_value<_Type>(i,_col,func);
				}
			}
			return func;
		}

		template<typename _Type>
		void set_na_values(size_t _col,const _Type& _value)
		{
			size_t i = 0;
			for(i=0;i<max_row;i++)
			{
				if(is_na(i,_col))
					set_value(i,_col,_value);
			}
		}

		bool load_from_file(
			const CString& _file_name
			,bool _use_quotes = true
			,TCHAR _quote = _T('\"')
			,const CString& _column_delim = _T(";")
			,const CString& _new_line_delim = _T("\n")
			,UINT _codepage = CP_UTF8
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
						line += str;
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

		void set_line(size_t _row,const CString& _line,bool _use_quotes,TCHAR _quote,const CString& _column_delim)
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
	};//template<> struct CVS

};//namespace cvs

/*
enum FcatorsEn{
	factor_sex=1
};

typedef TYPELIST_2(size_t,CString) ActorsTL;
typedef TYPELIST_3(size_t,size_t,cvs::Factor<factor_sex>) UsersTL;

void demo_test()
{
	// test loading and savings 

	CVS<ActorsTL> arctors_cvs;
	DWORD t1 = GetCurrentTime();
	arctors_cvs.load_from_file(_T("d:\\private\\projects\\RecommenderSystem\\data\\kinopark\\actors.csv"));
	DWORD t2 = GetCurrentTime();
	std::tcout << _T("data was loaded in ") << t2-t1 << _T(" ms\n");
	//arctors_cvs.load_from_file(_T("d:\\private\\projects\\tests\\cvs\\actors.txt"));
	t1 = GetCurrentTime();
	std::vector<size_t> ids;
	arctors_cvs.get_slice(0,always_true<size_t>(),ids);
	t2 = GetCurrentTime();
	std::tcout << _T("get all ids in ") << t2-t1 << _T(" ms\n");
	t1 = GetCurrentTime();
	std::vector<CString> names;
	arctors_cvs.get_slice(1,always_true<CString>(),names);
	t2 = GetCurrentTime();
	std::tcout << _T("get all names in ") << t2-t1 << _T(" ms\n");
	t1 = GetCurrentTime();
	arctors_cvs.save_to_file(_T("actors_ascii_001.csv"));
	t2 = GetCurrentTime();
	std::tcout << _T("data was saved in ") << t2-t1 << _T(" ms\n");

	// test factors

	cvs::CVS<UsersTL> users_cvs;
	t1 = GetCurrentTime();
	users_cvs.load_from_file(_T("d:\\private\\projects\\RecommenderSystem\\data\\kinopark\\users.csv"));
	t2 = GetCurrentTime();
	std::tcout << _T("data was loaded in ") << t2-t1 << _T(" ms\n");
	std::vector<cvs::Factor<factor_sex> > sex;
	t1 = GetCurrentTime();
	users_cvs.get_slice(2,always_true<cvs::Factor<factor_sex> >(),sex);
	t2 = GetCurrentTime();
	std::tcout << _T("get all sex in ") << t2-t1 << _T(" ms\n");
}

void free()
{
	// free factor description (after all work have been done)

	cvs::FactorDescription::get().free_factor_class(factor_sex);
}
*/

