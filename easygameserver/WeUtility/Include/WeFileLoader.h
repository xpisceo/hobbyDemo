/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	���غͱ����ļ�,�����ܽ��ܹ���
*********************************************************************/
#pragma once

#include <string>
using namespace std;

#include "WeConfig.h"

namespace We
{
	class TiXmlDocument;

	class We_EXPORTS FileLoader
	{
	public:
		/// �����ļ�
		static unsigned char* LoadFile( const string& path, uint32& dataSize, bool isEncrypt);

		/// ����xml�ļ�
		static bool LoadFile( We::TiXmlDocument* xmlDoc, const std::string& path, bool isEncrypt );

		/// �����ļ�
		static bool SaveFile( const std::string& path, const char* data, uint32 dataSize, bool isEncrypt );

	};
}