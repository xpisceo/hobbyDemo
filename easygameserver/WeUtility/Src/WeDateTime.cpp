#include "WeDateTime.h"
#include <sstream>
#include <iomanip>
using namespace std;

namespace We
{
	//------------------------------------------------------------------------
	DateTime::DateTime()
	{
		time(&m_Time);
		m_LocalTime = localtime(&m_Time);
	}
	void DateTime::Update()
	{
		time(&m_Time);
		m_LocalTime = localtime(&m_Time);
	}
	//------------------------------------------------------------------------
	int DateTime::GetYear() const
	{
		return 1900+m_LocalTime->tm_year;
	}
	//------------------------------------------------------------------------
	int DateTime::GetMonth() const
	{
		return 1+m_LocalTime->tm_mon;
	}
	//------------------------------------------------------------------------
	int DateTime::GetDay() const
	{
		return m_LocalTime->tm_mday;
	}
	//------------------------------------------------------------------------
	int DateTime::GetHour() const
	{
		return m_LocalTime->tm_hour;
	}
	//------------------------------------------------------------------------
	int DateTime::GetMinute() const
	{
		return m_LocalTime->tm_min;
	}
	//------------------------------------------------------------------------
	int DateTime::GetSecond() const
	{
		return m_LocalTime->tm_sec;
	}
	int DateTime::GetWeekDay() const
	{
		if( m_LocalTime->tm_wday == 0 )
			return 7;
		return m_LocalTime->tm_wday;
	}
	//------------------------------------------------------------------------
	string DateTime::ToString(DateTimeFormat format ) const
	{
		switch( format )
		{
		case DateTimeFormat_YYYY_MM_DD_HH_MM_SS:
			{
				stringstream stream;
				stream << setw(4) << (1900 + m_LocalTime->tm_year) << '-'
					<<setfill('0')<< setw(2) << 1 + m_LocalTime->tm_mon  <<'-'
					<<setfill('0')<< setw(2) << m_LocalTime->tm_mday << " ";
				stream << setfill('0') << setw(2) << m_LocalTime->tm_hour << ':' <<
					setfill('0') << setw(2) << m_LocalTime->tm_min << ':' <<
					setfill('0') << setw(2) << m_LocalTime->tm_sec;
				return string(stream.str());
			}
			break;

		case DateTimeFormat_YYYYMMDDHHMMSS:
			{
				stringstream stream;
				stream << setw(4) << (1900 + m_LocalTime->tm_year) 
					<<setfill('0')<< setw(2) << 1 + m_LocalTime->tm_mon
					<<setfill('0')<< setw(2) << m_LocalTime->tm_mday;
				stream << setfill('0') << setw(2) << m_LocalTime->tm_hour <<
					setfill('0') << setw(2) << m_LocalTime->tm_min <<
					setfill('0') << setw(2) << m_LocalTime->tm_sec;
				return string(stream.str());
			}
			break;

		default:
			{
				return "";
			}
			break;
		}
	}
	//------------------------------------------------------------------------
	string DateTime::GetDateString(DateTimeFormat format ) const
	{
		switch( format )
		{
		case DateTimeFormat_YYYY_MM_DD_HH_MM_SS:
			{
				stringstream stream;
				stream << setw(4) << (1900 + m_LocalTime->tm_year) << '-'
					<<setfill('0')<< setw(2) << 1 + m_LocalTime->tm_mon  <<'-'
					<<setfill('0')<< setw(2) << m_LocalTime->tm_mday;
				return string(stream.str());
			}
			break;

		case DateTimeFormat_YYYYMMDDHHMMSS:
			{
				stringstream stream;
				stream << setw(4) << (1900 + m_LocalTime->tm_year) 
					<<setfill('0')<< setw(2) << 1 + m_LocalTime->tm_mon
					<<setfill('0')<< setw(2) << m_LocalTime->tm_mday;
				return string(stream.str());
			}
			break;

		default:
			{
				return "";
			}
			break;
		}
	}
	//------------------------------------------------------------------------
	string DateTime::GetTimeString(DateTimeFormat format ) const
	{
		switch( format )
		{
		case DateTimeFormat_YYYY_MM_DD_HH_MM_SS:
			{
				stringstream stream;
				stream << setfill('0') << setw(2) << m_LocalTime->tm_hour << ':' <<
					setfill('0') << setw(2) << m_LocalTime->tm_min << ':' <<
					setfill('0') << setw(2) << m_LocalTime->tm_sec ;
				return string(stream.str());
			}
			break;

		case DateTimeFormat_YYYYMMDDHHMMSS:
			{
				stringstream stream;
				stream << setfill('0') << setw(2) << m_LocalTime->tm_hour <<
					setfill('0') << setw(2) << m_LocalTime->tm_min  <<
					setfill('0') << setw(2) << m_LocalTime->tm_sec;
				return string(stream.str());
			}
			break;

		default:
			{
				return "";
			}
			break;
		}
	}
	//------------------------------------------------------------------------
}