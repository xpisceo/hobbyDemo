#include "WeType.h"
#include <windows.h>

namespace We
{

	//------------------------------------------------------------------------
	string Type::ToString( float value, unsigned short precision /*= 6*/)
	{
		char ch[32];
		switch( precision )
		{
		case 1:
			sprintf_s( ch, sizeof(ch), "%.1f", value );
			break;
		case 2:
			sprintf_s( ch, sizeof(ch), "%.2f", value );
			break;
		case 3:
			sprintf_s( ch, sizeof(ch), "%.3f", value );
			break;
		case 4:
			sprintf_s( ch, sizeof(ch), "%.4f", value );
			break;
		case 5:
			sprintf_s( ch, sizeof(ch), "%.5f", value );
			break;
		case 6:
			sprintf_s( ch, sizeof(ch), "%.6f", value );
			break;
		case 7:
			sprintf_s( ch, sizeof(ch), "%.7f", value );
			break;
		case 8:
			sprintf_s( ch, sizeof(ch), "%.8f", value );
			break;
		default:
			sprintf_s( ch, sizeof(ch), "%f", value );
			break;
		}
		return string((const char*)ch);
	}
	//------------------------------------------------------------------------
	string Type::ToString( const wchar_t* value )
	{
		if( value == 0 )
			return "";
		if (::wcslen(value) < 256)
		{
			char buf[512];
			ToString(value, buf, sizeof(buf));
			return string(buf);
		}
		else
		{
			const int SIZE2 = GetToStringLen(value);
			char* p = new char[SIZE2];
			ToString(value, p, SIZE2);
			string res(p);
			delete[] p;
			return res;
		}
	}
	//------------------------------------------------------------------------
	wstring Type::ToWstring( float value, unsigned short precision /* = 6 */)
	{
		// 奇怪的问题,swprintf_s在vc2008里会导致崩溃
		wchar_t ch[32];
		switch( precision )
		{
		case 1:
			swprintf( ch, L"%.1f", value );
			break;
		case 2:
			swprintf( ch, L"%.2f", value );
			break;
		case 3:
			swprintf( ch, L"%.3f", value );
			break;
		case 4:
			swprintf( ch, L"%.4f", value );
			break;
		case 5:
			swprintf( ch, L"%.5f", value );
			break;
		case 6:
			swprintf( ch, L"%.6f", value );
			break;
		case 7:
			swprintf( ch, L"%.7f", value );
			break;
		case 8:
			swprintf( ch, L"%.8f", value );
			break;
		default:
			swprintf( ch, L"%f", value );
			break;
		}
		return wstring((const wchar_t*)ch);
	}
	//------------------------------------------------------------------------
	wstring Type::ToWstring( const char* value )
	{
		if( value == 0 )
			return L"";
		if (::strlen(value) < 256)
		{
			wchar_t buf[256];
			ToWideStr(value, buf, sizeof(buf));
			return wstring(buf);
		}
		else
		{
			const int SIZE2 = GetToWideStrLen(value);
			wchar_t* p = new wchar_t[SIZE2 / sizeof(wchar_t)];
			ToWideStr(value, p, SIZE2);
			wstring res(p);
			delete[] p;
			return res;
		}
	}
	//------------------------------------------------------------------------
}
