#include "WeDynamicLib.h"


#if defined(__WIN32__) || defined(_WIN32)
#   if defined(_MSC_VER)
#       pragma warning(disable : 4552)  // warning: operator has no effect; expected operator with side-effect
#   endif
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#if defined(__linux__)
#   include "dlfcn.h"
#endif


namespace We
{
	DynamicLib::DynamicLib(const string& name) :
m_ModuleName(name)
{
#if defined(__linux__)
	// dlopen() does not add .so to the filename, like windows does for .dll
	if (m_ModuleName.substr(m_ModuleName.length() - 3, 3) != ".so")
		m_ModuleName += ".so";

	// see if we need to add the leading 'lib'
	if (m_ModuleName.substr(0, 3) != "lib")
		m_ModuleName.insert(0, "lib");
#endif

	// Optionally add a _d to the module name for the debug config on Win32
#if defined(__WIN32__) || defined(_WIN32)
#   if defined (_DEBUG) && defined (CEGUI_LOAD_MODULE_APPEND_SUFFIX_FOR_DEBUG)
	// if name has .dll extension, assume it's complete and do not touch it.
	if (m_ModuleName.substr(m_ModuleName.length() - 4, 4) != ".dll")
		m_ModuleName += CEGUI_LOAD_MODULE_DEBUG_SUFFIX;
#   endif
#endif

	m_Handle = DYNLIB_LOAD(m_ModuleName.c_str());

	//// check for library load failure
	//if (!m_Handle)
	//	throw GenericException(
	//	"DynamicModule::DynamicModule - Failed to load module '" +
	//	m_ModuleName + "': " + getFailureString());
}


DynamicLib::~DynamicLib()
{
	DYNLIB_UNLOAD(m_Handle);
}


const string& DynamicLib::GetModuleName() const
{
	return m_ModuleName;
}


void* DynamicLib::GetSymbolAddress(const string& symbol) const
{
	return (void*)DYNLIB_GETSYM(m_Handle, symbol.c_str());
}


string DynamicLib::GetFailureString() const
{
	string retMsg;
#if defined(__linux__) || defined (__APPLE_CC__)
	retMsg = DYNLIB_ERROR();
#elif defined(__WIN32__) || defined(_WIN32)
	LPVOID msgBuffer;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		0,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&msgBuffer),
		0,
		0))
	{
		retMsg = reinterpret_cast<LPTSTR>(msgBuffer);
		LocalFree(msgBuffer);
	}
	else
	{
		retMsg = "Unknown Error";
	}
#else
	retMsg = "Unknown Error";
#endif
	return retMsg;
}

}
