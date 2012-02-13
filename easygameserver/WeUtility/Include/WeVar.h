/********************************************************************
	created:	2006-8-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	通用变量
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeObject.h"
#include "WeEntityID.h"
#include <string>
#include <vector>
using namespace std;

namespace We
{
	enum VarType
	{
		VarType_Unknown = 0,
		VarType_bool,
		VarType_int,
		VarType_uint,
		VarType_float,
		VarType_string,
		VarType_wstring,
		VarType_EntityID,
		VarType_Object,
		VarType_Pointer,		/// 指针
		VarType_AllocPointer,	/// 分配了内存的指针
		VarType_Count,
	};

	class We_EXPORTS Var
	{
	public:
		/// 构造函数
		Var() : m_Type(VarType_Unknown), _string(0), m_DataSize(0) {}
		Var( VarType type ) : m_Type(type), _string(0), m_DataSize(0) { if( m_Type == VarType_EntityID ) Set_EntityID( Null_EntityID ); }
		Var( bool value ) : m_Type(VarType_bool), m_DataSize(sizeof(bool))
		{
			Set_bool( value );
		}
		Var( int value ) : m_Type(VarType_Unknown), m_DataSize(sizeof(int))
		{
			Set_int( value );
		}
		Var( unsigned int value ) : m_Type(VarType_Unknown), m_DataSize(sizeof(uint32))
		{
			Set_uint( value );
		}
		Var( float value ) : m_Type(VarType_Unknown), m_DataSize(sizeof(float))
		{
			Set_float( value );
		}
		Var( const char* value ) : m_Type(VarType_Unknown), _string(0), m_DataSize(0)
		{
			Set_string( value );
		}
		Var( const string& value ) : m_Type(VarType_Unknown), _string(0), m_DataSize(0)
		{
			Set_string( value );
		}
		Var( const wchar_t* value ) : m_Type(VarType_Unknown), _wstring(0), m_DataSize(0)
		{
			Set_wstring( value );
		}
		Var( const wstring& value ) : m_Type(VarType_Unknown), _wstring(0), m_DataSize(0)
		{
			Set_wstring( value );
		}
		Var( const EntityID& value ) : m_Type(VarType_Unknown)
		{
			Set_EntityID( value );
		}
		Var( WeObject* value ) : m_Type(VarType_Object), m_DataSize(0)
		{
			Set_Object( value );
		}

		/// 拷贝
		Var( const Var& src );

		/// 析构
		~Var()
		{
			Clear();
		}

		/// 重载=
		Var& operator = ( const Var& src );

		bool operator == ( const Var& other ) const;

		bool operator != ( const Var& other ) const;

		/// 清除数据
		void Clear();

		/// 拷贝
		void CopyFrom( const Var& src );

		/// 返回值类型
		uint8 Type() const
		{
			return m_Type;
		}
		uint16 GetDataSize() const
		{
			return m_DataSize;
		}

		/// 取值
		bool Get_bool() const
		{
			//assert( m_Type == VarType_bool );
			if( m_Type != VarType_bool )
				return false;
			return _bool;
		}
		int Get_int() const
		{
			//assert( m_Type == VarType_int );
			if( m_Type != VarType_int && m_Type != VarType_uint )
				return 0;
			return _int;
		}
		unsigned int Get_uint() const
		{
			//assert( m_Type == VarType_int );
			if( m_Type != VarType_int && m_Type != VarType_uint )
				return 0;
			return _uint;
		}
		float Get_float() const
		{
			//assert( m_Type == VarType_float );
			if( m_Type != VarType_float )
				return 0.0f;
			return _float;
		}
		const char* Get_string() const
		{
			if( m_Type == VarType_Unknown )
				return "";
			//assert( m_Type == VarType_string && _string != 0 );
			if( m_Type != VarType_string || _string == 0 )
				return "";
			return _string;
		}
		const wchar_t* Get_wstring() const
		{
			if( m_Type == VarType_Unknown )
				return L"";
			//assert( m_Type == VarType_wstring && _wstring != 0 );
			if( m_Type != VarType_wstring || _wstring == 0 )
				return L"";
			return _wstring;
		}
		EntityID Get_EntityID() const
		{
			if( m_Type == VarType_Unknown )
				return Null_EntityID;
			//Assert( m_Type == VarType_EntityID && _string != 0 );
			if( m_Type != VarType_EntityID || _EntityID == 0 )
				return Null_EntityID;
			return (EntityID)(*_EntityID);
		}
		WeObject* Get_Object() const
		{
			if( m_Type != VarType_Object )
				return 0;
			return _Object;
		}
		void* Get_Pointer() const
		{
			if( m_Type != VarType_Pointer && m_Type != VarType_AllocPointer )
				return 0;
			return m_Pointer;
		}

		/// 赋值
		void Set_bool( bool value )
		{
			if( m_Type == VarType_Unknown )
				m_Type = VarType_bool;
			//assert( m_Type == VarType_bool );
			if( m_Type != VarType_bool )
				return;
			_bool = value;
		}
		void Set_int( int value )
		{
			if( m_Type == VarType_Unknown )
				m_Type = VarType_int;
			//assert( m_Type == VarType_int );
			if( m_Type != VarType_int && m_Type != VarType_uint )
				return;
			m_Type = VarType_int;
			_int = value;
		}
		void Set_uint( unsigned int value )
		{
			if( m_Type == VarType_Unknown )
				m_Type = VarType_uint;
			//assert( m_Type == VarType_int );
			if( m_Type != VarType_int && m_Type != VarType_uint )
				return;
			m_Type = VarType_uint;
			_uint = value;
		}
		void Set_float( float value )
		{
			if( m_Type == VarType_Unknown )
				m_Type = VarType_float;
			//assert( m_Type == VarType_float );
			if( m_Type != VarType_float )
				return;
			_float = value;
		}
		void Set_string( const char* value );
		void Set_string( const string& value )
		{
			Set_string( value.c_str() );
		}
		void Set_wstring( const wchar_t* value );
		void Set_wstring( const wstring& value )
		{
			Set_wstring( value.c_str() );
		}
		void Set_EntityID( const EntityID& value );
		void Set_Object( WeObject* value )
		{
			if( m_Type == VarType_Unknown )
				m_Type = VarType_Object;
			//assert( m_Type == VarType_Object );
			if( m_Type != VarType_Object )
				return;
			_Object = value;
		}
		void Set_Pointer( void* pointer, uint16 size );

		/// 强制转换成string
		/// 注: unsigned int 时,需要自己处理 
		string ForceToString() const;

		/// 强制转换成wstring
		/// 注: unsigned int 时,需要自己处理 
		wstring ForceToWstring() const;

		void ForceFromString( const string& str );

	protected:
		uint8				m_Type;
		uint16				m_DataSize;
		union
		{
			bool			_bool;
			char			_char;
			unsigned char	_uchar;
			//short			_short;
			int				_int;
			unsigned int	_uint;
			float			_float;
			char*			_string;
			wchar_t*		_wstring;
			EntityID*		_EntityID;
			WeObject*		_Object;
			void*			m_Pointer;
			/// __int64
		};

	};

	const Var Null_Var;

	class We_EXPORTS VarList
	{
	public:
		VarList();
		~VarList();

		int Size() const;
		void Clear();

		void Init( int size );
		void Resize( int size );

		uint8 Type( int index ) const;

		bool Get_bool( int index ) const;
		int Get_int( int index ) const;
		unsigned int Get_uint( int index ) const;
		float Get_float( int index ) const;
		const char* Get_string( int index ) const;
		const wchar_t* Get_wstring( int index ) const;
		EntityID Get_EntityID( int index ) const;
		WeObject* Get_Object( int index ) const;
		const Var& GetValue( int index ) const;

		bool Set_bool( int index, bool value );
		bool Set_int( int index, int value );
		bool Set_uint( int index, unsigned int value );
		bool Set_float( int index, float value );
		bool Set_string( int index, const char* value );
		bool Set_string( int index, const string& value );
		bool Set_wstring( int index, const wchar_t* value );
		bool Set_wstring( int index, const wstring& value );
		bool Set_EntityID( int index, const EntityID& value );
		bool Set_Object( int index, WeObject* value );
		bool SetValue( int index, const Var& value );

		void Add_bool( bool value );
		void Add_int( int value );
		void Add_uint( unsigned int value );
		void Add_float( float value );
		void Add_string( const char* value );
		void Add_string( const string& value );
		void Add_wstring( const wchar_t* value );
		void Add_wstring( const wstring& value );
		void Add_EntityID( const EntityID& value );
		void Add_Object( WeObject* value );
		void AddValue( const Var& value );

	protected:
		vector<Var> m_VarList;	/// 是否改用数组? 数组有什么优势?
	};

}
