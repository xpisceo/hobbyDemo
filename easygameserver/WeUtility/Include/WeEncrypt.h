/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	加密解密
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

		/// 设置密匙
		void SetKey( unsigned char key[], int keybytes );

		/// 设置默认密匙
		void SetDefaultKey();

		int GetOutputSize( int inputSize );

		/// 加密
		void Encode( unsigned char* inputData, int inputDataSize, unsigned char* outputData );

		/// 解密
		void Decode( unsigned char* inputData, int inputDataSize, unsigned char* outputData );

		///// 加密, 返回加密后的数据
		//unsigned char* Encode( unsigned char* inputData, int inputDataSize, int& outputDataSize );

		///// 解密, 返回解密后的数据
		//unsigned char* Decode( unsigned char* inputData, int inputDataSize, int& outputDataSize );

	protected:
		void* m_Encrypt;
	};
}
