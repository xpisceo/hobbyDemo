/********************************************************************
created:	2010-1-14
author:		Fish (于国平)
svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
purpose:	网络数据包对象池
*********************************************************************/
#pragma once

#include "WeSingleton.h"
#include "WePacketHeader.h"
#include "WeLock.h"
#include "WeObjectPool.h"

namespace We
{
	template <unsigned int PoolSize>
	class PacketPool
	{
	protected:
		WinLock m_PacketsLock;
		DynamicObjectPool<PacketHeader,PoolSize> m_Packets;
		uint16	m_PacketLength;
	public:
		PacketPool()
		{
			m_PacketLength = 0;
		}

		void Init( uint16 packetLength )
		{
			AutoLock autoLock( &m_PacketsLock );
			m_PacketLength = packetLength;
		}

		PacketHeader* AllocPacket()
		{
			assert( m_PacketLength > 0 );
			AutoLock autoLock( &m_PacketsLock );
			if( m_Packets.GetFreeIndex() <= 0 )
				return (PacketHeader*)( new uint8[m_PacketLength] );
			return m_Packets.Alloc();
		}

		void FreePacket( PacketHeader* packet )
		{
			AutoLock autoLock( &m_PacketsLock );
			m_Packets.Free( packet );
		}
	};
}