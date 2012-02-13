#include "WeVar.h"
#include "WeType.h"
#include "WeStringUtil.h"
#include <assert.h>

namespace We
{
	//------------------------------------------------------------------------
	//const Var Var::Null_Var;
	//------------------------------------------------------------------------
	Var::Var(const Var& src )
	{
		if( this == &src )
			return;
		_string = 0;
		m_Type = src.m_Type;
		m_DataSize = src.m_DataSize;
		if( m_Type == VarType_bool )
			_bool = src._bool;
		else if( m_Type == VarType_int )
			_int = src._int;
		else if( m_Type == VarType_uint )
			_uint = src._uint;
		else if( m_Type == VarType_float )
			_float = src._float;
		else if( m_Type == VarType_string )
			Set_string( src.Get_string() );
		else if( m_Type == VarType_wstring )
			Set_wstring( src.Get_wstring() );
		else if( m_Type == VarType_EntityID )
			Set_EntityID( src.Get_EntityID() );
		else if( m_Type == VarType_Object )
			_Object = src._Object;
		else if( m_Type == VarType_Pointer )
			m_Pointer = src.m_Pointer;
		else if( m_Type == VarType_AllocPointer )
			Set_Pointer( src.m_Pointer, src.m_DataSize );
	}
	//------------------------------------------------------------------------
	Var& Var::operator = (const Var& src )
	{
		if( this == &src )
			return *this;
		CopyFrom( src );
		return *this;
	}
	//------------------------------------------------------------------------
	bool Var::operator == ( const Var& other ) const
	{
		switch( m_Type )
		{
		case VarType_Unknown:
			return other.m_Type == VarType_Unknown;
			break;
		case VarType_bool:
			if( m_Type != other.m_Type )
				return false;
			return _bool == other._bool;
			break;
		case VarType_int:
		case VarType_uint:
			return _int == other._int;
			break;
		case VarType_float:
			if( m_Type != other.m_Type )
				return false;
			return _float == other._float;
			break;
		case VarType_string:
			if( m_Type != other.m_Type )
				return false;
			return ::strcmp( Get_string(), other.Get_string() ) == 0;
			break;
		case VarType_wstring:
			if( m_Type != other.m_Type )
				return false;
			return ::wcscmp( Get_wstring(), other.Get_wstring() ) == 0;
			break;
		case VarType_EntityID:
			if( m_Type != other.m_Type )
				return false;
			if( other._EntityID == NULL )
				return _EntityID == NULL;
			if( _EntityID == NULL )
				return false;
			return *_EntityID == *(other._EntityID);
			break;
		case VarType_Object:
			if( m_Type != other.m_Type )
				return false;
			return _Object == other._Object;
			break;
		case VarType_Pointer:
			if( m_Type != other.m_Type )
				return false;
			return m_Pointer == other.m_Pointer;
			break;
		case VarType_AllocPointer:
			if( m_Type != other.m_Type )
				return false;
			return (m_DataSize == other.m_DataSize && ::memcmp( m_Pointer, other.m_Pointer, m_DataSize ) == 0 );
			break;
		}
		return false;
	}
	//------------------------------------------------------------------------
	bool Var::operator !=( const Var& other ) const
	{
		return !( *this == other );
	}
	//------------------------------------------------------------------------
	void Var::CopyFrom(const Var& src )
	{
		Clear();
		m_Type = src.m_Type;
		if( m_Type == VarType_bool )
			_bool = src._bool;
		else if( m_Type == VarType_int )
			_int = src._int;
		else if( m_Type == VarType_uint )
			_uint = src._uint;
		else if( m_Type == VarType_float )
			_float = src._float;
		else if( m_Type == VarType_string )
			Set_string( src.Get_string() );
		else if( m_Type == VarType_wstring )
			Set_wstring( src.Get_wstring() );
		else if( m_Type == VarType_EntityID )
			Set_EntityID( src.Get_EntityID() );
		else if( m_Type == VarType_Object )
			_Object = src._Object;
		else if( m_Type == VarType_Pointer )
			m_Pointer = src.m_Pointer;
		else if( m_Type == VarType_AllocPointer )
			Set_Pointer( src.m_Pointer, src.m_DataSize );
	}
	//------------------------------------------------------------------------
	void Var::Clear()
	{
		if( m_Type == VarType_string && _string != 0 )
		{
			delete []_string;
			_string = 0;
		}
		else if( m_Type == VarType_wstring && _wstring != 0 )
		{
			delete []_wstring;
			_wstring = 0;
		}
		else if( m_Type == VarType_EntityID && _EntityID != 0 )
		{
			delete _EntityID;
			_EntityID = 0;
		}
		else if( m_Type == VarType_AllocPointer )
		{
			delete []m_Pointer;
			m_Pointer = 0;
		}
		m_Type = VarType_Unknown;
		_string = 0;
		m_DataSize = 0;
	}
	//------------------------------------------------------------------------
	void Var::Set_string(const char* value )
	{
		//assert( value != 0 );
		if( value == 0 )
			return;
		if( m_Type == VarType_string && _string != 0 )
		{
			size_t old_len = ::strlen( _string );
			size_t new_len = ::strlen( value );
			if( new_len <= old_len )	//要不要重新分配内存呢???
			{
				strcpy_s( _string, old_len+1, value );
				return;
			}
		}
		else if( m_Type == VarType_Unknown )
			m_Type = VarType_string;
		//assert( m_Type == VarType_string );
		if( m_Type != VarType_string )
			return;
		if( _string != 0 )
		{
			delete []_string;
			_string = 0;
			m_DataSize = 0;
		}
		size_t len = ::strlen(value);
		assert( len < 65535 );
		_string = new char[len+1];
		m_DataSize = len+1;
		if( _string == 0 )
		{
			//_LogError( "Var::Set_string new char[len+1] == 0" );
			m_Type = VarType_Unknown;
			return;
		}
		strcpy_s( _string, len+1, value );
	}
	//------------------------------------------------------------------------
	void Var::Set_wstring(const wchar_t* value )
	{
		//assert( value != 0 );
		if( value == 0 )
			return;
		if( m_Type == VarType_wstring && _wstring != 0 )
		{
			size_t old_len = ::wcslen( _wstring );
			size_t new_len = ::wcslen( value );
			if( new_len <= old_len )
			{
				::wcscpy_s( _wstring, old_len+1, value );
				return;
			}
		}
		else if( m_Type == VarType_Unknown )
			m_Type = VarType_wstring;
		//assert( m_Type == VarType_wstring );
		if( m_Type != VarType_wstring )
			return;
		if( _wstring != 0 )
		{
			delete []_wstring;
			_wstring = 0;
			m_DataSize = 0;
		}
		size_t len = ::wcslen(value);
		assert( len < 65535/2 );
		_wstring = new wchar_t[len+1];
		m_DataSize = (len+1)*sizeof(wchar_t);
		if( _wstring == 0 )
		{
			//_LogError( "Var::Set_string new wchar_t[len+1] == 0" );
			m_Type = VarType_Unknown;
			return;
		}
		::wcscpy_s( _wstring, len+1, value );
	}
	void Var::Set_EntityID( const EntityID& value )
	{
		if( m_Type == VarType_EntityID && _EntityID == 0 )
		{
			_EntityID = new EntityID();
		}
		if( m_Type == VarType_Unknown )
		{
			_EntityID = new EntityID();
			m_Type = VarType_EntityID;
		}
		//Assert( m_Type == VarType_EntityID );
		if( m_Type != VarType_EntityID )
			return;
		*_EntityID = value;
	}
	void Var::Set_Pointer( void* pointer, uint16 size )
	{
		if( pointer == 0 || size == 0 )
			return;
		if( m_Type == VarType_AllocPointer && m_Pointer != 0 )
		{
			delete []m_Pointer;
			m_Pointer = 0;
			m_DataSize = 0;
		}
		else if( m_Type == VarType_Unknown )
			m_Type = VarType_AllocPointer;
		if( m_Type != VarType_AllocPointer )
			return;
		m_Pointer = new unsigned char[size];
		::memcpy( m_Pointer, pointer, size );
		m_DataSize = size;
	}
	//------------------------------------------------------------------------
	string Var::ForceToString() const
	{
		string ret;
		switch( m_Type )
		{
		case VarType_Unknown:
			ret = "";
			break;
		case VarType_bool:
			ret = ( _bool?"true":"false" );
			break;
		case VarType_int:
			ret = Type::ToString( _int );
			break;
		case VarType_uint:
			ret = Type::ToString( _uint );
			break;
		case VarType_float:
			ret = Type::ToString( _float );
			break;
		case VarType_string:
			ret = Get_string();
			break;
		case VarType_wstring:
			if( _wstring )
				ret = Type::ToString( (const wchar_t*)_wstring );
			break;
		case VarType_EntityID:
			if( _EntityID )
				ret = Type::ToString( _EntityID->WholeIndex() )  + "." + Type::ToString( _EntityID->Serial() );
			break;
		case VarType_Object:
			return ret;
			break;
		}
		return ret;
	}
	//------------------------------------------------------------------------
	wstring Var::ForceToWstring() const
	{
		wstring ret;
		switch( m_Type )
		{
		case VarType_Unknown:
			ret = L"";
			break;
		case VarType_bool:
			ret = ( _bool?L"true":L"false" );
			break;
		case VarType_int:
			ret = Type::ToWstring( _int );
			break;
		case VarType_uint:
			ret = Type::ToWstring( _uint );
			break;
		case VarType_float:
			ret = Type::ToWstring( _float );
			break;
		case VarType_string:
			ret = Type::ToWstring((const char*)_string);
			break;
		case VarType_wstring:
			ret = Get_wstring();
			break;
		case VarType_EntityID:
			if( _EntityID )
				ret = Type::ToWstring( _EntityID->WholeIndex() )  + L"." + Type::ToWstring( _EntityID->Serial() );
			break;
		case VarType_Object:
			return ret;
			break;
		}
		return ret;
	}
	//------------------------------------------------------------------------
	void Var::ForceFromString( const string& str )
	{
		switch( m_Type )
		{
		case VarType_Unknown:
			break;
		case VarType_bool:
			_bool = ( str=="true" || str=="1" );
			break;
		case VarType_int:
			_int = Type::ToInt( str );  
			break;
		case VarType_uint:
			_uint = Type::ToUint( str );  
			break;
		case VarType_float:
			_float = Type::ToFloat( str );
			break;
		case VarType_string:
			Set_string( str );
			break;
		case VarType_wstring:
			Set_wstring( Type::ToWstring( str ) );
			break;
		case VarType_EntityID:
			{
				vector<string> params = StringUtil::Split( str, ".", 2 );
				if( params.size() == 2 )
				{
					int index = Type::ToInt( params[0] );
					int serial = Type::ToInt( params[1] );  
					EntityID entityID;
					entityID.Set( (EntityType)(index>>24), index&0x00ffffff, serial );
					Set_EntityID( entityID );
				}
			}
			break;
		case VarType_Object:
			{
			}
			break;
		}
	}
	//------------------------------------------------------------------------
	VarList::VarList()
	{
	}
	VarList::~VarList()
	{
		Clear();
	}
	void VarList::Clear()
	{
		if( !m_VarList.empty() )
		{
			m_VarList.clear();
		}
	}
	void VarList::Init( int size )
	{
		m_VarList.reserve( size );
	}
	void VarList::Resize( int size )
	{
		m_VarList.resize( size );
	}
	int VarList::Size() const
	{
		return (int)m_VarList.size();
	}
	uint8 VarList::Type( int index ) const
	{
		if( index < 0 || index >= Size() )
			return VarType_Unknown;
		return m_VarList[index].Type();
	}
	bool VarList::Get_bool( int index ) const
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_bool )
			return false;
		return m_VarList[index].Get_bool();
	}
	int VarList::Get_int( int index ) const
	{
		if( index < 0 || index >= Size() )
			return 0;
		if( m_VarList[index].Type() != VarType_int && m_VarList[index].Type() != VarType_uint )
			return 0;
		return m_VarList[index].Get_int();
	}
	unsigned int VarList::Get_uint( int index ) const
	{
		if( index < 0 || index >= Size() )
			return 0;
		if( m_VarList[index].Type() != VarType_int && m_VarList[index].Type() != VarType_uint )
			return 0;
		return m_VarList[index].Get_uint();
	}
	float VarList::Get_float( int index ) const
	{
		if( index < 0 || index >= Size() )
			return 0.0f;
		if( m_VarList[index].Type() != VarType_float )
			return 0.0f;
		return m_VarList[index].Get_float();
	}
	const char* VarList::Get_string( int index ) const
	{
		if( index < 0 || index >= Size() )
			return "";
		if( m_VarList[index].Type() != VarType_string )
			return "";
		return m_VarList[index].Get_string();
	}
	const wchar_t* VarList::Get_wstring( int index ) const
	{
		if( index < 0 || index >= Size() )
			return L"";
		if( m_VarList[index].Type() != VarType_wstring )
			return L"";
		return m_VarList[index].Get_wstring();
	}
	EntityID VarList::Get_EntityID( int index ) const
	{
		if( index < 0 || index >= Size() )
			return Null_EntityID;
		if( m_VarList[index].Type() != VarType_EntityID )
			return Null_EntityID;
		return m_VarList[index].Get_EntityID();
	}
	WeObject* VarList::Get_Object( int index ) const
	{
		if( index < 0 || index >= Size() )
			return 0;
		if( m_VarList[index].Type() != VarType_Object )
			return 0;
		return m_VarList[index].Get_Object();
	}
	const Var& VarList::GetValue( int index ) const
	{
		if( index < 0 || index >= Size() )
			return Null_Var;
		return m_VarList[index];
	}

	bool VarList::Set_bool( int index, bool value )
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_bool )
			return false;
		m_VarList[index].Set_bool( value );
		return true;
	}
	bool VarList::Set_int( int index, int value )
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_int && m_VarList[index].Type() != VarType_uint)
			return false;
		m_VarList[index].Set_int( value );
		return true;
	}
	bool VarList::Set_uint( int index, unsigned int value )
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_int && m_VarList[index].Type() != VarType_uint )
			return false;
		m_VarList[index].Set_uint( value );
		return true;
	}
	bool VarList::Set_float( int index, float value )
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_float )
			return false;
		m_VarList[index].Set_float( value );
		return true;
	}
	bool VarList::Set_string( int index, const char* value )
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_string )
			return false;
		m_VarList[index].Set_string( value );
		return true;
	}
	bool VarList::Set_string( int index, const string& value )
	{
		return Set_string( index, value.c_str() );
	}
	bool VarList::Set_wstring( int index, const wchar_t* value )
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_wstring )
			return false;
		m_VarList[index].Set_wstring( value );
		return true;
	}
	bool VarList::Set_wstring( int index, const wstring& value )
	{
		return Set_wstring( index, value.c_str() );
	}
	bool VarList::Set_EntityID( int index, const EntityID& value )
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_EntityID )
			return false;
		m_VarList[index].Set_EntityID( value );
		return true;
	}
	bool VarList::Set_Object( int index, WeObject* value )
	{
		if( index < 0 || index >= Size() )
			return false;
		if( m_VarList[index].Type() != VarType_Object )
			return false;
		m_VarList[index].Set_Object( value );
		return true;
	}
	bool VarList::SetValue( int index, const Var& value )
	{
		if( index < 0 || index >= Size() )
			return false;
		m_VarList[index] = value;
		return true;
	}

	void VarList::Add_bool( bool value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_bool m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_int( int value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_int m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_uint( unsigned int value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_int m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_float( float value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_float m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_string( const char* value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_string m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_string( const string& value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_string m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_wstring( const wchar_t* value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_wstring m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_wstring( const wstring& value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_wstring m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_EntityID( const EntityID& value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_UserData m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::Add_Object( WeObject* value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::Add_UserData m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
	void VarList::AddValue( const Var& value )
	{
		/// 太大了，应该出问题了
		if( m_VarList.size() > 1000 )
		{
			//_LogError( "VarList::AddValue m_VarList.size() > 1000" );
			return;
		}
		m_VarList.push_back( Var(value) );
	}
}