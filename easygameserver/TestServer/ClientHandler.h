#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class TestServer;

	class ClientHandler : public SocketHandler
	{
	public:
		unsigned int m_Id;
		TestServer* m_TestServer;
	public:
		ClientHandler();
		~ClientHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();
	};
}