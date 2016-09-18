#if !defined(__MEMSERIALIZER_H__99533A2E_C5B9_43E0_8F6E_AC40C6A816E7_INCLUDED)
#define __MEMSERIALIZER_H__99533A2E_C5B9_43E0_8F6E_AC40C6A816E7_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include "serialize.h"
#include "tracealloc.h"
#include "errors.h"

#include <security/safeint.hpp>


//#define MEMORY_SERIALIZED_TRACE

/**\defgroup MemorySerilize Utilities to support serialization of objects in memory.

This utilities override interfaces \ref INode , \ref INodeList and \ref IArchive to 
create memory storage for a objects serialization. This class usefull to create 
programming pattern Memento (http://sourcemaking.com/design_patterns/memento). 
*/

//@{

/**\brief overide of \ref INode for serialization of objects in memory.
This class presents serialized node item.
*/
struct CMemoryNode : public IBlobNode
{
protected:
	/**\brief inner class to serach for nodes by name*/
	struct Name
	{
		/**\brief constructor
		\param[in] _sName -- name
		*/
		Name(const CString_& _sName):m_sName(_sName){}

		/**brief operator to comapre \ref CMemoryNode::Name with \ref CMemoryNode
		\param _node -- node to compare
		\return true if names are the same. 
		*/
		bool operator==(const CMemoryNode& _node) const
		{
			VERIFY_EXIT1(NOT_NULL(&_node),false);
			return !m_sName.Compare(_node.get_name());
		}

		/**\brief operator to compare  \ref CMemoryNode::Name with \ref CMemoryNode pointer
		\param _pnode -- pointer to node to compare.
		\return true if names are the same. 
		*/
		bool operator () (const CMemoryNode* _pnode) const 
		{
			VERIFY_EXIT1(NOT_NULL(_pnode),false);
			return !m_sName.Compare(_pnode->get_name());
		}

		CString_ m_sName;	///< node name to search
	};//struct Name

	friend struct Name;
	typedef std::list<CPtrShared<CMemoryNode> > NodesLst;	///< typedef of list of nodes.

public:

	typedef CString_ name_type;
	typedef CString_ value_type;

	/**\brief constructor.
	\param[in] _sName -- node name
	\param[in] _sValue -- node value (sa a string)
	*/
	CMemoryNode(const CString_& _sName = _T(""),const CString_& _sValue = _T(""))
		:m_sName(_sName),m_sValue(_sValue)
	{
	}

	/**\brief Destructor.
	Clear inner data, free memory.
	*/
	virtual ~CMemoryNode() 
	{
		NodesLst::iterator 
			it = m_childrenlst.begin()
			,ite = m_childrenlst.end()
			;
		for(;it!=ite;++it) *it = NULL;
	};

	/**\brief return node name.
	This fucntion overide \ref INode::get_name() const
	\return node name.
	\sa INode::get_name() const
	*/
	virtual CString_ get_name() const {return m_sName;}

	/**\brief return node value.
	This fucntion override \ref INode::get_value() const
	\return node value (as string)
	\sa INode::get_value() const
	*/
	virtual CString_ get_value() const {return m_sValue;}

	/**\brief sets node name.
	This fucntion override \ref INode::set_name(const CString_& _sName)
	\param _sName -- node name
	\sa INode::set_name(const CString_& _sName)
	*/
	virtual void set_name(const CString_& _sName) {m_sName = _sName;}

	/**\brief set node value
	This fucntion override \ref INode::set_value(const CString_& _sValue)
	\param _sValue -- value of node (sa string)
	\sa INode::set_value(const CString_& _sValue)
	*/
	virtual void set_value(const CString_& _sValue) {m_sValue= _sValue;}

	/**\brief return this node children list
	This function override INode::INodeList* get_childlist() const
	\return \ref INodeList pointer
	\sa INode::INodeList* get_childlist() const
	*/
	virtual INodeList* get_childlist(const CString_& _sName) const;

	/**\brief return children list
	\return children list of this node
	*/
	virtual INodeList* get_allchildlist() const;

