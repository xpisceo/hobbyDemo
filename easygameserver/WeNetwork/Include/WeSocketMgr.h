/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	网络库管理器
*********************************************************************/
#pragma once

#include "WeSocket.h"
#include "WeSingleton.h"
#include <vector>
#include <map>
using namespace std;

namespace We
{
	class Socket;
	class SocketListener;

	class /*We_DLL_EXPORTS*/ SocketMgr : public Singleton<SocketMgr>
	{
		friend unsigned int WINAPI Thread_SendPackets(void* arg);	/// 发包线程
	public:
		SocketMgr();
		~SocketMgr();

		/// 添加一个监听
		void AddSocketListener( uint32 listenerId, SocketListenerHandler* handler, uint32 maxConnections, uint16 maxSendPacketSize,
			uint16 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize, bool beforeStart, uint encryptType=0 );

		/// 添加一个连接(不要在多线程里调用这个!!!)
		bool AddSocketConnector( uint32 connectorId, SocketHandler* handler, const char* ip, uint16 port,
			uint16 maxSendPacketSize, uint16 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize, bool beforeStart, uint encryptType=0 );

		/// 开启IOCP线程
		/// iocpThreadNum==0时,表示根据CPU数量来开启线程数
		void StartIOCP( unsigned int iocpThreadNum );

		/// 开启一个监听
		bool StartListen( uint32 listenerId, const char* ip, unsigned short port, bool listenAnyIP=false );

		/// 开始连接
		bool StartConnect( uint32 connectorId, const char* ip, unsigned short port );

		/// 所有连接发包,被发包线程调用
		void SendPackets();

		/// 逻辑线程调用该接口,处理数据包
		void Update();

		/// 关闭
		void Shutdown();

		inline HANDLE GetCompletionPort() const { return m_CompletionPort; }
		inline bool IsSendingPackets() const { return m_SendingPackets; }
		inline bool IsShutdown() const { return m_IsShutdown; }

	protected:
		inline void SetSendingPackets( bool isSending ) { m_SendingPackets = isSending; }

	protected:
		bool	m_IsShutdown;								/// 是否停止
		bool	m_SendingPackets;							/// 发包线程是否正在运行
		unsigned int m_IOCPThreadCount;						/// IOCP的线程数量
		HANDLE m_CompletionPort;							/// 完成端口句柄

		vector<SocketListener*> m_NewSocketListeners;		/// 新的监听列表
		vector<Socket*>			m_NewSocketConnectors;		/// 新的连接列表
		WinLock					m_NewSocketListenersLock;	/// 新的监听列表的线程锁
		WinLock					m_NewSocketConnectorsLock;	/// 新的连接列表的线程锁

		map<unsigned int,SocketListener*>	m_SocketListeners;		/// 监听列表
		map<unsigned int,Socket*>			m_SocketConnectors;		/// 连接列表
		WinLock								m_SocketListenersLock;	/// 监听列表的线程锁
		WinLock								m_SocketConnectorsLock;	/// 连接列表的线程锁
	};

}