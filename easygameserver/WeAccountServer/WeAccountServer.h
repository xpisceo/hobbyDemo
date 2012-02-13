/********************************************************************
	created:	2009-6-29
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	帐号服务器
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"
#include "WeQueue.h"
#include "WeLock.h"

namespace We
{
	class GameHandler;
	class Mysql;

#pragma pack(1)
	struct QueryBufferHeader
	{
		uint32 m_GameServerId;
	};

	/// 0.25K
	struct QueryBufferSmall : QueryBufferHeader
	{
		uint8 m_Buffer[252];
	};

	/// 1K
	struct QueryBufferMid : QueryBufferHeader
	{
		uint8 m_Buffer[1020];
	};

	/// 4K
	struct QueryBufferBig : QueryBufferHeader
	{
		uint8 m_Buffer[4092];
	};
#pragma pack()

	class AccountServer : public SocketListenerHandler
	{
		friend unsigned long WINAPI Thread_ProcessQueryList(void* arg);	/// 处理查询线程
	public:
		unsigned int m_TotalRecvMsgSizeSec;
		unsigned int m_TotalRecvMsgSize;
		unsigned int m_TotalSendMsgSizeSec;
		unsigned int m_TotalSendMsgSize;

		unsigned int m_LastSecTick;
		unsigned int m_LastShowTick;
		unsigned int m_LastSendTick;

		uint32	m_AccountServerId;
		string	m_GameListenIP;
		uint16	m_GameListenPort;
		uint32	m_MaxGameConnection;

		string	m_AccountIP;
		uint16	m_AccountPort;
		string	m_AccountUser;
		string	m_AccountPwd;
		string	m_AccountDbName;

		bool	m_Running;
		bool	m_IsProcessQueryList;
	protected:
		map<uint32, GameHandler*>	m_GameServers;					/// 与中心服务器的连接(注:可以用数组代替map)

		LockQueue<QueryBufferHeader*, WinLock> m_QueryList;			/// 查询消息队列
		LockQueue<QueryBufferHeader*, WinLock> m_QueryResultList;	/// 查询结果队列

		Mysql* m_Mysql;
	public:
		AccountServer();
		~AccountServer();

		inline uint32 GetAccountServerId() const { return m_AccountServerId; }

		bool Init();
		void Shutdown();

		/// 新客户端连接
		virtual void OnAccept( Socket* socket );
		/// 移除客户端连接
		virtual void OnRemove( Socket* socket );

		/// 发数据包给中心服务器
		bool SendToGame( uint32 gameServerId, PacketHeader* packet );

		/// 新增一个中心服务器
		void OnAddGameServer( GameHandler* gameHandler, PacketHeader* packet );
		/// 移除一个中心服务器
		void OnRemoveGameServer( PacketHeader* packet );

		/// 查询消息
		void PushQueryBuffer( uint32 gameServerId, PacketHeader* packet );
		void FreeQueryBuffer( QueryBufferHeader* buffer );

		/// 查询结果
		QueryBufferHeader* AllocResultBuffer( uint32 gameServerId, uint32 bufferSize );
		void FreeResultBuffer( QueryBufferHeader* buffer );

		/// 线程里调用: 处理查询请求
		void OnProcessQuery( QueryBufferHeader* buffer );

		/// 客户端登录
		void OnClientLogin( QueryBufferHeader* buffer );

		/// 创建帐号
		void OnCreateAccount( QueryBufferHeader* buffer );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}