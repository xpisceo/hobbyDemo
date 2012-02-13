/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	网络监听
*********************************************************************/
#pragma once

#include <vector>
using namespace std;

#include "WeSocket.h"
#include "WeObjectPool.h"

namespace We
{
	class SocketListener
	{
		friend class SocketMgr;
		friend unsigned int WINAPI Thread_Listening(void* arg); /// 监听线程
	public:
		SocketListener();
		virtual ~SocketListener();

		void Init( uint32 maxConnections, uint32 maxSendPacketSize, uint32 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize );

		inline void SetSocketListenerHandler( SocketListenerHandler* listener ) { m_SocketListenerHandler = listener; }
		inline SocketListenerHandler* GetSocketListenerHandler() const { return m_SocketListenerHandler; }

		/// 开始监听(开启一个监听线程)
		bool StartListen( const char* ip, unsigned short port, bool listenAnyIP=false );

		/// 关闭
		void Close();

		inline bool IsListening() const { return m_IsListening; }
		inline SOCKET GetSocket() const { return m_ListenSocket; }
		inline HANDLE GetIOCPHandle() { return m_CompletionPort; }

	protected:
		void OnAccept(SOCKET newSocket, sockaddr_in* address );

		void Update();

		void SendPackets();

	protected:
		unsigned int    m_ListenerId;
		unsigned		m_ListenThreadId;
		HANDLE			m_ListenThreadHandle;

		uint	m_MaxSendPacketSize;	/// 发包的大小限制	
		uint	m_MaxRecvPacketSize;	/// 收包的大小限制
		uint	m_SendBufferSize;		/// 发包缓存大小	
		uint	m_RecvBufferSize;		/// 收包缓存大小
		uint	m_DisconnectTime;		/// 检查超时断线的时间

		bool m_IsListening;						/// 是否已经在监听了	
		SOCKET m_ListenSocket;					/// 监听Socket
		struct sockaddr_in m_ListenAddress;		/// 监听地址 
		HANDLE m_CompletionPort;				/// 完成端口句柄

		ObjectPool<Socket>* m_SocketPool;		/// Socket的对象池

		unsigned int m_MaxConnections;			/// 最大连接数
		vector<Socket*> m_NewConnections;		/// 新连接
		vector<Socket*> m_ActiveConnections;	/// 活动的连接
		WinLock m_NewConnectionsLock;
		WinLock m_ActiveConnectionsLock;

		SocketListenerHandler* m_SocketListenerHandler; /// 接口

		EncryptPacketAPI	m_EncryptPacketAPI;
	};
}