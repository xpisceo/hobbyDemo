/********************************************************************
	created:	2009-8-6
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	地图消息
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"
#include "WePacketEnum.h"
#include "WePlayerStruct.h"

namespace We
{
#pragma pack(1)

	enum MapPacketType
	{
		MapPacketType_Sync_Npc = 1,
		MapPacketType_Sync_Monster,
		MapPacketType_Sync_Item,
		MapPacketType_Sync_Player,

		MapPacketType_SyncObjectMove,		/// 对象移动
	};

#define MAP_PACKET_BEG( packetName ) \
	struct MapPacket_##packetName## : We::PacketHeader\
	{\
	MapPacket_##packetName##() { m_MainType=PacketType_Map; m_SubType = MapPacketType_##packetName##; m_Length=sizeof( MapPacket_##packetName## ); }

#define MAP_PACKET_END };

	struct MapPacket_Sync_Npc : PacketHeader
	{
		MapPacket_Sync_Npc() { m_MainType = PacketType_Map; m_SubType = MapPacketType_Sync_Npc; m_Length=sizeof(*this); }
		uint32 m_ObjId;
		uint16 m_NpcId;
		Position	m_CurrentPos;	/// 当前坐标
	};

	struct MapPacket_Sync_Monster : PacketHeader
	{
		MapPacket_Sync_Monster() { m_MainType = PacketType_Map; m_SubType = MapPacketType_Sync_Monster; m_Length=sizeof(*this); }
		uint32		m_ObjId;
		uint16		m_MonsterId;
		Position	m_CurrentPos;	/// 当前坐标
		Position	m_TargetPos;	/// 目标坐标
	};

	struct MapPacket_Sync_Item : PacketHeader
	{
		MapPacket_Sync_Item() { m_MainType = PacketType_Map; m_SubType = MapPacketType_Sync_Item; m_Length=sizeof(*this); }
		uint32		m_ObjId;
		uint16		m_ItemId;
		Position	m_CurrentPos;	/// 当前坐标
	};

	struct MapPacket_Sync_Player : PacketHeader
	{
		MapPacket_Sync_Player() { m_MainType = PacketType_Map; m_SubType = MapPacketType_Sync_Player; m_Length=sizeof(*this); }
		uint32			m_PlayerId;
		char			m_RoleName[MAX_ROLE_NAME+1]; /// 15
		Position		m_Pos;
		Position		m_TargetPos;		/// 目标坐标
		uint8			m_Sex:2;			/// 性别(0-1)
		uint8			m_Race:4;			/// 种族(0-15)
		uint8			m_dummy2:2;			/// 预留
		SPlayerVisual	m_Visual;			/// 玩家的外观
	};

	MAP_PACKET_BEG( SyncObjectMove )
		uint32		m_ObjId;		/// 对象id
		Position	m_CurrentPos;	/// 当前坐标
		Position	m_TargetPos;	/// 目标坐标
	MAP_PACKET_END

#pragma pack()
}