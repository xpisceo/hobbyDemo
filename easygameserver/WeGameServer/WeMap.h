/********************************************************************
	created:	2009-8-1
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��ͼ
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeMapObject.h"

namespace We
{
	class MapCell;
	class Player;
	class Npc;
	class Monster;
	class Item;

	class Map
	{
		friend class World;
	protected:
		uint16		m_MapId;					/// ��ͼid
		std::string m_MapName;					/// ��ͼ����

		uint32		m_MapWidth;					/// ��ͼX�᳤��
		uint32		m_MapHeight;				/// ��ͼY�᳤��
		uint16		m_CellColumns;				/// X���ϵ�������Ŀ
		uint16		m_CellRows;					/// Y���ϵ�������Ŀ
		MapCell**	m_Cells;					/// 2ά������

		MapObjectMgr<Player>	m_Players;		/// �õ�ͼ������
		MapObjectMgr<Npc>		m_Npcs;			/// �õ�ͼ���NPC
		MapObjectMgr<Monster>	m_Monsters;		/// �õ�ͼ��Ĺ���
		MapObjectMgr<Item>		m_Items;		/// �õ�ͼ�����Ʒ

	public:
		Map();
		~Map();

		uint16 GetMapId() const { return m_MapId; }
		const std::string& GetMapName() const { return m_MapName; }

		uint32 GetMapWidth() const { return m_MapWidth; }
		uint32 GetMapHeight() const { return m_MapHeight; }

		/// ���ص�ͼ����
		bool LoadMap();

		/// �Ѷ���ŵ���ͼ�ϵ�ĳ������
		bool AddMapObject( MapObject* obj, float x, float y, float z );

		/// �ӵ�ͼ���Ƴ�����
		void RemoveMapObject( MapObject* obj );

		/// ��ȡ�������ڵ�����
		MapCell* GetCell( int x, int y );

		///
		MapCell* GetCellByIndex( int cell_x, int cell_y );

		/// ĳ�������Ƿ��������
		bool CanWalk( float x, float y, float z );

		/// �㲥��ͼ��Ϣ(������Ϣ)
		void SendMapPacket( const PacketHeader* packet, uint32 ignoreId=0 );

		void Update();
	};
}