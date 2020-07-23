//@	{
//@	"targets":[{"name":"brush.test","type":"application", "autorun":1}]
//@	}

#include "./brush.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterBrushFunctionCall()
	{
		Texpainter::Model::BrushFunction brush{Texpainter::Model::BrushType::Square};

		assert((brush(true, Texpainter::vec2_t{0.5, 0.5}) == true));
		assert((brush(true, Texpainter::vec2_t{1.5, 0.5}) == false));
		assert((brush(true, Texpainter::vec2_t{0.5, 1.5}) == false));
		assert((brush(true, Texpainter::vec2_t{0.5, -1.5}) == false));
	}
}

int main()
{
	Testcases::texpainterBrushFunctionCall();
	return 0;
}
