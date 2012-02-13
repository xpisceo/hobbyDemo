#include "WeAccountServer.h"
#include <stdio.h>
#include "WeSocket.h"
#include "WeSocketMgr.h"
#include "WeLogInfo.h"
#include "WeGameHandler.h"
#include "WeAccount2GamePackets_Inner.h"
#include "WeGame2AccountPackets_Inner.h"
#include "WeMysql.h"

namespace We
{
	/// 线程:处理查询请求
	unsigned long WINAPI Thread_ProcessQueryList(void* arg)
	{
		AccountServer* accountServer = (AccountServer*)arg;
		accountServer->m_IsProcessQueryList = true;
		while( accountServer->m_Running )
		{
			QueryBufferHeader* queryBuffer = 0;
			while( queryBuffer = accountServer->m_QueryList.Pop() )
			{
				accountServer->OnProcessQuery( queryBuffer );
				accountServer->FreeQueryBuffer( queryBuffer );
			}
			Sleep( 1 );
		}
		accountServer->m_IsProcessQueryList = false;
		return 1;
	}

	AccountServer::AccountServer()
	{
		m_TotalRecvMsgSizeSec = 0;
		m_TotalRecvMsgSize = 0;
		m_TotalSendMsgSizeSec = 0;
		m_TotalSendMsgSize = 0;

		m_LastSecTick = ::GetTickCount();
		m_LastShowTick = m_LastSecTick;
		m_LastSendTick = m_LastSecTick;

		m_AccountServerId = 0;
		m_GameListenPort = 10000;
		m_MaxGameConnection = 0;
		m_Running = false;
		m_IsProcessQueryList = false;

		m_Mysql = new Mysql();
	}
	AccountServer::~AccountServer()
	{
	}
	bool AccountServer::Init()
	{
		char path[256] = "";
		::GetCurrentDirectory(sizeof(path),path);
		string m_CfgPath = path;
		m_CfgPath += "/AccountServer.ini";

		char tmp[1024];
		::GetPrivateProfileString( "AccountServer", "AccountServerId", "1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountServerId = ::atoi( tmp );

		::GetPrivateProfileString( "GameServer", "ListenIP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GameListenIP = tmp;
		::GetPrivateProfileString( "GameServer", "ListenPort", "20003", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GameListenPort = (uint16)::atoi( tmp );
		::GetPrivateProfileString( "GameServer", "MaxConnection", "1000", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_MaxGameConnection = ::atoi( tmp );

		::GetPrivateProfileString( "AccountDb", "IP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountIP = tmp;
		::GetPrivateProfileString( "AccountDb", "Port", "3306", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountPort = (uint16)::atoi( tmp );
		::GetPrivateProfileString( "AccountDb", "User", "root", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountUser = tmp;
		::GetPrivateProfileString( "AccountDb", "Pwd", "123", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountPwd = tmp;
		::GetPrivateProfileString( "AccountDb", "DbName", "customer", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountDbName = tmp;

		if( !m_Mysql->Connect( m_AccountIP.c_str(), m_AccountPort, m_AccountUser.c_str(), m_AccountPwd.c_str(), m_AccountDbName.c_str() ) )
		{
			LogInfo( "Connect Account Db failed, IP=%s Port=%d User=%s Pwd=%s DbName=%s", m_AccountIP.c_str(), m_AccountPort, m_AccountUser.c_str(), m_AccountPwd.c_str(), m_AccountDbName.c_str() );
			return false;
		}
		else
		{
			LogInfo( "Connect Account Db OK, IP=%s Port=%d DbName=%s", m_AccountIP.c_str(), m_AccountPort, m_AccountDbName.c_str() );
		}

		SocketMgr* sockMgr = new SocketMgr();
		sockMgr->AddSocketListener( 1, this, m_MaxGameConnection, 1024*100, 1024*100, 1024*1024, 1024*1024, true );
		sockMgr->StartIOCP( 0 );
		if( !sockMgr->StartListen( 1, m_GameListenIP.c_str(), m_GameListenPort ) )
		{
			LogInfo( "StartListen failed, ListenIp=%s ListenPort=%d MaxConnection=%d", m_GameListenIP.c_str(), m_GameListenPort, m_MaxGameConnection );
			return false;
		}
		else
		{
			LogInfo( "StartListen OK, ListenIp=%s ListenPort=%d MaxConnection=%d", m_GameListenIP.c_str(), m_GameListenPort, m_MaxGameConnection );
		}
		m_Running = true;
		/// 开几个线程合适呢??????
		DWORD threadID = 0;
		CreateThread(NULL, 0, &Thread_ProcessQueryList, this, 0, (LPDWORD)&threadID);
		return true;
	}
	void AccountServer::Shutdown()
	{
		m_Running = false;
		while( m_IsProcessQueryList )
		{
			Sleep( 100 );
		}
		SocketMgr::getSingletonPtr()->Shutdown();
		delete SocketMgr::getSingletonPtr();
		map<uint32, GameHandler*>::iterator i = m_GameServers.begin();
		for( i; i!=m_GameServers.end(); ++i )
		{
			delete i->second;
		}
		m_GameServers.clear();
		QueryBufferHeader* buffer = 0;
		while( buffer = m_QueryList.Pop() )
		{
			FreeQueryBuffer( buffer );
		}
		while( buffer = m_QueryResultList.Pop() )
		{
			FreeResultBuffer( buffer );
		}
		if( m_Mysql != 0 )
		{
			m_Mysql->Disconnect();
			delete m_Mysql;
			m_Mysql = 0;
		}
	}
	void AccountServer::OnAccept( Socket* socket )
	{
		GameHandler* handler = new GameHandler();
		handler->m_AccountServer = this;
		socket->SetSocketHandler( handler );
		handler->SetSocket( socket );
		LogInfo( "OnAccept GameServer ip=%s port=%d", socket->GetRemoteIP().c_str(), socket->GetRemotePort() );
	}
	void AccountServer::OnRemove( Socket* socket )
	{
		GameHandler* handler = (GameHandler*)socket->GetSocketHandler();
		if( handler == 0 )
		{
			LogInfo( "OnRemove GameServer handler==0" );
		}
		else
		{
			map<uint32, GameHandler*>::iterator i = m_GameServers.find( handler->m_GameServerId );
			if( i == m_GameServers.end() )
			{
				if( handler->m_GameServerId == 0 )
				{
					delete handler;
				}
				return;
			}
			LogInfo( "OnRemove GameServer Id=%d", handler->m_GameServerId );
			socket->SetSocketHandler( 0 );
			/// use ObjectPool::Free
			delete handler;
			m_GameServers.erase( i );
		}
	}
	bool AccountServer::SendToGame( uint32 gameServerId, PacketHeader* packet )
	{
		map<uint32, GameHandler*>::iterator i = m_GameServers.find( gameServerId );
		if( i == m_GameServers.end() )
			return false;
		return i->second->SendPacket( packet );
	}
	void AccountServer::OnAddGameServer( GameHandler* gameHandler, PacketHeader* packet )
	{
		assert( gameHandler->m_GameServerId == 0 );
		Packet_GS2A_Inner_GameServerInfo* packetGameServerInfo = (Packet_GS2A_Inner_GameServerInfo*)packet;
		map<uint32, GameHandler*>::iterator i = m_GameServers.find( packetGameServerInfo->m_GameServerId );
		if( i != m_GameServers.end() )
		{
			if( i->second != gameHandler )
			{
				delete i->second;
			}
			m_GameServers.erase( i );
		}
		gameHandler->m_GameServerId = packetGameServerInfo->m_GameServerId;
		gameHandler->m_GameServerStatus = GameServerStatus_Running;
		m_GameServers[gameHandler->m_GameServerId] = gameHandler;
		LogInfo( "OnAddGameServer Id=%d Ip=%s Port=%d", gameHandler->m_GameServerId, gameHandler->GetSocket()->GetRemoteIP().c_str(), gameHandler->GetSocket()->GetRemotePort() );
	}
	void AccountServer::OnRemoveGameServer( PacketHeader* packet )
	{
	}
	void AccountServer::PushQueryBuffer( uint32 gameServerId, PacketHeader* packet )
	{
		QueryBufferHeader* queryBuffer = 0;
		/// Use ObjectPool......(should lock)
		if( packet->m_Length <= 256-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferSmall();
		}
		else if( packet->m_Length <= 1024-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferMid();
		}
		else if( packet->m_Length <= 4096-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferBig();
		}
		else
		{
			assert( !"AccountServer::PushQueryBuffer size too big" );
			return;
		}
		queryBuffer->m_GameServerId = gameServerId;
		::memcpy( ((uint8*)queryBuffer) + sizeof(QueryBufferHeader), packet, packet->m_Length );
		m_QueryList.Push( queryBuffer );
	}
	void AccountServer::FreeQueryBuffer( QueryBufferHeader* buffer )
	{
		/// Use ObjectPool...... (should lock)
		delete buffer;
	}
	QueryBufferHeader* AccountServer::AllocResultBuffer( uint32 gameServerId, uint32 bufferSize )
	{
		QueryBufferHeader* queryBuffer = 0;
		/// Use ObjectPool......(should lock)
		if( bufferSize <= 256-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferSmall();
		}
		else if( bufferSize <= 1024-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferMid();
		}
		else if( bufferSize <= 4096-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferBig();
		}
		else
		{
			assert( !"AccountServer::AllocResultBuffer size too big" );
			return 0;
		}
		queryBuffer->m_GameServerId = gameServerId;
		if( bufferSize >= sizeof(QueryBufferHeader) + sizeof(PacketHeader) )
		{
			PacketHeader* packetHeader = (PacketHeader*)(queryBuffer+1);
			packetHeader->m_Length = bufferSize;
		}
		return queryBuffer;
	}
	void AccountServer::FreeResultBuffer( QueryBufferHeader* buffer )
	{
		/// Use ObjectPool......(should lock)
		delete buffer;
	}
	void AccountServer::OnProcessQuery( QueryBufferHeader* buffer )
	{
		PacketHeader* packet = (PacketHeader*)(buffer + 1);
		if( packet->m_MainType == 0 )
		{
			switch( packet->m_SubType )
			{
				/// 创建帐号
			case PT_GS2A_Inner_CreateAccount:
				OnCreateAccount( buffer );
				break;
				/// 帐号登录
			case PT_GS2A_Inner_Login:
				OnClientLogin( buffer );
				break;
			/// 封号,解封,
			/// 一些记录信息
			/// ......
			default:
				break;
			}
		}
	}
	void AccountServer::OnClientLogin( QueryBufferHeader* buffer )
	{
		Packet_GS2A_Inner_Login* packeLogin = (Packet_GS2A_Inner_Login*)(buffer+1);

		uint32 accountId = 0;
		/// Execute SQL......
		char sql[1024];
		::sprintf( sql, "call p_Login('%s','%s')", packeLogin->m_AccountName, packeLogin->m_EncryptPwd );
		uint8 result = Packet_A2GS_Inner_LoginResult::LoginResult_Unknown;
		QueryResult sqlResult;
		if( m_Mysql->ExecuteProcedure( sql, &sqlResult, true ) )
		{
			if( sqlResult.FetchRow() )
			{
				accountId = sqlResult.GetUint( 0, 0 );
				if( accountId > 0 )
					result = Packet_A2GS_Inner_LoginResult::LoginResult_OK;
				else
					result = Packet_A2GS_Inner_LoginResult::LoginResult_AccountOrPwdError;
			}
		}
		sqlResult.Close();
		QueryBufferHeader* resultBuffer = AllocResultBuffer( buffer->m_GameServerId, sizeof(Packet_A2GS_Inner_LoginResult) );
		Packet_A2GS_Inner_LoginResult* packetLoginResult = (Packet_A2GS_Inner_LoginResult*)(resultBuffer+1);
		packetLoginResult->m_MainType = 0;
		packetLoginResult->m_SubType = PT_A2GS_Inner_LoginResult;
		packetLoginResult->m_ClientId = packeLogin->m_ClientId;
		packetLoginResult->m_AccountId = accountId;
		packeLogin->m_AccountName[sizeof(packeLogin->m_AccountName)-1] = 0;
		::strcpy_s( packetLoginResult->m_AccountName, sizeof(packetLoginResult->m_AccountName), packeLogin->m_AccountName );
		packetLoginResult->m_Result = result;

		LogInfo( "OnClientLogin AccountName=%s", packeLogin->m_AccountName );

		m_QueryResultList.Push( resultBuffer );
	}
	void AccountServer::OnCreateAccount( QueryBufferHeader* buffer )
	{
		Packet_GS2A_Inner_CreateAccount* packetCreateAccount = (Packet_GS2A_Inner_CreateAccount*)(buffer+1);

		uint32 accountId = 0;
		/// Execute SQL......
		uint8 result = Packet_A2GS_Inner_CreateAccountResult::CreateAccountResult_Unknown;
		char sql[1024];
		::sprintf( sql, "call p_CreateAccount('%s','%s')", packetCreateAccount->m_AccountName, packetCreateAccount->m_EncryptPwd );
		QueryResult sqlResult;
		if( m_Mysql->ExecuteProcedure( sql, &sqlResult, true ) )
		{
			if( sqlResult.FetchRow() )
			{
				const char* retStr = sqlResult.GetValue( 0 );
				if( retStr != 0 )
				{
					if( ::strcmp( retStr, "-1" ) == 0 )
						result = Packet_A2GS_Inner_CreateAccountResult::CreateAccountResult_AccountExist;
					else if( ::strcmp( retStr, "-2" ) == 0 )
						result = Packet_A2GS_Inner_CreateAccountResult::CreateAccountResult_SQLError;
					else
					{
						accountId = (uint32)::atol( retStr );
						if( accountId > 0 )
							result = Packet_A2GS_Inner_CreateAccountResult::CreateAccountResult_OK;
					}
				}
			}
		}
		QueryBufferHeader* resultBuffer = AllocResultBuffer( buffer->m_GameServerId, sizeof(Packet_A2GS_Inner_CreateAccountResult) );
		Packet_A2GS_Inner_CreateAccountResult* packetCreateAccountResult = (Packet_A2GS_Inner_CreateAccountResult*)(resultBuffer+1);
		packetCreateAccountResult->m_MainType = 0;
		packetCreateAccountResult->m_SubType = PT_A2GS_Inner_CreateAccountResult;
		packetCreateAccountResult->m_ClientId = packetCreateAccount->m_ClientId;
		packetCreateAccountResult->m_AccountId = accountId;
		packetCreateAccount->m_AccountName[sizeof(packetCreateAccount->m_AccountName)-1] = 0;
		::strcpy_s( packetCreateAccountResult->m_AccountName, sizeof(packetCreateAccountResult->m_AccountName), packetCreateAccount->m_AccountName );
		packetCreateAccountResult->m_Result = result;

		m_QueryResultList.Push( resultBuffer );
	}

	void AccountServer::Update()
	{
		SocketMgr::getSingletonPtr()->Update();

		QueryBufferHeader* resultBuffer = 0;
		while( resultBuffer = m_QueryResultList.Pop() )
		{
			PacketHeader* packet = (PacketHeader*)(resultBuffer+1);
			SendToGame( resultBuffer->m_GameServerId, packet );
			FreeResultBuffer( resultBuffer );
		}
		//unsigned int currTick = ::GetTickCount();
		//if( currTick - m_LastSecTick >= 1000 )
		//{
		//	m_LastSecTick += 1000;
		//	if( currTick - m_LastSecTick > 1000 )
		//		m_LastSecTick = currTick;
		//	if( currTick - m_LastShowTick >= 5*1000 )
		//	{
		//		m_LastShowTick = currTick;
		//		if( m_TotalRecvMsgSizeSec > 1024*1024 )
		//			LogInfo( "RecvSpeed=%.2fM,", (float)m_TotalRecvMsgSizeSec/(1024*1024) );
		//		else
		//			LogInfo( "RecvSpeed=%.2fK,", (float)m_TotalRecvMsgSizeSec/(1024) );
		//		if( m_TotalSendMsgSizeSec > 1024*1024 )
		//			LogInfo( "SendSpeed=%.2fM,", (float)m_TotalSendMsgSizeSec/(1024*1024) );
		//		else
		//			LogInfo( "SendSpeed=%.2fK,", (float)m_TotalSendMsgSizeSec/(1024) );
		//		LogInfo( "TotalRecv=%.2fM,TotalSend=%.2fM", (float)m_TotalRecvMsgSize/(1024*1024), (float)m_TotalSendMsgSize/(1024*1024) );
		//		LogInfo( " Online=%d", m_ClientHandlers.size() );
		//	}
		//	m_TotalRecvMsgSizeSec = 0;
		//	m_TotalSendMsgSizeSec = 0;
		//}
	}

	void AccountServer::OnInputCommand( const char* cmd )
	{
	}
}