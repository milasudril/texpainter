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

	void texpainterVec2Signum()
	{
		Texpainter::vec2_t a{-3.0, 4.0};
		Texpainter::vec2_t b{3.0, -4.0};
		Texpainter::vec2_t c{-3.0, -4.0};
		Texpainter::vec2_t d{3.0, 4.0};


		auto a_1 = Texpainter::signum(a);
		auto b_1 = Texpainter::signum(b);
		auto c_1 = Texpainter::signum(c);
		auto d_1 = Texpainter::signum(d);

		assert(a_1[0] == -1.0);
		assert(a_1[1] == 1.0);

		assert(b_1[0] == 1.0);
		assert(b_1[1] == -1.0);

		assert(c_1[0] == -1.0);
		assert(c_1[1] == -1.0);

		assert(d_1[0] == 1.0);
		assert(d_1[1] == 1.0);
	}
}


int main()
{
	Testcases::texpainterVec2Dot();
	Testcases::texpainterVec2Signum();
	return 0;
}