/********************************************************************
	created:	2009-8-1
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	地图
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeMapObject.h"

namespace We
{
	class MapCell;
	class Player;
	class Npc;
	class Monster;
	class Item;

	class Map
	{
		friend class World;
	protected:
		uint16		m_MapId;					/// 地图id
		std::string m_MapName;					/// 地图名字

		uint32		m_MapWidth;					/// 地图X轴长度
		uint32		m_MapHeight;				/// 地图Y轴长度
		uint16		m_CellColumns;				/// X轴上的区块数目
		uint16		m_CellRows;					/// Y轴上的区块数目
		MapCell**	m_Cells;					/// 2维的区块

		MapObjectMgr<Player>	m_Players;		/// 该地图里的玩家
		MapObjectMgr<Npc>		m_Npcs;			/// 该地图里的NPC
		MapObjectMgr<Monster>	m_Monsters;		/// 该地图里的怪物
		MapObjectMgr<Item>		m_Items;		/// 该地图里的物品

	public:
		Map();
		~Map();

		uint16 GetMapId() const { return m_MapId; }
		const std::string& GetMapName() const { return m_MapName; }

		uint32 GetMapWidth() const { return m_MapWidth; }
		uint32 GetMapHeight() const { return m_MapHeight; }

		/// 加载地图数据
		bool LoadMap();

		/// 把对象放到地图上的某个坐标
		bool AddMapObject( MapObject* obj, float x, float y, float z );

		/// 从地图上移除对象
		void RemoveMapObject( MapObject* obj );

		/// 获取坐标所在的区块
		MapCell* GetCell( int x, int y );

		///
		MapCell* GetCellByIndex( int cell_x, int cell_y );

		/// 某个坐标是否可以行走
		bool CanWalk( float x, float y, float z );

		/// 广播地图消息(房间消息)
		void SendMapPacket( const PacketHeader* packet, uint32 ignoreId=0 );

		void Update();
	};
}