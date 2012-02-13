#include "WeDbServer.h"
#include <stdio.h>
#include "WeSocket.h"
#include "WeSocketMgr.h"
#include "WeLogInfo.h"
#include "WeGame2DbPackets_Inner.h"
#include "WeDb2GamePackets_Inner.h"
#include "WeClient2GamePackets_Inner.h"
#include "WeGame2ClientPackets_Inner.h"
#include "WePlayerStruct.h"
#include "WeMysql.h"

namespace We
{
	/// 线程:处理查询请求
	unsigned long WINAPI Thread_ProcessQueryList(void* arg)
	{
		DbServer* dbServer = (DbServer*)arg;
		dbServer->m_IsProcessQueryList = true;
		while( dbServer->m_Running )
		{
			QueryBufferHeader* queryBuffer = 0;
			while( queryBuffer = dbServer->m_QueryList.Pop() )
			{
				dbServer->OnProcessQuery( queryBuffer );
				dbServer->FreeQueryBuffer( queryBuffer );
			}
			Sleep( 1 );
		}
		dbServer->m_IsProcessQueryList = false;
		return 1;
	}
	DbServer::DbServer()
	{
		m_TotalRecvMsgSizeSec = 0;
		m_TotalRecvMsgSize = 0;
		m_TotalSendMsgSizeSec = 0;
		m_TotalSendMsgSize = 0;

		m_LastSecTick = ::GetTickCount();
		m_LastShowTick = m_LastSecTick;
		m_LastSendTick = m_LastSecTick;

		m_DbServerId = 0;
		m_GameListenPort = 10000;
		m_MaxGameConnection = 0;
		m_Running = false;
		m_IsProcessQueryList = false;

		m_Mysql = new Mysql();
	}
	DbServer::~DbServer()
	{
	}
	bool DbServer::Init()
	{
		char path[256] = "";
		::GetCurrentDirectory(sizeof(path),path);
		string m_CfgPath = path;
		m_CfgPath += "/DbServer.ini";

		char tmp[1024];
		::GetPrivateProfileString( "DbServer", "Id", "1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbServerId = ::atoi( tmp );

		::GetPrivateProfileString( "GameServer", "ListenIP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GameListenIP = tmp;
		::GetPrivateProfileString( "GameServer", "ListenPort", "20004", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GameListenPort = (uint16)::atoi( tmp );
		::GetPrivateProfileString( "GameServer", "MaxConnection", "10", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_MaxGameConnection = ::atoi( tmp );

		::GetPrivateProfileString( "Db", "IP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbIP = tmp;
		::GetPrivateProfileString( "Db", "Port", "3306", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbPort = (uint16)::atoi( tmp );
		::GetPrivateProfileString( "Db", "User", "root", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbUser = tmp;
		::GetPrivateProfileString( "Db", "Pwd", "123", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbPwd = tmp;
		::GetPrivateProfileString( "Db", "DbName", "testserver", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbName = tmp;

		if( !m_Mysql->Connect( m_DbIP.c_str(), m_DbPort, m_DbUser.c_str(), m_DbPwd.c_str(), m_DbName.c_str() ) )
		{
			LogInfo( "Connect Db failed, IP=%s Port=%d User=%s Pwd=%s DbName=%s", m_DbIP.c_str(), m_DbPort, m_DbUser.c_str(), m_DbPwd.c_str(), m_DbName.c_str() );
			return false;
		}
		else
		{
			LogInfo( "Connect Db OK, IP=%s Port=%d DbName=%s", m_DbIP.c_str(), m_DbPort, m_DbName.c_str() );
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
		DWORD threadID = 0;
		CreateThread(NULL, 0, &Thread_ProcessQueryList, this, 0, (LPDWORD)&threadID);
		return true;
	}
	void DbServer::Shutdown()
	{
		/// 关闭前,要把当前的查询都执行完
		/// 防止维护服务器时,造成数据丢失!!!!!!
		/// ......
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
	void DbServer::OnAccept( Socket* socket )
	{
		GameHandler* handler = new GameHandler();
		handler->m_DbServer = this;
		socket->SetSocketHandler( handler );
		handler->SetSocket( socket );
		LogInfo( "OnAccept GameServer ip=%s port=%d", socket->GetRemoteIP().c_str(), socket->GetRemotePort() );
	}
	void DbServer::OnRemove( Socket* socket )
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
	bool DbServer::SendToGame( uint32 gameServerId, PacketHeader* packet )
	{
		map<uint32, GameHandler*>::iterator i = m_GameServers.find( gameServerId );
		if( i == m_GameServers.end() )
			return false;
		return i->second->SendPacket( packet );
	}
	void DbServer::OnAddGameServer( GameHandler* gameHandler, PacketHeader* packet )
	{
		assert( gameHandler->m_GameServerId == 0 );
		Packet_GS2D_Inner_GameServerInfo* packetGameServerInfo = (Packet_GS2D_Inner_GameServerInfo*)packet;
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
	void DbServer::OnRemoveGameServer( PacketHeader* packet )
	{
	}
	void DbServer::PushQueryBuffer( uint32 gameServerId, PacketHeader* packet )
	{
		QueryBufferHeader* queryBuffer = 0;
		/// Use ObjectPool......
		if( packet->m_Length <= 1024-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferSmall();
		}
		else if( packet->m_Length <= 4096-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferMid();
		}
		else if( packet->m_Length <= 40960-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferBig();
		}
		else
		{
			assert( !"DbServer::PushQueryBuffer size too big" );
			return;
		}
		queryBuffer->m_GameServerId = gameServerId;
		::memcpy( ((uint8*)queryBuffer) + sizeof(QueryBufferHeader), packet, packet->m_Length );
		m_QueryList.Push( queryBuffer );
	}
	void DbServer::FreeQueryBuffer( QueryBufferHeader* buffer )
	{
		/// Use ObjectPool......
		delete buffer;
	}
	QueryBufferHeader* DbServer::AllocResultBuffer( uint32 gameServerId, uint32 bufferSize )
	{
		QueryBufferHeader* queryBuffer = 0;
		/// Use ObjectPool......
		if( bufferSize <= 1024-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferSmall();
		}
		else if( bufferSize <= 4096-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferMid();
		}
		else if( bufferSize <= 40960-sizeof(QueryBufferHeader) )
		{
			queryBuffer = new QueryBufferBig();
		}
		else
		{
			assert( !"DbServer::AllocResultBuffer size too big" );
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
	void DbServer::FreeResultBuffer( QueryBufferHeader* buffer )
	{
		/// Use ObjectPool......
		delete buffer;
	}
	void DbServer::OnProcessQuery( QueryBufferHeader* buffer )
	{
		PacketHeader* packet = (PacketHeader*)(buffer + 1);
		if( packet->m_MainType == 0 )
		{
			switch( packet->m_SubType )
			{
			case PT_GS2D_Inner_RoleList:
				OnRoleList( buffer );
				break;
			case PT_GS2D_Inner_CreateRole:
				OnCreateRole( buffer );
				break;
			case PT_GS2D_Inner_ChooseRole:
				OnChooseRole( buffer );
				break;
			case PT_GS2D_Inner_DeleteRole:
				OnDeleteRole( buffer );
				break;
			case PT_GS2D_Inner_SaveRoleData:
				OnSaveRoleData( buffer );
				break;
			default:
				break;
			}
		}
	}
	void DbServer::Update()
	{
		SocketMgr::getSingletonPtr()->Update();
		QueryBufferHeader* resultBuffer = 0;
		while( resultBuffer = m_QueryResultList.Pop() )
		{
			PacketHeader* packet = (PacketHeader*)(resultBuffer+1);
			SendToGame( resultBuffer->m_GameServerId, packet );
			FreeResultBuffer( resultBuffer );
		}
	}

	void DbServer::OnInputCommand( const char* cmd )
	{
	}

	void DbServer::OnRoleList( QueryBufferHeader* buffer )
	{
		Packet_GS2D_Inner_RoleList* packeRoleList = (Packet_GS2D_Inner_RoleList*)(buffer+1);
		assert( packeRoleList->m_Length == sizeof(Packet_GS2D_Inner_RoleList) );
		if( packeRoleList->m_Length != sizeof(Packet_GS2D_Inner_RoleList) )
			return;
		uint32 roleId;
		uint32 accountId;
		uint8 roleIndex;
		char roleName[MAX_ROLE_NAME+1];
		roleName[0] = 0;
		uint8 sex;
		uint8 race;
		uint32 onlineTime;
		SRoleList_RoleInfo roleInfo;
		QueryBufferHeader* resultBuffer = AllocResultBuffer( buffer->m_GameServerId, sizeof(Packet_D2GS_Inner_RoleListResult) );
		Packet_D2GS_Inner_RoleListResult* packetRoleListResult = (Packet_D2GS_Inner_RoleListResult*)(resultBuffer+1);
		packetRoleListResult->m_MainType = 0;
		packetRoleListResult->m_SubType = PT_D2GS_Inner_RoleListResult;
		packetRoleListResult->m_RoleCount = 0;
		packetRoleListResult->m_AccountId = packeRoleList->m_AccountId;
		SRoleList_RoleInfo* pRoleInfo = (SRoleList_RoleInfo*)packetRoleListResult->m_RoleListData;
		/// Execute SQL......
		char sql[1024];
		::sprintf( sql, "call p_GetRoleList(%u)", packeRoleList->m_AccountId );
		QueryResult sqlResult;
		if( m_Mysql->ExecuteSTMT( sql, &sqlResult, 0, 0 ) )
		{
			MysqlBind<8> binds;
			binds.BindULong( 0, (unsigned long *)&roleId );
			binds.BindULong( 1, (unsigned long *)&accountId );
			binds.BindUTiny( 2, &roleIndex );
			binds.BindString( 3, roleName, sizeof(roleName) );
			binds.BindUTiny( 4, &sex );
			binds.BindUTiny( 5, &race );
			binds.BindULong( 6, (unsigned long *)&onlineTime );
			binds.BindBlob( 7, &roleInfo, sizeof(roleInfo) );
			while( sqlResult.FetchRow( binds.GetBinds() ) )
			{
				packetRoleListResult->m_RoleCount++;
				pRoleInfo->m_RoleId = roleId;
				pRoleInfo->m_RoleIndex = roleIndex;
				::strcpy_s( pRoleInfo->m_RoleName, sizeof(pRoleInfo->m_RoleName), roleName );
				pRoleInfo++;
			}
		}
		packetRoleListResult->m_Length = sizeof(Packet_D2GS_Inner_RoleListResult) - (MAX_ROLE_NUM-packetRoleListResult->m_RoleCount)*sizeof(SRoleList_RoleInfo);

		LogInfo( "OnRoleList AccountId=%u", packeRoleList->m_AccountId );

		m_QueryResultList.Push( resultBuffer );
	}

	void DbServer::OnCreateRole( QueryBufferHeader* buffer )
	{
		Packet_GS2D_Inner_CreateRole* packeCreateRole = (Packet_GS2D_Inner_CreateRole*)(buffer+1);
		assert( packeCreateRole->m_Length == sizeof(Packet_GS2D_Inner_CreateRole)-MAX_CREATEROLE_SIZE+sizeof(SCreateRoleInfo) );
		if( packeCreateRole->m_Length != sizeof(Packet_GS2D_Inner_CreateRole)-MAX_CREATEROLE_SIZE+sizeof(SCreateRoleInfo) )
			return;
		assert( packeCreateRole->m_RoleIndex < MAX_ROLE_NUM );
		if( packeCreateRole->m_RoleIndex >= MAX_ROLE_NUM )
			return;
		packeCreateRole->m_RoleName[MAX_ROLE_NAME] = 0;
		SCreateRoleInfo* createRoleInfo = (SCreateRoleInfo*)packeCreateRole->m_CreateRoleData;

		SPlayerVisual visual;
		::memset( &visual, 0, sizeof(visual) );
		visual.m_FaceId = createRoleInfo->m_FaceId;
		visual.m_HairId = createRoleInfo->m_HairId;
		
		uint8 ret = Packet_D2GS_Inner_CreateRoleResult::CreateRoleResult_Unknown;
		uint32 roleId = 0;

		/// Execute SQL......
		char sql[1024];
		::sprintf( sql, "call p_CreateRole(%u,%d,'%s',%d,%d,?)", packeCreateRole->m_AccountId, packeCreateRole->m_RoleIndex,
			packeCreateRole->m_RoleName, createRoleInfo->m_Sex, createRoleInfo->m_Race );
		MysqlBind<1> binds;
		binds.BindBlob( 0, &visual, sizeof(visual) );
		QueryResult sqlResult;
		if( m_Mysql->ExecuteSTMT( sql, &sqlResult, binds.GetBindCount(), binds.GetBinds() ) )
		{
			char retStr[64];
			retStr[0] = 0;
			MysqlBind<1> bindResult;
			bindResult.BindString( 0, retStr,64 );
			if( sqlResult.FetchRow( bindResult.GetBinds() ) )
			{
				if( retStr[0] != 0 )
				{
					if( strcmp( "-1", retStr ) == 0 )
						ret = Packet_D2GS_Inner_CreateRoleResult::CreateRoleResult_RoleNameExist;
					else if( strcmp( "-2", retStr ) == 0 )
						ret = Packet_D2GS_Inner_CreateRoleResult::CreateRoleResult_RoleIndexExist;
					else if( strcmp( "-3", retStr ) == 0 )
						ret = Packet_D2GS_Inner_CreateRoleResult::CreateRoleResult_SQLError;
					else
					{
						roleId = (uint32)::atol( retStr );
						if( roleId > 0 )
							ret = Packet_D2GS_Inner_CreateRoleResult::CreateRoleResult_OK;
					}
				}
			}
		}

		QueryBufferHeader* resultBuffer = AllocResultBuffer( buffer->m_GameServerId, sizeof(Packet_D2GS_Inner_CreateRoleResult) );
		Packet_D2GS_Inner_CreateRoleResult* packetRoleListResult = (Packet_D2GS_Inner_CreateRoleResult*)(resultBuffer+1);
		packetRoleListResult->m_MainType	= 0;
		packetRoleListResult->m_SubType		= PT_D2GS_Inner_CreateRoleResult;
		packetRoleListResult->m_AccountId	= packeCreateRole->m_AccountId;
		packetRoleListResult->m_Result		= ret;
		packetRoleListResult->m_RoleIndex	= packeCreateRole->m_RoleIndex;
		packetRoleListResult->m_RoleId		= roleId;
		::strcpy_s( packetRoleListResult->m_RoleName, MAX_ROLE_NAME+1, packeCreateRole->m_RoleName );
		if( ret == Packet_D2GS_Inner_CreateRoleResult::CreateRoleResult_OK )
		{
			SRoleList_RoleInfo* pRoleInfo = (SRoleList_RoleInfo*)packetRoleListResult->m_CreateRoleData;
			pRoleInfo->m_RoleIndex = packeCreateRole->m_RoleIndex;
			pRoleInfo->m_RoleId = roleId;
			::strcpy_s( pRoleInfo->m_RoleName, MAX_ROLE_NAME+1, packeCreateRole->m_RoleName );
			pRoleInfo->m_Sex = createRoleInfo->m_Sex;
			pRoleInfo->m_Race = createRoleInfo->m_Race;
			pRoleInfo->m_Visual = visual;
			pRoleInfo->m_OnlineTime = 0;
			packetRoleListResult->m_Length = sizeof(Packet_D2GS_Inner_CreateRoleResult)-MAX_CREATEROLE_SIZE+sizeof(SRoleList_RoleInfo);
		}
		else
			packetRoleListResult->m_Length = sizeof(Packet_D2GS_Inner_CreateRoleResult)-MAX_CREATEROLE_SIZE;

		LogInfo( "OnCreateRole AccountId=%u RoleName=%s", packeCreateRole->m_AccountId, packeCreateRole->m_RoleName );

		m_QueryResultList.Push( resultBuffer );
	}

	void DbServer::OnChooseRole( QueryBufferHeader* buffer )
	{
		Packet_GS2D_Inner_ChooseRole* packeChooseRole = (Packet_GS2D_Inner_ChooseRole*)(buffer+1);
		assert( packeChooseRole->m_Length == sizeof(Packet_GS2D_Inner_ChooseRole) );
		if( packeChooseRole->m_Length != sizeof(Packet_GS2D_Inner_ChooseRole) )
			return;

		QueryBufferHeader* resultBuffer = AllocResultBuffer( buffer->m_GameServerId, sizeof(Packet_D2GS_Inner_ChooseRoleResult) );
		Packet_D2GS_Inner_ChooseRoleResult* packetChooseRoleResult = (Packet_D2GS_Inner_ChooseRoleResult*)(resultBuffer+1);
		packetChooseRoleResult->m_MainType	= 0;
		packetChooseRoleResult->m_SubType	= PT_D2GS_Inner_ChooseRoleResult;
		packetChooseRoleResult->m_AccountId = packeChooseRole->m_AccountId;
		SRoleInfo_DB* pRoleInfo = (SRoleInfo_DB*)packetChooseRoleResult->m_RoleData;
		::memset( pRoleInfo, 0, sizeof(SRoleInfo_DB) );
		pRoleInfo->m_RoleId = packeChooseRole->m_RoleId;
		uint result = Packet_D2GS_Inner_ChooseRoleResult::ChooseRoleResult_Unknown;

		/// Execute SQL......
		char sql[1024];
		::sprintf( sql, "call p_ChooseRole(%u)", packeChooseRole->m_RoleId );
		QueryResult sqlResult;
		if( m_Mysql->ExecuteSTMT( sql, &sqlResult, 0, 0 ) )
		{
			uint8 sex = 0;
			uint8 race = 0;
			MysqlBind<12> binds;
			binds.BindUTiny( 0, &(pRoleInfo->m_RoleIndex) );
			binds.BindString( 1, pRoleInfo->m_RoleName, sizeof(pRoleInfo->m_RoleName) );
			binds.BindUTiny( 2, &sex );
			binds.BindUTiny( 3, &race );
			binds.BindULong( 4, (unsigned long *)&pRoleInfo->m_OnlineTime );
			binds.BindBlob( 5, &(pRoleInfo->m_Visual), sizeof(pRoleInfo->m_Visual) );
			binds.BindUShort( 6, &(pRoleInfo->m_MapId) );
			binds.BindLong( 7, (long*)&(pRoleInfo->m_Pos.x) );
			binds.BindLong( 8, (long*)&(pRoleInfo->m_Pos.y) );
			binds.BindLong( 9, (long*)&(pRoleInfo->m_Pos.z) );
			binds.BindBlob( 10, &(pRoleInfo->m_Bag), sizeof(pRoleInfo->m_Bag) );
			binds.BindBlob( 11, pRoleInfo->m_Data, sizeof(pRoleInfo->m_Data) );
			if( sqlResult.FetchRow( binds.GetBinds() ) )
			{
				pRoleInfo->m_Sex = sex;
				pRoleInfo->m_Race = race;
				result = Packet_D2GS_Inner_ChooseRoleResult::ChooseRoleResult_OK;
			}
			else
				result = Packet_D2GS_Inner_ChooseRoleResult::ChooseRoleResult_SQLError;
		}
		else
			result = Packet_D2GS_Inner_ChooseRoleResult::ChooseRoleResult_SQLError;

		packetChooseRoleResult->m_Result = result;
		packetChooseRoleResult->m_Length = sizeof(Packet_D2GS_Inner_ChooseRoleResult)-MAX_ROLE_DATA_SIZE+sizeof(SRoleInfo_DB);

		LogInfo( "OnChooseRole RoleName=%s", pRoleInfo->m_RoleName );

		m_QueryResultList.Push( resultBuffer );
	}

	void DbServer::OnDeleteRole( QueryBufferHeader* buffer )
	{
		LogInfo( "OnDeleteRole RoleName=%s", "NOT Finish!!!!" );
	}

	void DbServer::OnSaveRoleData( QueryBufferHeader* buffer )
	{
		LogInfo( "OnSaveRoleData RoleName=%s", "NOT Finish!!!!" );
	}
}