	/**\brief return child node with specified name
	This fucntion override INode::get_child(const CString_& _sName) const
	\param _sName -- name of child node to get.
	\return child node pointer (\ref INode)  specified by its name
	\sa INode::get_child(const CString_& _sName) const
	*/
	virtual INode* get_child(const CString_& _sName) const
	{
		NodesLst::const_iterator fit 
			= std::find_if(m_childrenlst.begin(),m_childrenlst.end(),Name(_sName))
			;
		if(fit==m_childrenlst.end()) return NULL;
		return *fit;
	}

	/**\brief add child with specified name.
	This fucntion implements INode::add_child(const CString_& _sName).
	\param _sName -- name of node to add as a child to this node.
	\return return child node pointer (\ref INode).
	\sa INode::add_child(const CString_& _sName)
	*/
	virtual INode* add_child(const CString_& _sName)
	{
		m_childrenlst.push_back(CPtrShared<CMemoryNode>(trace_alloc(new CMemoryNode(_sName))));
		return m_childrenlst.back();
	}

	/**\brief trace \ref CMemoryNode item.
	\param _shift shift from left screen border
	*/
	void trace(long _shift) const
	{
#if defined(MEMORY_SERIALIZED_TRACE)
		TRACE_(_T("%s%s=\"%s\"\n")
			,(LPCTSTR)shift(_shift)
			,(LPCTSTR)m_sName,(LPCTSTR)m_sValue
			);
		NodesLst::const_iterator 
			it = m_childrenlst.begin()
			,ite = m_childrenlst.end()
			;
		for(;it!=ite;++it)
		{
			(*it)->trace(_shift+1);
		}
#endif
	}

	virtual void get_value(CBlob& _blob) const
	{
		_blob = m_blob;
	}

	virtual void set_value(const CBlob& _blob)
	{
		m_blob = _blob;
	}

protected:
	CString_ m_sName;			///< name of node
	CString_ m_sValue;			///< value of node (as a string)
	CBlob m_blob;

	NodesLst m_childrenlst;		///< children list

	friend struct CMemoryNodeList;
};//struct CMemoryNode

/**\brief list of nodes. Implementation of \ref INodeList
This class represents children list for \ref CMemoryNode. \ref CMemoryNode::NodesLst
itself keeps in the CMemoryNode classes, and \ref CMemoryNodeList class is used to
provide for users work with memery list of each \ref CMemoryNode.
*/
struct CMemoryNodeList : public INodeList
{
protected:
	/**\brief Constructor
	\param _lst -- list to create from
	*/
	CMemoryNodeList(
		const CString_& _sName
		,CMemoryNode::NodesLst& _lst
		)
		:m_selectednodes(_lst,_sName)
	{
		//\todo select all nodes of m_sName
	}

	/**\brief Constructor
	\param _lst -- list to create from
	*/
	CMemoryNodeList(
		CMemoryNode::NodesLst& _lst
		)
		:m_selectednodes(_lst,_T(""))
	{
		//\todo select all nodes of m_sName
	}
public:

	/**\brief clear children list.
	This fucntion implements \ref INodeList::clear()
	\sa INodeList::clear()
	*/
	virtual void clear()	{m_selectednodes.clear();}

	/**\brief return size of children list.
	This function implements \ref INodeList::size() const
	\return size of children list.
	\sa INodeList::size() const
	*/
	virtual size_t size() const	{return m_selectednodes.size();}

	/**\brief operator to get node from list by it index
	This fucntion implements \ref INodeList::operator [] (long _index) const
	\param[in] _index -- index to get
	\return \ref INode pointer or NULL if index is not valid
	\sa INodeList::operator [] (long _index) const
	*/
	virtual INode* operator [] (long _index) const
	{
		return const_cast<INode*>(m_selectednodes[_index]);
	}

	/**\brief function to remove item from nodes list
	\param _index -- item index to remove
	\return true if item was successfully removed
	*/
	virtual bool remove(long _index)
	{
		return m_selectednodes.remove(_index);
	}

	/**\brief fucntion that allocates buffer for data.
	This function implements INodeList::alloc(size_t _size)
	\param _size -- items count to allocate
	\sa INodeList::alloc(size_t _size)
	*/
	virtual void alloc(size_t _size)
	{
		m_selectednodes.alloc((long)_size,CMemoryNode());
	}

