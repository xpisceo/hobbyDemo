/********************************************************************
	created:	2009-6-10
	author:		Fish (�ڹ�ƽ)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	�����
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	class We_EXPORTS Random
	{
	public:
		/// �������������
		static void SetSeed( int seed );

		/// �Ե�ǰʱ����Ϊ�����������
		static void SetSeed_Now();

		/// �������
		static int RandomInt( int minValue, int maxValue );

		/// ���������
		static float RandomFloat( float minValue, float maxValue );

	protected:
		static unsigned int s_Seed;
	};

}
