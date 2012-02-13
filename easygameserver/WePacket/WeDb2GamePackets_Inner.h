/********************************************************************
	created:	2009-7-21
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	DB������������Ϸ���������ڲ���Ϣ
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"
#include "WeGame2ClientPackets_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_DB2GameServer_Inner
	{
		PT_D2GS_Inner_CreateRoleResult = 1,		/// ������ɫ����
		PT_D2GS_Inner_RoleListResult,			/// ��ɫ�б���
		PT_D2GS_Inner_ChooseRoleResult,			/// ѡ���ɫ���
		PT_D2GS_Inner_DeleteRoleResult,			/// ɾ����ɫ���

		PT_D2GS_Inner_MAX,
	};

#define PACKET_D2GS_INNER_BEG( packetName ) \
	struct Packet_D2GS_Inner_##packetName## : We::PacketHeader\
	{\
	Packet_D2GS_Inner_##packetName##() { m_MainType=0; m_SubType = PT_D2GS_Inner_##packetName##; m_Length=sizeof( Packet_D2GS_Inner_##packetName## ); }

#define PACKET_D2GS_INNER_END };

	PACKET_D2GS_INNER_BEG( CreateRoleResult )
		enum CreateRoleResult
		{
			CreateRoleResult_Unknown = 0,
			CreateRoleResult_OK,
			CreateRoleResult_RoleNameExist,		/// ��ɫ���Ѿ�����
			CreateRoleResult_RoleIndexExist,	/// ��λ���Ѿ��н�ɫ��
			CreateRoleResult_SQLError,			/// ���ݿ����
		};
		uint32	m_AccountId;
		uint8	m_Result;
		uint8	m_RoleIndex;
		uint32  m_RoleId;
		char	m_RoleName[MAX_ROLE_NAME+1];
		uint8   m_CreateRoleData[MAX_CREATEROLE_SIZE];	/// ��ϸ�Ľ�ɫ����,���߼�����������
	PACKET_D2GS_INNER_END

	PACKET_D2GS_INNER_BEG( ChooseRoleResult )
		enum ChooseRoleResult
		{
			ChooseRoleResult_Unknown = 0,
			ChooseRoleResult_OK,
			ChooseRoleResult_SQLError,			/// ���ݿ����
		};
		uint8	m_Result;
		uint32	m_AccountId;
		uint8	m_RoleData[MAX_ROLE_DATA_SIZE];	/// �̳�SRoleInfo_Header
	PACKET_D2GS_INNER_END

	/// ����Ϣ���Կ���ѹ��,ʵ��Ч���д����ԶԱ�
	PACKET_D2GS_INNER_BEG( RoleListResult )
		uint8	m_RoleCount;								/// ��ɫ��Ŀ
		uint32	m_AccountId;
		//uint8  m_Result;									///
		uint8   m_RoleListData[MAX_ROLELIST_SIZE];			/// ������������߼�����������
	PACKET_D2GS_INNER_END

#pragma pack()
}
