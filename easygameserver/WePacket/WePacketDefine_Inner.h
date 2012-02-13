/********************************************************************
	created:	2009-6-27
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	内部消息的一些常量定义
*********************************************************************/
#pragma once

namespace We
{
	const uint32 MAX_ACCOUNT_NAME = 16;			/// 帐号长度(不包括'\0')
	const uint32 MAX_ACCOUNT_PWD = 32;			/// 账号密码长度
	const uint32 MAX_ENCRYPT_PWD = 32;			/// 加密后的密码长度
	const uint32 MAX_ROLE_NAME = 14;			/// 角色名长度(不包括'\0')
	const uint32 MAX_ROLE_NUM = 5;				/// 一个账号的角色数目上限
	const uint32 MAX_CREATEROLE_SIZE = 1024;	/// 创建角色时,所需要的缓存大小	
	const uint32 MAX_ROLELIST_SIZE = 1024*10;	/// 角色列表界面,存放所有角色信息的最大缓存大小
	const uint32 MAX_ROLE_CACHE_SIZE = 20*1024;	/// 角色数据的最大缓存大小
	const uint32 MAX_ROLE_DATA_SIZE = 1024*20;	/// 角色数据的最大缓存大小
	const uint32 MAX_MAPCOUNT_ONEMAPSERVER = 32;/// 一台地图服务器上的最大地图数量
	const uint32 MAX_MAP_NAME = 32;				/// 地图名字的长度
	const uint32 MAX_MAPLIST_DATA_SIZE = 1024;  /// 房间列表数据的最大长度

	typedef uint32 PlayerID;

	struct Position
	{
		float x;
		float y;
		float z;
	};
}