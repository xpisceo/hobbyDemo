#include "WeClientHandler.h"
#include <stdio.h>
#include "WeGateServer.h"
#include "WeClient2CenterPackets_Inner.h"
#include "WeLogInfo.h"

namespace We
{
	ClientHandler::ClientHandler()
	{
		m_Id = 0;
		m_ConnectTime = 0;
		m_GateServer = 0;
		Init();
	}
	ClientHandler::~ClientHandler()
	{
	}
	void ClientHandler::Init()
	{
		m_MapId = 0;
		m_ClientStatus = ClientStatus_AccountLogin;
		m_LoginFailCount = 0;
	}
	void ClientHandler::OnProcessPacket( PacketHeader* packet )
	{
		/// 发给中心服务器的
		if( packet->Type2.m_ServerType == 0 )
		{
			/// 非法数据包,应该断开该连接
			if( packet->m_Length < sizeof(Packet_C2CS_PacketHeader) )
			{
				return;
			}
			/// 帐号登录和创建帐号的消息需要附加网关id,便于回传消息
			if( packet->Type2.m_SubType == PT_C2CS_Inner_Login )
			{
				if( packet->m_Length != sizeof(Packet_C2CS_Login) )
				{
					return;
				}
				Packet_C2CS_Login* packetLogin = (Packet_C2CS_Login*)packet;
				packetLogin->m_GateId = m_GateServer->GetGateServerId();
			}
			else if( packet->Type2.m_SubType == PT_C2CS_Inner_CreateAccount )
			{
				if( packet->m_Length != sizeof(Packet_C2CS_CreateAccount) )
				{
					return;
				}
				Packet_C2CS_CreateAccount* packetCreateAccount = (Packet_C2CS_CreateAccount*)packet;
				packetCreateAccount->m_GateId = m_GateServer->GetGateServerId();
			}
			/// 可以在此检查packet的m_PlayerId是否等于m_Id,来排除非法数据包
			/// 但是要注意某些包本来就不等于!!!!!!
			/// 赋值m_PlayerId,不能相信客户端自己发过来的数据
			((Packet_C2CS_PacketHeader*)packet)->m_PlayerID = m_Id;
			/// 客户端发给中心服务器的内部消息
			m_GateServer->SendToCenter( packet );
		}
		/// 发给地图服务器的
		else
		{
			if( m_MapId == 0 )
			{
				LogInfo( "ClientHandler::OnProcessPacket m_MapId == 0 Id=%d", m_Id );
				return;
			}
			m_GateServer->SendToMap( m_MapId, packet );
		}
	}
	void ClientHandler::OnDisconnect()
	{
		LogInfo( "ClientHandler::OnDisconnect Id=%d", m_Id );
	}
	void ClientHandler::Update()
	{
	}
}