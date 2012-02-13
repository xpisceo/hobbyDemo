/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	消息头
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	typedef uint16 PACKET_LENGTH;

	const uint16 MAX_PACKET_SIZE = 65535;	// 64K - 1 (1024*64 - 1)	

	struct PacketHeader
	{
		PACKET_LENGTH m_Length;	/// 消息长度
		uint8 m_MainType;		/// 主消息(0-254)
		uint8 m_SubType;		/// 子消息(0-254)
	};

	// Ping请求
	struct PacketPing : PacketHeader
	{
		PacketPing() { m_MainType=m_SubType=255; m_Length=sizeof(PacketPing); }
		uint m_Tick;
	};

	// Ping返回
	struct PacketPong : PacketHeader
	{
		PacketPong() { m_MainType=m_SubType=254; m_Length=sizeof(PacketPong); }
		uint m_Tick;
	};
}
