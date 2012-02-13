/********************************************************************
created:	2009-9-17
author:		Fish (于国平)
svn:		CardGameOnline
purpose:	流数据的读写
注:			读写流数据时,需要使用try catch来捕获异常
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeVar.h"
#include "WeType.h"
#include <exception>

#pragma warning( disable : 4018 )

namespace We
{
	// 是否使用流异常
#ifdef _DEBUG
#define USE_WESTREAM_EXCEPTION
#endif

#ifdef USE_WESTREAM_EXCEPTION
#define SET_WESTREAM_ERROR( streamError )\
	m_StreamError = streamError;\
	throw StreamException( #streamError )
#else
#define SET_WESTREAM_ERROR( streamError )\
	m_StreamError = streamError
#endif

	enum StreamError
	{
		StreamError_Error,
		StreamError_OK,
		StreamError_BufferFull,
		StreamError_BadArg,
	};

	class StreamException : public std::exception
	{
	public:
		StreamException(const char* errMsg ) : std::exception( errMsg ) {}
		virtual ~StreamException() _THROW0() {}
	};

	class We_EXPORTS StreamWriter
	{
	protected:
		uint8*	m_Buffer;
		uint8*	m_Pointer;
		int		m_Size;
		uint8	m_StreamError;
	public:
		StreamWriter( uint8* pBuffer, int size ) : m_Buffer(pBuffer), m_Pointer(pBuffer ), m_Size(size), m_StreamError(StreamError_OK) { assert( m_Buffer ); assert(size>0); }

		StreamError GetError() const { return (StreamError)m_StreamError; }

		inline int GetOffset() const	{ return (int)(m_Pointer-m_Buffer); }
		inline int GetSpace() const		{ return (int)(m_Size-(m_Pointer-m_Buffer)); }
		inline int GetSize() const { return m_Size; }

		StreamWriter& WriteVar( const We::Var& var )
		{
			switch( var.Type() )
			{
			case VarType_Unknown:
				{
					SET_WESTREAM_ERROR( StreamError_BadArg );
					return *this;
				}
				break;
			case VarType_bool:
				Write( var.Type() );
				return Write( var.Get_bool() );
				break;
			case VarType_int:
				Write( var.Type() );
				return Write( var.Get_int() );
				break;
			case VarType_uint:
				Write( var.Type() );
				return Write( var.Get_uint() );
				break;
			case VarType_float:
				Write( var.Type() );
				return Write( var.Get_float() );
				break;
			case VarType_string:
				Write( var.Type() );
				return Write( var.Get_string() );
				break;
			case VarType_wstring:
				Write( var.Type() );
				return Write( var.ForceToString().c_str() );
				break;
			case VarType_AllocPointer:
				Write( var.Type() );
				return Write( (uint8*)(var.Get_Pointer()), var.GetDataSize() );
				break;
			case VarType_EntityID:
				Write( var.Type() );
				return Write( var.Get_EntityID() );
				break;
			default:
				{
					SET_WESTREAM_ERROR( StreamError_BadArg );
					return *this;
				}
				break;
			}
			return *this;
		}

		/// 1字节 -0x7f - 0x7f
		/// 2字节 -0x7fff - 0x7fff
		/// 3字节 -0x7fffff - 0x7fffff
		/// 4字节 -0x7fffffff - 0x7fffffff
		StreamWriter& Write( int i )
		{
			StreamWriteData<int>( i );
			return *this;
		}
		StreamWriter& Write( uint32 u )
		{
			StreamWriteData<uint32>( u );
			return *this;
		}

		StreamWriter& Write( int8 i )
		{
			StreamWriteData<int8>( i );
			return *this;
		}
		StreamWriter& Write( uint8 u )
		{
			StreamWriteData<uint8>( u );
			return *this;
		}

		StreamWriter& Write( int16 i )
		{
			StreamWriteData<int16>( i );
			return *this;
		}
		StreamWriter& Write( uint16 u )
		{
			StreamWriteData<uint16>( u );
			return *this;
		}

		StreamWriter& Write( float f )
		{
			StreamWriteData<float>( f );
			return *this;
		}

		StreamWriter& Write( double d )
		{
			StreamWriteData<double>( d );
			return *this;
		}

		StreamWriter& Write( int64 i )
		{
			StreamWriteData<int64>( i );
			return *this;
		}
		StreamWriter& Write( uint64 u )
		{
			StreamWriteData<uint64>( u );
			return *this;
		}

		StreamWriter& Write( bool b )
		{
			if( GetSpace() < 1 )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return *this;
			}
			*m_Pointer = (b?1:0);
			++m_Pointer;
			return *this;
		}

		StreamWriter& Write( const char* s )
		{
			if( !s )
			{
				SET_WESTREAM_ERROR( StreamError_BadArg );
				return *this;
			}
			uint32 len = ::strlen( s );
			return Write( (const uint8*)s, len ); /// 不拷贝'\0'
		}

		StreamWriter& Write( const EntityID& id )
		{
			StreamWriteData<EntityID>( id );
			return *this;
		}

		// 写入一个结构数据
		StreamWriter& Write( const uint8* data, uint32 size )
		{
			if( !data )
			{
				SET_WESTREAM_ERROR( StreamError_BadArg );
				return *this;
			}
			if( GetSpace() < size + sizeof(uint32) )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return *this;
			}
			*((uint32*)m_Pointer) = size;
			m_Pointer += sizeof(uint32);
			::memcpy( m_Pointer, data, size );
			m_Pointer += size;
			return *this;
		}
	protected:
		template<class T>
		void StreamWriteData( T value )
		{
			if( GetSpace() < sizeof(T) )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return;
			}
			*((T*)m_Pointer) = value;
			m_Pointer += sizeof(T);
		}
	};

	class We_EXPORTS StreamReader
	{
	protected:
		uint8*	m_Buffer;
		uint8*	m_Pointer;
		int		m_Size;
		uint8	m_StreamError;
	public:
		StreamReader( uint8* pBuffer, int size ) : m_Buffer(pBuffer), m_Pointer(pBuffer), m_Size(size), m_StreamError(StreamError_OK) { assert( m_Buffer ); assert(size>=0); }

		StreamError GetError() const { return (StreamError)m_StreamError; }

		inline int GetOffset() const	{ return (int)(m_Pointer-m_Buffer); }
		inline int GetSpace() const		{ return (int)(m_Size-(m_Pointer-m_Buffer)); }

		bool ReadVar( Var& var )
		{
			if( GetSpace() < sizeof(uint8) )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return false;
			}
			uint8 varType = Read_uint8();
			switch( varType )
			{
			case VarType_Unknown:
				{
					SET_WESTREAM_ERROR( StreamError_BadArg );
					return false;
				}
				break;
			case VarType_bool:
				var.Set_bool( Read_bool() ); 
				return true;
				break;
			case VarType_int:
				var.Set_int( Read_int() );
				return true;
				break;
			case VarType_uint:
				var.Set_uint( Read_uint() );
				return true;
				break;
			case VarType_float:
				var.Set_float( Read_float() );
				return true;
				break;
			case VarType_string:
				var.Set_string( Read_string() );
				return true;
				break;
			case VarType_wstring:
				// 此处可改进,不需要字符串转换!!!
				var.Set_wstring( Type::ToWstring(Read_string()) );
				return true;
				break;
			case VarType_AllocPointer:
				{
					uint32 dataSize = 0;
					uint8* dataPointer = GetData( dataSize );
					if( dataPointer != 0 )
						var.Set_Pointer( dataPointer, dataSize );
					return true;
				}
				break;
			case VarType_EntityID:
				var.Set_EntityID( Read_EntityID() );
				return true;
				break;
			default:
				{
					SET_WESTREAM_ERROR( StreamError_BadArg );
					return false;
				}
				break;
			}
			return true;
		}

		int8 Read_int8()
		{
			return StreamReadData<int8>(0);
		}

		uint8 Read_uint8()
		{
			return StreamReadData<uint8>(0);
		}

		int16 Read_int16()
		{
			return StreamReadData<int16>(0);
		}

		uint16 Read_uint16()
		{
			return StreamReadData<uint16>(0);
		}

		int Read_int()
		{
			return StreamReadData<int>(0);
		}

		uint32 Read_uint()
		{
			return StreamReadData<uint32>(0);
		}

		float Read_float()
		{
			return StreamReadData<float>( 0.0f );
		}

		double Read_double()
		{
			return StreamReadData<double>( 0.0f );
		}

		int64 Read_int64()
		{
			return StreamReadData<int64>( 0 );
		}

		uint64 Read_uint64()
		{
			return StreamReadData<uint64>( 0 );
		}

		bool Read_bool()
		{
			if( GetSpace() < 1 )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return false;
			}
			bool b = (*m_Pointer==1);
			++m_Pointer;
			return b;
		}

		std::string Read_string()
		{
			if( GetSpace() < 4 )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return false;
			}
			uint32 len = *((uint32*)m_Pointer);
			m_Pointer += sizeof(uint32);
			if( GetSpace() < len )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return false;
			}
			std::string str( (const char*)m_Pointer, len );
			m_Pointer += len;
			return str;
		}

		EntityID Read_EntityID()
		{
			return StreamReadData<EntityID>( Null_EntityID );
		}

		// 返回结构数据的指针
		uint8* GetData( uint32& dataSize )
		{
			if( GetSpace() < 4 )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return false;
			}
			uint32 len = *((uint32*)m_Pointer);
			m_Pointer += sizeof(uint32);
			if( GetSpace() < len )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return false;
			}
			uint8* dataPointer = m_Pointer;
			dataSize = len;
			m_Pointer += len;
			return dataPointer;
		}
		// 读取一块数据
		bool ReadMem( void* dest, uint32 size )
		{
			if( size < 1 )
				return false;
			uint32 dataSize = 0;
			uint8* data = GetData( dataSize );
			if( !data || dataSize != size )
				return false;
			::memcpy( dest, data, size );
			return true;
		}

		void Read( int8& value ) { value = Read_int8(); }
		void Read( uint8& value ) { value = Read_uint8(); }
		void Read( int& value ) { value = Read_int(); }
		void Read( uint32& value ) { value = Read_uint(); }
		void Read( float& value ) { value = Read_float(); }
		void Read( double& value ) { value = Read_double(); }
		void Read( int64& value ) { value = Read_int64(); }
		void Read( uint64& value ) { value = Read_uint64(); }
		void Read( bool& value ) { value = Read_bool(); }
		void Read( std::string& value ) { value = Read_string(); }
		void Read( Var& value ) { ReadVar(value); }

	protected:
		template<class T>
		T StreamReadData( T defaultValue )
		{
			if( GetSpace() < sizeof(T) )
			{
				SET_WESTREAM_ERROR( StreamError_BufferFull );
				return defaultValue;
			}
			T ret = *((T*)m_Pointer);
			m_Pointer += sizeof(T);
			return ret;
		}
	};

	/// 重载运算符
	template <class T>
	StreamWriter& operator<<(StreamWriter& writer, T v)
	{
		return writer.Write(v);
	}
	template <class T>
	StreamReader& operator>>(StreamReader& reader, T& v)
	{
		reader.Read( v );
		return reader;
	}

}