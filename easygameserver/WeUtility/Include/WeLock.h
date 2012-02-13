/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	Windows���߳���
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	// �ٽ���
	class We_EXPORTS WinLock
	{
	public:
		WinLock() { InitializeCriticalSection(&cs); }
		virtual ~WinLock() { DeleteCriticalSection(&cs); }

		inline void Lock()
		{
			EnterCriticalSection(&cs);
		}

		inline void Unlock()
		{
			LeaveCriticalSection(&cs);
		}

	protected:
		CRITICAL_SECTION cs;
	};

	// �ֲ��Զ���
	class We_EXPORTS AutoLock
	{
	protected:
		WinLock* m_Lock;
	public:
		AutoLock( WinLock* lock )
		{
			m_Lock = lock;
			if( m_Lock )
				m_Lock->Lock();
		}
		~AutoLock()
		{
			if( m_Lock )
				m_Lock->Unlock();
		}
	};
}
