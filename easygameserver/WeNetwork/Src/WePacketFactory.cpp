#include "WePacketFactory.h"

namespace We
{
	Singleton_Implement( PacketFactory );

	PacketFactory::PacketFactory()
	{
		m_PacketLengthArray[PacketSize_16] = 16;
		m_PacketLengthArray[PacketSize_32] = 32;
		m_PacketLengthArray[PacketSize_64] = 64;
		m_PacketLengthArray[PacketSize_128] = 128;
		m_PacketLengthArray[PacketSize_256] = 256;
		m_PacketLengthArray[PacketSize_1024] = 1024;
		m_PacketLengthArray[PacketSize_4096] = 4096;
		m_PacketLengthArray[PacketSize_8192] = 8192;
		m_PacketLengthArray[PacketSize_16384] = 16384;
		m_PacketLengthArray[PacketSize_32768] = 32768;

		m_PacketPool_16.Init( 16 );
		m_PacketPool_32.Init( 32 );
		m_PacketPool_64.Init( 64 );
		m_PacketPool_128.Init( 128 );
		m_PacketPool_256.Init( 256 );
		m_PacketPool_1024.Init( 1024 );
		m_PacketPool_4096.Init( 4096 );
		m_PacketPool_8192.Init( 8192 );
		m_PacketPool_16384.Init( 16384 );
		m_PacketPool_32768.Init( 32768 );
	}
	PacketFactory::~PacketFactory()
	{

	}
	PacketHeader* PacketFactory::AllocPacket( uint16 length )
	{
		if( length <= 16 )
			return m_PacketPool_16.AllocPacket();
		else if( length <= 32 )
			return m_PacketPool_32.AllocPacket();
		else if( length <= 64 )
			return m_PacketPool_64.AllocPacket();
		else if( length <= 128 )
			return m_PacketPool_128.AllocPacket();
		else if( length <= 256 )
			return m_PacketPool_256.AllocPacket();
		else if( length <= 1024 )
			return m_PacketPool_1024.AllocPacket();
		else if( length <= 4096 )
			return m_PacketPool_4096.AllocPacket();
		else if( length <= 8192 )
			return m_PacketPool_8192.AllocPacket();
		else if( length <= 16384 )
			return m_PacketPool_16384.AllocPacket();
		else if( length <= 32768 )
			return m_PacketPool_32768.AllocPacket();
		else if( length <= MAX_PACKET_SIZE )
			return (PacketHeader*)( new uint8[length] );
		//assert( !"PacketFactory::AllocPacket ERROR: too long!" );
		return 0;
	}
	void PacketFactory::FreePacket( PacketHeader* packet )
	{
		assert( packet );
		if( packet->m_Length <= 16 )
			return m_PacketPool_16.FreePacket( packet );
		else if( packet->m_Length <= 32 )
			return m_PacketPool_32.FreePacket( packet );
		else if( packet->m_Length <= 64 )
			return m_PacketPool_64.FreePacket( packet );
		else if( packet->m_Length <= 128 )
			return m_PacketPool_128.FreePacket( packet );
		else if( packet->m_Length <= 256 )
			return m_PacketPool_256.FreePacket( packet );
		else if( packet->m_Length <= 1024 )
			return m_PacketPool_1024.FreePacket( packet );
		else if( packet->m_Length <= 4096 )
			return m_PacketPool_4096.FreePacket( packet );
		else if( packet->m_Length <= 8192 )
			return m_PacketPool_8192.FreePacket( packet );
		else if( packet->m_Length <= 16384 )
			return m_PacketPool_16384.FreePacket( packet );
		else if( packet->m_Length <= 32768 )
			return m_PacketPool_32768.FreePacket( packet );
		else if( packet->m_Length <= MAX_PACKET_SIZE )
			delete packet;
		//assert( !"PacketFactory::FreePacket ERROR: too long!" );
	}
}