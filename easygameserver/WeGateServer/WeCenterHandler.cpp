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
		/// �ڲ���Ϣ
		if( packet->Type1.m_MainType == 0 )
		{
			assert( packet->Type1.m_SubType < PT_CS2G_Inner_MAX );
			if( packet->Type1.m_SubType >= PT_CS2G_Inner_MAX )
				return;
			switch( packet->Type1.m_SubType )
			{
				/// ������ͼ������
			case PT_CS2G_Inner_AddMapServer:
				m_GateServer->OnAddMapServer( packet );
				break;
				/// �Ƴ���ͼ������
			case PT_CS2G_Inner_RemoveMapServer:
				m_GateServer->OnRemoveMapServer( packet );
				break;
				/// ��¼�������
			case PT_CS2C_Inner_LoginResult:
				m_GateServer->OnClientLogin( packet );
				break;
			//case PT_CS2C_Inner_ChooseRoleData:
			//	m_GateServer->OnClientChooseRole( packet );
			//	break;
				/// ������Ϸ����(�����˵�ͼ)
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
		/// �߼���Ϣ,ת������Ӧ�Ŀͻ���
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