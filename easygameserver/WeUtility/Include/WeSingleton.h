/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	单件模式
*********************************************************************/
#pragma once

#pragma warning (disable : 4311)
#pragma warning (disable : 4312)

#include "WeConfig.h"

namespace We
{
	//单件模式
	template <class T> 
	class /*We_EXPORTS*/ Singleton
	{
	protected:
		static T* ms_Singleton;

	public:
		Singleton( void )
		{
			assert( !ms_Singleton );
			ms_Singleton = static_cast<T*>(this);
		}
		~Singleton( void )
		{  assert( ms_Singleton );  ms_Singleton = 0;  }
		static T& getSingleton( void )
		{  assert( ms_Singleton );  return ( *ms_Singleton );  }
		static T* getSingletonPtr( void )
		{  return ( ms_Singleton );  }
	};

}

//定义
#define Singleton_Implement(ClassName) \
	template<> ClassName##* We::Singleton<##ClassName##>::ms_Singleton = 0;

//示例:
//in Example.h
//	class Example : public We::Singleton<Example>
//	{
//	public:
//		Example() {}
//		~Example() {}
//
//		void Foo() {}
//	};

//in Example.cpp
//	Singleton_Implement(Example);

//in main.cpp
//	 if( Example::getSingletonPtr() == 0 )
//		new Example();	//创建
//	 Example::getSingleton().Foo();	//调用函数
//	 if( Example::getSingletonPtr() != 0 )
//		delete Example::getSingletonPtr();	//销毁
//
