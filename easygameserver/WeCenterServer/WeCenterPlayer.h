/********************************************************************
	created:	2009-6-28
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��½�е���Һ����ķ������ϵ����
*********************************************************************/
#pragma once

#include "WePacketDefine_Inner.h"
#include "WePlayerStruct.h"

namespace We
{
	/// ���״̬
	enum CenterPlayerStatus
	{
		CenterPlayerStatus_None,
		CenterPlayerStatus_ChooseRoleWait,	/// ѡ���˽�ɫ,�ȴ����ݿⷵ�ظý�ɫ������
		CenterPlayerStatus_EnterWorldWait,	/// ��ȡ�˽�ɫ����,���ͼ������������Ϣ,�ȴ���ͼ������������Ҷ��󲢷����ͼ
		CenterPlayerStatus_ChangeMapWait,	/// ��һ����ͼ������ת����һ����ͼ������,�ȴ���һ����ͼ������������Ҷ��󲢷����ͼ
		CenterPlayerStatus_InMap,			/// ��ĳһ����ͼ��,������Ϸ

		CenterPlayerStatus_MAX,
	};

	/// ��¼�ڵ����(��ɫ�б�,������ɫ,ɾ����ɫ,ѡ���ɫ)
	class LoginPlayer
	{
	public:
		LoginPlayer() { m_Id = 0; m_AccountId = 0; m_LoginTime=0; m_GateId=0;
						m_RoleCount = 0; }

		uint32 m_Id;						/// ���ɵ�Ψһid
		uint32 m_AccountId;					/// �ʺ�id
		string m_AccountName;				/// �ʺ���
		uint32 m_LoginTime;					/// ��¼��¼�ɹ���ʱ��
		uint8  m_GateId;					/// ����id
		uint8  m_RoleCount;					/// ��ɫ����
		uint32 m_RoleIds[MAX_ROLE_NUM];		/// ��ɫid�б�
		string m_RoleNames[MAX_ROLE_NUM];	/// ��ɫ���б�
	};

	/// ѡ���ɫ������
	class CenterPlayer
	{
	public:
		CenterPlayer() { m_AccountId=0; m_RoleId=0; m_GateId=0; m_MapId=0; m_OldMapId=0; m_RoleIndex=-1;
						 m_IsWaitReconnect=false; m_IsCache=false; m_PlayerStatus=CenterPlayerStatus_None;}

		uint32 m_AccountId;			/// �ʺ�id
		string m_AccountName;		/// �ʺ���
		uint32 m_RoleId;			/// ��ɫid
		string m_RoleName;			/// ��ɫ��
		int8   m_RoleIndex;			/// ��ɫ����
		uint8  m_GateId;			/// ����id
		uint16 m_MapId;				/// ��ͼid
		uint16 m_OldMapId;			/// ��һ����ͼid
		CenterPlayerStatus m_PlayerStatus;		/// ���״̬���
		uint32 m_StartTime;						/// ״̬��ʼ��ʱ��
		bool   m_IsWaitReconnect;				/// ���ڵȴ���������
		bool   m_IsCache;						/// �Ƿ񻺴���
		SRoleInfo_DB m_Cache;					/// ��ɫ���ݻ���
	};
}