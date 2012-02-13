#include "WeScript.h"
#include <string>
#include <Windows.h>
#include <assert.h>
#include "TinyXml/TinyXml.h"
#include "WeType.h"
using namespace std;


namespace We
{
	LuaScript* LuaScript::s_LuaScript = 0; 
	uint8 LuaScript::s_TmpBuffer[1024*128]; 

	LuaScript::LuaScript()
	{
		s_LuaScript = this;
		OpenLua();
	}
	LuaScript::~LuaScript()
	{
		CloseLua();
	}

	void LuaScript::OpenLua()
	{
		L = lua_open();
		lua_gc( L, LUA_GCSTOP, 0 );
		luaL_openlibs( L );
		lua_gc( L, LUA_GCRESTART, 0 );
	}

	void LuaScript::CloseLua()
	{
		if( L )
		{
			lua_close( L );
			L = NULL;
		}
	}

	int LuaScript::GetTop()
	{
		return lua_gettop( L );
	}

	void LuaScript::SetTop( int top )
	{
		lua_settop( L, top );
	}

	int LuaScript::LoadFile(const char *luafile)
	{
		assert( luafile );
		if( luafile == 0 )
			return 0;

		int ret = luaL_loadfile( L, (m_ScriptPath+luafile).c_str() );
		if ( ret != 0 )
		{
			LPCSTR str = lua_tostring( L, -1 );
			if ( str )
			{
				char temp[1024];
				sprintf( temp, "luaL_loadfile: %s\n", str );
				OutputDebugString( temp );
				lua_pop( L, 1 );
			}
			return ret;
		}

		ret = lua_pcall( L, 0, LUA_MULTRET, 0 );
		if ( ret != 0 )
		{
			LPCSTR str = lua_tostring( L, -1 );
			if ( str )
			{
				char temp[1024];
				sprintf( temp, "%s\r\n", str );
				OutputDebugString( temp );
				lua_pop( L, 1 );
			}
		}

		return ret;
	}
	int LuaScript::ExecuteString(const char *str)
	{
		int ret = luaL_loadstring( L, str );
		if ( ret != 0 )
		{
			LPCSTR str = lua_tostring( L, -1 );
			if ( str )
			{
				char temp[1024];
				sprintf( temp, "%s\r\n", str );
				OutputDebugString( temp );
				lua_pop( L, 1 );
			}
			return ret;
		}

		ret = lua_pcall( L, 0, LUA_MULTRET, 0 );
		if ( ret != 0 )
		{
			LPCSTR str = lua_tostring( L, -1 );
			if ( str )
			{
				char temp[1024];
				sprintf( temp, "%s\r\n", str );
				OutputDebugString( temp );
				lua_pop( L, 1 );
			}
		}

		return ret;
	}

	int LuaScript::ExecuteBuffer(const char *buf, size_t size, const char *name)
	{
		int ret = luaL_loadbuffer( L, buf, size, name );
		if ( ret != 0 )
		{
			LPCSTR str = lua_tostring( L, -1 );
			if ( str )
			{
				char temp[1024];
				sprintf( temp, "%s\r\n", str );
				OutputDebugString( temp );
				lua_pop( L, 1 );
			}
			return ret;
		}
		ret = lua_pcall( L, 0, LUA_MULTRET, 0 );
		if ( ret != 0 )
		{
			LPCSTR str = lua_tostring( L, -1 );
			if ( str )
			{
				char temp[1024];
				sprintf( temp, "%s\r\n", str );
				OutputDebugString( temp );
				lua_pop( L, 1 );
			}
		}

		return ret;
	}

	int LuaScript::ExecuteBuffer(const char *buf, size_t size)
	{
		int ret = luaL_loadbuffer( L, buf, size, NULL );
		if ( ret != 0 )
		{
			LPCSTR str = lua_tostring( L, -1 );
			if ( str )
			{
				char temp[1024];
				sprintf( temp, "%s\r\n", str );
				OutputDebugString( temp );
				lua_pop( L, 1 );
			}
			return ret;
		}

		ret = lua_pcall( L, 0, LUA_MULTRET, 0 );
		if ( ret != 0 )
		{
			LPCSTR str = lua_tostring( L, -1 );
			if ( str )
			{
				char temp[1024];
				sprintf( temp, "%s\r\n", str );
				OutputDebugString( temp );
				lua_pop( L, 1 );
			}
		}

		return ret;
	}

