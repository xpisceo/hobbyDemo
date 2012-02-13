/********************************************************************
	created:	2009-7-21
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	玩家
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WePlayerStruct.h"
#include "WePacketHeader.h"
#include "WeMapObject.h"

namespace We
{
	class SocketHandler;
	class Map;
	class Cell;

	/// 玩家的不保存数据库的数据
	struct SPlayerData : SRoleInfo_DB
	{
	};

	class Player : public MapObject
	{
		friend class World;
		friend class GameServer;
	protected:
		uint32  m_AccountId;
		uint32	m_RoleId;
		string	m_RoleName;
		SPlayerData m_Data;

		SocketHandler* m_SocketHandler;

	public:
		uint16	m_DataChecker;	/// 数据变化标识,目前只用在玩家身上
	public:
		Player();
		virtual ~Player();

		virtual bool IsPlayer() const { return true; }

		uint32 GetAccountId() const { return m_AccountId; }
		uint32 GetRoleId() const { return m_RoleId; }
		const string& GetRoleName() const { return m_RoleName; }

		bool SendPacket( const PacketHeader* packet );

		/// 进入游戏世界
		void OnFirstTimeEnterWorld();
		void OnEnterWorld();

		/// 断线重连
		void OnReconnect();

		/// 进入玩家视野时,需要给玩家同步的数据
		virtual PacketHeader* GetSyncPacket_EnterView();
		/// 离开玩家视野时,需要给玩家同步的数据
		virtual PacketHeader* GetSyncPacket_LeaveView();

		virtual PacketHeader* GetPacket_EnterWorld();

		void OnRecv_Packet( PacketHeader* packet );
	};
}