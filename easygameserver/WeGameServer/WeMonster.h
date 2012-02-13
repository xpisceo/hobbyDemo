/********************************************************************
	created:	2009-8-2
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	����
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeMapObject.h"

namespace We
{
	class Map;
	class MapCell;

	class Monster : public MapObject
	{
		friend class Map;
	protected:
		uint16		m_MonsterId;	/// ����id
		Position	m_BornPos;		/// ������
		float		m_MoveRange;	/// ��뾶

	public:
		Monster();
		virtual ~Monster();

		virtual bool IsMonster() const { return true; }

		/// ���������Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_EnterView();
		/// �뿪�����Ұʱ,��Ҫ�����ͬ��������
		virtual PacketHeader* GetSyncPacket_LeaveView();

		virtual void Update();
	};
}