/********************************************************************
	created:	2009-8-2
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	NPC
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeMapObject.h"

namespace We
{
	class Map;
	class MapCell;

	class Npc : public MapObject
	{
	protected:
		uint16 m_NpcId;

	public:
		Npc();
		virtual ~Npc();

		virtual bool IsNpc() const { return true; }

		/// 进入玩家视野时,需要给玩家同步的数据
		virtual PacketHeader* GetSyncPacket_EnterView();
		/// 离开玩家视野时,需要给玩家同步的数据
		virtual PacketHeader* GetSyncPacket_LeaveView();
	};
}