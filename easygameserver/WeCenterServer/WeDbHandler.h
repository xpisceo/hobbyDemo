/********************************************************************
	created:	2009-7-5
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ķ�����->DB������
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class CenterServer;

	class DbHandler : public SocketHandler
	{
	public:
		DbHandler();
		~DbHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		uint32 m_Id;
		CenterServer* m_CenterServer;
	};
}