	bool LuaScript::Call( const int stackPos, bool returnResult/*=true*/ )
	{
		int curPos = lua_gettop( L );
		if ( stackPos > curPos )
		{
			char temp[1024];
			sprintf( temp, "ERROR: stackPos > curPos stackPos=%d curPos=%d\n", stackPos, curPos );
			OutputDebugString( temp );
			return false;
		}
		/// ִ�нű�����
		if ( lua_pcall( L, lua_gettop( L )-stackPos, LUA_MULTRET, 0 ) != 0 )
		{
			char temp[1024];
			sprintf( temp, "%s\r\n", lua_tostring( L, -1 ) );
			OutputDebugString( temp );
			lua_settop( L, stackPos-1 );
			return false;
		}
		/// ִ����ű�������,�����Լ�ѹջ�Ĳ�������ջ��,�ű���������ֵ��ѹ��ջ
		if( returnResult )
		{
			/// ȡ������ֵ
			int retNum = lua_gettop( L ) - stackPos + 1;
			m_Results.Resize( retNum );
			/// ����ֵ˳�����Ϊʵ�ʵĴ���˳��
			for ( int i = 0; i < retNum; i ++ )
			{
				switch( lua_type( L, -1 ) )
				{
				case LUA_TNIL:
					m_Results.SetValue( retNum-1-i, Null_Var );
					break;

				case LUA_TNUMBER:
					/// uint32�� float��ô����????????????
					/// �ѵ�Ҫ����string??????
					/// ��ʱ��Ҫ�ӽű��������number����float,unsigned int��double!!!!!!
					/// �������: Var���Ӷ�double��֧��!!!!!!
					m_Results.SetValue( retNum-1-i, (int)lua_tonumber(L,-1) );
					break;

				case LUA_TSTRING:
					m_Results.SetValue( retNum-1-i, lua_tostring(L,-1) );
					break;

				case LUA_TBOOLEAN:
					m_Results.SetValue( retNum-1-i, (lua_toboolean(L,-1)==1) );
					break;

				default:
					{
						char temp[1024];
						sprintf( temp, "LuaScript::Call ERROR: type %d=%d\n", i, lua_type( L, -1 ) );
						OutputDebugString( temp );
					}
					break;
				}
				lua_pop( L, 1 );
			}
			assert( lua_gettop( L ) == stackPos-1 );
		}
		return true;
	}

	bool LuaScript::Execute(const char *name)
	{
		int funcStackPos = PrepareCall( name );
		if( funcStackPos == -10000000 )
			return false;
		if ( !Call( funcStackPos ) )
			return false;
		return true;
	}

	bool LuaScript::Execute(const char* funcName, const We::Var& arg)
	{
		int funcStackPos = PrepareCall( funcName );
		if( funcStackPos == -10000000 )
			return false;
		PushArg( arg );
		if ( !Call( funcStackPos ) )
			return false;
		return true;
	}

	bool LuaScript::Execute(const char* funcName, const We::Var& arg1, const We::Var& arg2 )
	{
		int funcStackPos = PrepareCall( funcName );
		if( funcStackPos == -10000000 )
			return false;
		PushArg( arg1 );
		PushArg( arg2 );
		if ( !Call( funcStackPos ) )
			return false;
		return true;
	}

	bool LuaScript::Execute(const char* funcName, const We::Var& arg1, const We::Var& arg2, const We::Var& arg3 )
	{
		int funcStackPos = PrepareCall( funcName );
		if( funcStackPos == -10000000 )
			return false;
		PushArg( arg1 );
		PushArg( arg2 );
		PushArg( arg3 );
		if ( !Call( funcStackPos ) )
			return false;
		return true;
	}

	bool LuaScript::Execute(const char* funcName, const We::Var& arg1, const We::Var& arg2, const We::Var& arg3, const We::Var& arg4 )
	{
		int funcStackPos = PrepareCall( funcName );
		if( funcStackPos == -10000000 )
			return false;
		PushArg( arg1 );
		PushArg( arg2 );
		PushArg( arg3 );
		PushArg( arg4 );
		if ( !Call( funcStackPos ) )
			return false;
		return true;
	}

	bool LuaScript::Execute(const char* funcName, const We::VarList& args )
	{
		int funcStackPos = PrepareCall( funcName );
		if( funcStackPos == -10000000 )
			return false;
		for( int i=0; i<args.Size(); ++i )
			PushArg( args.GetValue(i) );
		if ( !Call( funcStackPos ) )
			return false;
		return true;
	}

