/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	
*********************************************************************/
#pragma once

// dll库导出
#ifdef  We_DLL_EXPORTS
#define We_EXPORTS __declspec(dllexport)
#else
// dll导入
#ifdef  We_DLL_IMPORTS
#define We_EXPORTS __declspec(dllimport)
#else
// 静态库
#define We_EXPORTS
#endif
#endif

// 使用标准库时,修饰名的长度超出限制，名称被截断
#pragma warning (disable : 4786)

// 使用标准库时,修饰名的长度超出限制，名称被截断
#pragma warning (disable : 4503)

// warning C4251: <type> 需要有 dll 接口由 class“ClassName”的客户端使用 
// 当模板库成员不是public时引起此warning
#pragma warning (disable : 4251)

// 非 dll 接口 class“ClassName”用作 dll 接口 class“ClassName”的基类
#pragma warning (disable : 4275)

// 没有为显式模板实例化请求提供适当的定义
#pragma warning( disable: 4661)

// 
#pragma warning( disable: 4996)

// 初始化”: 从“size_t”转换到“int”，可能丢失数据
#pragma warning( disable: 4267)

// signed/unsigned mismatch
#pragma warning( disable: 4018)


typedef signed __int64 int64;
typedef signed __int32 int32;
typedef signed __int16 int16;
typedef signed __int8 int8;

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;
typedef unsigned __int8 uint8;

typedef	unsigned long	ulong;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned char	uchar;

/// standard library
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <hash_map>
using namespace std;
using namespace stdext;

#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <winsock2.h>
#include <Windows.h>
#include <mmsystem.h>
#include <math.h>

#pragma comment(lib, "winmm.lib" )
#pragma comment(lib, "ws2_32.lib" )


#if !defined(SAFE_DELETE)
#define SAFE_DELETE(p) if((p)) { delete (p); (p)=0; }
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(p) if((p)) { delete [](p); (p)=0; }
#endif

namespace We
{
	// 输出窗口输出调试信息
	static void DebugOutput( const char* format, ... )
	{
#ifdef _DEBUG
		va_list arg_ptr;
		char str[2048] = {0};

		va_start(arg_ptr, format);
		vsprintf_s(str, sizeof(str)-4, format, arg_ptr);
		va_end(arg_ptr);

		strcat( str, "\r\n" );
		OutputDebugString( str );
#endif
	}

	static void OutputString( const char* format, ... )
	{
		va_list arg_ptr;
		char str[2048] = {0};

		va_start(arg_ptr, format);
		vsprintf_s(str, sizeof(str)-4, format, arg_ptr);
		va_end(arg_ptr);

		strcat( str, "\r\n" );
		OutputDebugString( str );
	}
}