	/**\brief add node with specified name and value
	This function implements INodeList::add(const CString_& _sName,const CString_& _sValue)
	\param[in] _sValue -- node value to add
	\return \ref INode pointer to newly added node
	*/
	virtual INode* add(const CString_& _sValue)
	{
		return m_selectednodes.add(_sValue,CMemoryNode());
	}

	/**\brief function that add child with specified name and value
	\param _sName -- child name to create
	\param _sValue -- child value to allocate
	\return \ref INode pointer of new child or NULL if failed to create new child node
	*/
	virtual INode* add(const CString_& _sName,const CString_& _sValue)
	{
		return m_selectednodes.add(_sName,_sValue,CMemoryNode());
	}

protected:
	CSelectedNodes<CMemoryNode::NodesLst> m_selectednodes;
	friend struct CMemoryNode;		
};//struct CMemoryNodeList

/**\brief Memory archive. 
This class implements \ref IArchive to support work with archive operations for serialization.
*/
struct CMemoryArchive : public IArchive
{
	/**\brief Constructor.*/
	CMemoryArchive()
	{
		m_root = trace_alloc(new CMemoryNode());
	}

	/**\brrief Destructor.*/
	~CMemoryArchive()
	{
		m_root = NULL;
	}

	/**\brief return root node on memory archive
	\return root item pointer (\ref INode pointer)
	*/
	virtual INode* get_root() const
	{
		return m_root;
	}

	/**\brief starts trace of memory archive
	this fucntion output all archive tree of nodes.
	*/
	void trace() const
	{
		if(!m_root) return;
		m_root->trace(0);
	}
protected:
	CPtrShared<CMemoryNode> m_root;		///< root memory archive item
};//struct CMemoryArchive

inline
INodeList* CMemoryNode::get_childlist(const CString_& _sName) const
{
	return trace_alloc(
		new CMemoryNodeList(
			_sName
			,const_cast<CMemoryNode::NodesLst&>(m_childrenlst)
			)
		);
}

inline
INodeList* CMemoryNode::get_allchildlist() const
{
	return trace_alloc(
			new CMemoryNodeList(
				const_cast<CMemoryNode::NodesLst&>(m_childrenlst) 
			)
		);
}


namespace Memory
{
	#pragma pack(push)
	#pragma pack(1)

	namespace Private
	{
		struct CValuePre
		{
			size_t m_size;
			bool m_bTag;
			bool m_bOpen;

			CValuePre()
				:m_size(0)
				,m_bTag(false)
				,m_bOpen(false)
			{
			}
		};

		struct CValue
		{
			CValuePre pre;
			WCHAR buf[1];

			CValue()
			{
				buf[0] = 0;
			}

			static void write(const CString_& _sValue,bool _bTag,bool _bOpen,size_t& _sz,std::vector<BYTE>& _buf)
			{
				try
				{
					SafeInt<size_t> sz = 
						SafeInt<size_t>(_sValue.GetLength()+1)*sizeof(WCHAR)
						+ sizeof(CValuePre)
						;
					_sz = sz;
				}
				catch(SafeIntException&)
				{
					_sz = 0;
					_buf.resize(0);
					return;
				}
				_buf.resize(_sz);
				CValue* pdata = reinterpret_cast<CValue*>(&_buf[0]);
				pdata->pre.m_size = _sValue.GetLength();
				pdata->pre.m_bTag = _bTag;
				pdata->pre.m_bOpen = _bOpen;
				string_converter<TCHAR,WCHAR> sValueW(_sValue,CP_ACP);
				wcscpy_s((PWCHAR)(pdata->buf),_sValue.GetLength()+1,sValueW);
			}

			static void prepare(const CValuePre& _pre,size_t& _sz,std::vector<BYTE>& _buf)
			{
				try
				{
					SafeInt<size_t> sz =
						SafeInt<size_t>(_pre.m_size+1)*sizeof(WCHAR)
						;
					_sz = sz;
				}
				catch(SafeIntException&)
				{
					_sz = 0;
					_buf.resize(0);
					return;
				}
				_buf.resize(_sz);
			}

