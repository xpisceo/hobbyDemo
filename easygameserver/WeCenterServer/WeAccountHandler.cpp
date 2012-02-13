#include "WeAccountHandler.h"
#include <stdio.h>
#include <assert.h>
#include "WeCenterServer.h"
#include "WeCenter2ClientPackets_Inner.h"
#include "WeCenter2GatePackets_Inner.h"
#include "WeAccount2CenterPackets_Inner.h"

namespace We
{
	AccountHandler::AccountHandler()
	{
		m_Id = 0;
		m_CenterServer = 0;
	}
	AccountHandler::~AccountHandler()
	{
	}
	void AccountHandler::OnProcessPacket( PacketHeader* packet )
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
			case PT_A2CS_Inner_LoginResult:
				m_CenterServer->OnRecv_LoginResult( packet );
				break;
			case PT_A2CS_Inner_CreateAccountResult:
				m_CenterServer->OnRecv_CreateAccountResult( packet );
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