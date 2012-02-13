/********************************************************************
	created:	2009-9-10
	author:		Fish (于国平)
	svn:		CGO
	purpose:	lua脚本
	注:			从脚本向程序传递float,int64,uint64时可以用tagLuaNumber来转换
				lua脚本里可以用debug.traceback来输出调用堆栈
*********************************************************************/
#pragma once

// 动态库
#ifdef We_DLL_EXPORTS
#pragma comment( lib, "lua51.lib" )
#else
// 静态库
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

	// 脚本序列化的标记
	enum ScriptStreamType
	{
		ScriptStreamType_bool		= 0x01,
		ScriptStreamType_number		= 0x02,
		ScriptStreamType_string		= 0x03,
		ScriptStreamType_Table		= 0x04,
		ScriptStreamType_END		= 0xFF,
	};

	// table保存标记
	enum ScriptStreamTableFlag
	{
		ScriptStreamTableFlag_Begin	= 1,
		ScriptStreamTableFlag_End	= 2,
	};

	// 用于和lua的number互相转换
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
		lua_State*	L;				/// lua句柄	
		std::string m_ScriptPath;	/// 脚本目录
		We::VarList m_Results;		/// 脚本函数返回值列表

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

		/// 加载脚本
		int LoadFile( const char *luafile );

		/// 执行
		int	ExecuteString( const char* str );
		int	ExecuteBuffer( const char* buf, size_t size, const char* name );
		int	ExecuteBuffer( const char* buf, size_t size) ;

		/// 调用脚本函数
		bool Execute( const char* funcName );
		bool Execute( const char* funcName, const We::Var& arg );
		bool Execute( const char* funcName, const We::Var& arg1, const We::Var& arg2 );
		bool Execute( const char* funcName, const We::Var& arg1, const We::Var& arg2, const We::Var& arg3 );
		bool Execute( const char* funcName, const We::Var& arg1, const We::Var& arg2, const We::Var& arg3, const We::Var& arg4 );

		bool Execute( const char* funcName, const We::VarList& args );

		/// 创建一个表,放在栈顶
		bool CreateTable( int narr = 0, int nrec = 0);
		/// 给栈顶的表赋值
		bool SetTableValue( int key, const We::Var& value );
		bool SetTableValue( const char* key, const We::Var& value );

		/// 给栈顶的表设置字表
		void SetTable( int idx );

		/// 返回脚本函数返回的结果集
		We::VarList&		GetResults() { return m_Results; }
		const We::VarList&	GetResults() const { return m_Results; }

		/// 取值
		Var ToVar();

		static Var ToVar( lua_State* L, int idx );
		static Var ToVar( lua_State* L, int idx, VarType varType );

		/// 把函数放在栈顶
		int  PrepareCall( const char* funcName );
		/// push参数
		void PushArg( const We::Var& arg );
		/// 调用栈顶的函数
		bool Call( const int stackPos, bool returnResult=true );

		/// 把lua的table保存到数据流中,从而可以保存到数据库或者二进制文件中,或者通过网络来传输数据
		bool SaveTableByName( const char* tableName, uint8* buffer, uint32 bufferSize, uint32& saveSize );
		bool SaveTable( int idx, StreamWriter& writer, bool writeEndFlag );
		/// 保存值
		bool SaveValue( int idx, StreamWriter& writer );

		/// 计算保存table需要的实际空间大小
		bool GetSaveTableSize( int idx, int& saveSize, bool writeEndFlag );
		bool GetSaveValueSize( int idx, int& saveSize );

		/// 从数据流加载table的数据
		bool LoadTable( const char* tableName, StreamReader& reader );
		bool LoadTable( int idx, StreamReader& reader );
		/// 读取值
		bool LoadValue( int idx, StreamReader& reader, bool& isKey, bool& loadOK  );

		/// 把脚本里的table的数据保存到xml里
		bool SaveToXml( const char* tableName, TiXmlElement* parentNode, const std::string& numberPrefix );
		bool SaveToXml( int idx, TiXmlElement* parentNode, const std::string& numberPrefix );
		bool SaveToXmlValue( int idx, TiXmlElement* parentNode, const std::string& key );

		/// 获取用户数据
		void* ToUserData( int idx, const char* name );

		/// 注册脚本函数
		void RegisterFunction( const char* funcName, lua_CFunction func );
	};

	// 脚本函数: 保存table的值到二进制文件
	// 参数1: 表
	// 参数2: 文件名
	// 返回值: 保存的文件大小 or nil
	int L_SaveTable( lua_State* L );

	// 脚本函数: 从二进制文件加载table的数据
	// 参数1: 表
	// 参数2: 文件名
	int L_LoadTable( lua_State* L );
}