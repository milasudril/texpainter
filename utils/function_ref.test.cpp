//@	{
//@	 "targets":[{"name":"function_ref.test","type":"application", "autorun":1}]
//@	}

#include "./function_ref.hpp"

#include <cassert>
#include <memory>

namespace Testcases
{
	void texpainterFunctionRefCreateAndCall()
	{
		auto callback = [capture = 0](std::unique_ptr<int> a, int b) mutable {
			++capture;
			return std::make_unique<int>(capture * (*a + b));
		};
		Texpainter::FunctionRef<std::unique_ptr<int>(std::unique_ptr<int>, int)> f{callback};

		auto res = f(std::make_unique<int>(1), 2);
		assert(*res == 3);

		res = f(std::make_unique<int>(1), 2);
		assert(*res == 6);
	}

	void texpainterFunctionRefShouldNotDoubleWrap()
	{
		auto callback = []() mutable {};
		Texpainter::FunctionRef<void()> f{callback};

		Texpainter::FunctionRef<void()> other{f};

		assert(other.handle() == f.handle());
		assert(other.function() == f.function());
	}
}

int main()
{
	Testcases::texpainterFunctionRefCreateAndCall();
	Testcases::texpainterFunctionRefShouldNotDoubleWrap();
	return 0;
}