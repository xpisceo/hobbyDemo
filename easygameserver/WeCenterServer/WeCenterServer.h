/********************************************************************
	created:	2009-6-28
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	中心服务器
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"
#include "WeCenterPlayer.h"
#include "WeAccountHandler.h"
#include "WeDbHandler.h"

namespace We
{
	class CenterServer;
	class GateHandler;
	class MapHandler;
	class AccountHandler;

	class MapServerListener : public SocketListenerHandler
	{
	protected:
		CenterServer* m_CenterServer;
	public:
		MapServerListener( CenterServer* centerServer );

		/// 新地图服务器连接
		virtual void OnAccept( Socket* socket );
		/// 移除地图服务器连接
		virtual void OnRemove( Socket* socket );
	};

	class CenterServer : public SocketListenerHandler
	{
	public:
		uint32 m_CenterId;

		string	m_GateListenIP;
		uint16	m_GateListenPort;
		uint32	m_GateMaxConnection;

		string	m_MapListenIP;
		uint16	m_MapListenPort;
		uint32	m_MapMaxConnection;

		string	m_AccountServerIP;
		uint16  m_AccountServerPort;

		string	m_DbServerIP;
		uint16  m_DbServerPort;

		MapServerListener* m_MapServerListener;

		AccountHandler				m_AccountHandler;	/// 与帐号服务器的连接
		DbHandler					m_DbHandler;		/// 与DB服务器的连接
		map<uint8, GateHandler*>	m_GateHandlers;		///	网关服务器的连接(注:可以用数组代替map)
		map<uint16, MapHandler*>	m_MapHandlers;		/// 地图服务器的连接(注:可以用数组代替map)
		MapHandler**				m_Maps;				/// 地图数组
		uint32						m_MapArraySize;		/// 地图数组的大小
		uint32						m_MaxMapId;			/// 当前最大的MapId

		map<uint32, LoginPlayer*>	m_LoginPlayers;		/// 登录列表(帐号id作为关键字)(注:可以用HashTable代替map)
		map<uint32, CenterPlayer*>	m_CenterPlayers;	/// 游戏中的玩家(角色id作为关键字)(注:可以用HashTable代替map)
	public:
		CenterServer();
		~CenterServer();

		bool Init();
		void Shutdown();

		/// 新网关服务器连接
		virtual void OnAccept( Socket* socket );
		/// 移除网关服务器连接
		virtual void OnRemove( Socket* socket );

		/// 新地图服务器连接
		virtual void OnAcceptMapServer( Socket* socket );
		/// 移除地图服务器连接
		virtual void OnRemoveMapServer( Socket* socket );

		bool SendToGate( uint8 gateId, PacketHeader* packet );
		void SendToAllGates( PacketHeader* packet );

		bool SendToMap( uint16 mapId, PacketHeader* packet );
		void SendToAllMaps( PacketHeader* packet );

		/// 收到网关服务器的配置信息
		void OnRecv_GateInfo( GateHandler* gateHandler, PacketHeader* packet );
		/// 收到地图服务器的配置信息
		void OnRecv_MapServerInfo( MapHandler* mapHandler, PacketHeader* packet );

		LoginPlayer* GetLoginPlayer( uint32 accountId );
		CenterPlayer* GetCenterPlayerById( uint32 roleId );
		CenterPlayer* GetCenterPlayerByAccountId( uint32 accountId );

		void RemoveLoginPlayer( uint32 accountId );

		/// 帐号登录
		void OnRecv_Login( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_LoginResult( PacketHeader* packet );

		void OnRecv_CreateAccount( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_CreateAccountResult( PacketHeader* packet );

		void OnRecv_RoleList( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_CreateRole( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_ChooseRole( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_DeleteRole( GateHandler* gateHandler, PacketHeader* packet );

		void OnRecv_RoleListResult( PacketHeader* packet );
		void OnRecv_CreateRoleResult( PacketHeader* packet );
		void OnRecv_ChooseRoleResult( PacketHeader* packet );
		void OnRecv_DeleteRoleResult( PacketHeader* packet );
		void OnRecv_EnterWorld( PacketHeader* packet );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}