	bool LuaScript::CreateTable(int narr /*= 0*/, int nrec /*= 0*/)
	{
		lua_createtable( L, narr, nrec );
		return lua_istable(L,-1);
	}

	bool LuaScript::SetTableValue( int key, const We::Var& value )
	{
		PushArg( value );
		lua_rawseti( L, -2, key );
		return true;
	}

	bool LuaScript::SetTableValue( const char* key, const We::Var& value )
	{
		assert( key );
		if( key == 0 )
			return false;
		lua_pushstring( L, key );
		PushArg( value );
		lua_settable( L, -3 );
		return true;
	}

	void LuaScript::SetTable( int idx )
	{
		lua_rawseti( L, -2, idx );
	}

	Var LuaScript::ToVar()
	{
		Var ret;
		switch( lua_type( L, -1 ) )
		{
		case LUA_TNIL:
			return ret;
			break;

		case LUA_TNUMBER:
			/// uint32�� float��ô����????????????
			/// �ѵ�Ҫ����string??????
			/// ��ʱ��Ҫ�ӽű��������number����float,unsigned int��double!!!!!!
			/// �������: Var���Ӷ�double��֧��!!!!!!
			ret.Set_int( (int)lua_tonumber(L,-1) );
			break;

		case LUA_TSTRING:
			ret.Set_string( lua_tostring(L,-1) );
			break;

		case LUA_TBOOLEAN:
			ret.Set_bool( lua_toboolean(L,-1)==1 );
			break;

		default:
			{
				char temp[1024];
				sprintf_s( temp, sizeof(temp), "ToVar ERROR type = %d\n", lua_type( L, -1 ) );
				OutputDebugString( temp );
			}
			break;
		}
		return ret;
	}

	Var LuaScript::ToVar( lua_State* L, int idx )
	{
		Var ret;
		switch( lua_type( L, idx ) )
		{
		case LUA_TNIL:
			return ret;
			break;

		case LUA_TNUMBER:
			/// uint32�� float��ô����????????????
			/// �ѵ�Ҫ����string??????
			/// ��ʱ��Ҫ�ӽű��������number����float,unsigned int��double!!!!!!
			/// �������: Var���Ӷ�double��֧��!!!!!!
			ret.Set_int( (int)lua_tonumber(L,idx) );
			break;

		case LUA_TSTRING:
			ret.Set_string( lua_tostring(L,idx) );
			break;

		case LUA_TBOOLEAN:
			ret.Set_bool( lua_toboolean(L,idx)==1 );
			break;

		default:
			{
				char temp[1024];
				sprintf_s( temp, sizeof(temp), "ToVar ERROR type = %d\n", lua_type( L, idx ) );
				OutputDebugString( temp );
			}
			break;
		}
		return ret;
	}

	Var LuaScript::ToVar( lua_State* L, int idx, VarType varType )
	{
		Var ret;
		switch ( varType )
		{
		case VarType_bool:
			ret.Set_bool( lua_toboolean(L,idx)==1 );
			break;
		case VarType_int:
			ret.Set_int( (int)lua_tonumber(L,idx) );
			break;
		case VarType_uint:
			ret.Set_uint( (uint)lua_tonumber(L,idx) );
			break;
		case VarType_float:
			ret.Set_float( (float)lua_tonumber(L,idx) );
			break;
		case VarType_string:
			ret.Set_string( lua_tostring( L, idx ) );
			break;
		case VarType_wstring:
			{
				const char* tmpStr = lua_tostring( L, idx );
				if( tmpStr )
				{
					ret.Set_wstring( Type::ToWstring(tmpStr) );
				}
			}
			break;
		case VarType_EntityID:
			{
				// double��10��λ�洢ʵ������,0��ʾ����ͨʵ��
				uint d = (uint)lua_tonumber( L, idx );
				if( d < 0 )
					return 0;
				int entityType = (int)(d/1000000000);
				uint id = (uint)(d-entityType*1000000000);
				if( entityType == 0 )
					entityType = EntityType_Entity;
				else if( entityType >= EntityType_Count )
					return 0;
				ret.Set_EntityID( EntityID((EntityType)entityType,0,id) );
			}
			break;
		default:
			break;
		}
		return ret;
	}

	int LuaScript::PrepareCall( const char* funcName )
	{
		m_Results.Clear();
		int oldStackPos = lua_gettop( L );
		lua_getglobal( L, funcName );	/// �Ѻ�������ջ��
		/// �жϸú����Ƿ����
		if ( lua_isnil( L, -1 ) )
		{
			char temp[1024];
			sprintf( temp, "Not find func %s\r\n", funcName );
			OutputDebugString( temp );
			lua_pop( L, 1 );
			return -10000000;
		}
		return oldStackPos + 1;
	}

