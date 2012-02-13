/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	测试的一些定义
*********************************************************************/
#pragma once

#include <assert.h>
#include "WeType.h"

#ifdef _DEBUG
	#if _MSC_VER>=1400
		#define Assert(p)	(void)( (!!(p)) || (_wassert(_CRT_WIDE(#p), _CRT_WIDE(__FILE__), __LINE__), 0) )
	#else
		#define Assert(p)	assert(p)
	#endif
#else
	#define Assert(p)
#endif

#ifdef _DEBUG

#if _MSC_VER>=1400
inline void inlineAssert_s_Output_String( const char* exp, const string& info, const wchar_t* file, unsigned line )
{
	wstring wInfo = We::Type::ToWstring( exp );
	wInfo += L"\n";
	wInfo += We::Type::ToWstring( info );
	_wassert( wInfo.c_str(), file, line );
}
#define Assert_s(exp, info)  (void)( (!!(exp)) || (inlineAssert_s_Output_String(#exp, info, _CRT_WIDE(__FILE__), __LINE__), 0) )
#else
	#define Assert_s(exp,info) assert(exp)
#endif

#else
	#define Assert_s(exp,info) assert(exp)
#endif
