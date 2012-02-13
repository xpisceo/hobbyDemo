/********************************************************************
	created:	2009-7-19
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	游戏服务器
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"
#include "WeLoginPlayer.h"
#include "WeAccountHandler.h"
#include "WeDbHandler.h"

namespace We
{
	class ClientHandler;

	class GameServer : public SocketListenerHandler
	{
	public:
		uint16  m_GameServerId;

		string	m_ClientListenIP;
		uint16	m_ClientListenPort;
		uint32	m_ClientMaxConnection;

		string	m_AccountServerIP;
		uint16  m_AccountServerPort;

		string	m_DbServerIP;
		uint16  m_DbServerPort;

		AccountHandler				m_AccountHandler;	/// 与帐号服务器的连接
		DbHandler					m_DbHandler;		/// 与DB服务器的连接

		uint32	m_ClientId;	
		map<uint32, ClientHandler*>	m_LoginClientHandlers;	///	登录期的客户端的连接
		map<uint32, ClientHandler*>	m_ClientHandlers;		///	游戏中的客户端的连接

		map<uint32, LoginPlayer*>	m_LoginPlayers;		/// 登录列表(帐号id作为关键字)(注:可以用HashTable代替map)
	public:
		GameServer();
		~GameServer();

		bool Init();
		void Shutdown();

		/// 新网关服务器连接
		virtual void OnAccept( Socket* socket );
		/// 移除网关服务器连接
		virtual void OnRemove( Socket* socket );

		bool SendToLoginClient( uint32 clientId, PacketHeader* packet );
		bool SendToClient( uint32 clientId, PacketHeader* packet );
		void SendToAllClients( PacketHeader* packet );

		LoginPlayer* GetLoginPlayer( uint32 accountId );

		void RemoveLoginPlayer( uint32 accountId );

		/// 帐号登录
		void OnRecv_Login( ClientHandler* clientHandler, PacketHeader* packet );
		void OnRecv_LoginResult( PacketHeader* packet );

		void OnRecv_CreateAccount( ClientHandler* clientHandler, PacketHeader* packet );
		void OnRecv_CreateAccountResult( PacketHeader* packet );

		void OnRecv_RoleList( ClientHandler* clientHandler, PacketHeader* packet );
		void OnRecv_CreateRole( ClientHandler* clientHandler, PacketHeader* packet );
		void OnRecv_ChooseRole( ClientHandler* clientHandler, PacketHeader* packet );
		void OnRecv_DeleteRole( ClientHandler* clientHandler, PacketHeader* packet );

		void OnRecv_RoleListResult( PacketHeader* packet );
		void OnRecv_CreateRoleResult( PacketHeader* packet );
		void OnRecv_ChooseRoleResult( PacketHeader* packet );
		void OnRecv_DeleteRoleResult( PacketHeader* packet );

		void OnRecv_PlayerPacket( ClientHandler* clientHandler, PacketHeader* packet );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}