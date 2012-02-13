/********************************************************************
	created:	2009-7-19
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�ͻ��˷�����Ϸ���������ڲ���Ϣ
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_Client2GameServer_Inner
	{
		PT_C2GS_Inner_CreateAccount = 1,	/// ע���˺�
		PT_C2GS_Inner_Login,				/// ��½
		PT_C2GS_Inner_CreateRole,			/// ������ɫ
		PT_C2GS_Inner_ChooseRole,			/// ѡ���ɫ
		PT_C2GS_Inner_DeleteRole,			/// ɾ����ɫ

		PT_C2GS_Inner_Move,					/// �ƶ�

		PT_C2GS_Inner_JoinMap,				/// ���뷿��
		PT_C2GS_Inner_GetObjData,			/// �����ȡĳ����������� 

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
		/// �����˺���Ϣ
	PACKET_C2GS_INNER_END

	/// ��¼��Ϣ���Կ������ܱ����������Ϊkey������
	/// ������ô���ܲűȽϿɿ�,��Ҫ�˽�һ�ºڿ�����ô�ƽ��,���ܶ�֢��ҩ!!!
	PACKET_C2GS_INNER_LOGIN_BEG( Login )
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		char	m_EncryptPwd[MAX_ENCRYPT_PWD+1];
		/// �ܱ���......
	PACKET_C2GS_INNER_END

	/// ������ɫʱ����Ҫ�Ļ�������
	PACKET_C2GS_INNER_LOGIN_BEG( CreateRole )
		uint8	m_RoleIndex;
		char	m_RoleName[MAX_ROLE_NAME+1];
		uint8   m_CreateRoleData[MAX_CREATEROLE_SIZE];	/// ��ϸ�Ľ�ɫ����,���߼�����������
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
		uint16 m_MapId;		/// ������
	PACKET_C2GS_INNER_END

	PACKET_C2GS_INNER_BEG( GetObjData )
		uint32 m_ObjId;
	PACKET_C2GS_INNER_END

#pragma pack()
}
