#include "WeItem.h"
#include "WeMapPacket.h"

namespace We
{
	Item::Item()
	{
		m_ItemId = 0;
	}
	Item::~Item()
	{
	}
	PacketHeader* Item::GetSyncPacket_EnterView()
	{
		static MapPacket_Sync_Item packetSyncItem;
		packetSyncItem.m_ObjId = GetObjId();
		packetSyncItem.m_ItemId = m_ItemId;
		packetSyncItem.m_CurrentPos = m_Pos;
		return &packetSyncItem;
	}
	PacketHeader* Item::GetSyncPacket_LeaveView()
	{
		return 0;
	}
}