#include "WePacketHandler.h"
#include <assert.h>

namespace We
{
	PacketHandlerMgr::PacketHandlerMgr()
	{
	}
	PacketHandlerMgr::~PacketHandlerMgr()
	{
		m_PacketHandlers.clear();
	}
	void PacketHandlerMgr::AddPacketHandler( uint8 mainType, uint8 subType, PacketHandler handler )
	{
		uint16 key = ((mainType<<8)|subType);
		map<uint16, PacketHandler>::iterator i = m_PacketHandlers.find( key );
		if( i != m_PacketHandlers.end() )
		{
			assert( false );
			return;
		}
		m_PacketHandlers[key] = handler;
	}
	void PacketHandlerMgr::RemovePacketHandler( uint8 mainType, uint8 subType )
	{
		uint16 key = ((mainType<<8)|subType);
		map<uint16, PacketHandler>::iterator i = m_PacketHandlers.find( key );
		if( i != m_PacketHandlers.end() )
			m_PacketHandlers.erase( i );
	}
	PacketHandler PacketHandlerMgr::GetPacketHandler( uint8 mainType, uint8 subType )
	{
		uint16 key = ((mainType<<8)|subType);
		map<uint16, PacketHandler>::iterator i = m_PacketHandlers.find( key );
		if( i != m_PacketHandlers.end() )
			return i->second;
		return 0;
	}
}