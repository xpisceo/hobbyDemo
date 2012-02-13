/********************************************************************
	created:	2009-6-29
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ķ�����->�ʺŷ�����
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class DbServer;

	enum GameServerStatus
	{
		GameServerStatus_WaitForInfo,			/// ��������,�ȴ����ķ����������Լ�����Ϣ
		GameServerStatus_Running,				/// �յ������ķ���������Ϣ,��������
	};

	class GameHandler : public SocketHandler
	{
	public:
		GameHandler();
		~GameHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		unsigned int m_GameServerId;
		GameServerStatus m_GameServerStatus;
		DbServer* m_DbServer;
	};
}