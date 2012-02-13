#include "WeMapHandler.h"
#include <stdio.h>
#include "WeCenterServer.h"
#include "WeMap2CenterPackets_Inner.h"

namespace We
{
	MapHandler::MapHandler()
	{
		m_MapServerId = 0;
		m_MapStatus = MapStatus_WaitForInfo;
		m_CenterServer = 0;
	}
	MapHandler::~MapHandler()
	{
	}
	void MapHandler::OnProcessPacket( PacketHeader* packet )
	{
		/// �ڲ���Ϣ
		if( packet->Type1.m_MainType == 0 )
		{
			switch( packet->Type1.m_SubType )
			{
				/// ��ͼ��������������Ϣ
			case PT_M2CS_Inner_MapServerInfo:
				m_CenterServer->OnRecv_MapServerInfo( this, packet );
				break;
			default:
				break;
			}
		}
		else
		{
		}
	}
	void MapHandler::OnDisconnect()
	{
	}
	void MapHandler::Update()
	{
	}
}