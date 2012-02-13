#include "TestClient.h"
#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include "Packet.h"

namespace We
{
	TestClient::TestClient()
	{
		m_LastPingTick = ::GetTickCount();
		m_LastMoveTick = m_LastPingTick;
		m_LastChatTick = m_LastPingTick;
		m_Id = 0;
	}
	TestClient::~TestClient()
	{
	}
	void TestClient::OnProcessPacket( PacketHeader* packet )
	{
		if( m_Id > 1 )
			return;
		if( packet->m_MainType == PacketType_PING )
		{
			Packet_PING* pakPing = (Packet_PING*)packet;
			printf( "PING=%d\n", ::GetTickCount()-pakPing->m_Tick );
		}
		else if( packet->m_MainType == PacketType_Move )
		{
		}
		else if( packet->m_MainType == PacketType_Chat )
		{
			Packet_Chat* pckChat = (Packet_Chat*)packet;
			//if( pckChat->m_Content[0] == '1' )
			//	printf( "Chat=1 %d\n", m_Id );
			//else
			//	printf( "Chat=0 %d\n", m_Id );
				printf( "Chat=%s\n", pckChat->m_Content );
		}
		else
		{
			assert( false );
		}
	}
	void TestClient::OnDisconnect()
	{
		printf( "TestClient::OnDisconnect %d\n", m_Id );
	}
	void TestClient::Update()
	{
		unsigned int tick = ::GetTickCount();
		if( tick - m_LastPingTick > 1000 )
		{
			m_LastPingTick = tick;
			Packet_PING pakPing;
			pakPing.m_Tick = tick;
			SendPacket( &pakPing );
		}
		if( tick - m_LastMoveTick > 1 )
		{
			m_LastMoveTick = tick;
			Packet_Move pakMove;
			pakMove.m_x = 0;
			pakMove.m_y = 0;
			pakMove.m_z = 0;
			SendPacket( &pakMove );
		}
		if( tick - m_LastChatTick > 1000 )
		{
			m_LastChatTick = tick;
			Packet_Chat pakMove;
			pakMove.SetString( "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" );
			//::memset( pakMove.m_Content, 0, sizeof(pakMove.m_Content) );
			SendPacket( &pakMove );
		}
	}
}