#include "WeCircularBuffer.h"
#include <stdlib.h>
#include <string.h>

namespace We
{
	/** Constructor
	*/
	CircularBuffer::CircularBuffer()
	{
		m_buffer = m_bufferEnd = m_regionAPointer = m_regionBPointer = NULL;
		m_regionASize = m_regionBSize = 0;
	}

	/** Destructor
	*/
	CircularBuffer::~CircularBuffer()
	{
		free(m_buffer);
	}

	/** Read bytes from the buffer
	* @param destination pointer to destination where bytes will be written
	* @param bytes number of bytes to read
	* @return true if there was enough data, false otherwise
	*/
	bool CircularBuffer::Read(void * destination, size_t bytes)
	{
		if (m_buffer == NULL)
			return false;

		// copy as much out of region a
		// ���ж��ܴ�С������
		size_t cnt = bytes;
		size_t aRead = 0, bRead = 0;
		if( (m_regionASize + m_regionBSize) < bytes )
			return false;

		// If we have both region A and region B, always "finish" off region A first, as
		// this will contain the "oldest" data
		// �ȿ���A������
		if( m_regionASize > 0 )
		{
			aRead = (cnt > m_regionASize) ? m_regionASize : cnt;
			memcpy(destination, m_regionAPointer, aRead);
			m_regionASize -= aRead;
			m_regionAPointer += aRead;
			cnt -= aRead;
		}

		// Data left over? read the data from buffer B
		// �ٿ���B������
		if( cnt > 0 && m_regionBSize > 0 )
		{
			bRead = (cnt > m_regionBSize) ? m_regionBSize : cnt;
			memcpy((char*)destination + aRead, m_regionBPointer, bRead);
			m_regionBSize -= bRead;
			m_regionBPointer += bRead;
			cnt -= bRead;
		}

		// is buffer A empty? move buffer B to buffer A, to increase future performance
		if( m_regionASize == 0 )
		{
			if( m_regionBSize > 0 )
			{
				// push it all to the start of the buffer.
				if( m_regionBPointer != m_buffer )
					memmove(m_buffer, m_regionBPointer, m_regionBSize);

				m_regionAPointer = m_buffer;
				m_regionASize = m_regionBSize;
				m_regionBPointer = NULL;
				m_regionBSize = 0;
			}
			else
			{
				// no data in region b
				m_regionBPointer = NULL;
				m_regionBSize = 0;
				m_regionAPointer = m_buffer;
				m_regionASize = 0;
			}
		}

		return true;
	}

	void CircularBuffer::AllocateB()
	{
		//printf("[allocating B]\n");
		m_regionBPointer = m_buffer;
	}

	/** Write bytes to the buffer
	* @param data pointer to the data to be written
	* @param bytes number of bytes to be written
	* @return true if was successful, otherwise false
	*/
	bool CircularBuffer::Write(const void * data, size_t bytes)
	{
		if (m_buffer == NULL)
			return false;

		// If buffer B exists, write to it.
		if( m_regionBPointer != NULL )
		{
			if( GetBFreeSpace() < bytes )
				return false;

			memcpy(&m_regionBPointer[m_regionBSize], data, bytes);
			m_regionBSize += bytes;
			return true;
		}

		// Otherwise, write to buffer A, or initialize buffer B depending on which has more space.
		// ���Aǰ��Ŀռ����,��д��ǰ��Ŀռ�
		/*
			 ___________________________________________________
			|                          |         |              |
			|                          |    A    |              |
			|_________________________ |_________|______________|
			|<-------SpaceBeforeA----->|
			                                     |<-AFreeSpace->|
		*/
		if( GetAFreeSpace() < GetSpaceBeforeA() )
		{
			// ���Aǰ��Ŀռ����,���ǰ����ΪB
			/*
				|<-------BFreeSpace------->|
				 ____________________________________________________
				|                          |         |               |
				|B                         |    A    |               |
				|__________________________|_________|_______________|
			*/
			AllocateB();
			if( GetBFreeSpace() < bytes )
				return false;

			memcpy(&m_regionBPointer[m_regionBSize], data, bytes);
			m_regionBSize += bytes;
			return true;
		}
		else
		{
			if( GetAFreeSpace() < bytes )
				return false;

			memcpy(&m_regionAPointer[m_regionASize], data, bytes);
			m_regionASize += bytes;
			return true;
		}
	}

	/** Returns the number of available bytes left.
	*/
	size_t CircularBuffer::GetSpace()
	{
		if( m_regionBPointer != NULL )
			return GetBFreeSpace();
		else
		{
			// would allocating buffer B get us more data?
			if( GetAFreeSpace() < GetSpaceBeforeA() )
			{
				AllocateB();
				return GetBFreeSpace();
			}

			// or not?
			return GetAFreeSpace();
		}
	}

