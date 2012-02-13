#include "WeMap.h"
#include "WeMapCell.h"
#include "WePlayer.h"
#include "WeNpc.h"
#include "WeMonster.h"
#include "WeItem.h"

#include "WeGame2ClientPackets_Inner.h"

namespace We
{
	Map::Map()
	{
		m_MapId = 0;
		m_Cells = 0;
		m_CellRows = 0;
		m_CellColumns = 0;
		m_MapWidth = 0;
		m_MapHeight = 0;
	}
	Map::~Map()
	{
	}
	bool Map::LoadMap()
	{
#pragma warning( disable : 4244 )
		/// 测试用
		m_MapWidth = 1024*10;
		m_MapHeight = 768*10;
		m_CellColumns = m_MapWidth/CELL_X_SIZE;
		m_CellRows = m_MapHeight/CELL_Y_SIZE;
		m_Cells = new MapCell*[m_CellRows];
		for( int y=0; y<m_CellRows; ++y )
		{
			m_Cells[y] = new MapCell[m_CellColumns];
			for( int x=0; x<m_CellColumns; ++x )
			{
				m_Cells[y][x].m_CellX = x;
				m_Cells[y][x].m_CellY = y;
			}
		}
		/// 生成一些NPC和怪物
		for( int i=0; i<20; ++i )
		{
			Npc* npc = new Npc();
			npc->m_Map = this;
			npc->m_ObjId = MapObject::NewNpcId();
			npc->MoveTo( GetMapId(), 100 + rand()%2000, 200 + rand()%500, 0 );
		}
		for( int i=0; i<10; ++i )
		{
			Monster* monster = new Monster();
			monster->m_Map = this;
			monster->m_ObjId = MapObject::NewMonsterId();
			monster->m_BornPos.x = 200 + ::rand()%8000;
			monster->m_BornPos.y = 200 + ::rand()%300;
			monster->m_BornPos.z = 0;
			monster->m_MoveRange = 500;
			monster->MoveTo( GetMapId(), monster->m_BornPos.x, monster->m_BornPos.y, monster->m_BornPos.z );
		}
		return true;
	}

	bool Map::AddMapObject( MapObject* obj, float x, float y, float z )
	{
#pragma warning( disable : 4244 )
		if( obj == 0 )
			return false;
		MapCell* cell = GetCell( x, y );
		if( cell == 0 )
			return false;
		if( !CanWalk( x, y, z ) )
			return false;
		MapCell* oldCell = obj->GetCell();
		/// 从旧的区块中移除,并加入新的区块
		if( oldCell != cell )
		{
			if( oldCell != 0 )
				oldCell->RemoveMapObject( obj );
			cell->AddMapObject( obj );
		}
		obj->m_Pos.x = x;
		obj->m_Pos.y = y;
		obj->m_Map = this;
		if( obj->IsPlayer() )
		{
			m_Players.AddObject( (Player*)obj );
		}
		else if( obj->IsNpc() )
		{
			m_Npcs.AddObject( (Npc*)obj );
		}
		else if( obj->IsMonster() )
		{
			m_Monsters.AddObject( (Monster*)obj );
		}
		else if( obj->IsItem() )
		{
			m_Items.AddObject( (Item*)obj );
		}
		return true;
	}
	void Map::RemoveMapObject( MapObject* obj )
	{
		if( obj == 0 )
			return;
		if( obj->GetCell() != 0 )
		{
			obj->GetCell()->RemoveMapObject( obj );
			obj->m_Cell = 0;
		}
		if( obj->IsPlayer() )
		{
			m_Players.RemoveObject( obj->GetObjId() );
		}
		else if( obj->IsNpc() )
		{
			m_Npcs.RemoveObject( obj->GetObjId() );
		}
		else if( obj->IsMonster() )
		{
			m_Monsters.RemoveObject( obj->GetObjId() );
		}
		else if( obj->IsItem() )
		{
			m_Items.RemoveObject( obj->GetObjId() );
		}
		else
		{
			assert( false );
			return;
		}
	}
	MapCell* Map::GetCell( int x, int y )
	{
		int cell_x = x/CELL_X_SIZE;
		int cell_y = y/CELL_Y_SIZE;
		return GetCellByIndex( cell_x, cell_y );
	}
	MapCell* Map::GetCellByIndex( int cell_x, int cell_y )
	{
		if( cell_x < 0 || cell_x >= m_CellColumns
			|| cell_y < 0 || cell_y >= m_CellRows )
			return 0;
		return &(m_Cells[cell_y][cell_x]);
	}
	bool Map::CanWalk( float x, float y, float z )
	{
		return true;
	}
	void Map::SendMapPacket( const PacketHeader* packet, uint32 ignoreId/*=0*/ )
	{
		std::map<uint32,Player*>::iterator i = m_Players.m_MapObjects.begin();
		for( i; i!=m_Players.m_MapObjects.end(); ++i )
		{
			Player* targetPlayer = i->second;
			if( targetPlayer == 0 )
				continue;
			if( targetPlayer->GetRoleId() == ignoreId )
				continue;
			targetPlayer->SendPacket( packet );
		}
	}
	void Map::Update()
	{
		m_Players.Update();
		m_Npcs.Update();
		m_Monsters.Update();
		m_Items.Update();
	}
}