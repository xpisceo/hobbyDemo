/********************************************************************
	created:	2009-6-28
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	中心服务器
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"

namespace We
{
	class MapServer : public SocketListenerHandler
	{
	public:
		MapServer();
		~MapServer();

		bool Init();
		void Shutdown();

		/// 新网关服务器连接
		virtual void OnAccept( Socket* socket );
		/// 移除网关服务器连接
		virtual void OnRemove( Socket* socket );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}