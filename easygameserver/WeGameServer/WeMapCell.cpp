#include "WeMapCell.h"
#include "WePlayer.h"
#include "WeNpc.h"
#include "WeMonster.h"
#include "WeItem.h"
#include "WeGame2ClientPackets_Inner.h"

namespace We
{
	MapCell::MapCell()
	{
		m_CellX = 0;
		m_CellY = 0;
	}
	MapCell::~MapCell()
	{
	}
	bool MapCell::AddMapObject( MapObject* obj )
	{
		if( obj == 0 )
			return false;
		obj->m_Cell = this;
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
	void MapCell::RemoveMapObject( MapObject* obj )
	{
		if( obj == 0 )
			return;
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
	}
	void MapCell::SendToCellPlayers( PacketHeader* packet, uint32 ignoreObjId )
	{
		std::map<uint32,Player*>::iterator i = m_Players.m_MapObjects.begin();
		for( i; i!=m_Players.m_MapObjects.end(); ++i )
		{
			if( i->second != 0 && i->second->GetRoleId() != ignoreObjId )
				i->second->SendPacket( packet );
		}
	}

	void MapCell::SyncObjectEnterCell( MapObject* enterObj, PacketHeader* syncPacketEnterView, uint32 ignoreObjId )
	{
		/// 告诉该格子里的玩家,enterObj进入视野了
		SendToCellPlayers( syncPacketEnterView, ignoreObjId );
		/// 告诉enterObj,该格子里的对象都进入视野了
		if( enterObj->IsPlayer() )
		{
			Player* player = (Player*)enterObj;
#define SYNC_ADD_OBJECT_TO_PLAYER( Type )\
			{\
				std::map<uint32,##Type##*>::iterator i = m_##Type##s.m_MapObjects.begin();\
				for( i; i!=m_##Type##s.m_MapObjects.end(); ++i )\
				{\
					if( i->second != 0 )\
					{\
						player->SendPacket( i->second->GetSyncPacket_EnterView() );\
					}\
				}\
			}

			SYNC_ADD_OBJECT_TO_PLAYER( Player );
			SYNC_ADD_OBJECT_TO_PLAYER( Npc );
			SYNC_ADD_OBJECT_TO_PLAYER( Monster );
			SYNC_ADD_OBJECT_TO_PLAYER( Item );
		}
	}

	/// 这个消息可以考虑不发
	void MapCell::SyncObjectLeaveCell( MapObject* leaveObj, PacketHeader* syncPacketLeaveView, uint32 ignoreObjId )
	{
		/// 告诉该格子里的玩家,leaveObj离开视野了
		SendToCellPlayers( syncPacketLeaveView, ignoreObjId );
		/// 告诉leaveObj,该格子里的对象都离开视野了
		if( leaveObj->IsPlayer() )
		{
			Player* player = (Player*)leaveObj;
#define SYNC_REMOVE_OBJECT_TO_PLAYER( Type )\
			{\
				Packet_GS2C_Inner_SyncRemove##Type##s packetRemove##Type##s;\
				packetRemove##Type##s.m_ObjCount = 0;\
				std::map<uint32,##Type##*>::iterator i = m_##Type##s.m_MapObjects.begin();\
				for( i; i!=m_##Type##s.m_MapObjects.end(); ++i )\
				{\
					if( i->second != 0 )\
					{\
						packetRemove##Type##s.m_ObjIds[packetRemove##Type##s.m_ObjCount++] = i->second->GetObjId();\
					}\
				}\
				if( packetRemove##Type##s.m_ObjCount > 0 )\
				{\
					packetRemove##Type##s.CalcSize();\
					player->SendPacket( &packetRemove##Type##s );\
				}\
			}

			SYNC_REMOVE_OBJECT_TO_PLAYER( Player );
			SYNC_REMOVE_OBJECT_TO_PLAYER( Npc );
			SYNC_REMOVE_OBJECT_TO_PLAYER( Monster );
			SYNC_REMOVE_OBJECT_TO_PLAYER( Item );
		}
	}
	
}