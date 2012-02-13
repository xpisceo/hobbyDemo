#include "WeClientHandler.h"
#include <stdio.h>
#include "WeGameServer.h"
#include "WeClient2GamePackets_Inner.h"
#include "WeLogInfo.h"
#include "WeSocket.h"

namespace We
{
	ClientHandler::ClientHandler()
	{
		m_ClientId = 0;
		m_ConnectTime = 0;
		m_GameServer = 0;
		Init();
	}
	ClientHandler::~ClientHandler()
	{
	}
	void ClientHandler::Init()
	{
		m_ClientStatus = PlayerStatus_None;
		m_LoginFailCount = 0;
	}
	void ClientHandler::OnProcessPacket( PacketHeader* packet )
	{
		if( GetSocket() == 0 )
			return;
		/// �ڲ���Ϣ
		if( packet->m_MainType == 0 )
		{
			switch( packet->m_SubType )
			{
			case PT_C2GS_Inner_CreateAccount:
				if( m_ClientStatus != PlayerStatus_None )
				{
					/// �Ƿ���Ϣ,�Ͽ�����
					GetSocket()->Delete();
					return;
				}
				m_GameServer->OnRecv_CreateAccount( this, packet );
				break;
			case PT_C2GS_Inner_Login:
				if( m_ClientStatus != PlayerStatus_None )
				{
					/// �Ƿ���Ϣ,�Ͽ�����
					GetSocket()->Delete();
					return;
				}
				m_GameServer->OnRecv_Login( this, packet );
				break;
			case PT_C2GS_Inner_CreateRole:
				if( m_ClientStatus != PlayerStatus_LoginOK )
				{
					/// �Ƿ���Ϣ,�Ͽ�����
					GetSocket()->Delete();
					return;
				}
				m_GameServer->OnRecv_CreateRole( this, packet );
				break;
			case PT_C2GS_Inner_ChooseRole:
				if( m_ClientStatus != PlayerStatus_LoginOK )
				{
					/// �Ƿ���Ϣ,�Ͽ�����
					GetSocket()->Delete();
					return;
				}
				m_GameServer->OnRecv_ChooseRole( this, packet );
				break;
			case PT_C2GS_Inner_DeleteRole:
				if( m_ClientStatus != PlayerStatus_LoginOK )
				{
					/// �Ƿ���Ϣ,�Ͽ�����
					GetSocket()->Delete();
					return;
				}
				m_GameServer->OnRecv_DeleteRole( this, packet );
				break;

			case PT_C2GS_Inner_Move:
			case PT_C2GS_Inner_JoinMap:
			case PT_C2GS_Inner_GetObjData:
				if( m_ClientStatus != PlayerStatus_LoginOK )
				{
					GetSocket()->Delete();
					return;
				}
				m_GameServer->OnRecv_PlayerPacket( this, packet );
				break;

			default:
				/// �Ƿ���Ϣ,�Ͽ�����
				GetSocket()->Delete();
				break;
			}
		}
		/// �߼���Ϣ
		else
		{
			m_GameServer->OnRecv_PlayerPacket( this, packet );
		}
	}
	void ClientHandler::OnDisconnect()
	{
		LogInfo( "ClientHandler::OnDisconnect ClientId=%d", m_ClientId );
	}
	void ClientHandler::Update()
	{
	}
}