/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	接口
*********************************************************************/
#pragma once

#include "WePacketHeader.h"

namespace We
{
	class Socket;

	class SocketHandler
	{
	public:
		SocketHandler() { m_Socket = NULL; }

		/// 处理网络数据包
		virtual void OnProcessPacket( PacketHeader* packet ) = 0;

		/// 连接断开
		virtual void OnDisconnect() = 0;

		void SetSocket( Socket* socket ) { m_Socket = socket; }
		Socket* GetSocket() { return m_Socket; }

		bool SendPacket( PacketHeader* packet );

	protected:
		Socket* m_Socket;
	};

	class SocketListenerHandler
	{
	public:
		virtual void OnAccept( Socket* socket ) = 0;

		// isDisconnect 表示是否是掉线
		virtual void OnRemove( Socket* socket, bool isDisconnect ) = 0;
	};
}