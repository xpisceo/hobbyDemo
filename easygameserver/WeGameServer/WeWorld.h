/********************************************************************
	created:	2009-7-21
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	游戏世界
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeSingleton.h"
#include "WeMapObject.h"
#include "WePacketHandler.h"

#include <map>
#include <list>
using namespace std;

namespace We
{
	class Map;
	class Player;
	class SocketHandler;

	class World : public Singleton<World>, public PacketHandlerMgr
	{
	protected:
		map<uint16, Map*> m_Maps;	/// 地图

		map<uint32,Player*> m_PlayersByAccount;
		map<uint32,Player*> m_PlayersByRoleId;
		map<string,Player*> m_PlayersByName;

		std::list<MapObject*> m_DeleteObjects;

	public:
		World();
		~World();

		Player* GetPlayerByAccountId( uint32 accountId );
		Player* GetPlayerByRoleId( uint32 roleId );
		Player* GetPlayerByName( const string& roleName );
		void RemovePlayer( Player* player );

		Map* GetMapById( uint16 mapId );
		bool LoadMaps();

		void OnPlayerDisconnect( uint32 roleId );
		bool OnPlayerEnterWorld( uint32 accountId, uint8* roleData, SocketHandler* socketHandler );

		void Update();
	};
}