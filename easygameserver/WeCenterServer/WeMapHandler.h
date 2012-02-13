/********************************************************************
	created:	2009-6-28
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	地图服务器->中心服务器
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class CenterServer;

	enum MapStatus
	{
		MapStatus_WaitForInfo,			/// 刚连接上,等待网关服务器发来自己的信息
		MapStatus_Running,				/// 收到了网关服务器的信息,正常运行
	};

	class MapHandler : public SocketHandler
	{
	public:
		MapHandler();
		~MapHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		uint16 m_MapServerId;
		MapStatus m_MapStatus;
		CenterServer* m_CenterServer;
	};
}