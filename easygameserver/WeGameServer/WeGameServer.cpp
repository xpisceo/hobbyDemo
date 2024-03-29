#include "WeGameServer.h"
#include "WeSocketMgr.h"
#include "WeLogInfo.h"
#include "WeClientHandler.h"
#include "WeAccountHandler.h"
#include "WeAccount2GamePackets_Inner.h"
#include "WeGame2AccountPackets_Inner.h"
#include "WeGame2DbPackets_Inner.h"
#include "WeGame2ClientPackets_Inner.h"
#include "WeDb2GamePackets_Inner.h"
#include "WePlayerStruct.h"
#include "WeWorld.h"
#include "WePlayer.h"

namespace We
{
	GameServer::GameServer()
	{
		m_GameServerId = 0;
		m_ClientId = 0;
	}
	GameServer::~GameServer()
	{
	}
	bool GameServer::Init()
	{
		char path[256] = "";
		::GetCurrentDirectory(sizeof(path),path);
		string m_CfgPath = path;
		m_CfgPath += "/GameServer.ini";
		char tmp[1024];

		::GetPrivateProfileString( "GameServer", "Id", "1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GameServerId = ::atoi( tmp );

		::GetPrivateProfileString( "AccountServer", "IP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountServerIP = tmp;
		::GetPrivateProfileString( "AccountServer", "Port", "20003", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_AccountServerPort = (uint16)::atoi( tmp );

		::GetPrivateProfileString( "DbServer", "IP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbServerIP = tmp;
		::GetPrivateProfileString( "DbServer", "Port", "20004", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_DbServerPort = (uint16)::atoi( tmp );

		::GetPrivateProfileString( "Client", "ListenIP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_ClientListenIP = tmp;
		::GetPrivateProfileString( "Client", "ListenPort", "20001", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_ClientListenPort = (uint16)::atoi( tmp );
		::GetPrivateProfileString( "Client", "MaxConnection", "10", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_ClientMaxConnection = ::atoi( tmp );

		new World();
		World::getSingleton().LoadMaps();

		SocketMgr* sockMgr = new SocketMgr();
		sockMgr->AddSocketListener( 1, this, m_ClientMaxConnection, 1024*50, 1024*10, 1024*1024*5, 1024*1024, true );
		/// connect to AccountServer
		m_AccountHandler.m_GameServer = this;
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
		m_DbHandler.m_GameServer = this;
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
		if( !sockMgr->StartListen( 1, m_ClientListenIP.c_str(), m_ClientListenPort ) )
		{
			LogInfo( "Listen Client failed, ListenIp=%s ListenPort=%d MaxConnection=%d", m_ClientListenIP.c_str(), m_ClientListenPort, m_ClientMaxConnection );
			return false;
		}
		else
		{
			LogInfo( "Listen Client OK, ListenIp=%s ListenPort=%d MaxConnection=%d", m_ClientListenIP.c_str(), m_ClientListenPort, m_ClientMaxConnection );
		}
		Packet_GS2A_Inner_GameServerInfo packetGameServerInfo;
		packetGameServerInfo.m_GameServerId = m_GameServerId;
		m_AccountHandler.SendPacket( &packetGameServerInfo );
		Packet_GS2D_Inner_GameServerInfo packetGameServerInfo2DB;
		packetGameServerInfo2DB.m_GameServerId = m_GameServerId;
		m_DbHandler.SendPacket( &packetGameServerInfo2DB );
		return true;
	}
	void GameServer::Shutdown()
	{
		SocketMgr::getSingletonPtr()->Shutdown();
		delete SocketMgr::getSingletonPtr();
		if( World::getSingletonPtr() != 0 )
			delete World::getSingletonPtr();
	}
	void GameServer::OnAccept( Socket* socket )
	{
		/// use ObjectPool::Alloc
		ClientHandler* handler = new ClientHandler();
		handler->m_GameServer = this;
		socket->SetSocketHandler( handler );
		handler->SetSocket( socket );
		handler->m_ClientId = ++m_ClientId;
		m_LoginClientHandlers[handler->m_ClientId] = handler;
		LogInfo( "OnAccept Client ip=%s port=%d", socket->GetRemoteIP().c_str(), socket->GetRemotePort() );
	}
	void GameServer::OnRemove( Socket* socket )
	{
		ClientHandler* handler = (ClientHandler*)socket->GetSocketHandler();
		if( handler == 0 )
		{
			LogInfo( "OnRemove Client handler==0" );
		}
		else
		{
			/// 玩家在登陆期间,断开了连接
			if( handler->m_ClientStatus >= PlayerStatus_None && handler->m_ClientStatus < PlayerStatus_InMap )
			{
				map<uint32, ClientHandler*>::iterator i = m_LoginClientHandlers.find( handler->m_ClientId );
				if( i == m_LoginClientHandlers.end() )
				{
					if( handler->m_ClientId == 0 )
					{
						delete handler;
					}
					return;
				}
				/// 已通过帐号验证
				if( handler->m_ClientStatus >= PlayerStatus_LoginOK && handler->m_ClientStatus < PlayerStatus_InMap )
				{
					map<uint32, LoginPlayer*>::iterator it = m_LoginPlayers.begin();
					for( it; it!=m_LoginPlayers.end(); ++it )
					{
						if( handler->m_ClientId == it->second->m_ClientId )
						{
							LogInfo( "LoginPlayer Disconnect AccountName=%s", it->second->m_AccountName.c_str() );
							delete it->second;
							m_LoginPlayers.erase( it );
							/// 刷新登录排队列表
							/// ......
							break;
						}
					}
					it = m_LoginPlayers.find( handler->m_AccountId );
					if( it != m_LoginPlayers.end() )
					{
						delete it->second;
						m_LoginPlayers.erase( it );
					}
				}
				/// 客户端断开连接,根据实际的游戏逻辑,这里不一定要删除Player对象!!!!!!
				LogInfo( "OnRemove Client ClientId=%d", handler->m_ClientId );
				socket->SetSocketHandler( 0 );
				/// use ObjectPool::Free
				delete handler;
				m_LoginClientHandlers.erase( i );
			}
			/// 游戏中的玩家断开了连接
			else
			{
				map<uint32, ClientHandler*>::iterator i = m_ClientHandlers.find( handler->m_AccountId );
				if( i != m_ClientHandlers.end() )
				{
					World::getSingleton().OnPlayerDisconnect( handler->m_RoleId );
					socket->SetSocketHandler( 0 );
					/// use ObjectPool::Free
					delete handler;
					m_ClientHandlers.erase(i);
				}
			}
		}
	}
	bool GameServer::SendToLoginClient( uint32 clientId, PacketHeader* packet )
	{
		map<uint32, ClientHandler*>::iterator i = m_LoginClientHandlers.find( clientId );
		if( i == m_LoginClientHandlers.end() )
		{
			LogInfo( "SendToLoginClient Not find Client id=%d", clientId );
			return false;
		}
		assert( i->second );
		if( i->second == 0 )
			return false;
		return i->second->SendPacket( packet );
	}
	bool GameServer::SendToClient( uint32 clientId, PacketHeader* packet )
	{
		map<uint32, ClientHandler*>::iterator i = m_ClientHandlers.find( clientId );
		if( i == m_ClientHandlers.end() )
		{
			LogInfo( "SendToClient Not find Client id=%d", clientId );
			return false;
		}
		assert( i->second );
		if( i->second == 0 )
			return false;
		return i->second->SendPacket( packet );
	}
	void GameServer::SendToAllClients( PacketHeader* packet )
	{
		map<uint32, ClientHandler*>::iterator i = m_ClientHandlers.begin();
		for( i; i!=m_ClientHandlers.end(); ++i )
		{
			assert( i->second );
			if( i->second == 0 )
				continue;
			i->second->SendPacket( packet );
		}
	}
	LoginPlayer* GameServer::GetLoginPlayer( uint32 accountId )
	{
		map<uint32, LoginPlayer*>::iterator i = m_LoginPlayers.find( accountId );
		if( i == m_LoginPlayers.end() )
			return 0;
		return i->second;
	}
	void GameServer::RemoveLoginPlayer( uint32 accountId )
	{
		map<uint32, LoginPlayer*>::iterator i = m_LoginPlayers.find( accountId );
		if( i == m_LoginPlayers.end() )
			return;
		delete i->second;
		m_LoginPlayers.erase(i);
	}
	void GameServer::OnRecv_Login( ClientHandler* clientHandler, PacketHeader* packet )
	{
		if( packet->m_Length != sizeof(Packet_C2GS_Login) )
			return;
		Packet_C2GS_Login* packetLogin = (Packet_C2GS_Login*)packet;
		packetLogin->m_AccountName[MAX_ACCOUNT_NAME] = 0;
		packetLogin->m_EncryptPwd[MAX_ENCRYPT_PWD] = 0;
		/// 这里不做处理,防止其他玩家恶意发送登录消息,导致该帐号玩家不能正常游戏,而是在返回登录成功时,再做处理
		/// 转发给AccountServer
		Packet_GS2A_Inner_Login pakLogin;
		pakLogin.m_ClientId = clientHandler->m_ClientId;
		::strcpy_s( pakLogin.m_AccountName, sizeof(pakLogin.m_AccountName), packetLogin->m_AccountName );
		::strcpy_s( pakLogin.m_EncryptPwd, sizeof(pakLogin.m_EncryptPwd), packetLogin->m_EncryptPwd );
		m_AccountHandler.SendPacket( &pakLogin );
	}
	void GameServer::OnRecv_LoginResult( PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_A2GS_Inner_LoginResult) );
		if( packet->m_Length != sizeof(Packet_A2GS_Inner_LoginResult) )
			return;
		Packet_A2GS_Inner_LoginResult* packetLoginResult = (Packet_A2GS_Inner_LoginResult*)packet;
		map<uint32, ClientHandler*>::iterator it = m_LoginClientHandlers.find( packetLoginResult->m_ClientId );
		/// 可能在返回验证结果之前,客户端就断开连接了
		if( it == m_LoginClientHandlers.end() )
			return;
		bool requestRoleList = false;
		ClientHandler* handler = it->second;
		/// 如果登录成功
		if( packetLoginResult->m_Result == Packet_A2GS_Inner_LoginResult::LoginResult_OK )
		{
			assert( packetLoginResult->m_AccountId > 0 );
			/// 先判断该账号是否已经在登录列表中
			map<uint32, LoginPlayer*>::iterator i = m_LoginPlayers.find( packetLoginResult->m_AccountId );
			if( i != m_LoginPlayers.end() )
			{
				/// 之前的连接可能在登陆期间异常退出了,先把之前的连接删除掉
				LoginPlayer* loginPlayer = i->second;
				if( loginPlayer->m_ClientId == handler->m_ClientId )
				{
					LogInfo( "Exist LoginPlayer AccountName=%s loginPlayer->m_ClientId == handler->m_ClientId", i->second->m_AccountName.c_str() );
					return;
				}
				map<uint32, ClientHandler*>::iterator it_old = m_LoginClientHandlers.find( loginPlayer->m_ClientId );
				if( it_old == m_LoginClientHandlers.end() )
				{
					LogInfo( "Exist LoginPlayer AccountName=%s it_old == m_LoginClientHandlers.end()", i->second->m_AccountName.c_str() );
					return;
				}
				m_LoginClientHandlers.erase( it_old );
				loginPlayer->m_ClientId = handler->m_ClientId;
				loginPlayer->m_LoginTime = ::timeGetTime(); /// 记录登录时间
				handler->SetClientStatus( PlayerStatus_LoginOK );
				requestRoleList = true;
				/// 
				LogInfo( "Exist LoginPlayer AccountName=%s", i->second->m_AccountName.c_str() );
			}
			else
			{
				handler->m_AccountId = packetLoginResult->m_AccountId;
				/// 判断该账号是否在游戏中
				Player* existPlayer = World::getSingleton().GetPlayerByAccountId( packetLoginResult->m_AccountId );
				if( existPlayer == 0 )
				{
					LoginPlayer* loginPlayer = new LoginPlayer();
					loginPlayer->m_ClientId = handler->m_ClientId;
					loginPlayer->m_AccountId = packetLoginResult->m_AccountId;
					loginPlayer->m_AccountName = packetLoginResult->m_AccountName;
					loginPlayer->m_LoginTime = ::timeGetTime(); /// 记录登录时间
					::memset( loginPlayer->m_RoleIds, 0, sizeof(loginPlayer->m_RoleIds) );
					m_LoginPlayers[loginPlayer->m_AccountId] = loginPlayer;
					handler->SetClientStatus( PlayerStatus_LoginOK );
					requestRoleList = true;
				}
				/// 如果在游戏中,则断线重连
				else
				{
					/// 从登陆列表转移到游戏列表中
					handler->m_RoleId = existPlayer->m_RoleId;
					m_LoginClientHandlers.erase( it );
					m_ClientHandlers[existPlayer->m_AccountId] = handler;
					handler->SetClientStatus( PlayerStatus_InMap );
					existPlayer->OnReconnect();	/// 断线重连
					LogInfo( "Player OnReconnect Name=%s", existPlayer->GetRoleName().c_str() );
					return;
				}
			}
		}
		/// 返回给客户端
		Packet_GS2C_Inner_LoginResult pakLoginResult;
		pakLoginResult.m_Result = packetLoginResult->m_Result;
		::strcpy_s( pakLoginResult.m_AccountName, sizeof(pakLoginResult.m_AccountName), packetLoginResult->m_AccountName );
		handler->SendPacket( &pakLoginResult );
		/// 向DbServer请求角色列表数据
		if( requestRoleList )
		{
			Packet_GS2D_Inner_RoleList packetRoleList;
			packetRoleList.m_AccountId = packetLoginResult->m_AccountId;
			m_DbHandler.SendPacket( &packetRoleList );
		}
	}
	void GameServer::OnRecv_CreateAccount( ClientHandler* clientHandler, PacketHeader* packet )
	{
		Packet_C2GS_CreateAccount* packetCreateAccount = (Packet_C2GS_CreateAccount*)packet;
		if( packetCreateAccount->m_Length != sizeof(Packet_C2GS_CreateAccount) )
		{
			clientHandler->GetSocket()->Delete();
			return;
		}
		/// 转发给AccountServer
		Packet_GS2A_Inner_CreateAccount pakCreateAccount;
		pakCreateAccount.m_ClientId = clientHandler->m_ClientId;
		pakCreateAccount.m_AccountName[MAX_ACCOUNT_NAME] = 0;
		pakCreateAccount.m_EncryptPwd[MAX_ENCRYPT_PWD] = 0;
		::strcpy_s( pakCreateAccount.m_AccountName, sizeof(pakCreateAccount.m_AccountName), packetCreateAccount->m_AccountName );
		::strcpy_s( pakCreateAccount.m_EncryptPwd, sizeof(pakCreateAccount.m_EncryptPwd), packetCreateAccount->m_EncryptPwd );
		m_AccountHandler.SendPacket( &pakCreateAccount );
	}
	void GameServer::OnRecv_CreateAccountResult( PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_A2GS_Inner_CreateAccountResult) );
		if( packet->m_Length != sizeof(Packet_A2GS_Inner_CreateAccountResult) )
			return;
		Packet_A2GS_Inner_CreateAccountResult* packetCreateAccountResult = (Packet_A2GS_Inner_CreateAccountResult*)packet;
		map<uint32, ClientHandler*>::iterator it = m_LoginClientHandlers.find( packetCreateAccountResult->m_ClientId );
		/// 可能在返结果之前,客户端就断开连接了
		if( it == m_LoginClientHandlers.end() )
			return;
		ClientHandler* handler = it->second;
		/// 返回给客户端
		Packet_GS2C_Inner_CreateAccountResult pakCreateAccountResult;
		pakCreateAccountResult.m_Result = packetCreateAccountResult->m_Result;
		::strcpy_s( pakCreateAccountResult.m_AccountName, sizeof(pakCreateAccountResult.m_AccountName), packetCreateAccountResult->m_AccountName );
		handler->SendPacket( &pakCreateAccountResult );
	}
	void GameServer::OnRecv_CreateRole( ClientHandler* clientHandler, PacketHeader* packet )
	{
		/// 先判断合法性.....
		/// 考虑到以后合服的可能性，是不是要保证角色名在整个区都是唯一的??????还是用其他方式来处理
		/// 转发给DbServer
		assert( packet->m_Length == sizeof(Packet_C2GS_CreateRole)-MAX_CREATEROLE_SIZE+sizeof(SCreateRoleInfo) );
		if( packet->m_Length != sizeof(Packet_C2GS_CreateRole)-MAX_CREATEROLE_SIZE+sizeof(SCreateRoleInfo) )
			return;
		Packet_C2GS_CreateRole* packetCreateRole = (Packet_C2GS_CreateRole*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( clientHandler->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_CreateRole not find LoginPlayer AccountId=%u", clientHandler->m_AccountId );
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
		Packet_GS2D_Inner_CreateRole pakCreateRole;
		pakCreateRole.m_AccountId = loginPlayer->m_AccountId;
		pakCreateRole.m_RoleIndex = packetCreateRole->m_RoleIndex;
		::strcpy_s( pakCreateRole.m_RoleName, sizeof(pakCreateRole.m_RoleName), packetCreateRole->m_RoleName );
		::memcpy( pakCreateRole.m_CreateRoleData, packetCreateRole->m_CreateRoleData, sizeof(SCreateRoleInfo) );
		pakCreateRole.m_Length = sizeof(Packet_GS2D_Inner_CreateRole)-MAX_CREATEROLE_SIZE+sizeof(SCreateRoleInfo);
		m_DbHandler.SendPacket( &pakCreateRole );
	}
	void GameServer::OnRecv_CreateRoleResult( PacketHeader* packet )
	{
		Packet_D2GS_Inner_CreateRoleResult* packetCreateRoleResult = (Packet_D2GS_Inner_CreateRoleResult*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetCreateRoleResult->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_CreateRoleResult not find LoginPlayer AccountId=%u", packetCreateRoleResult->m_AccountId );
			return;
		}
		if( packetCreateRoleResult->m_Result == Packet_GS2C_Inner_CreateRoleResult::CreateRoleResult_OK )
		{
			uint8 roleIndex = packetCreateRoleResult->m_RoleIndex;
			if( roleIndex >= MAX_ROLE_NUM )
			{
				return;
			}
			assert( loginPlayer->m_RoleIds[roleIndex] == 0 );
			assert( packetCreateRoleResult->m_RoleId > 0 );
			loginPlayer->m_RoleIds[roleIndex] = packetCreateRoleResult->m_RoleId;
			loginPlayer->m_RoleNames[roleIndex] = packetCreateRoleResult->m_RoleName;
		}
		map<uint32, ClientHandler*>::iterator it = m_LoginClientHandlers.find( loginPlayer->m_ClientId );
		/// 可能在返结果之前,客户端就断开连接了
		if( it == m_LoginClientHandlers.end() )
			return;
		Packet_GS2C_Inner_CreateRoleResult pakCreateRoleResult;
		pakCreateRoleResult.m_Result = packetCreateRoleResult->m_Result;
		pakCreateRoleResult.m_RoleIndex = packetCreateRoleResult->m_RoleIndex;
		pakCreateRoleResult.m_RoleId = packetCreateRoleResult->m_RoleId;
		::strcpy_s( pakCreateRoleResult.m_RoleName, sizeof(pakCreateRoleResult.m_RoleName), packetCreateRoleResult->m_RoleName );
		::memcpy( pakCreateRoleResult.m_CreateRoleData, packetCreateRoleResult->m_CreateRoleData, sizeof(SRoleList_RoleInfo) );
		pakCreateRoleResult.m_Length = sizeof(Packet_GS2C_Inner_CreateRoleResult)-MAX_CREATEROLE_SIZE+sizeof(SRoleList_RoleInfo);
		it->second->SendPacket( &pakCreateRoleResult );
	}
	void GameServer::OnRecv_ChooseRole( ClientHandler* clientHandler, PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_C2GS_ChooseRole) );
		if( packet->m_Length != sizeof(Packet_C2GS_ChooseRole) )
			return;
		Packet_C2GS_ChooseRole* packetChooseRole = (Packet_C2GS_ChooseRole*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( clientHandler->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_ChooseRole not find LoginPlayer AccountId=%u", clientHandler->m_AccountId );
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
			/// 断开该连接
			return;
		}
		Packet_GS2D_Inner_ChooseRole pakChooseRole;
		pakChooseRole.m_AccountId = loginPlayer->m_AccountId;
		pakChooseRole.m_RoleId = packetChooseRole->m_RoleId;
		/// 请求数据库服务器返回角色数据
		if( m_DbHandler.SendPacket( &pakChooseRole ) )
		{
			clientHandler->m_ClientStatus = PlayerStatus_ChooseRoleWait;
		}
		else
		{
			/// 返回一个错误信息给客户端
			LogInfo( "OnRecv_ChooseRole 严重错误:向DbServer发送数据失败" );
		}
	}

	void GameServer::OnRecv_ChooseRoleResult( PacketHeader* packet )
	{
		Packet_D2GS_Inner_ChooseRoleResult* packetChooseRoleResult = (Packet_D2GS_Inner_ChooseRoleResult*)packet;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetChooseRoleResult->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_CreateRoleResult not find LoginPlayer AccountId=%u", packetChooseRoleResult->m_AccountId );
			return;
		}
		SRoleInfo_Header* roleInfo = (SRoleInfo_Header*)packetChooseRoleResult->m_RoleData;
		if( packetChooseRoleResult->m_Length != sizeof(Packet_D2GS_Inner_ChooseRoleResult)-MAX_ROLE_DATA_SIZE+sizeof(SRoleInfo_DB) )
		{
			LogInfo( "OnRecv_ChooseRoleResult Error Length" );
			return;
		}
		map<uint32, ClientHandler*>::iterator iL = m_LoginClientHandlers.find( loginPlayer->m_ClientId );
		if( iL == m_LoginClientHandlers.end() )
		{
			LogInfo( "OnRecv_ChooseRoleResult ERROR iL == m_LoginClientHandlers.end()" );
			return;
		}
		map<uint32, ClientHandler*>::iterator iG = m_ClientHandlers.find( loginPlayer->m_AccountId );
		if( iG != m_ClientHandlers.end() )
		{
			LogInfo( "OnRecv_ChooseRoleResult ERROR iG != m_ClientHandlers.end()" );
			return;
		}
		assert( loginPlayer->m_AccountId > 0 );
		if( packetChooseRoleResult->m_Result == Packet_D2GS_Inner_ChooseRoleResult::ChooseRoleResult_OK )
		{
			assert( roleInfo->m_RoleId > 0 );
			/// 连接从登陆列表移到游戏列表
			ClientHandler* handler = iL->second;
			m_LoginClientHandlers.erase( iL );
			m_ClientHandlers[loginPlayer->m_AccountId] = handler;
			handler->m_RoleId = roleInfo->m_RoleId;
			/// 进入游戏世界
			if( World::getSingleton().OnPlayerEnterWorld( packetChooseRoleResult->m_AccountId, packetChooseRoleResult->m_RoleData, handler ) )
			{
				handler->SetClientStatus( PlayerStatus_InMap );
				/// 移除LoginPlayer
				RemoveLoginPlayer( packetChooseRoleResult->m_AccountId );
				Player* player = World::getSingleton().GetPlayerByAccountId( packetChooseRoleResult->m_AccountId );
				assert( player );
				if( player != 0 )
					LogInfo( "OnPlayerEnterWorld Name=%s", player->GetRoleName().c_str() );
			}
			else
			{
				/// 容错处理......
			}
		}
		else
		{
			//SendToLoginClient( loginPlayer->m_ClientId, packet );
		}
	}

	void GameServer::OnRecv_DeleteRole( ClientHandler* clientHandler, PacketHeader* packet )
	{
		/// 先判断缓存......
		assert( packet->m_Length == sizeof(Packet_C2GS_DeleteRole) );
		if( packet->m_Length != sizeof(Packet_C2GS_DeleteRole) )
			return;
		Packet_C2GS_DeleteRole* packetDeleteRole = (Packet_C2GS_DeleteRole*)packet;
		if( packetDeleteRole->m_RoleId < 1 )
			return;
		LoginPlayer* loginPlayer = GetLoginPlayer( clientHandler->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_DeleteRole not find LoginPlayer AccountId=%u", clientHandler->m_AccountId );
			return;
		}
		/// ......
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
		///// 判断该角色是否在游戏中
		//CenterPlayer* centerPlayer = GetCenterPlayerByAccountId( packetDeleteRole->m_AccountId );
		//if( centerPlayer != 0 )
		//{
		//	/// 帐号登录时,就判断了断线重连,所以不应该到这里!!!!!!
		//	/// 这时候不能删除该角色
		//	LogInfo( "OnRecv_DeleteRole 严重错误:角色已经在游戏中,不应该再收到删除角色的消息=%s", centerPlayer->m_RoleName.c_str() );
		//	return;
		//}
		m_DbHandler.SendPacket( packet );
	}
	void GameServer::OnRecv_RoleListResult( PacketHeader* packet )
	{
		Packet_D2GS_Inner_RoleListResult* packetRoleListResult = (Packet_D2GS_Inner_RoleListResult*)packet;
		if( packetRoleListResult->m_Length < sizeof(Packet_D2GS_Inner_RoleListResult)-MAX_ROLELIST_SIZE )
			return;
		LoginPlayer* loginPlayer = GetLoginPlayer( packetRoleListResult->m_AccountId );
		if( loginPlayer == 0 )
		{
			LogInfo( "OnRecv_RoleListResult not find LoginPlayer AccountId=%u", packetRoleListResult->m_AccountId );
			return;
		}
		/// 判断该角色是否在游戏中
		Player* player = World::getSingleton().GetPlayerByAccountId( loginPlayer->m_AccountId );
		if( player != 0 )
		{
			/// 帐号登录时,就判断了断线重连,所以不应该到这里!!!!!!
			LogInfo( "OnRecv_RoleListResult 严重错误:角色已经在游戏中,不应该再收到角色列表返回消息=%s", player->GetRoleName().c_str() );
			return;
		}
		if( packetRoleListResult->m_Length != sizeof(Packet_D2GS_Inner_RoleListResult)-(MAX_ROLE_NUM-packetRoleListResult->m_RoleCount)*sizeof(SRoleList_RoleInfo) )
		{
			LogInfo( "OnRecv_RoleListResult PacketLength Error AccountName=%s", loginPlayer->m_AccountName.c_str() );
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
		/// send to client
		packet->m_SubType = PT_GS2C_Inner_RoleListResult;
		SendToLoginClient( loginPlayer->m_ClientId, packet );
	}

	void GameServer::OnRecv_DeleteRoleResult( PacketHeader* packet )
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
	void GameServer::OnRecv_PlayerPacket( ClientHandler* clientHandler, PacketHeader* packet )
	{
		if( clientHandler == 0 || packet == 0 )
			return;
		Player* player = World::getSingleton().GetPlayerByAccountId( clientHandler->m_AccountId );
		if( player == 0 )
		{
			return;
		}
		PacketHandler packetHandler = World::getSingleton().GetPacketHandler( packet->m_MainType, packet->m_SubType );
		if( packetHandler != 0 )
		{
			/// 处理消息包之前,做一些记录
			packetHandler( player, packet );
			/// 处理消息包之后,做一下对比,检查是否造成内存溢出等问题
		}
		else
		{
			/// 处理消息包之前,做一些记录
			player->OnRecv_Packet( packet );
			/// 处理消息包之后,做一下对比,检查是否造成内存溢出等问题
		}
	}
	void GameServer::Update()
	{
		SocketMgr::getSingletonPtr()->Update();
		World::getSingleton().Update();
	}
	void GameServer::OnInputCommand( const char* cmd )
	{
	}
}