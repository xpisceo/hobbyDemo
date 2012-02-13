/********************************************************************
	created:	2009-9-10
	author:		Fish (�ڹ�ƽ)
	svn:		CGO
	purpose:	lua�ű�
	ע:			�ӽű�����򴫵�float,int64,uint64ʱ������tagLuaNumber��ת��
				lua�ű��������debug.traceback��������ö�ջ
*********************************************************************/
#pragma once

// ��̬��
#ifdef We_DLL_EXPORTS
#pragma comment( lib, "lua51.lib" )
#else
// ��̬��
#pragma comment( lib, "Lua514.lib" )
#endif

extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

#include "WeConfig.h"
#include "WeVar.h"
#include "WeStream.h"
#include <string>

namespace We
{
	class TiXmlElement;

	// �ű����л��ı��
	enum ScriptStreamType
	{
		ScriptStreamType_bool		= 0x01,
		ScriptStreamType_number		= 0x02,
		ScriptStreamType_string		= 0x03,
		ScriptStreamType_Table		= 0x04,
		ScriptStreamType_END		= 0xFF,
	};

	// table������
	enum ScriptStreamTableFlag
	{
		ScriptStreamTableFlag_Begin	= 1,
		ScriptStreamTableFlag_End	= 2,
	};

	// ���ں�lua��number����ת��
	union tagLuaNumber
	{
		float	f;
		uint64	u64;
		int64	i64;
		double	d;
	};

	class We_EXPORTS LuaScript
	{
	protected:
		lua_State*	L;				/// lua���	
		std::string m_ScriptPath;	/// �ű�Ŀ¼
		We::VarList m_Results;		/// �ű���������ֵ�б�

		static LuaScript* s_LuaScript;
	public:
		static const int NOT_FIND_FUNCTION = -10000000;
		static uint8 s_TmpBuffer[1024*128]; /// 128K
	public:
		LuaScript();
		virtual ~LuaScript();

		static LuaScript* GetLuaScript() { return s_LuaScript; }

		lua_State* GetL() { return L; }

		void OpenLua();
		void CloseLua();

		void				SetScriptPath( const std::string& path ) { m_ScriptPath=path; }
		const std::string	GetScriptPath() const { return m_ScriptPath; }

		int GetTop();
		void SetTop( int top );

		/// ���ؽű�
		int LoadFile( const char *luafile );

		/// ִ��
		int	ExecuteString( const char* str );
		int	ExecuteBuffer( const char* buf, size_t size, const char* name );
		int	ExecuteBuffer( const char* buf, size_t size) ;

		/// ���ýű�����
		bool Execute( const char* funcName );
		bool Execute( const char* funcName, const We::Var& arg );
		bool Execute( const char* funcName, const We::Var& arg1, const We::Var& arg2 );
		bool Execute( const char* funcName, const We::Var& arg1, const We::Var& arg2, const We::Var& arg3 );
		bool Execute( const char* funcName, const We::Var& arg1, const We::Var& arg2, const We::Var& arg3, const We::Var& arg4 );

		bool Execute( const char* funcName, const We::VarList& args );

		/// ����һ����,����ջ��
		bool CreateTable( int narr = 0, int nrec = 0);
		/// ��ջ���ı�ֵ
		bool SetTableValue( int key, const We::Var& value );
		bool SetTableValue( const char* key, const We::Var& value );

		/// ��ջ���ı������ֱ�
		void SetTable( int idx );

		/// ���ؽű��������صĽ����
		We::VarList&		GetResults() { return m_Results; }
		const We::VarList&	GetResults() const { return m_Results; }

		/// ȡֵ
		Var ToVar();

		static Var ToVar( lua_State* L, int idx );
		static Var ToVar( lua_State* L, int idx, VarType varType );

		/// �Ѻ�������ջ��
		int  PrepareCall( const char* funcName );
		/// push����
		void PushArg( const We::Var& arg );
		/// ����ջ���ĺ���
		bool Call( const int stackPos, bool returnResult=true );

		/// ��lua��table���浽��������,�Ӷ����Ա��浽���ݿ���߶������ļ���,����ͨ����������������
		bool SaveTableByName( const char* tableName, uint8* buffer, uint32 bufferSize, uint32& saveSize );
		bool SaveTable( int idx, StreamWriter& writer, bool writeEndFlag );
		/// ����ֵ
		bool SaveValue( int idx, StreamWriter& writer );

		/// ���㱣��table��Ҫ��ʵ�ʿռ��С
		bool GetSaveTableSize( int idx, int& saveSize, bool writeEndFlag );
		bool GetSaveValueSize( int idx, int& saveSize );

		/// ������������table������
		bool LoadTable( const char* tableName, StreamReader& reader );
		bool LoadTable( int idx, StreamReader& reader );
		/// ��ȡֵ
		bool LoadValue( int idx, StreamReader& reader, bool& isKey, bool& loadOK  );

		/// �ѽű����table�����ݱ��浽xml��
		bool SaveToXml( const char* tableName, TiXmlElement* parentNode, const std::string& numberPrefix );
		bool SaveToXml( int idx, TiXmlElement* parentNode, const std::string& numberPrefix );
		bool SaveToXmlValue( int idx, TiXmlElement* parentNode, const std::string& key );

		/// ��ȡ�û�����
		void* ToUserData( int idx, const char* name );

		/// ע��ű�����
		void RegisterFunction( const char* funcName, lua_CFunction func );
	};

	// �ű�����: ����table��ֵ���������ļ�
	// ����1: ��
	// ����2: �ļ���
	// ����ֵ: ������ļ���С or nil
	int L_SaveTable( lua_State* L );

	// �ű�����: �Ӷ������ļ�����table������
	// ����1: ��
	// ����2: �ļ���
	int L_LoadTable( lua_State* L );
}