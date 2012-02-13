/********************************************************************
	created:	2009-7-12
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�߼����ֵ���ҽṹ
*********************************************************************/
#pragma once

#include "WePlayerStruct_Inner.h"
#include <string>
using namespace std;

namespace We
{
//#pragma pack(1)
/// �ṹ�汾
#define PLAYER_STRUCT_VERSION 1

	/// ������ɫ
	struct SCreateRoleInfo
	{
		uint8	m_RoleIndex;					/// ��ɫ���� (size=1)
		char	m_RoleName[MAX_ROLE_NAME+1];	/// ��ɫ�� (size=15)

		/// size=4
		uint32  m_Sex:2;						/// �Ա�(0-1)
		uint32  m_Race:4;						/// ����(0-15)
		uint32  m_FaceId:8;						/// ����(0-255)
		uint32  m_HairId:8;						/// ����(0-255)
		uint32  _dummy1:10;						/// 4�ֽڶ���
	};

	/// ��ҵ����
	struct SPlayerVisual
	{
		uint32  m_FaceId:8;						/// ����(0-255)
		uint32  m_HairId:8;						/// ����(0-255)
		uint32  _dummy1:16;						/// 4�ֽڶ���
	};

	/// ��ɫ�б��õ��Ľ�ɫ����
	struct SRoleList_RoleInfo
	{
		uint8	m_RoleIndex;					/// ��ɫ���� (size=1)
		char	m_RoleName[MAX_ROLE_NAME+1];	/// ��ɫ�� (size=15)
		uint32  m_RoleId;						/// ��ɫid (size=4)

		/// size=4
		uint32  m_Sex:2;						/// �Ա�(0-1)
		uint32  m_Race:4;						/// ����(0-15)
		uint32  _dummy1:26;						/// 4�ֽڶ���

		uint32  m_OnlineTime;					/// ����ʱ��(����) (size=4)

		SPlayerVisual m_Visual;					/// ��ҵ����
	};

	/// ��ҵı���
	struct SPlayerBag
	{
		uint8  m_Data[1024];
	};

	/// ��ҵĲֿ�
	struct SPlayerStorage
	{
		uint8  m_Data[2048];
	};

	/// ���浽���ݿ������
	//struct SRoleInfo_Header
	//{
	//	char		m_RoleName[MAX_ROLE_NAME+1];
	//	uint32		m_RoleId;
	//	uint8		m_RoleIndex;
	//	uint16		m_MapId;
	//	Position	m_Pos;
	//};
	struct SRoleInfo_DB : SRoleInfo_Header
	{
		/// size=4
		uint32  m_Sex:2;						/// �Ա�(0-1)
		uint32  m_Race:4;						/// ����(0-15)
		uint32  _dummy1:26;						/// 4�ֽڶ���

		uint32  m_OnlineTime;					/// ����ʱ��(����) (size=4)

		SPlayerVisual m_Visual;					/// ��ҵ����
		SPlayerBag	  m_Bag;					/// ����
		uint8		  m_Data[4096];				/// ��������
	};

	struct SRoleInfo_Client
	{
		uint32		  m_RoleId;
		Position	  m_Pos;
		uint16		  m_MapId;
		uint16		  _dummy1;					/// 4�ֽڶ���
		SPlayerVisual m_Visual;					/// ��ҵ����
		SPlayerBag	  m_Bag;					/// ����
	};

//#pragma pack()
}
