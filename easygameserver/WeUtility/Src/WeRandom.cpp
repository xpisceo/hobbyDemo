#include "WeRandom.h"
//#include <Windows.h>
#include <time.h>

namespace We
{
	//------------------------------------------------------------------------
	/*static */unsigned int Random::s_Seed = 0;
	//------------------------------------------------------------------------
	void Random::SetSeed(int seed )
	{
		s_Seed = seed;
	}
	//------------------------------------------------------------------------
	void Random::SetSeed_Now()
	{
		s_Seed = /*(unsigned int)time(0)*/::timeGetTime();
	}
	//------------------------------------------------------------------------
	int Random::RandomInt(int minValue, int maxValue )
	{
		if( maxValue-minValue + 1 == 0 )
			return minValue;
		s_Seed = 214013*s_Seed + 2531011;
		return minValue+(s_Seed ^ s_Seed>>15)%(maxValue-minValue+1);
	}
	//------------------------------------------------------------------------
	float Random::RandomFloat(float minValue, float maxValue )
	{
		if( maxValue-minValue == 0.0f )
			return minValue;
		s_Seed = 214013*s_Seed + 2531011;
		return minValue+(s_Seed>>16)*(1.0f/65535.0f)*(maxValue-minValue);
	}
	//------------------------------------------------------------------------
}
