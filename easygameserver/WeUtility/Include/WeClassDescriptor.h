/********************************************************************
	created:	2007-1-9
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��������
*********************************************************************/
#pragma once

//#include "WeConfig.h"

namespace We
{
	class /*We_EXPORTS*/ ClassDescriptor
	{
	public:
		virtual int GetClassType() const = 0;
		virtual const char* GetClassName() const = 0;
	};
}
