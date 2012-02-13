/********************************************************************
	created:	2009-6-27
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�ڲ���Ϣ��һЩ��������
*********************************************************************/
#pragma once

namespace We
{
	const uint32 MAX_ACCOUNT_NAME = 16;			/// �ʺų���(������'\0')
	const uint32 MAX_ACCOUNT_PWD = 32;			/// �˺����볤��
	const uint32 MAX_ENCRYPT_PWD = 32;			/// ���ܺ�����볤��
	const uint32 MAX_ROLE_NAME = 14;			/// ��ɫ������(������'\0')
	const uint32 MAX_ROLE_NUM = 5;				/// һ���˺ŵĽ�ɫ��Ŀ����
	const uint32 MAX_CREATEROLE_SIZE = 1024;	/// ������ɫʱ,����Ҫ�Ļ����С	
	const uint32 MAX_ROLELIST_SIZE = 1024*10;	/// ��ɫ�б����,������н�ɫ��Ϣ����󻺴��С
	const uint32 MAX_ROLE_CACHE_SIZE = 20*1024;	/// ��ɫ���ݵ���󻺴��С
	const uint32 MAX_ROLE_DATA_SIZE = 1024*20;	/// ��ɫ���ݵ���󻺴��С
	const uint32 MAX_MAPCOUNT_ONEMAPSERVER = 32;/// һ̨��ͼ�������ϵ�����ͼ����
	const uint32 MAX_MAP_NAME = 32;				/// ��ͼ���ֵĳ���
	const uint32 MAX_MAPLIST_DATA_SIZE = 1024;  /// �����б����ݵ���󳤶�

	typedef uint32 PlayerID;

	struct Position
	{
		float x;
		float y;
		float z;
	};
}