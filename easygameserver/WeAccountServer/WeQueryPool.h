/********************************************************************
	created:	2009-7-5
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��ѯ��Ϣ�����
*********************************************************************/
#pragma once

namespace We
{
	//enum QueryType
	//{
	//	QueryType_Small,
	//	QueryType_Mid,
	//	QueryType_Big,
	//};

	//struct BaseQuery
	//{
	//	uint8 m_QueryType;
	//};

	//struct QuerySmall : BaseQuery
	//{
	//	uint8 m_Buffer[256];
	//};

	//struct QueryMid : BaseQuery
	//{
	//	uint8 m_Buffer[1024];
	//};

	//struct QueryBig : BaseQuery
	//{
	//	uint8 m_Buffer[4096];
	//};

	class QueryPool
	{
	public:
		QueryPool();
		~QueryPool();
	};
}