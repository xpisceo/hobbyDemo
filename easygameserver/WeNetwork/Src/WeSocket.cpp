#include <winsock2.h>
#include "WeSocket.h"
#include "WeSocketOps.h"
#include "WeSocketMgr.h"
#include "WePacketHeader.h"
#include "WePacketFactory.h"
#include "WeLog.h"

namespace We
{
	Socket::Socket() : m_Socket(0), m_IsConnected(false),	m_Deleted(false)
	{
		m_ConnectorId = 0;
		m_SocketHandler = 0;
		m_MaxSendPacketSize = 1024*20;
		m_MaxRecvPacketSize = m_MaxSendPacketSize;
		m_Alloc = false;
		m_IsConnected = false;
		m_Deleted = false;
		m_CurrentPacket = 0;
		m_LastActiveTick = m_StartTick = m_LastSendTick = m_LastRecvTick = 0;
		m_LastPingTick = m_LastPongTick = 0;
		m_LastSamplingTick = m_SendSamplingSize = m_RecvSamplingSize = m_SendPerSecond = m_RecvPerSecond = 0;
		m_TotalSendSize = m_TotalRecvSize = 0;
		m_HaveReadPacketHeader = false;
		m_Ping = 0;
		m_EncryptPacketAPI = NULL;
	}

	Socket::~Socket()
	{
		Disconnect();
		if( m_CurrentPacket != 0 )
		{
			PacketFactory::getSingleton().FreePacket( m_CurrentPacket );
			m_CurrentPacket = 0;
		}
		PacketHeader* packet = 0;
		while( packet = m_PacketQueue.Pop() )
		{
			PacketFactory::getSingleton().FreePacket( packet );
		}
	}

	void Socket::Init( SOCKET socket, uint32 maxSendPacketSize, uint32 maxRecvPacketSize, uint32 sendBufferSize, uint32 recvBufferSize )
	{
		if( !m_Alloc )
		{
			m_SendLock.Lock();
			m_SendPacketBuffer.Allocate( sendBufferSize );
			m_SendLock.Unlock();
			m_RecvLock.Lock();
			m_RecvPacketBuffer.Allocate( recvBufferSize );
			m_RecvLock.Unlock();
			m_Alloc = true;
		}
		else
		{
			m_SendLock.Lock();
			m_SendPacketBuffer.Reset();
			m_SendLock.Unlock();
			m_RecvLock.Lock();
			m_RecvPacketBuffer.Reset();
			m_RecvLock.Unlock();
			PacketHeader* packet = 0;
			while( packet = m_PacketQueue.Pop() )
			{
				PacketFactory::getSingleton().FreePacket( packet );
			}
		}
		if( m_CurrentPacket != 0 )
		{
			PacketFactory::getSingleton().FreePacket( m_CurrentPacket );
			m_CurrentPacket = 0;
		}
		m_IsConnected = false;
		m_Deleted = false;

		m_MaxSendPacketSize = maxSendPacketSize;
		m_MaxRecvPacketSize = maxRecvPacketSize;
		if( m_MaxSendPacketSize > MAX_PACKET_SIZE )
			m_MaxSendPacketSize = MAX_PACKET_SIZE;
		if( m_MaxRecvPacketSize > MAX_PACKET_SIZE )
			m_MaxRecvPacketSize = MAX_PACKET_SIZE;

		m_ConnectorId = 0;
		m_SocketHandler = 0;

		m_IsSending = 0;
		m_CompletionPort = 0;

		m_LastActiveTick = m_StartTick = m_LastSendTick = m_LastRecvTick = 0;
		m_LastPingTick = m_LastPongTick = 0;
		m_LastSamplingTick = m_SendSamplingSize = m_RecvSamplingSize = m_SendPerSecond = m_RecvPerSecond = 0;
		m_TotalSendSize = m_TotalRecvSize = 0;
		m_HaveReadPacketHeader = false;
		m_Ping = 0;

		m_Socket = socket;
		if(m_Socket == 0)
			m_Socket = SocketOps::CreateTCPFileDescriptor();
	}

