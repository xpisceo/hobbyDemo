/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���ܽ���
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	class We_EXPORTS Encrypt
	{
	public:
		Encrypt();
		~Encrypt();

		/// �����ܳ�
		void SetKey( unsigned char key[], int keybytes );

		/// ����Ĭ���ܳ�
		void SetDefaultKey();

		int GetOutputSize( int inputSize );

		/// ����
		void Encode( unsigned char* inputData, int inputDataSize, unsigned char* outputData );

		/// ����
		void Decode( unsigned char* inputData, int inputDataSize, unsigned char* outputData );

		///// ����, ���ؼ��ܺ������
		//unsigned char* Encode( unsigned char* inputData, int inputDataSize, int& outputDataSize );

		///// ����, ���ؽ��ܺ������
		//unsigned char* Decode( unsigned char* inputData, int inputDataSize, int& outputDataSize );

	protected:
		void* m_Encrypt;
	};
}
