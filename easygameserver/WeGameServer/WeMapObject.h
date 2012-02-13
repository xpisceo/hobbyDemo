/********************************************************************
	created:	2009-8-2
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��ͼ�ϵĶ���(���,NPC,����,��ͼ�ϵ���Ʒ)
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include <map>
#include <string>
#include <assert.h>
#include "WePacketDefine_Inner.h"
#include "WePacketHeader.h"

namespace We
{
	class Map;
	class MapCell;

	enum ObjectType
	{
		ObjectType_Player = 0,
		ObjectType_Npc = 1,
		ObjectType_Monster = 2,
		ObjectType_Item = 3,
	};

	class MapObject
	{
		friend class World;
		friend class Map;
		friend class MapCell;
	protected:
		Map*		m_Map;		/// ���ڵ�ͼ
		MapCell*	m_Cell;		/// ��������
		Position	m_Pos;		/// ��ǰ����
		Position	m_TargetPos;/// Ŀ������
		uint32		m_MoveStartTime;	/// ��ʼ�ƶ���ʱ��
		uint32		m_LastMoveTime;		/// �ϴ��ƶ���ʱ��
		uint32		m_MoveSpeed;		/// �ƶ��ٶ�(����ƶ�һ��)
		float		m_StepLength;		/// ����
		float		m_StepX;			/// ����X
		float		m_StepY;			/// ����Y
		int			m_StepCount;		/// �ܹ��߼���
		int			m_StepIndex;		/// ��ǰ�ߵ��ڼ���	
		bool		m_IsMoving;

		uint32	    m_ObjId;	/// ����id

		/// �����������к�
		static uint32 m_NpcSerial;
		static uint32 m_MonsterSerial;
		static uint32 m_ItemSerial;

	public:
		MapObject();
		virtual ~MapObject();

		virtual bool IsPlayer() const { return false; }
		virtual bool IsNpc() const { return false; }
		virtual bool IsMonster() const { return false; }
		virtual bool IsItem() const { return false; }

		inline uint32 GetObjId() const { return m_ObjId; }

		/// ���ObjId = (1<<28) = 268435456
		static inline uint32 NewNpcId() { return ((++m_NpcSerial) | 0x80000000); }			/// (ObjId>>29) == 4
		static inline uint32 NewMonsterId() { return ((++m_MonsterSerial) | 0x40000000); } /// (ObjId>>29) == 2
		static inline uint32 NewItemId() { return ((++m_ItemSerial) | 0x20000000); }		/// (ObjId>>29) == 1

		inline Map* GetMap() { return m_Map; }
		inline MapCell* GetCell() { return m_Cell; }

		inline const Position& GetPos() const { return m_Pos; }
		inline Position& GetPos() { return m_Pos; }
		inline float GetPosX() const { return m_Pos.x; }
		inline float GetPosY() const { return m_Pos.y; }
		inline float GetPosZ() const { return m_Pos.z; }
		inline void SetPosX( float x ) { m_Pos.x = x; }
		inline void SetPosY( float y ) { m_Pos.x = y; }
		inline void SetPosZ( float z ) { m_Pos.x = z; }
		inline void SetPos( float x, float y, float z ) { m_Pos.x = x; m_Pos.x = y; m_Pos.x = z; }

		/// ���������Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_EnterView() = 0;
		/// �뿪�����Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_LeaveView() = 0;

		/// �ƶ���ĳ������
		bool MoveTo( uint16 mapId, float x, float y, float z );
		bool MoveTo( float x, float y, float z );
		
		/// ��ʼ�ӵ�ǰ���ƶ���ĳ������
		bool StartMoveTo( float x, float y, float z );

		/// �����ݰ����Ź�����ĵ��������
		void SendCellPacket( PacketHeader* packet, uint32 ignoreObjId );

		virtual void Update();
	};

	template<class T>
	class MapObjectMgr
	{
	public:
		std::map<uint32,T*> m_MapObjects;
	public:
		void AddObject( T* obj )
		{
			std::map<uint32,T*>::iterator i = m_MapObjects.find( obj->GetObjId() );
			if( i == m_MapObjects.end() )
				m_MapObjects[obj->GetObjId()] = obj;
			else
			{
				assert( i->second == obj );
				//i->second = obj;
			}
		}
		void RemoveObject( uint32 objId )
		{
			std::map<uint32,T*>::iterator i = m_MapObjects.find( objId );
			if( i != m_MapObjects.end() )
			{
				m_MapObjects.erase( i );
			}
		}

		T* GetObject( uint32 objId ) const
		{
			std::map<uint32,T*>::iterator i = m_MapObjects.find( objId );
			if( i != m_MapObjects.end() )
			{
				return i->second;
			}
			return 0;
		}

		void Update()
		{
			std::map<uint32,T*>::iterator i = m_MapObjects.begin();
			for( i; i != m_MapObjects.end(); ++i )
			{
				i->second->Update();
			}
		}
	};
}