			static bool read(const CValuePre& _pre,const std::vector<BYTE>& _buf,CString_& _sValue,bool& _bTag,bool& _bOpen)
			{
				try
				{
					SafeInt<size_t>	sz = 
						SafeInt<size_t>(_pre.m_size+1)*sizeof(WCHAR)
						;
					if(sz!=_buf.size()) return false;
				}
				catch(SafeIntException&)
				{
					return false;
				}
				string_converter<WCHAR,TCHAR> sValueT(reinterpret_cast<const WCHAR*>(&_buf[0]),(long)_pre.m_size,CP_ACP);
				_sValue = (TCHAR*)sValueT;
				_bTag = _pre.m_bTag;
				_bOpen = _pre.m_bOpen;
				return true;
			}
		};//struct CValue
#pragma pack(pop)

		interface IMemoryOps
		{
			virtual bool send(const std::vector<BYTE>& _data) = 0;
			virtual bool recv(std::vector<BYTE>& _buf) = 0;
		};

		struct CMemoryOps
		{
		protected:
			IMemoryOps* m_memryops;
		public:
			CMemoryOps(IMemoryOps* _memoryops)
				:m_memryops(_memoryops)
			{
			}

			void write(const CString_& _sValue,bool _bTag,bool _bOpen)
			{
				VERIFY_EXIT(NOT_NULL(m_memryops));
				size_t sz = 0;
				std::vector<BYTE> buf;
				Private::CValue::write(_sValue,_bTag,_bOpen,sz,buf);
				if(buf.empty() || sz==0)
					throw_error(_T("failed to create buffer"));

				int err = ERROR_SUCCESS;
				m_memryops->send(buf);
			}

			void write_tag(const CString_& _sName,bool _bOpen)
			{
				write(_sName,true,_bOpen);
			}

			void write_value(const CString_& _sValue)
			{
				write(_sValue,false,false);
			}

			void read(CString_& _sValue,bool& _bTag,bool& _bOpen)
			{
				VERIFY_EXIT(NOT_NULL(m_memryops));
				int err = ERROR_SUCCESS;
				std::vector<byte> pre_buf;
				pre_buf.resize(sizeof(Private::CValuePre));
				Private::CValuePre* pre = (Private::CValuePre*)(&pre_buf[0]);

				if(!m_memryops->recv(pre_buf))
					throw_error(_T("failed to get data from memory"));

				size_t sz=0;
				std::vector<byte> buf;
				Private::CValue::prepare(*pre,sz,buf);

				if(buf.empty() || sz==0)
					throw_error(_T("failed to create buffer"));

				if(!m_memryops->recv(buf))
					throw_error(_T("failed to get data from memory"));

				if(!Private::CValue::read(*pre,buf,_sValue,_bTag,_bOpen))
					throw_error(_T("failed to read value"));
			}
		};//struct CMemoryOps

	};//namespace Private


