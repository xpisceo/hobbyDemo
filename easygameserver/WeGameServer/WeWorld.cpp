#include "WeWorld.h"
#include "WeMap.h"
#include "WePlayer.h"
#include "WeGame2ClientPackets_Inner.h"
#include "WeType.h"

namespace We
{
	Singleton_Implement( World );

	World::World()
	{
	}
	World::~World()
	{
	}
	Player* World::GetPlayerByAccountId( uint32 accountId )
	{
		map<uint32,Player*>::iterator i = m_PlayersByAccount.find( accountId );
		if( i == m_PlayersByAccount.end() )
			return 0;
		return i->second;
	}
	Player* World::GetPlayerByRoleId( uint32 roleId )
	{
		map<uint32,Player*>::iterator i = m_PlayersByRoleId.find( roleId );
		if( i == m_PlayersByRoleId.end() )
			return 0;
		return i->second;
	}
	Player* World::GetPlayerByName( const string& roleName )
	{
		map<string,Player*>::iterator i = m_PlayersByName.find( roleName );
		if( i == m_PlayersByName.end() )
			return 0;
		return i->second;
	}
	void World::RemovePlayer( Player* player )
	{
		map<uint32,Player*>::iterator i = m_PlayersByAccount.find( player->GetAccountId() );
		if( i != m_PlayersByAccount.end() )
			m_PlayersByAccount.erase( i );
		map<uint32,Player*>::iterator i2 = m_PlayersByRoleId.find( player->GetRoleId() );
		if( i2 != m_PlayersByRoleId.end() )
			m_PlayersByRoleId.erase( i2 );
		map<string,Player*>::iterator i3 = m_PlayersByName.find( player->GetRoleName() );
		if( i3 != m_PlayersByName.end() )
			m_PlayersByName.erase( i3 );
		/// ....
		if( player->GetMap() != 0 )
			player->GetMap()->RemoveMapObject( player );
		/// 检查是否已经在里面了
		list<MapObject*>::iterator i4 = m_DeleteObjects.begin();
		for( i4; i4!=m_DeleteObjects.end(); ++i4 )
		{
			if( *i4 == player )
				return;
		}
		m_DeleteObjects.push_back( player );
	}
	Map* World::GetMapById( uint16 mapId )
	{
		map<uint16, Map*>::iterator i = m_Maps.find( mapId );
		if( i == m_Maps.end() )
			return 0;
		return i->second;
	}
	bool World::LoadMaps()
	{
		/// 测试用
		for( int i=0; i<5; ++i )
		{
			Map* gameMap = new Map();
			gameMap->m_MapId = i+1;
			gameMap->m_MapName = std::string( "房间-" ) + Type::ToString( gameMap->m_MapId );
			m_Maps[gameMap->m_MapId] = gameMap;
			gameMap->LoadMap();
		}
		return false;
	}
	void World::OnPlayerDisconnect( uint32 roleId )
	{
		Player* player = GetPlayerByRoleId( roleId );
		if( player == 0 )
			return;
		player->m_SocketHandler = 0;
		RemovePlayer( player );
	}
	bool World::OnPlayerEnterWorld( uint32 accountId, uint8* roleData, SocketHandler* socketHandler )
	{
		SRoleInfo_DB* roleInfo = (SRoleInfo_DB*)roleData;
		Player* player = new Player();
		player->m_SocketHandler = socketHandler;
		player->m_AccountId = accountId;
		player->m_RoleId = roleInfo->m_RoleId;
		player->m_ObjId = player->m_RoleId;
		player->m_RoleName = roleInfo->m_RoleName;
		*((SRoleInfo_DB*)(&(player->m_Data))) = *roleInfo;
		/// 检查......
		m_PlayersByAccount[accountId] = player;
		m_PlayersByRoleId[player->m_RoleId] = player;
		m_PlayersByName[player->GetRoleName()] = player;

		player->SendPacket( player->GetPacket_EnterWorld() );

		player->OnEnterWorld();

		/// Send MapList
		return true;
	}
	void World::Update()
	{
		map<uint16, Map*>::iterator i = m_Maps.begin();
		for( i; i!=m_Maps.end(); ++i )
			i->second->Update();
		if( !m_DeleteObjects.empty() )
		{
			list<MapObject*>::iterator i = m_DeleteObjects.begin();
			for( i; i!=m_DeleteObjects.end(); ++i )
			{
				delete *i;
			}
			m_DeleteObjects.clear();
		}
	}
}