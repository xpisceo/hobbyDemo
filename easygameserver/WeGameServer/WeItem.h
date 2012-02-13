/********************************************************************
	created:	2009-8-2
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��ͼ�ϵ���Ʒ
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeMapObject.h"

namespace We
{
	class Map;
	class MapCell;

	class Item : public MapObject
	{
	protected:
		uint16 m_ItemId;

	public:
		Item();
		virtual ~Item();

		virtual bool IsItem() const { return true; }

		/// ���������Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_EnterView();
		/// �뿪�����Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_LeaveView();
	};
}