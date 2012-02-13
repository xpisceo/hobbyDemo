#include "WeLog.h"
#include "WeDebugUtil.h"
#include <ctime>
#include <iomanip>

namespace We
{
	//------------------------------------------------------------------------
	Log::Log()
	{

	}
	//------------------------------------------------------------------------
	Log::Log(const string& fileName, bool append/* =false  */)
	{
		SetLogFile( fileName, append );
	}
	//------------------------------------------------------------------------
	Log::~Log()
	{
		if( m_ostream.is_open() )
		{
			m_ostream.close();
		}
	}
	//------------------------------------------------------------------------
	bool Log::SetLogFile(const string& fileName, bool append/* =false  */)
	{
		if (m_ostream.is_open())
		{
			m_ostream.close();
		}
		m_ostream.open(fileName.c_str(), std::ios_base::out | (append ? std::ios_base::app : std::ios_base::trunc));
		if( !m_ostream )
		{
			return false;
			//string info = "Log::SetLogFile 创建日志文件失败:";
			//info += fileName;
			//Assert_s( 0, info );
		}
		return true;
	}
	//------------------------------------------------------------------------
	void Log::LogMessage(const string& message, LogLevel logLevel/* =LogLevel_Standard  */)
	{
		if( logLevel == LogLevel_Debug )
		{
#ifndef _DEBUG
			return;
#endif
		}
		time_t  et;
		time(&et);
		tm* etm = localtime(&et);
		Assert( etm != 0 );
		if( etm == 0 )
			return;
		ostringstream msgStream;
		// write date
		msgStream << setw(4) << (1900 + etm->tm_year) << '-'
			<<setfill('0')<< setw(2) << 1 + etm->tm_mon <<'-'
			<<setfill('0')<< setw(2) << etm->tm_mday
			<< ' ';
		// wite time
		msgStream << setfill('0') << setw(2) << etm->tm_hour << ':' <<
			setfill('0') << setw(2) << etm->tm_min << ':' <<
			setfill('0') << setw(2) << etm->tm_sec << ' ';
		// write LogLevel
		switch( logLevel )
		{
		case LogLevel_Standard:
			{
				msgStream << "(Log....)\t";
			}
			break;
		case LogLevel_Warning:
			{
				msgStream << "(Warning)\t";
			}
			break;
		case LogLevel_Error:
			{
				msgStream << "(Error..)\t";
			}
			break;
		case LogLevel_Debug:
			{
				msgStream << "(Debug..)\t";
			}
			break;

		default:
			{
				msgStream << "(Unknown)\t";
			}
			break;
		}
		msgStream << message << endl;
		m_ostream << msgStream.str();
		m_ostream.flush();
	}
	//------------------------------------------------------------------------
	void Log::LogWarning(const string& message )
	{
		LogMessage( message, LogLevel_Warning );
	}
	//------------------------------------------------------------------------
	void Log::LogError(const string& message )
	{
		LogMessage( message, LogLevel_Error );
	}
	//------------------------------------------------------------------------
	void Log::LogDebug(const string& message )
	{
		LogMessage( message, LogLevel_Debug );
	}
	//------------------------------------------------------------------------
	void Log::WriteLog( const char* format, ... )
	{
		va_list arg_ptr;
		char str[2048] = {0};

		va_start(arg_ptr, format);
		vsprintf_s(str, sizeof(str)-4, format, arg_ptr);
		va_end(arg_ptr);

		LogMessage( str, LogLevel_Error );
	}
	void Log::Close()
	{
		if( m_ostream.is_open() )
		{
			m_ostream.close();
		}
	}
}