#include "WeDbHandler.h"
#include <stdio.h>
#include <assert.h>
#include "WeGameServer.h"
#include "WeDb2GamePackets_Inner.h"

namespace We
{
	DbHandler::DbHandler()
	{
		m_Id = 0;
		m_GameServer = 0;
	}
	DbHandler::~DbHandler()
	{
	}
	void DbHandler::OnProcessPacket( PacketHeader* packet )
	{
		/// 内部消息
		if( packet->m_MainType == 0 )
		{
			switch( packet->m_SubType )
			{
			case PT_D2GS_Inner_RoleListResult:
				m_GameServer->OnRecv_RoleListResult( packet );
				break;
			case PT_D2GS_Inner_CreateRoleResult:
				m_GameServer->OnRecv_CreateRoleResult( packet );
				break;
			case PT_D2GS_Inner_ChooseRoleResult:
				m_GameServer->OnRecv_ChooseRoleResult( packet );
				break;
			case PT_D2GS_Inner_DeleteRoleResult:
				m_GameServer->OnRecv_DeleteRoleResult( packet );
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
	void DbHandler::OnDisconnect()
	{
	}
	void DbHandler::Update()
	{
	}
}