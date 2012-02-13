#pragma once

#include "WePacketHeader.h"
#include <string.h>

namespace We
{
#pragma pack(1)

#define PACKET_BEG( packetName ) \
	struct Packet_##packetName## : We::PacketHeader\
	{\
	Packet_##packetName##() { m_MainType = PacketType_##packetName##; m_Length=sizeof( Packet_##packetName## ); }

#define PACKET_END };

	enum PacketType
	{
		PacketType_PING = 1,
		PacketType_Move,
		PacketType_Chat,
	};

	//struct Packet_Test : We::PacketHeader
	//{
	//	Packet_Test() { m_Type = PacketType_PING; m_Length=sizeof(Packet_PING); }
	//	unsigned int m_Tick;
	//};

	PACKET_BEG( PING )
		unsigned int m_Tick;
	PACKET_END

	PACKET_BEG( Move )
		float m_x;
		float m_y;
		float m_z;
	PACKET_END

	PACKET_BEG( Chat )
		char m_Content[8192];
		void SetString( const char* str )
		{
			strcpy_s( m_Content, sizeof(m_Content), str );
			m_Length = (unsigned short)(sizeof( Packet_Chat) - sizeof(m_Content) + ::strlen(m_Content) + 1);
		}
	PACKET_END

#pragma pack()
}