	bool Socket::SetConnectAddress( const char* ip, uint16 port )
	{
		struct hostent * ci = gethostbyname( ip );
		if(ci == 0)
			return false;

		m_ClientAddress.sin_family = ci->h_addrtype;
		m_ClientAddress.sin_port = ntohs((u_short)port);
		memcpy(&m_ClientAddress.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);
		return true;
	}

	bool Socket::Connect( const char* ip, uint16 port )
	{
		if( !SetConnectAddress( ip, port ) )
			return false;

		SocketOps::Blocking( m_Socket );
		if(connect(m_Socket, (const sockaddr*)&m_ClientAddress, sizeof(m_ClientAddress)) == -1)
			return false;

		m_CompletionPort = SocketMgr::getSingleton().GetCompletionPort();

		_OnConnect();
		return true;
	}

	bool Socket::Reconnect()
	{
		if( m_IsConnected )
			return true;
		SocketOps::Blocking( m_Socket );
		if(connect(m_Socket, (const sockaddr*)&m_ClientAddress, sizeof(m_ClientAddress)) == -1)
			return false;
		m_CompletionPort = SocketMgr::getSingleton().GetCompletionPort();
		_OnConnect();
		return true;
	}

	void Socket::Accept(sockaddr_in * address)
	{
		memcpy(&m_ClientAddress, address, sizeof(*address));
		_OnConnect();
	}

	void Socket::AssignToCompletionPort()
	{
		CreateIoCompletionPort((HANDLE)m_Socket, m_CompletionPort, (ULONG_PTR)this, 0);
	}

	void Socket::_OnConnect()
	{
		SocketOps::Nonblocking(m_Socket);
		SocketOps::DisableBuffering(m_Socket);
		m_IsConnected = true;

		m_StartTick			= ::timeGetTime();
		m_LastSamplingTick	= m_StartTick;

		AssignToCompletionPort();
		StartRecv();
	}

	bool Socket::Send( PacketHeader* packet )
	{
		assert( packet->m_Length >= sizeof(PacketHeader) );
		assert( packet->m_Length <= m_MaxSendPacketSize );
		bool ret = false;
		uint16 len = packet->m_Length;
		// �ڴ˶���Ϣ���м���
		if( m_EncryptPacketAPI )
			m_EncryptPacketAPI( (uint8*)packet, len, 1 );	// ����
		m_SendLock.Lock();
		ret = m_SendPacketBuffer.Write( (const uint8*)packet, len);
		m_SendLock.Unlock();
		if( ret )
			m_LastSendTick = ::timeGetTime();
		//if( m_EncryptPacketAPI )
		//	m_EncryptPacketAPI( (uint8*)packet, len, 2 );	// ����
		return ret;
	}

	bool Socket::Ping()
	{
		PacketPing packetPing;
		// �ڴ˶���Ϣ���м���
		packetPing.m_Tick = ::timeGetTime();
		return Send( &packetPing );
	}
	bool Socket::Pong( PacketPing* packetPing )
	{
		// �ڴ˶���Ϣ���м���
		packetPing->m_SubType = 254;
		return Send( packetPing );
	}

	uint Socket::GetSendBufferSize()
	{
		return m_SendPacketBuffer.GetSize();
	}

	string Socket::GetRemoteIP()
	{
		char* ip = (char*)inet_ntoa( m_ClientAddress.sin_addr );
		if( ip != NULL )
			return string( ip );
		else
			return "";
	}

	void Socket::Disconnect()
	{
		m_IsConnected = false;
		m_Deleted = false;

		if( m_Socket != INVALID_SOCKET )
		{
			SocketOps::CloseSocket( m_Socket );
			m_Socket = INVALID_SOCKET;
		}
	}

	void Socket::Delete()
	{
		if( this != 0 )
			m_Deleted = true;
	}

