/********************************************************************
	created:	2009-7-19
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	游戏服务器发给DB服务器的内部消息
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"
#include "WeClient2GamePackets_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_GameServer2DB_Inner
	{
		PT_GS2D_Inner_GameServerInfo = 1,			/// 游戏服务器的配置信息
		PT_GS2D_Inner_RoleList,						/// 请求角色列表数据
		PT_GS2D_Inner_CreateRole,					/// 创建角色
		PT_GS2D_Inner_ChooseRole,					/// 选择角色
		PT_GS2D_Inner_DeleteRole,					/// 删除角色
		PT_GS2D_Inner_SaveRoleData,					/// 保存角色数据

		PT_GS2D_Inner_MAX,
	};

#define PACKET_GS2D_INNER_BEG( packetName ) \
	struct Packet_GS2D_Inner_##packetName## : We::PacketHeader\
	{\
	Packet_GS2D_Inner_##packetName##() { m_MainType=0; m_SubType = PT_GS2D_Inner_##packetName##; m_Length=sizeof( Packet_GS2D_Inner_##packetName## ); }

#define PACKET_GS2D_INNER_END };

	PACKET_GS2D_INNER_BEG( GameServerInfo )
		uint16	m_GameServerId;			/// 游戏服务器的id
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( RoleList )
		uint32 m_AccountId;
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( CreateRole )
		uint32  m_AccountId;							/// size=4	
		uint8	m_RoleIndex;							/// size=1
		char	m_RoleName[MAX_ROLE_NAME+1];			/// size=15
		uint8   m_CreateRoleData[MAX_CREATEROLE_SIZE];	/// 详细的角色数据,由逻辑部分来解析
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( ChooseRole )
		uint32  m_AccountId;
		uint32 m_RoleId;
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( DeleteRole )
		uint32	m_RoleId;
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( SaveRoleData )
		uint32	m_RoleId;
		uint8   m_SaveRoleData[MAX_ROLE_DATA_SIZE];		/// 要保存的角色数据,由逻辑部分来解析
	PACKET_GS2D_INNER_END

#pragma pack()
}
