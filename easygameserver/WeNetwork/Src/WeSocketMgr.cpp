#include "WeSocketMgr.h"
#include <process.h>
#include "WeSocketListener.h"
#include "WePacketFactory.h"
#include "WeFileLoader.h"

namespace We
{
	/// IOCP的查询线程
	unsigned int WINAPI Thread_IOCP_QueueResult(void* arg)
	{
		printf( "Thread_IOCP_QueueResult\n" );
		//THREAD_TRY_EXECUTION2
		HANDLE cp = SocketMgr::getSingleton().GetCompletionPort();
		DWORD len;
		Socket * s;
		OverlappedStruct * ov;
		LPOVERLAPPED ol_ptr;

		while(true)
		{
#ifndef _WIN64
			if(!GetQueuedCompletionStatus(cp, &len, (LPDWORD)&s, &ol_ptr, 10000))
#else
			if(!GetQueuedCompletionStatus(cp, &len, (PULONG_PTR)&s, &ol_ptr, 10000))
#endif
				continue;

			ov = CONTAINING_RECORD(ol_ptr, OverlappedStruct, m_overlap);

			if(ov->m_event == SOCKET_IO_THREAD_SHUTDOWN)
			{
				delete ov;
				return 0;
			}
			if(ov->m_event == SOCKET_IO_EVENT_RECV_COMPLETE )
			{
				if(!s->IsDeleted())
				{
					s->OnRecvBuffer( len );
				}
			}
			else if(ov->m_event == SOCKET_IO_EVENT_SEND_END )
			{
				if(!s->IsDeleted())
				{
					s->OnSendBuffer( len );
				}
			}
		}

		//THREAD_HANDLE_CRASH2
		_endthreadex( 0 );
		return 0;
	}

	/// 发包线程
	unsigned int WINAPI Thread_SendPackets(void* arg)
	{
		printf( "Thread_SendPackets\n" );
		SocketMgr* socketMgr = (SocketMgr*)arg;
		socketMgr->SetSendingPackets( true );
		while( !socketMgr->IsShutdown() )
		{
			socketMgr->SendPackets();
			Sleep(1);
		}
		socketMgr->SetSendingPackets( false );
		_endthreadex( 0 );
		return 0;
	}

	void InitKeyMap()
	{
	}

	// 默认加密解密算法
	static bool DefaultEncryptPacketAPI( uint8* data, uint len, uint type )
	{
		return true;
	}

	Singleton_Implement( SocketMgr );