	void LuaScript::PushArg(const We::Var& arg)
	{
		switch( arg.Type() )
		{
		case We::VarType_bool:
			lua_pushboolean( L, arg.Get_bool()?1:0 );
			break;
		case We::VarType_int:
			lua_pushnumber( L, arg.Get_int() );
			break;
		case We::VarType_uint:
			lua_pushnumber( L, arg.Get_uint() );
			break;
		case We::VarType_float:
			lua_pushnumber( L, arg.Get_float() );
			break;
		case We::VarType_string:
			lua_pushstring( L, arg.Get_string() );
			break;
		case We::VarType_wstring:
			lua_pushstring( L, arg.ForceToString().c_str() );
			break;
		case VarType_EntityID:
			// ��ʱֻ����id
			lua_pushnumber( L, arg.Get_EntityID().Serial() );
			break;
		default:
			lua_pushnil( L );
			break;
		}
	}

	void* LuaScript::ToUserData( int idx, const char* name )
	{
		if( !name )
			return NULL;
		// the same code as luaL_checkudata except no error thrown
		void* p = lua_touserdata( L, idx );
		/* value is a userdata? */
		if ( p != NULL )
		{
			/* does it have a metatable? */
			if ( lua_getmetatable( L, idx ) ) // p��Ԫ��ŵ�ջ��
			{
				// ��Ϊname��Ԫ��ŵ�ջ��
				lua_getfield( L, LUA_REGISTRYINDEX, name ); /* get correct metatable */
				/* does it have the correct mt? */
				if (lua_rawequal(L, -1, -2)) // �Ƚ��ǲ���ͬһ��Ԫ��
				{
					// �Ƴ�Ԫ��,��ջƽ��
					lua_pop(L, 2); /* remove both metatables */
					return p;
				}
				else
				{
					// �Ƴ�Ԫ��,��ջƽ��
					lua_pop(L, 2);
				}
			}
		}
		return NULL; /* to avoid warnings */
	}

	void LuaScript::RegisterFunction(const char* funcName, lua_CFunction func)
	{
		lua_register(L, funcName, func);
	}

	bool LuaScript::SaveTableByName( const char* tableName, uint8* buffer, uint32 bufferSize, uint32& saveSize )
	{
		assert( tableName );
		assert( buffer );
		assert( bufferSize > 0 );
		if( tableName == 0 || buffer == 0 || bufferSize == 0 )
			return false;
		int oldStackPos = lua_gettop( L );
		lua_getglobal( L, tableName );	/// ��table����ջ��
		if ( !lua_istable( L, -1 ) )
		{
			char temp[1024];
			sprintf( temp, "SaveTableByName ERROR: Not find table %s\r\n", tableName );
			OutputDebugString( temp );
			lua_pop( L, 1 );
			return false;
		}
		/// �ݹ�table
		uint32 offset = 0;
		uint8* p = buffer;
		StreamWriter writer( buffer, bufferSize );
		if( !SaveTable( lua_gettop( L ), writer, true ) )
		{
			char temp[1024];
			sprintf( temp, "SaveTableByName ERROR %s\r\n", tableName );
			OutputDebugString( temp );
			return lua_settop( L,oldStackPos),false;
		}
		lua_pop(L,1); /// ��table��ջ
		assert( lua_gettop( L ) == oldStackPos );
		if( lua_gettop( L ) != oldStackPos )
			return lua_settop( L,oldStackPos),false;
		saveSize = writer.GetOffset();
		return true;
	}

	bool LuaScript::SaveTable( int idx, StreamWriter& writer, bool writeEndFlag )
	{
		/* http://www.lua.org/manual/5.1/manual.html#lua_next
		lua_next: Pops a key from the stack, and pushes a key-value pair from the table at the given index
		(the "next" pair after the given key). If there are no more elements in the table, then lua_next returns 0 (and pushes nothing). 
		*/
		int oldStackPos = lua_gettop( L );
		//try
		//{
			lua_pushnil( L );
			while( lua_next(L, idx ) ) /// pop1��,push2��
			{
				if( !SaveValue( -2, writer ) )	/// key
					return false;
				if( !SaveValue( -1, writer ) )	/// value
					return false;
				/// �ӱ�
				if( lua_istable(L,-1) )
				{
					if( !SaveTable( lua_gettop(L), writer, false ) )
					{
						lua_settop( L,oldStackPos);
						return false;
					}
					writer.Write( (uint8)ScriptStreamType_Table );		// �ӱ�������
					writer.Write( (uint8)ScriptStreamTableFlag_End );	//
				}
				lua_pop( L, 1 ); /// ��Ϊlua_next��pop1��,push2��,��������ֻ��Ҫpop1���Ϳ�����
			}
			assert( lua_gettop( L ) == oldStackPos );
		//}
		//catch( ... )
		//{
		//	lua_settop( L,oldStackPos);
		//	return false;
		//}
		if( writeEndFlag )
			writer.Write( (uint8)ScriptStreamType_END );
		return true;
	}

