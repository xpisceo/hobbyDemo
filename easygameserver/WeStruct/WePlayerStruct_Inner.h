/********************************************************************
	created:	2009-7-26
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��������ҽṹ
*********************************************************************/
#pragma once

#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	/// �߼����ֵĽ�ɫ��Ϣ��Ҫ������ṹ�̳�
	struct SRoleInfo_RoleList_Header
	{
		char	m_RoleName[MAX_ROLE_NAME+1];
		uint32  m_RoleId;
		uint8	m_RoleIndex;
	};

	/// ��ɫ����
	/// �߼����ּ̳иýṹ
	struct SRoleInfo_Header
	{
		char		m_RoleName[MAX_ROLE_NAME+1];
		uint8		m_RoleIndex;
		uint32		m_RoleId;
		uint16		m_MapId;
		Position	m_Pos;
	};

#pragma pack()
}
