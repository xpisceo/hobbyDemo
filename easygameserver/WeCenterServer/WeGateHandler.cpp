#include "WeGateHandler.h"
#include <stdio.h>
#include "WeCenterServer.h"
#include "WeCenter2ClientPackets_Inner.h"
#include "WeCenter2GatePackets_Inner.h"
#include "WeClient2CenterPackets_Inner.h"
#include "WeGate2CenterPackets_Inner.h"

namespace We
{
	GateHandler::GateHandler()
	{
		m_GateId = 0;
		m_GateStatus = GateStatus_WaitForInfo;
		m_CenterServer = 0;
	}
	GateHandler::~GateHandler()
	{
	}
	void GateHandler::OnProcessPacket( PacketHeader* packet )
	{
		/// 内部消息
		if( packet->Type1.m_MainType == 0 )
		{
			switch( packet->Type1.m_SubType )
			{
				/// 网关配置信息
			case PT_G2CS_Inner_GateInfo:
				m_CenterServer->OnRecv_GateInfo( this, packet );
				break;
			case PT_C2CS_Inner_Login:
				m_CenterServer->OnRecv_Login( this, packet );
				break;
			case PT_C2CS_Inner_CreateAccount:
				m_CenterServer->OnRecv_CreateAccount( this, packet );
				break;
			case PT_C2CS_Inner_RoleList:
				m_CenterServer->OnRecv_RoleList( this, packet );
				break;
			case PT_C2CS_Inner_CreateRole:
				m_CenterServer->OnRecv_CreateRole( this, packet );
				break;
			case PT_C2CS_Inner_ChooseRole:
				m_CenterServer->OnRecv_ChooseRole( this, packet );
				break;
			case PT_C2CS_Inner_DeleteRole:
				m_CenterServer->OnRecv_DeleteRole( this, packet );
				break;
			default:
				break;
			}
		}
		else
		{
		}
	}
	void GateHandler::OnDisconnect()
	{
	}
	void GateHandler::Update()
	{
	}
}