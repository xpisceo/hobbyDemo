#include "WeStringParser.h"
#include "WeStringUtil.h"
#include "WeFileLoader.h"
#include "WeLogger.h"
#include "TinyXml/tinyxml.h"

namespace We
{
    Singleton_Implement( StringParser );
	//------------------------------------------------------------------------
    StringParser::StringParser()
	{
	}
	//------------------------------------------------------------------------
	StringParser::~StringParser()
	{
		Clear();
	}
	void StringParser::Initialize( unsigned int size )
	{
		m_StringItems.Initialize( size );
	}
	//------------------------------------------------------------------------
	bool StringParser::LoadStringItems( const string& fileList, const string& directory, bool isEncrypt )
	{
		TiXmlDocument doc;
		if( !FileLoader::LoadFile( &doc, fileList, isEncrypt ) )
		{
			Assert(0);
			_LogError( string("StringParser::LoadStringItems : ") + fileList + "加载失败." );
			return false;
		}
		TiXmlElement* root = doc.FirstChildElement( "StringList" );
		if( root == 0 )
		{
			doc.Clear();
			Assert(0);
			_LogError( string("StringParser::LoadStringItems : ") + fileList + "加载失败." );
			return false;
		}
		TiXmlElement* entityNode = root->FirstChildElement( "StringFile" );
		for( entityNode; entityNode; entityNode=entityNode->NextSiblingElement("StringFile") )
		{
			const char* fileName = entityNode->Attribute( "File" );
			LoadStringItem( directory + "\\" + fileName, isEncrypt );
		}
		return true;
	}
	bool StringParser::LoadStringItem( const string& file, bool isEncrypt )
	{
		TiXmlDocument doc;
		if( !FileLoader::LoadFile( &doc, file, isEncrypt ) )
		{
			Assert(0);
			_LogError( string("StringParser::LoadStringItem : ") + file + "加载失败." );
			return false;
		}
		TiXmlElement* root = doc.FirstChildElement( "Strings" );
		if( root == 0 )
		{
			doc.Clear();
			Assert(0);
			_LogError( string("StringParser::LoadStringItem : ") + file + "加载失败." );
			return false;
		}
		TiXmlElement* entityNode = root->FirstChildElement();
		for( entityNode; entityNode; entityNode=entityNode->NextSiblingElement() )
		{
			const char* key = entityNode->Value();
			const char* text = entityNode->Attribute( "Text" );
			if( text == 0 )
			{
				_LogError( string("StringParser::LoadStringItem Error: ") + key + " 缺少Text属性" );
				continue;
			}
			if( m_StringItems.GetStringItemById( key ) != 0 )
			{
				_LogError( string("StringParser::LoadStringItem Error: ") + key + " 已存在" );
				continue;
			}
			string theText = text;
			int len = ::strlen( text );
			int start = 0;
			int replaceCount = 0;
			for( int i=0; i<len; ++i )
			{
				/// Find "\n"
				if( text[i] == '\\' && i+1<len && text[i+1] == 'n' )
				{
					theText.erase( theText.begin()+(i-replaceCount),theText.begin()+(i-replaceCount+2));
					string strEnter = "\n";
					theText.insert( theText.begin()+(i-replaceCount), strEnter.begin(), strEnter.begin()+strEnter.length() );
					++i;
					++replaceCount;
				}
			}
			StringParser::StringItem* item = new StringParser::StringItem;
			Parse( key, We::Type::ToWstring( theText ), item );
			m_StringItems.Add( item );
		}
		return true;
	}
	void DestroyStringItem( StringParser::StringItem* p )
	{
		delete p;
		p = 0;
	}
	void StringParser::Clear()
	{
		m_StringItems.Clear( DestroyStringItem );
	}
	void StringParser::Parse( const string& key, const wstring& text, StringParser::StringItem* item )
	{
		item->m_Key = key;
		item->m_Text = text;
		int i = 0;
		int j = 0;
		int k = 0;
		bool hasError = false;
		while( true )
		{
			for( int x=i; x<text.length(); ++x )
			{
				if( text[x] == L'{' && x < text.length()-1 && text[x+1] == L'%' )
				{
					i = x;
					break;
				}
			}
			//i = text.find_first_of( L"{%", i );
			if( i < 0 )
				break;
			/// 0123{%s1}
			/// 012345678
			if( text.length() < i+5 )
				break;
			wchar_t typeChar = text.at( i+2 );
			/// 0123{%{%s1}
			if( typeChar != L's' 
				&& typeChar != L'S'
				&& typeChar != L'd'
				&& typeChar != L'D'
				&& typeChar != L'i'
				&& typeChar != L'I'
				&& typeChar != L'f'
				&& typeChar != L'F' )
			{
				i += 2;
				continue;
			}
			j = text.find_first_of( L"}", i+4 );
			if( j < 0 )
				break;
			int indexLen = j - i - 3;
			/// 0123{%s}8
			if( indexLen < 1 )
			{
				i = j + 1;
				continue;
			}
			/// 只支持1-99的序号
			/// 0123{%s100}
			if( indexLen > 2 )
			{
				i = j + 1;
				continue;
			}
			wstring indexStr = text.substr( i+3, indexLen );
			/// 判断是否是数字
			if( indexStr < L"0" || indexStr > L"99" )
			{
				break;
			}
			int index = We::Type::ToInt( indexStr );
			if( index <= 0 )
			{
				_LogError( string("StringParser::Parse Error : ") + key );
				hasError = true;
				break;
			}
			item->m_Indexes.push_back( index );
			item->m_Texts.push_back( text.substr( k, i-k ) );
			k = j + 1;
			i = j + 1;
		}
		item->m_Texts.push_back( text.substr( k, text.length()-k ) );
		/// 检查序号是否有误
		for( int i=0; i<item->m_Indexes.size(); ++i )
		{
			/// 序号超出范围
			if( item->m_Indexes[i] > item->m_Indexes.size() )
			{
				_LogError( string("StringParser::Parse Error : ") + key );
				hasError = true;
				break;
			}
			/// 序号重复
			for( int j=i+1; j<item->m_Indexes.size(); ++j )
			{
				if( item->m_Indexes[i] == item->m_Indexes[j] )
				{
					_LogError( string("StringParser::Parse Error : ") + key );
					hasError = true;
					break;
				}
			}
		}
		if( hasError )
		{
			item->m_Texts.clear();
			item->m_Indexes.clear();
		}
	}
	//------------------------------------------------------------------------
	wstring StringParser::GetString( const string& key, bool returnKeyIfFailed/*=true*/ )
	{
		StringParser::StringItem* item = m_StringItems.GetStringItemById( key );
		if( item == 0 )
		{
			_LogError( string("StringParser::GetString Error : ") + key + " 不存在" );
			if( returnKeyIfFailed )
				return We::Type::ToWstring( key );
			return L"";
		}
		return item->m_Text;
	}
	//------------------------------------------------------------------------
	wstring StringParser::GetString( const string& key, const Var& arg, bool returnKeyIfFailed/*=true*/ )
	{
		StringParser::StringItem* item = m_StringItems.GetStringItemById( key );
		if( item == 0 )
		{
			_LogError( string("StringParser::GetString Error : ") + key + " 不存在" );
			if( returnKeyIfFailed )
				return We::Type::ToWstring( key );
			return L"";
		}
		if( item->m_Texts.size() != 2 )
		{
			_LogError( string("StringParser::GetString Error : ") + key + " 参数个数不匹配" );
			if( returnKeyIfFailed )
				return We::Type::ToWstring( key );
			return L"";
		}
		return item->m_Texts[0] + arg.ForceToWstring() + item->m_Texts[1];
	}
	//------------------------------------------------------------------------
	wstring StringParser::GetString( const string& key, const VarList& args, bool returnKeyIfFailed/*=true*/ )
	{
		StringParser::StringItem* item = m_StringItems.GetStringItemById( key );
		if( item == 0 )
		{
			_LogError( string("StringParser::GetString Error : ") + key + " 不存在" );
			if( returnKeyIfFailed )
				return We::Type::ToWstring( key );
			return L"";
		}
		if( item->m_Texts.size() != args.Size() + 1 )
		{
			_LogError( string("StringParser::GetString Error : ") + key + " 参数个数不匹配" );
			if( returnKeyIfFailed )
				return We::Type::ToWstring( key );
			return L"";
		}
		wstring ret = item->m_Texts[0];
		for( int i=0; i<args.Size(); ++i )
			ret += args.GetValue( item->m_Indexes[i]-1 ).ForceToWstring() + item->m_Texts[i+1];
		return ret;
	}
}