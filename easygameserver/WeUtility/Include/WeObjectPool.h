/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	对象池(ObjectPool),提高new和delete的性能,而且可以防止频繁的new和delete导致的内存碎片
	注:         调用ObjectPool::Alloc出来的对象，需要进行初始化操作，因为没有调用该对象的构造函数
*********************************************************************/
#pragma once

#include <assert.h>

namespace We
{
	// 预分配好内存的对象池
	template <class T>
	class SimpleObjectPool
	{
	protected:
		T* m_Pool;			/// 对象数组
		int m_PoolSize;		/// 对象数组的大小
		int m_FreeIndex;	/// 可分配对象的索引
		T** m_FreePool;		/// 可分配对象的指针数组

	public:
		/// 怎么能把ObjectPool设置为SimpleObjectPool的友元类呢??????
		SimpleObjectPool* m_Prev;	/// 上一个节点
		SimpleObjectPool* m_Next;	/// 下一个节点
	public:
		SimpleObjectPool()
		{
			m_Pool = 0;
			m_PoolSize = 0;
			m_FreeIndex = 0;
			m_FreePool = 0;
			m_Prev = 0;
			m_Next = 0;
		}
		~SimpleObjectPool()
		{
			Clear();
		}

		int GetPoolSize() const { return m_PoolSize; }
		int GetFreeIndex() const { return m_FreeIndex; }
		SimpleObjectPool* GetPrev() { return m_Prev; }
		SimpleObjectPool* GetNext() { return m_Next; }

		void Init( int poolSize )
		{
			assert( m_Pool == 0 );
			assert( poolSize > 0 );
			assert( m_FreePool == 0 );
			m_PoolSize = poolSize;
			m_Pool = new T[m_PoolSize];
			assert( m_Pool );
			m_FreeIndex = 0;
			m_FreePool = new T*[m_PoolSize];
			assert( m_FreePool );
			for( int i=0; i<m_PoolSize; ++i )
				m_FreePool[i] = &m_Pool[i];
		}
		void Clear()
		{
			if( m_Pool != 0 )
			{
				delete []m_Pool;
				m_Pool = 0;
			}
			if( m_FreePool != 0 )
			{
				delete []m_FreePool;
				m_FreePool = 0;
			}
			m_PoolSize = 0;
			m_FreeIndex = 0;
		}
		T* Alloc()
		{
			assert( m_FreeIndex < m_PoolSize );
			if( m_FreeIndex >= m_PoolSize )
				return 0;
			T* newObj = m_FreePool[m_FreeIndex++];
			return newObj;
		}
		void Free( T* obj )
		{
			assert( m_FreeIndex > 0 );
			if( m_FreeIndex <= 0 )
				return;
			m_FreePool[--m_FreeIndex] = obj;
		}
	};

	// 动态对象池,不会预分配内存
	template <class T, unsigned int PoolSize>
	class DynamicObjectPool
	{
	protected:
		int m_FreeIndex;			/// 可分配对象的索引
		T*	m_FreePool[PoolSize];	/// 可分配对象的指针数组

	public:
		DynamicObjectPool()
		{
			m_FreeIndex = 0;
			for( int i=0; i<PoolSize; ++i )
				m_FreePool[i] = 0;
		}
		~DynamicObjectPool()
		{
			Clear();
		}

		unsigned int GetPoolSize() const { return PoolSize; }
		int GetFreeIndex() const { return m_FreeIndex; }

		// 释放所有对象池里的对象
		void Clear()
		{
			for( int i=0; i<PoolSize; ++i )
			{
				if( m_FreePool[i] )
				{
					delete m_FreePool[i];
					m_FreePool[i] = 0;
				}
			}
			m_FreeIndex = 0;
		}
		// 申请对象,注意这里没有调用构造函数
		T* Alloc()
		{
			// 对象池里没有对象了,则直接分配
			if( m_FreeIndex <= 0 )
			{
				return new T;
			}
			T* obj = m_FreePool[--m_FreeIndex];
			return obj;
		}
		// 释放对象,可能放在对象池里
		void Free( T* obj )
		{
			// 超过对象池容量了,则直接释放
			if( m_FreeIndex >= PoolSize )
			{
				delete obj;
				return;
			}
			m_FreePool[m_FreeIndex++] = obj;
		}
	};


	/// 带自增长功能的对象池
	/// 注:虽然可以自增长，但是实际使用的时候，还是应该尽量避免增长
	template <class T>
	class ObjectPool
	{
	protected:
		SimpleObjectPool<T>* m_Head;
		SimpleObjectPool<T>* m_Tail;
		int m_BasePoolSize;
		int m_AutoIncreaseSize;

	public:
		ObjectPool()
		{
			m_Head = 0;
			m_Tail = 0;
			m_BasePoolSize = 0;
			m_AutoIncreaseSize = 0;
		}
		~ObjectPool()
		{
			Clear();
		}
		void Init( int basePoolSize, int autoIncreaseSize )
		{
			assert( m_Head == 0 );
			m_Head = new SimpleObjectPool<T>;
			assert( m_Head );
			m_Head->Init( basePoolSize );
			m_Tail = m_Head;
			m_BasePoolSize = basePoolSize;
			m_AutoIncreaseSize = autoIncreaseSize;
		}
		void Clear()
		{
			SimpleObjectPool<T>* pool = m_Head;
			while( pool )
			{
				pool = pool->m_Next;
			}
		}
		T* Alloc()
		{
			if( m_Head->GetFreeIndex() < m_Head->GetPoolSize() )
			{
				return m_Head->Alloc();
			}
			/// 第一个分配不成功，找后面的节点
			SimpleObjectPool<T>* pPool = m_Head->m_Next;
			while( pPool )
			{
				if( pPool->GetFreeIndex() < pPool->GetPoolSize() )
				{
					return pPool->Alloc();
				}
				pPool = pPool->m_Next;
			}
			/// 如果不自动增长,还不如直接用SimpleObjectPool
			if( m_AutoIncreaseSize < 1 )
			{
				assert( !"ObjectPool::Alloc" );
				return 0;
			}
			/// 创建一个新节点
			SimpleObjectPool<T>* newPool = new SimpleObjectPool<T>;
			assert( newPool );
			newPool->Init( m_AutoIncreaseSize );
			m_Tail->m_Next = newPool;
			newPool->m_Prev = m_Tail;
			newPool->m_Next = 0;
			m_Tail = newPool;
			T* newObj = newPool->Alloc();
			assert( newObj );
			return newObj;
		}
		void Free( T* obj )
		{
			/// 注:不一定能释放在原来的SimpleObjectPool里
			if( m_Head->GetFreeIndex() > 0 )
			{
				m_Head->Free( obj );
				return;
			}
			SimpleObjectPool<T>* pPool = m_Head->m_Next;
			while( pPool )
			{
				if( pPool->GetFreeIndex() > 0 )
				{
					pPool->Free( obj );
					return;
				}
				pPool = pPool->m_Next;
			}
			assert( !"ObjectPool::Free" );
		}
	};
}