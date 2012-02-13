#include "WePlayer.h"
#include "WoSocketHandler.h"
#include "WeSocket.h"
#include "WeWorld.h"
#include "WeMap.h"
#include "WeMapPacket.h"

#include "WeGame2ClientPackets_Inner.h"
#include "WeClient2GamePackets_Inner.h"

namespace We
{
	Player::Player()
	{
		m_AccountId = 0;
		m_RoleId = 0;
		m_SocketHandler = 0;
		::memset( &m_Data, 0, sizeof(m_Data) );
		m_DataChecker = 0;
	}
	Player::~Player()
	{
	}
	bool Player::SendPacket( const PacketHeader* packet )
	{
		if( m_SocketHandler == 0 || packet == 0 )
			return false;
		return m_SocketHandler->SendPacket( packet );
	}
	void Player::OnFirstTimeEnterWorld()
	{
		/// 放入新手地图的出生点
		Map* map = World::getSingleton().GetMapById( 1 );
		if( map != 0 )
		{
			if( MoveTo( 1, 400, 300, 0 ) )
			{
			}
		}
	}
	void Player::OnEnterWorld()
	{
		/// 第一次进游戏
		if( m_Data.m_OnlineTime == 0 )
		{
			OnFirstTimeEnterWorld();
			m_Data.m_OnlineTime = 1;
		}
		else
		{
			MoveTo( m_Data.m_MapId, m_Data.m_Pos.x, m_Data.m_Pos.y, m_Data.m_Pos.z );
		}
	}
	void Player::OnReconnect()
	{
	}
	PacketHeader* Player::GetSyncPacket_EnterView()
	{
		static MapPacket_Sync_Player packetSyncPlayer;
		packetSyncPlayer.m_PlayerId = m_RoleId;
		::strcpy_s( packetSyncPlayer.m_RoleName, sizeof(packetSyncPlayer.m_RoleName), m_RoleName.c_str() );
		packetSyncPlayer.m_Sex = m_Data.m_Sex;
		packetSyncPlayer.m_Visual = m_Data.m_Visual;
		packetSyncPlayer.m_Pos = m_Data.m_Pos;
		packetSyncPlayer.m_TargetPos = m_TargetPos;
		packetSyncPlayer.m_Race = m_Data.m_Race;
		return &packetSyncPlayer;
	}
	PacketHeader* Player::GetSyncPacket_LeaveView()
	{
		return 0;
	}
	PacketHeader* Player::GetPacket_EnterWorld()
	{
		static Packet_GS2C_Inner_EnterWorld packetEnterWorld;
		m_Data.m_Pos = m_Pos;
		SRoleInfo_Client* pRoleInfo = (SRoleInfo_Client*)packetEnterWorld.m_PlayerData;
		pRoleInfo->m_RoleId = m_RoleId;
		pRoleInfo->m_Pos = m_Data.m_Pos;
		pRoleInfo->m_MapId = m_Data.m_MapId;
		pRoleInfo->m_Visual = m_Data.m_Visual;
		pRoleInfo->m_Bag = m_Data.m_Bag;
		packetEnterWorld.m_Length = sizeof(Packet_GS2C_Inner_EnterWorld)-MAX_ROLE_DATA_SIZE+sizeof(SRoleInfo_Client);
		return &packetEnterWorld;
	}
	void Player::OnRecv_Packet( PacketHeader* packet )
	{
		switch( packet->m_MainType )
		{
			/// 内部消息
		case 0:
			{
				switch( packet->m_SubType )
				{
					/// 移动
				case PT_C2GS_Inner_Move:
					{
						Packet_C2GS_Move* packetMove = (Packet_C2GS_Move*)packet;
						if( packetMove->m_Length != sizeof( Packet_C2GS_Move ) )
							return;
#pragma warning( disable : 4244 )
						/// 一步走的太远了,同步一下
						if( ::abs(GetPosX()-packetMove->x) > 1200/2
							|| ::abs(GetPosY()-packetMove->y) > 800/2 )
						{
							MapPacket_SyncObjectMove packetMove;
							packetMove.m_ObjId = GetObjId();
							packetMove.m_CurrentPos = m_Pos;
							packetMove.m_TargetPos = m_Pos;
							SendCellPacket( &packetMove, 0 );
							return;
						}
						StartMoveTo( packetMove->x, packetMove->y, packetMove->z );
					}
					break;

				//case PT_C2GS_Inner_JoinMap:
				//	{
				//		Packet_C2GS_JoinMap* packetJoinMap = (Packet_C2GS_JoinMap*)packet;
				//		if( packetJoinMap->m_Length != sizeof( Packet_C2GS_JoinMap ) )
				//			return;
				//		/// 是否在这里检测一下玩家切换房间的时间间隔???
				//		EntryMap( packetJoinMap->m_MapId );
				//	}
				//	break;

				case PT_C2GS_Inner_GetObjData:
					{
						Packet_C2GS_GetObjData* packetGetObjData = (Packet_C2GS_GetObjData*)packet;
						/// 目前只对玩家这么处理,所以直接查找玩家
						Player* targetPlayer = World::getSingleton().GetPlayerByRoleId( packetGetObjData->m_ObjId );
						if( targetPlayer == 0 )
							return;
						SendPacket( targetPlayer->GetSyncPacket_EnterView() );
					}
					break;

				default:
					break;
				}
			}
			break;

		default:
			break;
		}
	}
}