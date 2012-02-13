/********************************************************************
	created:	2009-8-2
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	地图上的对象(玩家,NPC,怪物,地图上的物品)
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
		Map*		m_Map;		/// 所在地图
		MapCell*	m_Cell;		/// 所在区块
		Position	m_Pos;		/// 当前坐标
		Position	m_TargetPos;/// 目标坐标
		uint32		m_MoveStartTime;	/// 开始移动的时刻
		uint32		m_LastMoveTime;		/// 上次移动的时刻
		uint32		m_MoveSpeed;		/// 移动速度(多久移动一步)
		float		m_StepLength;		/// 步长
		float		m_StepX;			/// 步长X
		float		m_StepY;			/// 步长Y
		int			m_StepCount;		/// 总共走几步
		int			m_StepIndex;		/// 当前走到第几步	
		bool		m_IsMoving;

		uint32	    m_ObjId;	/// 对象id

		/// 对象生成序列号
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

		/// 最大ObjId = (1<<28) = 268435456
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

		/// 进入玩家视野时,需要给玩家同步的数据
		virtual PacketHeader* GetSyncPacket_EnterView() = 0;
		/// 离开玩家视野时,需要给玩家同步的数据
		virtual PacketHeader* GetSyncPacket_LeaveView() = 0;

		/// 移动到某个坐标
		bool MoveTo( uint16 mapId, float x, float y, float z );
		bool MoveTo( float x, float y, float z );
		
		/// 开始从当前点移动到某个坐标
		bool StartMoveTo( float x, float y, float z );

		/// 发数据包给九宫格里的的所有玩家
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