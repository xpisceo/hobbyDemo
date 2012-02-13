#include "WePacketBuffer.h"
#include <assert.h>
#include <string.h>

namespace We
{
	PacketBuffer::PacketBuffer()
	{
		m_Buffer = 0;
		m_Size = 0;
		m_DataSize = 0;
		m_ReadPointer = m_WritePointer = 0;
		m_TmpReadPointerOffset = 0;
	}
	PacketBuffer::~PacketBuffer()
	{
		if( m_Buffer != 0 )
		{
			delete []m_Buffer;
			m_Buffer = 0;
		}
	}
	void PacketBuffer::Alloc( unsigned int size )
	{
		assert( m_Buffer == 0 );
		assert( size > 0 );
		m_Buffer = new unsigned char[size];
		m_Size = size;
		m_DataSize = 0;
		m_ReadPointer = m_WritePointer = m_Buffer;
		m_BufferTail = m_Buffer + size;
	}
	unsigned int PacketBuffer::GetWriteSpace()
	{
		/// β��
		if( m_WritePointer - m_ReadPointer >= 0 )
		{
			if( m_WritePointer == m_ReadPointer && m_DataSize > 0 )
				return 0;
			assert( (unsigned int)(m_BufferTail - m_WritePointer) + m_DataSize <= m_Size );
			return (unsigned int)(m_BufferTail - m_WritePointer);
		}
		/// ͷ��
		else
		{
			if( m_ReadPointer - m_WritePointer <= 1 )
				return 0;
			assert( (unsigned int)(m_ReadPointer - m_WritePointer - 1) + m_DataSize <= m_Size );
			return (unsigned int)(m_ReadPointer - m_WritePointer - 1);
		}
	}
	void PacketBuffer::MoveWritePointer( unsigned int len )
	{
		m_WritePointer += len;
		m_DataSize += len;
		assert( m_DataSize <= m_Size );
		assert( m_WritePointer <= m_BufferTail );
		/// �������,�Ƶ���ͷ
		if( m_WritePointer == m_BufferTail )
			m_WritePointer = m_Buffer;
	}

	unsigned int PacketBuffer::GetTailDataSize() const
	{
		if( m_WritePointer - m_ReadPointer < 0 )
		{
			return (unsigned int)(m_BufferTail - m_ReadPointer);
		}
		return m_DataSize;
	}

	void PacketBuffer::MoveReadPointer( unsigned int len )
	{
		assert( m_DataSize >= len );
		m_DataSize -= len;
		/// β��
		if( m_WritePointer - m_ReadPointer > 0 )
		{
			assert( len <= (unsigned int)(m_WritePointer - m_ReadPointer) );
			m_ReadPointer += len;
		}
		else
		{
			unsigned int tailDataSize = (unsigned int)(m_BufferTail - m_ReadPointer);
			if( tailDataSize > len )
			{
				m_ReadPointer += len;
			}
			else
			{
				/// �Ƶ�ͷ��
				m_ReadPointer = m_Buffer + (len-tailDataSize);
			}
		}
	}

	bool PacketBuffer::Read( unsigned char* out, unsigned int len )
	{
		/// ������������
		if( m_ReadPointer - m_WritePointer > 0 )
		{
			unsigned int tailSize = (unsigned int)(m_BufferTail - m_ReadPointer);
			if( tailSize >= len )
			{
				::memcpy( out, m_ReadPointer, len );
				return true;
			}
			else
			{
				unsigned int headSize = (unsigned int)(m_WritePointer - m_Buffer);
				if( tailSize + headSize < len )
					return false;
				if( tailSize > 0 )
					::memcpy( out, m_ReadPointer, tailSize );
				::memcpy( out+tailSize, m_Buffer, len-tailSize );
				return true;
			}
		}
		/// ����������
		else
		{
			if( m_DataSize < len )
				return false;
			::memcpy( out, m_ReadPointer, len );
			return true;
		}
		return true;
	}
}