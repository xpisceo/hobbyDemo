#include "WeStringUtil.h"

namespace We
{
	//-----------------------------------------------------------------------
	//const string StringUtil::BLANK = "";
	//-----------------------------------------------------------------------
	void StringUtil::Trim(string& str, bool left, bool right)
	{
		static const string delims = " \t\r";
		if(right)
			str.erase(str.find_last_not_of(delims)+1); // trim right
		if(left)
			str.erase(0, str.find_first_not_of(delims)); // trim left
	}
	//-----------------------------------------------------------------------
	std::vector<string> StringUtil::Split( const string& str, const string& delims, unsigned int maxSplits)
	{
		// static unsigned dl;
		std::vector<string> ret;
		unsigned int numSplits = 0;

		if( str.length() == 0 )
			return ret;

		// Use STL methods 
		size_t start, pos;
		start = 0;
		do 
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				// Do Nothing
				ret.push_back( "" );
				start = pos + 1;
			}
			else if (pos == string::npos || (maxSplits && numSplits == maxSplits))
			{
				// Copy the rest of the string
				ret.push_back( str.substr(start) );
				break;
			}
			else
			{
				// Copy up to delimiter
				ret.push_back( str.substr(start, pos - start) );
				start = pos + 1;
			}
			// parse up to next real data
			//start = str.find_first_not_of(delims, start);
			++numSplits;

		} while (pos != string::npos);



		return ret;
	}

	//-----------------------------------------------------------------------
	void StringUtil::ToLowerCase(string& str)
	{
		std::transform(
			str.begin(),
			str.end(),
			str.begin(),
			tolower);
	}

	//-----------------------------------------------------------------------
	void StringUtil::ToUpperCase(string& str) 
	{
		std::transform(
			str.begin(),
			str.end(),
			str.begin(),
			toupper);
	}
	//-----------------------------------------------------------------------
	bool StringUtil::StartsWith(const string& str, const string& pattern, bool lowerCase)
	{
		size_t thisLen = str.length();
		size_t patternLen = pattern.length();
		if (thisLen < patternLen || patternLen == 0)
			return false;

		string startOfThis = str.substr(0, patternLen);
		if (lowerCase)
			StringUtil::ToLowerCase(startOfThis);

		return (startOfThis == pattern);
	}
	//-----------------------------------------------------------------------
	bool StringUtil::EndsWith(const string& str, const string& pattern, bool lowerCase)
	{
		size_t thisLen = str.length();
		size_t patternLen = pattern.length();
		if (thisLen < patternLen || patternLen == 0)
			return false;

		string endOfThis = str.substr(thisLen - patternLen, patternLen);
		if (lowerCase)
			StringUtil::ToLowerCase(endOfThis);

		return (endOfThis == pattern);
	}
	//-----------------------------------------------------------------------
	string StringUtil::StandardisePath(const string& init)
	{
		string path = init;

		std::replace( path.begin(), path.end(), '/', '\\' );
		if( path[path.length() - 1] != '\\' )
			path += '\\';

		return path;
	}
	//-----------------------------------------------------------------------
	void StringUtil::SplitFilename(const string& qualifiedName, string& outBasename, string& outPath)
	{
		string path = qualifiedName;
		// Replace \ with / first
		std::replace( path.begin(), path.end(), '\\', '/' );
		// split based on final /
		size_t i = path.find_last_of('/');

		if (i == string::npos)
		{
			outPath = "";
			outBasename = qualifiedName;
		}
		else
		{
			outBasename = path.substr(i+1, path.size() - i - 1);
			outPath = path.substr(0, i+1);
		}

	}
	//-----------------------------------------------------------------------
	bool StringUtil::Match(const string& str, const string& pattern, bool caseSensitive)
	{
		string tmpStr = str;
		string tmpPattern = pattern;
		if (!caseSensitive)
		{
			StringUtil::ToLowerCase(tmpStr);
			StringUtil::ToLowerCase(tmpPattern);
		}

		string::const_iterator strIt = tmpStr.begin();
		string::const_iterator patIt = tmpPattern.begin();
		while (strIt != tmpStr.end() && patIt != tmpPattern.end())
		{
			if (*patIt == '*')
			{
				// Skip over looking for next character
				++patIt;
				if (patIt == tmpPattern.end())
				{
					// Skip right to the end since * matches the entire rest of the string
					strIt = tmpStr.end();
				}
				else
				{
					// scan until we find next pattern character
					while(strIt != tmpStr.end() && *strIt != *patIt)
						++strIt;
				}
			}
			else
			{
				if (*patIt != *strIt)
				{
					return false;
				}
				else
				{
					++patIt;
					++strIt;
				}
			}

		}
		// If we reached the end of both the pattern and the string, we succeeded
		if (patIt == tmpPattern.end() && strIt == tmpStr.end())
		{
			return true;
		}
		else
		{
			return false;
		}

	}

}
