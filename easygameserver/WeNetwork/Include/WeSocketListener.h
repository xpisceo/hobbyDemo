/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�������
*********************************************************************/
#pragma once

#include <vector>
using namespace std;

#include "WeSocket.h"
#include "WeObjectPool.h"

namespace We
{
	class SocketListener
	{
		friend class SocketMgr;
		friend unsigned int WINAPI Thread_Listening(void* arg); /// �����߳�
	public:
		SocketListener();
		virtual ~SocketListener();

		void Init( uint32 maxConnections, uint32 maxSendPacketSize, uint32 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize );

		inline void SetSocketListenerHandler( SocketListenerHandler* listener ) { m_SocketListenerHandler = listener; }
		inline SocketListenerHandler* GetSocketListenerHandler() const { return m_SocketListenerHandler; }

		/// ��ʼ����(����һ�������߳�)
		bool StartListen( const char* ip, unsigned short port, bool listenAnyIP=false );

		/// �ر�
		void Close();

		inline bool IsListening() const { return m_IsListening; }
		inline SOCKET GetSocket() const { return m_ListenSocket; }
		inline HANDLE GetIOCPHandle() { return m_CompletionPort; }

	protected:
		void OnAccept(SOCKET newSocket, sockaddr_in* address );

		void Update();

		void SendPackets();

	protected:
		unsigned int    m_ListenerId;
		unsigned		m_ListenThreadId;
		HANDLE			m_ListenThreadHandle;

		uint	m_MaxSendPacketSize;	/// �����Ĵ�С����	
		uint	m_MaxRecvPacketSize;	/// �հ��Ĵ�С����
		uint	m_SendBufferSize;		/// ���������С	
		uint	m_RecvBufferSize;		/// �հ������С
		uint	m_DisconnectTime;		/// ��鳬ʱ���ߵ�ʱ��

		bool m_IsListening;						/// �Ƿ��Ѿ��ڼ�����	
		SOCKET m_ListenSocket;					/// ����Socket
		struct sockaddr_in m_ListenAddress;		/// ������ַ 
		HANDLE m_CompletionPort;				/// ��ɶ˿ھ��

		ObjectPool<Socket>* m_SocketPool;		/// Socket�Ķ����

		unsigned int m_MaxConnections;			/// ���������
		vector<Socket*> m_NewConnections;		/// ������
		vector<Socket*> m_ActiveConnections;	/// �������
		WinLock m_NewConnectionsLock;
		WinLock m_ActiveConnectionsLock;

		SocketListenerHandler* m_SocketListenerHandler; /// �ӿ�

		EncryptPacketAPI	m_EncryptPacketAPI;
	};
}