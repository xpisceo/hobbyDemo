/********************************************************************
	created:	2009-6-27
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ط�����
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"
#include "WeCenterHandler.h"
#include "WeMapHandler.h"

namespace We
{
	class ClientHandler;

	class GateServer : public SocketListenerHandler
	{
	public:
		unsigned int m_ClientHandlerId;

		unsigned int m_TotalRecvMsgSizeSec;
		unsigned int m_TotalRecvMsgSize;
		unsigned int m_TotalSendMsgSizeSec;
		unsigned int m_TotalSendMsgSize;

		unsigned int m_LastSecTick;
		unsigned int m_LastShowTick;
		unsigned int m_LastSendTick;

		uint8	m_GateServerId;
		string	m_ClientListenIP;
		uint16	m_ClientListenPort;
		uint32	m_MaxClientConnection;

		string	m_CenterServerIP;
		uint16	m_CenterServerPort;
	protected:
		map<uint32, ClientHandler*> m_LogingClientHandlers;		/// ��¼�еĿͻ���(�����ɵ�Ψһid��Ϊ��ʶ)
		map<uint32, ClientHandler*> m_ClientHandlers;			/// �Ѿ���¼�Ŀͻ���(�Խ�ɫid��Ϊ��ʶ)

		CenterHandler	m_CenterServer;							/// �����ķ�����������
		map<uint16, MapHandler*>	m_MapServers;				/// ���ͼ������������
	public:
		GateServer();
		~GateServer();

		inline uint8 GetGateServerId() const { return m_GateServerId; }

		bool Init();
		void Shutdown();

		/// �¿ͻ�������
		virtual void OnAccept( Socket* socket );
		/// �Ƴ��ͻ�������
		virtual void OnRemove( Socket* socket );

		/// �����ݰ������ķ�����
		bool SendToCenter( PacketHeader* packet );

		/// �����ݰ�����ͼ������
		bool SendToMap( uint16 mapId, PacketHeader* packet );

		/// ����һ����ͼ������
		void OnAddMapServer( PacketHeader* packet );

		/// �Ƴ�һ����ͼ������
		void OnRemoveMapServer( PacketHeader* packet );

		/// �ͻ��˵�¼�������
		void OnClientLogin( PacketHeader* packet );

		/// �ͻ���ѡ���˽�ɫ
		void OnClientChooseRole( PacketHeader* packet );

		/// ������Ϸ����
		void OnClientEnterWorld( PacketHeader* packet );

		/// ת����¼�ڵ���Ϣ->Client
		bool DispatchLoginPacket( PacketHeader* packet );

		/// ת����Ϸ�е���Ϣ->Client
		bool DispatchPacket( PacketHeader* packet );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}