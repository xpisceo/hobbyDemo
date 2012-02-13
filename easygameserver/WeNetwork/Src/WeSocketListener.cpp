#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include "WeSocketListener.h"
#include "WeSocketOps.h"
#include "WeSocketMgr.h"
#include "WeLog.h"

namespace We
{
	unsigned int WINAPI Thread_Listening(void* arg)
	{
		printf( "Thread_Listening\n" );

		SocketListener* socketListener = (SocketListener*)arg;
		SOCKET newSocket = INVALID_SOCKET;
		struct sockaddr_in tempAddress;
		int len = sizeof(sockaddr_in);
		Socket* newConnection = 0;
		while( socketListener->IsListening() )
		{
			newSocket = WSAAccept( socketListener->GetSocket(), (sockaddr*)&tempAddress, (socklen_t*)&len, NULL, NULL);
			if( newSocket == INVALID_SOCKET)
				continue;		// shouldn't happen, we are blocking.

			/// 监听到了一个新的连接
			socketListener->OnAccept( newSocket, &tempAddress );
		}
		_endthreadex( 0 );
		return 0;
	}

	SocketListener::SocketListener()
	{
		m_ListenerId = 0;
		m_ListenThreadId = 0;
		m_ListenThreadHandle = 0;
		m_ListenSocket = INVALID_SOCKET;
		m_IsListening = false;
		m_SocketListenerHandler = 0;
		m_MaxConnections = 10;
		m_MaxSendPacketSize = 1024*10;
		m_MaxRecvPacketSize = m_MaxSendPacketSize;
		m_SendBufferSize = m_MaxSendPacketSize*5;
		m_RecvBufferSize = m_MaxRecvPacketSize*5;
		m_SocketPool = new ObjectPool<Socket>();
		m_DisconnectTime = 1000*60*5;	// 默认超时断线为5分钟
		m_EncryptPacketAPI = NULL;
	}
	SocketListener::~SocketListener()
	{
		delete m_SocketPool;
	}
	void SocketListener::Init( uint32 maxConnections, uint32 maxSendPacketSize, uint32 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize )
	{
		m_MaxConnections = maxConnections;
		m_MaxSendPacketSize = maxSendPacketSize;
		m_MaxRecvPacketSize = maxRecvPacketSize;
		m_SendBufferSize = sendBufferSize;
		m_RecvBufferSize = recvBufferSize;
		m_SocketPool->Init( m_MaxConnections, m_MaxConnections/5 );
	}
	bool SocketListener::StartListen( const char* ip, unsigned short port, bool listenAnyIP/*=false*/ )
	{
		m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		SocketOps::ReuseAddr(m_ListenSocket);
		SocketOps::Blocking(m_ListenSocket);
		SocketOps::SetTimeout(m_ListenSocket, 60*2 ); // 超时设置为2分钟

		m_ListenAddress.sin_family = AF_INET;
		m_ListenAddress.sin_port = ntohs((u_short)port);
		m_ListenAddress.sin_addr.s_addr = htonl(INADDR_ANY);

		if( !listenAnyIP )
		{
			struct hostent * hostname = gethostbyname( ip );
			if(hostname != 0)
				memcpy(&m_ListenAddress.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
		}

		int ret = bind(m_ListenSocket, (const sockaddr*)&m_ListenAddress, sizeof(m_ListenAddress));
		if(ret != 0)
		{
			printf("Bind failed on port %u.", port);
			return false;
		}

		ret = listen(m_ListenSocket, 5);
		if(ret != 0) 
		{
			printf("Unable to listen on port %u.", port);
			return false;
		}

		m_IsListening = true;
		m_CompletionPort = SocketMgr::getSingleton().GetCompletionPort();

		m_ListenThreadHandle = (HANDLE)_beginthreadex( 0, 0, &Thread_Listening, this, 0, &m_ListenThreadId );
		return true;
	}
	void SocketListener::Close()
	{
		m_ActiveConnectionsLock.Lock();
		vector<Socket*>::iterator i = m_ActiveConnections.begin();
		for( i; i!=m_ActiveConnections.end(); ++i )
		{
			Socket* socket = *i;
			socket->Delete();
			socket->Disconnect();	/// 断开连接
			m_SocketPool->Free( socket );
		}
		m_ActiveConnections.clear();
		m_ActiveConnectionsLock.Unlock();

		if( m_IsListening )
		{
			SocketOps::CloseSocket( m_ListenSocket );
			m_IsListening = false;
			if( m_ListenThreadHandle != 0 )
			{
				CloseHandle( m_ListenThreadHandle );
				m_ListenThreadHandle = 0;
			}
		}
	}
	void SocketListener::OnAccept(SOCKET newSocket, sockaddr_in* address)
	{
		Socket* newConnection = m_SocketPool->Alloc();
		assert( newConnection );
		newConnection->m_EncryptPacketAPI = m_EncryptPacketAPI;
		newConnection->Init( newSocket, m_MaxSendPacketSize, m_MaxRecvPacketSize, m_SendBufferSize, m_RecvBufferSize );
		newConnection->SetCompletionPort( m_CompletionPort );
		newConnection->Accept( address );

		m_NewConnectionsLock.Lock();
		m_NewConnections.push_back( newConnection );
		m_NewConnectionsLock.Unlock();
	}
	void SocketListener::Update()
	{
		/// 新的连接
		if( !m_NewConnections.empty() )
		{
			m_NewConnectionsLock.Lock();
			vector<Socket*> tmpSockets = m_NewConnections;
			m_NewConnections.clear();
			m_NewConnectionsLock.Unlock();

			m_ActiveConnectionsLock.Lock();
			vector<Socket*>::iterator it = tmpSockets.begin();
			for( it; it != tmpSockets.end(); )
			{
				if( m_ActiveConnections.size() >= m_MaxConnections )
				{
					/// 任何能在关闭客户端连接前,发一个数据包,告诉客户端,服务器已经满了??????
					/// 也许这个任务可以交给登陆服务器(服务器满员情况)
					(*it)->Disconnect();
					m_SocketPool->Free( *it );
					it = tmpSockets.erase( it );
					continue;
				}
				m_ActiveConnections.push_back( *it );
				++it;
			}
			m_ActiveConnectionsLock.Unlock();

			for( int i=0; i<tmpSockets.size(); ++i )
			{
				if( m_SocketListenerHandler != 0 )
					m_SocketListenerHandler->OnAccept( tmpSockets[i] );
			}
		}

		/// 这里不加锁,避免造成阻塞,稍后再对m_ActiveConnections操作
		bool removeActiveConnection = false;
		vector<Socket*>::iterator i = m_ActiveConnections.begin();
		for( i; i!=m_ActiveConnections.end(); ++i)
		{
			Socket* socket = *i;
			if( socket->IsDeleted() )
			{
				removeActiveConnection = true;
				if( m_SocketListenerHandler != 0 )
					m_SocketListenerHandler->OnRemove( socket, false );
				if( socket->GetSocketHandler() != 0 )
				{
					socket->GetSocketHandler()->SetSocket( 0 );
				}
				continue;
			}
			/// 处理数据包
			socket->Update();
			/// 检查超时断线
			uint now = ::timeGetTime();
			if( socket->m_LastActiveTick > 0 && now >= socket->m_LastActiveTick + m_DisconnectTime )
			{
				printf( "超时断线了 IP=%s Port=%d now=%u LastActiveTick=%u\n",
					socket->GetRemoteIP().c_str(), socket->GetRemotePort(), now, socket->m_LastActiveTick );
				// 记录到日志文件,便于找出掉线原因
				Log fileLog( "Log/Network.log", true );
				fileLog.WriteLog( "超时断线了 IP=%s Port=%d now=%u LastActiveTick=%u LastRecvTick=%u LastSendTick=%u\n",
					socket->GetRemoteIP().c_str(), socket->GetRemotePort(), now, socket->m_LastActiveTick, socket->m_LastRecvTick, socket->m_LastSendTick );
				fileLog.Close();
				socket->Delete();
				removeActiveConnection = true;
				if( m_SocketListenerHandler != 0 )
					m_SocketListenerHandler->OnRemove( socket, true );
				if( socket->GetSocketHandler() != 0 )
				{
					socket->GetSocketHandler()->SetSocket( 0 );
				}
			}
		}

		/// 有些连接需要移除
		if( removeActiveConnection )
		{
			m_ActiveConnectionsLock.Lock();
			vector<Socket*>::iterator i = m_ActiveConnections.begin();
			for( i; i!=m_ActiveConnections.end(); )
			{
				Socket* socket = *i;
				if( socket->IsDeleted() )
				{
					socket->Disconnect();	/// 断开连接
					m_SocketPool->Free( socket );
					i = m_ActiveConnections.erase(i);
				}
				else
					++i;
			}
			m_ActiveConnectionsLock.Unlock();
		}
	}

	void SocketListener::SendPackets()
	{
		m_ActiveConnectionsLock.Lock();
		vector<Socket*>::iterator i = m_ActiveConnections.begin();
		for( i; i!=m_ActiveConnections.end(); ++i)
		{
			Socket* socket = *i;
			socket->StartSend();
		}
		m_ActiveConnectionsLock.Unlock();
	}

}