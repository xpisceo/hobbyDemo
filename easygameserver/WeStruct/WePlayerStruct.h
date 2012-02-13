/********************************************************************
	created:	2009-7-12
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	逻辑部分的玩家结构
*********************************************************************/
#pragma once

#include "WePlayerStruct_Inner.h"
#include <string>
using namespace std;

namespace We
{
//#pragma pack(1)
/// 结构版本
#define PLAYER_STRUCT_VERSION 1

	/// 创建角色
	struct SCreateRoleInfo
	{
		uint8	m_RoleIndex;					/// 角色索引 (size=1)
		char	m_RoleName[MAX_ROLE_NAME+1];	/// 角色名 (size=15)

		/// size=4
		uint32  m_Sex:2;						/// 性别(0-1)
		uint32  m_Race:4;						/// 种族(0-15)
		uint32  m_FaceId:8;						/// 脸型(0-255)
		uint32  m_HairId:8;						/// 发型(0-255)
		uint32  _dummy1:10;						/// 4字节对齐
	};

	/// 玩家的外观
	struct SPlayerVisual
	{
		uint32  m_FaceId:8;						/// 脸型(0-255)
		uint32  m_HairId:8;						/// 发型(0-255)
		uint32  _dummy1:16;						/// 4字节对齐
	};

	/// 角色列表用到的角色数据
	struct SRoleList_RoleInfo
	{
		uint8	m_RoleIndex;					/// 角色索引 (size=1)
		char	m_RoleName[MAX_ROLE_NAME+1];	/// 角色名 (size=15)
		uint32  m_RoleId;						/// 角色id (size=4)

		/// size=4
		uint32  m_Sex:2;						/// 性别(0-1)
		uint32  m_Race:4;						/// 种族(0-15)
		uint32  _dummy1:26;						/// 4字节对齐

		uint32  m_OnlineTime;					/// 在线时间(分钟) (size=4)

		SPlayerVisual m_Visual;					/// 玩家的外观
	};

	/// 玩家的背包
	struct SPlayerBag
	{
		uint8  m_Data[1024];
	};

	/// 玩家的仓库
	struct SPlayerStorage
	{
		uint8  m_Data[2048];
	};

	/// 保存到数据库的数据
	//struct SRoleInfo_Header
	//{
	//	char		m_RoleName[MAX_ROLE_NAME+1];
	//	uint32		m_RoleId;
	//	uint8		m_RoleIndex;
	//	uint16		m_MapId;
	//	Position	m_Pos;
	//};
	struct SRoleInfo_DB : SRoleInfo_Header
	{
		/// size=4
		uint32  m_Sex:2;						/// 性别(0-1)
		uint32  m_Race:4;						/// 种族(0-15)
		uint32  _dummy1:26;						/// 4字节对齐

		uint32  m_OnlineTime;					/// 在线时间(分钟) (size=4)

		SPlayerVisual m_Visual;					/// 玩家的外观
		SPlayerBag	  m_Bag;					/// 背包
		uint8		  m_Data[4096];				/// 其他数据
	};

	struct SRoleInfo_Client
	{
		uint32		  m_RoleId;
		Position	  m_Pos;
		uint16		  m_MapId;
		uint16		  _dummy1;					/// 4字节对齐
		SPlayerVisual m_Visual;					/// 玩家的外观
		SPlayerBag	  m_Bag;					/// 背包
	};

//#pragma pack()
}
