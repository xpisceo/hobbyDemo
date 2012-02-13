/********************************************************************
	created:	2009-6-30
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ķ�����->�ʺŷ�����
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