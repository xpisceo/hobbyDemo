/********************************************************************
	created:	2009-7-19
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��Ϸ����������DB���������ڲ���Ϣ
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"
#include "WeClient2GamePackets_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_GameServer2DB_Inner
	{
		PT_GS2D_Inner_GameServerInfo = 1,			/// ��Ϸ��������������Ϣ
		PT_GS2D_Inner_RoleList,						/// �����ɫ�б�����
		PT_GS2D_Inner_CreateRole,					/// ������ɫ
		PT_GS2D_Inner_ChooseRole,					/// ѡ���ɫ
		PT_GS2D_Inner_DeleteRole,					/// ɾ����ɫ
		PT_GS2D_Inner_SaveRoleData,					/// �����ɫ����

		PT_GS2D_Inner_MAX,
	};

#define PACKET_GS2D_INNER_BEG( packetName ) \
	struct Packet_GS2D_Inner_##packetName## : We::PacketHeader\
	{\
	Packet_GS2D_Inner_##packetName##() { m_MainType=0; m_SubType = PT_GS2D_Inner_##packetName##; m_Length=sizeof( Packet_GS2D_Inner_##packetName## ); }

#define PACKET_GS2D_INNER_END };

	PACKET_GS2D_INNER_BEG( GameServerInfo )
		uint16	m_GameServerId;			/// ��Ϸ��������id
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( RoleList )
		uint32 m_AccountId;
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( CreateRole )
		uint32  m_AccountId;							/// size=4	
		uint8	m_RoleIndex;							/// size=1
		char	m_RoleName[MAX_ROLE_NAME+1];			/// size=15
		uint8   m_CreateRoleData[MAX_CREATEROLE_SIZE];	/// ��ϸ�Ľ�ɫ����,���߼�����������
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( ChooseRole )
		uint32  m_AccountId;
		uint32 m_RoleId;
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( DeleteRole )
		uint32	m_RoleId;
	PACKET_GS2D_INNER_END

	PACKET_GS2D_INNER_BEG( SaveRoleData )
		uint32	m_RoleId;
		uint8   m_SaveRoleData[MAX_ROLE_DATA_SIZE];		/// Ҫ����Ľ�ɫ����,���߼�����������
	PACKET_GS2D_INNER_END

#pragma pack()
}
