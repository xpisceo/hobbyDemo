/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�ӿ�
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

		/// �����������ݰ�
		virtual void OnProcessPacket( PacketHeader* packet ) = 0;

		/// ���ӶϿ�
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

		// isDisconnect ��ʾ�Ƿ��ǵ���
		virtual void OnRemove( Socket* socket, bool isDisconnect ) = 0;
	};
}