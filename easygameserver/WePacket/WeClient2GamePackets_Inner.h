/********************************************************************
	created:	2009-7-19
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	客户端发给游戏服务器的内部消息
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_Client2GameServer_Inner
	{
		PT_C2GS_Inner_CreateAccount = 1,	/// 注册账号
		PT_C2GS_Inner_Login,				/// 登陆
		PT_C2GS_Inner_CreateRole,			/// 创建角色
		PT_C2GS_Inner_ChooseRole,			/// 选择角色
		PT_C2GS_Inner_DeleteRole,			/// 删除角色

		PT_C2GS_Inner_Move,					/// 移动

		PT_C2GS_Inner_JoinMap,				/// 进入房间
		PT_C2GS_Inner_GetObjData,			/// 请求获取某个对象的数据 

		PT_C2GS_Inner_MAX,
	};

#define PACKET_C2GS_INNER_LOGIN_BEG( packetName ) \
	struct Packet_C2GS_##packetName## : PacketHeader\
	{\
	Packet_C2GS_##packetName##() { m_MainType=0; m_SubType = PT_C2GS_Inner_##packetName##; m_Length=sizeof( Packet_C2GS_##packetName## ); }

#define PACKET_C2GS_INNER_BEG( packetName ) \
	struct Packet_C2GS_##packetName## : PacketHeader\
	{\
	Packet_C2GS_##packetName##() { m_MainType=0; m_SubType = PT_C2GS_Inner_##packetName##; m_Length=sizeof( Packet_C2GS_##packetName## ); }

#define PACKET_C2GS_INNER_END };

	PACKET_C2GS_INNER_LOGIN_BEG( CreateAccount )
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		char	m_EncryptPwd[MAX_ENCRYPT_PWD+1];
		/// 其他账号信息
	PACKET_C2GS_INNER_END

	/// 登录消息可以考虑用密保卡随机号作为key来加密
	/// 具体怎么加密才比较可靠,还要了解一下黑客是怎么破解的,才能对症下药!!!
	PACKET_C2GS_INNER_LOGIN_BEG( Login )
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		char	m_EncryptPwd[MAX_ENCRYPT_PWD+1];
		/// 密保卡......
	PACKET_C2GS_INNER_END

	/// 创建角色时所需要的基本数据
	PACKET_C2GS_INNER_LOGIN_BEG( CreateRole )
		uint8	m_RoleIndex;
		char	m_RoleName[MAX_ROLE_NAME+1];
		uint8   m_CreateRoleData[MAX_CREATEROLE_SIZE];	/// 详细的角色数据,由逻辑部分来解析
	PACKET_C2GS_INNER_END

	PACKET_C2GS_INNER_LOGIN_BEG( ChooseRole )
		uint32	m_RoleId;
	PACKET_C2GS_INNER_END

	PACKET_C2GS_INNER_LOGIN_BEG( DeleteRole )
		uint32	m_RoleId;
	PACKET_C2GS_INNER_END

	PACKET_C2GS_INNER_BEG( Move )
		float x;
		float y;
		float z;
	PACKET_C2GS_INNER_END

	PACKET_C2GS_INNER_BEG( JoinMap )
		uint16 m_MapId;		/// 房间编号
	PACKET_C2GS_INNER_END

	PACKET_C2GS_INNER_BEG( GetObjData )
		uint32 m_ObjId;
	PACKET_C2GS_INNER_END

#pragma pack()
}
