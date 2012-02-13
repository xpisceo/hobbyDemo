/********************************************************************
	created:	2009-7-19
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	游戏服务器发给帐号服务器的内部消息
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_GameServer2Account_Inner
	{
		PT_GS2A_Inner_GameServerInfo = 1,	/// 中心服务器的配置信息
		PT_GS2A_Inner_CreateAccount,		/// 创建帐号
		PT_GS2A_Inner_Login,				/// 登录
		PT_GS2A_Inner_MAX,
	};

#define PACKET_GS2A_INNER_BEG( packetName ) \
	struct Packet_GS2A_Inner_##packetName## : We::PacketHeader\
	{\
	Packet_GS2A_Inner_##packetName##() { m_MainType=0; m_SubType = PT_GS2A_Inner_##packetName##; m_Length=sizeof( Packet_GS2A_Inner_##packetName## ); }

#define PACKET_GS2A_INNER_END };

	PACKET_GS2A_INNER_BEG( GameServerInfo )
		uint16	m_GameServerId;			/// 游戏服务器的id
	PACKET_GS2A_INNER_END

	PACKET_GS2A_INNER_BEG( CreateAccount )
		uint32  m_ClientId;
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		char	m_EncryptPwd[MAX_ENCRYPT_PWD+1];
		/// 密保卡......
	PACKET_GS2A_INNER_END

	PACKET_GS2A_INNER_BEG( Login )
		uint32  m_ClientId;
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		char	m_EncryptPwd[MAX_ENCRYPT_PWD+1];
	PACKET_GS2A_INNER_END

#pragma pack()
}
