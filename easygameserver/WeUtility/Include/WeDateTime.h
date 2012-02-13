/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	日期
*********************************************************************/
#pragma once

#include "WeConfig.h"

#include <ctime>
#include <string>
using namespace std;

namespace We
{
	enum  DateTimeFormat
	{
		DateTimeFormat_YYYY_MM_DD_HH_MM_SS,	// 2006-08-01 15:01:03
		DateTimeFormat_YYYYMMDDHHMMSS,		// 20060801150103
		//DateTimeFormat_DDMMYYYY,	// 01082006
		//DateTimeFormat_DD_MM_YYYY,	// 01-08-2006
		//DateTimeFormat_MM_DD_YYYY,	// 08-01-2006
		/// ......
	};

	class We_EXPORTS DateTime
	{
	public:
		DateTime();

		void Update();

		int GetYear() const;
		int GetMonth() const;
		int GetDay() const;
		int GetHour() const;
		int GetMinute() const;
		int GetSecond() const;

		/// 星期几 1-7
		int GetWeekDay() const;

		string ToString( DateTimeFormat format ) const;
		string GetDateString( DateTimeFormat format ) const;
		string GetTimeString( DateTimeFormat format ) const;

	public:
		time_t  m_Time;
		tm*		m_LocalTime;
	};
}
