/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	线程安全的队列模板
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	template<class T, class LOCK>
	class LockQueue
	{
	protected:
		struct Node
		{
			T element;
			Node* next;
		};

		Node* first;	// 队头
		Node* last;		// 队尾
		// use DynamicObjectPool<Node>?
		LOCK m_lock;

	public:
		LockQueue()
		{
			last = 0;
			first = 0;
		}

		~LockQueue()
		{
			Clear();
		}

		void Clear()
		{
			while(last != 0)
				Pop();
		}

		void Push(T elem)
		{
			m_lock.Lock();
			Node* n = new Node;
			if(last)
				last->next = n;
			else
				first = n;

			last = n;
			n->next = 0;
			n->element = elem;
			m_lock.Unlock();
		}

		T Pop()
		{
			m_lock.Lock();
			if(first == 0)
			{
				m_lock.Unlock();
				return reinterpret_cast<T>(0);
			}

			T ret = first->element;
			Node* td = first;
			first = td->next;
			if(!first)
				last = 0;

			delete td;
			m_lock.Unlock();
			return ret;
		}

		T Front()
		{
			m_lock.Lock();
			if(first == 0)
			{
				m_lock.Unlock();
				return reinterpret_cast<T>(0);
			}

			T ret = first->element;
			m_lock.Unlock();
			return ret;
		}

		void PopFront()
		{
			m_lock.Lock();
			if(first == 0)
			{
				m_lock.Unlock();
				return;
			}

			Node* td = first;
			first = td->next;
			if(!first)
				last = 0;

			delete td;
			m_lock.Unlock();
		}

		void PopAll( vector<T>& allElements )
		{
			m_lock.Lock();
			while ( last )
			{
				if(first == 0)
				{
					m_lock.Unlock();
					return;
				}

				allElements.push_back( first->element );
				Node* td = first;
				first = td->next;
				if(!first)
					last = 0;

				delete td;
			}
			m_lock.Unlock();
		}

		bool HasItems()
		{
			bool ret;
			m_lock.Lock();
			ret = (first != 0);
			m_lock.Unlock();
			return ret;
		}
	};
}