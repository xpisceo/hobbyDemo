/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��־
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
		LogLevel_Standard,	//��ͨ
		LogLevel_Warning,	//����
		LogLevel_Error,		//����
		LogLevel_Debug,		//ֻ��Debug�汾�¼�¼��־
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
