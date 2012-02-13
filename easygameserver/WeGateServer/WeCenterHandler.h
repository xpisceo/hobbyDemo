/********************************************************************
	created:	2009-6-27
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	网关服务器->中心服务器
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class GateServer;

	class CenterHandler : public SocketHandler
	{
	public:
		CenterHandler();
		~CenterHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		uint32 m_Id;
		GateServer* m_GateServer;
	};
}