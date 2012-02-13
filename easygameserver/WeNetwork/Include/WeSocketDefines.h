/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	网络事件的定义
*********************************************************************/
#pragma once

#include <assert.h>

/// IOCP的事件
enum SocketIOEvent
{
	SOCKET_IO_EVENT_RECV_COMPLETE   = 0,
	SOCKET_IO_EVENT_SEND_END		= 1,
	SOCKET_IO_THREAD_SHUTDOWN		= 2,
	NUM_SOCKET_IO_EVENTS			= 3,
};

class OverlappedStruct
{
public:
	OVERLAPPED m_overlap;
	SocketIOEvent m_event;
	volatile long m_inUse;
	OverlappedStruct(SocketIOEvent ev) : m_event(ev)
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
		m_inUse = 0;
	};

	OverlappedStruct()
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
		m_inUse = 0;
	}

	__forceinline void Reset(SocketIOEvent ev)
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
		m_event = ev;
	}

	void Mark()
	{
		long val = InterlockedCompareExchange(&m_inUse, 1, 0);
		if(val != 0)
		{
			if( m_event == SOCKET_IO_EVENT_RECV_COMPLETE )
				assert("!!!! Network: Detected double use of read/write event! Previous event was SOCKET_IO_EVENT_RECV_COMPLETE.\n");
			else if( m_event == SOCKET_IO_EVENT_SEND_END )
				assert("!!!! Network: Detected double use of read/write event! Previous event was SOCKET_IO_EVENT_SEND_END.\n");
			else if( m_event == SOCKET_IO_THREAD_SHUTDOWN )
				assert("!!!! Network: Detected double use of read/write event! Previous event was SOCKET_IO_THREAD_SHUTDOWN.\n");
		}
	}

	void Unmark()
	{
		InterlockedExchange(&m_inUse, 0);
	}
};