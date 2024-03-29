#include "WeCenterServer.h"
#include "WeSocketMgr.h"
#include "WeLogInfo.h"
#include "WeGateHandler.h"
#include "WeMapHandler.h"
#include "WeAccountHandler.h"
#include "WeGate2CenterPackets_Inner.h"
#include "WeAccount2CenterPackets_Inner.h"
#include "WeCenter2AccountPackets_Inner.h"
#include "WeCenter2DbPackets_Inner.h"
#include "WeCenter2MapPackets_Inner.h"
#include "WeMap2CenterPackets_Inner.h"
#include "WePlayerStruct.h"

namespace We
{
	MapServerListener::MapServerListener( CenterServer* centerServer )
	{
		m_CenterServer = centerServer;
	}
	void MapServerListener::OnAccept( Socket* socket )
	{
		m_CenterServer->OnAcceptMapServer( socket );
	}
	void MapServerListener::OnRemove( Socket* socket )
	{
		m_CenterServer->OnRemoveMapServer( socket );
	}
	CenterServer::CenterServer()
	{
		m_MapServerListener = 0;
		m_CenterId = 0;
		m_Maps = 0;
		m_MapArraySize = 0;
		m_MaxMapId = 0;
	}
	CenterServer::~CenterServer()
	{
		if( m_MapServerListener != 0 )
		{
			delete m_MapServerListener;
			m_MapServerListener = 0;
		}
	}
	bool CenterServer::Init()
	{
		char path[256] = "";
		::GetCurrentDirectory(sizeof(path),path);
		string m_CfgPath = path;
		m_CfgPath += "/CenterServer.ini";
		char tmp[1024];

		::GetPrivateProfileString( "CenterServer", "Id", "1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_CenterId = ::atoi( tmp );
		::GetPrivateProfileString( "CenterServer", "MaxMapId", "32", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_MapArraySize = ::atoi( tmp );

		::GetPrivateProfileString( "GateServer", "ListenIP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GateListenIP = tmp;
		::GetPrivateProfileString( "GateServer", "ListenPort", "20001", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GateListenPort = (uint16)::atoi( tmp );
		::GetPrivateProfileString( "GateServer", "MaxConnection", "10", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GateMaxConnection = ::atoi( tmp );

		::GetPrivateProfileString( "MapServer", "ListenIP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_MapListenIP = tmp;
		::GetPrivateProfileString( "MapServer", "ListenPort", "20002", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_MapListenPort = (uint16)::atoi( tmp );
		::GetPrivateProfileString( "MapServer", "MaxConnection", "100", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_MapMaxConnection = ::atoi( tmp );

		::GetPrivateProfileString( "AccountServer", "IP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountServerIP = tmp;
		::GetPrivateProfileString( "AccountServer", "Port", "20003", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountServerPort = (uint16)::atoi( tmp );

		::GetPrivateProfileString( "DbServer", "IP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbServerIP = tmp;
		::GetPrivateProfileString( "DbServer", "Port", "20004", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbServerPort = (uint16)::atoi( tmp );

		m_Maps = new MapHandler*[m_MapArraySize];
		for( int i=0; i<m_MapArraySize; ++i )
			m_Maps[i] = 0;

		m_MapServerListener = new MapServerListener( this );
		SocketMgr* sockMgr = new SocketMgr();
		sockMgr->AddSocketListener( 1, this, m_GateMaxConnection, 1024*50, 1024*10, 1024*1024*5, 1024*1024, true );
		sockMgr->AddSocketListener( 2, m_MapServerListener, m_MapMaxConnection, 1024*50, 1024*50, 1024*1024, 1024*1024, true );
		/// connect to AccountServer
		m_AccountHandler.m_CenterServer = this;
		if( !sockMgr->AddSocketConnector( 10000, &m_AccountHandler, m_AccountServerIP.c_str(), m_AccountServerPort,
			1024*100, 1024*100, 1024*1024, 1024*1024, true ) )
		{
			LogInfo( "Connect AccountServer Failed, Ip=%s Port=%d", m_AccountServerIP.c_str(), m_AccountServerPort );
			return false;
		}
		else
		{
			LogInfo( "Connect AccountServer OK, Ip=%s Port=%d", m_AccountServerIP.c_str(), m_AccountServerPort );
		}
		/// connect to DbServer
		m_DbHandler.m_CenterServer = this;
		if( !sockMgr->AddSocketConnector( 10001, &m_DbHandler, m_DbServerIP.c_str(), m_DbServerPort,
			1024*100, 1024*100, 1024*1024, 1024*1024, true ) )
		{
			LogInfo( "Connect DbServer Failed, Ip=%s Port=%d", m_DbServerIP.c_str(), m_DbServerPort );
			return false;
		}
		else
		{
			LogInfo( "Connect DbServer OK, Ip=%s Port=%d", m_DbServerIP.c_str(), m_DbServerPort );
		}

		sockMgr->StartIOCP( 0 );
		if( !sockMgr->StartListen( 1, m_GateListenIP.c_str(), m_GateListenPort ) )
		{
			LogInfo( "Listen Gate failed, ListenIp=%s ListenPort=%d MaxConnection=%d", m_GateListenIP.c_str(), m_GateListenPort, m_GateMaxConnection );
			return false;
		}
		else
		{
			LogInfo( "Listen Gate OK, ListenIp=%s ListenPort=%d MaxConnection=%d", m_GateListenIP.c_str(), m_GateListenPort, m_GateMaxConnection );
		}
		if( !sockMgr->StartListen( 2, m_MapListenIP.c_str(), m_MapListenPort ) )
		{
			LogInfo( "Listen Map failed, ListenIp=%s ListenPort=%d MaxConnection=%d", m_MapListenIP.c_str(), m_MapListenPort, m_MapMaxConnection );
			return false;
		}
		else
		{
			LogInfo( "Listen Map OK, ListenIp=%s ListenPort=%d MaxConnection=%d", m_MapListenIP.c_str(), m_MapListenPort, m_MapMaxConnection );
		}
		Packet_CS2A_Inner_CenterInfo packetCenterInfo;
		packetCenterInfo.m_CenterId = m_CenterId;
		m_AccountHandler.SendPacket( &packetCenterInfo );
		Packet_CS2D_Inner_CenterInfo packetCenterInfo2DB;
		packetCenterInfo2DB.m_CenterId = m_CenterId;
		m_DbHandler.SendPacket( &packetCenterInfo2DB );
		return true;
	}
	void CenterServer::Shutdown()
	{
		SocketMgr::getSingletonPtr()->Shutdown();
		delete SocketMgr::getSingletonPtr();
	}
	void CenterServer::OnAccept( Socket* socket )
	{
		/// use ObjectPool::Alloc
		GateHandler* handler = new GateHandler();
		handler->m_CenterServer = this;
		socket->SetSocketHandler( handler );
		handler->SetSocket( socket );
		LogInfo( "OnAccept Gate ip=%s port=%d", socket->GetRemoteIP().c_str(), socket->GetRemotePort() );
	}
	void CenterServer::OnRemove( Socket* socket )
	{
		GateHandler* handler = (GateHandler*)socket->GetSocketHandler();
		if( handler == 0 )
		{
			LogInfo( "OnRemove Gate handler==0" );
		}
		else
		{
			map<uint8, GateHandler*>::iterator i = m_GateHandlers.find( handler->m_GateId );
			if( i == m_GateHandlers.end() )
			{
				if( handler->m_GateId == 0 )
				{
					delete handler;
				}
				return;
			}
			/// 连接该网关的玩家已经都掉线了
			/// 这里需要通知这些玩家所在的MapServer做相应的下线处理
			/// 要区分是正常维护还是异常掉线
			LogInfo( "OnRemove Gate Id=%d", handler->m_GateId );
			socket->SetSocketHandler( 0 );
			/// use ObjectPool::Free
			delete handler;
			m_GateHandlers.erase( i );
		}
	}
	void CenterServer::OnAcceptMapServer( Socket* socket )
	{
		/// use ObjectPool::Alloc
		MapHandler* handler = new MapHandler();
		handler->m_CenterServer = this;
		socket->SetSocketHandler( handler );
		handler->SetSocket( socket );
		LogInfo( "OnAccept MapServer ip=%s port=%d", socket->GetRemoteIP().c_str(), socket->GetRemotePort() );
	}
	void CenterServer::OnRemoveMapServer( Socket* socket )
	{
		MapHandler* handler = (MapHandler*)socket->GetSocketHandler();
		if( handler == 0 )
		{
			LogInfo( "OnRemove MapServer handler==0" );
		}
		else
		{
			map<uint16, MapHandler*>::iterator i = m_MapHandlers.find( handler->m_MapServerId );
			if( i == m_MapHandlers.end() )
			{
				return;
			}
			/// 连接该网关的玩家已经都掉线了
			/// 这里需要通知这些玩家所在的MapServer做相应的下线处理
			/// 要区分是正常维护还是异常掉线
			LogInfo( "OnRemove MapServer Id=%d", handler->m_MapServerId );
			socket->SetSocketHandler( 0 );

			for( int i=0; i<m_MapArraySize; ++i )
			{
				if( m_Maps[i] == handler )
					m_Maps[i] = 0;
			}
			/// use ObjectPool::Free
			delete handler;
			m_MapHandlers.erase( i );
		}
	}
	bool CenterServer::SendToGate( uint8 gateId, PacketHeader* packet )
	{
		map<uint8, GateHandler*>::iterator i = m_GateHandlers.find( gateId );
		if( i == m_GateHandlers.end() )
		{
			LogInfo( "SendToGate Not find gate id=%d", gateId );
			return false;
		}
		return i->second->SendPacket( packet );
	}
	void CenterServer::SendToAllGates( PacketHeader* packet )
	{
		map<uint8, GateHandler*>::iterator i = m_GateHandlers.begin();
		for( i; i!=m_GateHandlers.end(); ++i )
		{
			i->second->SendPacket( packet );
		}
	}
	bool CenterServer::SendToMap( uint16 mapId, PacketHeader* packet )
	{
		assert( mapId > 0 && mapId <= m_MaxMapId );
		if( mapId < 1 || mapId > m_MaxMapId )
			return false;
		if( m_Maps[mapId-1] == 0 )
		{
			LogInfo( "SendToMap Not find map id=%d", mapId );
			return false;
		}
		return m_Maps[mapId-1]->SendPacket( packet );
	}
	void CenterServer::SendToAllMaps( PacketHeader* packet )
	{
		map<uint16, MapHandler*>::iterator i = m_MapHandlers.begin();
		for( i; i!=m_MapHandlers.end(); ++i )
		{
			i->second->SendPacket( packet );
		}
	}
	void CenterServer::OnRecv_GateInfo( GateHandler* gateHandler, PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_G2CS_Inner_GateInfo) );
		if( packet->m_Length != sizeof(Packet_G2CS_Inner_GateInfo) )
			return;
		const Packet_G2CS_Inner_GateInfo* packetGateInfo = (const Packet_G2CS_Inner_GateInfo*)packet;
		if( gateHandler->m_GateStatus != GateStatus_WaitForInfo )
		{
			LogInfo( "OnRecvGateInfo != GateStatus_WaitForInfo" );
			return;
		}
		map<uint8, GateHandler*>::iterator i = m_GateHandlers.find( packetGateInfo->m_GateId );
		if( i != m_GateHandlers.end() )
		{
			///
			GateHandler* exitGateHandler = i->second;
			if( exitGateHandler->GetSocket() != 0 )
			{
				exitGateHandler->GetSocket()->SetSocketHandler( 0 );
				exitGateHandler->GetSocket()->Delete();
			}
			delete exitGateHandler; /// use SafeDeleteList?
			m_GateHandlers.erase( i );
		}
		gateHandler->m_GateId = packetGateInfo->m_GateId;
		gateHandler->m_GateStatus = GateStatus_Running;
		m_GateHandlers[gateHandler->m_GateId] = gateHandler;
		LogInfo( "OnRecvGateInfo id=%d", gateHandler->m_GateId );
	}
	void CenterServer::OnRecv_MapServerInfo( MapHandler* mapHandler, PacketHeader* packet )
	{
		Packet_M2CS_Inner_MapServerInfo* packetMapServerInfo = (Packet_M2CS_Inner_MapServerInfo*)packet;
		if( packetMapServerInfo->m_Length < sizeof(Packet_M2CS_Inner_MapServerInfo)-sizeof(SMapInfo_Inner)*MAX_MAPCOUNT_ONEMAPSERVER )
			return;
		uint16 mapCount = packetMapServerInfo->m_MapCount;
		if( mapCount > MAX_MAPCOUNT_ONEMAPSERVER )
			return;
		if( mapCount == 0 )
			return;
		if( packetMapServerInfo->m_Length != sizeof(Packet_M2CS_Inner_MapServerInfo)-sizeof(SMapInfo_Inner)*(MAX_MAPCOUNT_ONEMAPSERVER-mapCount) )
			return;
		SMapInfo_Inner* mapInfo = (SMapInfo_Inner*)packetMapServerInfo->m_MapInfos;
		for( int i=0; i<mapCount; ++i )
		{
			if( mapInfo->m_MapId > 0 && mapInfo->m_MapId <= m_MapArraySize )
			{
				m_Maps[mapInfo->m_MapId-1] = mapHandler;
			}
			else
			{
				assert( false );
			}
			mapInfo++;
		}
	}
	LoginPlayer* CenterServer::GetLoginPlayer( uint32 accountId )
	{
		map<uint32, LoginPlayer*>::iterator i = m_LoginPlayers.find( accountId );
		if( i == m_LoginPlayers.end() )
			return 0;
		return i->second;
	}
	void CenterServer::RemoveLoginPlayer( uint32 accountId )
	{
		map<uint32, LoginPlayer*>::iterator i = m_LoginPlayers.find( accountId );
		if( i == m_LoginPlayers.end() )
			return;
		delete i->second;
		m_LoginPlayers.erase(i);
	}
	CenterPlayer* CenterServer::GetCenterPlayerById( uint32 roleId )
	{
		map<uint32, CenterPlayer*>::iterator i = m_CenterPlayers.find(roleId);
		if( i == m_CenterPlayers.end() )
			return 0;
		return i->second;
	}
	CenterPlayer* CenterServer::GetCenterPlayerByAccountId( uint32 accountId )
	{
		map<uint32, CenterPlayer*>::iterator i = m_CenterPlayers.begin();
		for( i; i!=m_CenterPlayers.end(); ++i )
		{
			if( i->second->m_AccountId == accountId )
			{
				return i->second;
			}
		}
		return 0;
	}
	void CenterServer::OnRecv_Login( GateHandler* gateHandler, PacketHeader* packet )
	{
		/// 先判断该账号是否已经在游戏中
		/// 这里不做处理,防止其他玩家恶意发送登录消息,导致该帐号玩家不能正常游戏,而是在返回登录成功时,再做处理
		/// 转发给AccountServer
		/// 是否在这里生成一个唯一的标识并记录下来,以便后续的验证??????
		m_AccountHandler.SendPacket( packet );
	}
	void CenterServer::OnRecv_LoginResult( PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_CS2C_Inner_LoginResult) );
		if( packet->m_Length != sizeof(Packet_CS2C_Inner_LoginResult) )
			return;
		Packet_CS2C_Inner_LoginResult* packetLoginResult = (Packet_CS2C_Inner_LoginResult*)packet;
		/// 如果登录成功
		if( packetLoginResult->m_Result == Packet_CS2C_Inner_LoginResult::LoginResult_OK )
		{
			LoginPlayer* loginPlayer = 0;
			/// 先判断该账号是否已经在登录列表中
			map<uint32, LoginPlayer*>::iterator i = m_LoginPlayers.find( packetLoginResult->m_AccountId );
			if( i != m_LoginPlayers.end() )
			{
				/// 什么情况下,会到这里???
				loginPlayer = i->second;
				loginPlayer->m_Id = packetLoginResult->m_PlayerID;
				loginPlayer->m_AccountId = packetLoginResult->m_AccountId;
				loginPlayer->m_AccountName = packetLoginResult->m_AccountName;
				loginPlayer->m_LoginTime = ::timeGetTime(); /// 记录登录时间
				loginPlayer->m_GateId = packetLoginResult->m_GateId;
				LogInfo( "Exist LoginPlayer AccountName=%s", loginPlayer->m_AccountName.c_str() );
			}
			else
			{
				loginPlayer = new LoginPlayer();
				loginPlayer->m_Id = packetLoginResult->m_PlayerID;
				loginPlayer->m_AccountId = packetLoginResult->m_AccountId;
				loginPlayer->m_AccountName = packetLoginResult->m_AccountName;
				loginPlayer->m_LoginTime = ::timeGetTime(); /// 记录登录时间
				loginPlayer->m_GateId = packetLoginResult->m_GateId;
				::memset( loginPlayer->m_RoleIds, 0, sizeof(loginPlayer->m_RoleIds) );
				m_LoginPlayers[loginPlayer->m_AccountId] = loginPlayer;
			}
			/// 判断该账号是否在游戏中
			CenterPlayer* centerPlayer = GetCenterPlayerByAccountId( packetLoginResult->m_AccountId );
			if( centerPlayer != 0 )
			{
				/// 断线重连
				LogInfo( "断线重连 AccountName=%s RoleName=%s", loginPlayer->m_AccountName.c_str(), centerPlayer->m_RoleName.c_str() );
				packetLoginResult->m_Result = Packet_CS2C_Inner_LoginResult::LoginResult_OKAndReconnect;
				centerPlayer->m_GateId = packetLoginResult->m_GateId;
				centerPlayer->m_IsWaitReconnect = true;	/// 标记断线重连
				/// 如果该角色正在游戏中,向该地图服务器发送断线重连的消息
				if( centerPlayer->m_PlayerStatus == CenterPlayerStatus_InMap )
				{
					Packet_CS2M_Inner_PlayerReconnect packetPlayerReconnect;
					packetPlayerReconnect.m_RoleId = centerPlayer->m_RoleId;
					SendToMap( centerPlayer->m_MapId, &packetPlayerReconnect );
				}
				SendToGate( packetLoginResult->m_GateId, packet );
				/// 从登陆列表中移除
				delete loginPlayer;
				m_LoginPlayers.erase( i );
				return;
			}
		}
		/// 返回给客户端
		SendToGate( packetLoginResult->m_GateId, packet );
	}
	void CenterServer::OnRecv_CreateAccount( GateHandler* gateHandler, PacketHeader* packet )
	{
		/// 转发给AccountServer
		m_AccountHandler.SendPacket( packet );
	}
	void CenterServer::OnRecv_CreateAccountResult( PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_CS2C_Inner_CreateAccountResult) );
		if( packet->m_Length != sizeof(Packet_CS2C_Inner_CreateAccountResult) )
			return;
		Packet_CS2C_Inner_CreateAccountResult* packetCreateAccountResult = (Packet_CS2C_Inner_CreateAccountResult*)packet;
		/// 返回给客户端
		SendToGate( packetCreateAccountResult->m_GateId, packet );
	}
	void CenterServer::OnRecv_RoleList( GateHandler* gateHandler, PacketHeader* packet )
	{
		/// 先判断缓存......
		assert( packet->m_Length == sizeof(Packet_C2CS_RoleList) );
		if( packet->m_Length != sizeof(Packet_C2CS_RoleList) )
			return;
		Packet_C2CS_RoleList* packetRoleList = (Packet_C2CS_RoleList*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetRoleList->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_RoleList not find LoginPlayer AccountId=%u", packetRoleList->m_AccountId );
			return;
		}
		if( loginPlayer->m_Id != packetRoleList->m_PlayerID )
		{
			LogInfo( "OnRecv_RoleList Error PlayerID=%u", packetRoleList->m_PlayerID );
			/// 通知GateServer断开该连接
			return;
		}
		m_DbHandler.SendPacket( packet );
	}
	void CenterServer::OnRecv_CreateRole( GateHandler* gateHandler, PacketHeader* packet )
	{
		/// 先判断合法性.....
		/// 考虑到以后合服的可能性，是不是要保证角色名在整个区都是唯一的??????还是用其他方式来处理
		/// 转发给DbServer
		assert( packet->m_Length == sizeof(Packet_C2CS_CreateRole)-MAX_CREATEROLE_SIZE+sizeof(SCreateRoleInfo) );
		if( packet->m_Length != sizeof(Packet_C2CS_CreateRole)-MAX_CREATEROLE_SIZE+sizeof(SCreateRoleInfo) )
			return;
		Packet_C2CS_CreateRole* packetCreateRole = (Packet_C2CS_CreateRole*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetCreateRole->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_CreateRole not find LoginPlayer AccountId=%u", packetCreateRole->m_AccountId );
			return;
		}
		if( loginPlayer->m_AccountId != packetCreateRole->m_AccountId )
		{
			LogInfo( "OnRecv_CreateRole Error AccountId=%u", packetCreateRole->m_AccountId );
			/// 通知GateServer断开该连接
			return;
		}
		if( loginPlayer->m_Id != packetCreateRole->m_PlayerID )
		{
			LogInfo( "OnRecv_CreateRole Error PlayerID=%u", packetCreateRole->m_PlayerID );
			/// 通知GateServer断开该连接
			return;
		}
		if( packetCreateRole->m_RoleIndex >= MAX_ROLE_NUM )
		{
			return;
		}
		if( loginPlayer->m_RoleIds[packetCreateRole->m_RoleIndex] != 0 )
		{
			/// 返回一个错误信息
			LogInfo( "OnRecv_CreateRole Error RoleIndex=%d", packetCreateRole->m_RoleIndex );
			return;
		}
		/// 判断该角色是否在游戏中
		CenterPlayer* centerPlayer = GetCenterPlayerByAccountId( packetCreateRole->m_AccountId );
		if( centerPlayer != 0 )
		{
			/// 帐号登录时,就判断了断线重连,所以不应该到这里!!!!!!
			LogInfo( "OnRecv_CreateRole 严重错误:角色已经在游戏中,不应该再收到创建角色的消息=%s", centerPlayer->m_RoleName.c_str() );
			return;
		}
		m_DbHandler.SendPacket( packet );
	}
	void CenterServer::OnRecv_ChooseRole( GateHandler* gateHandler, PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_C2CS_ChooseRole) );
		if( packet->m_Length != sizeof(Packet_C2CS_ChooseRole) )
			return;
		Packet_C2CS_ChooseRole* packetChooseRole = (Packet_C2CS_ChooseRole*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetChooseRole->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_ChooseRole not find LoginPlayer AccountId=%u", packetChooseRole->m_AccountId );
			return;
		}
		if( loginPlayer->m_Id != packetChooseRole->m_PlayerID )
		{
			LogInfo( "OnRecv_ChooseRole Error PlayerID=%u", packetChooseRole->m_PlayerID );
			/// 通知GateServer断开该连接
			return;
		}
		if( packetChooseRole->m_RoleId < 1 )
			return;
		/// 判断m_RoleId是否正确
		int8 roleIndex = -1;
		for( int i=0; i<MAX_ROLE_NUM; ++i )
		{
			if( packetChooseRole->m_RoleId == loginPlayer->m_RoleIds[i] )
			{
				roleIndex = i;
				break;
			}
		}
		if( roleIndex == -1 )
		{
			LogInfo( "OnRecv_ChooseRole Error RoleId=%u", packetChooseRole->m_RoleId );
			/// 通知GateServer断开该连接
			return;
		}
		/// 判断该角色是否已经在游戏中
		CenterPlayer* centerPlayer = GetCenterPlayerByAccountId( packetChooseRole->m_AccountId );
		if( centerPlayer != 0 )
		{
			/// 帐号登录时,就判断了断线重连,所以不应该到这里!!!!!!
			LogInfo( "OnRecv_ChooseRole 严重错误:角色已经在游戏中,不应该再收到选择角色的消息=%s", centerPlayer->m_RoleName.c_str() );
			return;
			///// 选择的角色正在游戏中
			//if( centerPlayer->m_RoleIndex == packetChooseRole->m_RoleIndex )
			//{
			//	if( centerPlayer->m_IsWaitReconnect )
			//	{
			//		LogInfo( "OnRecv_ChooseRole 严重错误:正在断线重连中,又收到选择角色的消息,角色=%s", centerPlayer->m_RoleName.c_str() );
			//		return;
			//	}
			//	/// 断线重连
			//	centerPlayer->m_IsWaitReconnect = true;
			//	centerPlayer->m_GateId = loginPlayer->m_GateId;
			//	/// 如果该角色正在游戏中,向该地图服务器发送断线重连的消息
			//	if( centerPlayer->m_PlayerStatus == CenterPlayerStatus_InMap )
			//	{
			//		Packet_CS2M_Inner_PlayerReconnect packetPlayerReconnect;
			//		packetPlayerReconnect.m_RoleId = centerPlayer->m_RoleId;
			//		SendToMap( centerPlayer->m_MapId, &packetPlayerReconnect );
			//	}
			//	/// 告诉客户端稍作等待......
			//	Packet_CS2C_Inner_WaitLogin packetWaitLogin;
			//	packetWaitLogin.m_LoginPlayerId = loginPlayer->m_Id;
			//	packetWaitLogin.m_PlayerID = centerPlayer->m_RoleId;
			//	packetWaitLogin.m_Reason = Packet_CS2C_Inner_WaitLogin::WaitLoginReason_RoleReconnect;
			//	SendToGate( centerPlayer->m_GateId, &packetWaitLogin );
			//	return;
			//}
			///// 选择的另外的角色
			//else
			//{
			//	/// 要先把上一个角色退出游戏
			//	if( centerPlayer->m_PlayerStatus == CenterPlayerStatus_InMap )
			//	{
			//		//SendToMap( centerPlayer->m_MapId, 0 );
			//		/// 等待地图服务器返回删除成功的消息
			//		LogInfo( "OnRecv_ChooseRole 等待地图服务器删除角色=%s", centerPlayer->m_RoleName.c_str() );
			//	}
			//	else
			//	{
			//		LogInfo( "OnRecv_ChooseRole 需要把旧的角色删除=%s", centerPlayer->m_RoleName.c_str() );
			//	}
			//	centerPlayer->m_GateId = loginPlayer->m_GateId;
			//	/// 告诉客户端稍作等待......
			//	Packet_CS2C_Inner_WaitLogin packetWaitLogin;
			//	packetWaitLogin.m_LoginPlayerId = loginPlayer->m_Id;
			//	packetWaitLogin.m_PlayerID = centerPlayer->m_RoleId;
			//	packetWaitLogin.m_Reason = Packet_CS2C_Inner_WaitLogin::WaitLoginReason_RemoveOldOnlineRole;
			//	SendToGate( centerPlayer->m_GateId, &packetWaitLogin );
			//	return;
			//}
			//return;
		}
		/// 请求数据库服务器返回角色数据
		if( m_DbHandler.SendPacket( packet ) )
		{
			CenterPlayer* newCenterPlayer = new CenterPlayer();
			newCenterPlayer->m_PlayerStatus = CenterPlayerStatus_ChooseRoleWait;	/// 等待DbServer返回角色数据
			newCenterPlayer->m_AccountId = loginPlayer->m_AccountId;
			newCenterPlayer->m_AccountName = loginPlayer->m_AccountName;
			newCenterPlayer->m_RoleIndex = roleIndex;
			newCenterPlayer->m_RoleId = loginPlayer->m_RoleIds[roleIndex];
			newCenterPlayer->m_RoleName = loginPlayer->m_RoleNames[roleIndex];
			newCenterPlayer->m_GateId = loginPlayer->m_GateId;
			newCenterPlayer->m_IsCache = false;
			newCenterPlayer->m_StartTime = ::timeGetTime(); /// 记录时间
			/// 加入游戏中玩家列表
			m_CenterPlayers[newCenterPlayer->m_RoleId] = newCenterPlayer;
			/// 从登陆列表中删除
			RemoveLoginPlayer( newCenterPlayer->m_AccountId );
		}
		else
		{
			/// 返回一个错误信息给客户端
			LogInfo( "OnRecv_ChooseRole 严重错误:向DbServer发送数据失败" );
		}
	}
	void CenterServer::OnRecv_DeleteRole( GateHandler* gateHandler, PacketHeader* packet )
	{
		/// 先判断缓存......
		assert( packet->m_Length == sizeof(Packet_C2CS_DeleteRole) );
		if( packet->m_Length != sizeof(Packet_C2CS_DeleteRole) )
			return;
		Packet_C2CS_DeleteRole* packetDeleteRole = (Packet_C2CS_DeleteRole*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetDeleteRole->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_DeleteRole not find LoginPlayer AccountId=%u", packetDeleteRole->m_AccountId );
			return;
		}
		if( loginPlayer->m_Id != packetDeleteRole->m_PlayerID )
		{
			LogInfo( "OnRecv_DeleteRole Error PlayerID=%u", packetDeleteRole->m_PlayerID );
			/// 通知GateServer断开该连接
			return;
		}
		if( packetDeleteRole->m_RoleId < 1 )
			return;
		/// 判断m_RoleId是否正确
		int roleIndex = -1;
		for( int i=0; i<MAX_ROLE_NUM; ++i )
		{
			if( packetDeleteRole->m_RoleId == loginPlayer->m_RoleIds[i] )
			{
				roleIndex = i;
				break;
			}
		}
		if( roleIndex == -1 )
		{
			return;
		}
		/// 判断该角色是否在游戏中
		CenterPlayer* centerPlayer = GetCenterPlayerByAccountId( packetDeleteRole->m_AccountId );
		if( centerPlayer != 0 )
		{
			/// 帐号登录时,就判断了断线重连,所以不应该到这里!!!!!!
			/// 这时候不能删除该角色
			LogInfo( "OnRecv_DeleteRole 严重错误:角色已经在游戏中,不应该再收到删除角色的消息=%s", centerPlayer->m_RoleName.c_str() );
			return;
		}
		m_DbHandler.SendPacket( packet );
	}
	void CenterServer::OnRecv_RoleListResult( PacketHeader* packet )
	{
		/// 判断缓存,放入缓存???
		Packet_CS2C_Inner_RoleListResult* packetRoleListResult = (Packet_CS2C_Inner_RoleListResult*)packet;
		if( packetRoleListResult->m_Length < sizeof(Packet_CS2C_Inner_RoleListResult)-MAX_ROLELIST_SIZE )
			return;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetRoleListResult->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_RoleListResult not find LoginPlayer AccountId=%u", packetRoleListResult->m_AccountId );
			return;
		}
		/// 判断该角色是否在游戏中
		CenterPlayer* centerPlayer = GetCenterPlayerByAccountId( loginPlayer->m_AccountId );
		if( centerPlayer != 0 )
		{
			/// 帐号登录时,就判断了断线重连,所以不应该到这里!!!!!!
			LogInfo( "OnRecv_RoleListResult 严重错误:角色已经在游戏中,不应该再收到角色列表返回消息=%s", centerPlayer->m_RoleName.c_str() );
			return;
		}
		if( packetRoleListResult->m_Length != sizeof(Packet_CS2C_Inner_RoleListResult)-MAX_ROLELIST_SIZE+sizeof(SRoleList_RoleInfo)*packetRoleListResult->m_RoleCount )
		{
			LogInfo( "OnRecv_RoleListResult PacketLength Error AccountName=%s", loginPlayer->m_AccountName );
			return;
		}
		::memset( loginPlayer->m_RoleIds, 0, sizeof(loginPlayer->m_RoleIds) );
		/// 保存角色id,用于之后的验证
		uint8* pData = packetRoleListResult->m_RoleListData;
		for( int i=0; i<packetRoleListResult->m_RoleCount; ++i )
		{
			SRoleList_RoleInfo* roleInfo = (SRoleList_RoleInfo*)pData;
			loginPlayer->m_RoleIds[i] = roleInfo->m_RoleId;
			loginPlayer->m_RoleNames[i] = roleInfo->m_RoleName;
			pData += sizeof(SRoleList_RoleInfo);
		}
		SendToGate( loginPlayer->m_GateId, packet );
	}
	void CenterServer::OnRecv_CreateRoleResult( PacketHeader* packet )
	{
		Packet_CS2C_Inner_CreateRoleResult* packetCreateRoleResult = (Packet_CS2C_Inner_CreateRoleResult*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetCreateRoleResult->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_CreateRoleResult not find LoginPlayer AccountId=%u", packetCreateRoleResult->m_AccountId );
			return;
		}
		/// 判断该角色是否在游戏中
		CenterPlayer* centerPlayer = GetCenterPlayerByAccountId( loginPlayer->m_AccountId );
		if( centerPlayer != 0 )
		{
			/// 帐号登录时,就判断了断线重连,所以不应该到这里!!!!!!
			LogInfo( "OnRecv_CreateRoleResult 严重错误:角色已经在游戏中,不应该收到创建角色返回消息=%s", centerPlayer->m_RoleName.c_str() );
			return;
		}
		if( packetCreateRoleResult->m_Result == Packet_CS2C_Inner_CreateRoleResult::CreateRoleResult_OK )
		{
			uint8 roleIndex = packetCreateRoleResult->m_RoleIndex;
			if( roleIndex >= MAX_ROLE_NUM )
			{
				return;
			}
			assert( loginPlayer->m_RoleIds[roleIndex] == 0 );
			loginPlayer->m_RoleIds[roleIndex] = packetCreateRoleResult->m_RoleId;
			loginPlayer->m_RoleNames[roleIndex] = packetCreateRoleResult->m_RoleName;
		}
		SendToGate( loginPlayer->m_GateId, packet );
	}
	void CenterServer::OnRecv_ChooseRoleResult( PacketHeader* packet )
	{
		Packet_CS2C_Inner_ChooseRoleResult* packetChooseRoleResult = (Packet_CS2C_Inner_ChooseRoleResult*)packet;
		SRoleInfo_Header* roleInfo = (SRoleInfo_Header*)packetChooseRoleResult->m_RoleData;
		CenterPlayer* centerPlayer = GetCenterPlayerById( roleInfo->m_RoleId );
		if( centerPlayer == 0 )
		{
			/// 帐号登录时,就判断了断线重连,所以不应该到这里!!!!!!
			LogInfo( "OnRecv_ChooseRoleResult Not find CenterPlayer=%s", roleInfo->m_RoleName );
			return;
		}
		if( packetChooseRoleResult->m_Length != sizeof(Packet_CS2C_Inner_ChooseRoleResult)-MAX_ROLE_DATA_SIZE+sizeof(SRoleInfo_DB) )
		{
			LogInfo( "OnRecv_ChooseRoleResult Error Length CenterPlayer=%s", roleInfo->m_RoleName );
			return;
		}
		/// 判断地图是否存在
		if( roleInfo->m_MapId == 0 || roleInfo->m_MapId > m_MapArraySize )
		{
			/// 是否在这里设置为默认地图
			LogInfo( "OnRecv_ChooseRoleResult Error MapId=%d", roleInfo->m_MapId );
			return;
		}
		if( m_Maps[roleInfo->m_MapId-1] == 0 )
		{
			LogInfo( "OnRecv_ChooseRoleResult Not find MapId=%d", roleInfo->m_MapId );
			return;
		}
		/// 发送角色信息给地图服务器,等待地图服务器上创建玩家对象,然后返回相关数据
		/// 收到返回数据后,从登陆列表中移到游戏列表中,再发给客户端
		Packet_CS2M_Inner_PlayerEnterMap packetPlayerEnterMap;
		packetPlayerEnterMap.m_AccountId = centerPlayer->m_AccountId;
		::strcpy_s( packetPlayerEnterMap.m_AccountName, sizeof(packetPlayerEnterMap.m_AccountName), centerPlayer->m_AccountName.c_str() );
		::memcpy( packetPlayerEnterMap.m_RoleData, packetChooseRoleResult->m_RoleData, sizeof(SRoleInfo_DB) );
		if( SendToMap( roleInfo->m_MapId, &packetPlayerEnterMap ) )
		{
			centerPlayer->m_PlayerStatus = CenterPlayerStatus_EnterWorldWait;
			/// 缓存一下角色数据
			::memcpy( &(centerPlayer->m_Cache), packetChooseRoleResult->m_RoleData, sizeof(SRoleInfo_DB) );
		}
		else
		{
			/// 返回一个错误消息给客户端
			return;
		}
		SendToGate( centerPlayer->m_GateId, packet );
	}
	void CenterServer::OnRecv_DeleteRoleResult( PacketHeader* packet )
	{
		//Packet_CS2C_Inner_DeleteRoleResult* packetDeleteRoleResult = (Packet_CS2C_Inner_DeleteRoleResult*)packet;
		//LoginPlayer* loginPlayer = GetLoginPlayer( packetDeleteRoleResult->m_AccountId );
		//if( loginPlayer == 0 )
		//{
		//	LogInfo( "OnRecv_DeleteRoleResult not find LoginPlayer AccountId=%u", packetDeleteRoleResult->m_AccountId );
		//	return;
		//}
		/// 更新LoginServer的m_RoleIds和m_RoleNames!!!!!!
		//SendToGate( loginPlayer->m_GateId, packet );
	}
	void CenterServer::OnRecv_EnterWorld( PacketHeader* packet )
	{
		///// 判断缓存,放入缓存
		//Packet_CS2C_Inner_EnterWorld* packetEnterWorld = (Packet_CS2C_Inner_EnterWorld*)packet;
		//LoginPlayer* loginPlayer = GetLoginPlayer( packetEnterWorld->m_AccountId );
		//if( loginPlayer == 0 )
		//{
		//	LogInfo( "OnRecv_EnterWorld not find LoginPlayer AccountId=%u", packetEnterWorld->m_AccountId );
		//	return;
		//}
		//SendToGate( loginPlayer->m_GateId, packet );
	}
	void CenterServer::Update()
	{
		SocketMgr::getSingletonPtr()->Update();
	}
	void CenterServer::OnInputCommand( const char* cmd )
	{
	}
}