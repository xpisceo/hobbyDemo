/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	日志
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include <fstream>
#include <sstream>
using namespace std;

namespace We
{
	enum LogLevel
	{
		LogLevel_Standard,	//普通
		LogLevel_Warning,	//警告
		LogLevel_Error,		//错误
		LogLevel_Debug,		//只在Debug版本下记录日志
	};

	class We_EXPORTS Log
	{
	public:
		Log();
		Log( const string& fileName, bool append=false );
		~Log();

		bool SetLogFile( const string& fileName, bool append=false );

		void LogMessage( const string& message, LogLevel logLevel=LogLevel_Standard );

		void LogWarning( const string& message );

		void LogError( const string& message );

		void LogDebug( const string& message );

		void WriteLog( const char* format, ... );

		void Close();

	protected:
		ofstream	m_ostream;
		string		m_LogFileName;
	};
}
