/********************************************************************
	created:	2009-6-28
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ط�����->���ķ�����
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"

namespace We
{
	class CenterServer;

	enum GateStatus
	{
		GateStatus_WaitForInfo,			/// ��������,�ȴ����ط����������Լ�����Ϣ
		GateStatus_Running,				/// �յ������ط���������Ϣ,��������
	};

	class GateHandler : public SocketHandler
	{
	public:
		GateHandler();
		~GateHandler();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		void Update();

	public:
		uint8 m_GateId;
		GateStatus m_GateStatus;
		CenterServer* m_CenterServer;
	};
}