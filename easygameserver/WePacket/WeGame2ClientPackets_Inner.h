/********************************************************************
	created:	2009-6-27
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��Ϸ�����������ͻ��˵��ڲ���Ϣ
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_GameServer2Client_Inner
	{
		PT_GS2C_Inner_CreateAccountResult=1,/// ע���˺Ž��
		PT_GS2C_Inner_LoginResult,			/// ��½���
		PT_GS2C_Inner_RoleListResult,		/// ��ɫ�б���
		PT_GS2C_Inner_CreateRoleResult,		/// ������ɫ���
		PT_GS2C_Inner_ChooseRoleResult,		/// ѡ���ɫ���
		PT_GS2C_Inner_DeleteRoleResult,		/// ɾ����ɫ���
		PT_GS2C_Inner_EnterWorld,			/// ��ɫ������Ϸ
		PT_GS2C_Inner_WaitLogin,			/// ��������½,֪ͨ�ͻ��������ȴ�

		PT_GS2C_Inner_SyncRemovePlayers,	/// ĳ���������������뿪����Ұ
		PT_GS2C_Inner_SyncRemoveNpcs,		/// ĳ�����������Npc�뿪����Ұ
		PT_GS2C_Inner_SyncRemoveMonsters,	/// ĳ����������й����뿪����Ұ
		PT_GS2C_Inner_SyncRemoveItems,		/// ĳ�������������Ʒ�뿪����Ұ

		PT_GS2C_Inner_MapList,				/// �����б�	
		PT_GS2C_Inner_JoinMap,				/// ���뷿��
		PT_GS2C_Inner_PlayerEntryMap,		/// ��ҽ�������
		PT_GS2C_Inner_PlayerLeaveMap,		/// ����뿪������
		//PT_GS2C_Inner_GetObjData,			/// ���ض��������

		PT_GS2C_Inner_MAX,
	};

#define PACKET_GS2C_INNER_LOGIN_BEG( packetName ) \
	struct Packet_GS2C_Inner_##packetName## : PacketHeader\
	{\
	Packet_GS2C_Inner_##packetName##() { m_MainType=0; m_SubType = PT_GS2C_Inner_##packetName##; m_Length=sizeof( Packet_GS2C_Inner_##packetName## ); }

#define PACKET_GS2C_INNER_BEG( packetName ) \
	struct Packet_GS2C_Inner_##packetName## : PacketHeader\
	{\
	Packet_GS2C_Inner_##packetName##() { m_MainType=0; m_SubType = PT_GS2C_Inner_##packetName##; m_Length=sizeof( Packet_GS2C_Inner_##packetName## ); }

#define PACKET_GS2C_INNER_END };

	PACKET_GS2C_INNER_LOGIN_BEG( CreateAccountResult )
		enum CreateAccountResult
		{
			CreateAccountResult_Unknown = 0,
			CreateAccountResult_OK,
			CreateAccountResult_AccountExist,	/// �ʺ��Ѿ�����
		};
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		uint8	m_Result;
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_LOGIN_BEG( LoginResult )
		enum LoginResult
		{
			LoginResult_Unknown = 0,
			LoginResult_OK,
			LoginResult_OKAndReconnect,		/// ��֤OK,�ȴ���������
			LoginResult_AccountOrPwdError,	/// �ʺŻ��������
			LoginResult_MibaoError,			/// �ܱ�������
		};
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		uint8	m_Result;
	PACKET_GS2C_INNER_END

	/// ����Ϣ���Կ���ѹ��,ʵ��Ч���д����ԶԱ�
	PACKET_GS2C_INNER_LOGIN_BEG( RoleListResult )
		uint8	m_RoleCount;								/// ��ɫ��Ŀ
		uint32	m_AccountId;
		//uint8  m_Result;								///
		uint8   m_RoleListData[MAX_ROLELIST_SIZE];			/// ������������߼�����������
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_LOGIN_BEG( CreateRoleResult )
		enum CreateRoleResult
		{
			CreateRoleResult_Unknown = 0,
			CreateRoleResult_OK,
			CreateRoleResult_RoleNameExist,		/// ��ɫ���Ѿ�����
		};
		uint8	m_Result;
		uint8	m_RoleIndex;
		uint32  m_RoleId;
		char	m_RoleName[MAX_ROLE_NAME+1];
		uint8   m_CreateRoleData[MAX_CREATEROLE_SIZE];	/// ��ϸ�Ľ�ɫ����,���߼�����������
	PACKET_GS2C_INNER_END

	/// ����Ϣ���Կ���ѹ��,ʵ��Ч���д����ԶԱ�
	PACKET_GS2C_INNER_LOGIN_BEG( ChooseRoleResult )
		uint32	m_AccountId;
		uint8	m_RoleData[MAX_ROLE_DATA_SIZE];	/// �̳�SRoleInfo_Header
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_LOGIN_BEG( EnterWorld )
		uint8	m_PlayerData[MAX_ROLE_DATA_SIZE];
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_LOGIN_BEG( WaitLogin )
		enum WaitLoginReason
		{
			WaitLoginReason_RoleReconnect,		/// ��ɫ��������,�ȴ���������
			WaitLoginReason_RemoveOldOnlineRole,/// ͬһ�˺ŵ�������ɫ������Ϸ��,�ȴ��ɽ�ɫ�˳���Ϸ
		};
		uint32	m_LoginPlayerId; 
		uint8	m_Reason;		/// �ȴ�ԭ��
	PACKET_GS2C_INNER_END


	PACKET_GS2C_INNER_BEG( SyncRemovePlayers )
		uint16 m_ObjCount;
		uint32 m_ObjIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*sizeof(uint32));
		}
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( SyncRemoveNpcs )
		uint16 m_ObjCount;
		uint32 m_ObjIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*sizeof(uint32));
		}
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( SyncRemoveMonsters )
		uint16 m_ObjCount;
		uint32 m_ObjIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*sizeof(uint32));
		}
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( SyncRemoveItems )
		uint16 m_ObjCount;
		uint32 m_ObjIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*sizeof(uint32));
		}
	PACKET_GS2C_INNER_END

	struct MapInfo
	{
		char	m_Name[MAX_MAP_NAME+1];	/// ������
		uint16	m_MapId;				/// ����id
		uint16	m_PlayerNum;			/// ��������������
	};
	PACKET_GS2C_INNER_BEG( MapList )
		uint16 m_MapCount;
		uint8  m_Data[MAX_MAPLIST_DATA_SIZE];
	PACKET_GS2C_INNER_END

	struct SyncObjectId
	{
		uint32 m_ObjId;
		uint16 m_DataChecker;	/// ���ݱ仯�ı�ʶ
	};

	PACKET_GS2C_INNER_BEG( JoinMap )
		uint16			m_ObjCount;
		SyncObjectId	m_SyncObjectIds[1024];
		void CalcSize()
		{
			m_Length = (sizeof(*this)-(1024-m_ObjCount)*(sizeof(SyncObjectId)));
		}
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( PlayerEntryMap )
		SyncObjectId m_SyncObjectId;
	PACKET_GS2C_INNER_END

	PACKET_GS2C_INNER_BEG( PlayerLeaveMap )
		uint32 m_ObjId;
	PACKET_GS2C_INNER_END

#pragma pack()
}
