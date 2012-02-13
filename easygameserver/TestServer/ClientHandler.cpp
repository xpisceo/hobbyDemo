#include "ClientHandler.h"
#include "..\TestClient\Packet.h"
#include <stdio.h>
#include "TestServer.h"

namespace We
{
	ClientHandler::ClientHandler()
	{
		m_Id = 0;
		m_TestServer = 0;
	}
	ClientHandler::~ClientHandler()
	{
	}
	void ClientHandler::OnProcessPacket( PacketHeader* packet )
	{
		m_TestServer->m_TotalRecvMsgSizeSec += packet->m_Length;
		m_TestServer->m_TotalRecvMsgSize += packet->m_Length;
		//printf( "ClientHandler::OnProcessPacket :%d %d %d\n", m_Id, packet->m_Type, packet->m_Length );
		if( packet->m_MainType == PacketType_PING )
		{
			m_TestServer->m_TotalSendMsgSizeSec += packet->m_Length;
			m_TestServer->m_TotalSendMsgSize += packet->m_Length; 
			SendPacket( packet );
		}
		else if( packet->m_MainType == PacketType_Move )
		{
			m_TestServer->SendPacketAround( packet );
		}
		else if( packet->m_MainType == PacketType_Chat )
		{
			Packet_Chat* pckChat = (Packet_Chat*)packet;
			m_TestServer->SendPacketAround( packet );
		}
	}
	void ClientHandler::OnDisconnect()
	{
		printf( "ClientHandler::OnDisconnect Id=%d\n", m_Id );
	}
	void ClientHandler::Update()
	{
	}
}