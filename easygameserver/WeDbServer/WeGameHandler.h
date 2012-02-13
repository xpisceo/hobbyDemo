/********************************************************************
	created:	2009-6-29
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	中心服务器->帐号服务器
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class DbServer;

	enum GameServerStatus
	{
		GameServerStatus_WaitForInfo,			/// 刚连接上,等待中心服务器发来自己的信息
		GameServerStatus_Running,				/// 收到了中心服务器的信息,正常运行
	};

	class GameHandler : public SocketHandler
	{
	public:
		GameHandler();
		~GameHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		unsigned int m_GameServerId;
		GameServerStatus m_GameServerStatus;
		DbServer* m_DbServer;
	};
}