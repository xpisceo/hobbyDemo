/********************************************************************
	created:	2009-6-28
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	网关服务器->中心服务器
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class CenterServer;

	enum GateStatus
	{
		GateStatus_WaitForInfo,			/// 刚连接上,等待网关服务器发来自己的信息
		GateStatus_Running,				/// 收到了网关服务器的信息,正常运行
	};

	class GateHandler : public SocketHandler
	{
	public:
		GateHandler();
		~GateHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		uint8 m_GateId;
		GateStatus m_GateStatus;
		CenterServer* m_CenterServer;
	};
}