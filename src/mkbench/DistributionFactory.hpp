#pragma once
#include <string>
#include <random>
#include <functional>

class DistributionFactory
{
	nuehnth
	public:
		static std::function<float(std::default_random_engine)> create(std::string distribution) {
			return std::uniform_real_distribution<float>(0.0f, 1.0f);
		}
}
