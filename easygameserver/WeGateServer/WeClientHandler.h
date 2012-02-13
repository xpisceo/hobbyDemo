/********************************************************************
	created:	2009-6-27
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�ͻ���->���ط�����
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"
#include <string>
using namespace std;

namespace We
{
	class GateServer;

	/// �ͻ��˵�״̬
	enum ClientStatus
	{
		ClientStatus_AccountLogin,			/// �ʺ���֤
		ClientStatus_EnterWorld,			/// ������Ϸ����
	};

	class ClientHandler : public SocketHandler
	{
	public:
		ClientHandler();
		~ClientHandler();

		void Init();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		inline uint32 GetId() const { return m_Id; }
		inline ClientStatus GetClientStatus() const { return m_ClientStatus; }
		inline void SetClientStatus( ClientStatus clientStatus ) { m_ClientStatus = clientStatus; }
		inline uint32 GetLoginFailCount() const { return m_LoginFailCount; }
		inline void SetLoginFailCount( uint32 loginFailCount ) { m_LoginFailCount = loginFailCount; }

		void Update();

	public:
		uint32 m_Id;					/// ��¼�ڼ����ɵ�ΨһId
		GateServer* m_GateServer;

		string	m_AccountName;			/// �˺���
		string	m_RoleName;				/// ��ɫ��
		uint32  m_AccountId;			/// �ʺ����ݿ�Id
		uint32  m_RoleId;				/// ��ɫ���ݿ�Id
		ClientStatus m_ClientStatus;	/// ��ǰ״̬	
		uint16  m_MapId;				/// ��ǰ���ڵĵ�ͼ���

		uint32 m_ConnectTime;			/// ��¼���ӵ�ʱ��
		uint32 m_LoginFailCount;		/// ��¼ʧ�ܴ���ͳ��
	};
}