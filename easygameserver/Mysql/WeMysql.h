/********************************************************************
	created:	2009-7-27
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	Mysql�����ķ�װ
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

		/// Ӱ�������
		my_ulonglong GetAffectedRows() const { return m_AffectedRows; }

		/// �����������
		unsigned int GetFieldNum() const { return m_FieldNum; }

		/// �����������
		my_ulonglong GetRows();

		/// ��ȡ�����,���ڴ洢������,��Ϊһ���洢���̿��ܷ��ض�������
		bool FetchResult();

		/// �ж����Ƶ����ݵ���
		bool FetchRow( MYSQL_BIND* bind );

		/// �޶����Ƶ���
		bool FetchRow();

		/// ĳ�е�ֵ
		const char* GetValue( int columnIdx );

		/// ĳ�е�ֵ
		int GetInt( int columnIdx, int defaultValue );
		unsigned int GetUint( int columnIdx, unsigned int defaultValue );

		/// ��ȡ�е���Ϣ
		MYSQL_FIELD* GetField( unsigned int fieldIndex );

		/// �ͷ�
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

		/// ִ����ͨSQL���
		bool ExecuteSQL( const char* sql, QueryResult* result );

		/// ִ�д洢����,���Է��ض�������
		bool ExecuteProcedure( const char* sql, QueryResult* result, bool singleResultSet );

		/// ִ�д��󶨲�����SQL�����ߴ洢����(mysql5.1����֧��STMT���ض�������!!!����˵5.4������֧�ֽӿ�)
		/// bindArgCount��ָ�󶨵Ķ����Ʋ����ĸ���
		/// ��Ҫ��QueryResult::FetchRow( MYSQL_BIND* bind )����ȡ���,��ʹ�������û�ж���������
		bool ExecuteSTMT( const char* sql, QueryResult* result, unsigned long bindArgCount, MYSQL_BIND* bindArgs );

	protected:
		MYSQL		m_Mysql;
		MYSQL_STMT* m_Stmt;
		bool		m_IsConnected;
	};
}