/********************************************************************
	created:	2009-6-28
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ķ�����
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

		/// �����ط���������
		virtual void OnAccept( Socket* socket );
		/// �Ƴ����ط���������
		virtual void OnRemove( Socket* socket );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}