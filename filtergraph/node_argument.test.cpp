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
		int foo          = 123;
		char const* cstr = "Hej";
		Texpainter::FilterGraph::NodeArgument test{&foo, cstr};

		auto vals = test.inputs<int, char>();
		assert(std::get<0>(vals) == &foo);
		assert(std::get<1>(vals) == cstr);
		assert(*std::get<0>(vals) == 123);
		assert(strcmp(std::get<1>(vals), cstr) == 0);
	}
}

int main()
{
	Testcases::texpainterFilterGraphNodeArgumentGetPointers();
	return 0;
}