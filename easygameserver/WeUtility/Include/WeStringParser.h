/*************************************************************************
	created:	  2008-04-21   20:26
	filename: 	  WeStringParser.h
	reference:  
	created:	  fish        (于国平)
	modified:
	company:      
 
	description:  文字解析
*************************************************************************/
#ifndef _WeStringParser_H_
#define _WeStringParser_H_

#include "WeConfig.h"
#include "WeSingleton.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "DS_HashTable.h"
#include "WeVar.h"

namespace We
{
    class WeEXPORT StringParser : public Singleton<StringParser>
	{
	public:
		struct StringItem
		{
			string			m_Key;
			wstring			m_Text;
			vector<wstring>	m_Texts;
			vector<int>		m_Indexes;
		};

		static unsigned int HashStringItem( StringParser::StringItem* &item )
		{
			return DataStructures::HashUtil::HashString_Lower( item->m_Key.c_str() );
		}

		class StringItemTable : public DataStructures::HashTable< StringParser::StringItem*, StringParser::HashStringItem >
		{
		public:
			StringParser::StringItem* GetStringItemById( const string& key )
			{
				int index = DataStructures::HashUtil::HashString_Lower( key.c_str() );
				StringItemTable::Node* pNode = (StringItemTable::Node*)(&(GetNode( index%Size() ) ));
				while( pNode != 0 )
				{
					if( !pNode->null
						&& stricmp( pNode->data->m_Key.c_str(), key.c_str() ) == 0 )
						return pNode->data;
					pNode = pNode->pNext;
				}
				return 0;
			}
		};

	public:
		Singleton_Declare( StringParser );

		StringParser();
        ~StringParser();

		void Initialize( unsigned int size );

		bool LoadStringItems( const string& fileList, const string& directory, bool isEncrypt );

		bool LoadStringItem( const string& file, bool isEncrypt );

		void Clear();

		void Parse( const string& key, const wstring& text, StringParser::StringItem* item );

		wstring GetString( const string& key, bool returnKeyIfFailed=true );
		wstring GetString( const string& key, const Var& arg, bool returnKeyIfFailed=true );
		wstring GetString( const string& key, const VarList& args, bool returnKeyIfFailed=true );

	protected:
		StringItemTable m_StringItems;
	};
}


#endif