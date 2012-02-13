/********************************************************************
	created:	2009-6-27
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�ͻ���->���ط�����
*********************************************************************/
#pragma once

#include "WoSocketHandler.h"
#include "WeLoginPlayer.h"
#include <string>
using namespace std;

namespace We
{
	class GameServer;

	class ClientHandler : public SocketHandler
	{
	public:
		ClientHandler();
		~ClientHandler();

		void Init();

		virtual void OnProcessPacket( PacketHeader* packet );
		virtual void OnDisconnect();

		inline uint32 GetId() const { return m_ClientId; }
		inline PlayerStatus GetClientStatus() const { return m_ClientStatus; }
		inline void SetClientStatus( PlayerStatus clientStatus ) { m_ClientStatus = clientStatus; }
		inline uint32 GetLoginFailCount() const { return m_LoginFailCount; }
		inline void SetLoginFailCount( uint32 loginFailCount ) { m_LoginFailCount = loginFailCount; }

		void Update();

	public:
		uint32 m_ClientId;				/// ��¼�ڼ����ɵ�ΨһId
		GameServer* m_GameServer;

		string	m_AccountName;			/// �˺���
		string	m_RoleName;				/// ��ɫ��
		uint32  m_AccountId;			/// �ʺ����ݿ�Id
		uint32  m_RoleId;				/// ��ɫ���ݿ�Id
		PlayerStatus m_ClientStatus;	/// ��ǰ״̬	
		uint16  m_MapId;				/// ��ǰ���ڵĵ�ͼ���

		uint32 m_ConnectTime;			/// ��¼���ӵ�ʱ��
		uint32 m_LoginFailCount;		/// ��¼ʧ�ܴ���ͳ��
	};
}