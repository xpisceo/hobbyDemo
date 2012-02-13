/********************************************************************
	created:	2009-6-27
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	网关服务器
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"
#include "WeCenterHandler.h"
#include "WeMapHandler.h"

namespace We
{
	class ClientHandler;

	class GateServer : public SocketListenerHandler
	{
	public:
		unsigned int m_ClientHandlerId;

		unsigned int m_TotalRecvMsgSizeSec;
		unsigned int m_TotalRecvMsgSize;
		unsigned int m_TotalSendMsgSizeSec;
		unsigned int m_TotalSendMsgSize;

		unsigned int m_LastSecTick;
		unsigned int m_LastShowTick;
		unsigned int m_LastSendTick;

		uint8	m_GateServerId;
		string	m_ClientListenIP;
		uint16	m_ClientListenPort;
		uint32	m_MaxClientConnection;

		string	m_CenterServerIP;
		uint16	m_CenterServerPort;
	protected:
		map<uint32, ClientHandler*> m_LogingClientHandlers;		/// 登录中的客户端(以生成的唯一id作为标识)
		map<uint32, ClientHandler*> m_ClientHandlers;			/// 已经登录的客户端(以角色id作为标识)

		CenterHandler	m_CenterServer;							/// 与中心服务器的连接
		map<uint16, MapHandler*>	m_MapServers;				/// 与地图服务器的连接
	public:
		GateServer();
		~GateServer();

		inline uint8 GetGateServerId() const { return m_GateServerId; }

		bool Init();
		void Shutdown();

		/// 新客户端连接
		virtual void OnAccept( Socket* socket );
		/// 移除客户端连接
		virtual void OnRemove( Socket* socket );

		/// 发数据包给中心服务器
		bool SendToCenter( PacketHeader* packet );

		/// 发数据包给地图服务器
		bool SendToMap( uint16 mapId, PacketHeader* packet );

		/// 新增一个地图服务器
		void OnAddMapServer( PacketHeader* packet );

		/// 移除一个地图服务器
		void OnRemoveMapServer( PacketHeader* packet );

		/// 客户端登录结果返回
		void OnClientLogin( PacketHeader* packet );

		/// 客户端选择了角色
		void OnClientChooseRole( PacketHeader* packet );

		/// 进入游戏世界
		void OnClientEnterWorld( PacketHeader* packet );

		/// 转发登录期的消息->Client
		bool DispatchLoginPacket( PacketHeader* packet );

		/// 转发游戏中的消息->Client
		bool DispatchPacket( PacketHeader* packet );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}