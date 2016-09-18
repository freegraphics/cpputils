#pragma once

#include <utils/utils.h>
#include <utils/typelist.h>
#include <utils/convert.h>
#include <utils/strconv.h>


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

	inline 
	void free_factor_class(size_t _factor_class)
	{
		cvs::FactorDescription::get().free_factor_class(_factor_class);
	}
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
