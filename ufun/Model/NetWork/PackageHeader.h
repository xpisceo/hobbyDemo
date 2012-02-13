//
//  PackageHeader.h
//  ufun
//
//  Created by  on 12-2-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef ufun_PackageHeader_h
#define ufun_PackageHeader_h

typedef uint16_t PACKET_LENGTH;

const uint16_t MAX_PACKET_SIZE = 65535;	// 64K - 1 (1024*64 - 1)	

struct PacketHeader
{
    PACKET_LENGTH m_Length;	/// 
    uint8_t m_MainType;		/// (0-254)
    uint8_t m_SubType;		/// (0-254)
};

// Ping«Î«Û
struct PacketPing : PacketHeader
{
    PacketPing() { m_MainType=m_SubType=255; m_Length=sizeof(PacketPing); }
    uint32_t m_Tick;
};

// Ping∑µªÿ
struct PacketPong : PacketHeader
{
    PacketPong() { m_MainType=m_SubType=254; m_Length=sizeof(PacketPong); }
    uint32_t m_Tick;
};

#endif
