/********************************************************************
	created:	2009-6-27
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ط�����->���ķ�����
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