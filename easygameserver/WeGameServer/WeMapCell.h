/********************************************************************
	created:	2009-8-2
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	地图块(九宫格)
				-------------
				| 1 | 2 | 3 |
				-------------
				| 4 | 5 | 6 |
				-------------
				| 7 | 8 | 9 |
				-------------
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WePacketHeader.h"
#include "WeMapObject.h"

/// 区块的大小
#define CELL_X_SIZE 360
#define CELL_Y_SIZE 260

namespace We
{
	class Map;
	class Player;
	class Npc;
	class Monster;
	class Item;

	class MapCell
	{
		friend class Map;
	protected:
		MapObjectMgr<Player>	m_Players;		/// 该地图块里的玩家
		MapObjectMgr<Npc>		m_Npcs;			/// 该地图块里的NPC
		MapObjectMgr<Monster>	m_Monsters;		/// 该地图块里的怪物
		MapObjectMgr<Item>		m_Items;		/// 该地图块里的物品

		uint16 m_CellX;		/// X轴上的区块索引
		uint16 m_CellY;		/// Y轴上的区块索引
	public:
		MapCell();
		~MapCell();

		bool AddMapObject( MapObject* obj );
		void RemoveMapObject( MapObject* obj );

		inline uint16 GetCellX() const { return m_CellX; }
		inline uint16 GetCellY() const { return m_CellY; }

		/// 发数据包给格子里的所有玩家
		void SendToCellPlayers( PacketHeader* packet, uint32 ignoreObjId );

		/// 一个对象进入格子时,做同步数据
		void SyncObjectEnterCell( MapObject* enterObj, PacketHeader* syncPacketEnterView, uint32 ignoreObjId );

		/// 一个对象离开格子时,做同步数据
		/// 这个消息可以考虑不发
		void SyncObjectLeaveCell( MapObject* leaveObj, PacketHeader* syncPacketLeaveView, uint32 ignoreObjId );

	};
}