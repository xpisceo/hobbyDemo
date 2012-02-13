#include "WeGateServer.h"
#include <stdio.h>
#include "WeSocket.h"
#include "WeClientHandler.h"
#include "WeSocketMgr.h"
#include "WeLogInfo.h"
#include "WeCenter2ClientPackets_Inner.h"
#include "WeCenter2GatePackets_Inner.h"
#include "WeGate2CenterPackets_Inner.h"
#include "WeAccount2CenterPackets_Inner.h"

namespace We
{
	GateServer::GateServer()
	{
		m_ClientHandlerId = 0;

		m_TotalRecvMsgSizeSec = 0;
		m_TotalRecvMsgSize = 0;
		m_TotalSendMsgSizeSec = 0;
		m_TotalSendMsgSize = 0;

		m_LastSecTick = ::GetTickCount();
		m_LastShowTick = m_LastSecTick;
		m_LastSendTick = m_LastSecTick;

		m_GateServerId = 0;
		m_ClientListenPort = 10000;
		m_MaxClientConnection = 0;
	}
	GateServer::~GateServer()
	{
	}
	bool GateServer::Init()
	{
		char path[256] = "";
		::GetCurrentDirectory(sizeof(path),path);
		string m_CfgPath = path;
		m_CfgPath += "/GateServer.ini";

		char tmp[1024];
		::GetPrivateProfileString( "GateServer", "GateServerId", "1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_GateServerId = (uint8)::atoi( tmp );

		::GetPrivateProfileString( "Client", "ListenIP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_ClientListenIP = tmp;
		::GetPrivateProfileString( "Client", "ListenPort", "10000", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_ClientListenPort = (uint16)::atoi( tmp );
		::GetPrivateProfileString( "Client", "MaxConnection", "1000", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_MaxClientConnection = ::atoi( tmp );

		::GetPrivateProfileString( "CenterServer", "IP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_CenterServerIP = tmp;
		::GetPrivateProfileString( "CenterServer", "Port", "20001", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_CenterServerPort = (uint16)::atoi( tmp );

		SocketMgr* sockMgr = new SocketMgr();
		sockMgr->AddSocketListener( 1, this, m_MaxClientConnection, 1024*50, 1024*10, 1024*100, 1024*10, true );
		m_CenterServer.m_GateServer = this;
		if( !sockMgr->AddSocketConnector( 10000, &m_CenterServer, m_CenterServerIP.c_str(), m_CenterServerPort,
			1024*10, 1024*50, 1024*1024, 1024*1024, true ) )
		{
			LogInfo( "Connect CenterServer Failed, Ip=%s Port=%d", m_CenterServerIP.c_str(), m_CenterServerPort );
			return false;
		}
		else
		{
			/// 发送自己的配置信息给中心服务器
			LogInfo( "Connect CenterServer OK, Ip=%s Port=%d", m_CenterServerIP.c_str(), m_CenterServerPort );
		}
		sockMgr->StartIOCP( 0 );
		if( !sockMgr->StartListen( 1, m_ClientListenIP.c_str(), m_ClientListenPort ) )
		{
			LogInfo( "StartListen failed, ListenIp=%s ListenPort=%d MaxConnection=%d", m_ClientListenIP.c_str(), m_ClientListenPort, m_MaxClientConnection );
			return false;
		}
		else
		{
			LogInfo( "StartListen OK, ListenIp=%s ListenPort=%d MaxConnection=%d", m_ClientListenIP.c_str(), m_ClientListenPort, m_MaxClientConnection );
		}
		Packet_G2CS_Inner_GateInfo packetGateInfo;
		packetGateInfo.m_GateId = m_GateServerId;
		SendToCenter( &packetGateInfo );
		return true;
	}
	void GateServer::Shutdown()
	{
		SocketMgr::getSingletonPtr()->Shutdown();
		delete SocketMgr::getSingletonPtr();
	}
	void GateServer::OnAccept( Socket* socket )
	{
		/// use ObjectPool::Alloc
		ClientHandler* handler = new ClientHandler();
		handler->m_GateServer = this;
		handler->SetClientStatus( ClientStatus_AccountLogin );
		handler->m_Id = ++m_ClientHandlerId;
		handler->m_ConnectTime = ::timeGetTime();
		/// 生成密保卡随机号,发送给客户端
		/// ......
		socket->SetSocketHandler( handler );
		handler->SetSocket( socket );
		m_LogingClientHandlers[handler->m_Id] = handler;
		LogInfo( "OnAccept Id=%u ip=%s port=%d", handler->m_Id, socket->GetRemoteIP().c_str(), socket->GetRemotePort() );
	}
	void GateServer::OnRemove( Socket* socket )
	{
		ClientHandler* handler = (ClientHandler*)socket->GetSocketHandler();
		if( handler != 0 )
		{
			if( handler->GetClientStatus() == ClientStatus_AccountLogin )
			{
				LogInfo( "OnRemove Id=%u ip=%s port=%d", handler->m_Id, socket->GetRemoteIP().c_str(), socket->GetRemotePort() );
				map<uint32, ClientHandler*>::iterator i = m_LogingClientHandlers.find(handler->m_Id);
				if( i != m_LogingClientHandlers.end() )
					m_LogingClientHandlers.erase(i);
				else
					LogInfo( "OnRemove i = m_LogingClientHandlers.end() Id=%u", handler->m_Id );
			}
			else
			{
				LogInfo( "OnRemove RoleName=%s ip=%s port=%d", handler->m_RoleName.c_str(), socket->GetRemoteIP().c_str(), socket->GetRemotePort() );
				map<uint32, ClientHandler*>::iterator i = m_ClientHandlers.find(handler->m_RoleId);
				if( i != m_ClientHandlers.end() )
					m_ClientHandlers.erase(i);
				else
					LogInfo( "OnRemove i = m_ClientHandlers.end() RoleId=%u", handler->m_RoleId );
			}
			socket->SetSocketHandler( 0 );
			/// use ObjectPool::Free
			delete handler;
		}
		else
		{
			LogInfo( "OnRemove handler=0" );
		}
	}
	bool GateServer::SendToCenter( PacketHeader* packet )
	{
		return m_CenterServer.SendPacket( packet );
	}
	bool GateServer::SendToMap( uint16 mapId, PacketHeader* packet )
	{
		map<uint16, MapHandler*>::iterator i = m_MapServers.find( mapId );
		if( i == m_MapServers.end() )
		{
			return false;
		}
		return i->second->SendPacket( packet );
	}
	void GateServer::OnAddMapServer( PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_CS2G_Inner_AddMapServer) );
		if( packet->m_Length != sizeof(Packet_CS2G_Inner_AddMapServer) )
			return;
		Packet_CS2G_Inner_AddMapServer* packetAddMap = (Packet_CS2G_Inner_AddMapServer*)packet;
		MapHandler* mapHandler = new MapHandler();
		if( !SocketMgr::getSingleton().AddSocketConnector( packetAddMap->m_MapId, mapHandler, packetAddMap->m_IP, packetAddMap->m_ListenPort,
			1024*10, 1024*50, 1024*1024, 1024*1024, false ) )
		{
			LogInfo( "GateServer::OnAddMapServer AddSocketConnector Failed MapId=%u ip=%s port=%d", packetAddMap->m_MapId, packetAddMap->m_IP, packetAddMap->m_ListenPort );
			delete mapHandler;
			return;
		}
		map<uint16, MapHandler*>::iterator i = m_MapServers.find( packetAddMap->m_MapId );
		if( i != m_MapServers.end() )
		{
			LogInfo( "GateServer::OnAddMapServer Remove ExistMap MapId=%u", packetAddMap->m_MapId );
			MapHandler* existMapHandler = i->second;
			if( existMapHandler != 0 && existMapHandler->GetSocket() != 0 )
			{
				existMapHandler->GetSocket()->Delete();
				existMapHandler->GetSocket()->SetSocketHandler( 0 );
				delete existMapHandler;
			}
			m_MapServers.erase( i );
		}
		m_MapServers[packetAddMap->m_MapId] = mapHandler;
		LogInfo( "GateServer::OnAddMapServer MapId=%u ip=%s port=%d", packetAddMap->m_MapId, packetAddMap->m_IP, packetAddMap->m_ListenPort );
	}
	void GateServer::OnRemoveMapServer( PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_CS2G_Inner_RemoveMapServer) );
		if( packet->m_Length != sizeof(Packet_CS2G_Inner_RemoveMapServer) )
			return;
		Packet_CS2G_Inner_RemoveMapServer* packetRemoveMap = (Packet_CS2G_Inner_RemoveMapServer*)packet;
		map<uint16, MapHandler*>::iterator i = m_MapServers.find( packetRemoveMap->m_MapId );
		if( i == m_MapServers.end() )
		{
			LogInfo( "GateServer::OnRemoveMapServer Not find Map MapId=%u", packetRemoveMap->m_MapId );
			return;
		}
		MapHandler* existMapHandler = i->second;
		if( existMapHandler != 0 && existMapHandler->GetSocket() != 0 )
		{
			existMapHandler->GetSocket()->Delete();
			existMapHandler->GetSocket()->SetSocketHandler( 0 );
			delete existMapHandler;
		}
		m_MapServers.erase( i );
	}
	void GateServer::OnClientLogin( PacketHeader* packet )
	{
		assert( packet->m_Length == sizeof(Packet_CS2C_Inner_LoginResult) );
		if( packet->m_Length != sizeof(Packet_CS2C_Inner_LoginResult) )
			return;
		const Packet_CS2C_Inner_LoginResult* packetLogin = (const Packet_CS2C_Inner_LoginResult*)packet;
		map<uint32, ClientHandler*>::iterator i = m_LogingClientHandlers.find( packetLogin->m_PlayerID );
		if( i == m_LogingClientHandlers.end() )
		{
			LogInfo( "GateServer::OnClientLogin i == m_LogingClientHandlers.end() id=%u", packetLogin->m_PlayerID );
			return;
		}
		ClientHandler* clientHandler = i->second;
		assert( clientHandler );
		if( clientHandler->GetClientStatus() != ClientStatus_AccountLogin )
		{
			LogInfo( "GateServer::OnClientLogin != ClientStatus_AccountLogin id=%u ClientStatus=%d", packetLogin->m_PlayerID, clientHandler->GetClientStatus() );
			return;
		}
		/// 登录成功
		if( packetLogin->m_Result == Packet_CS2C_Inner_LoginResult::LoginResult_OK )
		{
			clientHandler->SetLoginFailCount( 0 );
			clientHandler->m_AccountId = packetLogin->m_AccountId;
			clientHandler->m_AccountName = packetLogin->m_AccountName;
		}
		else
		{
			/// 如果是账号密码错误,就统计失败次数,当达到一定次数时,关闭连接
			clientHandler->SetLoginFailCount( clientHandler->GetLoginFailCount() + 1 );
		}
		DispatchLoginPacket( packet );
	}

	void GateServer::OnClientChooseRole( PacketHeader* packet )
	{
		//assert( packet->m_Length >= sizeof(Packet_CS2C_Inner_ChooseRoleData) );
		//if( packet->m_Length < sizeof(Packet_CS2C_Inner_ChooseRoleData) )
		//	return;
		//const Packet_CS2C_Inner_ChooseRoleData* packetChooseRole = (const Packet_CS2C_Inner_ChooseRoleData*)packet;
		//map<uint32, ClientHandler*>::iterator i = m_LogingClientHandlers.find( packetChooseRole->m_PlayerID );
		//if( i == m_LogingClientHandlers.end() )
		//{
		//	LogInfo( "GateServer::OnClientChooseRole i == m_LogingClientHandlers.end() id=%u", packetChooseRole->m_PlayerID );
		//	return;
		//}
		//ClientHandler* clientHandler = i->second;
		//assert( clientHandler );
		//if( clientHandler->m_AccountId == 0 )
		//{
		//	LogInfo( "GateServer::OnClientEnterWorld clientHandler->m_AccountId == 0 id=%u", packetChooseRole->m_PlayerID );
		//	return;
		//}
		//else if( clientHandler->GetClientStatus() != ClientStatus_AccountLogin )
		//{
		//	LogInfo( "GateServer::OnClientEnterWorld != ClientStatus_AccountLogin id=%u ClientStatus=%d", packetChooseRole->m_PlayerID, clientHandler->GetClientStatus() );
		//	return;
		//}
		//clientHandler->m_RoleId = packetChooseRole->m_RoleId;
		//clientHandler->m_RoleName = packetChooseRole->m_RoleName;
		//DispatchLoginPacket( packet );
	}

	void GateServer::OnClientEnterWorld( PacketHeader* packet )
	{
		assert( packet->m_Length >= sizeof(Packet_CS2C_Inner_EnterWorld) );
		if( packet->m_Length < sizeof(Packet_CS2C_Inner_EnterWorld) )
			return;
		const Packet_CS2C_Inner_EnterWorld* packetEnterWorld = (const Packet_CS2C_Inner_EnterWorld*)packet;
		map<uint32, ClientHandler*>::iterator i = m_LogingClientHandlers.find( packetEnterWorld->m_PlayerID );
		if( i == m_LogingClientHandlers.end() )
		{
			LogInfo( "GateServer::OnClientEnterWorld i == m_LogingClientHandlers.end() id=%u", packetEnterWorld->m_PlayerID );
			return;
		}
		ClientHandler* clientHandler = i->second;
		assert( clientHandler );
		if( clientHandler->m_AccountId == 0 )
		{
			LogInfo( "GateServer::OnClientEnterWorld clientHandler->m_AccountId == 0 id=%u", packetEnterWorld->m_PlayerID );
			return;
		}
		else if( clientHandler->m_RoleId == 0 )
		{
			LogInfo( "GateServer::OnClientEnterWorld clientHandler->m_RoleId == 0 id=%u", packetEnterWorld->m_PlayerID );
			return;
		}
		else if( clientHandler->GetClientStatus() != ClientStatus_AccountLogin )
		{
			LogInfo( "GateServer::OnClientEnterWorld != ClientStatus_AccountLogin id=%u ClientStatus=%d", packetEnterWorld->m_PlayerID, clientHandler->GetClientStatus() );
			return;
		}
		map<uint32, ClientHandler*>::iterator i2 = m_ClientHandlers.find( clientHandler->m_RoleId );
		if( i2 != m_ClientHandlers.end() )
		{
			LogInfo( "GateServer::OnClientEnterWorld i2 != m_ClientHandlers.end() RoleId=%u RoleName=%s", clientHandler->m_RoleId, clientHandler->m_RoleName.c_str() );
			ClientHandler* existClientHandler = i2->second;
			assert( existClientHandler );
			/// 什么情况下会到这里???
			/// 断开连接
			if( existClientHandler != 0 && existClientHandler->GetSocket() != 0 )
				existClientHandler->GetSocket()->Delete();
			m_ClientHandlers.erase( i2 );
		}
		/// 从登陆列表移到游戏列表
		clientHandler->SetClientStatus( ClientStatus_EnterWorld );
		clientHandler->m_Id = clientHandler->m_RoleId;
		m_ClientHandlers[clientHandler->m_RoleId] = clientHandler;
		m_LogingClientHandlers.erase( i );
		LogInfo( "GateServer::OnClientEnterWorld AccountName=%s RoleName=%s", clientHandler->m_AccountName.c_str(), clientHandler->m_RoleName.c_str() );
		DispatchLoginPacket( packet );
	}

	bool GateServer::DispatchLoginPacket( PacketHeader* packet )
	{
		Packet_CS2C_PacketHeader* packetHeader = (Packet_CS2C_PacketHeader*)packet;
		map<uint32, ClientHandler*>::iterator i = m_LogingClientHandlers.find( packetHeader->m_PlayerID );
		if( i == m_LogingClientHandlers.end() )
		{
			LogInfo( "GateServer::DispatchLoginPacket i == m_LogingClientHandlers.end() id=%u", packetHeader->m_PlayerID );
			return false;
		}
		return i->second->SendPacket( packet );
	}

	bool GateServer::DispatchPacket( PacketHeader* packet )
	{
		Packet_CS2C_PacketHeader* packetHeader = (Packet_CS2C_PacketHeader*)packet;
		map<uint32, ClientHandler*>::iterator i = m_ClientHandlers.find( packetHeader->m_PlayerID );
		if( i == m_ClientHandlers.end() )
		{
			LogInfo( "GateServer::DispatchPacket i == m_ClientHandlers.end() id=%u", packetHeader->m_PlayerID );
			return false;
		}
		return i->second->SendPacket( packet );
	}

	void GateServer::Update()
	{
		SocketMgr::getSingletonPtr()->Update();
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

	void GateServer::OnInputCommand( const char* cmd )
	{
	}
}