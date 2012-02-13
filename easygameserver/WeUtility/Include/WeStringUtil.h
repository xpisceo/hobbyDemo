/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	字符串操作
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

namespace We
{
	typedef vector<string> StringVector;

	class We_EXPORTS StringUtil
	{
	public:
		typedef std::stringstream StrStreamType;

		static void Trim( string& str, bool left = true, bool right = true );

		static std::vector<string> Split( const string& str, const string& delims = "\t\n ", unsigned int maxSplits = 0);

		static void ToLowerCase( string& str );

		static void ToUpperCase( string& str );

		static bool StartsWith(const string& str, const string& pattern, bool lowerCase = true);

		static bool EndsWith(const string& str, const string& pattern, bool lowerCase = true);

		static string StandardisePath( const string &init);

		static void SplitFilename(const string& qualifiedName, string& outBasename, string& outPath);

		static bool Match(const string& str, const string& pattern, bool caseSensitive = true);
	};
}
