#include "WeMonster.h"
#include "WeMapPacket.h"
#include "WeMap.h"
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>

namespace We
{
	Monster::Monster()
	{
		m_MonsterId = 0;
		::memset( &m_BornPos, 0, sizeof(m_BornPos) );
		m_MoveRange = 600; /// Լ�����Ļ
	}
	Monster::~Monster()
	{
	}
	PacketHeader* Monster::GetSyncPacket_EnterView()
	{
		static MapPacket_Sync_Monster packetSyncMonster;
		packetSyncMonster.m_ObjId = GetObjId();
		packetSyncMonster.m_MonsterId = m_MonsterId;
		packetSyncMonster.m_CurrentPos = m_Pos;
		packetSyncMonster.m_TargetPos = m_TargetPos;
		return &packetSyncMonster;
	}
	PacketHeader* Monster::GetSyncPacket_LeaveView()
	{
		return 0;
	}
	void Monster::Update()
	{
		MapObject::Update();
		if( !m_IsMoving )
		{
			/// 5�����ƶ�һ��
			if( ::timeGetTime() - m_LastMoveTime > 5000 )
			{
				float x = GetPosX() + ::rand()%300 - 150;
				float y = GetPosY() + ::rand()%300 - 150;
				if( x < 0 || x >= GetMap()->GetMapWidth() )
					return;
				if( y < 0 || y >= GetMap()->GetMapHeight() )
					return;
				if( ::abs(x - m_BornPos.x) > m_MoveRange )
					return;
				if( ::abs(y - m_BornPos.y) > m_MoveRange )
					return;
				/// ����߶�
				StartMoveTo( x, y, 0 );
			}
		}
	}
}