/********************************************************************
	created:	2009-12-16
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	��С
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	struct Size
	{
		uint m_Width;	// ��
		uint m_Height;	// ��

		Size( uint w, uint h )
		{
			m_Width = w;
			m_Height = h;
		}
	};
}