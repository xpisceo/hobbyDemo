/********************************************************************
	created:	2009-8-2
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��ͼ��(�Ź���)
				-------------
				| 1 | 2 | 3 |
				-------------
				| 4 | 5 | 6 |
				-------------
				| 7 | 8 | 9 |
				-------------
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WePacketHeader.h"
#include "WeMapObject.h"

/// ����Ĵ�С
#define CELL_X_SIZE 360
#define CELL_Y_SIZE 260

namespace We
{
	class Map;
	class Player;
	class Npc;
	class Monster;
	class Item;

	class MapCell
	{
		friend class Map;
	protected:
		MapObjectMgr<Player>	m_Players;		/// �õ�ͼ��������
		MapObjectMgr<Npc>		m_Npcs;			/// �õ�ͼ�����NPC
		MapObjectMgr<Monster>	m_Monsters;		/// �õ�ͼ����Ĺ���
		MapObjectMgr<Item>		m_Items;		/// �õ�ͼ�������Ʒ

		uint16 m_CellX;		/// X���ϵ���������
		uint16 m_CellY;		/// Y���ϵ���������
	public:
		MapCell();
		~MapCell();

		bool AddMapObject( MapObject* obj );
		void RemoveMapObject( MapObject* obj );

		inline uint16 GetCellX() const { return m_CellX; }
		inline uint16 GetCellY() const { return m_CellY; }

		/// �����ݰ�����������������
		void SendToCellPlayers( PacketHeader* packet, uint32 ignoreObjId );

		/// һ������������ʱ,��ͬ������
		void SyncObjectEnterCell( MapObject* enterObj, PacketHeader* syncPacketEnterView, uint32 ignoreObjId );

		/// һ�������뿪����ʱ,��ͬ������
		/// �����Ϣ���Կ��ǲ���
		void SyncObjectLeaveCell( MapObject* leaveObj, PacketHeader* syncPacketLeaveView, uint32 ignoreObjId );

	};
}