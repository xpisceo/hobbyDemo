/********************************************************************
	created:	2009-7-26
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	基本的玩家结构
*********************************************************************/
#pragma once

#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	/// 逻辑部分的角色信息需要从这个结构继承
	struct SRoleInfo_RoleList_Header
	{
		char	m_RoleName[MAX_ROLE_NAME+1];
		uint32  m_RoleId;
		uint8	m_RoleIndex;
	};

	/// 角色数据
	/// 逻辑部分继承该结构
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