	class CLoader 
		: public Private::IMemoryOps
	{
	protected:
		struct LastRead
		{
			CString_ m_sValue;
			bool m_bTag;
			bool m_bOpen;

			bool isOpenTag() const {return m_bTag && m_bOpen;}
			bool isCloseTag() const {return m_bTag && !m_bOpen;}
			bool isValue() const {return !m_bTag;}
			const CString_& get_name() const {return m_sValue;}
			const CString_& get_value() const {return m_sValue;}
		};

	protected:
		Private::CMemoryOps m_memoryops;
		LastRead m_top;

	protected:
		std::vector<byte>& m_buf;
		size_t m_pos;

		//interface IMemoryOps
		//{
		virtual bool send(const std::vector<BYTE>& _data)
		{
			VERIFY_EXIT1(false,false); // is not used
			return false;
		}

		virtual bool recv(std::vector<BYTE>& _buf)
		{
			if(_buf.size() + m_pos > m_buf.size()) 
			{
				m_pos = m_buf.size();
				return false;
			}
			size_t i=0,j=0;
			for(i=m_pos,j=0;j<_buf.size() && i<m_buf.size();i++,j++) {_buf[j] = m_buf[i];}
			m_pos += _buf.size();
			return true;
		}
		//};

	public:
		CLoader(std::vector<byte>& _buf)
			:m_memoryops(this)
			,m_buf(_buf)
			,m_pos(0)
		{
		}

	/*
	---pseudo code------------

		process()
		{
			while(is_open_tag)
			{
				name = get_top();
				create_node(name);
				next();
				if(! is_value) error();
				set_node_value();
				next();
				process();
				if(!is_close_tag() || get_top()!=name) error();
				next();
			}
		}
	*/

		void load(IArchive& _archive)
		{
			VERIFY_EXIT(NOT_NULL(&_archive) && NOT_NULL(_archive.get_root()));
			INode* pnode = _archive.get_root();
			read();
			utils::CErrorTag __et(
				cnamedvalue(_T("tag"),m_top.m_sValue)
				,cnamedvalue(_T("bTag"),m_top.m_bTag)
				,cnamedvalue(_T("bOpen"),m_top.m_bOpen)
				);

			if(!m_top.isOpenTag()) throw_error(_T("expected open tag"));
			pnode->set_name(m_top.get_name());
			read();

			utils::CErrorTag __et1(
				cnamedvalue(_T("value"),m_top.m_sValue)
				,cnamedvalue(_T("bTag1"),m_top.m_bTag)
				,cnamedvalue(_T("bOpen1"),m_top.m_bOpen)
				);

			if(!m_top.isValue()) throw_error(_T("expected value of tag"));
			pnode->set_value(m_top.get_value());
			read();

			utils::CErrorTag __et2(
				cnamedvalue(_T("next"),m_top.m_sValue)
				,cnamedvalue(_T("bTag2"),m_top.m_bTag)
				,cnamedvalue(_T("bOpen2"),m_top.m_bOpen)
				);

			process(pnode);
			if(!m_top.isCloseTag()
				|| m_top.get_name()!=pnode->get_name()
				) 
				throw_error(_T("expected close tag"));
		}

	protected:
		void read()
		{
			m_memoryops.read(m_top.m_sValue,m_top.m_bTag,m_top.m_bOpen);
		}

		void process(INode* _parent)
		{
			VERIFY_EXIT(NOT_NULL(_parent));
			while(m_top.isOpenTag())
			{
				utils::CErrorTag __et(
					cnamedvalue(_T("tag"),m_top.m_sValue)
					,cnamedvalue(_T("bTag"),m_top.m_bTag)
					,cnamedvalue(_T("bOpen"),m_top.m_bOpen)
					);

				INode* pnode = _parent->add_child(m_top.get_name());
				read();

				utils::CErrorTag __et1(
					cnamedvalue(_T("value"),m_top.m_sValue)
					,cnamedvalue(_T("bTag1"),m_top.m_bTag)
					,cnamedvalue(_T("bOpen1"),m_top.m_bOpen)
					);

				if(!m_top.isValue()) throw_error(_T("expected value of tag"));
				pnode->set_value(m_top.get_value());
				read();

				utils::CErrorTag __et2(
					cnamedvalue(_T("next"),m_top.m_sValue)
					,cnamedvalue(_T("bTag2"),m_top.m_bTag)
					,cnamedvalue(_T("bOpen2"),m_top.m_bOpen)
					);

				process(pnode);
				if(!m_top.isCloseTag()
					|| m_top.get_name()!=pnode->get_name()
					) 
					throw_error(_T("expected close tag"));
				read();
			}
		}
	};//class CLoader 


	class CSaver 
		: public ISerializeVisitor
		, public Private::IMemoryOps
	{
	protected:
		Private::CMemoryOps m_memoryops;

	protected:
		std::vector<byte> m_buf;

		//interface IMemoryOps
		//{
		virtual bool send(const std::vector<BYTE>& _data)
		{
			size_t pos = m_buf.size();
			m_buf.resize(m_buf.size()+_data.size());
			size_t i=0;
			for(i=0;i<_data.size() && pos<m_buf.size();i++,pos++)	{m_buf[pos] = _data[i];}
			return true;
		}

		virtual bool recv(std::vector<BYTE>& _buf)
		{
			VERIFY_EXIT1(false,false);	// is not used
			return false;
		}
		//};

	public:
		CSaver()
			:m_memoryops(this)
		{
		}

	public:
		void save(IArchive& _arch)
		{
			visit(&_arch);
		}

		const std::vector<byte>& get_data() const 
		{
			return m_buf;
		}

		virtual void visit(IArchive* _p)
		{
			VERIFY_EXIT(NOT_NULL(_p) && NOT_NULL(_p->get_root()));
			INode* pnode = _p->get_root();
			if(pnode->get_name().IsEmpty())
				pnode->set_name(_T("root"));
			pnode->visit(*this);
		}

		virtual void visit(INode* _p)
		{
			VERIFY_EXIT(NOT_NULL(_p));
			utils::CErrorTag __et(
				cnamedvalue(_T("name"),_p->get_name())
				,cnamedvalue(_T("value"),_p->get_value())
				);

			m_memoryops.write_tag(_p->get_name(),true);
			m_memoryops.write_value(_p->get_value());

			CPtrShared<INodeList> pnodelist = _p->get_allchildlist();
			pnodelist->visit(*this);

			m_memoryops.write_tag(_p->get_name(),false);
		}

		virtual void visit(INodeList* _p)
		{
			VERIFY_EXIT(NOT_NULL(_p));
			size_t i=0,cnt=_p->size();
			for(i=0;i<cnt;i++)
			{
				CPtrShared<INode> pnode = (*_p)[(long)i];
				VERIFY_DO(NOT_NULL(pnode),continue);
				pnode->visit(*this);
			}
		}
	};
};//namespace Memory



