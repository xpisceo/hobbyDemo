#include "WeNpc.h"
#include "WeMapPacket.h"

namespace We
{
	Npc::Npc()
	{
		m_NpcId = 0;
	}
	Npc::~Npc()
	{
	}
	PacketHeader* Npc::GetSyncPacket_EnterView()
	{
		static MapPacket_Sync_Npc packetSyncNpc;
		packetSyncNpc.m_ObjId = GetObjId();
		packetSyncNpc.m_NpcId = m_NpcId;
		packetSyncNpc.m_CurrentPos = m_Pos;
		return &packetSyncNpc;
	}
	PacketHeader* Npc::GetSyncPacket_LeaveView()
	{
		return 0;
	}
}