	SocketMgr::SocketMgr()
	{
		m_IOCPThreadCount = 0;
		m_SendingPackets = false;
		m_IsShutdown = false;

		InitKeyMap();

		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,0), &wsaData);
		m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 0);

		new PacketFactory();
	}

	SocketMgr::~SocketMgr()
	{
	}

	void SocketMgr::AddSocketListener( uint32 id, SocketListenerHandler* handler, uint32 maxConnections, uint16 maxSendPacketSize,
		uint16 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize, bool beforeStart, uint encryptType/*=0*/ )
	{
		assert( id > 0 );
		SocketListener* listener = new SocketListener();
		if( encryptType > 0 )
			listener->m_EncryptPacketAPI = DefaultEncryptPacketAPI;
		listener->Init( maxConnections, maxSendPacketSize, maxRecvPacketSize, sendBufferSize, recvBufferSize );
		listener->SetSocketListenerHandler( handler );
		listener->m_ListenerId = id;
		if( beforeStart )
		{
			map<unsigned int,SocketListener*>::iterator i = m_SocketListeners.find( id );
			if( i != m_SocketListeners.end() )
			{
				assert( false );
				delete listener;
				return;
			}
			m_SocketListeners[id] = listener;
		}
		else
		{
			m_NewSocketListenersLock.Lock();
			m_NewSocketListeners.push_back( listener );
			m_NewSocketListenersLock.Unlock();
		}
	}

	bool SocketMgr::AddSocketConnector( uint32 id, SocketHandler* handler, const char* ip, uint16 port,
		uint16 maxSendPacketSize, uint16 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize, bool beforeStart, uint encryptType/*=0*/ )
	{
		Socket* connector = new Socket();
		if( encryptType > 0 )
			connector->m_EncryptPacketAPI = DefaultEncryptPacketAPI;
		connector->Init( 0, maxSendPacketSize, maxRecvPacketSize, sendBufferSize, recvBufferSize );
		connector->m_ConnectorId = id;
		if( !connector->SetConnectAddress( ip, port ) )
		{
			//assert( false );
			delete connector;
			return false;
		}
		connector->SetSocketHandler( handler );
		handler->SetSocket( connector );
		if( !connector->Connect( ip, port ) )
		{
			handler->SetSocket( 0 );
			delete connector;
			return false;
		}
		if( beforeStart )
		{
			map<unsigned int,Socket*>::iterator i = m_SocketConnectors.find( id );
			if( i != m_SocketConnectors.end() )
			{
				//assert( false );
				handler->SetSocket( 0 );
				delete connector;
				return false;
			}
			m_SocketConnectors[id] = connector;
		}
		else
		{
			//m_NewSocketConnectorsLock.Lock();
			//m_NewSocketConnectors.push_back( connector );
			//m_NewSocketConnectorsLock.Unlock();
			m_SocketConnectorsLock.Lock();
			map<unsigned int,Socket*>::iterator i = m_SocketConnectors.find( id );
			if( i != m_SocketConnectors.end() )
			{
				handler->SetSocket( 0 );
				delete connector;
				m_SocketConnectorsLock.Unlock();
				assert( false );
				return false;
			}
			m_SocketConnectors[id] = connector;
			m_SocketConnectorsLock.Unlock();
		}
		return true;
	}

	void SocketMgr::StartIOCP( unsigned int iocpThreadNum )
	{
		if( iocpThreadNum == 0 )
		{
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			m_IOCPThreadCount = si.dwNumberOfProcessors;
		}
		else
			m_IOCPThreadCount = iocpThreadNum;
		for(int i = 0; i < m_IOCPThreadCount; ++i)
		{
			unsigned int threadID = 0;
			_beginthreadex( 0, 0, &Thread_IOCP_QueueResult, 0, 0, &threadID);
		}
		m_SendingPackets = true;
		unsigned int threadID = 0;
		_beginthreadex(NULL, 0, &Thread_SendPackets, this, 0, &threadID);
	}

	bool SocketMgr::StartListen( uint32 listenerId, const char* ip, unsigned short port, bool listenAnyIP/*=false*/ )
	{
		if( !ip || port == 0 || listenerId == 0 || strlen(ip) <= 7 )
			return false;
		// 验证授权文件,忽略局域网
		string strIP = ip;
		string ipHeader = strIP.substr( 0, 7 );	// 192.168
		if( ipHeader == "192.168" || ipHeader == "127.0.0" )
		{
		}
		else
		{
			uint dataSize = 0;
			uint8* data = FileLoader::LoadFile( strIP+".license", dataSize, true );
			if( !data )
			{
				return false;
			}
			if( data )
			{
				string strVersion = (const char*)data;
				delete []data;
				if( strVersion != strIP )
					return false;
			}
		}
		//
		map<unsigned int,SocketListener*>::iterator i = m_SocketListeners.find( listenerId );
		assert( i != m_SocketListeners.end() );
		if( i == m_SocketListeners.end() )
			return false;
		return i->second->StartListen( ip, port, listenAnyIP );
	}

	bool SocketMgr::StartConnect( uint32 connectorId, const char* ip, unsigned short port )
	{
		map<unsigned int,Socket*>::iterator i = m_SocketConnectors.find( connectorId );
		if( i == m_SocketConnectors.end() )
			return false;
		return i->second->Connect( ip, port );
	}

	void SocketMgr::SendPackets()
	{
		m_SocketListenersLock.Lock();
		map<unsigned int,SocketListener*>::iterator i1 = m_SocketListeners.begin();
		for( i1; i1 != m_SocketListeners.end(); ++i1 )
		{
			i1->second->SendPackets();
		}
		m_SocketListenersLock.Unlock();

		m_SocketConnectorsLock.Lock();
		map<unsigned int,Socket*>::iterator i2 = m_SocketConnectors.begin();
		for( i2; i2 != m_SocketConnectors.end(); ++i2 )
		{
			i2->second->StartSend();
		}
		m_SocketConnectorsLock.Unlock();
	}

	void SocketMgr::Update()
	{
		if( !m_NewSocketListeners.empty() )
		{
			m_NewSocketListenersLock.Lock();
			vector<SocketListener*> newSocketListeners = m_NewSocketListeners;
			m_NewSocketListeners.clear();
			m_NewSocketListenersLock.Unlock();
			// 新的Listener
			m_SocketListenersLock.Lock();
			for( int i=0; i<newSocketListeners.size(); ++i )
			{
				map<unsigned int,SocketListener*>::iterator i1 = m_SocketListeners.find( newSocketListeners[i]->m_ListenerId );
				if( i1 != m_SocketListeners.end() )
				{
					delete newSocketListeners[i];
					assert( false );
					continue;
				}
				m_SocketListeners[newSocketListeners[i]->m_ListenerId] = newSocketListeners[i];
			}
			m_SocketListenersLock.Unlock();
			newSocketListeners.clear();
		}
		if( !m_NewSocketConnectors.empty() )
		{
			m_NewSocketConnectorsLock.Lock();
			vector<Socket*> newSocketConnectors = m_NewSocketConnectors;
			m_NewSocketConnectors.clear();
			m_NewSocketConnectorsLock.Unlock();
			// 新的Connector
			m_SocketConnectorsLock.Lock();
			for( int i=0; i<newSocketConnectors.size(); ++i )
			{
				map<unsigned int,Socket*>::iterator i2 = m_SocketConnectors.find( newSocketConnectors[i]->m_ConnectorId );
				if( i2 != m_SocketConnectors.end() )
				{
					delete newSocketConnectors[i];
					assert( false );
					continue;
				}
				m_SocketConnectors[newSocketConnectors[i]->m_ConnectorId] = newSocketConnectors[i];
			}
			m_SocketConnectorsLock.Unlock();
			newSocketConnectors.clear();
		}
		bool removeListener = false;
		bool removeConnector = false;
		/// 这里不加锁,因为其他线程不会修改m_SocketListeners和m_SocketConnectors
		/// 避免造成阻塞,稍后再执行移除
		map<unsigned int,SocketListener*>::iterator i1 = m_SocketListeners.begin();
		for( i1; i1 != m_SocketListeners.end(); ++i1 )
		{
			i1->second->Update();
		}
		map<unsigned int,Socket*>::iterator i2 = m_SocketConnectors.begin();
		for( i2; i2 != m_SocketConnectors.end(); ++i2 )
		{
			i2->second->Update();
			if( i2->second->IsDeleted() )
			{
				removeConnector = true;
				if( i2->second->GetSocketHandler() != 0 )
					i2->second->GetSocketHandler()->OnDisconnect();
			}
		}
		if( removeListener )
		{
		}
		if( removeConnector )
		{
			// 这里会和发包线程互斥
			m_SocketConnectorsLock.Lock();
			map<unsigned int,Socket*>::iterator it = m_SocketConnectors.begin();
			for( it; it != m_SocketConnectors.end(); )
			{
				Socket* socket = it->second;
				if( socket->IsDeleted() )
				{
					socket->Disconnect();
					if( socket->GetSocketHandler() != 0 )
						socket->GetSocketHandler()->SetSocket( 0 );
					/// use ObjectPool?
					delete socket;
					it = m_SocketConnectors.erase( it );
					continue;
				}
				++it;
			}
			m_SocketConnectorsLock.Unlock();
		}
	}

	void SocketMgr::Shutdown()
	{
		m_IsShutdown = true;

		/// 停止发包线程
		while( m_SendingPackets )
		{
			Sleep( 10 );
		}

		/// 发包线程已经结束了,不用加锁了
		map<unsigned int,SocketListener*>::iterator i1 = m_SocketListeners.begin();
		for( i1; i1 != m_SocketListeners.end(); ++i1 )
		{
			i1->second->Close();
		}
		map<unsigned int,Socket*>::iterator i2 = m_SocketConnectors.begin();
		for( i2; i2 != m_SocketConnectors.end(); ++i2 )
		{
			i2->second->Disconnect();
		}

		/// 停止IOCP线程
		for(int i = 0; i < m_IOCPThreadCount; ++i)
		{
			OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_THREAD_SHUTDOWN);
			PostQueuedCompletionStatus(m_CompletionPort, 0, (ULONG_PTR)0, &ov->m_overlap);
		}

		i1 = m_SocketListeners.begin();
		for( i1; i1 != m_SocketListeners.end(); ++i1 )
		{
			delete i1->second;
		}
		m_SocketListeners.clear();
		i2 = m_SocketConnectors.begin();
		for( i2; i2 != m_SocketConnectors.end(); ++i2 )
		{
			delete i2->second;
		}
		m_SocketConnectors.clear();

		if( PacketFactory::getSingletonPtr() )
			delete PacketFactory::getSingletonPtr();
	}

}