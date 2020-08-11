//@	{
//@	 "targets":[{"name":"node_argument.test", "type":"application", "autorun":1}]
//@	}

#include "./node_argument.hpp"

#include <cassert>
#include <cstring>

namespace Testcases
{
	void texpainterFilterGraphNodeArgumentGetPointers()
	{
		Texpainter::Size2d size{12, 24};
		int foo          = 123;
		char const* cstr = "Hej";
		Texpainter::FilterGraph::NodeArgument test{size, {&foo, cstr}};

		assert(test.size() == size);
		auto vals = test.inputs<int, char, double>();
		assert(std::get<0>(vals) == &foo);
		assert(std::get<1>(vals) == cstr);
		assert(*std::get<0>(vals) == 123);
		assert(strcmp(std::get<1>(vals), cstr) == 0);
		assert(std::get<2>(vals) == nullptr);

	}
}

int main()
{
	Testcases::texpainterFilterGraphNodeArgumentGetPointers();
	return 0;
}