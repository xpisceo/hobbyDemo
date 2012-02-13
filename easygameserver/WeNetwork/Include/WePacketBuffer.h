/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	环形内存,是连续的,用于收包
				尾部留一些空白数据,以避免一些内存溢出问题
*********************************************************************/
#pragma once

namespace We
{
	class PacketBuffer
	{
	protected:
		unsigned char*	m_Buffer;		/// 缓存
		unsigned int	m_Size;			/// 缓存大小
		unsigned int	m_DataSize;		/// 已写入的数据(头部和尾部的和)
		unsigned char*  m_BufferTail;

		unsigned char*  m_ReadPointer;
		unsigned char*  m_WritePointer;
		unsigned int	m_TmpReadPointerOffset;
	public:
		PacketBuffer();
		~PacketBuffer();

		inline unsigned int GetSize() const { return m_Size; }
		inline unsigned int GetDataSize() const { return m_DataSize; }
		unsigned int GetTailDataSize() const;
		inline unsigned int GetTmpReadPointerOffset() const { return m_TmpReadPointerOffset; }
		inline void SetTmpReadPointerOffset( unsigned int tmpReadPointerOffset ) { m_TmpReadPointerOffset=tmpReadPointerOffset; }

		void Alloc( unsigned int size );

		unsigned int GetWriteSpace();
		unsigned char* GetWritePointer() { return m_WritePointer; }
		void MoveWritePointer( unsigned int len );

		unsigned char* GetReadPointer() { return m_ReadPointer; }
		void MoveReadPointer( unsigned int len );

		bool Read( unsigned char* out, unsigned int len );

		inline void Reset() { m_DataSize=0;m_ReadPointer=m_WritePointer=m_Buffer; m_TmpReadPointerOffset=0;}
	};
}