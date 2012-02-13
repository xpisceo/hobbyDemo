/********************************************************************
	created:	2009-6-28
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	登陆中的玩家和中心服务器上的玩家
*********************************************************************/
#pragma once

#include "WePacketDefine_Inner.h"
#include "WePlayerStruct.h"

namespace We
{
	/// 玩家状态
	enum CenterPlayerStatus
	{
		CenterPlayerStatus_None,
		CenterPlayerStatus_ChooseRoleWait,	/// 选择了角色,等待数据库返回该角色的数据
		CenterPlayerStatus_EnterWorldWait,	/// 获取了角色数据,向地图服务器发送消息,等待地图服务器创建玩家对象并放入地图
		CenterPlayerStatus_ChangeMapWait,	/// 从一个地图服务器转向另一个地图服务器,等待另一个地图服务器创建玩家对象并放入地图
		CenterPlayerStatus_InMap,			/// 在某一个地图中,正常游戏

		CenterPlayerStatus_MAX,
	};

	/// 登录期的玩家(角色列表,创建角色,删除角色,选择角色)
	class LoginPlayer
	{
	public:
		LoginPlayer() { m_Id = 0; m_AccountId = 0; m_LoginTime=0; m_GateId=0;
						m_RoleCount = 0; }

		uint32 m_Id;						/// 生成的唯一id
		uint32 m_AccountId;					/// 帐号id
		string m_AccountName;				/// 帐号名
		uint32 m_LoginTime;					/// 记录登录成功的时间
		uint8  m_GateId;					/// 网关id
		uint8  m_RoleCount;					/// 角色数量
		uint32 m_RoleIds[MAX_ROLE_NUM];		/// 角色id列表
		string m_RoleNames[MAX_ROLE_NUM];	/// 角色名列表
	};

	/// 选择角色后的玩家
	class CenterPlayer
	{
	public:
		CenterPlayer() { m_AccountId=0; m_RoleId=0; m_GateId=0; m_MapId=0; m_OldMapId=0; m_RoleIndex=-1;
						 m_IsWaitReconnect=false; m_IsCache=false; m_PlayerStatus=CenterPlayerStatus_None;}

		uint32 m_AccountId;			/// 帐号id
		string m_AccountName;		/// 帐号名
		uint32 m_RoleId;			/// 角色id
		string m_RoleName;			/// 角色名
		int8   m_RoleIndex;			/// 角色索引
		uint8  m_GateId;			/// 网关id
		uint16 m_MapId;				/// 地图id
		uint16 m_OldMapId;			/// 上一个地图id
		CenterPlayerStatus m_PlayerStatus;		/// 玩家状态标记
		uint32 m_StartTime;						/// 状态开始的时间
		bool   m_IsWaitReconnect;				/// 正在等待断线重连
		bool   m_IsCache;						/// 是否缓存了
		SRoleInfo_DB m_Cache;					/// 角色数据缓存
	};
}