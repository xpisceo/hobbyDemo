/********************************************************************
	created:	2007-1-9
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�������
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	class We_EXPORTS WeObject
	{
	public:
		WeObject();
		virtual ~WeObject();
		//virtual int GetClassType() const = 0;
		virtual const char* GetClassName() const = 0;

		bool IsClass( const char* name );

		void IncRefCount();
		void DecRefCount();
		uint32 GetRefCount() const;

	private:
		volatile uint32 m_RefCount;	// ���ü���
	};
}
