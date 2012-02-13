#include "WeTestClient.h"
#include <stdio.h>
#include "WeSocket.h"
#include "WeSocketMgr.h"
#include "WeLogInfo.h"
#include "WeGame2ClientPackets_Inner.h"
#include "WeClient2GamePackets_Inner.h"
#include "WeStringUtil.h"
#include "WePlayerStruct.h"

namespace We
{
	TestClient::TestClient()
	{
		m_TotalRecvMsgSizeSec = 0;
		m_TotalRecvMsgSize = 0;
		m_TotalSendMsgSizeSec = 0;
		m_TotalSendMsgSize = 0;

		m_LastSecTick = ::GetTickCount();
		m_LastShowTick = m_LastSecTick;
		m_LastSendTick = m_LastSecTick;

		m_AccountId = 0;
		::memset( &m_RoleInfos, 0, sizeof(m_RoleInfos) );
	}
	TestClient::~TestClient()
	{
	}
	bool TestClient::Init()
	{
		char path[256] = "";
		::GetCurrentDirectory(sizeof(path),path);
		string m_CfgPath = path;
		m_CfgPath += "/TestClient.ini";

		char tmp[1024];
		//::GetPrivateProfileString( "TestClient", "Id", "1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		//m_AccountServerId = ::atoi( tmp );

		::GetPrivateProfileString( "Server", "IP", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_ServerIP = tmp;
		::GetPrivateProfileString( "Server", "Port", "10000", tmp, sizeof(tmp), m_CfgPath.c_str() );
		m_ServerPort = (uint16)::atoi( tmp );

		SocketMgr* sockMgr = new SocketMgr();
		if( sockMgr->AddSocketConnector( 1, this, m_ServerIP.c_str(), m_ServerPort,
			1024*10, 1024*50, 1024*20, 1024*100, true ) )
		{
			LogInfo( "Connect Server OK, Ip=%s Port=%d", m_ServerIP.c_str(), m_ServerPort );
		}
		else
		{
			LogInfo( "Connect Server Failed, Ip=%s Port=%d", m_ServerIP.c_str(), m_ServerPort );
			return false;
		}
		sockMgr->StartIOCP( 0 );
		return true;
	}
	void TestClient::Shutdown()
	{
		SocketMgr::getSingletonPtr()->Shutdown();
		delete SocketMgr::getSingletonPtr();
	}
	void TestClient::OnProcessPacket( PacketHeader* packet )
	{
		if( packet->m_MainType == 0 )
		{
			switch( packet->m_SubType )
			{
			case PT_GS2C_Inner_LoginResult:
				LogInfo( "OnProcessPacketLoginResult" );
				OnRecv_LoginResult( packet );
				break;
			case PT_GS2C_Inner_CreateAccountResult:
				LogInfo( "OnRecv_CreateAccountResult" );
				OnRecv_CreateAccountResult( packet );
				break;
			case PT_GS2C_Inner_RoleListResult:
				LogInfo( "OnRecv_RoleListResult" );
				OnRecv_RoleListResult( packet );
				break;
			case PT_GS2C_Inner_CreateRoleResult:
				LogInfo( "OnRecv_CreateRoleResult" );
				OnRecv_CreateRoleResult( packet );
				break;
			case PT_GS2C_Inner_ChooseRoleResult:
				LogInfo( "OnRecv_ChooseRoleResult" );
				OnRecv_ChooseRoleResult( packet );
				break;
			case PT_GS2C_Inner_DeleteRoleResult:
				LogInfo( "OnRecv_DeleteRoleResult" );
				OnRecv_DeleteRoleResult( packet );
				break;
			case PT_GS2C_Inner_EnterWorld:
				LogInfo( "OnRecv_EnterWorld" );
				OnRecv_EnterWorld( packet );
				break;
			default:
				LogInfo( "OnProcessPacket Main=%d Sub=%d", packet->m_MainType, packet->m_SubType );
				break;
			}
		}
	}
	void TestClient::OnDisconnect()
	{
		LogInfo( "OnDisconnect !!!!!!" );
	}
	void TestClient::OnRecv_LoginResult( PacketHeader* packet )
	{
		Packet_GS2C_Inner_LoginResult* packetLoginResult = (Packet_GS2C_Inner_LoginResult*)packet;
		if( packetLoginResult->m_Result == Packet_GS2C_Inner_LoginResult::LoginResult_OK )
		{
			LogInfo( "OnRecv_LoginResult AccountName=%s", packetLoginResult->m_AccountName );
			//m_AccountId = packetLoginResult->m_AccountId;
		}
		else
		{
			LogInfo( "OnRecv_LoginResult Failed Result=%d", packetLoginResult->m_Result );
		}
	}
	void TestClient::OnRecv_CreateAccountResult( PacketHeader* packet )
	{
		Packet_GS2C_Inner_CreateAccountResult* packetCreateAccountResult = (Packet_GS2C_Inner_CreateAccountResult*)packet;
		if( packetCreateAccountResult->m_Result == Packet_GS2C_Inner_CreateAccountResult::CreateAccountResult_OK )
		{
			LogInfo( "OnRecv_CreateAccountResult AccountName=%s", packetCreateAccountResult->m_AccountName );
		}
		else
		{
			LogInfo( "OnRecv_CreateAccountResult Failed Result=%d", packetCreateAccountResult->m_Result );
		}
	}
	void TestClient::OnRecv_RoleListResult( PacketHeader* packet )
	{
		Packet_GS2C_Inner_RoleListResult* packetRoleListResult = (Packet_GS2C_Inner_RoleListResult*)packet;
		if( packetRoleListResult->m_RoleCount == 0 )
		{
			LogInfo( "OnRecv_RoleListResult RoleCount == 0" );
		}
		else
		{
			LogInfo( "OnRecv_RoleListResult RoleCount=%d", packetRoleListResult->m_RoleCount );
			SRoleList_RoleInfo* pRoleInfo = (SRoleList_RoleInfo*)packetRoleListResult->m_RoleListData;
			for( int i=0; i<packetRoleListResult->m_RoleCount; ++i )
			{
				LogInfo( "RoleIndex=%d RoleName=%s", pRoleInfo->m_RoleIndex, pRoleInfo->m_RoleName );
				m_RoleInfos[pRoleInfo->m_RoleIndex] = *pRoleInfo;
				pRoleInfo++;
			}
		}
	}
	void TestClient::OnRecv_CreateRoleResult( PacketHeader* packet )
	{
		Packet_GS2C_Inner_CreateRoleResult* packetCreateRoleResult = (Packet_GS2C_Inner_CreateRoleResult*)packet;
		if( packetCreateRoleResult->m_Result == Packet_GS2C_Inner_CreateRoleResult::CreateRoleResult_OK )
		{
			SRoleList_RoleInfo* pRoleInfo = (SRoleList_RoleInfo*)packetCreateRoleResult->m_CreateRoleData;
			LogInfo( "RoleIndex=%d RoleName=%s", packetCreateRoleResult->m_RoleIndex, packetCreateRoleResult->m_RoleName );
			m_RoleInfos[pRoleInfo->m_RoleIndex] = *pRoleInfo;
		}
		else
		{
			LogInfo( "OnRecv_CreateRoleResult Failed Result=%d", packetCreateRoleResult->m_Result );
		}
	}
	void TestClient::OnRecv_ChooseRoleResult( PacketHeader* packet )
	{
		Packet_GS2C_Inner_ChooseRoleResult* packetChooseRoleResult = (Packet_GS2C_Inner_ChooseRoleResult*)packet;
		SRoleInfo_DB* roleInfo = (SRoleInfo_DB*)packetChooseRoleResult->m_RoleData;
		LogInfo( "RoleIndex=%d RoleName=%s", roleInfo->m_RoleIndex, roleInfo->m_RoleName );
	}
	void TestClient::OnRecv_DeleteRoleResult( PacketHeader* packet )
	{
		//Packet_CS2C_Inner_DeleteRoleResult* packetDeleteRoleResult = (Packet_CS2C_Inner_DeleteRoleResult*)packet;
	}
	void TestClient::OnRecv_EnterWorld( PacketHeader* packet )
	{
		Packet_GS2C_Inner_EnterWorld* packetEnterWorld = (Packet_GS2C_Inner_EnterWorld*)packet;
		LogInfo( "OnRecv_EnterWorld" );
	}
	void TestClient::Update()
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

	void TestClient::OnInputCommand( const char* cmd )
	{
		vector<string> args = StringUtil::Split( cmd, " " );
		if( args.size() == 0 )
			return;
		if( stricmp( "login", args[0].c_str() ) == 0 )
		{
			Packet_C2GS_Login packetLogin;
			::sprintf( packetLogin.m_AccountName, "%s", "TestAccount" );
			::sprintf( packetLogin.m_EncryptPwd, "%s", "TestPwd" );
			SendPacket( &packetLogin );
			LogInfo( "Send Login" );
		}
		else if( stricmp( "login2", args[0].c_str() ) == 0 )
		{
			Packet_C2GS_Login packetLogin;
			::sprintf( packetLogin.m_AccountName, "%s", "asdasd" );
			::sprintf( packetLogin.m_EncryptPwd, "%s", "123123" );
			SendPacket( &packetLogin );
			LogInfo( "Send Login2" );
		}
		else if( stricmp( "CreateAccount", args[0].c_str() ) == 0 )
		{
			Packet_C2GS_CreateAccount packetCreateAccount;
			::sprintf( packetCreateAccount.m_AccountName, "%s", "TestAccount" );
			::sprintf( packetCreateAccount.m_EncryptPwd, "%s", "TestPwd" );
			SendPacket( &packetCreateAccount );
			LogInfo( "Send CreateAccount" );
		}
		else if( stricmp( "CreateRole", args[0].c_str() ) == 0 )
		{
			Packet_C2GS_CreateRole packetCreateRole;
			packetCreateRole.m_RoleIndex = 0;
			if( args.size() > 1 )
				packetCreateRole.m_RoleIndex = ::atoi( args[1].c_str() );
			if( args.size() > 2 )
				::sprintf( packetCreateRole.m_RoleName, "%s", args[2].c_str() );
			else
				::sprintf( packetCreateRole.m_RoleName, "½ÇÉ«%d", ::rand()%100000 + 1 );
			SCreateRoleInfo* roleInfo = (SCreateRoleInfo*)packetCreateRole.m_CreateRoleData;
			roleInfo->m_Sex = ::rand()%2;
			roleInfo->m_Race = ::rand()%4;
			roleInfo->m_FaceId = ::rand()%16;
			roleInfo->m_HairId = ::rand()%16;
			roleInfo->m_RoleIndex = packetCreateRole.m_RoleIndex;
			packetCreateRole.m_Length = sizeof(Packet_C2GS_CreateRole)-MAX_CREATEROLE_SIZE+sizeof(SCreateRoleInfo);
			SendPacket( &packetCreateRole );
			LogInfo( "Send CreateRole" );
		}
		else if( stricmp( "ChooseRole", args[0].c_str() ) == 0 )
		{
			Packet_C2GS_ChooseRole packetChooseRole;
			packetChooseRole.m_RoleId = args.size()>1?m_RoleInfos[::atoi(args[1].c_str())].m_RoleId:0;
			SendPacket( &packetChooseRole );
			LogInfo( "Send ChooseRole" );
		}
		else if( stricmp( "Move", args[0].c_str() ) == 0 )
		{
			if( args.size() < 3 )
				return;
			Packet_C2GS_Move packetMove;
			packetMove.x = (float)::atof( args[1].c_str() );
			packetMove.y = (float)::atof( args[2].c_str() );
			packetMove.z = 0;
			if( SendPacket( &packetMove ) )
				LogInfo( "Send Move" );
			else
				LogInfo( "Send Move ERROR !!!!!!" );
		}
	}
}