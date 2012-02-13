#include "WeGameHandler.h"
#include <stdio.h>
#include "WeAccountServer.h"
#include "WeAccount2GamePackets_Inner.h"
#include "WeGame2AccountPackets_Inner.h"

namespace We
{
	GameHandler::GameHandler()
	{
		m_GameServerId = 0;
		m_GameServerStatus = GameServerStatus_WaitForInfo;
		m_AccountServer = 0;
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
				/// 游戏服务器的配置信息
			case PT_GS2A_Inner_GameServerInfo:
				m_AccountServer->OnAddGameServer( this, packet );
				break;
			default:
				m_AccountServer->PushQueryBuffer( m_GameServerId, packet );
				break;
			}
		}
		else
		{
			m_AccountServer->PushQueryBuffer( m_GameServerId, packet );
		}
	}
	void GameHandler::OnDisconnect()
	{
	}
	void GameHandler::Update()
	{
	}
}