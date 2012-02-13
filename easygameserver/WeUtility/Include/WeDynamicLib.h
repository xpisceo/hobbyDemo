/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	dll的加载
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include <string>
using namespace std;

/*************************************************************************
The following is basically taken from DynLib.h, which is part of
the Ogre project (http://www.ogre3d.org/)
*************************************************************************/
#if defined(__WIN32__) || defined(_WIN32)
#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibrary( a )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) !FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif defined(__linux__)
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY )
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )
#    define DYNLIB_ERROR( ) dlerror( )
#endif

namespace We
{

	class We_EXPORTS DynamicLib
	{
	public:
		/*!
		\brief
		Construct the DynamicModule object by loading the dynamic loadable
		module specified.

		\param name
		String object holding the name of a loadable module.

		\return
		Nothing
		*/
		DynamicLib(const string& name);

		/*!
		\brief
		Destroys the DynamicModule object and unloads the associated loadable
		module.

		\return
		Nothing
		*/
		~DynamicLib();

		/*!
		\brief
		Return a String containing the name of the dynamic module.
		*/
		const string& GetModuleName() const;

		/*!
		\brief
		Return the address of the specified symbol.

		\param symbol
		String holding the symbol to look up in the module.

		\return
		Pointer to the requested symbol.

		\exception
		InvalidRequestException thrown if the symbol does not exist.
		*/
		void* GetSymbolAddress(const string& symbol) const;

	private:
		/*!
		\brief
		Return a String containing the last failure message from the platforms
		dynamic loading system.
		*/
		string GetFailureString() const;

		//! Holds the name of the loaded module.
		string m_ModuleName;
		//! Handle for the loaded module
		DYNLIB_HANDLE m_Handle;
	};

}
