#include "WeObject.h"

namespace We
{
	WeObject::WeObject()
	{
		m_RefCount = 0;
		InterlockedIncrement( (LONG*)&m_RefCount );
	}
	//---------------------------------------------------------------------------
	WeObject::~WeObject()
	{
		InterlockedDecrement( (LONG*)&m_RefCount );
	}
	bool WeObject::IsClass( const char* name )
	{
		if( !name )
			return false;
		return (strcmp( name, GetClassName() ) == 0);
	}
	//---------------------------------------------------------------------------
	void WeObject::IncRefCount()
	{
		InterlockedIncrement( (LONG*)&m_RefCount );
	}
	//---------------------------------------------------------------------------
	void WeObject::DecRefCount()
	{
		assert( m_RefCount > 0 );
		InterlockedDecrement( (LONG*)&m_RefCount );
	}
	//---------------------------------------------------------------------------
	uint32 WeObject::GetRefCount() const
	{
		return m_RefCount;
	}
}