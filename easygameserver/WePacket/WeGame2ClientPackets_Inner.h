/********************************************************************
	created:	2009-6-27
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	游戏服务器发给客户端的内部消息
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_GameServer2Client_Inner
	{
		PT_GS2C_Inner_CreateAccountResult=1,/// 注册账号结果
		PT_GS2C_Inner_LoginResult,			/// 登陆结果
		PT_GS2C_Inner_RoleListResult,		/// 角色列表结果
		PT_GS2C_Inner_CreateRoleResult,		/// 创建角色结果
		PT_GS2C_Inner_ChooseRoleResult,		/// 选择角色结果
		PT_GS2C_Inner_DeleteRoleResult,		/// 删除角色结果
		PT_GS2C_Inner_EnterWorld,			/// 角色进入游戏
		PT_GS2C_Inner_WaitLogin,			/// 非正常登陆,通知客户端稍作等待

		PT_GS2C_Inner_SyncRemovePlayers,	/// 某个区块的所有玩家离开了视野
		PT_GS2C_Inner_SyncRemoveNpcs,		/// 某个区块的所有Npc离开了视野
		PT_GS2C_Inner_SyncRemoveMonsters,	/// 某个区块的所有怪物离开了视野
		PT_GS2C_Inner_SyncRemoveItems,		/// 某个区块的所有物品离开了视野

		PT_GS2C_Inner_MapList,				/// 房间列表	
		PT_GS2C_Inner_JoinMap,				/// 进入房间
		PT_GS2C_Inner_PlayerEntryMap,		/// 玩家进房间了
		PT_GS2C_Inner_PlayerLeaveMap,		/// 玩家离开房间了
		//PT_GS2C_Inner_GetObjData,			/// 返回对象的数据

		PT_GS2C_Inner_MAX,
	};

#define PACKET_GS2C_INNER_LOGIN_BEG( packetName ) \
	struct Packet_GS2C_Inner_##packetName## : PacketHeader\
	{\
	Packet_GS2C_Inner_##packetName##() { m_MainType=0; m_SubType = PT_GS2C_Inner_##packetName##; m_Length=sizeof( Packet_GS2C_Inner_##packetName## ); }

#define PACKET_GS2C_INNER_BEG( packetName ) \
	struct Packet_GS2C_Inner_##packetName## : PacketHeader\
	{\
	Packet_GS2C_Inner_##packetName##() { m_MainType=0; m_SubType = PT_GS2C_Inner_##packetName##; m_Length=sizeof( Packet_GS2C_Inner_##packetName## ); }

#define PACKET_GS2C_INNER_END };

	PACKET_GS2C_INNER_LOGIN_BEG( CreateAccountResult )
		enum CreateAccountResult
		{
			CreateAccountResult_Unknown = 0,
			CreateAccountResult_OK,
			CreateAccountResult_AccountExist,	/// 帐号已经存在
		};
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		uint8	m_Result;
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_LOGIN_BEG( LoginResult )
		enum LoginResult
		{
			LoginResult_Unknown = 0,
			LoginResult_OK,
			LoginResult_OKAndReconnect,		/// 验证OK,等待断线重连
			LoginResult_AccountOrPwdError,	/// 帐号或密码错误
			LoginResult_MibaoError,			/// 密保卡错误
		};
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		uint8	m_Result;
	PACKET_GS2C_INNER_END

	/// 该消息可以考虑压缩,实际效果有待测试对比
	PACKET_GS2C_INNER_LOGIN_BEG( RoleListResult )
		uint8	m_RoleCount;								/// 角色数目
		uint32	m_AccountId;
		//uint8  m_Result;								///
		uint8   m_RoleListData[MAX_ROLELIST_SIZE];			/// 具体的数据由逻辑部分来解析
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_LOGIN_BEG( CreateRoleResult )
		enum CreateRoleResult
		{
			CreateRoleResult_Unknown = 0,
			CreateRoleResult_OK,
			CreateRoleResult_RoleNameExist,		/// 角色名已经存在
		};
		uint8	m_Result;
		uint8	m_RoleIndex;
		uint32  m_RoleId;
		char	m_RoleName[MAX_ROLE_NAME+1];
		uint8   m_CreateRoleData[MAX_CREATEROLE_SIZE];	/// 详细的角色数据,由逻辑部分来解析
	PACKET_GS2C_INNER_END

	/// 该消息可以考虑压缩,实际效果有待测试对比
	PACKET_GS2C_INNER_LOGIN_BEG( ChooseRoleResult )
		uint32	m_AccountId;
		uint8	m_RoleData[MAX_ROLE_DATA_SIZE];	/// 继承SRoleInfo_Header
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_LOGIN_BEG( EnterWorld )
		uint8	m_PlayerData[MAX_ROLE_DATA_SIZE];
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_LOGIN_BEG( WaitLogin )
		enum WaitLoginReason
		{
			WaitLoginReason_RoleReconnect,		/// 角色断线重连,等待返回数据
			WaitLoginReason_RemoveOldOnlineRole,/// 同一账号的其他角色还在游戏中,等待旧角色退出游戏
		};
		uint32	m_LoginPlayerId; 
		uint8	m_Reason;		/// 等待原因
	PACKET_GS2C_INNER_END


	PACKET_GS2C_INNER_BEG( SyncRemovePlayers )
		uint16 m_ObjCount;
		uint32 m_ObjIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*sizeof(uint32));
		}
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( SyncRemoveNpcs )
		uint16 m_ObjCount;
		uint32 m_ObjIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*sizeof(uint32));
		}
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( SyncRemoveMonsters )
		uint16 m_ObjCount;
		uint32 m_ObjIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*sizeof(uint32));
		}
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( SyncRemoveItems )
		uint16 m_ObjCount;
		uint32 m_ObjIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*sizeof(uint32));
		}
	PACKET_GS2C_INNER_END

	struct MapInfo
	{
		char	m_Name[MAX_MAP_NAME+1];	/// 房间名
		uint16	m_MapId;				/// 房间id
		uint16	m_PlayerNum;			/// 房间里的玩家数量
	};
	PACKET_GS2C_INNER_BEG( MapList )
		uint16 m_MapCount;
		uint8  m_Data[MAX_MAPLIST_DATA_SIZE];
	PACKET_GS2C_INNER_END

	struct SyncObjectId
	{
		uint32 m_ObjId;
		uint16 m_DataChecker;	/// 数据变化的标识
	};

	PACKET_GS2C_INNER_BEG( JoinMap )
		uint16			m_ObjCount;
		SyncObjectId	m_SyncObjectIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*(sizeof(SyncObjectId)));
		}
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( PlayerEntryMap )
		SyncObjectId m_SyncObjectId;
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( PlayerLeaveMap )
		uint32 m_ObjId;
	PACKET_GS2C_INNER_END

#pragma pack()
}