//@}

/**\page Page_QuickStart_MemorySerializer Quick start: "Object serialization to memory".

Thera two main task that propose memory archive: save some object state and restore 
this objects state. For example state will be saved in a memory. 

Because different object is packed to one type of data \ref IArchive pointer, so it 
possible to store different object in a same way (and deal with them in same way).

To complite this task you need.
	-# Create data struct layout. 
	-# Use fucntion save() and load() that save and load data from archive.
	.

Lets see code examples.

[1] Create layout(s)
\code
struct A
{
	CString_ m_str;
	struct DefaultLayout : public Layout<A>
	{
		DefaultLayout()
		{
			add_simple(_T("A"),&A::m_str);
		}
	};
};

typedef std::list<A> ALst;

struct Base
{
	long m_nBaseCode;
	ALst m_list;

	struct DefaultLayout : public Layout<Base>
	{
		DefaultLayout()
		{
			add_simple(_T("BaseCode"),&Base::m_nBaseCode);
			add_list(_T("list"),&Base::m_list,get_structlayout<A>());
		}
	};
};

struct B : public Base
{
	long m_BCode;
	struct DefaultLayout : public Layout<B>
	{
		DefaultLayout()
		{
			add_base(get_structlayout<Base>());
			add_simple(_T("BCode"),&B::m_BCode);
		}
	};
};
\endcode

[2] Use fucntion save() and load() to work with memory archives.

\code
void test()
{
	B b;
	CMemoryArchive arch,arch2;
	save(arch,_T("root"),b,get_structlayout<B>()); // datas was saved to archive;
	Base base;
	save(arch2,_T("root"),base,get_structlayout<Base>()); // datas was save to archive

	B b1;
	load(arch,_T("root"),b1,get_structlayout<B>()); // data from b (arch) was loaded to b1;
	Base base1;
	load(arch2,_T("root"),base2,get_structlayout<Base>());	// data was loaded into base1 
															// from arch2 or base objects.
}
\endcode

\sa save(IArchive& _arch,const CString_& _nameid,const _Type& _item)
\sa save(IArchive& _arch,const CString_& _nameid,const std::vector<_ItemType>& _lst,const IMetaClass<_ItemType>& _itemmetaclass)
\sa save(IArchive& _arch,const CString_& _nameid,const std::list<_ItemType>& _lst,const IMetaClass<_ItemType>& _itemmetaclass)
\sa save(IArchive& _arch,const CString_& _nameid,const _StructType& _struct,const IMetaClass<_StructType>& _itemmetaclass)
\sa load(const IArchive& _arch,const CString_& _nameid,_Type& _item)
\sa load(const IArchive& _arch,const CString_& _nameid,std::vector<_ItemType>& _lst,const IMetaClass<_ItemType>& _itemmetaclass)
\sa load(const IArchive& _arch,const CString_& _nameid,std::list<_ItemType>& _lst,const IMetaClass<_ItemType>& _itemmetaclass)
\sa load(const IArchive& _arch,const CString_& _nameid,_StructType& _struct,const IMetaClass<_StructType>& _itemmetaclass)
\sa get_primitivelayout()
\sa get_structlayout()

*/

#endif//#if !defined(__MEMSERIALIZER_H__99533A2E_C5B9_43E0_8F6E_AC40C6A816E7_INCLUDED)
