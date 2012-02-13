#include "WeMapObject.h"
#include "WeWorld.h"
#include "WeMap.h"
#include "WeMapCell.h"
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include "WeGame2ClientPackets_Inner.h"
#include "WeMapPacket.h"

namespace We
{
	uint32 MapObject::m_NpcSerial = 0;
	uint32 MapObject::m_MonsterSerial = 0;
	uint32 MapObject::m_ItemSerial = 0;

	MapObject::MapObject()
	{
		m_Map = 0;
		m_Cell = 0;
		m_ObjId = 0;
		::memset( &m_Pos, 0, sizeof(m_Pos) );
		::memset( &m_TargetPos, 0, sizeof(m_TargetPos) );
		m_LastMoveTime = m_MoveStartTime = m_MoveSpeed = m_StepCount = m_StepIndex = 0;
		m_StepLength = m_StepX = m_StepY = 0.0f;
		m_IsMoving = false;

		m_StepLength = 20.0f;	/// 每步20个像素
		m_MoveSpeed = 200;		/// 每秒走5步
	}
	MapObject::~MapObject()
	{
	}
	bool MapObject::MoveTo( uint16 mapId, float x, float y, float z )
	{
#pragma warning( disable : 4244 )
		Map* map = World::getSingleton().GetMapById( mapId );
		if( map == 0 )
			return false;
		Map* oldMap = m_Map;
		/// 切换了地图或者第一次进地图
		if( oldMap != map || m_Cell == 0 )
		{
			MapCell* cell = map->GetCell( x, y );
			if( cell == 0 )
				return false;
			/// 从旧的地图移除
			if( oldMap != 0 )
				oldMap->RemoveMapObject( this );
			/// 加入新的地图
			if( !map->AddMapObject( this, x, y, z ) )
				return false;
			/// 同步新的区块
			PacketHeader* packetEnterView = GetSyncPacket_EnterView();
			for( int cx=-1; cx<=1; ++cx )
			{
				for( int cy=-1; cy<=1; ++cy )
				{
					if( MapCell* cell_1 = map->GetCellByIndex( cell->GetCellX() + cx, cell->GetCellY() + cy ) )
						cell_1->SyncObjectEnterCell( this, packetEnterView, 0 ); /// 是否排除玩家自己??????
				}
			}
			m_Pos.x = x;
			m_Pos.y = y;
			m_Pos.z = z;
		}
		else
			MoveTo( x, y, z );
		return true;
	}
	bool MapObject::MoveTo( float x, float y, float z )
	{
		if( GetMap() == 0 )
			return false;
		MapCell* cell = GetMap()->GetCell( x, y );
		if( cell == 0 )
			return false;
		if( m_Cell == 0 )
			return false;
		/// 切换了区块
		if( cell != m_Cell )
		{
			Map* map = GetMap();
			int cell_d_x = cell->GetCellX() - m_Cell->GetCellX();
			int cell_d_y = cell->GetCellY() - m_Cell->GetCellY();
			/*
			横向移动了1格: A->B
			1,2,3格子里的对象和A从相互的视野里移除
			4,5,6格子里的对象和A加入相互的视野
			---------------------
			|	|	|   |	|	|
			---------------------
			|	| 1 |   |	| 4 |
			---------------------
			|	| 2 | A | B | 5 |
			---------------------
			|	| 3 |   |	| 6 |
			---------------------
			|	|	|   |	|	|
			---------------------
			*/
			if( ::abs(cell_d_x) == 1 && cell_d_y == 0 )
			{
				PacketHeader* packetLeaveView = GetSyncPacket_LeaveView();
				if( MapCell* cell_1 = map->GetCellByIndex( m_Cell->GetCellX() - cell_d_x, m_Cell->GetCellY() - 1 ) )
					cell_1->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_2 = map->GetCellByIndex( m_Cell->GetCellX() - cell_d_x, m_Cell->GetCellY() ) )
					cell_2->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_3 = map->GetCellByIndex( m_Cell->GetCellX() - cell_d_x, m_Cell->GetCellY() + 1 ) )
					cell_3->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );

				PacketHeader* packetEnterView = GetSyncPacket_EnterView();
				if( MapCell* cell_4 = map->GetCellByIndex( cell->GetCellX() + cell_d_x, m_Cell->GetCellY() - 1 ) )
					cell_4->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_5 = map->GetCellByIndex( cell->GetCellX() + cell_d_x, m_Cell->GetCellY() ) )
					cell_5->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_6 = map->GetCellByIndex( cell->GetCellX() + cell_d_x, m_Cell->GetCellY() + 1 ) )
					cell_6->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
			}
			/*
			纵向移动了1格: A->B
			1,2,3格子里的对象和A从相互的视野里移除
			4,5,6格子里的对象和A加入相互的视野
			---------------------
			|	| 1 | 2 | 3 |   |
			---------------------
			|	|   | A |   |   |
			---------------------
			|	|   | B |	|   |
			---------------------
			|	| 4 | 5 | 6 |	|
			---------------------
			*/
			else if( cell_d_x == 0 && ::abs(cell_d_y) == 1 )
			{
				PacketHeader* packetLeaveView = GetSyncPacket_LeaveView();
				if( MapCell* cell_1 = map->GetCellByIndex( m_Cell->GetCellX() - 1, m_Cell->GetCellY() - cell_d_y ) )
					cell_1->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_2 = map->GetCellByIndex( m_Cell->GetCellX(), m_Cell->GetCellY() - cell_d_y ) )
					cell_2->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_3 = map->GetCellByIndex( m_Cell->GetCellX() + 1, m_Cell->GetCellY() - cell_d_y ) )
					cell_3->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );

				PacketHeader* packetEnterView = GetSyncPacket_EnterView();
				if( MapCell* cell_4 = map->GetCellByIndex( m_Cell->GetCellX() - 1, cell->GetCellY() + cell_d_y ) )
					cell_4->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_5 = map->GetCellByIndex( m_Cell->GetCellX(), cell->GetCellY() + cell_d_y ) )
					cell_5->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_6 = map->GetCellByIndex( m_Cell->GetCellX() + 1, cell->GetCellY() + cell_d_y ) )
					cell_6->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
			}
			/*
			斜着移到邻近的格子: A->B
			1,2,3,4,5格子里的对象和A从相互的视野里移除
			6,7,8,9,10格子里的对象和A加入相互的视野
			---------------------
			|	|	| 6 | 7 | 8 |
			---------------------
			|	| 1 |   | B | 9 |
			---------------------
			|	| 2 | A |   | 10|
			---------------------
			|	| 3 | 4 | 5 |	|
			---------------------
			*/
			else if( ::abs(cell_d_x) == 1 && ::abs(cell_d_y) == 1 )
			{
				PacketHeader* packetLeaveView = GetSyncPacket_LeaveView();
				if( MapCell* cell_1 = map->GetCellByIndex( m_Cell->GetCellX() - cell_d_x, m_Cell->GetCellY() + cell_d_y ) )
					cell_1->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_2 = map->GetCellByIndex( m_Cell->GetCellX() - cell_d_x, m_Cell->GetCellY() ) )
					cell_2->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_3 = map->GetCellByIndex( m_Cell->GetCellX() - cell_d_x, m_Cell->GetCellY() - cell_d_y ) )
					cell_3->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_4 = map->GetCellByIndex( m_Cell->GetCellX(),			  m_Cell->GetCellY() - cell_d_y ) )
					cell_4->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_5 = map->GetCellByIndex( m_Cell->GetCellX() + cell_d_x, m_Cell->GetCellY() - cell_d_y ) )
					cell_5->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );

				PacketHeader* packetEnterView = GetSyncPacket_EnterView();
				if( MapCell* cell_6 = map->GetCellByIndex( cell->GetCellX() - cell_d_x, cell->GetCellY() + cell_d_y ) )
					cell_6->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_7 = map->GetCellByIndex( cell->GetCellX(),			cell->GetCellY() + cell_d_y ) )
					cell_7->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_8 = map->GetCellByIndex( cell->GetCellX() + cell_d_x, cell->GetCellY() + cell_d_y ) )
					cell_8->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_9 = map->GetCellByIndex( cell->GetCellX() + cell_d_x, cell->GetCellY() ) )
					cell_9->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
				if( MapCell* cell_10 = map->GetCellByIndex( cell->GetCellX()+ cell_d_x, cell->GetCellY() - cell_d_y ) )
					cell_10->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
			}
			/// 完全移出了以前的九宫格
			else if( ::abs(cell_d_x) > 2 || ::abs(cell_d_y) > 2 )
			{
				PacketHeader* packetLeaveView = GetSyncPacket_LeaveView();
				for( int cx=-1; cx<=1; ++cx )
				{
					for( int cy=-1; cy<=1; ++cy )
					{
						if( MapCell* cell_1 = map->GetCellByIndex( m_Cell->GetCellX() + cx, m_Cell->GetCellY() + cy ) )
							cell_1->SyncObjectLeaveCell( this, packetLeaveView, (IsPlayer()?GetObjId():0) );
					}
				}
				PacketHeader* packetEnterView = GetSyncPacket_EnterView();
				for( int cx=-1; cx<=1; ++cx )
				{
					for( int cy=-1; cy<=1; ++cy )
					{
						if( MapCell* cell_1 = map->GetCellByIndex( cell->GetCellX() + cx, cell->GetCellY() + cy ) )
							cell_1->SyncObjectEnterCell( this, packetEnterView, (IsPlayer()?GetObjId():0) );
					}
				}
			}
			/// 2个格子的情况
			else if( ::abs(cell_d_x) == 2 || ::abs(cell_d_y) == 2 )
			{
			}
			else
			{
				assert( false );
			}
			m_Cell->RemoveMapObject( this );
			if( !cell->AddMapObject( this ) )
			{
				assert( false );
			}
		}
		/// 改变了坐标
		m_Pos.x = x;
		m_Pos.y = y;
		m_Pos.z = z;
		return true;
	}
	bool MapObject::StartMoveTo( float x, float y, float z )
	{
		/// 判断目标点是否可走......
		if( GetMap() == 0 )
			return false;
		if( x < 0 || x >= GetMap()->GetMapWidth() )
			return false;
		if( y < 0 || y >= GetMap()->GetMapHeight() )
			return false;
		m_TargetPos.x = x;
		m_TargetPos.y = y;
		m_TargetPos.z = z;
		/// 还要计算z值......
		float dist = ::sqrt( (float)(m_TargetPos.x-m_Pos.x)*(m_TargetPos.x-m_Pos.x) + (float)(m_TargetPos.y-m_Pos.y)*(m_TargetPos.y-m_Pos.y) );
		if( m_StepLength == 0 )
			return false;
		m_StepCount = ::ceil( dist/m_StepLength );
		if( m_StepCount == 0 )
			m_StepCount = 1;
		m_StepX = (m_TargetPos.x-m_Pos.x)/m_StepCount;
		m_StepY = (m_TargetPos.y-m_Pos.y)/m_StepCount;
		m_StepIndex = 0;
		m_MoveStartTime = ::timeGetTime();
		m_LastMoveTime = ::timeGetTime();
		m_IsMoving = true;

		MapPacket_SyncObjectMove packetMove;
		packetMove.m_ObjId = GetObjId();
		packetMove.m_CurrentPos = m_Pos;
		packetMove.m_TargetPos = m_TargetPos;
		SendCellPacket( &packetMove, (IsPlayer()?GetObjId():0) );
		return true;
	}
	void MapObject::SendCellPacket( PacketHeader* packet, uint32 ignoreObjId )
	{
		if( GetMap() == 0 || GetCell() == 0 )
			return;
		/*
			-------------
			| 1 | 2 | 3 |
			-------------
			| 4 |Obj| 6 |
			-------------
			| 7 | 8 | 9 |
			-------------
		*/
		if( MapCell* cell1 = GetMap()->GetCellByIndex( GetCell()->GetCellX()-1, GetCell()->GetCellY()-1 ) )
			cell1->SendToCellPlayers( packet, ignoreObjId );
		if( MapCell* cell2 = GetMap()->GetCellByIndex( GetCell()->GetCellX(),   GetCell()->GetCellY()-1 ) )
			cell2->SendToCellPlayers( packet, ignoreObjId );
		if( MapCell* cell3 = GetMap()->GetCellByIndex( GetCell()->GetCellX()+1, GetCell()->GetCellY()-1 ) )
			cell3->SendToCellPlayers( packet, ignoreObjId );
		if( MapCell* cell4 = GetMap()->GetCellByIndex( GetCell()->GetCellX()-1, GetCell()->GetCellY()   ) )
			cell4->SendToCellPlayers( packet, ignoreObjId );
		if( MapCell* cell5 = GetMap()->GetCellByIndex( GetCell()->GetCellX()  , GetCell()->GetCellY()   ) )
			cell5->SendToCellPlayers( packet, ignoreObjId );
		if( MapCell* cell6 = GetMap()->GetCellByIndex( GetCell()->GetCellX()+1, GetCell()->GetCellY()   ) )
			cell6->SendToCellPlayers( packet, ignoreObjId );
		if( MapCell* cell7 = GetMap()->GetCellByIndex( GetCell()->GetCellX()-1, GetCell()->GetCellY()+1 ) )
			cell7->SendToCellPlayers( packet, ignoreObjId );
		if( MapCell* cell8 = GetMap()->GetCellByIndex( GetCell()->GetCellX(),   GetCell()->GetCellY()+1 ) )
			cell8->SendToCellPlayers( packet, ignoreObjId );
		if( MapCell* cell9 = GetMap()->GetCellByIndex( GetCell()->GetCellX()+1, GetCell()->GetCellY()+1 ) )
			cell9->SendToCellPlayers( packet, ignoreObjId );
	}
	void MapObject::Update()
	{
		uint32 currTime = ::timeGetTime();
		if( m_IsMoving )
		{
			if( currTime - m_LastMoveTime >= m_MoveSpeed )
			{
				/// 判断该点是否可走......
				Position nextPos;
				if( m_StepIndex + 1 < m_StepCount )
				{
					nextPos.x = m_Pos.x + m_StepX;
					nextPos.y = m_Pos.y + m_StepY;
					nextPos.z = m_Pos.z;
				}
				else
					nextPos = m_TargetPos;
				if( !GetMap()->CanWalk( nextPos.x, nextPos.y, nextPos.z ) )
				{
					m_LastMoveTime = currTime;
					m_StepCount = 0;
					m_StepIndex = 0;
					m_IsMoving = false;
					/// 同步当前位置
					return;
				}
				MoveTo( nextPos.x, nextPos.y, nextPos.z );
				//m_Pos = nextPos;
				if( currTime - m_LastMoveTime > m_MoveSpeed*2 )
					m_LastMoveTime = currTime;
				else
					m_LastMoveTime += m_MoveSpeed;
				if( ++m_StepIndex >= m_StepCount )
				{
					m_LastMoveTime = currTime;
					m_StepCount = 0;
					m_StepIndex = 0;
					m_IsMoving = false;
				}
			}
		}
	}
}