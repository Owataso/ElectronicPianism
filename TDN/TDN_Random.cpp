
#include	"TDNLIB.h"

//*****************************************************
//		ƒ‰ƒ“ƒ_ƒ€¶¬ƒNƒ‰ƒX
//*****************************************************

std::mt19937 tdnRandom::engine;

void tdnRandom::Initialize()
{
	std::random_device device;
	engine.seed((unsigned long)device());
}

std::uniform_int_distribution<int> tdnRandom::Make_distribution(int min_num, int max_num)
{
	return std::uniform_int_distribution<int>(min_num, max_num);
}

std::uniform_real_distribution<float> tdnRandom::Make_distribution(float min_num, float max_num)
{
	return std::uniform_real_distribution<float>(min_num, max_num);
}

int tdnRandom::Get(int min_num, int max_num)
{
	return std::uniform_int_distribution<int>(min_num, max_num)(engine);
}

float tdnRandom::Get(float min_num, float max_num)
{
	return std::uniform_real_distribution<float>(min_num, max_num)(engine);
}
