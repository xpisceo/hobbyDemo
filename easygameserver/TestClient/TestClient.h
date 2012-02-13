#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class TestClient : public SocketHandler
	{
	public:
		unsigned int m_LastPingTick;
		unsigned int m_LastMoveTick;
		unsigned int m_LastChatTick;
		unsigned int m_Id;
	public:
		TestClient();
		~TestClient();

		virtual void OnProcessPacket( PacketHeader* packet );

		virtual void OnDisconnect();

		void Update();
	};
}