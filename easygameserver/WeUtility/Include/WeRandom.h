/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	随机数
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	class We_EXPORTS Random
	{
	public:
		/// 设置随机数种子
		static void SetSeed( int seed );

		/// 以当前时间作为随机数的种子
		static void SetSeed_Now();

		/// 随机整数
		static int RandomInt( int minValue, int maxValue );

		/// 随机浮点数
		static float RandomFloat( float minValue, float maxValue );

	protected:
		static unsigned int s_Seed;
	};

}
