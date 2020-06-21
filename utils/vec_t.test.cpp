//@	{"targets":[{"name":"vec_t.test", "type":"application", "autorun":1}]}

#include "./vec_t.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterVec2Dot()
	{
		Texpainter::vec2_t a{3.0, 4.0};
		assert(Texpainter::dot(a, a) == 25.0);
	}
}


int main()
{
	Testcases::texpainterVec2Dot();
	return 0;
}