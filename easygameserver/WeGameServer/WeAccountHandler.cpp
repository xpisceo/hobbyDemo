#include "WeAccountHandler.h"
#include <stdio.h>
#include <assert.h>
#include "WeGameServer.h"
#include "WeGame2ClientPackets_Inner.h"
#include "WeAccount2GamePackets_Inner.h"

namespace We
{
	AccountHandler::AccountHandler()
	{
		m_Id = 0;
		m_GameServer = 0;
	}
	AccountHandler::~AccountHandler()
	{
	}
	void AccountHandler::OnProcessPacket( PacketHeader* packet )
	{
		/// 内部消息
		if( packet->m_MainType == 0 )
		{
			switch( packet->m_SubType )
			{
			case PT_A2GS_Inner_LoginResult:
				m_GameServer->OnRecv_LoginResult( packet );
				break;
			case PT_A2GS_Inner_CreateAccountResult:
				m_GameServer->OnRecv_CreateAccountResult( packet );
				break;
			default:
				break;
			}
		}
		/// 逻辑消息
		else
		{
		}
	}
	void AccountHandler::OnDisconnect()
	{
	}
	void AccountHandler::Update()
	{
	}
}