	bool LuaScript::SaveValue( int idx, StreamWriter& writer )
	{
		int type = lua_type( L, idx );
		switch( type )
		{
		case LUA_TNUMBER:
			writer.Write(  (uint8)ScriptStreamType_number );
			writer.Write( lua_tonumber(L,idx) ); // sizeof(lua_Number)
			break;

		case LUA_TSTRING:
			{
				const char* s = lua_tostring(L, idx);
				assert( s );
				if( !s )
					return false;
				writer.Write(  (uint8)ScriptStreamType_string );
				writer.Write( s ); // sizeof(uint32) + strlen(s)
			}
			break;

		case LUA_TBOOLEAN:
			writer.Write(  (uint8)ScriptStreamType_bool );
			writer.Write( lua_toboolean(L,idx)==0?false:true ); // 1
			break;

		case LUA_TTABLE:
			{
				assert( idx == -1 ); /// tableֻ����value,����������key!!!!!!
				if( idx != -1 )
					return false;
				writer.Write( (uint8)ScriptStreamType_Table );		/// �ӱ�ʼ���
				writer.Write( (uint8)ScriptStreamTableFlag_Begin );
			}
			break;

		default:
			break;
		}
		return true;
	}
	bool LuaScript::GetSaveTableSize( int idx, int& saveSize, bool writeEndFlag )
	{
		int oldStackPos = lua_gettop( L );
		lua_pushnil( L );
		while( lua_next(L, idx ) ) /// pop1��,push2��
		{
			if( !GetSaveValueSize( -2, saveSize ) )	/// key
				return false;
			if( !GetSaveValueSize( -1, saveSize ) )	/// value
				return false;
			/// �ӱ�
			if( lua_istable(L,-1) )
			{
				if( !GetSaveTableSize( lua_gettop(L), saveSize, false ) )
				{
					lua_settop( L,oldStackPos);
					return false;
				}
				saveSize += 2;
				//writer.Write( (uint8)ScriptStreamType_Table );		// �ӱ�������
				//writer.Write( (uint8)ScriptStreamTableFlag_End );	//
			}
			lua_pop( L, 1 ); /// ��Ϊlua_next��pop1��,push2��,��������ֻ��Ҫpop1���Ϳ�����
		}
		assert( lua_gettop( L ) == oldStackPos );
		if( lua_gettop( L ) != oldStackPos )
			return false;
		if( writeEndFlag )
			++saveSize;
		return true;
	}
	bool LuaScript::GetSaveValueSize( int idx, int& saveSize )
	{
		int type = lua_type( L, idx );
		switch( type )
		{
		case LUA_TNUMBER:
			{
				saveSize += ( 1 + sizeof(lua_Number) );
			}
			break;

		case LUA_TSTRING:
			{
				const char* s = lua_tostring(L,idx);
				if( !s )
					return false;
				int size = ::strlen(s);
				saveSize += ( 1 + sizeof(uint32) +size );
			}
			break;

		case LUA_TBOOLEAN:
			saveSize += 2;
			break;

		case LUA_TTABLE:
			{
				assert( idx == -1 ); /// tableֻ����value,����������key!!!!!!
				if( idx != -1 )
					return false;
				saveSize += 2;
			}
			break;

		default:
			break;
		}
		return true;
	}
	bool LuaScript::LoadTable( const char* tableName, StreamReader& reader )
	{
		assert( tableName );
		if( tableName == 0 )
			return false;
		int oldStackPos = lua_gettop( L );
		lua_getglobal( L, tableName );	/// ��table����ջ��

		if( !LoadTable( lua_gettop( L ), reader ) )
			return lua_settop( L,oldStackPos),false; 
		lua_pop(L,1); /// ��table��ջ
		assert( lua_gettop( L ) == oldStackPos );
		if( lua_gettop( L ) != oldStackPos )
			return lua_settop( L,oldStackPos),false;
		return true;
	}
	bool LuaScript::LoadTable( int idx, StreamReader& reader )
	{
		int oldStackPos = lua_gettop( L );
		if ( !lua_istable( L, -1 ) )
		{
			char temp[1024];
			sprintf( temp, "LoadTable Not a table\r\n" );
			OutputDebugString( temp );
			return false;
		}
		/// �ݹ�table
		bool isKey = true;
		bool loadOK = false;
		while( reader.GetSpace() > 0 )
		{
			if( !LoadValue( lua_gettop( L ), reader, isKey, loadOK ) )
				return lua_settop( L,oldStackPos),false;
			if( loadOK )
				break;
		}
		assert( isKey );
		if( !isKey )
			return lua_settop( L,oldStackPos),false;
		return true;
	}
	bool LuaScript::LoadValue( int idx, StreamReader& reader, bool& isKey, bool& loadOK )
	{
		if( reader.GetSpace() < 1 )
			return true;
		int oldStackPos = lua_gettop( L );
		uint8 type = reader.Read_uint8();
		switch( type )
		{
		case ScriptStreamType_number:
			{
				lua_pushnumber( L, reader.Read_double() );
				/// �����һ�㲻��table,��һ����key
				/// -1 number value
				/// -2 key
				/// -3 table
				if( !isKey && lua_istable( L, -3 ) )
				{
					assert( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING );
					if( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING )
						lua_settable( L, -3 );
				}
				isKey = !isKey;
			}
			break;

		case ScriptStreamType_string:
			{
				lua_pushstring( L, reader.Read_string().c_str() );
				/// -1 string value
				/// -2 key
				/// -3 table 
				if( !isKey && lua_istable( L, -3 ) )
				{
					assert( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING );
					if( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING )
						lua_settable( L, -3 );
				}
				isKey = !isKey;
			}
			break;

		case ScriptStreamType_bool:
			{
				lua_pushboolean( L, reader.Read_bool()?1:0 );
				/// -1 bool value
				/// -2 key
				/// -3 table 
				if( !isKey && lua_istable( L, -3 ) )
				{
					assert( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING );
					if( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING )
						lua_settable( L, -3 );
				}
				isKey = !isKey;
			}
			break;

		case ScriptStreamType_Table:
			{
				uint8 flagValue = reader.Read_uint8();
				if( flagValue == ScriptStreamType_END ) /// �������ݿ�Ľ������
				{
					assert( isKey );
					loadOK = true;
					return isKey;
				}
				else if( flagValue == ScriptStreamTableFlag_Begin ) /// table��ʼ���
				{
					/// ����һ���ֱ�
					lua_newtable( L );
					/// -1 sub table
					/// -2 key
					/// -3 parent table
					assert( lua_istable( L, -3 ) );
					assert( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING );
					assert( !isKey );
					if( !isKey && lua_istable( L, -3 ) )
					{
						assert( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING );
					}
					isKey = !isKey;
					bool loadOK2 = false;
					/// �����ӱ�
					if( !LoadValue( lua_gettop(L), reader, isKey, loadOK2 ) )
						return false;
				}
				else if( flagValue == ScriptStreamTableFlag_End ) /// table�������
				{
					/// ����������һ��һ����table
					/// -1 sub table
					/// -2 key
					/// -3 parent table
					assert( lua_istable( L, -1 ) && lua_istable( L, -3 ) );
					if( lua_istable( L, -1 ) )
					{
						assert( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING );
						if( lua_type(L,-2) == LUA_TNUMBER || lua_type(L,-2) == LUA_TSTRING )
							lua_settable( L, -3 ); /// �����ӱ�
					}
				}
				else
				{
					assert( false );
					return false;
				}
			}
			break;

		case ScriptStreamType_END:
			{
				assert( isKey );
				loadOK = true;
				return isKey;
			}
			break;

		default:
			assert( false );
			return false;
			break;
		}
		//assert( lua_gettop( L ) == oldStackPos );
		return true;
	}

