/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	网络连接
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeLock.h"
#include "WeSocketDefines.h"
#include "WoSocketHandler.h"
#include "WeCircularBuffer.h"
#include "WePacketBuffer.h"
#include "WePacketHeader.h"
#include "WeQueue.h"
#include "WeLock.h"

namespace We
{
	typedef bool (*EncryptPacketAPI)( uint8* data, uint len, uint type );

	class /*We_DLL_EXPORTS*/ Socket
	{
		friend class SocketListener;
		friend class SocketMgr;
		friend unsigned int WINAPI Thread_IOCP_QueueResult(void* arg); /// 完成端口线程
	public:
		Socket();
		virtual ~Socket();

		void Init( SOCKET socket, uint32 maxSendPacketSize, uint32 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize );

		void SetSocketHandler( SocketHandler* socketHandler) { m_SocketHandler = socketHandler; }
		SocketHandler* GetSocketHandler() { return m_SocketHandler; }

		/// ip可以是IP地址, 也可以是域名
		bool SetConnectAddress( const char* ip, uint16 port );
		bool Connect( const char* ip, uint16 port );
		bool Reconnect();

		void Disconnect();

		void Delete();

		string GetRemoteIP();
		inline uint32 GetRemotePort() { return ntohs(m_ClientAddress.sin_port); }
		inline SOCKET GetSocket() { return m_Socket; }

		inline bool IsDeleted() { return m_Deleted; }
		inline bool IsConnected() { return m_IsConnected; }

		inline sockaddr_in& GetRemoteAddress() { return m_ClientAddress; }

		/// 获取发包缓存的数据大小
		uint GetSendBufferSize();

		// 数据统计
		uint GetSendPerSecond() const { return m_SendPerSecond; }
		uint GetRecvPerSecond() const { return m_RecvPerSecond; }
		uint GetTotalSendSize() const { return m_TotalSendSize; }
		uint GetTotalRecvSize() const { return m_TotalRecvSize; }
		uint GetStartTick() const { return m_StartTick; }
		uint GetPing() const { return m_Ping; }

		// 最后一次ping的时刻
		uint GetLastPingTick() const { return m_LastPingTick; }
		// 最后一个pong返回的时刻
		uint GetLastPongTick() const { return m_LastPongTick; }

		/// 把数据放入发送缓存
		bool Send( PacketHeader* packet );
		bool Ping();
	protected:
		bool Pong( PacketPing* packetPing );

	protected:
		void _OnConnect();

		void AssignToCompletionPort();

		inline void SetCompletionPort(HANDLE cp) { m_CompletionPort = cp; }

		void Accept(sockaddr_in * address);

		inline void IncSendLock() { InterlockedIncrement(&m_IsSending); }
		inline void DecSendLock() { InterlockedDecrement(&m_IsSending); }

		/// 向IOCP请求接收数据包
		void StartRecv();

		/// 向IOCP请求发送数据包
		void StartSend();

		/// 接收到数据
		void OnRecvBuffer( unsigned int len );

		/// 发送了数据
		void OnSendBuffer( unsigned int len );

		/// 处理数据包
		void ProcessPackets();

		void Update();

	protected:
		unsigned int m_ConnectorId;
		SocketHandler* m_SocketHandler;			/// 供逻辑程序调用的接口,不要在多线程中使用!!!
		bool m_Alloc;							/// 是否已经分配了内存
		bool m_IsConnected;
		bool m_Deleted;
		volatile long m_IsSending;				/// 是否正在发送数据(原子锁)

		SOCKET m_Socket;
		sockaddr_in m_ClientAddress;
		HANDLE m_CompletionPort;

		OverlappedStruct m_RecvEvent;
		OverlappedStruct m_SendEvent;

		WinLock m_SendLock;
		WinLock m_RecvLock;

		unsigned int	m_MaxSendPacketSize;	/// 发包的大小限制
		unsigned int	m_MaxRecvPacketSize;	/// 收包的大小限制
		//PacketBuffer	m_RecvPacketBuffer;		/// 收包缓存(连续的环形缓存)
		CircularBuffer	m_RecvPacketBuffer;		/// 收包缓存(非连续的环形缓存)
		CircularBuffer	m_SendPacketBuffer;		/// 发包缓存(非连续的环形缓存)
		PacketHeader	m_PacketHeader;			/// 读取的消息头
		bool			m_HaveReadPacketHeader;	/// 是否已经读取了消息头

		PacketHeader*	m_CurrentPacket;				/// 正在接收的数据包
		LockQueue<PacketHeader*,WinLock> m_PacketQueue;	/// 数据包队列

		uint	m_StartTick;					/// 开始时刻
		uint	m_LastSendTick;					/// 发送数据包的最后时刻
		uint	m_LastRecvTick;					/// 收到数据包的最后时刻
		uint	m_LastPingTick;					/// 最后ping的时刻
		uint	m_LastPongTick;					/// 最后收到pong的时刻
		uint	m_LastActiveTick;				/// 最后活动时刻,包括收包和发包
		uint	m_LastSamplingTick;				/// 上次采样时刻
		uint	m_SendSamplingSize;				/// 采样期间的发送量
		uint	m_RecvSamplingSize;				/// 采样期间的接受量
		uint	m_SendPerSecond;				/// 每秒发送的数据量
		uint	m_RecvPerSecond;				/// 每秒接受的数据量
		uint	m_TotalSendSize;				/// 总发送量
		uint	m_TotalRecvSize;				/// 总接收量
		uint	m_Ping;							/// Ping值

		EncryptPacketAPI	m_EncryptPacketAPI;
	};

}
