/********************************************************************
	created:	2009-6-28
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ķ�����
*********************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

#include "WoSocketHandler.h"
#include "WeCenterPlayer.h"
#include "WeAccountHandler.h"
#include "WeDbHandler.h"

namespace We
{
	class CenterServer;
	class GateHandler;
	class MapHandler;
	class AccountHandler;

	class MapServerListener : public SocketListenerHandler
	{
	protected:
		CenterServer* m_CenterServer;
	public:
		MapServerListener( CenterServer* centerServer );

		/// �µ�ͼ����������
		virtual void OnAccept( Socket* socket );
		/// �Ƴ���ͼ����������
		virtual void OnRemove( Socket* socket );
	};

	class CenterServer : public SocketListenerHandler
	{
	public:
		uint32 m_CenterId;

		string	m_GateListenIP;
		uint16	m_GateListenPort;
		uint32	m_GateMaxConnection;

		string	m_MapListenIP;
		uint16	m_MapListenPort;
		uint32	m_MapMaxConnection;

		string	m_AccountServerIP;
		uint16  m_AccountServerPort;

		string	m_DbServerIP;
		uint16  m_DbServerPort;

		MapServerListener* m_MapServerListener;

		AccountHandler				m_AccountHandler;	/// ���ʺŷ�����������
		DbHandler					m_DbHandler;		/// ��DB������������
		map<uint8, GateHandler*>	m_GateHandlers;		///	���ط�����������(ע:�������������map)
		map<uint16, MapHandler*>	m_MapHandlers;		/// ��ͼ������������(ע:�������������map)
		MapHandler**				m_Maps;				/// ��ͼ����
		uint32						m_MapArraySize;		/// ��ͼ����Ĵ�С
		uint32						m_MaxMapId;			/// ��ǰ����MapId

		map<uint32, LoginPlayer*>	m_LoginPlayers;		/// ��¼�б�(�ʺ�id��Ϊ�ؼ���)(ע:������HashTable����map)
		map<uint32, CenterPlayer*>	m_CenterPlayers;	/// ��Ϸ�е����(��ɫid��Ϊ�ؼ���)(ע:������HashTable����map)
	public:
		CenterServer();
		~CenterServer();

		bool Init();
		void Shutdown();

		/// �����ط���������
		virtual void OnAccept( Socket* socket );
		/// �Ƴ����ط���������
		virtual void OnRemove( Socket* socket );

		/// �µ�ͼ����������
		virtual void OnAcceptMapServer( Socket* socket );
		/// �Ƴ���ͼ����������
		virtual void OnRemoveMapServer( Socket* socket );

		bool SendToGate( uint8 gateId, PacketHeader* packet );
		void SendToAllGates( PacketHeader* packet );

		bool SendToMap( uint16 mapId, PacketHeader* packet );
		void SendToAllMaps( PacketHeader* packet );

		/// �յ����ط�������������Ϣ
		void OnRecv_GateInfo( GateHandler* gateHandler, PacketHeader* packet );
		/// �յ���ͼ��������������Ϣ
		void OnRecv_MapServerInfo( MapHandler* mapHandler, PacketHeader* packet );

		LoginPlayer* GetLoginPlayer( uint32 accountId );
		CenterPlayer* GetCenterPlayerById( uint32 roleId );
		CenterPlayer* GetCenterPlayerByAccountId( uint32 accountId );

		void RemoveLoginPlayer( uint32 accountId );

		/// �ʺŵ�¼
		void OnRecv_Login( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_LoginResult( PacketHeader* packet );

		void OnRecv_CreateAccount( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_CreateAccountResult( PacketHeader* packet );

		void OnRecv_RoleList( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_CreateRole( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_ChooseRole( GateHandler* gateHandler, PacketHeader* packet );
		void OnRecv_DeleteRole( GateHandler* gateHandler, PacketHeader* packet );

		void OnRecv_RoleListResult( PacketHeader* packet );
		void OnRecv_CreateRoleResult( PacketHeader* packet );
		void OnRecv_ChooseRoleResult( PacketHeader* packet );
		void OnRecv_DeleteRoleResult( PacketHeader* packet );
		void OnRecv_EnterWorld( PacketHeader* packet );

		void Update();

		void OnInputCommand( const char* cmd );
	};
}