	bool LuaScript::SaveToXml( const char* tableName, TiXmlElement* parentNode, const std::string& numberPrefix )
	{
		/// �ж�numberPrefix���������ֿ�ͷ!!!!!!
		assert( tableName );
		if( tableName == 0 )
			return false;
		int oldStackPos = lua_gettop( L );
		lua_getglobal( L, tableName );	/// ��table����ջ��
		if ( !lua_istable( L, -1 ) )
		{
			char temp[1024];
			sprintf( temp, "SaveToXml ERROR: Not find table %s\r\n", tableName );
			OutputDebugString( temp );
			lua_pop( L, 1 );
			return false;
		}
		/// �ݹ�table
		if( !SaveToXml( lua_gettop( L ), parentNode, numberPrefix ) )
		{
			char temp[1024];
			sprintf( temp, "SaveToXml ERROR %s\r\n", tableName );
			OutputDebugString( temp );
			return lua_settop( L,oldStackPos),false;
		}
		lua_pop(L,1); /// ��table��ջ
		assert( lua_gettop( L ) == oldStackPos );
		if( lua_gettop( L ) != oldStackPos )
			return lua_settop( L,oldStackPos),false;
		return true;
	}

	bool LuaScript::SaveToXml( int idx, TiXmlElement* parentNode, const std::string& numberPrefix )
	{
		int oldStackPos = lua_gettop( L );
		lua_pushnil( L );
		while( lua_next(L, idx ) ) /// pop1��,push2��
		{
			/// �������ֱ����lua_tostring,���ᵼ�±���!!!!
			/// ���http://www.lua.org/manual/5.1/manual.html#lua_next
			std::string key;
			if( lua_type( L, -2 ) == LUA_TSTRING )
			{
				const char* luaString = lua_tostring( L, -2 );
				assert( luaString );
				if( luaString == 0 )
					return lua_settop(L,oldStackPos), false;
				key = luaString; /// keyҲ���������ֿ�ͷ!Ҳ�����ǲ�����ΪXML�ڵ������ַ�!������ô�õĻ�,�Լ�Ҫ��֤������Щ�ַ�����key!
			}
			else if( lua_type( L, -2 ) == LUA_TNUMBER )
			{
				int keyIdx = (int)lua_tonumber( L, -2 );
				key = numberPrefix + We::Type::ToString(keyIdx); ///����XML�ڵ㲻�������ֿ�ͷ,����Ҫ��ǰ׺
			}
			if( !SaveToXmlValue( -1, parentNode, key ) )	/// value
				return false;
			/// �ӱ�,�����ӽڵ�
			if( lua_istable(L,-1) )
			{
				TiXmlElement subNode( key );
				if( !SaveToXml( lua_gettop(L), &subNode, numberPrefix ) )
				{
					return lua_settop( L, oldStackPos),false;
				}
				if( parentNode->InsertEndChild( subNode ) == 0 )
					return lua_settop( L, oldStackPos),false;
			}
			lua_pop( L, 1 ); /// ��Ϊlua_next��pop1��,push2��,��������ֻ��Ҫpop1���Ϳ�����
		}
		assert( lua_gettop( L ) == oldStackPos );
		return true;
	}

