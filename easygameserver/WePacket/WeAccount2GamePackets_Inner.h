/********************************************************************
	created:	2009-6-30
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�ʺŷ�����������Ϸ���������ڲ���Ϣ
*********************************************************************/
#pragma once

#include "WePacketHeader.h"
#include "WePacketDefine_Inner.h"

namespace We
{
#pragma pack(1)

	enum PacketType_Account2GameServer_Inner
	{
		PT_A2GS_Inner_CreateAccountResult = 1,		/// �����ʺŷ���
		PT_A2GS_Inner_LoginResult,					/// �ʺŵ�¼����

		PT_A2GS_Inner_MAX,
	};

#define PACKET_A2GS_INNER_BEG( packetName ) \
	struct Packet_A2GS_Inner_##packetName## : We::PacketHeader\
	{\
	Packet_A2GS_Inner_##packetName##() { m_MainType=0; m_SubType = PT_A2GS_Inner_##packetName##; m_Length=sizeof( Packet_A2GS_Inner_##packetName## ); }

#define PACKET_A2GS_INNER_END };

	PACKET_A2GS_INNER_BEG( CreateAccountResult )
		enum CreateAccountResult
		{
			CreateAccountResult_Unknown = 0,
			CreateAccountResult_OK,
			CreateAccountResult_AccountExist,	/// �ʺ��Ѿ�����
			CreateAccountResult_SQLError,		/// ���ݿ����
		};
		uint32	m_ClientId;
		uint32	m_AccountId; 
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		uint8	m_Result;
	PACKET_A2GS_INNER_END

	PACKET_A2GS_INNER_BEG( LoginResult )
		enum LoginResult
		{
			LoginResult_Unknown = 0,
			LoginResult_OK,
			LoginResult_OKAndReconnect,		/// ��֤OK,�ȴ���������
			LoginResult_AccountOrPwdError,	/// �ʺŻ��������
			LoginResult_MibaoError,			/// �ܱ�������
		};
		uint32	m_ClientId;
		uint32	m_AccountId; 
		char	m_AccountName[MAX_ACCOUNT_NAME+1];
		uint8	m_Result;
	PACKET_A2GS_INNER_END


#pragma pack()
}
