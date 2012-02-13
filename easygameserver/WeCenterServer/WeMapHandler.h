/********************************************************************
	created:	2009-6-28
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��ͼ������->���ķ�����
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class CenterServer;

	enum MapStatus
	{
		MapStatus_WaitForInfo,			/// ��������,�ȴ����ط����������Լ�����Ϣ
		MapStatus_Running,				/// �յ������ط���������Ϣ,��������
	};

	class MapHandler : public SocketHandler
	{
	public:
		MapHandler();
		~MapHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		uint16 m_MapServerId;
		MapStatus m_MapStatus;
		CenterServer* m_CenterServer;
	};
}