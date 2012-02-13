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
		/// �������ķ�������
		if( packet->Type2.m_ServerType == 0 )
		{
			/// �Ƿ����ݰ�,Ӧ�öϿ�������
			if( packet->m_Length < sizeof(Packet_C2CS_PacketHeader) )
			{
				return;
			}
			/// �ʺŵ�¼�ʹ����ʺŵ���Ϣ��Ҫ��������id,���ڻش���Ϣ
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
			/// �����ڴ˼��packet��m_PlayerId�Ƿ����m_Id,���ų��Ƿ����ݰ�
			/// ����Ҫע��ĳЩ�������Ͳ�����!!!!!!
			/// ��ֵm_PlayerId,�������ſͻ����Լ�������������
			((Packet_C2CS_PacketHeader*)packet)->m_PlayerID = m_Id;
			/// �ͻ��˷������ķ��������ڲ���Ϣ
			m_GateServer->SendToCenter( packet );
		}
		/// ������ͼ��������
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