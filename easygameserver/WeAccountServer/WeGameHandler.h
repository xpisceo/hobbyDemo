/********************************************************************
	created:	2009-6-29
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��Ϸ������->�ʺŷ�����
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class AccountServer;

	enum GameServerStatus
	{
		GameServerStatus_WaitForInfo,			/// ��������,�ȴ���Ϸ�����������Լ�����Ϣ
		GameServerStatus_Running,				/// �յ�����Ϸ����������Ϣ,��������
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
		AccountServer* m_AccountServer;
	};
}