	void Socket::StartSend()
	{
		if(m_Deleted || !m_IsConnected)
			return;

		m_SendLock.Lock();
		/// ������һ�εķ��ͻ�û�����
		/// IOCP,ÿ��Socketͬʱֻ����һ����������
		if( m_IsSending )
		{
			m_SendLock.Unlock();
			return;
		}
		if( m_SendPacketBuffer.GetContiguiousBytes() )
		{
			IncSendLock();

			DWORD w_length = 0;
			DWORD flags = 0;

			WSABUF buf;
			buf.len = (u_long)m_SendPacketBuffer.GetContiguiousBytes();	// һ�ο���ȡ��������,��һ����ȫ������
			buf.buf = (char*)m_SendPacketBuffer.GetBufferStart();

			m_SendEvent.Mark();
			m_SendEvent.Reset(SOCKET_IO_EVENT_SEND_END);
			int r = WSASend(m_Socket, &buf, 1, &w_length, flags, &m_SendEvent.m_overlap, 0);
			if(r == SOCKET_ERROR)
			{
				if(WSAGetLastError() != WSA_IO_PENDING)
				{
					int wsaLastError = WSAGetLastError();
					m_SendEvent.Unmark();
					DecSendLock();
					Delete();
				}
			}
		}
		m_SendLock.Unlock();
	}

	void Socket::OnSendBuffer( unsigned int len )
	{
		m_SendEvent.Unmark();
		m_SendLock.Lock();
		m_SendPacketBuffer.Remove(len);
		DecSendLock();
		m_SendLock.Unlock();

		m_TotalSendSize		+= len;
		m_SendSamplingSize	+= len;
		m_LastActiveTick	= ::timeGetTime();
	}

