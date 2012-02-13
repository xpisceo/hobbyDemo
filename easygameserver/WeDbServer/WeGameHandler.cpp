#include "WeGameHandler.h"
#include <stdio.h>
#include "WeDbServer.h"
#include "WeDb2GamePackets_Inner.h"
#include "WeGame2DbPackets_Inner.h"

namespace We
{
	GameHandler::GameHandler()
	{
		m_GameServerId = 0;
		m_GameServerStatus = GameServerStatus_WaitForInfo;
		m_DbServer = 0;
	}
	GameHandler::~GameHandler()
	{
	}
	void GameHandler::OnProcessPacket( PacketHeader* packet )
	{
		/// 内部消息
		if( packet->m_MainType == 0 )
		{
			switch( packet->m_SubType )
			{
				/// 中心服务器的配置信息
			case PT_GS2D_Inner_GameServerInfo:
				m_DbServer->OnAddGameServer( this, packet );
				break;
			default:
				m_DbServer->PushQueryBuffer( m_GameServerId, packet );
				break;
			}
		}
		else
		{
			m_DbServer->PushQueryBuffer( m_GameServerId, packet );
		}
	}
	void GameHandler::OnDisconnect()
	{
	}
	void GameHandler::Update()
	{
	}
}