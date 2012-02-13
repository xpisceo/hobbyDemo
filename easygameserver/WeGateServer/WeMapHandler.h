/********************************************************************
	created:	2009-6-27
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	网关服务器->地图服务器
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class GateServer;

	class MapHandler : public SocketHandler
	{
	public:
		MapHandler();
		~MapHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		unsigned int m_Id;
		GateServer* m_GateServer;
	};
}