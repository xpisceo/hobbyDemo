/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��������
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeLock.h"
#include "WeSocketDefines.h"
#include "WoSocketHandler.h"
#include "WeCircularBuffer.h"
#include "WePacketBuffer.h"
#include "WePacketHeader.h"
#include "WeQueue.h"
#include "WeLock.h"

namespace We
{
	typedef bool (*EncryptPacketAPI)( uint8* data, uint len, uint type );

	class /*We_DLL_EXPORTS*/ Socket
	{
		friend class SocketListener;
		friend class SocketMgr;
		friend unsigned int WINAPI Thread_IOCP_QueueResult(void* arg); /// ��ɶ˿��߳�
	public:
		Socket();
		virtual ~Socket();

		void Init( SOCKET socket, uint32 maxSendPacketSize, uint32 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize );

		void SetSocketHandler( SocketHandler* socketHandler) { m_SocketHandler = socketHandler; }
		SocketHandler* GetSocketHandler() { return m_SocketHandler; }

		/// ip������IP��ַ, Ҳ����������
		bool SetConnectAddress( const char* ip, uint16 port );
		bool Connect( const char* ip, uint16 port );
		bool Reconnect();

		void Disconnect();

		void Delete();

		string GetRemoteIP();
		inline uint32 GetRemotePort() { return ntohs(m_ClientAddress.sin_port); }
		inline SOCKET GetSocket() { return m_Socket; }

		inline bool IsDeleted() { return m_Deleted; }
		inline bool IsConnected() { return m_IsConnected; }

		inline sockaddr_in& GetRemoteAddress() { return m_ClientAddress; }

		/// ��ȡ������������ݴ�С
		uint GetSendBufferSize();

		// ����ͳ��
		uint GetSendPerSecond() const { return m_SendPerSecond; }
		uint GetRecvPerSecond() const { return m_RecvPerSecond; }
		uint GetTotalSendSize() const { return m_TotalSendSize; }
		uint GetTotalRecvSize() const { return m_TotalRecvSize; }
		uint GetStartTick() const { return m_StartTick; }
		uint GetPing() const { return m_Ping; }

		// ���һ��ping��ʱ��
		uint GetLastPingTick() const { return m_LastPingTick; }
		// ���һ��pong���ص�ʱ��
		uint GetLastPongTick() const { return m_LastPongTick; }

		/// �����ݷ��뷢�ͻ���
		bool Send( PacketHeader* packet );
		bool Ping();
	protected:
		bool Pong( PacketPing* packetPing );

	protected:
		void _OnConnect();

		void AssignToCompletionPort();

		inline void SetCompletionPort(HANDLE cp) { m_CompletionPort = cp; }

		void Accept(sockaddr_in * address);

		inline void IncSendLock() { InterlockedIncrement(&m_IsSending); }
		inline void DecSendLock() { InterlockedDecrement(&m_IsSending); }

		/// ��IOCP����������ݰ�
		void StartRecv();

		/// ��IOCP���������ݰ�
		void StartSend();

		/// ���յ�����
		void OnRecvBuffer( unsigned int len );

		/// ����������
		void OnSendBuffer( unsigned int len );

		/// �������ݰ�
		void ProcessPackets();

		void Update();

	protected:
		unsigned int m_ConnectorId;
		SocketHandler* m_SocketHandler;			/// ���߼�������õĽӿ�,��Ҫ�ڶ��߳���ʹ��!!!
		bool m_Alloc;							/// �Ƿ��Ѿ��������ڴ�
		bool m_IsConnected;
		bool m_Deleted;
		volatile long m_IsSending;				/// �Ƿ����ڷ�������(ԭ����)

		SOCKET m_Socket;
		sockaddr_in m_ClientAddress;
		HANDLE m_CompletionPort;

		OverlappedStruct m_RecvEvent;
		OverlappedStruct m_SendEvent;

		WinLock m_SendLock;
		WinLock m_RecvLock;

		unsigned int	m_MaxSendPacketSize;	/// �����Ĵ�С����
		unsigned int	m_MaxRecvPacketSize;	/// �հ��Ĵ�С����
		//PacketBuffer	m_RecvPacketBuffer;		/// �հ�����(�����Ļ��λ���)
		CircularBuffer	m_RecvPacketBuffer;		/// �հ�����(�������Ļ��λ���)
		CircularBuffer	m_SendPacketBuffer;		/// ��������(�������Ļ��λ���)
		PacketHeader	m_PacketHeader;			/// ��ȡ����Ϣͷ
		bool			m_HaveReadPacketHeader;	/// �Ƿ��Ѿ���ȡ����Ϣͷ

		PacketHeader*	m_CurrentPacket;				/// ���ڽ��յ����ݰ�
		LockQueue<PacketHeader*,WinLock> m_PacketQueue;	/// ���ݰ�����

		uint	m_StartTick;					/// ��ʼʱ��
		uint	m_LastSendTick;					/// �������ݰ������ʱ��
		uint	m_LastRecvTick;					/// �յ����ݰ������ʱ��
		uint	m_LastPingTick;					/// ���ping��ʱ��
		uint	m_LastPongTick;					/// ����յ�pong��ʱ��
		uint	m_LastActiveTick;				/// ���ʱ��,�����հ��ͷ���
		uint	m_LastSamplingTick;				/// �ϴβ���ʱ��
		uint	m_SendSamplingSize;				/// �����ڼ�ķ�����
		uint	m_RecvSamplingSize;				/// �����ڼ�Ľ�����
		uint	m_SendPerSecond;				/// ÿ�뷢�͵�������
		uint	m_RecvPerSecond;				/// ÿ����ܵ�������
		uint	m_TotalSendSize;				/// �ܷ�����
		uint	m_TotalRecvSize;				/// �ܽ�����
		uint	m_Ping;							/// Pingֵ

		EncryptPacketAPI	m_EncryptPacketAPI;
	};

}
