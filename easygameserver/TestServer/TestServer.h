#pragma once

#include "WoSocketHandler.h"
#include <map>
using namespace std;

namespace We
{
	class ClientHandler;

	class TestServer : public SocketListenerHandler
	{
	public:
		unsigned int m_ClientHandlerId;
		map<unsigned int, ClientHandler*> m_ClientHandlers;

		unsigned int m_TotalRecvMsgSizeSec;
		unsigned int m_TotalRecvMsgSize;
		unsigned int m_TotalSendMsgSizeSec;
		unsigned int m_TotalSendMsgSize;

		unsigned int m_LastSecTick;
		unsigned int m_LastShowTick;
		unsigned int m_LastSendTick;
	public:
		TestServer();
		~TestServer();

		virtual void OnAccept( Socket* socket );

		virtual void OnRemove( Socket* socket );

		/// 发给周围的20个玩家
		void SendPacketAround( const PacketHeader* packet );

		void Update();
	};
}