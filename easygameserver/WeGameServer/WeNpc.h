/********************************************************************
	created:	2009-8-2
	author:		Fish (�ڹ�ƽ)
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

		/// ���������Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_EnterView();
		/// �뿪�����Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_LeaveView();
	};
}