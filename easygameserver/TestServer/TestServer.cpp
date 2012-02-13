#include "TestServer.h"
#include <stdio.h>
#include "WeSocket.h"
#include "ClientHandler.h"

namespace We
{
	TestServer::TestServer()
	{
		m_ClientHandlerId = 0;

		m_TotalRecvMsgSizeSec = 0;
		m_TotalRecvMsgSize = 0;
		m_TotalSendMsgSizeSec = 0;
		m_TotalSendMsgSize = 0;

		m_LastSecTick = ::GetTickCount();
		m_LastShowTick = m_LastSecTick;
		m_LastSendTick = m_LastSecTick;
	}
	TestServer::~TestServer()
	{
	}
	void TestServer::OnAccept( Socket* socket )
	{
		/// use ObjectPool::Alloc
		ClientHandler* handler = new ClientHandler();
		handler->m_Id = ++m_ClientHandlerId;
		handler->m_TestServer = this;
		socket->SetSocketHandler( handler );
		handler->SetSocket( socket );
		m_ClientHandlers[handler->m_Id] = handler;
		printf( "OnAccept Id=%d\n", handler->m_Id );
	}
	void TestServer::OnRemove( Socket* socket )
	{
		ClientHandler* handler = (ClientHandler*)socket->GetSocketHandler();
		if( handler != 0 )
		{
			printf( "OnRemove Id=%d\n", handler->m_Id );
			map<unsigned int, ClientHandler*>::iterator i = m_ClientHandlers.find(handler->m_Id);
			if( i != m_ClientHandlers.end() )
				m_ClientHandlers.erase(i);
			handler->GetSocket()->SetSocketHandler( 0 );
			/// use ObjectPool::Free
			delete handler;
		}
		else
		{
			printf( "OnRemove\n" );
		}
	}
	void TestServer::SendPacketAround( const PacketHeader* packet )
	{
		if( m_ClientHandlers.size() == 0 )
			return;
		int randIndex = 0;
		if( m_ClientHandlers.size() > 20 )
			randIndex = ::rand()%(m_ClientHandlers.size()-20);
		map<unsigned int, ClientHandler*>::iterator i = m_ClientHandlers.begin();
		int index = 0;
		int sendCount = 0;
		for( i; i!=m_ClientHandlers.end(); ++i )
		{
			if( ++index > randIndex )
			{
				m_TotalSendMsgSizeSec += packet->m_Length;
				m_TotalSendMsgSize += packet->m_Length; 
				i->second->SendPacket( packet );
				++sendCount;
				if( sendCount >= 20 )
					break;
			}
		}
	}

	void TestServer::Update()
	{
		unsigned int currTick = ::GetTickCount();
		if( currTick - m_LastSecTick >= 1000 )
		{
			m_LastSecTick += 1000;
			if( currTick - m_LastSecTick > 1000 )
				m_LastSecTick = currTick;
			if( currTick - m_LastShowTick >= 5*1000 )
			{
				m_LastShowTick = currTick;
				if( m_TotalRecvMsgSizeSec > 1024*1024 )
					printf( "RecvSpeed=%.2fM,", (float)m_TotalRecvMsgSizeSec/(1024*1024) );
				else
					printf( "RecvSpeed=%.2fK,", (float)m_TotalRecvMsgSizeSec/(1024) );
				if( m_TotalSendMsgSizeSec > 1024*1024 )
					printf( "SendSpeed=%.2fM,", (float)m_TotalSendMsgSizeSec/(1024*1024) );
				else
					printf( "SendSpeed=%.2fK,", (float)m_TotalSendMsgSizeSec/(1024) );
				printf( "TotalRecv=%.2fM,TotalSend=%.2fM", (float)m_TotalRecvMsgSize/(1024*1024), (float)m_TotalSendMsgSize/(1024*1024) );
				printf( " Online=%d\n", m_ClientHandlers.size() );
			}
			m_TotalRecvMsgSizeSec = 0;
			m_TotalSendMsgSizeSec = 0;
		}
	}
}