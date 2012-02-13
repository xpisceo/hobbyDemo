/********************************************************************
	created:	2009-6-30
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	帐号服务器发给游戏服务器的内部消息
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_Account2GameServer_Inner
	{
		PT_A2GS_Inner_CreateAccountResult = 1,		/// 创建帐号返回
		PT_A2GS_Inner_LoginResult,					/// 帐号登录返回

		PT_A2GS_Inner_MAX,
	};

#define PACKET_A2GS_INNER_BEG( packetName ) \
	struct Packet_A2GS_Inner_##packetName## : We::PacketHeader\
	{\
	Packet_A2GS_Inner_##packetName##() { m_MainType=0; m_SubType = PT_A2GS_Inner_##packetName##; m_Length=sizeof( Packet_A2GS_Inner_##packetName## ); }

#define PACKET_A2GS_INNER_END };

	PACKET_A2GS_INNER_BEG( CreateAccountResult )
		enum CreateAccountResult
		{
			CreateAccountResult_Unknown = 0,
			CreateAccountResult_OK,
			CreateAccountResult_AccountExist,	/// 帐号已经存在
			CreateAccountResult_SQLError,		/// 数据库错误
		};
		uint32	m_ClientId;
		uint32	m_AccountId; 
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		uint8	m_Result;
	PACKET_A2GS_INNER_END

	PACKET_A2GS_INNER_BEG( LoginResult )
		enum LoginResult
		{
			LoginResult_Unknown = 0,
			LoginResult_OK,
			LoginResult_OKAndReconnect,		/// 验证OK,等待断线重连
			LoginResult_AccountOrPwdError,	/// 帐号或密码错误
			LoginResult_MibaoError,			/// 密保卡错误
		};
		uint32	m_ClientId;
		uint32	m_AccountId; 
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		uint8	m_Result;
	PACKET_A2GS_INNER_END


#pragma pack()
}