	bool LuaScript::SaveToXmlValue( int idx, TiXmlElement* parentNode, const std::string& key )
	{
		int type = lua_type( L, idx );
		switch( type )
		{
		case LUA_TNUMBER:
			parentNode->SetAttribute( key, (int)lua_tonumber(L,idx) );
			break;

		case LUA_TSTRING:
			{
				const char* s = lua_tostring( L, idx );
				if( !s )
					return false;
				parentNode->SetAttribute( key, s );
			}
			break;

		case LUA_TBOOLEAN:
			parentNode->SetAttribute( key, (lua_toboolean(L,idx)==0?"false":"true") );
			break;

		case LUA_TTABLE:
			/// ��һ��ᴦ��,����Ͳ�����
			break;

			/// Ҳ�п��ܴ���ṹ����,�ݲ�����
		case LUA_TUSERDATA:
			assert( false );
			break;

		default:
			assert( false );
			break;
		}
		return true;
	}

	// ������ݱ��浽�ļ�
	// ����1: ��
	// ����2: �ļ���
	int L_SaveTable( lua_State* L )
	{
		// ������2������
		if( lua_gettop( L ) != 2 )
			return 0;
		if( !lua_istable( L, 1 ) )
			return 0;
		const char* fileName = lua_tostring( L, 2 );
		if( fileName == 0 )
			return 0;
		string file = fileName;
		lua_pop( L, 1 );
		if( !lua_istable( L, -1 ) ) /// ����table��ջ������
			return 0;
		int oldStackPos = lua_gettop( L );
		int saveTableSize = 0;
		if( !LuaScript::GetLuaScript()->GetSaveTableSize( lua_gettop( L ), saveTableSize, true ) )
			return 0;
		uint8* saveBuffer = LuaScript::s_TmpBuffer; /// 128K
		bool newBuffer = false;
		//try
		//{
			if( saveTableSize > sizeof(LuaScript::s_TmpBuffer) )
			{
				saveBuffer = new uint8[saveTableSize];
				newBuffer = true;
				OutputString( "L_SaveTable FileName=%s saveTableSize=%d", file.c_str(), saveTableSize );
			}
			// ����table�����ݵ���
			StreamWriter writer( saveBuffer, saveTableSize );
			if( !LuaScript::GetLuaScript()->SaveTable( lua_gettop( L ), writer, true ) )
			{
				if( newBuffer && saveBuffer != 0 )
				{
					delete []saveBuffer;
					saveBuffer = 0;
				}
				OutputString( "L_SaveTable ERROR1 FileName=%s saveTableSize=%d", file.c_str(), saveTableSize );
				lua_settop( L,oldStackPos);
				return 0;
			}
			assert( lua_gettop( L ) == oldStackPos );
			if( lua_gettop( L ) != oldStackPos )
			{
				if( newBuffer && saveBuffer != 0 )
				{
					delete []saveBuffer;
					saveBuffer = 0;
				}
				OutputString( "L_SaveTable ERROR2 FileName=%s saveTableSize=%d", file.c_str(), saveTableSize );
				lua_settop( L,oldStackPos);
				return 0;
			}
			// �����ݱ��浽�ļ�
			FILE* pf = ::fopen( file.c_str(), "wb+" );
			if( pf != 0 )
			{
				if( writer.GetOffset() > 0 )
					::fwrite( saveBuffer, 1, writer.GetOffset(), pf );
				::fclose( pf );
				lua_pushnumber( L, writer.GetOffset() ); // �����ļ���С
				if( newBuffer && saveBuffer != 0 )
				{
					delete []saveBuffer;
					saveBuffer = 0;
				}
				return 1;
			}
			else
			{
				OutputString( "L_SaveTable ERROR3 FileName=%s saveTableSize=%d", file.c_str(), saveTableSize );
			}
			if( newBuffer && saveBuffer != 0 )
			{
				delete []saveBuffer;
				saveBuffer = 0;
			}
	/*	}
		catch( ... )
		{
			if( newBuffer && saveBuffer != 0 )
			{
				delete []saveBuffer;
				saveBuffer = 0;
			}
			lua_settop( L,oldStackPos);
			return 0;
		}*/
		return 0;
	}
	// �ű�����: �Ӷ������ļ�����table������
	// ����1: ��
	// ����2: �ļ���
	int L_LoadTable( lua_State* L )
	{
		if( !lua_istable( L, 1 ) )
			return 0;
		const char* fileName = lua_tostring( L, 2 );
		if( fileName == 0 )
			return 0;
		string file = fileName;
		lua_pop( L, 1 );
		if( !lua_istable( L, -1 ) ) /// ����table��ջ������
			return 0;

		FILE* pf = ::fopen( file.c_str(), "rb" );
		if( pf != 0 )
		{
			long bufferSize = 0;
			fseek( pf, 0, SEEK_END );
			bufferSize = ftell( pf );
			fseek( pf, 0, SEEK_SET );
			uint8* buffer = new uint8[ bufferSize+1 ];
			buffer[0] = 0;
			buffer[bufferSize] = 0;
			if ( fread( buffer, bufferSize, 1, pf ) != 1 )
			{
				::fclose( pf );
				delete []buffer;
				OutputString( "L_LoadTable ERROR: %s\n", file.c_str() );
				return 0;
			}
			::fclose( pf );

			int oldStackPos = lua_gettop( L );
			/// �ݹ�table
			StreamReader reader( buffer, bufferSize );
			bool isKey = true;
			bool loadOK = false;
			while( reader.GetSpace() > 0 )
			{
				if( !LuaScript::GetLuaScript()->LoadValue( lua_gettop( L ), reader, isKey, loadOK ) )
				{
					delete []buffer;
					buffer = 0;
					return lua_settop( L,oldStackPos),0;
				}
				if( loadOK )
					break;
			}
			delete []buffer;
			buffer = 0;
			assert( lua_gettop( L ) == oldStackPos );
			if( lua_gettop( L ) != oldStackPos )
				return lua_settop( L,oldStackPos),0;
		}
		else
		{
			OutputString( "L_LoadTable ERROR not find file: %s\n", file.c_str() );
			return 0;
		}

		lua_pushboolean( L, 1 );
		return 1;
	}

}