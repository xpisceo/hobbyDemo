/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	基本类型定义及转换操作
*********************************************************************/
#pragma once

#include <string>
#include <sstream>
using namespace std;

#include <stdlib.h>

#include "WeConfig.h"

namespace We
{
#pragma warning( disable : 4996 )
	class We_EXPORTS Type
	{
	public:
		/// to string
		static string ToString( int i ) { char ch[16]; ::itoa( i, ch, 10 ); return string((const char*)ch); }
		static string ToString( unsigned int i ) 
		{ 
			char ch[16];
			sprintf( ch, "%u", i );
			return string((const char*)ch);
		}
		static string ToString( float value, unsigned short precision = 6 );

		static string ToString( const wchar_t* value );
		static inline string ToString( const wstring& value )
		{ return ToString(value.c_str()); }
		static string ToString( wchar_t* value )
		{ return ToString( (const wchar_t*)value ); }

		// / to wstring
		static wstring ToWstring( int i ) { wchar_t ch[16]; swprintf(ch,L"%d", i); return wstring((const wchar_t*)ch); }
		static wstring ToWstring( unsigned int i ) 
		{
			wchar_t ch[16];
			swprintf(ch,L"%u", i);
			return wstring((const wchar_t*)ch);
		}
		static wstring ToWstring( float value, unsigned short precision = 6 );

		static wstring ToWstring( const char* value );
		static wstring ToWstring( const string& value )
		{ return ToWstring(value.c_str()); }
		static wstring ToWstring( char* value )
		{ return ToWstring((const char*)value); }

		/// string to
		static inline int ToInt( const char* str )
		{
			if( !str )
				return 0;
			return atoi( str );
		}
		static inline uint ToUint( const char* str )
		{
			if( !str )
				return 0;
			return static_cast<unsigned int>(strtoul(str, 0, 10));
		}
		static inline long ToLong( const char* str )
		{
			if( !str )
				return 0;
			return atol( str );
		}
		static inline ulong ToUlong( const char* str )
		{
			if( !str )
				return 0;
			return strtoul( str, 0, 10 );
		}
		static inline float ToFloat( const char* str )
		{
			if( !str )
				return 0.0f;
			return static_cast<float>(atof( str ));
		}

		static inline int ToInt( char* str )
		{
			return ToInt( (const char*)str );
		}
		static inline uint ToUint( char* str )
		{
			return ToUint( (const char*)str );
		}
		static inline long ToLong( char* str )
		{
			return ToLong( (const char*)str );
		}
		static inline ulong ToUlong( char* str )
		{
			return ToUlong( (const char*)str );
		}
		static inline float ToFloat( char* str )
		{
			return ToFloat( (const char*)str );
		}

		static inline int ToInt( const string& str )
		{
			return ToInt( str.c_str() );
		}
		static inline uint ToUint( const string& str )
		{
			return ToUint( str.c_str() );
		}
		static inline long ToLong( const string& str )
		{
			return ToLong( str.c_str() );
		}
		static inline ulong ToUlong( const string& str )
		{
			return ToUlong( str.c_str() );
		}
		static inline float ToFloat( const string& str )
		{
			return ToFloat( str.c_str() );
		}

		/// wstring to
		static inline int ToInt( const wchar_t* str )
		{
			if( !str )
				return 0;
			return _wtoi( str );
		}
		static inline uint ToUint( const wchar_t* str )
		{
			if( !str )
				return 0;
			return static_cast<unsigned int>(wcstoul(str, 0, 10));
		}
		static inline long ToLong( const wchar_t* str )
		{
			if( !str )
				return 0;
			return _wtol( str );
		}
		static inline ulong ToUlong( const wchar_t* str )
		{
			if( !str )
				return 0;
			return wcstoul( str, 0, 10 );
		}
		static inline float ToFloat( const wchar_t* str )
		{
			if( !str )
				return 0.0f;
			return static_cast<float>(_wtof( str ));
		}

		static inline int ToInt( wchar_t* str )
		{
			return ToInt( (const wchar_t*)str );
		}
		static inline uint ToUint( wchar_t* str )
		{
			return ToUint( (const wchar_t*)str );
		}
		static inline long ToLong( wchar_t* str )
		{
			return ToLong( (const wchar_t*)str );
		}
		static inline ulong ToUlong( wchar_t* str )
		{
			return ToUlong( (const wchar_t*)str );
		}
		static inline float ToFloat( wchar_t* str )
		{
			return ToFloat( (const wchar_t*)str );
		}

		static inline int ToInt( const wstring& str )
		{
			return ToInt( str.c_str() );
		}
		static inline uint ToUint( const wstring& str )
		{
			return ToUint( str.c_str() );
		}
		static inline long ToLong( const wstring& str )
		{
			return ToLong( str.c_str() );
		}
		static inline ulong ToUlong( const wstring& str )
		{
			return ToUlong( str.c_str() );
		}
		static inline float ToFloat( const wstring& str )
		{
			return ToFloat( str.c_str() );
		}

	private:
		static inline void ToString(const wchar_t* wstr, char* str, int size)
		{
			if ( !wstr || !str )
				return;
			if (::WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, size, NULL, NULL) == size)
			{
				str[size-1] = 0;
			}
		}

		static inline void ToWideStr(const char* str, wchar_t* wstr, int size)
		{
			if ( !wstr || !str )
				return;
			const int len = size / sizeof(wchar_t);
			if (::MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len) == len)
			{
				wstr[len-1] = 0;
			}
		}

		static inline int GetToStringLen(const wchar_t* wstr)
		{
			if ( !wstr )
				return 0;
			return ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		}

		static inline int GetToWideStrLen(const char* str)
		{
			if ( !str )
				return 0;
			return ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0) * sizeof(wchar_t);
		}
	};

}
