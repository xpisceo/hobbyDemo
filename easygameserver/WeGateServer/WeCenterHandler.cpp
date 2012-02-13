#include "WeClientHandler.h"
#include <stdio.h>
#include <assert.h>
#include "WeGateServer.h"
#include "WeCenter2ClientPackets_Inner.h"
#include "WeCenter2GatePackets_Inner.h"

namespace We
{
	CenterHandler::CenterHandler()
	{
		m_Id = 0;
		m_GateServer = 0;
	}
	CenterHandler::~CenterHandler()
	{
	}
	void CenterHandler::OnProcessPacket( PacketHeader* packet )
	{
		if( packet->m_Length < sizeof(Packet_CS2C_PacketHeader) )
		{
			assert( false );
			return;
		}
		/// 内部消息
		if( packet->Type1.m_MainType == 0 )
		{
			assert( packet->Type1.m_SubType < PT_CS2G_Inner_MAX );
			if( packet->Type1.m_SubType >= PT_CS2G_Inner_MAX )
				return;
			switch( packet->Type1.m_SubType )
			{
				/// 新增地图服务器
			case PT_CS2G_Inner_AddMapServer:
				m_GateServer->OnAddMapServer( packet );
				break;
				/// 移除地图服务器
			case PT_CS2G_Inner_RemoveMapServer:
				m_GateServer->OnRemoveMapServer( packet );
				break;
				/// 登录结果返回
			case PT_CS2C_Inner_LoginResult:
				m_GateServer->OnClientLogin( packet );
				break;
			//case PT_CS2C_Inner_ChooseRoleData:
			//	m_GateServer->OnClientChooseRole( packet );
			//	break;
				/// 进入游戏世界(进入了地图)
			case PT_CS2C_Inner_EnterWorld:
				m_GateServer->OnClientEnterWorld( packet );
				break;
			default:
				{
					if( packet->Type1.m_SubType < PT_CS2C_Inner_EnterWorld )
						m_GateServer->DispatchLoginPacket( packet );
					else
						m_GateServer->DispatchPacket( packet );
				}
				break;
			}
		}
		/// 逻辑消息,转发给相应的客户端
		else
		{
			m_GateServer->DispatchPacket( packet );
		}
	}
	void CenterHandler::OnDisconnect()
	{
	}
	void CenterHandler::Update()
	{
	}
}