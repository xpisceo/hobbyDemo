#include "WeDbHandler.h"
#include <stdio.h>
#include <assert.h>
#include "WeCenterServer.h"
#include "WeCenter2ClientPackets_Inner.h"
#include "WeCenter2GatePackets_Inner.h"
#include "WeAccount2CenterPackets_Inner.h"

namespace We
{
	DbHandler::DbHandler()
	{
		m_Id = 0;
		m_CenterServer = 0;
	}
	DbHandler::~DbHandler()
	{
	}
	void DbHandler::OnProcessPacket( PacketHeader* packet )
	{
		if( packet->m_Length < sizeof(Packet_CS2C_PacketHeader) )
		{
			assert( false );
			return;
		}
		/// 内部消息
		if( packet->Type1.m_MainType == 0 )
		{
			switch( packet->Type1.m_SubType )
			{
			case PT_CS2C_Inner_RoleListResult:
				m_CenterServer->OnRecv_RoleListResult( packet );
				break;
			case PT_CS2C_Inner_CreateRoleResult:
				m_CenterServer->OnRecv_CreateRoleResult( packet );
				break;
			case PT_CS2C_Inner_ChooseRoleResult:
				m_CenterServer->OnRecv_ChooseRoleResult( packet );
				break;
			case PT_CS2C_Inner_DeleteRoleResult:
				m_CenterServer->OnRecv_DeleteRoleResult( packet );
				break;
			case PT_CS2C_Inner_EnterWorld:
				m_CenterServer->OnRecv_EnterWorld( packet );
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