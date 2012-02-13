/********************************************************************
	created:	2009-7-21
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	DB服务器发给游戏服务器的内部消息
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"
#include "WeGame2ClientPackets_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_DB2GameServer_Inner
	{
		PT_D2GS_Inner_CreateRoleResult = 1,		/// 创建角色返回
		PT_D2GS_Inner_RoleListResult,			/// 角色列表返回
		PT_D2GS_Inner_ChooseRoleResult,			/// 选择角色结果
		PT_D2GS_Inner_DeleteRoleResult,			/// 删除角色结果

		PT_D2GS_Inner_MAX,
	};

#define PACKET_D2GS_INNER_BEG( packetName ) \
	struct Packet_D2GS_Inner_##packetName## : We::PacketHeader\
	{\
	Packet_D2GS_Inner_##packetName##() { m_MainType=0; m_SubType = PT_D2GS_Inner_##packetName##; m_Length=sizeof( Packet_D2GS_Inner_##packetName## ); }

#define PACKET_D2GS_INNER_END };

	PACKET_D2GS_INNER_BEG( CreateRoleResult )
		enum CreateRoleResult
		{
			CreateRoleResult_Unknown = 0,
			CreateRoleResult_OK,
			CreateRoleResult_RoleNameExist,		/// 角色名已经存在
			CreateRoleResult_RoleIndexExist,	/// 该位置已经有角色了
			CreateRoleResult_SQLError,			/// 数据库错误
		};
		uint32	m_AccountId;
		uint8	m_Result;
		uint8	m_RoleIndex;
		uint32  m_RoleId;
		char	m_RoleName[MAX_ROLE_NAME+1];
		uint8   m_CreateRoleData[MAX_CREATEROLE_SIZE];	/// 详细的角色数据,由逻辑部分来解析
	PACKET_D2GS_INNER_END

	PACKET_D2GS_INNER_BEG( ChooseRoleResult )
		enum ChooseRoleResult
		{
			ChooseRoleResult_Unknown = 0,
			ChooseRoleResult_OK,
			ChooseRoleResult_SQLError,			/// 数据库错误
		};
		uint8	m_Result;
		uint32	m_AccountId;
		uint8	m_RoleData[MAX_ROLE_DATA_SIZE];	/// 继承SRoleInfo_Header
	PACKET_D2GS_INNER_END

	/// 该消息可以考虑压缩,实际效果有待测试对比
	PACKET_D2GS_INNER_BEG( RoleListResult )
		uint8	m_RoleCount;								/// 角色数目
		uint32	m_AccountId;
		//uint8  m_Result;									///
		uint8   m_RoleListData[MAX_ROLELIST_SIZE];			/// 具体的数据由逻辑部分来解析
	PACKET_D2GS_INNER_END

#pragma pack()
}