	void Socket::StartRecv()
	{
		if(m_Deleted || !m_IsConnected)
			return;

		m_RecvLock.Lock();
		DWORD r_length = 0;
		DWORD flags = 0;
		WSABUF buf;
		buf.len = (u_long)m_RecvPacketBuffer.GetSpace(); // GetSpace()�����ѡ��AB�����Ŀռ�,��Ҫ��������һ��֮ǰ������һ��!
		buf.buf = (char*)m_RecvPacketBuffer.GetBuffer();
		assert( buf.len > 0 );

		m_RecvEvent.Mark();
		m_RecvEvent.Reset(SOCKET_IO_EVENT_RECV_COMPLETE);
		if(WSARecv(m_Socket, &buf, 1, &r_length, &flags, &m_RecvEvent.m_overlap, 0) == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSA_IO_PENDING)
			{
				m_RecvEvent.Unmark();
				Delete();
			}
		}
		m_RecvLock.Unlock();
	}

	void Socket::OnRecvBuffer( unsigned int len )
	{
		m_RecvEvent.Unmark();
		if(len)
		{
			m_RecvLock.Lock();
			m_RecvPacketBuffer.IncrementWritten( len );
			ProcessPackets();		// �����������ݰ��������
			m_RecvLock.Unlock();
			StartRecv();			// ��������������ݰ�

			m_TotalRecvSize		+= len;
			m_RecvSamplingSize	+= len;
			m_LastActiveTick = m_LastRecvTick = ::timeGetTime();
		}
		else
		{
			DWORD lastError = GetLastError();
			int wsaLastError = WSAGetLastError();
			Delete();	    /// ��������,�Żᵽ��
			//DebugOutput( "Socket::OnRecvBuffer len=0 lastError=%u wsaLastError=%d", lastError, wsaLastError );
		}
	}

	void Socket::ProcessPackets()
	{
		for(;;)
		{
			// ��û��ȡ��Ϣͷ,���ȶ�ȡ��Ϣͷ
			if( !m_HaveReadPacketHeader )
			{
				// �ж��Ƿ���Ϣͷ�ĳ���
				if( m_RecvPacketBuffer.GetSize() < sizeof(PacketHeader) )
				{
					return;
				}
				// ��ȡ��Ϣͷ,�հ������ָ�뽫�ƶ�
				m_RecvPacketBuffer.Read( &m_PacketHeader, sizeof(PacketHeader) );
				// �ڴ˽�����Ϣͷ
				if( m_EncryptPacketAPI )
					m_EncryptPacketAPI( (uint8*)&m_PacketHeader, sizeof(PacketHeader), 2 );
				// ���������Ϣͷ��������
				if( m_PacketHeader.m_Length < sizeof(PacketHeader) )
				{
					Delete();
					DebugOutput( "�������Ϣͷ����,����̫��,IP=%s", GetRemoteIP().c_str() );
					return;
				}
				else if( m_PacketHeader.m_Length > m_MaxRecvPacketSize )
				{
					Delete();
					DebugOutput( "�������Ϣͷ����,���ȳ����������,IP=%s Len=%u LIMIT=%u", GetRemoteIP().c_str(), m_PacketHeader.m_Length, m_MaxRecvPacketSize );
					return;
				}
				m_HaveReadPacketHeader = true;
			}
			// ��û�չ����������ݰ�
			if( m_RecvPacketBuffer.GetSize() + sizeof(PacketHeader) < m_PacketHeader.m_Length )
			{
				return;
			}
			// �������ݰ�
			assert( m_CurrentPacket == 0 );
			m_CurrentPacket = PacketFactory::getSingleton().AllocPacket( m_PacketHeader.m_Length );
			if( m_CurrentPacket == 0 )
			{
				Delete();
				return;
			}
			// �ȸ�ֵ��Ϣͷ
			*m_CurrentPacket = m_PacketHeader;
			// �����и��������
			if( m_PacketHeader.m_Length > sizeof(PacketHeader) )
			{
				// ��ȡ������Ϣͷ֮��,���������
				if( !m_RecvPacketBuffer.Read( m_CurrentPacket+1, m_PacketHeader.m_Length-sizeof(PacketHeader) ) )
				{
					Delete();
					PacketFactory::getSingleton().FreePacket( m_CurrentPacket );
					m_CurrentPacket = 0;
					m_HaveReadPacketHeader = false;
					assert( false );
					return;
				}
			}
			// ������Ϣ
			if( m_EncryptPacketAPI )
				m_EncryptPacketAPI( (uint8*)m_CurrentPacket, m_CurrentPacket->m_Length, 3 );
			// ����Ƿ���Ping��Ϣ
			if( m_CurrentPacket->m_MainType == 255 && m_CurrentPacket->m_Length == sizeof(PacketPing))
			{
				if( m_CurrentPacket->m_SubType == 255 )
				{
					Pong( (PacketPing*)m_CurrentPacket );
				}
				else if( m_CurrentPacket->m_SubType == 254 )
				{
					PacketPong* packetPing = (PacketPong*)m_CurrentPacket;
					m_Ping = ::timeGetTime() - packetPing->m_Tick;
					m_LastPongTick = ::timeGetTime();
				}
				PacketFactory::getSingleton().FreePacket( m_CurrentPacket );
			}
			else
			{
				// �ڴ˶���Ϣ���н���
				m_PacketQueue.Push( m_CurrentPacket );	// push�����ݰ�����
			}
			m_CurrentPacket = 0;					// ����
			m_HaveReadPacketHeader = false;			// ����,׼����һ�����ݰ��Ķ�ȡ
		}
	}

	void Socket::Update()
	{
		vector<PacketHeader*> packets;
		m_PacketQueue.PopAll( packets );
		for ( int i=0; i<packets.size(); ++i )
		{
			if( m_SocketHandler )
				m_SocketHandler->OnProcessPacket( packets[i] );
			PacketFactory::getSingleton().FreePacket( packets[i] );
		}
		uint now = ::timeGetTime();
		// 1���Ӳ���һ��
		if( now >= m_LastSamplingTick + 1000 )
		{
			m_SendPerSecond = m_SendSamplingSize;
			m_RecvPerSecond = m_RecvSamplingSize;
			m_SendSamplingSize = m_RecvSamplingSize = 0;
			m_LastSamplingTick = now;
		}
		// 30���ӷ�һ��PING��Ϣ,��ֹ����
		if( m_ConnectorId > 0 )
		{
			if( now > m_LastPingTick + 1000*30 )
			{
				Ping();
				m_LastPingTick = now;
#ifdef _DEBUG
				// ��¼����־�ļ�,�����ҳ�����ԭ��
				Log fileLog( "Log/Ping.log", true );
				fileLog.WriteLog( "Ping: now=%u LastActiveTick=%u LastRecvTick=%u m_LastPingTick=%u\n",
					now, m_LastActiveTick, m_LastRecvTick, m_LastPingTick );
				fileLog.Close();
#endif
			}
		}
	}
}