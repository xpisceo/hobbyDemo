#include "WeMysql.h"
#include "WeLogInfo.h"
#include <vector>
using namespace std;

namespace We
{
	QueryResult::~QueryResult()
	{
		Close();
	}
	void QueryResult::Close()
	{
		if( m_Mysql != 0 )
			while( FetchResult() ) ;
		if( m_Res != 0 )
		{
			mysql_free_result( m_Res );
			m_Res = 0;
		}
		if( m_Stmt != 0 )
		{
			mysql_stmt_free_result( m_Stmt );
			m_Stmt = 0;
		}
	}
	bool QueryResult::FetchResult()
	{
		if( m_Res == 0 )
		{
			m_Res = mysql_store_result( m_Mysql );
			OnStoreResult();
			return m_Res != 0;
		}
		int nextResult = mysql_next_result( m_Mysql );
		/// Successful and there are more results
		if( nextResult == 0 )
		{
			/// next_res可能为空,为什么nextResult==0呢??????
			/// 感觉mysql的这个接口不太人性化哦
			MYSQL_RES* next_res = mysql_store_result( m_Mysql );
			if( next_res != 0 )
			{
				if( m_Res != 0 )
				{
					mysql_free_result( m_Res );
				}
				m_Res = next_res;
				OnStoreResult();
				return true;
			}
		}
		/// Successful and there are no more results
		else if( nextResult == -1 )
		{
			return false;
		}
		/// An error occurred
		else
		{
			mysql_free_result( m_Res );
			m_Res = 0;
			return false;
		}
		return false;
	}
	bool QueryResult::FetchRow( MYSQL_BIND* bind )
	{
		if( m_Stmt == 0 )
			return false;
		int ret = mysql_stmt_bind_result( m_Stmt, bind );
		if( ret != 0 )
		{
			const char* err = mysql_stmt_error( m_Stmt );
			LogInfo( "mysql_stmt_bind_result ERROR = %s", err?err:"NULL" );
			return false;
		}
		ret = mysql_stmt_fetch( m_Stmt ); /// MYSQL_NO_DATA = 100
		if( ret != 0 )
		{
			/// 没有数据了
			if( ret == MYSQL_NO_DATA )
			{
				return false;
			}
			else if( ret == 1 ) /// Error occurred.
			{
				const char* err = mysql_stmt_error( m_Stmt );
				LogInfo( "mysql_stmt_fetch ERROR = %s", err?err:"NULL" );
			}
			else if( ret == MYSQL_DATA_TRUNCATED )
			{
				const char* err = mysql_stmt_error( m_Stmt );
				LogInfo( "mysql_stmt_fetch MYSQL_DATA_TRUNCATED ERROR = %s", err?err:"NULL" );
			}
			return false;
		}
		return true;
	}
	bool QueryResult::FetchRow()
	{
		if( m_Res == 0 )
		{
			m_Row = 0;
			return false;
		}
		m_Row = mysql_fetch_row( m_Res );
		return m_Row != 0;
	}
	my_ulonglong QueryResult::GetRows()
	{
		if( m_Res == 0 )
			return 0;
		return mysql_num_rows( m_Res );
	}
	const char* QueryResult::GetValue( int columnIdx )
	{
		if( m_Row == 0 )
			return 0;
#pragma warning( disable : 4018 )
		if( columnIdx < 0 || columnIdx >= m_FieldNum )
			return 0;
		return m_Row[columnIdx];
	}
	int QueryResult::GetInt( int columnIdx, int defaultValue )
	{
		const char* value = GetValue( columnIdx );
		if( value == 0 )
			return defaultValue;
		return ::atoi( value );
	}
	unsigned int QueryResult::GetUint( int columnIdx, unsigned int defaultValue )
	{
		const char* value = GetValue( columnIdx );
		if( value == 0 )
			return defaultValue;
		return (unsigned int)::atol( value );
	}
	MYSQL_FIELD* QueryResult::GetField( unsigned int fieldIndex )
	{
		if( fieldIndex >= m_FieldNum )
			return 0;
		if( m_Res == 0 )
			return 0;
		return mysql_fetch_field_direct( m_Res, fieldIndex );
	}
	void QueryResult::OnStoreResult()
	{
		if( m_Res == 0 )
		{
			m_FieldNum = 0;
			return;
		}
		m_FieldNum = mysql_num_fields( m_Res );
		//LogInfo( "FieldNum = %d", m_FieldNum );
		my_ulonglong rows = mysql_num_rows( m_Res );
		if( rows == (my_ulonglong)-1 )
			rows = 0;
		//LogInfo( "Rows = %d", rows );
	}
	Mysql::Mysql()
	{
		m_IsConnected = false;
		m_Stmt = 0;
		mysql_init(&m_Mysql);
		m_Stmt = mysql_stmt_init( &m_Mysql );
	}
	Mysql::~Mysql()
	{
		Disconnect();
	}
	bool Mysql::Connect( const char* ip, unsigned short port, const char* account, const char* pwd, const char* dbName )
	{
		/// 允许返回多个结果集,使用存储过程,必须设置该选项
		/// mysql5.4版本后,该选项会变成默认参数
		m_Mysql.client_flag |= CLIENT_MULTI_RESULTS;
		mysql_options( &m_Mysql, MYSQL_SET_CHARSET_NAME, "gbk" );
		return mysql_real_connect( &m_Mysql, ip, account, pwd, dbName, port, 0, m_Mysql.client_flag ) != 0;
	}
	void Mysql::Disconnect()
	{
		if( m_IsConnected )
		{
			mysql_close(&m_Mysql);
			m_IsConnected = false;
		}
		if( m_Stmt != 0 )
		{
			mysql_stmt_close( m_Stmt );
			m_Stmt = 0;
		}
	}
	bool Mysql::Shutdown()
	{
		return mysql_shutdown(&m_Mysql, SHUTDOWN_DEFAULT) == 0;
	}
	const char* Mysql::MysqlInfo()
	{
		return mysql_info(&m_Mysql);
	}
	bool Mysql::ExecuteSQL( const char* sql, QueryResult* result )
	{
		//LogInfo( "sql = %s", sql );
		if( mysql_real_query( &m_Mysql, sql, static_cast<unsigned long>( ::strlen( sql ) ) ) != 0 )
		{
			LogInfo( "mysql_real_query ERROR sql = %s", sql );
			const char* err = mysql_error( &m_Mysql );
			LogInfo( "mysql_real_query ERROR = %s stat=%s", err?err:"NULL", mysql_sqlstate(&m_Mysql) );
			return false;
		}
		my_ulonglong affected_rows = mysql_affected_rows( &m_Mysql );
		if( affected_rows == (my_ulonglong)-1 )
			affected_rows = 0;
		// LogInfo( "affected_rows = %d", affected_rows );
		if( result != 0 )
		{
			MYSQL_RES* res = mysql_store_result( &m_Mysql );
			result->m_Mysql = &m_Mysql;
			result->m_Stmt = 0;
			result->m_AffectedRows = affected_rows;
			if( res != 0 )
			{
				result->m_Res = res;
				int fieldNum = mysql_num_fields( result->m_Res );
				result->m_FieldNum = mysql_num_fields( result->m_Res );
				// LogInfo( "FieldNum = %d", fieldNum );
				my_ulonglong rows = mysql_num_rows( result->m_Res );
				if( rows == (my_ulonglong)-1 )
					rows = 0;
				// LogInfo( "Rows = %d", rows );
			}
		}
		else
		{
			// 默认的QueryResult,释放缓存的结果集
			QueryResult defaultResult;
			defaultResult.m_Mysql = &m_Mysql;
		}
		return true;
	}
	bool Mysql::ExecuteProcedure( const char* sql, QueryResult* result, bool singleResultSet )
	{
		// LogInfo( "sql = %s", sql );
		if( mysql_real_query( &m_Mysql, sql, static_cast<unsigned long>( ::strlen( sql ) ) ) != 0 )
		{
			LogInfo( "mysql_real_query ERROR sql = %s", sql );
			const char* err = mysql_error( &m_Mysql );
			LogInfo( "mysql_real_query ERROR = %s stat=%s", err?err:"NULL", mysql_sqlstate(&m_Mysql) );
			return false;
		}
		my_ulonglong affected_rows = mysql_affected_rows( &m_Mysql );
		if( affected_rows == (my_ulonglong)-1 )
			affected_rows = 0;
		// LogInfo( "affected_rows = %d", affected_rows );
		if( result != 0 )
		{
			result->m_Mysql = &m_Mysql;
			result->m_Stmt = 0;
			result->m_AffectedRows = affected_rows;
			/// 单个结果集,直接获取结果集,如果有多个结果集,只取最后一个
			if( singleResultSet )
			{
				result->m_Res = mysql_store_result( &m_Mysql );
				/// 后面不一定有结果集,但是如果不这么做,就会出现out of sync的错误
				while( true )
				{
					int nextResult = mysql_next_result( &m_Mysql );
					/// Successful and there are more results
					if( nextResult == 0 )
					{
						MYSQL_RES* next_res = mysql_store_result( &m_Mysql );
						if( next_res != 0 )
						{
							if( result->m_Res != 0 )
							{
								mysql_free_result( result->m_Res );
							}
							result->m_Res = next_res;
							continue;
						}
					}
					/// Successful and there are no more results
					else if( nextResult == -1 )
					{
						break;
					}
					/// An error occurred
					else
					{
						const char* err = mysql_error( &m_Mysql );
						LogInfo( "mysql_next_result ERROR = %s stat=%s", err?err:"NULL", mysql_sqlstate(&m_Mysql) );
						break;
					}
				}
				result->OnStoreResult();
			}
		}
		else
		{
			// 默认的QueryResult,释放缓存的结果集
			QueryResult defaultResult;
			defaultResult.m_Mysql = &m_Mysql;
		}
		return true;
	}
	bool Mysql::ExecuteSTMT( const char* sql, QueryResult* result, unsigned long bindCount, MYSQL_BIND* binds )
	{
#pragma warning( disable : 4267 )
		// LogInfo( "sql = %s", sql );
		int ret = mysql_stmt_prepare( m_Stmt, sql, ::strlen(sql) );
		if( ret != 0 )
		{
			LogInfo( "mysql_stmt_prepare ERROR ret=%d sql = %s", ret, sql );
			const char* err = mysql_stmt_error( m_Stmt );
			LogInfo( "mysql_stmt_prepare ERROR = %s", err?err:"NULL" );
			return false;
		}
		unsigned long param_count = mysql_stmt_param_count( m_Stmt );
		if( bindCount != param_count )
		{
			LogInfo( "ExecuteSTMT ERROR param_count = %d", param_count );
			return false;
		}
		if( param_count > 0 )
		{
			if( binds == 0 )
			{
				LogInfo( "ExecuteSTMT ERROR binds == 0 param_count = %d", param_count );
				return false;
			}
			ret = mysql_stmt_bind_param( m_Stmt, binds );
			if( ret != 0 )
			{
				const char* err = mysql_stmt_error( m_Stmt );
				LogInfo( "mysql_stmt_bind_param ERROR = %s", err?err:"NULL" );
				return false;
			}
		}
		ret = mysql_stmt_execute( m_Stmt );
		if( ret != 0 )
		{
			LogInfo( "mysql_stmt_execute ERROR sql = %s", sql );
			const char* err = mysql_stmt_error( m_Stmt );
			LogInfo( "mysql_stmt_execute ERROR = %s", err?err:"NULL" );
			return false;
		}
		if( result != 0 )
		{
			result->m_Mysql = &m_Mysql;
			result->m_Res = mysql_stmt_result_metadata( m_Stmt );
			if( result->m_Res == 0 )
			{
				const char* err = mysql_stmt_error( m_Stmt );
				LogInfo( "mysql_stmt_result_metadata ERROR = %s", err?err:"NULL" );
				mysql_stmt_free_result( m_Stmt );
				return false;
			}
			ret = mysql_stmt_store_result( m_Stmt );
			if( ret != 0 )
			{
				LogInfo( "mysql_stmt_store_result ERROR sql = %s", sql );
				const char* err = mysql_stmt_error( m_Stmt );
				LogInfo( "mysql_stmt_store_result ERROR = %s", err?err:"NULL" );
				if( result->m_Res != 0 )
				{
					mysql_free_result( result->m_Res );
					result->m_Res = 0;
				}
				return false;
			}
			my_ulonglong affected_rows = mysql_stmt_affected_rows( m_Stmt );
			// LogInfo( "affected_rows = %d", affected_rows );

			result->m_Stmt = m_Stmt;
			result->m_AffectedRows = affected_rows;
			if( result->m_Res != 0 )
			{
				result->m_FieldNum = mysql_num_fields( result->m_Res );
			}
		}
		else
		{
			// 默认的QueryResult,释放缓存的结果集
			QueryResult defaultResult;
			defaultResult.m_Mysql = &m_Mysql;
		}
		return true;
	}
}