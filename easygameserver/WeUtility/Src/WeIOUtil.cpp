#include "WeIOUtil.h"
#include "WeStringUtil.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <windows.h>
#include <direct.h>
#include <io.h>
#include <errno.h>


namespace We
{
	//---------------------------------------------------------------------
	bool IOUtil::WeCreateDirectory(const string& dirName )
	{
		if( _mkdir(dirName.c_str()) != 0 )
		{
			if( errno == EEXIST )
				return true;
			string tmp = dirName;
			StringUtil::StandardisePath(tmp);
			StringVector dirVector = StringUtil::Split( tmp, "/" );
			tmp = "";
			for(int i=0; i<dirVector.size(); ++i)
			{
				tmp += dirVector[i] +"/";
				if( !(tmp[tmp.length()-2] == ':' && tmp[tmp.length()-1] == '/') && _mkdir(tmp.c_str()) != 0 )
				{
					if( errno == EEXIST )
						continue;
					else
						return false;
				}
			}
		}
		return true;
	}
	//---------------------------------------------------------------------
	void IOUtil::WeRemoveDirectory(const string& dirName, const string& pattern/*="*.*"*/, bool recursive/*=true*/ )
	{
		string dir = StringUtil::StandardisePath( dirName );
		char tmpDir[MAX_PATH];
		getcwd( tmpDir, MAX_PATH );
		chdir( dir.c_str() );
		FindFiles(dir, pattern, recursive, RemoveDirectoryDelegate, 0 );
		FindFiles(dir, pattern, recursive, RemoveDirectoryDelegate, 0 );
		::RemoveDirectory(dir.c_str());
		chdir( tmpDir );
	}
	//---------------------------------------------------------------------
	bool IOUtil::WeCreateFile(const string& fileName )
	{
		int fh = creat(fileName.c_str(), _S_IREAD | _S_IWRITE); 
		if( fh == -1 )
		{
			if( errno == ENOENT )
			{
				string tmp = fileName;
				std::replace( tmp.begin(), tmp.end(), '\\', '/' );
				int pos = tmp.find_last_of('/');
				string dir = tmp.substr( 0, pos );
				WeCreateDirectory(dir);
				fh = creat(fileName.c_str(), _S_IREAD | _S_IWRITE);
			}
		}
		if( fh != -1 )
		{
			_close(fh);
			return true;
		}
		return false;
	}
	//---------------------------------------------------------------------
	bool IOUtil::WeCopyFile(const string& existingFile, const string& newFile, bool overwrite, bool autoCreateDirectory )
	{
		if( !overwrite && Exists(newFile.c_str()) )
			return false;
		if( ::CopyFile(existingFile.c_str(), newFile.c_str(), overwrite?FALSE:TRUE) == 0 )
		{
			int err = ::GetLastError();
			if( err == 3 ) //系统找不到指定的路径
			{
				if( !Exists(existingFile) )
					return false;
				if( !autoCreateDirectory )
					return false;
				string tmp = newFile;
				std::replace( tmp.begin(), tmp.end(), '\\', '/' );
				int pos = tmp.find_last_of('/');
				string dir = tmp.substr( 0, pos );
				WeCreateDirectory(dir);	//创建目录
				if( ::CopyFile(existingFile.c_str(), newFile.c_str(), overwrite?FALSE:TRUE) == 0 )
					return false;
			}
		}
		return true;
	}
	//---------------------------------------------------------------------
	bool IOUtil::WeCopyDirectory(const string& existingDir, const string& newDir, bool overwrite/*=true*/, const string& pattern/*="*.*"*/, bool recursive/*=true*/ )
	{
		StringVector files = GetFiles(existingDir, pattern, recursive);
		string tmp = existingDir;
		StringUtil::StandardisePath(tmp);
		int dirLen = tmp.length();
		tmp = newDir;
		StringUtil::StandardisePath(tmp);
		string relativeFileName;
		bool ret = true;
		for(int i=0; i<files.size(); ++i)
		{
			string ttmp = files[i];
			relativeFileName = files[i].substr(dirLen, files[i].length()-dirLen);
			if( !WeCopyFile( files[i], newDir+relativeFileName, overwrite, true ) )
				ret = false;
		}
		return ret;
	}
	//---------------------------------------------------------------------
	bool IOUtil::Exists(const string& fileName )
	{
		struct stat tagStat;
		return (stat(fileName.c_str(), &tagStat) == 0);
	}
	//---------------------------------------------------------------------
	StringVector IOUtil::GetFiles(const string& dirName, const string& pattern, bool recursive/* =true */ )
	{
		string dir = StringUtil::StandardisePath( dirName );
		StringVector ret;
		if( !Exists(dirName ) )
			return ret;
		char tmpDir[MAX_PATH];
		getcwd( tmpDir, MAX_PATH );
		chdir( dir.c_str() );
		FindFiles( ret, dir, pattern, recursive );
		chdir( tmpDir );
		return ret;
	}
	//---------------------------------------------------------------------
	StringVector IOUtil::GetDirectories(const string& dirName, bool recursive/* =true */ )
	{
		string dir = StringUtil::StandardisePath( dirName );
		StringVector ret;
		char tmpDir[MAX_PATH];
		getcwd( tmpDir, MAX_PATH );
		chdir( dir.c_str() );
		FindDirs( ret, dir, recursive );
		chdir( tmpDir );
		return ret;
	}
	//---------------------------------------------------------------------
	void IOUtil::FindFiles(StringVector& fileList, const string& dirName, const string& pattern, bool recursive/*=true*/ )
	{
		long lHandle, res;
		struct _finddata_t tagData;
#pragma warning( disable : 4244 )
		lHandle = _findfirst(pattern.c_str(), &tagData);
		res = 0;
		while (lHandle != -1 && res != -1)
		{
			if(!(tagData.attrib & _A_SUBDIR))
			{
				fileList.push_back( dirName + tagData.name );
			}
			res = _findnext( lHandle, &tagData );
		}
		// Close if we found any files
		if(lHandle != -1)
		{
			_findclose(lHandle);
		}

		// Now find directories
		if (recursive)
		{
#pragma warning( disable : 4244 )
			lHandle = _findfirst("*", &tagData);
			res = 0;
			while (lHandle != -1 && res != -1)
			{
				if((tagData.attrib & _A_SUBDIR)
					&& strcmp(tagData.name, ".")
					&& strcmp(tagData.name, ".."))
				{
					// recurse
					string dir = dirName + tagData.name + "/";
					char tmpDir[MAX_PATH];
					getcwd( tmpDir, MAX_PATH );
					chdir( (string(tmpDir)+"/"+tagData.name).c_str() );
					FindFiles( fileList,dir,pattern, recursive );
					chdir(tmpDir);
				}
				res = _findnext( lHandle, &tagData );
			}
			// Close if we found any files
			if(lHandle != -1)
			{
				_findclose(lHandle);
			}

		}
	}
	//---------------------------------------------------------------------
	void IOUtil::FindDirs(StringVector& fileList, const string& dirName, bool recursive/*=true*/ )
	{
		long lHandle, res;
		struct _finddata_t tagData;
#pragma warning( disable : 4244 )
		lHandle = _findfirst("*.*", &tagData);
		res = 0;
		while (lHandle != -1 && res != -1)
		{
			if((tagData.attrib & _A_SUBDIR)
				&& strcmp(tagData.name, ".")
				&& strcmp(tagData.name, ".."))
			{
				fileList.push_back( dirName + tagData.name );
			}
			res = _findnext( lHandle, &tagData );
		}
		// Close if we found any files
		if(lHandle != -1)
		{
			_findclose(lHandle);
		}

		// Now find directories
		if (recursive)
		{
#pragma warning( disable : 4244 )
			lHandle = _findfirst("*", &tagData);
			res = 0;
			while (lHandle != -1 && res != -1)
			{
				if((tagData.attrib & _A_SUBDIR)
					&& strcmp(tagData.name, ".")
					&& strcmp(tagData.name, ".."))
				{
					// recurse
					string dir = dirName + tagData.name + "/";
					char tmpDir[MAX_PATH];
					getcwd( tmpDir, MAX_PATH );
					chdir( dir.c_str() );
					FindDirs( fileList,dir, recursive );
					chdir(tmpDir);
				}
				res = _findnext( lHandle, &tagData );
			}
			// Close if we found any files
			if(lHandle != -1)
			{
				_findclose(lHandle);
			}

		}
	}
	//----------------------------------------------------------------------
	string IOUtil::GetFileName(const string& fullFileName )
	{
		int pos = fullFileName.find_last_of( '/' );
		if( pos == std::string::npos )
		{
			pos = fullFileName.find_last_of( '\\' );
		}
		if( pos != std::string::npos )
		{
			return fullFileName.substr( pos+1, fullFileName.length()-pos-1 );
		}
		return fullFileName;
	}
	//----------------------------------------------------------------------
	string IOUtil::GetFilePath(const string& fullFileName )
	{
		int pos = fullFileName.find_last_of( '/' );
		if( pos == std::string::npos )
		{
			pos = fullFileName.find_last_of( '\\' );
		}
		if( pos != std::string::npos )
		{
			return fullFileName.substr( 0, pos );
		}
		return "";
	}
	//----------------------------------------------------------------------
	string IOUtil::GetFileExt(const string& fullFileName )
	{
		int pos = fullFileName.find_last_of( '.' );
		if( pos != std::string::npos )
		{
			return fullFileName.substr( pos+1, fullFileName.length()-pos-1 );
		}
		return "";
	}
	//---------------------------------------------------------------------
	string IOUtil::WeGetCurrentDirectory()
	{
		char tmpDir[MAX_PATH];
		getcwd( tmpDir, MAX_PATH );
		return string(tmpDir);
	}
	//---------------------------------------------------------------------
	string IOUtil::GetModuleDirectory()
	{
		char tmpDir[MAX_PATH];
		GetModuleFileName( 0, tmpDir, MAX_PATH );
		string ret(tmpDir);
		ret.erase(ret.find_last_of('\\')+1);
		return ret;
	}
	//---------------------------------------------------------------------
	void IOUtil::WeSetWorkDirectory(const string& workDirectory )
	{
		chdir(workDirectory.c_str());
	}
	//----------------------------------------------------------------------
	bool IOUtil::IsDirectory(const string& filename )
	{
		bool ret = false;
		long lHandle;
		struct _finddata_t tagData;
#pragma warning( disable : 4244 )
		lHandle = _findfirst(filename.c_str(), &tagData);
		if( lHandle != -1 )
		{
			ret = ( (tagData.attrib & _A_SUBDIR) == 1 );
			_findclose(lHandle);
		}
		return ret;
	}
	//---------------------------------------------------------------------
	bool IOUtil::IsReadonly(const string& fileName )
	{
		bool ret = false;
		long lHandle;
		struct _finddata_t tagData;
#pragma warning( disable : 4244 )
		lHandle = _findfirst(fileName.c_str(), &tagData);
		if( lHandle != -1 )
		{
			ret = (tagData.attrib & _A_RDONLY);
			_findclose(lHandle);
		}
		return ret;
	}
	//---------------------------------------------------------------------
	void IOUtil::SetReadonly(const string& fileName, bool readonly)
	{
		DWORD attr = ::GetFileAttributes(fileName.c_str());
		attr = readonly?attr|FILE_ATTRIBUTE_READONLY:attr&(~FILE_ATTRIBUTE_READONLY);
		::SetFileAttributes(fileName.c_str(), attr);
	}
	//---------------------------------------------------------------------
	void IOUtil::FindFiles(const string& dirName, const string& pattern, bool recursive, FindFileDelegate findFileDelegate, void* outArgs)
	{
		long lHandle, res;
		struct _finddata_t tagData;
#pragma warning( disable : 4244 )
		lHandle = _findfirst(pattern.c_str(), &tagData);
		res = 0;
		while (lHandle != -1 && res != -1)
		{
			if(!(tagData.attrib & _A_SUBDIR))
			{
				findFileDelegate(dirName+tagData.name, false, tagData, outArgs);//delegate
			}
			res = _findnext( lHandle, &tagData );
		}
		// Close if we found any files
		if(lHandle != -1)
		{
			_findclose(lHandle);
		}

		// Now find directories
		if (recursive)
		{
#pragma warning( disable : 4244 )
			lHandle = _findfirst("*", &tagData);
			res = 0;
			while (lHandle != -1 && res != -1)
			{
				if((tagData.attrib & _A_SUBDIR)
					&& strcmp(tagData.name, ".")
					&& strcmp(tagData.name, ".."))
				{
					// recurse
					string dir = dirName + tagData.name + "/";

					char tmpDir[MAX_PATH];
					getcwd( tmpDir, MAX_PATH );
					chdir( dir.c_str() );
					FindFiles( dir,pattern, recursive, findFileDelegate, outArgs );
					findFileDelegate(dirName, true, tagData, outArgs);//delegate
					chdir(tmpDir);
				}
				res = _findnext( lHandle, &tagData );
			}
			// Close if we found any files
			if(lHandle != -1)
			{
				_findclose(lHandle);
			}
		}
	}
	//---------------------------------------------------------------------
	void IOUtil::FindFileCustom(const string& dirName, const string& pattern, bool recursive, FindFileDelegate findFileDelegate, void* outArgs)
	{
		string dir = StringUtil::StandardisePath( dirName );
		char tmpDir[MAX_PATH];
		getcwd( tmpDir, MAX_PATH );
		chdir( dir.c_str() );
		FindFiles(dir, pattern, recursive, findFileDelegate, outArgs);
		chdir( tmpDir );
	}
	//---------------------------------------------------------------------
	void IOUtil::RemoveDirectoryDelegate(const string& fileName, bool isDirectory, const struct _finddata_t& fileAttr, void* out)
	{
		if(isDirectory)
		{
			if( !::RemoveDirectory(fileName.c_str()) )
			{
				SetReadonly(fileName, false);
				::RemoveDirectory(fileName.c_str());
			}
		}
		else
		{
			if( !::DeleteFile(fileName.c_str()) )
			{
				SetReadonly(fileName, false);
				::DeleteFile(fileName.c_str());
			}
		}
	}
	//---------------------------------------------------------------------

}
