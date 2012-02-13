/********************************************************************
	created:	2009-8-12
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�ͻ��˵���Ϣ������Ӧ�ӿ�
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