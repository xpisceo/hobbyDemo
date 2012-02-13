#include "WeFileLoader.h"
#include "WeEncrypt.h"
#include "TinyXml/tinyxml.h"
#include <Windows.h>

namespace We
{
	//------------------------------------------------------------------------
	unsigned char* FileLoader::LoadFile(const std::string& fileName, uint32& dataSize, bool isEncrypt )
	{
		HANDLE hF = ::CreateFile( fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
		if(hF == INVALID_HANDLE_VALUE)
		{
			return 0;
		}
		dataSize = ::GetFileSize(hF, 0);
		uint32 realDataSize = dataSize;
		//// ����,��֤��8�ı���
		//if( (dataSize%8) != 0 )
		//	dataSize += 8-(dataSize%8);
		//dataSize += 8;	// ������8���ֽ�,����������(uint64)
		unsigned char* encodeData = new unsigned char[dataSize];
		DWORD bytes;
		if( ::ReadFile(hF, encodeData, realDataSize, &bytes, NULL ) == 0 )
		{
			CloseHandle(hF);
			delete []encodeData;
			return 0;
		}
		CloseHandle(hF);

		// ����Ĳ����ֽ���0
		::memset( encodeData + realDataSize, 0, dataSize-realDataSize );

		if( !isEncrypt )
		{
			dataSize = realDataSize;
			return encodeData;
		}
		else
		{
			// ��ȷ�ļ����ļ�������8�ı������Ҵ���8
			assert( (dataSize%8) == 0 && dataSize > 8 );
			if( (dataSize%8) != 0 || dataSize <= 8 )
				return 0;
			// ����
			We::Encrypt encrypt;
			encrypt.SetDefaultKey();
			int decodeDataSize = dataSize;
			unsigned char* decodeData = new unsigned char[decodeDataSize];
			encrypt.Decode( encodeData, dataSize, decodeData );
			delete []encodeData;
			// ���8���ֽڱ�ʾԭʼ�����ݳ���(uint64)
			uint64 originalSize = *((uint64*)(decodeData + (decodeDataSize-8)));
			assert( originalSize > 0 );
			dataSize = (uint32)originalSize;
			return decodeData;
		}
		return 0;
	}
	//------------------------------------------------------------------------
	bool FileLoader::LoadFile(We::TiXmlDocument* xmlDoc, const std::string& path, bool isEncrypt )
	{
		unsigned char* data = 0;
		uint32 dataSize = 0;
		data = FileLoader::LoadFile( path, dataSize, isEncrypt );
		if( data == 0 )
			return false;
		xmlDoc->Parse( (const char*)data );
		delete []data;
		return !xmlDoc->Error();
	}
	//------------------------------------------------------------------------
	bool FileLoader::SaveFile(const std::string& path, const char* data, uint32 dataSize, bool isEncrypt )
	{
		unsigned char* saveData = (unsigned char*)data;
		if( dataSize < 1)
			return false;
		int saveDataSize = dataSize;
		/// ����
		if( isEncrypt )
		{
			// saveDataSize������8�ı���
			assert( (saveDataSize%8) == 0 );
			We::Encrypt encrypt;
			encrypt.SetDefaultKey();
			saveDataSize = encrypt.GetOutputSize( saveDataSize );
			saveData = new unsigned char[saveDataSize];
			encrypt.Encode( (unsigned char*)data, dataSize, saveData );
		}
		FILE* pf = ::fopen( path.c_str(), "wb+" );
		::fwrite( saveData, 1, saveDataSize, pf );
		::fclose( pf );
		if( isEncrypt )
		{
			delete []saveData;
		}
		return true;
	}
	//------------------------------------------------------------------------
}