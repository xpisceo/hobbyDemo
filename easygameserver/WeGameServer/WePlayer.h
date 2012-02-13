/********************************************************************
	created:	2009-7-21
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WePlayerStruct.h"
#include "WePacketHeader.h"
#include "WeMapObject.h"

namespace We
{
	class SocketHandler;
	class Map;
	class Cell;

	/// ��ҵĲ��������ݿ������
	struct SPlayerData : SRoleInfo_DB
	{
	};

	class Player : public MapObject
	{
		friend class World;
		friend class GameServer;
	protected:
		uint32  m_AccountId;
		uint32	m_RoleId;
		string	m_RoleName;
		SPlayerData m_Data;

		SocketHandler* m_SocketHandler;

	public:
		uint16	m_DataChecker;	/// ���ݱ仯��ʶ,Ŀǰֻ�����������
	public:
		Player();
		virtual ~Player();

		virtual bool IsPlayer() const { return true; }

		uint32 GetAccountId() const { return m_AccountId; }
		uint32 GetRoleId() const { return m_RoleId; }
		const string& GetRoleName() const { return m_RoleName; }

		bool SendPacket( const PacketHeader* packet );

		/// ������Ϸ����
		void OnFirstTimeEnterWorld();
		void OnEnterWorld();

		/// ��������
		void OnReconnect();

		/// ���������Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_EnterView();
		/// �뿪�����Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_LeaveView();

		virtual PacketHeader* GetPacket_EnterWorld();

		void OnRecv_Packet( PacketHeader* packet );
	};
}