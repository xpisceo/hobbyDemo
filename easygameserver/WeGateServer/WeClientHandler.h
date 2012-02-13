/********************************************************************
	created:	2009-6-27
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	客户端->网关服务器
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"
#include <string>
using namespace std;

namespace We
{
	class GateServer;

	/// 客户端的状态
	enum ClientStatus
	{
		ClientStatus_AccountLogin,			/// 帐号验证
		ClientStatus_EnterWorld,			/// 进入游戏世界
	};

	class ClientHandler : public SocketHandler
	{
	public:
		ClientHandler();
		~ClientHandler();

		void Init();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		inline uint32 GetId() const { return m_Id; }
		inline ClientStatus GetClientStatus() const { return m_ClientStatus; }
		inline void SetClientStatus( ClientStatus clientStatus ) { m_ClientStatus = clientStatus; }
		inline uint32 GetLoginFailCount() const { return m_LoginFailCount; }
		inline void SetLoginFailCount( uint32 loginFailCount ) { m_LoginFailCount = loginFailCount; }

		void Update();

	public:
		uint32 m_Id;					/// 登录期间生成的唯一Id
		GateServer* m_GateServer;

		string	m_AccountName;			/// 账号名
		string	m_RoleName;				/// 角色名
		uint32  m_AccountId;			/// 帐号数据库Id
		uint32  m_RoleId;				/// 角色数据库Id
		ClientStatus m_ClientStatus;	/// 当前状态	
		uint16  m_MapId;				/// 当前所在的地图编号

		uint32 m_ConnectTime;			/// 记录连接的时间
		uint32 m_LoginFailCount;		/// 登录失败次数统计
	};
}