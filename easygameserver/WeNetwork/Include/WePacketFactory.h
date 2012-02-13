/********************************************************************
created:	2010-1-14
author:		Fish (于国平)
svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
purpose:	网络数据包工厂
*********************************************************************/
#pragma once

#include "WeSingleton.h"
#include "WePacketHeader.h"
#include "WeLock.h"
#include "WePacketPool.h"

namespace We
{
	enum PacketSize
	{
		PacketSize_16,
		PacketSize_32,
		PacketSize_64,
		PacketSize_128,
		PacketSize_256,
		PacketSize_1024,	// 1K	
		PacketSize_4096,	// 4K	
		PacketSize_8192,	// 8K
		PacketSize_16384,	// 16K
		PacketSize_32768,	// 32K
		PacketSize_Count,
	};

	const uint32 MAX_ALLOC_PACKET_SIZE = 1024*32;	// 最大数据包的大小

	class PacketFactory : public Singleton<PacketFactory>
	{
	protected:
		WinLock m_PacketsLock;
		uint16 m_PacketLengthArray[PacketSize_Count];

		PacketPool<10240>	m_PacketPool_16;
		PacketPool<10240>	m_PacketPool_32;
		PacketPool<10240>	m_PacketPool_64;
		PacketPool<10240>	m_PacketPool_128;
		PacketPool<10240>	m_PacketPool_256;
		PacketPool<10240>	m_PacketPool_1024;
		PacketPool<1024>	m_PacketPool_4096;
		PacketPool<512>		m_PacketPool_8192;
		PacketPool<256>		m_PacketPool_16384;
		PacketPool<256>		m_PacketPool_32768;
	public:
		PacketFactory();
		~PacketFactory();

		PacketHeader* AllocPacket( uint16 length );

		void FreePacket( PacketHeader* packet );
	};
}