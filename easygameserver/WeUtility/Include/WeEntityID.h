/*************************************************************************
	created:	  2006-8-10   17:44
	filename: 	  WeEntityID.h
	reference:  
	created:	  Fish        (于国平)
	company:      
 
	description:  实体的Id
*************************************************************************/
#ifndef _WeEntityID_H_
#define _WeEntityID_H_

#include "WeConfig.h"

namespace We
{

	enum EntityType
	{
		EntityType_Unknown=0,
		EntityType_Global,
		EntityType_Scene,
		EntityType_Client,
		EntityType_Player,
		EntityType_Entity,
		EntityType_Count,
	};

	struct EntityID
	{
	private:
		unsigned int	m_Index;		//第一个字节表示类型,后3个字节表示实体在Hash表里的索引
		unsigned int	m_Serial;		//实体唯一的序列号

	public:
		EntityID() : m_Index(0), m_Serial(0) {}
		EntityID( EntityType type, unsigned int index, unsigned int serial )
		{
			m_Index = ((int)type<<24)|index;
			m_Serial = serial;
		}
		/// 拷贝
		EntityID( const EntityID& src )
		{
			this->m_Index = src.WholeIndex();
			this->m_Serial = src.Serial();
		}
		inline void Set( EntityType type, unsigned int index, unsigned int serial )
		{
			m_Index = ((int)type<<24)|index;
			m_Serial = serial;
		}
		bool operator == ( const EntityID& b ) const
		{
			return ( m_Index == b.m_Index && m_Serial == b.m_Serial );
		}
		bool operator != ( const EntityID& b ) const
		{
			return !(b == *this);
		}
		bool operator < ( const EntityID& b ) const
		{
			if( m_Index < b.WholeIndex() )
				return true;
			else if( m_Index == b.WholeIndex() )
			{
				if( m_Serial < b.Serial() )
					return true;
			}
			return false;
		}
		inline EntityType Type() const
		{
			return (EntityType)(m_Index>>24);
		}
		inline unsigned int Index() const
		{
			return (m_Index&0x00ffffff);
		}
		inline unsigned int Serial() const
		{
			return m_Serial;
		}
		inline unsigned int WholeIndex() const
		{
			return m_Index;
		}
	};

	const EntityID Null_EntityID = EntityID();

}

// 用于hash_map
template<> inline
size_t stdext::hash_value< We::EntityID >(const We::EntityID& s)
{
	return (size_t)(s.Serial());
}

#endif