	/** Returns the number of bytes currently stored in the buffer.
	�洢��ȫ�����ݵĴ�С
	*/
	size_t CircularBuffer::GetSize()
	{
		return m_regionASize + m_regionBSize;
	}

	/** Returns the number of contiguous bytes (that can be pushed out in one operation)
	һ�ο���ȡ��������,��һ����ȫ������,��ȡA
	*/
	size_t CircularBuffer::GetContiguiousBytes()
	{
		if( m_regionASize )			// A before B
			return m_regionASize;
		else
			return m_regionBSize;
	}

	/** Removes len bytes from the front of the buffer
	* @param len the number of bytes to "cut"
	*/
	void CircularBuffer::Remove(size_t len)
	{
		// remove from A first before we remove from b
		size_t cnt = len;
		size_t aRem, bRem;

		// If we have both region A and region B, always "finish" off region A first, as
		// this will contain the "oldest" data
		// �ȴ�A�Ƴ�
		if( m_regionASize > 0 )
		{
			// removeǰ
			/*
							|<-BFreeSpace->|<-----ASize---->|
				 ___________________________________________________________
				|    |      |              |				|				|
				|    |   B  |              |    A			|				|
				|____|______|______________|________________|_______________|
				|<-------SpaceBeforeA----->|
													        |<-AFreeSpace-->|
			*/
			// remove��(len<=m_regionASize)
			/*
							|<----BFreeSpace--->|<--ASize-->|
				 ___________________________________________________________
				|    |      |                   |			|				|
				|    |   B  |                   |    A		|				|
				|____|______|___________________|___________|_______________|
				|<---------SpaceBeforeA-------->|
															|<-AFreeSpace-->|
			*/
			// remove��(len>m_regionASize)
			/*
							|<----------BFreeSpace--------->|
				 ___________________________________________________________
				|    |      |                    			|				|
				|    |   B  |                         		|				|
				|____|______|___________________ ___________|_______________|
				|<---------------SpaceBeforeA-------------->|
															|<-AFreeSpace-->|
			*/
			aRem = (cnt > m_regionASize) ? m_regionASize : cnt;
			m_regionASize -= aRem;
			m_regionAPointer += aRem;	// Aǰ��
			cnt -= aRem;
		}

		// Data left over? cut the data from buffer B
		// A������,����B
		if( cnt > 0 && m_regionBSize > 0 )
		{
			bRem = (cnt > m_regionBSize) ? m_regionBSize : cnt;
			m_regionBSize -= bRem;
			m_regionBPointer += bRem; // Bǰ��
			cnt -= bRem;
		}

		// is buffer A empty? move buffer B to buffer A, to increase future performance
		// ���A������
		if( m_regionASize == 0 )
		{
			// B��������
			if( m_regionBSize > 0 )
			{
				// push it all to the start of the buffer.
				// �Ȱ�B�Ƶ���ǰ��
				if( m_regionBPointer != m_buffer )
					memmove(m_buffer, m_regionBPointer, m_regionBSize);

				// �ٰ�A������ǰ��
				m_regionAPointer = m_buffer;
				m_regionASize = m_regionBSize;
				m_regionBPointer = NULL;
				m_regionBSize = 0;
			}
			else
			{
				// no data in region b
				// ����������������,��A�ŵ���ǰ��
				m_regionBPointer = NULL;
				m_regionBSize = 0;
				m_regionAPointer = m_buffer;
				m_regionASize = 0;
			}
		}
	}

	/** Returns a pointer at the "end" of the buffer, where new data can be written
	д����ʱ,���ж�B,��ΪB�Ǻ�д���
	*/
	void * CircularBuffer::GetBuffer()
	{
		if( m_regionBPointer != NULL )
			return m_regionBPointer + m_regionBSize;
		else
			return m_regionAPointer + m_regionASize;
	}

	/** Allocate the buffer with room for size bytes
	* @param size the number of bytes to allocate
	*/
	void CircularBuffer::Allocate(size_t size)
	{
		m_buffer = (uint8*)malloc(size);
		m_bufferEnd = m_buffer + size;
		m_regionAPointer = m_buffer;		// reset A to the start
	}
	void CircularBuffer::Reset()
	{
		m_regionAPointer = m_buffer;
		m_regionBPointer = 0;
		m_regionASize = m_regionBSize = 0;
	}

	/** Increments the "writen" pointer forward len bytes
	* @param len number of bytes to step
	*/
	void CircularBuffer::IncrementWritten(size_t len)			// known as "commit"
	{
		if( m_regionBPointer != NULL )
			m_regionBSize += len;
		else
			m_regionASize += len;
	}

	/** Returns a pointer at the "beginning" of the buffer, where data can be pulled from
	����Ŀ�ʼ��ַ,ȡ���ݵ�ʱ��,��ȡA,��ΪA���ȱ�д���
	*/
	void * CircularBuffer::GetBufferStart()
	{
		if( m_regionASize > 0 )
			return m_regionAPointer;
		else
			return m_regionBPointer;

	}

}