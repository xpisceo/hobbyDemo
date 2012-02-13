/********************************************************************
	created:	2009-6-30
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	中心服务器->帐号服务器
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class GameServer;

	class AccountHandler : public SocketHandler
	{
	public:
		AccountHandler();
		~AccountHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		uint32 m_Id;
		GameServer* m_GameServer;
	};
}