//@	{
//@	 "targets":[{"name":"unique_function.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./unique_function.hpp"

#include <cassert>
#include <memory>

namespace Testcases
{
	void maikeUniqueFunctionCall()
	{
		int obj = 0;

		Texpainter::UniqueFunction<std::unique_ptr<double>(int)> f{[&obj](int x) {
			obj = x;
			return std::make_unique<double>(2.0);
		}};
		auto ptr = f(5);
		assert(obj == 5);
		assert(*ptr == 2.0);
	}

	void maikeUniqueFunctionMove()
	{
		Texpainter::UniqueFunction<void()> f{[]() {}};
		assert(f.valid());
		auto g = std::move(f);
		assert(g.valid());
		assert(!f.valid());

		f = std::move(g);
		assert(f.valid());
		assert(!g.valid());
	}
}

int main()
{
	Testcases::maikeUniqueFunctionCall();
	Testcases::maikeUniqueFunctionMove();
	return 0;
}