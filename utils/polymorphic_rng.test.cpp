//@	{
//@	 "targets":[{"name":"polymorphic_rng.test","type":"application", "autorun":1}]
//@	}

#include "./polymorphic_rng.hpp"

#include <random>
#include <cassert>

namespace Testcases
{
	void texpainterPolymorphicRngInstanciateAndGenerateNumber()
	{
		std::uniform_int_distribution<int> dice{1, 6};

		std::mt19937 rng;
		Texpainter::PolymorphicRng r{std::ref(rng)};
		auto a = dice(r);

		// r holds a reference, not a copy
		rng    = std::mt19937{};
		auto b = dice(r);

		assert(a == b);
	}
}

int main()
{
	Testcases::texpainterPolymorphicRngInstanciateAndGenerateNumber();
	return 0;
}