/********************************************************************
	created:	2009-12-16
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	大小
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	struct Size
	{
		uint m_Width;	// 宽
		uint m_Height;	// 高

		Size( uint w, uint h )
		{
			m_Width = w;
			m_Height = h;
		}
	};
}