/********************************************************************
	created:	2009-8-12
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	客户端的消息包的响应接口
*********************************************************************/
#pragma once

#include "WePacketHeader.h"

#include <map>
using namespace std;

namespace We
{
	class Player;

	typedef void (*PacketHandler)( Player* player, PacketHeader* packet );

	class PacketHandlerMgr
	{
	protected:
		std::map<uint16, PacketHandler> m_PacketHandlers;
	public:
		PacketHandlerMgr();
		virtual ~PacketHandlerMgr();

		void AddPacketHandler( uint8 mainType, uint8 subType, PacketHandler handler );
		void RemovePacketHandler( uint8 mainType, uint8 subType );

		PacketHandler GetPacketHandler( uint8 mainType, uint8 subType );
	};
}