/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�����(ObjectPool),���new��delete������,���ҿ��Է�ֹƵ����new��delete���µ��ڴ���Ƭ
	ע:         ����ObjectPool::Alloc�����Ķ�����Ҫ���г�ʼ����������Ϊû�е��øö���Ĺ��캯��
*********************************************************************/
#pragma once

#include <assert.h>

namespace We
{
	// Ԥ������ڴ�Ķ����
	template <class T>
	class SimpleObjectPool
	{
	protected:
		T* m_Pool;			/// ��������
		int m_PoolSize;		/// ��������Ĵ�С
		int m_FreeIndex;	/// �ɷ�����������
		T** m_FreePool;		/// �ɷ�������ָ������

	public:
		/// ��ô�ܰ�ObjectPool����ΪSimpleObjectPool����Ԫ����??????
		SimpleObjectPool* m_Prev;	/// ��һ���ڵ�
		SimpleObjectPool* m_Next;	/// ��һ���ڵ�
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

	// ��̬�����,����Ԥ�����ڴ�
	template <class T, unsigned int PoolSize>
	class DynamicObjectPool
	{
	protected:
		int m_FreeIndex;			/// �ɷ�����������
		T*	m_FreePool[PoolSize];	/// �ɷ�������ָ������

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

		// �ͷ����ж������Ķ���
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
		// �������,ע������û�е��ù��캯��
		T* Alloc()
		{
			// �������û�ж�����,��ֱ�ӷ���
			if( m_FreeIndex <= 0 )
			{
				return new T;
			}
			T* obj = m_FreePool[--m_FreeIndex];
			return obj;
		}
		// �ͷŶ���,���ܷ��ڶ������
		void Free( T* obj )
		{
			// ���������������,��ֱ���ͷ�
			if( m_FreeIndex >= PoolSize )
			{
				delete obj;
				return;
			}
			m_FreePool[m_FreeIndex++] = obj;
		}
	};


	/// �����������ܵĶ����
	/// ע:��Ȼ����������������ʵ��ʹ�õ�ʱ�򣬻���Ӧ�þ�����������
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
			/// ��һ�����䲻�ɹ����Һ���Ľڵ�
			SimpleObjectPool<T>* pPool = m_Head->m_Next;
			while( pPool )
			{
				if( pPool->GetFreeIndex() < pPool->GetPoolSize() )
				{
					return pPool->Alloc();
				}
				pPool = pPool->m_Next;
			}
			/// ������Զ�����,������ֱ����SimpleObjectPool
			if( m_AutoIncreaseSize < 1 )
			{
				assert( !"ObjectPool::Alloc" );
				return 0;
			}
			/// ����һ���½ڵ�
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
			/// ע:��һ�����ͷ���ԭ����SimpleObjectPool��
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