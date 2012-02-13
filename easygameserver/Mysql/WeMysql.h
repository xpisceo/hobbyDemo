/********************************************************************
	created:	2009-7-27
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	Mysql操作的封装
*********************************************************************/
#pragma once

#include <winsock.h>
#include <mysql.h>
#pragma comment( lib, "libmysql.lib")

namespace We
{
	template<int BindCount>
	class MysqlBind
	{
	public:
		MysqlBind() { ::memset( this, 0, sizeof(*this) ); }

		void BindLong( int idx, long *data )						/// int
		{
			_BindNumber( idx, MYSQL_TYPE_LONG, data, 0 );
		}
		void BindULong( int idx, unsigned long *data )				/// unsigned int
		{
			_BindNumber( idx, MYSQL_TYPE_LONG, data, 1 );
		}
		void BindLongLong( int idx, __int64 *data )					/// __int64
		{
			_BindNumber( idx, MYSQL_TYPE_LONGLONG, data, 0 );
		}
		void BindULongLong( int idx, unsigned __int64 *data )		/// unsigned __int64
		{
			_BindNumber( idx, MYSQL_TYPE_LONGLONG, data, 1 );
		}
		void BindTiny( int idx, char *data )						/// char
		{
			_BindNumber( idx, MYSQL_TYPE_TINY, data, 0 );
		}
		void BindUTiny( int idx, unsigned char *data )				/// unsigned char
		{
			_BindNumber( idx, MYSQL_TYPE_TINY, data, 1 );
		}
		void BindShort( int idx, short *data )						/// char
		{
			_BindNumber( idx, MYSQL_TYPE_SHORT, data, 0 );
		}
		void BindUShort( int idx, unsigned short *data )				/// unsigned char
		{
			_BindNumber( idx, MYSQL_TYPE_SHORT, data, 1 );
		}

		void BindString( int idx, char* data, unsigned long size )	/// Varchar
		{
			_BindBlob( idx, MYSQL_TYPE_STRING, data, size );
		}
		void BindBlob( int idx, void *data, unsigned long size )	/// Binary
		{
			_BindBlob( idx, MYSQL_TYPE_BLOB, data, size );
		}

		MYSQL_BIND* GetBinds() { return m_Binds; }
		int GetBindCount() const { return BindCount; }

		bool IsNull( int idx ) const { return m_Nulls[idx]==1; }
		unsigned long GetLength( int idx ) const { return m_Lens[idx]; }

	protected:
		void _BindNumber( int idx, enum_field_types field_type, void* data, my_bool is_unsigned )
		{
			m_Binds[idx].buffer_type = field_type;
			m_Binds[idx].buffer = data;
			m_Binds[idx].is_null = &(m_Nulls[idx]);
			m_Binds[idx].length = &(m_Lens[idx]);
			m_Binds[idx].is_unsigned = is_unsigned;
		}
		void _BindBlob( int idx, enum_field_types field_type, void* data, unsigned long size )			/// Binary
		{
			m_Lens[idx] = size;
			m_Binds[idx].buffer_type = field_type;
			m_Binds[idx].buffer_length = size;
			m_Binds[idx].buffer = data;
			m_Binds[idx].is_null = &(m_Nulls[idx]);
			m_Binds[idx].length = &(m_Lens[idx]);
		}

	protected:
		MYSQL_BIND      m_Binds[BindCount];
		unsigned long   m_Lens[BindCount];
		my_bool         m_Nulls[BindCount];
	};

	class QueryResult
	{
		friend class Mysql;
	public:
		QueryResult() : m_Mysql(0),m_Stmt(0),m_Res(0),m_Row(0),m_AffectedRows(0),m_FieldNum(0) {}
		~QueryResult();

		/// 影响的行数
		my_ulonglong GetAffectedRows() const { return m_AffectedRows; }

		/// 结果集的列数
		unsigned int GetFieldNum() const { return m_FieldNum; }

		/// 结果集的行数
		my_ulonglong GetRows();

		/// 获取结果集,用在存储过程中,因为一个存储过程可能返回多个结果集
		bool FetchResult();

		/// 有二进制的数据的行
		bool FetchRow( MYSQL_BIND* bind );

		/// 无二进制的行
		bool FetchRow();

		/// 某列的值
		const char* GetValue( int columnIdx );

		/// 某列的值
		int GetInt( int columnIdx, int defaultValue );
		unsigned int GetUint( int columnIdx, unsigned int defaultValue );

		/// 获取列的信息
		MYSQL_FIELD* GetField( unsigned int fieldIndex );

		/// 释放
		void Close();
	protected:
		void OnStoreResult();
	protected:
		MYSQL*		 m_Mysql;
		MYSQL_STMT*  m_Stmt;
		MYSQL_RES*   m_Res;
		MYSQL_ROW	 m_Row;
		unsigned int m_FieldNum;
		my_ulonglong m_AffectedRows;
	};

	class Mysql
	{
	public:
		Mysql();
		~Mysql();

		bool Connect( const char* ip, unsigned short port, const char* account, const char* pwd, const char* dbName );
		void Disconnect();
		bool Shutdown();

		const char* MysqlInfo();

		/// 执行普通SQL语句
		bool ExecuteSQL( const char* sql, QueryResult* result );

		/// 执行存储过程,可以返回多个结果集
		bool ExecuteProcedure( const char* sql, QueryResult* result, bool singleResultSet );

		/// 执行带绑定参数的SQL语句或者存储过程(mysql5.1还不支持STMT返回多个结果集!!!官网说5.4会增加支持接口)
		/// bindArgCount是指绑定的二进制参数的个数
		/// 需要用QueryResult::FetchRow( MYSQL_BIND* bind )来获取结果,即使结果集中没有二进制数据
		bool ExecuteSTMT( const char* sql, QueryResult* result, unsigned long bindArgCount, MYSQL_BIND* bindArgs );

	protected:
		MYSQL		m_Mysql;
		MYSQL_STMT* m_Stmt;
		bool		m_IsConnected;
	};
}