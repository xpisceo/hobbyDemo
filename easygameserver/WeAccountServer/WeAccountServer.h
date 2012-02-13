/********************************************************************
	created:	2009-6-29
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�ʺŷ�����
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"
#include "WeQueue.h"
#include "WeLock.h"

namespace We
{
	class GameHandler;
	class Mysql;

#pragma pack(1)
	struct QueryBufferHeader
	{
		uint32 m_GameServerId;
	};

	/// 0.25K
	struct QueryBufferSmall : QueryBufferHeader
	{
		uint8 m_Buffer[252];
	};

	/// 1K
	struct QueryBufferMid : QueryBufferHeader
	{
		uint8 m_Buffer[1020];
	};

	/// 4K
	struct QueryBufferBig : QueryBufferHeader
	{
		uint8 m_Buffer[4092];
	};
#pragma pack()

	class AccountServer : public SocketListenerHandler
	{
		friend unsigned long WINAPI Thread_ProcessQueryList(void* arg);	/// �����ѯ�߳�
	public:
		unsigned int m_TotalRecvMsgSizeSec;
		unsigned int m_TotalRecvMsgSize;
		unsigned int m_TotalSendMsgSizeSec;
		unsigned int m_TotalSendMsgSize;

		unsigned int m_LastSecTick;
		unsigned int m_LastShowTick;
		unsigned int m_LastSendTick;

		uint32	m_AccountServerId;
		string	m_GameListenIP;
		uint16	m_GameListenPort;
		uint32	m_MaxGameConnection;

		string	m_AccountIP;
		uint16	m_AccountPort;
		string	m_AccountUser;
		string	m_AccountPwd;
		string	m_AccountDbName;

		bool	m_Running;
		bool	m_IsProcessQueryList;
	protected:
		map<uint32, GameHandler*>	m_GameServers;					/// �����ķ�����������(ע:�������������map)

		LockQueue<QueryBufferHeader*, WinLock> m_QueryList;			/// ��ѯ��Ϣ����
		LockQueue<QueryBufferHeader*, WinLock> m_QueryResultList;	/// ��ѯ�������

		Mysql* m_Mysql;
	public:
		AccountServer();
		~AccountServer();

		inline uint32 GetAccountServerId() const { return m_AccountServerId; }

		bool Init();
		void Shutdown();

		/// �¿ͻ�������
		virtual void OnAccept( Socket* socket );
		/// �Ƴ��ͻ�������
		virtual void OnRemove( Socket* socket );

		/// �����ݰ������ķ�����
		bool SendToGame( uint32 gameServerId, PacketHeader* packet );

		/// ����һ�����ķ�����
		void OnAddGameServer( GameHandler* gameHandler, PacketHeader* packet );
		/// �Ƴ�һ�����ķ�����
		void OnRemoveGameServer( PacketHeader* packet );

		/// ��ѯ��Ϣ
		void PushQueryBuffer( uint32 gameServerId, PacketHeader* packet );
		void FreeQueryBuffer( QueryBufferHeader* buffer );

		/// ��ѯ���
		QueryBufferHeader* AllocResultBuffer( uint32 gameServerId, uint32 bufferSize );
		void FreeResultBuffer( QueryBufferHeader* buffer );

		/// �߳������: �����ѯ����
		void OnProcessQuery( QueryBufferHeader* buffer );

		/// �ͻ��˵�¼
		void OnClientLogin( QueryBufferHeader* buffer );

		/// �����ʺ�
		void OnCreateAccount( QueryBufferHeader* buffer );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}