/********************************************************************
	created:	2009-7-2
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	测试客户端
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"
#include "WePlayerStruct.h"

namespace We
{
	class TestClient : public SocketHandler
	{
	public:
		unsigned int m_TotalRecvMsgSizeSec;
		unsigned int m_TotalRecvMsgSize;
		unsigned int m_TotalSendMsgSizeSec;
		unsigned int m_TotalSendMsgSize;

		unsigned int m_LastSecTick;
		unsigned int m_LastShowTick;
		unsigned int m_LastSendTick;

		string	m_ServerIP;
		uint16	m_ServerPort;

		uint32  m_AccountId;

		SRoleList_RoleInfo m_RoleInfos[MAX_ROLE_NUM];

	public:
		TestClient();
		~TestClient();

		bool Init();
		void Shutdown();

		void OnProcessPacket( PacketHeader* packet );

		void OnDisconnect();

		void OnRecv_LoginResult( PacketHeader* packet );
		void OnRecv_CreateAccountResult( PacketHeader* packet );
		void OnRecv_RoleListResult( PacketHeader* packet );
		void OnRecv_CreateRoleResult( PacketHeader* packet );
		void OnRecv_ChooseRoleResult( PacketHeader* packet );
		void OnRecv_DeleteRoleResult( PacketHeader* packet );
		void OnRecv_EnterWorld( PacketHeader* packet );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}