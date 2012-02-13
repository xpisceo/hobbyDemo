/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	加载和保存文件,带加密解密功能
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
		/// 加载文件
		static unsigned char* LoadFile( const string& path, uint32& dataSize, bool isEncrypt);

		/// 加载xml文件
		static bool LoadFile( We::TiXmlDocument* xmlDoc, const std::string& path, bool isEncrypt );

		/// 保存文件
		static bool SaveFile( const std::string& path, const char* data, uint32 dataSize, bool isEncrypt );

	};
}