/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	文件的一些操作
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include "WeStringUtil.h"
#include <io.h>

namespace We
{
	class We_EXPORTS IOUtil
	{
	public:
		typedef void (*FindFileDelegate)(const string& fileName, bool isDirectory, const struct _finddata_t& fileAttr, void* out);

		static bool WeCreateDirectory(const string& dirName );
		static void WeRemoveDirectory(const string& dirName, const string& pattern="*.*", bool recursive=true  );
		static bool WeCreateFile(const string& fileName );
		static bool Exists(const string& fileName );
		static bool WeCopyFile(const string& existingFile, const string& newFile, bool overwrite, bool autoCreateDirectory );
		//不复制空文件夹
		static bool WeCopyDirectory(const string& existingDir, const string& newDir, bool overwrite=true, const string& pattern="*.*", bool recursive=true );

		static StringVector GetFiles(const string& dirName, const string& pattern="*.*", bool recursive=true );
		static StringVector GetDirectories(const string& dirName, bool recursive=true);

		static string GetFileName(const string& fullFileName );
		static string GetFilePath(const string& fullFileName );
		static string GetFileExt(const string& fullFileName );
		static string WeGetCurrentDirectory();
		static void   WeSetWorkDirectory(const string& workDirectory );
		static string GetModuleDirectory();

		static bool   IsDirectory(const string& filename );
		static bool   IsReadonly(const string& fileName );
		static void   SetReadonly(const string& fileName, bool readonly);

		static void FindFileCustom(const string& dirName, const string& pattern, bool recursive, FindFileDelegate findFileDelegate, void* outArgs);

	private:
		static void FindFiles(StringVector& fileList, const string& dirName, const string& pattern, bool recursive=true );
		static void FindDirs(StringVector& fileList, const string& dirName, bool recursive=true);

		static void FindFiles(const string& dirName, const string& pattern, bool recursive, FindFileDelegate findFileDelegate, void* outArgs);
		static void RemoveDirectoryDelegate(const string& fileName, bool isDirectory, const struct _finddata_t& fileAttr, void* out);
	};

}
