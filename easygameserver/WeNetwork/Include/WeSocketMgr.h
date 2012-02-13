/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	����������
*********************************************************************/
#pragma once

#include "WeSocket.h"
#include "WeSingleton.h"
#include <vector>
#include <map>
using namespace std;

namespace We
{
	class Socket;
	class SocketListener;

	class /*We_DLL_EXPORTS*/ SocketMgr : public Singleton<SocketMgr>
	{
		friend unsigned int WINAPI Thread_SendPackets(void* arg);	/// �����߳�
	public:
		SocketMgr();
		~SocketMgr();

		/// ���һ������
		void AddSocketListener( uint32 listenerId, SocketListenerHandler* handler, uint32 maxConnections, uint16 maxSendPacketSize,
			uint16 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize, bool beforeStart, uint encryptType=0 );

		/// ���һ������(��Ҫ�ڶ��߳���������!!!)
		bool AddSocketConnector( uint32 connectorId, SocketHandler* handler, const char* ip, uint16 port,
			uint16 maxSendPacketSize, uint16 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize, bool beforeStart, uint encryptType=0 );

		/// ����IOCP�߳�
		/// iocpThreadNum==0ʱ,��ʾ����CPU�����������߳���
		void StartIOCP( unsigned int iocpThreadNum );

		/// ����һ������
		bool StartListen( uint32 listenerId, const char* ip, unsigned short port, bool listenAnyIP=false );

		/// ��ʼ����
		bool StartConnect( uint32 connectorId, const char* ip, unsigned short port );

		/// �������ӷ���,�������̵߳���
		void SendPackets();

		/// �߼��̵߳��øýӿ�,�������ݰ�
		void Update();

		/// �ر�
		void Shutdown();

		inline HANDLE GetCompletionPort() const { return m_CompletionPort; }
		inline bool IsSendingPackets() const { return m_SendingPackets; }
		inline bool IsShutdown() const { return m_IsShutdown; }

	protected:
		inline void SetSendingPackets( bool isSending ) { m_SendingPackets = isSending; }

	protected:
		bool	m_IsShutdown;								/// �Ƿ�ֹͣ
		bool	m_SendingPackets;							/// �����߳��Ƿ���������
		unsigned int m_IOCPThreadCount;						/// IOCP���߳�����
		HANDLE m_CompletionPort;							/// ��ɶ˿ھ��

		vector<SocketListener*> m_NewSocketListeners;		/// �µļ����б�
		vector<Socket*>			m_NewSocketConnectors;		/// �µ������б�
		WinLock					m_NewSocketListenersLock;	/// �µļ����б���߳���
		WinLock					m_NewSocketConnectorsLock;	/// �µ������б���߳���

		map<unsigned int,SocketListener*>	m_SocketListeners;		/// �����б�
		map<unsigned int,Socket*>			m_SocketConnectors;		/// �����б�
		WinLock								m_SocketListenersLock;	/// �����б���߳���
		WinLock								m_SocketConnectorsLock;	/// �����б���߳���
	};

}