/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	
*********************************************************************/
#pragma once

// dll�⵼��
#ifdef  We_DLL_EXPORTS
#define We_EXPORTS __declspec(dllexport)
#else
// dll����
#ifdef  We_DLL_IMPORTS
#define We_EXPORTS __declspec(dllimport)
#else
// ��̬��
#define We_EXPORTS
#endif
#endif

// ʹ�ñ�׼��ʱ,�������ĳ��ȳ������ƣ����Ʊ��ض�
#pragma warning (disable : 4786)

// ʹ�ñ�׼��ʱ,�������ĳ��ȳ������ƣ����Ʊ��ض�
#pragma warning (disable : 4503)

// warning C4251: <type> ��Ҫ�� dll �ӿ��� class��ClassName���Ŀͻ���ʹ�� 
// ��ģ����Ա����publicʱ�����warning
#pragma warning (disable : 4251)

// �� dll �ӿ� class��ClassName������ dll �ӿ� class��ClassName���Ļ���
#pragma warning (disable : 4275)

// û��Ϊ��ʽģ��ʵ���������ṩ�ʵ��Ķ���
#pragma warning( disable: 4661)

// 
#pragma warning( disable: 4996)

// ��ʼ����: �ӡ�size_t��ת������int�������ܶ�ʧ����
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
	// ����������������Ϣ
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