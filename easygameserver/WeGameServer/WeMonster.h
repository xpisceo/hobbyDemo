/********************************************************************
	created:	2009-8-2
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	怪物
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeMapObject.h"

namespace We
{
	class Map;
	class MapCell;

	class Monster : public MapObject
	{
		friend class Map;
	protected:
		uint16		m_MonsterId;	/// 怪物id
		Position	m_BornPos;		/// 出生点
		float		m_MoveRange;	/// 活动半径

	public:
		Monster();
		virtual ~Monster();

		virtual bool IsMonster() const { return true; }

		/// 进入玩家视野时,需要给玩家同步的数据
		virtual PacketHeader* GetSyncPacket_EnterView();
		/// 离开玩家视野时,需要给玩家同步的数据
		virtual PacketHeader* GetSyncPacket_LeaveView();

		virtual void Update();
	};
}